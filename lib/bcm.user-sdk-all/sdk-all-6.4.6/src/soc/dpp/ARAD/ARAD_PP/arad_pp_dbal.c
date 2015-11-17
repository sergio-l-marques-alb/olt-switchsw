/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
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
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_fem.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <shared/l3.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <shared/swstate/access/sw_state_access.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif


/*#define DBAL_PRINTS_ENABLED*/

#define DBAL_MAX_QUALIFIER_LENGTH           32
#define DBAL_KEY_SIZE_IN_BITS               80 
#define DBAL_KEY_C_ARAD_SIZE_IN_BITS        160 

typedef enum
{
    SOC_DPP_DBAL_CE_SEARCH_MODE_USE_FIRST_16_BIT_FIRST,
    SOC_DPP_DBAL_CE_SEARCH_MODE_FIRST_FREE
}SOC_DPP_DBAL_CE_SEARCH_MODE;


#define DBAL_TABLE_INFO_UPDATE(table_info, db_prefix,db_prefix_len, physical_db, nof_qualifiers, additional_info, qual_info, table_name)\
                                table_info.db_prefix        = db_prefix;\
                                table_info.db_prefix_len    = db_prefix_len;\
                                table_info.physical_db_type = physical_db;\
                                table_info.nof_qualifiers   = nof_qualifiers;\
                                table_info.additional_table_info = additional_info;\
                                sal_memcpy(&(table_info.qual_info[0]), &qual_info[0], sizeof(SOC_DPP_DBAL_QUAL_INFO)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);\
                                sal_strncpy(&(table_info.table_name[0]), table_name, sal_strlen(table_name));

/********* FUNCTION DECLARTIONS *********/

STATIC uint32 arad_pp_dbal_next_available_ce_get(int unit, int qual_nof_bits, uint8 use_32_ce, uint32 last_unchecked_ce, ARAD_FP_DATABASE_STAGE stage, uint32 ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE mode, int is_msb, ARAD_PP_KEY_CE_ID* free_ce);
STATIC uint32 arad_pp_dbal_entry_key_to_lem_buffer(int unit, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, ARAD_PP_LEM_ACCESS_KEY *key);
STATIC int    arad_pp_dbal_program_to_string(int unit, ARAD_FP_DATABASE_STAGE stage, int cam_line, const char**);
STATIC void   arad_pp_dbal_print_buffer(uint8* stream, uint32 steam_size, char* msg);
STATIC uint32 arad_pp_dbal_next_available_tcam_db_id_get(int unit, uint32* tcam_db_id);
STATIC uint32 arad_pp_dbal_next_available_lem_app_id_get(int unit, uint32* app_id);
STATIC uint32 arad_pp_dbal_next_available_key_get(int unit, ARAD_FP_DATABASE_STAGE stage, uint32 prog_id, SOC_DPP_DBAL_PROGRAM_KEYS* key_id);
STATIC uint32 arad_pp_dbal_next_available_table_id_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS* table_id);
STATIC uint32 arad_pp_dbal_sem_entry_key_build(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE]);
STATIC uint32 arad_pp_dbal_sem_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type, uint8 prefix, SOC_DPP_DBAL_SW_TABLE_IDS table_id);
STATIC uint32 arad_pp_dbal_sem_next_available_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8* prefix);
STATIC uint32 arad_pp_dbal_sem_prefix_usage_get(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, uint8 db_prefix, int* count);
STATIC uint32 arad_pp_dbal_tcam_table_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_FP_DATABASE_STAGE stage);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32 arad_pp_dbal_entry_key_to_kbp_buffer(int unit, SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                   uint32 *prefix_len, uint8 *data_bytes);
#endif


#define DBAL_GENERAL_FUNCTIONS 
/************************************  GENERAL DBAL FUNCTION ************************************/
uint32
    arad_pp_dbal_init(int unit)
{
    uint8 is_allocated;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(!SOC_WARM_BOOT(unit)){
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.is_allocated(unit, &is_allocated));
        if(!is_allocated) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.alloc(unit));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_deinit(int unit)
{    
    return 0;
}
#undef DBAL_GENERAL_FUNCTIONS





#define DBAL_SERVICE_FUNCTIONS 
/************************************ SERVICE DBAL FUNCTION ************************************/

uint32
    arad_pp_dbal_sem_entry_key_build(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE])
{
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, ARAD_FP_DATABASE_STAGE_INGRESS_VT, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry_key, entry_mask ));
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&(table.db_prefix), ARAD_PP_ISEM_ACCESS_LSB_PREFIX, table.db_prefix_len, entry_key));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_db_predfix_get(int unit, uint32 table_id, uint32* db_prefix)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(table.is_table_initiated == 0){         
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_db_predfix_get - table nit initiated table_id %d"), table_id));
    }

    (*db_prefix) = table.db_prefix;

exit:    
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_ce_info_get(int unit, uint32 table_id, ARAD_FP_DATABASE_STAGE stage, ARAD_PMF_CE *ce_array, uint8 *nof_ces, uint8 *is_key_320b, uint8 *ces_ids)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*nof_ces) = table.nof_qualifiers;
        (*is_key_320b) = 0;

        for (i = 0; i < table.nof_qualifiers; i++) {
            ces_ids[i] = i;
            ce_array[table.nof_qualifiers - 1 - i].is_used = 1;
            ce_array[table.nof_qualifiers - 1 - i].qual_type = table.qual_info[i].qual_type;
            ce_array[table.nof_qualifiers - 1 - i].msb       = table.qual_info[i].qual_nof_bits-1;
            ce_array[table.nof_qualifiers - 1 - i].qual_lsb  = table.qual_info[i].qual_offset;
            if ((table.qual_info[i].qual_is_in_hdr) && (table.qual_info[i].qual_full_size > 32)) {
                ce_array[table.nof_qualifiers - 1 - i].qual_lsb = (table.qual_info[i].qual_full_size-1 ) - table.qual_info[i].qual_offset - (table.qual_info[i].qual_nof_bits-1);
            }
            /*for header qualifiers bigger than 32 bit the offset calculation is */
            /*qual_lsb = header_qual_info.lsb - qual_info.qual_offset - (qual_info.qual_nof_bits - 1);*/
        }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
    arad_pp_dbal_table_physical_db_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type )
{

    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - table not initiated")));
    }

    (*physical_db_type) = table.physical_db_type;

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_table_is_initiated(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int* is_table_initiated)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    *is_table_initiated = table.is_table_initiated;
        
exit:        
    SOCDNX_FUNC_RETURN;
}

/*
   looking the next free instruction in the program and use it.
   strats from the 32bit instructions.
 
   two modes of operation:
   must use 16 instruction for qualifiers that are less than 16 bits.
   use the first available CE (can use 32 bit instructions for qualifiers that are less than 16 bit)
*/
STATIC uint32    
    arad_pp_dbal_next_available_ce_get(int unit, int qual_nof_bits, uint8 use_ce_32, uint32 last_unchecked_ce, ARAD_FP_DATABASE_STAGE stage, uint32 ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE mode, int is_msb, ARAD_PP_KEY_CE_ID* free_ce)
{
    ARAD_PP_KEY_CE_ID ce_ndx;
    uint32 ce_rsrc_lcl[1];
    int low_limit = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (is_msb) {
        low_limit = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
    }

    *ce_rsrc_lcl = ce_rsrc; 
    for (ce_ndx = last_unchecked_ce; (int)ce_ndx >= low_limit; ce_ndx--) {
        /* look the next free CE */

        if ((!SHR_BITGET(ce_rsrc_lcl ,ce_ndx))) {
            /*CE is available */
            if (qual_nof_bits > 16 || use_ce_32) {
                if ((arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx))){
                    (*free_ce) = ce_ndx;
                    break; /* found CE fit to size */
                }
            }else if ((!arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx)) || (mode == SOC_DPP_DBAL_CE_SEARCH_MODE_FIRST_FREE)) {
                    (*free_ce) = ce_ndx;
                    break; /* found CE fit to size */
                }
        }                
    }

    if((int)ce_ndx < 0) { /* no available CE */
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Error - no available CE! last_unchecked_ce = %d"
                                                              " ce_rsrc = %u, is_msb = %d, qual_nof_bits = %d "), last_unchecked_ce, ce_rsrc, is_msb, qual_nof_bits));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

           
uint32
    arad_pp_dbal_qualifier_full_size_get(int unit, ARAD_FP_DATABASE_STAGE stage, SOC_PPC_FP_QUAL_TYPE qual_type, uint8* qual_full_size, uint8* qual_is_in_hdr)
{
    uint8                           found = FALSE;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    ARAD_PMF_CE_HEADER_QUAL_INFO    header_qual_info;

    SOCDNX_INIT_FUNC_DEFS;

    /* See if the qualifier is in the internal fields */
    SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_field_info_find(unit, qual_type, stage, 0, &found, &irpp_qual_info));

    if (!found) {
        /* See if the qualifier is in the header packet */        
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_header_info_find(unit, qual_type, stage, &found, &header_qual_info));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid Qualifier")));
        }else {
            (*qual_full_size) = (header_qual_info.lsb - header_qual_info.msb + 1);
            (*qual_is_in_hdr) = 1;
        }
    }else{
        (*qual_full_size) = irpp_qual_info.info.qual_nof_bits;
        (*qual_is_in_hdr) = 0;
    }    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_next_available_tcam_db_id_get(int unit, uint32* tcam_db_id) 
{
    uint8 is_in_use;
    uint32 curr_tcam_db_id;
    
    SOCDNX_INIT_FUNC_DEFS;

    (*tcam_db_id) = 0xFFFFFFFF;

    for(curr_tcam_db_id = ARAD_PP_ISEM_ACCESS_TCAM_DYNAMIC_VALUES_BASE; curr_tcam_db_id < ARAD_PP_ISEM_ACCESS_TCAM_DYNAMIC_VALUES_END; curr_tcam_db_id++){

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.valid.get(unit, curr_tcam_db_id, &is_in_use ));
        if (!is_in_use) {
            (*tcam_db_id) = curr_tcam_db_id;
            break;
        }
    }

    if ((*tcam_db_id) == 0xFFFFFFFF) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_next_available_tcam_db_id_get - tcam_db_id available ")));
    }
   
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 
    arad_pp_dbal_next_available_lem_app_id_get(int unit, uint32* app_id)
{
    uint32 curr_app_id;
    uint32 prefix = ARAD_PP_FLP_MAP_PROG_NOT_SET;
    
    SOCDNX_INIT_FUNC_DEFS;

    (*app_id) = 0xFFFFFFFF;

    for(curr_app_id = ARAD_PP_LEM_ACCESS_KEY_TYPE_DBAL_BASE; curr_app_id <= ARAD_PP_LEM_ACCESS_KEY_TYPE_DBAL_END; curr_app_id++){
                   
        arad_pp_lem_access_app_to_prefix_get(unit, curr_app_id, &prefix);
        if (prefix == ARAD_PP_FLP_MAP_PROG_NOT_SET) {/* it means that the app_id is not used */
            (*app_id) = curr_app_id;
            break;
        }
    }

    if ((*app_id) == 0xFFFFFFFF) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_next_available_lem_app_id_get - no app_id available ")));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

/* only applicable for FLP */
STATIC uint32 
    arad_pp_dbal_next_available_key_get(int unit, ARAD_FP_DATABASE_STAGE stage, uint32 prog_id, SOC_DPP_DBAL_PROGRAM_KEYS* key_id)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_DPP_DBAL_PROGRAM_KEYS curr_key;
    
    SOCDNX_INIT_FUNC_DEFS;

    (*key_id) = SOC_DPP_DBAL_PROGRAM_NOF_KEYS;

    if (stage != ARAD_FP_DATABASE_STAGE_INGRESS_FLP) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR cannot allocate key dynamiclly to VTT stage")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, prog_id, &flp_lookups_tbl));
    
    for (curr_key = SOC_DPP_DBAL_PROGRAM_KEY_A; curr_key < ARAD_PMF_LOW_LEVEL_NOF_KEYS; curr_key++) {
        if (flp_lookups_tbl.elk_lkp_valid) {
            if (flp_lookups_tbl.elk_key_a_valid_bytes) {
                break;
                
            }
        }
        if (flp_lookups_tbl.lem_1st_lkp_valid) {
            if (flp_lookups_tbl.lem_1st_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.lem_2nd_lkp_valid) {
            if (flp_lookups_tbl.lem_2nd_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.lpm_1st_lkp_valid) {
            if (flp_lookups_tbl.lpm_1st_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.lpm_2nd_lkp_valid) {
            if (flp_lookups_tbl.lpm_2nd_lkp_key_select == curr_key) {
                continue;
            }
        }
        if (flp_lookups_tbl.tcam_lkp_db_profile != 0x3F) {
            if(flp_lookups_tbl.tcam_lkp_key_select == curr_key){
                continue;
            }
            if (!SOC_IS_JERICHO(unit)) {
                if( curr_key == SOC_DPP_DBAL_PROGRAM_KEY_B){
                    continue;/* key B is not available for TCAM in arad/arad+ */
                }
            }            
        }
        (*key_id) = curr_key;
        break;
    }

    if ((*key_id) == SOC_DPP_DBAL_PROGRAM_NOF_KEYS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR no available key found for lookup ")));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32 
    arad_pp_dbal_next_available_table_id_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS* table_id)
{
    uint32 curr_table_id;
    int is_table_initiated;
    
    SOCDNX_INIT_FUNC_DEFS;

    (*table_id) = SOC_DPP_DBAL_SW_TABLE_ID_INVALID;

    for(curr_table_id = SOC_DPP_DBAL_SW_TABLE_DYNAMIC_BASE_ID; curr_table_id <= SOC_DPP_DBAL_SW_TABLE_DYNAMIC_END_ID; curr_table_id++){

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_is_initiated(unit, curr_table_id, &is_table_initiated));
        if (is_table_initiated == 0) {
            (*table_id) = curr_table_id;
            break;
        }
    }

    if ((*table_id) == SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_next_available_table_id_get - no available tables ")));
    }

    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 
    arad_pp_dbal_sem_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES  physical_db_type, uint8 prefix, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_SW_TABLE_IDS curr_table_id;
    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; /* 6 Jericho, 4 arad  */
    int max_prefix_value = ((1 << max_prefix_len) - 1);    
    
    SOCDNX_INIT_FUNC_DEFS;

    if (prefix > max_prefix_value) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_sem_prefix_allocate - prefix out of range %d"), prefix));
    }

    switch(physical_db_type){
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.get(unit, (int)prefix, &curr_table_id ));
            if((curr_table_id != 0) && (curr_table_id != table_id)){
                /* LOG_CLI((BSL_META("SEM A prefix_allocate - prefix alredy in use %d, curr_table_id = %d new table_id = %d\n"), prefix, curr_table_id, table_id));*/
                /* for static table creation we allow to use the same prefix for multiple tables */
            }
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.set(unit, (int)prefix, table_id ));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.get(unit, (int)prefix, &curr_table_id ));
            if((curr_table_id != 0) && (curr_table_id != table_id)){/* prefix can be already allocated to this table */
                /* LOG_CLI((BSL_META("SEM B prefix_allocate - prefix alredy in use %d, curr_table_id = %d new table_id = %d\n"), prefix, curr_table_id, table_id));*/
                /* for static table creation we allow to use the same prefix for multiple tables */
            }
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.set(unit, (int)prefix, table_id ));
            break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_sem_prefix_allocate - physcal DB error %d"), physical_db_type));
        break;
    }    
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 
    arad_pp_dbal_sem_next_available_prefix_allocate(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8* prefix)
{
    SOC_DPP_DBAL_SW_TABLE_IDS curr_table_id = 0;
    int curr_prefix = 0;
    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; /* 6 Jericho, 4 arad  */
    int max_prefix_value = ((1 << max_prefix_len) - 1);    
    
    SOCDNX_INIT_FUNC_DEFS;

    (*prefix) = 0xFF;

    for (curr_prefix = 0; curr_prefix <=  max_prefix_value; curr_prefix++) {
        if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.get(unit, curr_prefix, &curr_table_id ));
        } else{
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.get(unit, curr_prefix, &curr_table_id ));
        }

        if (curr_table_id == 0) {
            (*prefix) = curr_prefix;
            break;
        }
    }

    if((*prefix) == 0xFF){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_sem_next_available_prefix_allocate - no available prefixes")));
    }

    if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.set(unit, (int)(*prefix), table_id ));
    }else{
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.set(unit, (int)(*prefix), table_id ));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32 
    arad_pp_dbal_sem_prefix_usage_get(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, uint8 db_prefix, int* count)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    (*count) = 0;

    for (i = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST; i <= SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST; i++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));
        if (table.is_table_initiated) {
            if (table.physical_db_type == physical_db_type) {
                if (db_prefix == table.db_prefix) {
                    (*count)++;
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
STATIC uint32
    arad_pp_dbal_tcam_table_init(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_FP_DATABASE_STAGE stage)
{
    ARAD_TCAM_ACCESS_INFO tcam_access_info;
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, key_size = 0;
    SOC_SAND_SUCCESS_FAILURE success;
    uint32 tcam_db_id = 0;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    arad_ARAD_TCAM_ACCESS_INFO_clear(&tcam_access_info);

    if (table.additional_table_info == 0) {
        /* dynamic table allocation - this case we need to assign handler for the key_type*/
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_tcam_db_id_get(unit, &tcam_db_id));
    }else {
        if (stage != ARAD_FP_DATABASE_STAGE_INGRESS_FLP) {
            tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table.additional_table_info);
        } else {
            tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table.additional_table_info);
        }
    }

    table.db_prefix = tcam_db_id;

    /* setting the db_prefix */
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

    if (table.additional_table_info != 0) {
        /* static tcams is initiated from isem/lem_access files, no need HW configuration */
        goto exit;
    }

    /* dynamic tcam table allocation - only for tcam tables that is created by DBAL */
    /* for TCAM tables that not need to be configured use ARAD_PP_ISEM_ACCESS_KEY_TYPE_DUMMY */

    /* values shared for all stages */
    tcam_access_info.is_direct          = FALSE;
    tcam_access_info.action_bitmap_ndx  = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
    tcam_access_info.min_banks          = ARAD_PP_ISEM_ACCESS_TCAM_DEF_MIN_BANKS; /* ARAD_PP_FRWRD_IP_TCAM_MIN_BANKS */
    tcam_access_info.user_data          = tcam_db_id;
    tcam_access_info.prefix_size        = ARAD_PP_ISEM_ACCESS_TCAM_DEF_PREFIX_SIZE; 
    tcam_access_info.callback           = arad_pp_fp_tcam_callback; /* generic callback that do noting */

    /* calculate the key size according to qualifiers*/
    for(i = 0; i < table.nof_qualifiers; i++) {
        key_size += table.qual_info[i].qual_nof_bits;
    }    

    if(key_size <= 80) {
        tcam_access_info.entry_size         = ARAD_TCAM_BANK_ENTRY_SIZE_80_BITS;
    } else {
        tcam_access_info.entry_size         = ARAD_TCAM_BANK_ENTRY_SIZE_160_BITS;   
    }    

    switch (stage) {
    
    case ARAD_FP_DATABASE_STAGE_INGRESS_VT:
        tcam_access_info.bank_owner         = ARAD_TCAM_BANK_OWNER_VT;
        /*tcam_access_info.callback           = arad_pp_isem_access_tcam_callback;*/
        break;

    case ARAD_FP_DATABASE_STAGE_INGRESS_TT:
        tcam_access_info.bank_owner         = ARAD_TCAM_BANK_OWNER_TT;
        /*tcam_access_info.callback           = arad_pp_isem_access_tcam_callback;  */      
        break;

    case ARAD_FP_DATABASE_STAGE_INGRESS_FLP:
        tcam_access_info.bank_owner         = ARAD_TCAM_BANK_OWNER_FLP_TCAM;
        tcam_access_info.action_bitmap_ndx  = ARAD_TCAM_ACTION_SIZE_FIRST_20_BITS;
        /*tcam_access_info.callback           = arad_pp_frwrd_ip_tcam_callback;        */
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_tcam_table_init - ilegal stage")));
        break;
    }    

    SOCDNX_IF_ERR_EXIT(arad_tcam_access_create_unsafe( unit, tcam_db_id, &tcam_access_info, &success));
    
    if(success != SOC_SAND_SUCCESS){

        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_tcam_table_init - arad_tcam_access_create_unsafe failed")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef DBAL_SERVICE_FUNCTIONS 


#define DBAL_TABLE_FUNCTIONS 
/************************************ TABLE FUNCTIONS ************************************/


/*
    the order of the qualifiers in qual_info affect the CE selected for the qualifer.
    the qulifier that added to qual_info[0] received the highest CE available (and so on).
    when CE 0 is the MSB of the KEY.
*/
uint32
    arad_pp_dbal_table_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 db_prefix, uint32 db_prefix_len, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db,
                              int nof_qualifiers, uint32 additional_info, SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 app_id = 0; /* can be one of the dynamic app IDs the reserved to DBAL, this value should be sent as prefix to table_create*/
    uint32 prefix; /* this is the actual prefix allocated to the table */
    uint8 sem_prefix;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    sal_memset(&table, 0x0, sizeof(SOC_DPP_DBAL_TABLE_INFO));

    if(table.is_table_initiated != 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_create - trying to init table more than once table_id %d"), table_id));
    }

    /* input params validate*/
    if (nof_qualifiers > SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("nof_qualifiers out of range")));}    

    for (i = 0; i < nof_qualifiers; i++) {
        if (qual_info[i].qual_nof_bits > DBAL_MAX_QUALIFIER_LENGTH) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ilegal qualifier nof_bits")));}

        if (qual_info[i].qual_offset%4 != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ilegal qualifier offset, must be multiple of 4 %d"), qual_info[i].qual_offset));}

        if (qual_info[i].qual_offset > (DBAL_MAX_QUALIFIER_LENGTH*2 -1)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ilegal qualifier offset")));}

        if (qual_info[i].qual_type > SOC_PPC_NOF_FP_QUAL_TYPES) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown qual_type")));}
    }


    switch (physical_db) {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            db_prefix = DBAL_PREFIX_NOT_DEFINED;
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            if (db_prefix == DBAL_PREFIX_NOT_DEFINED) {
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_next_available_prefix_allocate(unit, physical_db, table_id, &sem_prefix));
                additional_info = 0;
                db_prefix = (sem_prefix);
            }else{
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_allocate(unit, physical_db, db_prefix, table_id));
            }
            break;    

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            if ((additional_info == 0) && (db_prefix == DBAL_PREFIX_NOT_DEFINED)) {
                SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_lem_app_id_get(unit, &app_id));
                SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_prefix_alloc(unit,0, app_id, ARAD_PP_LEM_ACCESS_PREFIX_NUM_1, &prefix));

                db_prefix = app_id;
                additional_info = app_id;
                db_prefix_len = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
            }
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            if (db_prefix == DBAL_PREFIX_NOT_DEFINED) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("must give valid db_prefix")));
            }
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_stage_create - ilegal physical type")));
    }
        
    if(table.is_table_initiated == 0){
        DBAL_TABLE_INFO_UPDATE(table, db_prefix, db_prefix_len, physical_db, nof_qualifiers, additional_info, qual_info, table_name);
        table.is_table_initiated = 1;        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    }else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_create - trying to init table more than once table_id %d"), table_id));
    }
            
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_dynamic_table_create(int unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db, int nof_qualifiers,
                                  SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], char* table_name, SOC_DPP_DBAL_SW_TABLE_IDS* table_id)
{
    uint32 additional_info, db_prefix, db_prefix_len;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_table_id_get(unit, table_id));

    db_prefix = DBAL_PREFIX_NOT_DEFINED;

    switch (physical_db) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        additional_info = 0; /* by setting it to 0 the tcam_db_id and tcam resources will be allocated */
        db_prefix_len = 0; 
        break;
        
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:                
        additional_info = 0; /* by setting it to 0 the app_type will be allocated */
        db_prefix_len = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:        
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        additional_info = 0; /* always zero - value not used */
        db_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX;        
        break;
    
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_dynamic_table_create, physical db type not supported %d"), physical_db));
    }    
    
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, (*table_id), db_prefix, db_prefix_len, physical_db, nof_qualifiers, additional_info,
                                                  qual_info, table_name));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_stage_create(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_FP_DATABASE_STAGE stage )
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int curr_qual_id;
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;

    SOCDNX_INIT_FUNC_DEFS;    

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_stage_create - table not initiated %d"), table_id));
    }

    for (curr_qual_id = 0; curr_qual_id < table.nof_qualifiers; curr_qual_id++){

        qualifier_info = &(table.qual_info[curr_qual_id]);
        
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage, qualifier_info->qual_type, &(qualifier_info->qual_full_size), &(qualifier_info->qual_is_in_hdr)));
        if (qualifier_info->qual_nof_bits == 0) {
            qualifier_info->qual_nof_bits = qualifier_info->qual_full_size;
        }
    }

    /* updating the qualifiers info for the table */
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));

    if ((table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) && table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tcam_table_init(unit, table_id, stage));
    }    
    
exit:
    SOCDNX_FUNC_RETURN;
}



#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_table_clear_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    /* private table */
    if (JER_KAPS_ENABLE_PRIVATE_DB(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, NULL/*qual_vals*/, 1/*private_table*/, &kaps_table_id));
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_table_clear(unit, kaps_table_id));
    }

    /* public table */
    if (JER_KAPS_ENABLE_PUBLIC_DB(unit)) {
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, NULL/*qual_vals*/, 0/*private_table*/, &kaps_table_id));
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_table_clear(unit, kaps_table_id));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif


uint32
    arad_pp_dbal_table_clear(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - LEM not supported")));
        break;    

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - TCAM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - KBP not supported")));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - KBP not inuse")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - SEM_A not inuse")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - SEM_B not inuse")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_clear_kaps(unit, table_id));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - ilegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef DBAL_TABLE_FUNCTIONS 


uint32
    arad_pp_dbal_table_destroy(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    
    SOC_DPP_DBAL_TABLE_INFO table;
    int count = 0;
    uint32 lem_prefix = 0;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_disassociate_all(unit, table_id));

    switch (table.physical_db_type) {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_access_destroy_unsafe(unit, table.db_prefix));            
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, table.physical_db_type, table.db_prefix, &count));
            if (count > 1) {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.set(unit, table.db_prefix, 0));
            }
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:            
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, table.physical_db_type, table.db_prefix, &count));
            if (count > 1) {
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.set(unit, table.db_prefix, 0));
            }
            break;    

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_app_to_prefix_get( unit, table.db_prefix, &lem_prefix));
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_prefix_dealloc(unit, lem_prefix));            
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:            
            break;

        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_stage_create - ilegal physical type")));
    }

    sal_memset(&table, 0x0, sizeof(SOC_DPP_DBAL_TABLE_INFO));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_table_disassociate_all(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, prog_index;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    for (i = 0; i < table.nof_table_programs; i++) {
        /* deleting form the last program to the first, just because it is more simple to manage */
        prog_index = (table.nof_table_programs - i) - 1;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_table_disassociate(unit, table.table_programs[prog_index].program_id,
                                                                        table.table_programs[prog_index].stage, table_id));        
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

#define DBAL_PROGRAM_FUNCTIONS 

/************************************ PROGRAM FUNCTIONS ************************************/

STATIC uint32
    dbal_program_configure(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, int key_id, SOC_DPP_DBAL_QUAL_INFO qual_info, int is_msb, int ce_id)
{
    uint8                           found = FALSE;
    ARAD_PMF_CE_PACKET_HEADER_INFO  ce_packet_header_info;
    ARAD_PMF_CE_IRPP_QUALIFIER_INFO irpp_qual_info;
    ARAD_PMF_CE_HEADER_QUAL_INFO    header_qual_info;

    SOCDNX_INIT_FUNC_DEFS;

    if (is_msb) { /* if we using MSB the ce vaues are also between 0-15*/
        ce_id -= (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1);
    }

    /* See if the qualifier is in the internal fields */
    SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_field_info_find(unit, qual_info.qual_type, stage, is_msb, &found, &irpp_qual_info));

    if (found) {
    /* Same key at both lookup since it is much more simple */
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_internal_info_entry_set_unsafe(
                unit,
                stage,
                program_id,
                key_id,
                ce_id, /* Instruction ID */
                is_msb,
                0, /* is_second_lookup */
                qual_info.qual_offset, /* Offset to start within the qualifier */
                0, /* lost_bits */
                qual_info.qual_nof_bits,
                qual_info.qual_type
            ));

    }else {

        /* See if the qualifier is in the header packet */        
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_header_info_find(unit, qual_info.qual_type, stage, &found, &header_qual_info));

        if (found) {
          ARAD_PMF_CE_PACKET_HEADER_INFO_clear(&ce_packet_header_info);
          ce_packet_header_info.sub_header = header_qual_info.header_ndx_0;
          ce_packet_header_info.offset = header_qual_info.msb + qual_info.qual_offset;
          /* Set the number of bits to be the user if defined, otherwise all the field */
          ce_packet_header_info.nof_bits = qual_info.qual_nof_bits;

          SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_packet_header_entry_set_unsafe(unit, stage, program_id, key_id, ce_id, is_msb, 0, &ce_packet_header_info));
        }
    }

    if (!found) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid Qualifier")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_flp_hw_based_key_enable(int unit, int program_id, SOC_DPP_HW_KEY_LOOKUP hw_key_based_lookup_enable)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, program_id, &flp_lookups_tbl));    

    switch(hw_key_based_lookup_enable ) {
    
    case SOC_DPP_HW_KEY_LOOKUP_IN_LEM_1ST:
        flp_lookups_tbl.lem_1st_lkp_valid      = 1;
        flp_lookups_tbl.lem_1st_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL; 
        flp_lookups_tbl.lem_1st_lkp_and_value  = 0x0;
        flp_lookups_tbl.lem_1st_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
        break;

    case SOC_DPP_HW_KEY_LOOKUP_IN_LEM_2ND:
        flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
        flp_lookups_tbl.lem_2nd_lkp_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL; 
        flp_lookups_tbl.lem_2nd_lkp_and_value  = 0x0;
        flp_lookups_tbl.lem_2nd_lkp_or_value   = ARAD_PP_FLP_ETH_KEY_OR_MASK(unit);
        break;

    case SOC_DPP_HW_KEY_LOOKUP_IN_LEARN_KEY:
        flp_lookups_tbl.learn_key_select = ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL;
        break;

    case SOC_DPP_HW_KEY_LOOKUP_DISABLED:
    default:
        break;
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, program_id, &flp_lookups_tbl));
        
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_flp_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove, uint8* lookup_used)
{
    ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
    SOC_DPP_DBAL_TABLE_INFO table;
    int max_prefix_len = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS; /* 5 Jericho 4 arad  */
    int max_prefix_value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
    uint32 lem_prefix = 0;
    uint32 access_profile_id;    

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, table_program.program_id, &flp_lookups_tbl));

    if (table_program.lookup_number == 0xFF ) {
        /* in this case we need to find the available lookup to use */
        table_program.lookup_number = 1;
        if (table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM) {
            /* only for LEM the lookup number is allocated dynamiclly */
            if(flp_lookups_tbl.lem_1st_lkp_valid == 0){
                table_program.lookup_number = 1;
            } else if(flp_lookups_tbl.lem_2nd_lkp_valid == 0 ){
                table_program.lookup_number = 2;
            } else{
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("no available lookup number")));
            }
        } else if(table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM){
            if(flp_lookups_tbl.tcam_lkp_db_profile != 0x3F){
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("no available lookups for TCAM")));
            }
        }
    }

    if (table_program.lookup_number > 2 ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number")));
    }

    if (table_program.lookup_number == 0) {
        LOG_CLI((BSL_META("\t program lookup update with lookup number 0 \n")));
        goto exit;
    }

    (*lookup_used) = table_program.lookup_number;

    switch(table.physical_db_type){
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        arad_pp_lem_access_app_to_prefix_get(unit,table.db_prefix, &lem_prefix);
        if (table_program.lookup_number == 1) {
            flp_lookups_tbl.lem_1st_lkp_valid      = 1;
            if(is_to_remove){
                flp_lookups_tbl.lem_1st_lkp_valid      = 0;
            }
            flp_lookups_tbl.lem_1st_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lem_1st_lkp_and_value  = max_prefix_value >> table.db_prefix_len;            
            flp_lookups_tbl.lem_1st_lkp_or_value   = lem_prefix << (max_prefix_len - table.db_prefix_len);            
        } else {/*table_program.lookup_number == 2 */
            flp_lookups_tbl.lem_2nd_lkp_valid      = 1;
            if(is_to_remove){
                flp_lookups_tbl.lem_2nd_lkp_valid      = 0;
            }
            flp_lookups_tbl.lem_2nd_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lem_2nd_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lem_2nd_lkp_or_value   = lem_prefix << (max_prefix_len - table.db_prefix_len);			
        }
        break;
    
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
        if (table_program.lookup_number == 1) {
            flp_lookups_tbl.lpm_1st_lkp_valid      = 1;
            if(is_to_remove){
                flp_lookups_tbl.lpm_1st_lkp_valid      = 0;
            }
            flp_lookups_tbl.lpm_1st_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lpm_1st_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lpm_1st_lkp_or_value   = table.db_prefix << (max_prefix_len - table.db_prefix_len);
            if(table_program.public_lpm_lookup_size != 0){
                flp_lookups_tbl.lpm_public_1st_lkp_valid = 1;
                if(is_to_remove){
                    flp_lookups_tbl.lpm_public_1st_lkp_valid = 0;
                }
                if (table_program.public_lpm_lookup_size != SOC_DPP_DBAL_USE_COMPLETE_KEY) {
                    flp_lookups_tbl.lpm_public_1st_lkp_key_select = 4; /* : Lpm_1stLkpKey[0 +: 4*LpmPublic_2ndLkpKeySize] */
                    flp_lookups_tbl.lpm_public_1st_lkp_key_size   = table_program.public_lpm_lookup_size;
                } else {
                    flp_lookups_tbl.lpm_public_1st_lkp_key_select = table_program.key_id;
                    flp_lookups_tbl.lpm_public_1st_lkp_key_size   = 0;
                }
                
                flp_lookups_tbl.lpm_public_1st_lkp_and_value = max_prefix_value >> table.db_prefix_len;
                flp_lookups_tbl.lpm_public_1st_lkp_or_value  = table.db_prefix << (max_prefix_len - table.db_prefix_len);
            }
        } else {/*table_program.lookup_number == 2 */
            flp_lookups_tbl.lpm_2nd_lkp_valid      = 1;
            if(is_to_remove){
                flp_lookups_tbl.lpm_2nd_lkp_valid      = 0;
            }
            flp_lookups_tbl.lpm_2nd_lkp_key_select = table_program.key_id;
            flp_lookups_tbl.lpm_2nd_lkp_and_value  = max_prefix_value >> table.db_prefix_len;
            flp_lookups_tbl.lpm_2nd_lkp_or_value   = table.db_prefix << (max_prefix_len - table.db_prefix_len);

            if(table_program.public_lpm_lookup_size != 0){
                flp_lookups_tbl.lpm_public_2nd_lkp_valid = 1;
                if(is_to_remove){
                    flp_lookups_tbl.lpm_public_2nd_lkp_valid = 0;
                }
                if (table_program.public_lpm_lookup_size != SOC_DPP_DBAL_USE_COMPLETE_KEY) {
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_select = 4; /* : Lpm_1stLkpKey[0 +: 4*LpmPublic_2ndLkpKeySize] */
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_size   = table_program.public_lpm_lookup_size;
                } else {
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_select = table_program.key_id;
                    flp_lookups_tbl.lpm_public_2nd_lkp_key_size   = 0;
                }
                
                flp_lookups_tbl.lpm_public_2nd_lkp_and_value = max_prefix_value >> table.db_prefix_len;
                flp_lookups_tbl.lpm_public_2nd_lkp_or_value  = table.db_prefix << (max_prefix_len - table.db_prefix_len);
            }
        }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        /* in TCAM for arad+ we take 160 bit (key a+b or key c)the key selection values are only 0/1 */
        if (SOC_IS_JERICHO(unit)) {
            flp_lookups_tbl.tcam_lkp_key_select = table_program.key_id;
        }else{
            if (table_program.key_id == SOC_DPP_DBAL_PROGRAM_KEY_C) {
                flp_lookups_tbl.tcam_lkp_key_select = 1;
            }else {
                flp_lookups_tbl.tcam_lkp_key_select = 0;
            }
        }        
        /* flp_lookups_tbl.tcam_lkp_db_profile = table.db_prefix;*/
        SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
        flp_lookups_tbl.tcam_lkp_db_profile = access_profile_id;
        if(is_to_remove){
                flp_lookups_tbl.tcam_lkp_db_profile = 0x3F;
            }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        {
            ARAD_PP_IHB_FLP_PROCESS_TBL_DATA flp_process_tbl;

            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_process_tbl_get_unsafe(unit, table_program.program_id, &flp_process_tbl));
            flp_process_tbl.include_elk_fwd_in_result_a = 1;
            arad_pp_ihb_flp_process_tbl_set_unsafe(unit, table_program.program_id, &flp_process_tbl);
            flp_lookups_tbl.elk_lkp_valid = 1;
            if(is_to_remove){
                flp_lookups_tbl.elk_lkp_valid = 0;
            }
            flp_lookups_tbl.elk_wait_for_reply = 1;
             
            
            switch(table_program.key_id){
                case SOC_DPP_DBAL_PROGRAM_KEY_A:
                    flp_lookups_tbl.elk_key_a_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_B:
                    flp_lookups_tbl.elk_key_b_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_C:
                    flp_lookups_tbl.elk_key_c_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_D:
                    flp_lookups_tbl.elk_key_d_lsb_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_A_MSB:
                    flp_lookups_tbl.elk_key_a_msb_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_B_MSB:
                    flp_lookups_tbl.elk_key_b_msb_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_C_MSB:
                    flp_lookups_tbl.elk_key_c_msb_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                case SOC_DPP_DBAL_PROGRAM_KEY_D_MSB:
                    flp_lookups_tbl.elk_key_d_msb_valid_bytes = (table_program.nof_bits_used_in_key/8);
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid key id")));
            }
        }
        break;            

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid physical DB TYPE")));
    }    
  
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihb_flp_lookups_tbl_set_unsafe(unit, table_program.program_id, &flp_lookups_tbl));
    
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 
    arad_pp_dbal_vt_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove)
{
    ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA ihp_vtt1st_key_construction_tbl_data;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 access_profile_id;

    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; /* 6 Jericho, 4 arad  */
    int max_prefix_value = ((1 << max_prefix_len) - 1);    

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    soc_sand_os_memset(&ihp_vtt1st_key_construction_tbl_data, 0x0, sizeof(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA));

    if (table_program.lookup_number == 0xFF ) {
        table_program.lookup_number = 0;
    }

    if (table_program.lookup_number > 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number")));
    }    

    ihp_vtt1st_key_construction_tbl_data.dbal = 1;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(unit, table_program.program_id, &ihp_vtt1st_key_construction_tbl_data ));

    switch(table.physical_db_type){
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        ihp_vtt1st_key_construction_tbl_data.isa_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt1st_key_construction_tbl_data.isa_lookup_enable = 0;
        }
        ihp_vtt1st_key_construction_tbl_data.isa_and_mask      = max_prefix_value >> table.db_prefix_len; /* most of the cases should be zero */
        ihp_vtt1st_key_construction_tbl_data.isa_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); /* most of the cases full prefix */
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        ihp_vtt1st_key_construction_tbl_data.isb_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt1st_key_construction_tbl_data.isb_lookup_enable = 0;
        }
        ihp_vtt1st_key_construction_tbl_data.isb_and_mask      = max_prefix_value >> table.db_prefix_len; /* most of the cases should be zero */
        ihp_vtt1st_key_construction_tbl_data.isb_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); /* most of the cases full prefix */
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        /* ihp_vtt1st_key_construction_tbl_data.tcam_db_profile   = table.db_prefix;*/
        SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
        ihp_vtt1st_key_construction_tbl_data.tcam_db_profile = access_profile_id;
        if (is_to_remove) {
            ihp_vtt1st_key_construction_tbl_data.tcam_db_profile = 0x3F;
        }
        break;
            
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid physical DB TYPE")));
    }    

    ihp_vtt1st_key_construction_tbl_data.dbal = 1;
  
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(unit, table_program.program_id, &ihp_vtt1st_key_construction_tbl_data));
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_tt_lookup_update(int unit, DBAL_PROGRAM_INFO table_program, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint8 is_to_remove)
{
    ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  ihp_vtt2nd_key_construction_tbl_data;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 access_profile_id;

    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; /* 6 Jericho, 4 arad  */
    int max_prefix_value = ((1 << max_prefix_len) - 1);    

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    soc_sand_os_memset(&ihp_vtt2nd_key_construction_tbl_data, 0x0, sizeof(ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA));

    if (table_program.lookup_number == 0xFF ) {
        table_program.lookup_number = 0;
    }
    if (table_program.lookup_number > 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid lookup number")));
    }    

    ihp_vtt2nd_key_construction_tbl_data.dbal = 1;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(unit, table_program.program_id, &ihp_vtt2nd_key_construction_tbl_data ));

    switch(table.physical_db_type){
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        ihp_vtt2nd_key_construction_tbl_data.isa_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt2nd_key_construction_tbl_data.isa_lookup_enable = 0;
        }
        ihp_vtt2nd_key_construction_tbl_data.isa_and_mask      = max_prefix_value >> table.db_prefix_len; /* most of the cases should be zero */
        ihp_vtt2nd_key_construction_tbl_data.isa_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); /* most of the cases full prefix */
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        ihp_vtt2nd_key_construction_tbl_data.isb_lookup_enable = 1;
        if (is_to_remove) {
            ihp_vtt2nd_key_construction_tbl_data.isb_lookup_enable = 0;
        }
        ihp_vtt2nd_key_construction_tbl_data.isb_and_mask      = max_prefix_value >> table.db_prefix_len; /* most of the cases should be zero */
        ihp_vtt2nd_key_construction_tbl_data.isb_or_mask       = table.db_prefix << (max_prefix_len - table.db_prefix_len); /* most of the cases full prefix */
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:        
        /*ihp_vtt2nd_key_construction_tbl_data.tcam_db_profile   = table.db_prefix;*/
        SOCDNX_SAND_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tcam.tcam_db.access_profile_id.get(unit, table.db_prefix, 0, &access_profile_id ));
        ihp_vtt2nd_key_construction_tbl_data.tcam_db_profile = access_profile_id;
        if (is_to_remove) {
            ihp_vtt2nd_key_construction_tbl_data.tcam_db_profile = 0x3F;
        }
        break;
            
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid physical DB TYPE")));
    }    

    ihp_vtt2nd_key_construction_tbl_data.dbal = 1;
  
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(unit, table_program.program_id, &ihp_vtt2nd_key_construction_tbl_data));
    
exit:
    SOCDNX_FUNC_RETURN;
}


/*
    this function connects a table to a specific program.
    the connection configure the key construction and lookup part of the program.
*/
uint32
    arad_pp_dbal_program_to_tables_associate(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                             SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], int nof_valid_keys)
{
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;
    int                         curr_key_indx, curr_qual_id, curr_table_program, table_id;
    uint32                      ce_rsrc[1];
    ARAD_PP_KEY_CE_ID           ce_ndx = 0;
    uint32                      last_unchecked_ce;
    SOC_DPP_DBAL_TABLE_INFO     table;
    int                         nof_bits_used_in_key;
    int                         is_msb; /* which key is costructed */
    SOC_DPP_DBAL_PROGRAM_KEYS   curr_key;
    int                         curr_use_32_bit_ce;

    SOCDNX_INIT_FUNC_DEFS;

    if (stage < 0 || stage >= ARAD_NOF_FP_DATABASE_STAGES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid stage %d. \n\rThe range is: 0 - ARAD_NOF_FP_DATABASE_STAGES-1.\n\r"), stage));
    }
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, ce_rsrc));

    for (curr_key_indx = 0; curr_key_indx < nof_valid_keys; curr_key_indx++){

        table_id = keys_to_table_id[curr_key_indx].sw_table_id;

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_stage_create(unit, table_id, stage));        

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));        

        nof_bits_used_in_key = 0;
        last_unchecked_ce = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
        is_msb = FALSE;

        curr_table_program = table.nof_table_programs;
        curr_key = keys_to_table_id[curr_key_indx].key_id;
        if (keys_to_table_id[curr_key_indx].key_id == SOC_DPP_DBAL_PROGRAM_NOF_KEYS) { /* in this case we will allocate the next available key */
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_key_get(unit,stage, program_id, &curr_key));
            keys_to_table_id[curr_key_indx].key_id = curr_key;
        }else if(keys_to_table_id[curr_key_indx].key_id > SOC_DPP_DBAL_PROGRAM_KEY_D) {
            curr_key = curr_key - SOC_DPP_DBAL_PROGRAM_NOF_COMPLETE_KEYS;
            is_msb = TRUE;
        }

        for (curr_qual_id = 0; curr_qual_id < table.nof_qualifiers; curr_qual_id++){

            qualifier_info = &(table.qual_info[curr_qual_id]);
            
            /* validations for key size */            
            if (nof_bits_used_in_key + qualifier_info->qual_nof_bits > DBAL_KEY_C_ARAD_SIZE_IN_BITS){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associat nof bits is bigger than 160, current nof bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
            }

            if( ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) {
                if (((nof_bits_used_in_key + qualifier_info->qual_nof_bits) > DBAL_KEY_SIZE_IN_BITS) && (!is_msb)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associate nof bits for LSB key is bigger than 80 current bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
                }            
            } else if( (curr_key != SOC_DPP_DBAL_PROGRAM_KEY_C) && ((nof_bits_used_in_key + qualifier_info->qual_nof_bits) > DBAL_KEY_SIZE_IN_BITS)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ERROR Tables associat nof bits is bigger than 80 for key current bits is %d"), nof_bits_used_in_key + qualifier_info->qual_nof_bits));
            }                         

            if (use_32_bit_ce) {
                curr_use_32_bit_ce = use_32_bit_ce[curr_key_indx][curr_qual_id];
            }else {
                curr_use_32_bit_ce = 0;
            }

            /* function returns error if no CE available */
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_ce_get(unit, qualifier_info->qual_nof_bits, curr_use_32_bit_ce, last_unchecked_ce, stage, *ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE_USE_FIRST_16_BIT_FIRST, is_msb, &ce_ndx));

            last_unchecked_ce = ce_ndx - 1;

            SOCDNX_IF_ERR_EXIT(dbal_program_configure(unit, program_id, stage, curr_key, (*qualifier_info), is_msb, ce_ndx));
            /* last_unchecked_ce can't be 134217727 as the coverity claims. It can be only if stage is not between 0 and ARAD_NOF_FP_DATABASE_STAGES - 1.
             * We do such check above 
             */
            /* coverity[overrun-local:FALSE] */
            SHR_BITSET(ce_rsrc, ce_ndx);
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, program_id, 0, *ce_rsrc));
            table.table_programs[curr_table_program].ce_assigned[curr_qual_id] = ce_ndx;
            nof_bits_used_in_key += qualifier_info->qual_nof_bits;
            if ((nof_bits_used_in_key == DBAL_KEY_SIZE_IN_BITS) && (!is_msb)) { /* exactly 80 bits, it means that from now we will use the MSB CEs to fill the MSB part of the key */
                if (ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) { /*check if there is MSB part to the key..*/
                    is_msb = TRUE;
                    last_unchecked_ce = (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB) + 1;
				}
            }            
        }

        /* update table with the associated program and stage */                    
        table.table_programs[curr_table_program].stage = stage;
        table.table_programs[curr_table_program].program_id = program_id;
        table.table_programs[curr_table_program].key_id = keys_to_table_id[curr_key_indx].key_id;
        table.table_programs[curr_table_program].lookup_number = keys_to_table_id[curr_key_indx].lookup_number;
        table.table_programs[curr_table_program].nof_bits_used_in_key = nof_bits_used_in_key;
        table.table_programs[curr_table_program].public_lpm_lookup_size = keys_to_table_id[curr_key_indx].public_lpm_lookup_size;

        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_FLP) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_lookup_update(unit, table.table_programs[curr_table_program], table_id, 0, &(table.table_programs[curr_table_program].lookup_number)));
        } else if(stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vt_lookup_update(unit, table.table_programs[curr_table_program], table_id, 0));
        } else if(stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tt_lookup_update(unit, table.table_programs[curr_table_program], table_id, 0));
        }        
        table.nof_table_programs++;

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/* a backdoor to insert which CE ID to each qualifier
   use the array qualifier_to_ce_id to indicate which qualifier use which ce_id.
   the CE ids is running between 0-31 when 0-15 is for LSB key and 16-31 is for MSB key.
   the qualifiers in qualifier_to_ce_id should be in the same order of the qualifiers set in the table. */
uint32
    arad_pp_dbal_program_to_tables_associate_implicit(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                                      SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],
                                                      uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                      int nof_valid_keys)
{    
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;
    int                         curr_key_indx, curr_qual_id;
    int                         nof_table_programs;
    SOC_DPP_DBAL_TABLE_INFO     table;
    uint32                      ce_rsrc[1];
    ARAD_PP_KEY_CE_ID           ce_ndx;
    int                         is_msb = 0;
    int                         nof_bits_used_in_key = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, ce_rsrc));
    
    for (curr_key_indx = 0; curr_key_indx < nof_valid_keys; curr_key_indx++){

        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_stage_create(unit, keys_to_table_id[curr_key_indx].sw_table_id, stage));

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, keys_to_table_id[curr_key_indx].sw_table_id, &table));
        nof_table_programs = table.nof_table_programs;

        for (curr_qual_id = 0; curr_qual_id < table.nof_qualifiers; curr_qual_id++){
            
            /* configure the HW */
            qualifier_info = &(table.qual_info[curr_qual_id]);
            ce_ndx = qualifier_to_ce_id[curr_key_indx][curr_qual_id];            

            nof_bits_used_in_key += qualifier_info->qual_nof_bits;
            if (ce_ndx > ARAD_PMF_LOW_LEVEL_CE_NDX_MAX) {                
                is_msb = 1;
            }

            SOCDNX_IF_ERR_EXIT(dbal_program_configure(unit, program_id, stage, keys_to_table_id[curr_key_indx].key_id, (*qualifier_info), is_msb, ce_ndx));
            SHR_BITSET(ce_rsrc, ce_ndx);
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, program_id, 0, *ce_rsrc));
            table.table_programs[nof_table_programs].ce_assigned[curr_qual_id] = ce_ndx;
        }

        /* update table with the associated program and stage */        
        table.table_programs[nof_table_programs].stage = stage;
        table.table_programs[nof_table_programs].program_id = program_id;
        table.table_programs[nof_table_programs].key_id = keys_to_table_id[curr_key_indx].key_id;
        table.table_programs[nof_table_programs].lookup_number = keys_to_table_id[curr_key_indx].lookup_number;
        table.table_programs[nof_table_programs].nof_bits_used_in_key = nof_bits_used_in_key;
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_FLP) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_lookup_update(unit, table.table_programs[nof_table_programs], keys_to_table_id[curr_key_indx].sw_table_id, 0, &(table.table_programs[nof_table_programs].lookup_number)));
        } else if(stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vt_lookup_update(unit, table.table_programs[nof_table_programs], keys_to_table_id[curr_key_indx].sw_table_id, 0));
        } else if(stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tt_lookup_update(unit, table.table_programs[nof_table_programs], keys_to_table_id[curr_key_indx].sw_table_id, 0));
        }
        table.nof_table_programs ++;

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, keys_to_table_id[curr_key_indx].sw_table_id, &table));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_program_table_disassociate(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    int i, is_msb;
    int program_index = -1;
    uint32 ce_rsrc[1];
    SOC_DPP_DBAL_TABLE_INFO table;
    ARAD_PP_KEY_CE_ID       ce_ndx;    
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, ce_rsrc));

    for (i = 0; i < table.nof_table_programs; i++) {
        if(table.table_programs[i].program_id == program_id){
            if (stage == table.table_programs[i].stage) {
                program_index = i;
                break;
            }            
        }
    }

    if (program_index == -1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_table_disassociate table %d not associated to program %d"), table_id, program_id));
    }

    if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_FLP) {
            uint8 lookup_used;
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_lookup_update(unit, table.table_programs[program_index], table_id, 1, &(lookup_used)));
        } else if(stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vt_lookup_update(unit, table.table_programs[program_index], table_id, 1));
        } else if(stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT){
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tt_lookup_update(unit, table.table_programs[program_index], table_id, 1));
        }

    /* WARNNING:: sharing of CEs is not supported */
    for (i = 0; i < table.nof_qualifiers; i++) {
        is_msb = 0;
        ce_ndx = table.table_programs[program_index].ce_assigned[i];
        SHR_BITCLR(ce_rsrc, ce_ndx);
        if (ce_ndx > ARAD_PMF_LOW_LEVEL_CE_NDX_MAX) {                
            is_msb = 1;
            ce_ndx -= (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX +1);
        }
        SOCDNX_SAND_IF_ERR_EXIT(arad_pmf_ce_nop_entry_set_unsafe(unit, stage, program_id, table.table_programs[program_index].key_id, ce_ndx, is_msb, 0, TRUE));
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.set(unit, stage, program_id, 0, *ce_rsrc));

    sal_memset(&(table.table_programs[program_index]), 0x0, sizeof(DBAL_PROGRAM_INFO));

    if (program_index != table.nof_table_programs-1) {
        sal_memcpy(&(table.table_programs[program_index]), &(table.table_programs[table.nof_table_programs-1]), sizeof(DBAL_PROGRAM_INFO));
    }
    table.nof_table_programs--;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.set(unit, table_id, &table));
    
exit:
    SOCDNX_FUNC_RETURN;
}
#undef DBAL_PROGRAM_FUNCTIONS 


#define DBAL_ENTRY_MANAGMENT_FUNCTIONS
/********* ENTRY MANAGEMENT FUNCTIONS *********/

STATIC uint32
  arad_pp_dbal_entry_add_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    ARAD_PP_LEM_ACCESS_ACK_STATUS      ack;
    ARAD_PP_LEM_ACCESS_REQUEST         request;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lem_buffer(unit, qual_vals, table_id, &(request.key)));

    request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_add_unsafe(unit, &request, (ARAD_PP_LEM_ACCESS_PAYLOAD*)(payload), &ack));

    if(ack.is_success == TRUE){
      *success = SOC_SAND_SUCCESS;
    } else{
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_add_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};    
    uint32  entry_payload[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)];
    
    SOCDNX_INIT_FUNC_DEFS;

    entry_payload[0] = *((uint32*)payload); /* payload in SEM is uint32 */  

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, entry_key));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_add_unsafe(unit, entry_key, entry_payload, sem_id, success));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_add_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority,  SOC_SAND_SUCCESS_FAILURE *success)
{
    /* adding entry to tcam from VTT/FLP */
    uint32 data_indx, tcam_db_id;
    uint8  stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2 + 1]; /* the stream consist of value mask table Id */
    SOC_SAND_HASH_TABLE_INFO *hash_tbl;
    uint8 found, entry_added;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;
    
    SOC_DPP_DBAL_TABLE_INFO table;
    uint8 is_in_vtt_stage = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    is_in_vtt_stage = ((table.table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table.table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    /* recieve hash table */
    hash_tbl = arad_sw_db_entry_key_to_entry_id_hash_get(unit);
    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table_id), 1);

    /* SOCDNX_SAND_IF_ERR_EXIT(soc_sand_U32_to_U8(entry.value, ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES,stream)); */    
  
    if (is_in_vtt_stage) {
        action.value[0] = (*(ARAD_PP_ISEM_ACCESS_ENTRY*)payload).sem_result_ndx;
    } else { /* in FLP stage the result is uint32 */
        action.value[0] = (*(uint32*)payload);
    }
    
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(unit, hash_tbl, stream, &data_indx, &found));

    if (!found) {
        /* Insert the rule to the route_key -> entry_id table*/
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_add(unit, hash_tbl, stream, &data_indx, &entry_added ));
    } else {
        entry_added = TRUE;
    }

    if (entry_added) { /* Remove the old entry, if applicable */
        tcam_db_id = table.db_prefix;
        
        SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_remove_unsafe(unit, FALSE /* is_bank_freed_if_no_entry */, tcam_db_id, data_indx));
        entry.valid = TRUE;
        /* Insert the new rule to the TCAM */
        if (!is_in_vtt_stage) {
            entry.is_for_update = found;               
        }

        SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_add_unsafe(unit, tcam_db_id, data_indx, FALSE, priority, &entry, &action, success ));

        if ((*success != SOC_SAND_SUCCESS) && (!found)) {
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(unit, hash_tbl, data_indx));
        }
    } else {
        *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)    
STATIC uint32
    arad_pp_dbal_entry_add_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority,  SOC_SAND_SUCCESS_FAILURE *success)
{
    
    uint32  table_size_in_bytes, table_payload_in_bytes;
    /*uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];*/
    /*uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];*/
    uint8   elk_data[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    /*uint8   elk_mask[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];*/
    uint8   elk_ad_value[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  prefix_len;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(unit, table_size_in_bytes, entry_key, entry_mask, elk_data, elk_mask));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_to_kbp_payload_buffer_encode(unit, table_payload_in_bytes, ((ARAD_TCAM_ACTION *)payload)->value, elk_ad_value));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(elk_mask, table_size_in_bytes, &prefix_len));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_lpm_route_add(unit, table.db_prefix, 0, prefix_len, elk_data, elk_ad_value, success));
        /* entry Id in this case is has no significance  */

#ifdef KBP_COMPARE_DEBUG
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_kbp_compare(unit, table.db_prefix, elk_data));
#endif /* KBP_COMPARE_DEBUG */

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_add_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority,  SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  prefix_len;
    uint8   data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint8   ad_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1/*private_table*/, &kaps_table_id));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_encode(unit, payload, ad_bytes));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_add_hw(unit, kaps_table_id, prefix_len, data_bytes, ad_bytes, success));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_add(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_lem(unit, table_id, qual_vals, payload, success));
        break;    

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_tcam(unit, table_id, qual_vals, payload, priority, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_kbp(unit, table_id, qual_vals, payload, priority, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KBP not inuse")));
#endif         
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, table_id, qual_vals, payload, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_B, table_id, qual_vals, payload, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_kaps(unit, table_id, qual_vals, payload, priority, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - ilegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry added to table %s \n"), table.table_name));    
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
    LOG_CLI((BSL_META("\nLIF: %d \n"), *(uint32*)payload));
#endif

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, (table.nof_entries_added_to_table++)));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_delete_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, entry_key));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_remove_unsafe(unit, entry_key, sem_id));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_delete_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    ARAD_PP_LEM_ACCESS_ACK_STATUS      ack;
    ARAD_PP_LEM_ACCESS_REQUEST         request;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lem_buffer(unit, qual_vals, table_id, &(request.key)));

    request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_remove_unsafe(unit, &request, &ack));

    if(ack.is_success == TRUE){
      *success = SOC_SAND_SUCCESS;
    } else{
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_delete_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{

    uint32 data_indx;
    uint8  stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2 + 1]; /* the stream consist of value mask table Id */
    SOC_SAND_HASH_TABLE_INFO *hash_tbl;
    uint8 found;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;

    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    /* recieve hash table */
    hash_tbl = arad_sw_db_entry_key_to_entry_id_hash_get(unit);
    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table_id), 1);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(unit, hash_tbl, stream, &data_indx, &found));

    if (!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry doesn't exists")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(unit, hash_tbl,data_indx));

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit, table.db_prefix, data_indx));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_delete_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    
    uint32  table_size_in_bytes, table_payload_in_bytes;
    uint8   elk_data[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    /*uint8   elk_mask[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];*/
    /*uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];*/
    /*uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];*/
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  prefix_len;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*success) = SOC_SAND_FAILURE_INTERNAL_ERR;

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(unit, table_size_in_bytes, entry_key, entry_mask, elk_data, elk_mask));*/
    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(elk_mask, table_size_in_bytes, &prefix_len));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove(unit, table.db_prefix, prefix_len, elk_data));
        /* entry Id in this case is has no significance  */

    (*success) = SOC_SAND_SUCCESS;

#ifdef KBP_COMPARE_DEBUG
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_kbp_compare(unit, table.db_prefix, elk_data ));
#endif

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_delete_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  prefix_len;
    uint8   data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*success) = SOC_SAND_FAILURE_INTERNAL_ERR;

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1/*private_table*/, &kaps_table_id));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));
    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_remove_hw(unit, kaps_table_id, prefix_len, data_bytes));

    (*success) = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_delete(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_lem(unit, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_tcam(unit, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_kbp(unit, table_id, qual_vals, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - KBP not inuse")));
#endif 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_B, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_kaps(unit, table_id, qual_vals, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - ilegal physical DB type")));
        break;
    }
    
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, (table.nof_entries_added_to_table--)));

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry deleted from table %s \n"),table.table_name));    
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_get_lem(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL  qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint8 *found)
{
    ARAD_PP_LEM_ACCESS_REQUEST         request;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lem_buffer(unit, qual_vals, table_id, &(request.key)));
    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_by_key_get_unsafe(unit, &request.key, (ARAD_PP_LEM_ACCESS_PAYLOAD*)(payload), found));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_get_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint8 *found)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_payload[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)];
    
    SOCDNX_INIT_FUNC_DEFS;

    entry_payload[0] = 0;
                                                                                                                    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, entry_key));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_get_unsafe(unit, entry_key, entry_payload, sem_id, found));

    *((uint32*)payload) = entry_payload[0]; /* payload in SEM is uint32 */

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_get_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    uint32 data_indx;
    uint8  stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2 + 1]; /* the stream consist of value mask table Id */
    SOC_SAND_HASH_TABLE_INFO *hash_tbl;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;    

    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    
    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);
    
    /* recieve hash table */
    hash_tbl = arad_sw_db_entry_key_to_entry_id_hash_get(unit);
                                                                                                                
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table_id), 1);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(unit, hash_tbl, stream, &data_indx, found));

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_get_unsafe(unit, table.db_prefix, data_indx, TRUE /*hit_bit_clear*/,  priority, &entry, &action, found, hit_bit ));

    sal_memcpy((uint32*)payload, action.value, ARAD_TCAM_ACTION_MAX_LEN*sizeof(uint32));    

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)    
STATIC uint32
    arad_pp_dbal_entry_get_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* found)
{
    
    uint32  table_size_in_bytes, table_payload_in_bytes;
    uint8   elk_data[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    /*uint8   elk_mask[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];*/
    uint8   elk_ad_value[SOC_DPP_TCAM_ACTION_ELK_KBP_MAX_LEN_BYTES];
    /*uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];*/
    /*uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];*/
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  prefix_len;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    (*priority) = 0;
                                                                                                                
    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(unit, table_size_in_bytes, entry_key, entry_mask, elk_data, elk_mask));*/
    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(elk_mask, table_size_in_bytes, &prefix_len));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_lpm_route_get(unit, table.db_prefix, prefix_len, elk_data, elk_ad_value, found));

    if (*found) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode(unit, table.db_prefix, elk_ad_value,((ARAD_TCAM_ACTION*)payload)->value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_get_kaps(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* found)
{
    uint32  prefix_len;
    uint8   data_bytes[JER_KAPS_KEY_BUFFER_NOF_BYTES];
    uint8   ad_bytes[JER_KAPS_AD_BUFFER_NOF_BYTES];
    SOC_DPP_DBAL_TABLE_INFO table;

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1/*private_table*/, &kaps_table_id));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));
    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_get_hw(unit, kaps_table_id, prefix_len, data_bytes, ad_bytes, found));

    if (*found) {
        SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_decode(unit, ad_bytes, payload));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - table not initiated %d"), table_id));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_lem(unit, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_tcam(unit, table_id, qual_vals, payload, priority, hit_bit, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_kbp(unit, table_id, qual_vals, payload, priority, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - KBP not inuse")));
#endif         
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_sem(unit, ARAD_PP_ISEM_ACCESS_ID_ISEM_B, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_kaps(unit, table_id, qual_vals, payload, priority, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - ilegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("Entry searched in table %s, found=%d\n"), table.table_name, (*found)));
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
/*the first entry in the qual_vals_array should contain the desired vrf_ndx*/
uint32
    arad_pp_dbal_block_get_kaps(int unit, ARAD_PP_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                           ARAD_PP_FP_QUAL_VAL *qual_vals_array/*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX per entry*/, void* payload,  uint32 *nof_entries)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    uint32 vrf_ndx;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    *nof_entries = 0;

    /*retrieve vrf and reinitialize the qual_val*/
    if (qual_vals_array[0].type != SOC_PPC_FP_QUAL_IRPP_VRF) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get_kaps - missing vrf from qual_vals")));
    }
    vrf_ndx = qual_vals_array[0].val.arr[0];
    SOC_PPC_FP_QUAL_VAL_clear(qual_vals_array);

    SOCDNX_IF_ERR_EXIT(jer_pp_kaps_entry_get_block_hw(unit, table_id, vrf_ndx, &table, block_range_key, qual_vals_array, payload, nof_entries));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/

uint32
    arad_pp_dbal_block_get(int unit, ARAD_PP_IP_ROUTING_TABLE_RANGE *block_range_key, SOC_DPP_DBAL_SW_TABLE_IDS table_id,
                           ARAD_PP_FP_QUAL_VAL *qual_vals_array, void* payload,  uint32 *nof_entries)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - LEM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - TCAM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - KBP not supported")));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - KBP not in use")));
#endif
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - SEM_A not inuse")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - SEM_B not inuse")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_block_get_kaps(unit, block_range_key, table_id, qual_vals_array, payload,  nof_entries));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - KAPS not in use")));
#endif
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_block_get - ilegal physical DB type")));
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_add_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority, SOC_SAND_SUCCESS_FAILURE* success)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    ARAD_TCAM_ENTRY  entry;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    
    ARAD_TCAM_ENTRY_clear(&entry);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));
    
    entry.valid = TRUE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_add_unsafe(unit, table.db_prefix, entry_id, FALSE, priority, &entry, (ARAD_TCAM_ACTION*)payload, success ));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_add_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 payload[ARAD_TCAM_ACTION_MAX_LEN], uint32 priority, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE* success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_tcam_entry_add(unit, table.db_prefix, entry_id, is_for_update,
                                                               priority, entry_key, entry_mask, payload, success));    
exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_add_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, ARAD_TCAM_ACTION* payload, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_id_tcam(unit, table_id, entry_id, qual_vals, payload, priority, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_id_kbp(unit, table_id, entry_id, qual_vals, payload->value, priority, is_for_update,  success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KBP not in use")));
#endif         
        break;
    
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:        
        break;

    default:
     /* SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:*/
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - ilegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry added to table %s \n"), table.table_name));   
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, (table.nof_entries_added_to_table++)));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_delete_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    
    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    
    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit, table.db_prefix, entry_id));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_delete_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_route_remove(unit, table.db_prefix, entry_id));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_delete_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_id_tcam(unit, table_id, entry_id, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_id_kbp(unit, table_id, entry_id, success));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - KBP not in use")));
#endif         
        break;
    
    default:
     /* SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS*/
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - ilegal physical DB type")));
        break;
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.nof_entries_added_to_table.set(unit, table_id, (table.nof_entries_added_to_table--)));
    
#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry deleted from table %s entry_id = %d\n"),table.table_name, entry_id));
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32    
    arad_pp_dbal_entry_get_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    ARAD_TCAM_ENTRY  entry;

    SOC_DPP_DBAL_TABLE_INFO table;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
                                                                                                                    
    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_get_unsafe(unit, table.db_prefix, entry_id, TRUE /*hit_bit_clear*/,  priority, &entry, (ARAD_TCAM_ACTION*)payload, found, hit_bit ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 0, 1, qual_vals, entry.value, entry.mask ));

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_get_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_tcam_entry_get( 
           unit, 
           table.db_prefix, 
           entry_id,
           entry_key,
           entry_mask,
           payload,
           priority,
           found,
           hit_bit
         ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 1, 1, qual_vals, entry_key, entry_mask ));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif /*#if defined(INCLUDE_KBP) && !defined(BCM_88030) */



uint32
    arad_pp_dbal_entry_get_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    SOC_DPP_DBAL_TABLE_INFO table;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if(!table.is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get_id - table not initiated")));
    }

    switch(table.physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_id_tcam(unit, table_id, entry_id, qual_vals, payload, priority, hit_bit, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_id_kbp(unit, table_id, entry_id, qual_vals, payload, priority, hit_bit, found));
#else
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - KBP not in use")));
#endif         
        break;
    
    default:
     /* SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B
        SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS*/
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - ilegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("Entry to get (%d), found %d\n"), entry_id, (*found)));
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_key_to_lem_buffer(int unit, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, ARAD_PP_LEM_ACCESS_KEY *key)
{
    SOC_DPP_DBAL_TABLE_INFO table;
    int i, offset_fix = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    ARAD_PP_LEM_ACCESS_KEY_clear(key);

    key->type = table.additional_table_info;

    key->nof_params = table.nof_qualifiers;

    for (i = 0; i < table.nof_qualifiers; i++) {        
        if(i > 0){
            if(table.qual_info[i].qual_type == table.qual_info[i-1].qual_type) {
                key->param[i-1-offset_fix].nof_bits += table.qual_info[i].qual_nof_bits;
                key->param[i-1-offset_fix].value[1] = qual_vals[i-1-offset_fix].val.arr[1];
                key->nof_params -= 1;
                offset_fix++;
            }else {
                key->param[i-offset_fix].nof_bits = table.qual_info[i].qual_nof_bits;
                key->param[i-offset_fix].value[0] = qual_vals[i-offset_fix].val.arr[0];
            }
        }else {
                key->param[i].nof_bits = table.qual_info[i].qual_nof_bits;
                key->param[i].value[0] = qual_vals[i].val.arr[0];
        }
    }

    key->prefix.nof_bits = table.db_prefix_len;
    key->prefix.value = table.db_prefix;

exit:
    SOCDNX_FUNC_RETURN;
}



#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
  arad_pp_dbal_entry_key_to_kbp_buffer(int unit, SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                       uint32 *prefix_len, uint8 *data_bytes)
{
    uint8
        nof_quals = dbal_table->nof_qualifiers,
        iter_bit_len,
        temp_prefix_len = 0,
        qual_total_bits,
        qual_prefix_len,
        byte_prefix_len;
    uint32
        bit_offset = 0,
        qual_remaining_bits,
        qual_offset,
        data_idx = 0,
        user_qual_idx,
        temp_byte_mask,
        qual_idx, i;
    uint64
        val = {0},
        temp_val = {0},
        mask = {0},
        temp_mask = {0};
    SOC_PPC_FP_QUAL_TYPE    qual_type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(dbal_table);
    SOC_SAND_CHECK_NULL_INPUT(qual_vals);
    SOC_SAND_CHECK_NULL_INPUT(data_bytes);

    sal_memset(data_bytes, 0x0, sizeof(uint8) * table_size_in_bytes);

    /* add table prefix to key buffer */
    if (dbal_table->db_prefix != DBAL_PREFIX_NOT_DEFINED) {
        data_bytes[0] = dbal_table->db_prefix;
        data_bytes[0] <<= (SOC_SAND_NOF_BITS_IN_CHAR - dbal_table->db_prefix_len);
        bit_offset = SOC_SAND_NOF_BITS_IN_CHAR - dbal_table->db_prefix_len;
        *prefix_len = dbal_table->db_prefix_len;
    } else {
        *prefix_len = 0;
    }

    /* add qualifiers to key buffer */
    for (i = 0; i < nof_quals; i++)
    {
        qual_idx = nof_quals - i - 1;
        qual_prefix_len = 0; /*used to check if this is a prefix field*/
        qual_type = dbal_table->qual_info[qual_idx].qual_type;
        qual_total_bits = dbal_table->qual_info[qual_idx].qual_full_size;
        qual_remaining_bits = dbal_table->qual_info[qual_idx].qual_nof_bits;
        /*if the qual_type is not from the packet header its qual_offset is from lsb bit, otherwise its qual_offset is from msb*/
        if (!(dbal_table->qual_info[qual_idx].qual_is_in_hdr)) {
            qual_offset = dbal_table->qual_info[qual_idx].qual_offset;
        } else{
            qual_offset = qual_total_bits - qual_remaining_bits - dbal_table->qual_info[qual_idx].qual_offset;
        }

        if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            /*find user_qual_idx*/
            user_qual_idx = 0;
            while ((qual_type != qual_vals[user_qual_idx].type) && (user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)) {
                user_qual_idx++;
            }

            if (user_qual_idx == SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
                /*qual_val was not found, treat as is_valid = 0*/
                COMPILER_64_SET(val, 0, 0);
                COMPILER_64_SET(mask, 0, 0);
            } else{
                COMPILER_64_SET(val, qual_vals[user_qual_idx].val.arr[1], qual_vals[user_qual_idx].val.arr[0]);
                COMPILER_64_SET(mask, qual_vals[user_qual_idx].is_valid.arr[1], qual_vals[user_qual_idx].is_valid.arr[0]);
            }
        } else if (qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ONES) {
            COMPILER_64_SET(val, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX);
            COMPILER_64_SET(mask, SOC_SAND_U32_MAX, SOC_SAND_U32_MAX);
        }

        /*Copy this qual into the data array*/
        while (qual_remaining_bits > 0)
        {
            /*Either fill the data byte or finish copying the qual*/
            iter_bit_len = bit_offset > qual_remaining_bits ? qual_remaining_bits : bit_offset;

            if (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) {
                temp_val = val;
                temp_mask = mask;

                /*temp_mask takes only the bits about to be copied into data*/
                temp_byte_mask = (((SOC_SAND_U8_MAX >> (SOC_SAND_NOF_BITS_IN_CHAR - iter_bit_len)) &
                                  COMPILER_64_SHR(temp_mask, qual_remaining_bits + qual_offset - iter_bit_len))
                                  << (bit_offset - iter_bit_len)); /*shift left in case the qual ends and the byte isn't filled*/
                byte_prefix_len = soc_sand_nof_on_bits_in_char(temp_byte_mask);

                /*only extend prefix_len for ones/zeros if they're in the middle of the key*/
                if (qual_type == SOC_PPC_FP_QUAL_IRPP_ALL_ONES) {
                    temp_prefix_len += byte_prefix_len;
                }else {
                    *prefix_len += byte_prefix_len + temp_prefix_len;
                    temp_prefix_len = 0;
                }
                qual_prefix_len += byte_prefix_len;

                data_bytes[data_idx] |= ((COMPILER_64_SHR(temp_val, qual_remaining_bits + qual_offset - iter_bit_len)
                                         << (bit_offset - iter_bit_len)) & temp_byte_mask); /*shift left in case the qual ends and the byte isn't filled*/
            } else {
                /*in case of all zeroes: only increment the prefix_len count, no need to copy zeros*/
                temp_prefix_len += iter_bit_len;
                qual_prefix_len += iter_bit_len;
            }

            qual_remaining_bits -= iter_bit_len;
            bit_offset -= iter_bit_len;

            if (bit_offset == 0) {
                data_idx++;
                bit_offset = SOC_SAND_NOF_BITS_IN_CHAR;
            }
        }

        /*Only the last field can be prefix and therefore it is possible to stop*/
        if (qual_prefix_len != dbal_table->qual_info[qual_idx].qual_nof_bits) {
            break;
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in jer_pp_kaps_dbal_key_encode()",0,0);
}
#endif /*#if defined(INCLUDE_KBP) && !defined(BCM_88030)*/

#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
uint32
  arad_pp_dbal_kbp_buffer_to_entry_key(int unit, const SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, uint32 prefix_len, uint8 *data_bytes,
                                       ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    uint8
        nof_quals = dbal_table->nof_qualifiers,
        iter_bit_len,
        qual_total_bits,
        byte_prefix_len;
    uint32
        bit_offset = 0,
        qual_remaining_bits,
        qual_offset,
        data_idx = 0,
        user_qual_idx = 0,
        temp_byte_mask,
        qual_idx, i,
        prefix_remain;
    uint64
        part_qual_val,
        part_qual_mask;

    SOC_PPC_FP_QUAL_TYPE    qual_type;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(dbal_table);
    SOC_SAND_CHECK_NULL_INPUT(qual_vals);
    SOC_SAND_CHECK_NULL_INPUT(data_bytes);

    prefix_remain = prefix_len;

    /* ignore table prefix in the key buffer */
    if (dbal_table->db_prefix != DBAL_PREFIX_NOT_DEFINED) {
        bit_offset = SOC_SAND_NOF_BITS_IN_CHAR - dbal_table->db_prefix_len;
        prefix_remain -= dbal_table->db_prefix_len;
    }

    /* copy qualifiers from key buffer */
    for (i = 0; i < nof_quals; i++)
    {
        qual_idx = nof_quals - i - 1;
        qual_type = dbal_table->qual_info[qual_idx].qual_type;
        qual_total_bits = dbal_table->qual_info[qual_idx].qual_full_size;
        qual_remaining_bits = dbal_table->qual_info[qual_idx].qual_nof_bits;
        /*if the qual_type is not from the packet header its qual_offset is from lsb bit, otherwise its qual_offset is from msb*/
        if (!(dbal_table->qual_info[qual_idx].qual_is_in_hdr)) {
            qual_offset = dbal_table->qual_info[qual_idx].qual_offset;
        } else{
            qual_offset = qual_total_bits - qual_remaining_bits - dbal_table->qual_info[qual_idx].qual_offset;
        }

        if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {
            /*find user_qual_idx*/
            user_qual_idx = 0;
            while ((qual_type != qual_vals[user_qual_idx].type) && (user_qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)) {
                /*if we have reached an empty qual_val, then we should use it*/
                if (qual_vals[user_qual_idx].type == SOC_PPC_NOF_FP_QUAL_TYPES) {
                    SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[user_qual_idx]);
                    qual_vals[user_qual_idx].type = qual_type;
                    break;
                }
                user_qual_idx++;
            }
        }

        /*Copy this qual from the data array*/
        while (qual_remaining_bits > 0 && prefix_remain > 0)
        {
            /*Either finish the data byte or finish copying the qual*/
            iter_bit_len = bit_offset > qual_remaining_bits ? qual_remaining_bits : bit_offset;
            /*It is also possible we have finished copying into the quals, prefix_remain = 0*/
            iter_bit_len = iter_bit_len > prefix_remain ? prefix_remain : iter_bit_len;

            if ((qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES) && (qual_type != SOC_PPC_FP_QUAL_IRPP_ALL_ONES)) {

                temp_byte_mask = (SOC_SAND_U8_MAX >> (SOC_SAND_NOF_BITS_IN_BYTE - iter_bit_len)); /*the size of the mask is based on iter_bit_len*/
                part_qual_mask = COMPILER_64_INIT(0, temp_byte_mask);

                COMPILER_64_SHL(part_qual_mask, qual_remaining_bits + qual_offset - iter_bit_len);

                byte_prefix_len = soc_sand_nof_on_bits_in_char(temp_byte_mask);

                /*copy the byte into a 64bit and then shift it into the correct location, mask it and add it to the qual*/
                part_qual_val = COMPILER_64_INIT(0, (data_bytes[data_idx] >> (bit_offset - iter_bit_len)) & temp_byte_mask);
                COMPILER_64_SHL(part_qual_val, qual_remaining_bits + qual_offset - iter_bit_len);


                qual_vals[user_qual_idx].val.arr[0] +=  COMPILER_64_LO(part_qual_val);
                qual_vals[user_qual_idx].val.arr[1] +=  COMPILER_64_HI(part_qual_val);
                qual_vals[user_qual_idx].is_valid.arr[0] +=  COMPILER_64_LO(part_qual_mask);
                qual_vals[user_qual_idx].is_valid.arr[1] +=  COMPILER_64_HI(part_qual_mask);

                prefix_remain -= byte_prefix_len;
            } else {
                /*in case of all zeroes or all ones: only decrease prefix_remain if in the middle of the key*/
                if (prefix_remain != prefix_len) {
                    prefix_remain -= iter_bit_len;
                }
            }

            qual_remaining_bits -= iter_bit_len;
            bit_offset -= iter_bit_len;

            if (bit_offset == 0) {
                data_idx++;
                bit_offset = SOC_SAND_NOF_BITS_IN_CHAR;
            }
        }

        if (prefix_remain == 0) {
            break;
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_dbal_kbp_buffer_to_entry_key()",0,0);
}
#endif /*defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)*/


#undef DBAL_ENTRY_MANAGMENT_FUNCTIONS
#define DBAL_DIAGNOSTICS_FUNCTIONS 
/****************** DIAGNOSTICS FUNCTIONS ******************/
uint32
    arad_pp_dbal_entry_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    SOC_DPP_DBAL_TABLE_INFO     table;
    uint32                      data[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] = {0};
    uint32                      mask[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] = {0};
    uint8* data_as_uint8;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
    
    for (i = 0; i < table.nof_qualifiers; i++) {
        /* mask can also be printed if needed (but disabled because of last packet diagnostics)
           LOG_CLI((BSL_META("\t Qualifier %s value (0x%x) mask (0x%x)\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(qual_vals[i].type)), qual_vals[i].val.arr[0], qual_vals[i].is_valid.arr[0]));*/
        LOG_CLI((BSL_META("\t Qualifier %s value (0x%x)\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(qual_vals[i].type)), qual_vals[i].val.arr[0]));
    }    

    LOG_CLI((BSL_META("\t DB prefix (%d)"), table.db_prefix));


    switch (table.physical_db_type) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_sem_entry_key_build(unit, table_id, qual_vals, &data[0]));
        data_as_uint8 = (uint8*)(&(data[0]));
        arad_pp_dbal_print_buffer(data_as_uint8, ARAD_PP_ISEM_ACCESS_KEY_SIZE*4, "\n\t Full key (include prefix) as buffer");
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, data, mask));
        data_as_uint8 = (uint8*)(&(data[0]));
        arad_pp_dbal_print_buffer(data_as_uint8, 16, "\n\t Full tcam key as buffer");
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        {
            uint32  table_size_in_bytes, table_payload_in_bytes, prefix_len;
            uint8   elk_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];
            SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table.db_prefix, &table_size_in_bytes, &table_payload_in_bytes));
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, &table, table_size_in_bytes, qual_vals, &prefix_len, elk_key));
            /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table.table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, data, mask)); 
              data_as_uint8 = (uint8*)(&(data[0])); */
            arad_pp_dbal_print_buffer(elk_key, 16, "\n\t Full KBP/KAPS key as buffer");
        }
#endif /*defined(INCLUDE_KBP) && !defined(BCM_88030)*/
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
    default:
        break;
    }

    LOG_CLI((BSL_META("\n")));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_table_info_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id)
{
    const char* str;
    int i,j;
    SOC_DPP_DBAL_TABLE_INFO table;
    uint32 lem_prefix = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    if (!table.is_table_initiated) {
        goto exit;
    }

    /* dump information about the table */
    LOG_CLI((BSL_META("\nTable information for: %s, ID - %d\n\n"), (table.table_name), table_id));
    LOG_CLI((BSL_META("  Physical DB type: %s\n"), (arad_pp_dbal_physical_db_to_string(table.physical_db_type) )));
    if (table.db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        LOG_CLI((BSL_META("  DB prefix not defined\n")));
    }else {
        switch(table.physical_db_type){
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            arad_pp_lem_access_app_to_prefix_get(unit, table.db_prefix, &lem_prefix);
            LOG_CLI((BSL_META("  App type: %s, logical prefix value: 0x%x, prefix length: %d\n"), ARAD_PP_LEM_ACCESS_KEY_TYPE_to_string(table.db_prefix), lem_prefix, (table.db_prefix_len)));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            LOG_CLI((BSL_META("  Table prefix: %d\n"), (table.db_prefix)));
            break;

        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            LOG_CLI((BSL_META("  Table prefix: %d, prefix length: %d\n"), table.db_prefix, (table.db_prefix_len)));
            break;

        default:
            break;
        }    
    }        
    LOG_CLI((BSL_META("  Entries in table: %d\n"), (table.nof_entries_added_to_table)));
    LOG_CLI((BSL_META("  Table qualifiers: \n")));
    for (i = 0; i < table.nof_qualifiers; i++) {
        LOG_CLI((BSL_META("  \tQualifier - %s, offset (%d), size (%d)\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(table.qual_info[i].qual_type)), (table.qual_info[i].qual_offset), (table.qual_info[i].qual_nof_bits)));
    }
    LOG_CLI((BSL_META("  Number of programs associated to this table: %d\n\n"), (table.nof_table_programs)));
    for (i = 0; i < table.nof_table_programs; i++) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, table.table_programs[i].stage, table.table_programs[i].program_id, &str));
        LOG_CLI((BSL_META("  \tProgram: %s, ID %d, stage: %s \n"), str, (table.table_programs[i].program_id), SOC_PPC_FP_DATABASE_STAGE_to_string(table.table_programs[i].stage)));
        if (!SOC_IS_JERICHO(unit) && table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) {
            uint8 key_id = table.table_programs[i].key_id;
            if (key_id == SOC_DPP_DBAL_PROGRAM_KEY_B) {
                key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
            }
            LOG_CLI((BSL_META("  \t\tLookup Key: %s, lookup number %d, allocated bits for key = %d\n"), arad_pp_dbal_key_id_to_string(key_id), (table.table_programs[i].lookup_number), table.table_programs[i].nof_bits_used_in_key));      
        }else{
            LOG_CLI((BSL_META("  \t\tLookup Key: %s, lookup number %d, allocated bits for key = %d\n"), arad_pp_dbal_key_id_to_string(table.table_programs[i].key_id), (table.table_programs[i].lookup_number), table.table_programs[i].nof_bits_used_in_key));
        }
        for (j = 0; j < table.nof_qualifiers; j++) {
            LOG_CLI((BSL_META("  \t\tCE %d assigned for qualifier %s, number of bits = %d\n"), (table.table_programs[i].ce_assigned[j]), (ARAD_PP_FP_QUAL_TYPE_to_string(table.qual_info[j].qual_type)), table.qual_info[j].qual_nof_bits));            
        }
        LOG_CLI((BSL_META("\n")));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_tables_dump(int unit, int is_full_info)
{
    int table_id;
    SOC_DPP_DBAL_TABLE_INFO  table;
    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nExisting dbal tables\n--------------------\n")));
                         
    for (table_id = 0; table_id < SOC_DPP_DBAL_SW_NOF_TABLES; table_id++) {
        if (is_full_info) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_table_info_dump(unit, table_id));
        }else {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
            if (table.is_table_initiated) {
                /* dump information about the table */
                LOG_CLI((BSL_META("\nTable: %s, ID (%d)"), (table.table_name), table_id));
            }
        }    
     }
    LOG_CLI((BSL_META("\n")));
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_ce_per_program_dump(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage)
{

    uint32              ce_rsrc[1], last_ce_ndx = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
    ARAD_PP_KEY_CE_ID   ce_ndx;
    char*               ce_available;
    uint8               ce_size;

    SOCDNX_INIT_FUNC_DEFS;

    if (stage < 0 || stage >= ARAD_NOF_FP_DATABASE_STAGES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid stage %d. \n\rThe range is: 0 - ARAD_NOF_FP_DATABASE_STAGES-1.\n\r"), stage));
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.rsources.ce.get(unit, stage, program_id, 0, &(ce_rsrc[0])));

    LOG_CLI((BSL_META("\nCopy Engines information:\n----------------\n")));
    LOG_CLI((BSL_META("\nprogram_id = %d, stage = %d, ce_rsrc = %u:\n"), program_id, stage, ce_rsrc[0]));

    if(ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) {
        last_ce_ndx = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB + 1;
    }

    for (ce_ndx = 0; ce_ndx <= last_ce_ndx; ce_ndx++) {

        ce_available = "in use";
        ce_size = 16;
        
        /* ce_ndx can't be 134217727 as the coverity claims. It can be only if stage is not between 0 and ARAD_NOF_FP_DATABASE_STAGES - 1.
         * We do such check above 
         */
        /* coverity[overrun-local:FALSE] */
        if ((!SHR_BITGET(ce_rsrc,ce_ndx))) {
            ce_available = "available";
        }
        if ((arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx))){
            ce_size = 32;
        }
        LOG_CLI((BSL_META("CE %d: size (%d) - is %s\n"), ce_ndx, ce_size, ce_available));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




uint32
    arad_pp_dbal_key_dump(int unit, int core_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int lookup_number, ARAD_FP_DATABASE_STAGE  stage)
{
    uint32 ind, qual_val_ndx, res;
    uint32 val[ARAD_PP_DIAG_DBG_VAL_LEN];
    SOC_DPP_DBAL_TABLE_INFO table;
    uint8   is_for_kbp = 0;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 mask_in[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5];/* max signals per key */
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE hit_signal_id = 0, res_signal_id = 0, opcode_signal_id = 0;
    int nof_key_signals = 1;
    int size_of_signal_in_bits;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));

    for(qual_val_ndx = 0; qual_val_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_val_ndx) {
        ARAD_PP_FP_QUAL_VAL_clear(&qual_vals[qual_val_ndx]);
    }

    LOG_CLI((BSL_META("\n   A lookup was made in table: %s, lookup information:\n\n"), table.table_name ));
    
    SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_ids_get(unit, table.physical_db_type, lookup_number, stage, key_signal_id, &res_signal_id, &hit_signal_id, &opcode_signal_id, &nof_key_signals));

    if(table.physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
        is_for_kbp = 1;
    }

    LOG_CLI((BSL_META("    KEY:\n")));

    SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, key_signal_id[0], val, &size_of_signal_in_bits));
    if (nof_key_signals > 1) {
        uint32 segment_val[ARAD_PP_DIAG_DBG_VAL_LEN];
        int last_pos = size_of_signal_in_bits;        
        for (ind = 1; ind < nof_key_signals; ind++) {
            SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, key_signal_id[ind], segment_val, &size_of_signal_in_bits));
            SHR_BITCOPY_RANGE(/*dest*/&val[0],/*pos is dest*/ last_pos, /*src*/segment_val, 0, size_of_signal_in_bits);
            last_pos += size_of_signal_in_bits;
        }        
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, is_for_kbp, 1, qual_vals, val, mask_in));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_dump(unit, table_id, qual_vals));
    
    if (is_for_kbp) {
        LOG_CLI((BSL_META("    OPCODE:\n")));
        SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, opcode_signal_id, val, &size_of_signal_in_bits));
        LOG_CLI((BSL_META("0x%08x "), val[0]));
    }    

	
	LOG_CLI((BSL_META("    RESULT:\n")));
    res = arad_pp_signal_mngr_signal_get(unit, core_id, hit_signal_id, val, &size_of_signal_in_bits);
    if ((val[0] != 0) || (res == -1)) {
        int num_of_int; 
        SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, core_id, res_signal_id, val, &size_of_signal_in_bits));
        num_of_int = size_of_signal_in_bits/32 + ((size_of_signal_in_bits%32)?1:0);
        LOG_CLI((BSL_META("\t Lookup found, Lookup result: ")));
        for (ind = 0; ind < num_of_int; ind++) {
            LOG_CLI((BSL_META("0x%08x "), val[ind])); /* result size for SEM is 32 bit */
        }
        LOG_CLI((BSL_META("\n")));
    } else {
        LOG_CLI((BSL_META("\t Lookup not found any result\n")));
 	}
 
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_isem_prefix_table_dump(int unit)
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_id = 0;
    int prefix = 0, available_prefixes = 0, count;
    int max_prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; /* 6 Jericho, 4 arad  */
    int max_prefix_value = ((1 << max_prefix_len) - 1);    
    SOC_DPP_DBAL_TABLE_INFO  table;
    
    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nSEM A Logical database mapping\n")));
    LOG_CLI((BSL_META("------------------------------\n")));
    LOG_CLI((BSL_META("(for static tables it is possible that prefix is allocated for more than one table)\n")));
    for (prefix = 0; prefix <=  max_prefix_value; prefix++) {        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_a_prefix_mapping.get(unit, prefix, &table_id ));
        if (table_id != 0) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A, prefix, &count));
            if (count > 1) {
                LOG_CLI((BSL_META("Prefix %02d is mapped to multiple tables (%d)\n"), prefix, count));
            }else{
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
                LOG_CLI((BSL_META("Prefix %02d is mapped to table %s (%d)\n"), prefix, table.table_name, table_id));
            }
        } else {
            available_prefixes++;
            /*LOG_CLI((BSL_META("Prefix %02d is available\n"), prefix));*/
        }        
    }
    LOG_CLI((BSL_META("Toatal prefixes %d, available prefixes = %d\n"), max_prefix_value+1, available_prefixes));

    available_prefixes = 0;
    LOG_CLI((BSL_META("\nSEM B Logical database mapping\n")));
    LOG_CLI((BSL_META("------------------------------\n")));
    LOG_CLI((BSL_META("(for static tables it is possible that prefix is allocated for more than one table)\n")));
    for (prefix = 0; prefix <=  max_prefix_value; prefix++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.sem_b_prefix_mapping.get(unit, prefix, &table_id ));
        if (table_id != 0) {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_sem_prefix_usage_get(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B, prefix, &count));
            if (count > 1) {
                LOG_CLI((BSL_META("Prefix %02d is mapped to multiple tables (%d)\n"), prefix, count));
            }else{
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));
                LOG_CLI((BSL_META("Prefix %02d is mapped to table %s (%d)\n"), prefix, table.table_name, table_id));
            }
        } else {
            available_prefixes++;
            /*LOG_CLI((BSL_META("Prefix %02d is available\n"), prefix));*/
        }        
    }    
    LOG_CLI((BSL_META("Toatal prefixes %d, available prefixes = %d\n"), max_prefix_value+1, available_prefixes));   
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_lem_prefix_table_dump(int unit)
{
    uint8 app_id = 0;
    int prefix = 0;
    /*SOC_DPP_DBAL_TABLE_INFO  table;*/
    
    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\nLEM Logical database mapping \n")));
    LOG_CLI((BSL_META("---------------------------- \n")));
    for (prefix = 0; prefix < ARAD_PP_LEM_ACCESS_NOF_PREFIXES; ++prefix) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.fec.lem_prefix_mapping.get(unit, prefix, &app_id));
        if (app_id == ARAD_PP_FLP_MAP_PROG_NOT_SET) {
            LOG_CLI((BSL_META("Prefix %02d is available\n"), prefix));
        } else{
            /*SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, table_id, &table));*/
            LOG_CLI((BSL_META("Prefix %02d is mapped to app %s (%d)\n"), prefix, ARAD_PP_LEM_ACCESS_KEY_TYPE_to_string(app_id), app_id));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
    arad_pp_dbal_last_packet_dump(int unit, int core_id)
{
#define NUM_OF_STAGES  3
    int prog_id[NUM_OF_STAGES], i, stage, j;
    int first_table_id[NUM_OF_STAGES];
    int last_table_id[NUM_OF_STAGES];
    SOC_DPP_DBAL_TABLE_INFO  table;    
    char* stage_names[] = {"VT", "TT", "FLP"};
    const char *str;
    uint8 found;

    ARAD_FP_DATABASE_STAGE  actual_stages[3];
    int stage_vt = 0, stage_tt = 1, stage_flp = 2;

    SOCDNX_INIT_FUNC_DEFS;

    /* init parameters for performing diagnostics */

    actual_stages[stage_vt] = ARAD_FP_DATABASE_STAGE_INGRESS_VT;
    actual_stages[stage_tt] = ARAD_FP_DATABASE_STAGE_INGRESS_TT;
    actual_stages[stage_flp] = ARAD_FP_DATABASE_STAGE_INGRESS_FLP;

    last_table_id[stage_tt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST + 1;
    last_table_id[stage_vt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST + 1;
    last_table_id[stage_flp] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST + 1;

    first_table_id[stage_vt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST +1;
    first_table_id[stage_tt] = SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST +1;
    first_table_id[stage_flp] = SOC_DPP_DBAL_SW_TABLE_ID_INVALID +1;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_print_last_vtt_program_data(unit,core_id, 0, &prog_id[stage_vt], &prog_id[stage_tt]));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_flp_access_print_last_programs_data(unit, core_id, 0, &prog_id[stage_flp]));        

    LOG_CLI((BSL_META("\n Packet information by stage\n")));
    LOG_CLI((BSL_META("\n ---------------------------\n")));
    for (stage = 0; stage < NUM_OF_STAGES; stage++) {

        LOG_CLI((BSL_META("\n *************** Stage - %s******************\n\n"), stage_names[stage]));    
        if (prog_id[stage] == -1) { /* no program invoked continue to next stage */
            LOG_CLI((BSL_META("  No program was invoked for this stage\n")));
            continue;
        } else {
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, actual_stages[stage], prog_id[stage], &str));
            LOG_CLI((BSL_META("  Program %s was invoked for this stage\n"),str));
        }
        found = 0;

        /* finding all tables that related to the program and dumping information */
        for (i = first_table_id[stage]; i < last_table_id[stage]; i++) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, i, &table));

            if (!(table.is_table_initiated)) {
                continue;
            }        
            for (j = 0; j < table.nof_table_programs; j++) {
                if ((table.table_programs[j].program_id == prog_id[stage]) && (table.table_programs[j].stage == actual_stages[stage])) {                    
                    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_dump(unit, core_id, i, table.table_programs[j].lookup_number, actual_stages[stage]));
                    found = 1;
                }
            }
        }
        if (found == 0) {
            LOG_CLI((BSL_META("  No information provided for this program \n\n")));
        }
    } /* for all stages */    
exit:
#undef NUM_OF_STAGES
    SOCDNX_FUNC_RETURN;
}




/*********************************************************************************************************/


int arad_pp_dbal_flp_entry_test(uint32 unit, int table_id, uint8 is_tcam) {

    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	ARAD_PP_LEM_ACCESS_PAYLOAD payload;
    ARAD_PP_LEM_ACCESS_PAYLOAD payload_to_check;
    uint32 value[ARAD_TCAM_ACTION_MAX_LEN] = {0};
    uint8 hit_bit, found;
    uint32 priority;
    
    SOCDNX_INIT_FUNC_DEFS;

	ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload); 
	ARAD_PP_FP_QUAL_VAL_clear(qual_vals);

    payload.flags = ARAD_PP_FWD_DECISION_PARSE_DEST;
	payload.dest = 10;

    value[0] = 0xaf;
    
	qual_vals[0].val.arr[0] = 0xFF;
	qual_vals[0].val.arr[1] = 0;
	qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
	
	qual_vals[1].val.arr[0] = SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM;
	qual_vals[1].val.arr[1] = 0;	
	qual_vals[1].type = SOC_PPC_FP_QUAL_OAM_OPCODE;
	
	qual_vals[2].val.arr[0] = 3;
	qual_vals[2].val.arr[1] = 0;
	qual_vals[2].type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL;	
	
    if (is_tcam) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals ,0, &value, &success));
        value[0] = value[1] = value[2] = 0;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, value, &priority, &hit_bit, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("FLP entry test failed table_id %d, found = %d"), table_id, found));
        }
        if ((value[0] != 0xaf)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VTT entry test failed table_id %d, found = %d, value not valid"), table_id, found));
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
        LOG_CLI((BSL_META("FLP: entry test (add-get-delete) to table_id %d (TCAM), verified \n"), table_id));
    }else{
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals ,0, &payload, &success));
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, &payload_to_check, 0/*priority*/, NULL/*hit_bit*/, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("FLP entry test failed table_id %d, found = %d"), table_id, found));
        }
        if ((payload.dest != payload_to_check.dest)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VTT entry test failed table_id %d, found = %d, value not valid"), table_id, found));
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
        LOG_CLI((BSL_META("FLP: entry test (add-get-delete) to table_id %d (LEM), verified\n"), table_id));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_pp_dbal_vtt_entry_test(uint32 unit, int table_id, uint8 is_tcam) {

    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_SAND_SUCCESS_FAILURE success;
	ARAD_PP_ISEM_ACCESS_ENTRY sem_entry;
    ARAD_PP_ISEM_ACCESS_ENTRY sem_entry_to_check;
    uint32 value[ARAD_TCAM_ACTION_MAX_LEN] = {0};
    uint8 hit_bit, found;
    uint32 priority;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_os_memset(&sem_entry, 0x0, sizeof(ARAD_PP_ISEM_ACCESS_ENTRY));
	ARAD_PP_FP_QUAL_VAL_clear(qual_vals);
	
	sem_entry.sem_result_ndx = 0xff;

    value[0] = 0xaf;
    
	qual_vals[0].val.arr[0] = 0xFF;
	qual_vals[0].val.arr[1] = 0;
	qual_vals[0].type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
	
	qual_vals[1].val.arr[0] = 0;
	qual_vals[1].val.arr[1] = 0;	
	qual_vals[1].type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
	
	qual_vals[2].val.arr[0] = 0xa;
	qual_vals[2].val.arr[1] = 0;
	qual_vals[2].type = SOC_PPC_FP_QUAL_INITIAL_VID;	
	
    if (is_tcam) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals ,0, &value, &success));
        value[0] = value[1] = value[2] = 0;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, value, &priority, &hit_bit, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VTT entry test failed table_id %d, found = %d"), table_id, found));
        }
        if ((value[0] != 0xaf)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VTT entry test failed table_id %d, found = %d, value not valid"), table_id, found));
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));
        LOG_CLI((BSL_META("VTT: entry added to table_id %d (TCAM), found = %d value verified \n"), table_id, found));
    }else{
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_add(unit, table_id, qual_vals ,0, &sem_entry, &success));
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get(unit, table_id, qual_vals, &sem_entry_to_check, 0/*priority*/, NULL, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VTT entry test failed table_id %d, found = %d"), table_id, found));
        }
        if ((sem_entry_to_check.sem_result_ndx != 0xff)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("VTT entry test failed table_id %d, found = %d, value not valid"), table_id, found));
        }
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete(unit, table_id, qual_vals, &success));        
        LOG_CLI((BSL_META("VTT: entry test (add-get-delete) to table_id %d (SEM), verified\n"), table_id));
    }        

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
   arad_pp_dbal_flp_dynamic_tables_init(int unit)
{
	int prog_id = 22;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_DPP_DBAL_SW_TABLE_IDS lem_table_id, tcam_table_id;	

    SOCDNX_INIT_FUNC_DEFS;

    /* creating the table that related to the program */
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_OPCODE;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL;


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_dynamic_table_create(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, qual_info, "LEM_FLP_DYNAMIC", &lem_table_id));
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_dynamic_table_create(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 3, qual_info, "TCAM_FLP_DYNAMIC", &tcam_table_id));

    LOG_CLI((BSL_META("FLP: LEM and TCAM tables created \n")));

    /* associating the tables to the program */
    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = lem_table_id;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));
	
	keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.lookup_number = 1;
    keys_to_table_id.sw_table_id = tcam_table_id;
		
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_FLP, &keys_to_table_id, NULL, 1));

    LOG_CLI((BSL_META("FLP: tables associated to program = %d\n"), prog_id));

	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_entry_test(unit, lem_table_id, FALSE));
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_entry_test(unit, tcam_table_id, TRUE));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_destroy(unit, lem_table_id));
    LOG_CLI((BSL_META("FLP: LEM table %d destroyed successfully\n"), lem_table_id));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_destroy(unit, tcam_table_id));
    LOG_CLI((BSL_META("FLP: TCAM table %d destroyed successfully\n"), tcam_table_id));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
   arad_pp_dbal_vtt_dynamic_tables_init(int unit)
{
	int prog_id = 10;
    SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id = {0};
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
	SOC_DPP_DBAL_SW_TABLE_IDS sem_table_id, tcam_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    /* creating the table that related to the program */
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
	qual_info[1].qual_nof_bits = 2;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;


    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_dynamic_table_create(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A, 3, qual_info, "SEM_VT_DYNAMIC", &sem_table_id));
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_dynamic_table_create(unit, SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 3, qual_info, "TCAM_VT_DYNAMIC", &tcam_table_id));

    LOG_CLI((BSL_META("VTT: SEM and TCAM tables created \n")));

    /* associating the tables to the program */
    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_A;
    keys_to_table_id.sw_table_id = sem_table_id;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_VT, &keys_to_table_id, NULL, 1));    
	
    prog_id++;

	/* associating the tables to the program */
    keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id.sw_table_id = tcam_table_id;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_VT, &keys_to_table_id, NULL, 1));    
	
	keys_to_table_id.key_id = SOC_DPP_DBAL_PROGRAM_KEY_C;
    keys_to_table_id.sw_table_id = tcam_table_id;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_program_to_tables_associate(unit, prog_id, ARAD_FP_DATABASE_STAGE_INGRESS_TT, &keys_to_table_id, NULL, 1));

    LOG_CLI((BSL_META("VT: tables associated to program = %d, %d\n"), prog_id-1, prog_id));
    LOG_CLI((BSL_META("TT: tcam table associated to program = %d \n"), prog_id));

	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vtt_entry_test(unit, sem_table_id, FALSE));
	SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vtt_entry_test(unit, tcam_table_id, TRUE));

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_destroy(unit, sem_table_id));
    LOG_CLI((BSL_META("VTT: SEM table %d destroyed successfully\n"), sem_table_id));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_destroy(unit, tcam_table_id));
    LOG_CLI((BSL_META("VTT: TCAM table %d destroyed successfully\n"), tcam_table_id));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_dbal_dynamic_table_test(int unit)
{

  SOCDNX_INIT_FUNC_DEFS;

  LOG_CLI((BSL_META("\nDBAL dynamic table allocation test is not valid ")));

  goto exit;
  
  LOG_CLI((BSL_META("\nDBAL dynamic table allocation test \n----------------------------------\n")));
  
  SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_dynamic_tables_init(unit));
  
  SOCDNX_IF_ERR_EXIT(arad_pp_dbal_vtt_dynamic_tables_init(unit));

  LOG_CLI((BSL_META("************ DBAL dynamic table allocation test finished successfully ************\n")));

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************************************************/



const char*
    arad_pp_dbal_physical_db_to_string(SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type)
{
    const char* str = NULL;

    switch(physical_db_type)
    {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            str = "LEM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            str = "TCAM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            str = "KBP";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
            str = "SEM A";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
            str = "SEM B";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:
            str = "KAPS";
            break;
        default:
        str = " Unknown physical DB";
    }
    return str;
}

/* The program ID is the line in the selection, need to transform it to program ID */
STATIC int
    arad_pp_dbal_program_to_string(int unit, ARAD_FP_DATABASE_STAGE stage, int cam_line, const char**str)
{
    uint8 prog_id;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    switch (stage) {
    case ARAD_FP_DATABASE_STAGE_INGRESS_FLP:
        rv = sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, cam_line,&prog_id);
        SOCDNX_IF_ERR_EXIT(rv);
        *str = arad_pp_flp_prog_id_to_prog_name(unit,prog_id);
        break;
    case ARAD_FP_DATABASE_STAGE_INGRESS_VT:
        arad_pp_isem_access_program_sel_line_to_program_id(unit, cam_line, 1, &prog_id);
        *str = arad_pp_isem_access_print_vt_program_data(unit, prog_id, 0);
        break;
    case ARAD_FP_DATABASE_STAGE_INGRESS_TT:
        arad_pp_isem_access_program_sel_line_to_program_id(unit, cam_line, 0, &prog_id);
        *str =  arad_pp_isem_access_print_tt_program_data(unit, prog_id, 0);
        break;
    default:
        *str = "stage unknown";
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

const char*
    arad_pp_dbal_key_id_to_string(uint8 key_id)
{
    switch (key_id) {
    case SOC_DPP_DBAL_PROGRAM_KEY_A:
        return "key A";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_B:
        return "key B";        
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_C:
        return "key C";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_D:
        return "key D";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_A_MSB:
        return "key A MSB";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_B_MSB:
        return "key B MSB";        
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_C_MSB:
        return "key C MSB";
        break;

    case SOC_DPP_DBAL_PROGRAM_KEY_D_MSB:
        return "key D MSB";
        break;

    default:
        return "unknown key";
        break;
    }
}

STATIC void
    arad_pp_dbal_print_buffer(uint8* stream, uint32 steam_size, char* msg) 
{
    int i;
    LOG_CLI((BSL_META("\n %s: "), msg));
    for (i = 0; i < steam_size; i++) {
        if (i == steam_size-1) {
            LOG_CLI((BSL_META("%02x"), (*(i+stream))));
        }else {
            LOG_CLI((BSL_META("%02x:"), (*(i+stream))));
        }
    }
    LOG_CLI((BSL_META("\n")));
}

#undef DBAL_DIAGNOSTICS_FUNCTIONS



/********************************* SIGNAL MANAGER *********************************/  

STATIC SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO arad_pp_signal_mngr_arad_signals_table[SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES] = {                           
    /*  TYPE             | is_valid |  block  |addr_high|addr_low| msb | lsb | num_of_bits| offset*/
    {/*FLP_2_LEM1_KEY     */ TRUE,  ARAD_IHB_ID,  22,       0,     234,  161,      74,        0}, 
    {/*FLP_2_LEM2_KEY     */ TRUE,  ARAD_IHB_ID,  22,       0,     116,   43,      74,        0},
    {/*LEM1_2_FLP_RES     */ TRUE,  ARAD_IHB_ID,  21,       0,      88,   46,      43,        0},
    {/*LEM2_2_FLP_RES     */ TRUE,  ARAD_IHB_ID,  21,       0,      42,    0,      43,        0},
    {/*LEM1_2_FLP_HIT     */ TRUE,  ARAD_IHB_ID,  21,       0,      91,   91,       1,        0},
    {/*LEM2_2_FLP_HIT     */ TRUE,  ARAD_IHB_ID,  21,       0,      45,   45,       1,        0},
    {/*FLP_2_LPM1_KEY     */ TRUE,  ARAD_IHB_ID,  13,       0,      43,    0,      43,        0},
    {/*FLP_2_LPM1_KEY_2   */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    {/*FLP_2_LPM2_KEY     */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    {/*FLP_2_LPM2_KEY_2   */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    {/*LPM1_2_FLP_RES     */ TRUE,  ARAD_IHB_ID,  14,       0,      15,    1,      15,        0},
    {/*LPM2_2_FLP_RES     */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    {/*LPM1_2_FLP_HIT     */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    {/*LPM2_2_FLP_HIT     */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
    {/*FLP_2_TCAM_KEY     */ TRUE,  ARAD_IHP_ID,  15,       1,     235,   76,     160,        0},
    {/*TCAM_2_FLP_RES     */ TRUE,  ARAD_IHP_ID,  16,       0,     124,   85,      40,        0},
    {/*TCAM_2_FLP_HIT     */ TRUE,  ARAD_IHP_ID,  16,       0,     125,  125,       1,        0},
    {/*FLP_2_KBP_OPCODE   */ TRUE,  ARAD_IHB_ID,   3,       0,       7,    0,       8,        0},
    {/*FLP_2_KBP_KEY_PART1*/ TRUE,  ARAD_IHB_ID,   3,       0,     255,    8,     248,        0},
    {/*FLP_2_KBP_KEY_PART2*/ TRUE,  ARAD_IHB_ID,   3,       1,     255,    0,     248,        0},
    {/*FLP_2_KBP_KEY_PART3*/ TRUE,  ARAD_IHB_ID,   3,       2,     255,    0,     248,        0},
    {/*FLP_2_KBP_KEY_PART4*/ TRUE,  ARAD_IHB_ID,   3,       3,     255,    0,     248,        0},
    {/*FLP_2_KBP_KEY_PART5*/ TRUE,  ARAD_IHB_ID,   3,       4,       7,    0,     248,        0},
    {/*KBP_2_FLP_RES      */ TRUE,  ARAD_IHB_ID,   4,       0,     128,    1,     128,        0},
    {/*KBP_2_FLP_HIT      */ TRUE,  ARAD_IHB_ID,   4,       0,     128,    1,       8,        0},
    {/*VT_2_SEM_A_KEY     */ TRUE,  ARAD_IHP_ID,  13,       0,      81,   41,      41,        0},
    {/*VT_2_SEM_A_RES     */ TRUE,  ARAD_IHP_ID,  14,       0,      33,   18,      16,        0},
    {/*VT_2_SEM_A_HIT     */ TRUE,  ARAD_IHP_ID,  14,       0,      35,   35,       1,        0},
    {/*VT_2_SEM_B_KEY     */ TRUE,  ARAD_IHP_ID,  15,       0,      81,   41,      41,        0},
    {/*VT_2_SEM_B_RES     */ TRUE,  ARAD_IHP_ID,  16,       0,      33,   18,      16,        0},
    {/*VT_2_SEM_B_HIT     */ TRUE,  ARAD_IHP_ID,  16,       0,      35,   35,       1,        0},
    {/*VT_2_TCAM_KEY_LSB  */ TRUE,  ARAD_IHP_ID,  11,       0,     255,  166,      90,        0},
    {/*VT_2_TCAM_KEY_MSB  */ TRUE,  ARAD_IHP_ID,  11,       1,      69,    0,      70,        0},
    {/*VT_2_TCAM_RES      */ TRUE,  ARAD_IHP_ID,  12,       0,      82,   43,      40,        0},
    {/*VT_2_TCAM_HIT      */ TRUE,  ARAD_IHP_ID,  12,       0,      83,   83,       1,        0},
    {/*TT_2_SEM_A_KEY     */ TRUE,  ARAD_IHP_ID,  13,       0,      40,    0,      41,        0},
    {/*TT_2_SEM_A_RES     */ TRUE,  ARAD_IHP_ID,  14,       0,      15,    0,      16,        0},
    {/*TT_2_SEM_A_HIT     */ TRUE,  ARAD_IHP_ID,  14,       0,      17,   17,       1,        0},
    {/*TT_2_SEM_B_KEY     */ TRUE,  ARAD_IHP_ID,  15,       0,      40,    0,      41,        0},
    {/*TT_2_SEM_B_RES     */ TRUE,  ARAD_IHP_ID,  16,       0,      15,    0,      16,        0},
    {/*TT_2_SEM_B_HIT     */ TRUE,  ARAD_IHP_ID,  16,       0,      17,   17,       1,        0},
    {/*TT_2_TCAM_KEY      */ TRUE,  ARAD_IHP_ID,  11,       0,     165,    6,     160,        0},
    {/*TT_2_TCAM_RES      */ TRUE,  ARAD_IHP_ID,  12,       0,      40,    1,      40,        0},
    {/*TT_2_TCAM_HIT      */ TRUE,  ARAD_IHP_ID,  12,       0,      41,   41,       1,        0},   
    {/*RIF                */ TRUE,  ARAD_IHP_ID,   4,       1,     225,  214,      11,        0},
    {/*RIF_PRofile        */ FALSE, ARAD_IHP_ID,   0,       0,       0,    0,       0,        0},
};


STATIC SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO arad_pp_signal_mngr_jericho_signals_table[SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_NOF_TYPES] = {                           
    /*  TYPE             | is_valid |  block  |addr_high|addr_low| msb | lsb | num_of_bits| offset*/
    {/*FLP_2_LEM1_KEY     */ TRUE,  ARAD_IHP_ID,  26,       0,     205,  126,      80,        0},
    {/*FLP_2_LEM2_KEY     */ TRUE,  ARAD_IHP_ID,  26,       0,      79,    0,      80,        0},
    {/*LEM1_2_FLP_RES     */ TRUE,  ARAD_IHP_ID,  27,       0,      44,    0,      44,        0},
    {/*LEM2_2_FLP_RES     */ TRUE,  ARAD_IHP_ID,  27,       0,      92,   48,      45,        0},
    {/*LEM1_2_FLP_HIT     */ TRUE,  ARAD_IHP_ID,  27,       0,      95,   95,       1,        0},
    {/*LEM2_2_FLP_HIT     */ TRUE,  ARAD_IHP_ID,  27,       0,      47,   47,       1,        0},
    {/*FLP_2_LPM1_KEY     */ TRUE,  ARAD_IHP_ID,  30,       1,     255,  252,       4,        0},
    {/*FLP_2_LPM1_KEY_2   */ TRUE,  ARAD_IHP_ID,  30,       2,     155,    0,     156,        0},
    {/*FLP_2_LPM2_KEY     */ TRUE,  ARAD_IHP_ID,  30,       0,      77,    0,     160,       82},
    {/*FLP_2_LPM2_KEY_2   */ TRUE,  ARAD_IHP_ID,  30,       1,     255,  174,     160,       78},
    {/*LPM1_2_FLP_RES     */ TRUE,  ARAD_IHP_ID,  31,       0,     119,  100,      20,        0},
    {/*LPM2_2_FLP_RES     */ TRUE,  ARAD_IHP_ID,  31,       0,      59,   40,      20,        0},
    {/*LPM1_2_FLP_HIT     */ TRUE,  ARAD_IHP_ID,  31,       0,       9,    8,       2,        0},
    {/*LPM2_2_FLP_HIT     */ TRUE,  ARAD_IHP_ID,  31,       0,      99,  100,       2,        0},
    {/*FLP_2_TCAM_KEY     */ TRUE,  ARAD_IHP_ID,  28,       0,     255,  166,     160,        0},
    {/*TCAM_2_FLP_RES     */ TRUE,  ARAD_IHP_ID,  29,       0,      98,   51,      48,        0},
    {/*TCAM_2_FLP_HIT     */ TRUE,  ARAD_IHP_ID,  29,       0,      99,   99,       1,        0},
    {/*FLP_2_KBP_OPCODE   */ TRUE,  ARAD_IHP_ID,  32,       0,       7,    0,       8,        0},
    {/*FLP_2_KBP_KEY_PART1*/ TRUE,  ARAD_IHP_ID,  32,       0,     255,    8,     248,        0},
    {/*FLP_2_KBP_KEY_PART2*/ TRUE,  ARAD_IHP_ID,  32,       1,     255,    0,     248,        0},
    {/*FLP_2_KBP_KEY_PART3*/ TRUE,  ARAD_IHP_ID,  32,       2,     255,    0,     248,        0},
    {/*FLP_2_KBP_KEY_PART4*/ TRUE,  ARAD_IHP_ID,  32,       3,     255,    0,     248,        0},
    {/*FLP_2_KBP_KEY_PART5*/ TRUE,  ARAD_IHP_ID,  32,       4,       7,    0,     248,        0},
    {/*KBP_2_FLP_RES      */ TRUE,  ARAD_IHP_ID,  33,       0,     128,    1,     128,        0},
    {/*KBP_2_FLP_HIT      */ TRUE,  ARAD_IHP_ID,  33,       0,     128,    1,       8,        0},
    {/*VT_2_SEM_A_KEY     */ TRUE,  ARAD_IHP_ID,  20,       0,      99,   50,      50,        0},
    {/*VT_2_SEM_A_RES     */ TRUE,  ARAD_IHP_ID,  21,       0,      35,   19,      17,        0},
    {/*VT_2_SEM_A_HIT     */ TRUE,  ARAD_IHP_ID,  21,       0,      37,   37,       1,        0},
    {/*VT_2_SEM_B_KEY     */ TRUE,  ARAD_IHP_ID,  22,       0,      99,   50,      50,        0},
    {/*VT_2_SEM_B_RES     */ TRUE,  ARAD_IHP_ID,  23,       0,      35,   19,      17,        0},
    {/*VT_2_SEM_B_HIT     */ TRUE,  ARAD_IHP_ID,  23,       0,      37,   37,       1,        0},
    {/*VT_2_TCAM_KEY_LSB  */ TRUE,  ARAD_IHP_ID,  24,       0,     255,  172,      90,        0},
    {/*VT_2_TCAM_KEY_MSB  */ TRUE,  ARAD_IHP_ID,  24,       1,      75,    0,      70,        0},
    {/*VT_2_TCAM_RES      */ TRUE,  ARAD_IHP_ID,  25,       0,      98,   51,      48,        0},
    {/*VT_2_TCAM_HIT      */ TRUE,  ARAD_IHP_ID,  25,       0,      99,   99,       1,        0},
    {/*TT_2_SEM_A_KEY     */ TRUE,  ARAD_IHP_ID,  20,       0,      49,    0,      50,        0},
    {/*TT_2_SEM_A_RES     */ TRUE,  ARAD_IHP_ID,  21,       0,      16,    0,      17,        0},
    {/*TT_2_SEM_A_HIT     */ TRUE,  ARAD_IHP_ID,  21,       0,      18,   18,       1,        0},
    {/*TT_2_SEM_B_KEY     */ TRUE,  ARAD_IHP_ID,  22,       0,      49,    0,      50,        0},
    {/*TT_2_SEM_B_RES     */ TRUE,  ARAD_IHP_ID,  23,       0,      16,    0,      17,        0},
    {/*TT_2_SEM_B_HIT     */ TRUE,  ARAD_IHP_ID,  23,       0,      18,   18,       1,        0},
    {/*TT_2_TCAM_KEY      */ TRUE,  ARAD_IHP_ID,  24,       0,     165,    6,     160,        0},
    {/*TT_2_TCAM_RES      */ TRUE,  ARAD_IHP_ID,  25,       0,      48,    1,      48,        0},
    {/*TT_2_TCAM_HIT      */ TRUE,  ARAD_IHP_ID,  25,       0,      49,   49,       1,        0},
    {/*RIF                */ TRUE,  ARAD_IHP_ID,   3,       1,     215,  201,      15,        0},
    {/*RIF_PRofile        */ TRUE,  ARAD_IHP_ID,   3,       1,     200,  195,       6,        0},
};

void
    arad_pp_signal_mngr_signal_info_get(int unit, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO* sig_info)
{        
    if(SOC_IS_JERICHO(unit)) {
        sal_memcpy(sig_info, &arad_pp_signal_mngr_jericho_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));
    } else if(SOC_IS_ARADPLUS(unit)){ 
        /* at the moment same values for arad and arad+, if there is a signal that it is different from arad and arad+ need to use different structures */
        sal_memcpy(sig_info, &arad_pp_signal_mngr_arad_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));        
    } else{
        sal_memcpy(sig_info, &arad_pp_signal_mngr_arad_signals_table[signal_id], sizeof(SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO));        
    }
}
		/************************************************* SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS *************************************************/
uint32
    arad_pp_signal_mngr_signal_get(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, uint32 val[24], int* size_of_signal_in_bits)
{
    uint32 val_aligned[24] = {0}; /* used only if the value is not aligned*/
    SOC_DPP_SIGNAL_MNGR_SIGNAL_INFO sig_info;
    ARAD_PP_DIAG_REG_FIELD prm_fld;
    int num_of_bytes = 0, ind;

    SOCDNX_INIT_FUNC_DEFS;
    
    arad_pp_signal_mngr_signal_info_get(unit, signal_id, &sig_info);
    if(sig_info.is_valid) {
        (*size_of_signal_in_bits) = sig_info.signal_length_in_bits;
        prm_fld.base = (sig_info.addr_high << 16) + sig_info.addr_low;
        prm_fld.lsb = sig_info.lsb;
        prm_fld.msb = sig_info.msb;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_diag_dbg_val_get_unsafe(unit, core_id, (sig_info.prm_blk), &prm_fld, val));
        
        num_of_bytes = sig_info.signal_length_in_bits/8 + 1;

        if(sig_info.signal_offset > 0) {
            for(ind = 0; ind < sig_info.signal_offset + num_of_bytes; ++ind) {
                val_aligned[ind] = val[ind + sig_info.signal_offset];
		}

            sal_memcpy(val, val_aligned, sizeof(uint32) * 24);
        }
    } else {        
        return -1;
    }

exit:
    SOCDNX_FUNC_RETURN;    
}


uint32
    arad_pp_signal_mngr_signal_print(int unit, int core_id, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id)
{
    int num_of_int = 0;
    int ind, size_of_signal_in_bits;
    uint32 val[ARAD_PP_DIAG_DBG_VAL_LEN];
    int res = 0;        

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_signal_mngr_signal_get(unit, core_id, signal_id, val, &size_of_signal_in_bits);
    
    num_of_int = (size_of_signal_in_bits/32) + 1;

    /* prints the signal */

    if (res == 0) {
        for(ind = 0; ind < num_of_int; ++ind) {
            LOG_CLI((BSL_META("\t buff[%u]: 0x%08x\n\r"), ind, val[ind]));
        }    
    } else {
        LOG_CLI((BSL_META("\t SIGNAL NOT VALID \n\r")));
    }

    SOCDNX_FUNC_RETURN;
}


/* find the related signals according to the physical DB and the stage*/
uint32
    arad_pp_signal_mngr_signal_ids_get(
       int unit, 
       SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type, 
       int lookup_number, 
       ARAD_FP_DATABASE_STAGE  stage,
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5],
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* res_signal_id,
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* hit_signal_id,
       SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE* opcode_signal_id, 
       int* nof_key_signals)
{

    SOCDNX_INIT_FUNC_DEFS;

    (*nof_key_signals) = 1;

    switch (physical_db_type) {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        if(lookup_number == 2) {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM2_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM2_2_FLP_RES;
            
        } else {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LEM1_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LEM1_2_FLP_RES;            
        }
        break;
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:    
        if(lookup_number == 2) {
            if(SOC_IS_JERICHO(unit)) {
                (*nof_key_signals) = 2;                
                key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY_2;                   
            } else {
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM2_KEY;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM2_2_FLP_RES;
            }
            
        } else {
            if(SOC_IS_JERICHO(unit)) {
                (*nof_key_signals) = 2;
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY;
                key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_LPM1_KEY_2;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_LPM1_2_FLP_RES;   
            }
        }                 
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_FLP) {            
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_TCAM_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TCAM_2_FLP_RES;
        } else {
            
            if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT) {
                (*nof_key_signals) = 2;
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_LSB;
                key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_KEY_MSB;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_TCAM_RES;
            } else {/* TT stage */
                key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_KEY;
                (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_HIT;
                (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_TCAM_RES;
            }
        }
            break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
        key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART1;
        key_signal_id[1] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART2;
        key_signal_id[2] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART3;
        key_signal_id[3] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART4;
        key_signal_id[4] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_KEY_PART5;
        (*opcode_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_FLP_2_KBP_OPCODE;
        (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_HIT;
        (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_KBP_2_FLP_RES;
                
        (*nof_key_signals) = 5;
            break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A:
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT) {            
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_A_RES;                      
        } else { /* TT */            
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_A_RES;   
        }
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:
        if (stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT) {
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_VT_2_SEM_B_RES;           
            
        } else { /* TT */
            key_signal_id[0] = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_KEY;
            (*hit_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_HIT;
            (*res_signal_id) = SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE_TT_2_SEM_B_RES;            
        }
            break;
                
    default:
        goto exit;
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

