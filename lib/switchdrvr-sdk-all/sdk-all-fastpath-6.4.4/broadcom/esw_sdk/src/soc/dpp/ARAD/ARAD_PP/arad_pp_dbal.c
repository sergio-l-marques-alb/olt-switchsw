/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

#include <shared/bsl.h>
#include <shared/swstate/sw_state_access.h>
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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include <shared/l3.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <shared/swstate/sw_state_access.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#endif
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif


/*#define DBAL_PRINTS_ENABLED*/
/*#define DBAL_ADVANCED_PRINTS_ENABLED*/

#define DBAL_PREFIX_NOT_DEFINED             (-1)
#define DBAL_MAX_QUALIFIER_LENGTH           32
#define DBAL_KEY_SIZE_IN_BITS               80 
#define DBAL_KEY_C_ARAD_SIZE_IN_BITS        160 

typedef enum
{
    SOC_DPP_DBAL_CE_SEARCH_MODE_USE_FIRST_16_BIT_FIRST,
    SOC_DPP_DBAL_CE_SEARCH_MODE_FIRST_FREE
}SOC_DPP_DBAL_CE_SEARCH_MODE;


#define DBAL_TABLE_PARAMS_CLEAR(table_info)             sal_memset(table_info, 0, sizeof(SOC_DPP_DBAL_TABLE_INFO))

#define DBAL_TABLE_INFO_UPDATE(table_info, db_prefix,db_prefix_len, physical_db, nof_qualifiers, additional_info, qual_info, table_name)\
                                table_info.db_prefix        = db_prefix;\
                                table_info.db_prefix_len    = db_prefix_len;\
                                table_info.physical_db_type = physical_db;\
                                table_info.nof_qualifiers   = nof_qualifiers;\
                                table_info.additional_table_info = additional_info;\
                                sal_memcpy(&(table_info.qual_info[0]), &qual_info[0], sizeof(SOC_DPP_DBAL_QUAL_INFO)*SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);\
                                sal_strncpy(&(table_info.table_name[0]), table_name, sal_strlen(table_name));


/* global parameter that holds all tables information */
STATIC SOC_DPP_DBAL_TABLE_INFO      dbal_tables[BCM_MAX_NUM_UNITS][SOC_DPP_DBAL_SW_NOF_TABLES];


/********* FUNCTION DECLARTIONS *********/

STATIC uint32 arad_pp_dbal_next_available_ce_get(int unit, int qual_nof_bits, uint8 use_32_ce, uint32 last_unchecked_ce, ARAD_FP_DATABASE_STAGE stage, uint32 ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE mode, int is_msb, ARAD_PP_KEY_CE_ID* free_ce);
STATIC uint32 arad_pp_dbal_qualifier_full_size_get(int unit, ARAD_FP_DATABASE_STAGE stage, SOC_PPC_FP_QUAL_TYPE qual_type, uint8* qual_full_size, uint8* qual_is_in_hdr);

STATIC uint32 arad_pp_dbal_entry_key_to_lem_buffer(int unit, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, ARAD_PP_LEM_ACCESS_KEY *key);
STATIC uint32 arad_pp_dbal_entry_key_to_lpm_buffer(int unit, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, uint32* prefix, uint32* vrf, uint32* prefix_len);
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32 arad_pp_dbal_entry_key_to_kbp_buffer(int unit, SOC_DPP_DBAL_TABLE_INFO *dbal_table, uint32 table_size_in_bytes, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                   uint32 *prefix_len, uint8 *data_bytes);
#endif
STATIC int arad_pp_dbal_program_to_string(int unit, ARAD_FP_DATABASE_STAGE stage, int cam_line, const char**);
STATIC void   arad_pp_dbal_print_buffer(uint8* stream, uint32 steam_size, char* msg);
uint32 arad_pp_dbal_flp_tables_create(int unit);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32 arad_pp_dbal_flp_kaps_tables_create(int unit);
STATIC uint32 arad_pp_dbal_flp_kbp_tables_create(int unit);
#endif

STATIC uint32 arad_pp_dbal_flp_oam_tables_create(int unit);


/********* GENERAL DBAL FUNCTION *********/
uint32
    arad_pp_dbal_init(int unit)
{            
    SOCDNX_INIT_FUNC_DEFS;
    
    sal_memset(&(dbal_tables[unit]), 0, sizeof(SOC_DPP_DBAL_TABLE_INFO) * SOC_DPP_DBAL_SW_NOF_TABLES);

    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_tables_create(unit));    
        
#ifdef DBAL_PRINTS_ENABLED
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_tables_dump(unit, 0));    
#endif

exit:
    SOCDNX_FUNC_RETURN;
}

/* only sets to ZERO all global parameters */
uint32
    arad_pp_dbal_deinit(int unit)
{    
    

    return 0;
}

uint32 
    arad_pp_dbal_db_predfix_get(int unit, uint32 table_id, uint32* db_prefix)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(table->is_table_initiated == 0){         
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_db_predfix_get - table nit initiated table_id %d"), table_id));
    }

    (*db_prefix) = table->db_prefix;

exit:    
    SOCDNX_FUNC_RETURN;
}

uint32 
    arad_pp_dbal_ce_info_get(int unit, uint32 table_id, ARAD_FP_DATABASE_STAGE stage, ARAD_SW_DB_PMF_CE *ce_array, uint8 *nof_ces, uint8 *is_key_320b, uint8 *ces_ids)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    (*nof_ces) = table->nof_qualifiers;
        (*is_key_320b) = 0;

        for (i = 0; i < table->nof_qualifiers; i++) {
            ces_ids[i] = i;
            ce_array[table->nof_qualifiers - 1 - i].is_used = 1;
            ce_array[table->nof_qualifiers - 1 - i].qual_type = table->qual_info[i].qual_type;
            ce_array[table->nof_qualifiers - 1 - i].msb       = table->qual_info[i].qual_nof_bits-1;
            ce_array[table->nof_qualifiers - 1 - i].qual_lsb  = table->qual_info[i].qual_offset;
        }

    SOCDNX_FUNC_RETURN;
}



/********* TABLE FUNCTIONS *********/


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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;    

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
        
    if(table->is_table_initiated == 0){         
        DBAL_TABLE_INFO_UPDATE((*table), db_prefix, db_prefix_len, physical_db, nof_qualifiers, additional_info, qual_info, table_name);
        table->is_table_initiated = 1;
    }else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_create - trying to init table more than once table_id %d"), table_id));
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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - table not initiated")));
    }

    switch(table->physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - LEM not supported")));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_table_clear - LPM not supported")));
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


/********* PROGRAM FUNCTIONS *********/

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
    arad_pp_dbal_program_to_tables_associate(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                             SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],uint8 use_32_bit_ce[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] [SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], int nof_valid_keys)
{
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;
    int                         curr_key_id, curr_qual_id, nof_table_programs, table_id;
    uint32                      ce_rsrc[1];
    ARAD_PP_KEY_CE_ID           ce_ndx = 0;
    uint32                      last_unchecked_ce;
    SOC_DPP_DBAL_TABLE_INFO*    table;
    int                         nof_bits_used_in_key;
    int                         is_msb; /* which key is costructed */

    SOCDNX_INIT_FUNC_DEFS;

    if (stage < 0 || stage >= ARAD_NOF_FP_DATABASE_STAGES) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid stage %d. \n\rThe range is: 0 - ARAD_NOF_FP_DATABASE_STAGES-1.\n\r"), stage));
    }
    SOCDNX_IF_ERR_EXIT(arad_sw_db_pgm_ce_rsrc_get(unit, stage, program_id, 0, ce_rsrc));

    for (curr_key_id = 0; curr_key_id < nof_valid_keys; curr_key_id++){

        table_id = keys_to_table_id[curr_key_id].sw_table_id;
        table = &(dbal_tables[unit][table_id]);
        if (!table->is_table_initiated) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_to_tables_associate - table not initiated")));
        }
        nof_bits_used_in_key = 0;
        last_unchecked_ce = ARAD_PMF_LOW_LEVEL_CE_NDX_MAX;
        is_msb = FALSE;

        nof_table_programs = table->nof_table_programs;

#ifdef DBAL_ADVANCED_PRINTS_ENABLED
        LOG_CLI((BSL_META("associating table %s to program %d key ID = %d, ce_rsrc %u\n"),table->table_name, program_id, keys_to_table_id[curr_key_id].key_id, (*ce_rsrc) ));
#endif

        for (curr_qual_id = 0; curr_qual_id < table->nof_qualifiers; curr_qual_id++){

            qualifier_info = &(table->qual_info[curr_qual_id]);

            /* validations for key size */
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage, qualifier_info->qual_type, &(qualifier_info->qual_full_size), &(qualifier_info->qual_is_in_hdr)));
            if (qualifier_info->qual_nof_bits == 0) {
                qualifier_info->qual_nof_bits = qualifier_info->qual_full_size;
            }
            if (nof_bits_used_in_key + qualifier_info->qual_nof_bits > DBAL_KEY_C_ARAD_SIZE_IN_BITS){
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_to_tables_associate nof bits is bigger than 160, current nof bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
            }
            if( ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) {
                if (nof_bits_used_in_key + qualifier_info->qual_nof_bits > DBAL_KEY_SIZE_IN_BITS && !is_msb) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_to_tables_associate number of bits for LSB key is bigger than 80 current bits is %d + %d"), nof_bits_used_in_key, qualifier_info->qual_nof_bits));
                }            
            } else if( (keys_to_table_id[curr_key_id].key_id != SOC_DPP_DBAL_PROGRAM_KEY_C) && nof_bits_used_in_key + qualifier_info->qual_nof_bits > DBAL_KEY_SIZE_IN_BITS) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_to_tables_associate number of bits is bigger than 80 for key current bits is %d"), nof_bits_used_in_key + qualifier_info->qual_nof_bits));
            } 

            /* function returns error if no CE available */
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_next_available_ce_get(unit, qualifier_info->qual_nof_bits, use_32_bit_ce[curr_key_id][curr_qual_id], last_unchecked_ce, stage, *ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE_USE_FIRST_16_BIT_FIRST, is_msb, &ce_ndx));

            last_unchecked_ce = ce_ndx - 1;            

#ifdef DBAL_ADVANCED_PRINTS_ENABLED
            LOG_CLI((BSL_META("\tfound free CE, ndx = %d, is_msb = %d, qualifier type %d, offset %d, size %d\n"), ce_ndx, is_msb, qualifier_info->qual_type, qualifier_info->qual_offset, qualifier_info->qual_nof_bits));
#endif
            SOCDNX_IF_ERR_EXIT(dbal_program_configure(unit, program_id, stage, keys_to_table_id[curr_key_id].key_id, (*qualifier_info), is_msb, ce_ndx));
            /* last_unchecked_ce can't be 134217727 as the coverity claims. It can be only if stage is not between 0 and ARAD_NOF_FP_DATABASE_STAGES - 1.
             * We do such check above 
             */
            /* coverity[overrun-local:FALSE] */
            SHR_BITSET(ce_rsrc, ce_ndx);
            SOCDNX_IF_ERR_EXIT(arad_sw_db_pgm_ce_rsrc_set(unit, stage, program_id, 0, ce_rsrc));
            table->table_programs[nof_table_programs].ce_assigned[curr_qual_id] = ce_ndx;
            nof_bits_used_in_key += qualifier_info->qual_nof_bits;
            if (nof_bits_used_in_key == DBAL_KEY_SIZE_IN_BITS) { /* exactly 80 bits, it means that from now we will use the MSB CEs to fill the MSB part of the key */
                if (ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB > 1) { /*check if there is MSB part to the key..*/
                    is_msb = TRUE;
                    last_unchecked_ce = (ARAD_PMF_LOW_LEVEL_CE_NDX_MAX*ARAD_PMF_LOW_LEVEL_NOF_LSB_MSB) + 1;
				}
            }
        }

        /* update table with the associated program and stage */                    
        table->table_programs[nof_table_programs].stage = stage;
        table->table_programs[nof_table_programs].program_id = program_id;
        table->table_programs[nof_table_programs].key_id = keys_to_table_id[curr_key_id].key_id;            
        table->nof_table_programs++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/* a backdoor to insert which CE ID to each qualifier
   use the array qualifier_to_ce_id to indicate which qualifier use which ce_id.
   the CE ids is running between 0-31 when 0-15 is for LSB key and 16-31 is for MSB key.
   the qualifiers should be in the same order of the qualifiers set in the table. */
uint32
    arad_pp_dbal_program_to_tables_associate_implicit(int unit, int program_id, ARAD_FP_DATABASE_STAGE stage, 
                                                      SOC_DPP_DBAL_KEY_TO_TABLE keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS],
                                                      uint8 qualifier_to_ce_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS][SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
                                                      int nof_valid_keys)
{    
    SOC_DPP_DBAL_QUAL_INFO*     qualifier_info;
    int                         curr_key_id, curr_qual_id;
    int                         nof_table_programs;
    SOC_DPP_DBAL_TABLE_INFO*    table;
    uint32                      ce_rsrc[1];
    ARAD_PP_KEY_CE_ID           ce_ndx;
    int                         is_msb = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_sw_db_pgm_ce_rsrc_get(unit, stage, program_id, 0, ce_rsrc));
    
    for (curr_key_id = 0; curr_key_id < nof_valid_keys; curr_key_id++){

        table = &(dbal_tables[unit][keys_to_table_id[curr_key_id].sw_table_id]);
        nof_table_programs = table->nof_table_programs;

#ifdef DBAL_ADVANCED_PRINTS_ENABLED
        LOG_CLI((BSL_META("associating table %s to program %d key ID = %d, ce_rsrc %u\n"),table->table_name, program_id, keys_to_table_id[curr_key_id].key_id, (*ce_rsrc) ));
#endif
        for (curr_qual_id = 0; curr_qual_id < table->nof_qualifiers; curr_qual_id++){
            
            /* configure the HW */
            qualifier_info = &(table->qual_info[curr_qual_id]);
            ce_ndx = qualifier_to_ce_id[curr_key_id][curr_qual_id];
            SOCDNX_IF_ERR_EXIT(arad_pp_dbal_qualifier_full_size_get(unit, stage, qualifier_info->qual_type, &(qualifier_info->qual_full_size), &(qualifier_info->qual_is_in_hdr)));
            if (qualifier_info->qual_nof_bits == 0) {
                qualifier_info->qual_nof_bits = qualifier_info->qual_full_size;
            }
            if (ce_ndx > ARAD_PMF_LOW_LEVEL_CE_NDX_MAX) {                
                is_msb = 1;
            }
#ifdef DBAL_ADVANCED_PRINTS_ENABLED
            LOG_CLI((BSL_META("using CE ndx = %d, is_msb = %d for qualifier type %d, offset %d, size %d\n"), ce_ndx, is_msb, qualifier_info->qual_type, qualifier_info->qual_offset, qualifier_info->qual_nof_bits));
#endif
            SOCDNX_IF_ERR_EXIT(dbal_program_configure(unit, program_id, stage, keys_to_table_id[curr_key_id].key_id, (*qualifier_info), is_msb, ce_ndx));
            SHR_BITSET(ce_rsrc, ce_ndx);
            SOCDNX_IF_ERR_EXIT(arad_sw_db_pgm_ce_rsrc_set(unit, stage, program_id, 0, ce_rsrc));
            table->table_programs[nof_table_programs].ce_assigned[curr_qual_id] = ce_ndx;
        }

    /* update table with the associated program and stage */        
        table->table_programs[nof_table_programs].stage = stage;
        table->table_programs[nof_table_programs].program_id = program_id;
        table->table_programs[nof_table_programs].key_id = curr_key_id;
        table->nof_table_programs ++;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
    arad_pp_dbal_table_physical_db_get(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, SOC_DPP_DBAL_PHYSICAL_DB_TYPES* physical_db_type )
{

    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - table not initiated")));
    }

    (*physical_db_type) = table->physical_db_type;

exit:
    SOCDNX_FUNC_RETURN;
}

/********* ENTRY MANAGEMENT FUNCTIONS *********/

STATIC uint32
    arad_pp_dbal_entry_add_lpm(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
  ARAD_PP_IPV4_LPM_MNGR_INFO  lpm_mngr;
  uint8                       is_pending_op,success_bool;
  SOC_SAND_PP_IPV4_SUBNET     key;
  uint32                      prefix = 0, vrf = 0, prefix_len = 0, dest = 0;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lpm_buffer(unit, qual_vals, table_id, &prefix, &vrf, &prefix_len));

  dest = (*((uint32*)payload)); /*payload in LPM is uint32*/

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_lpm_mngr_get(unit, &lpm_mngr));

  key.ip_address = prefix;
  key.prefix_len = prefix_len;

  /* is pedning depends on IPV4 routing status */
  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(unit, ARAD_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM, &is_pending_op));

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ipv4_lpm_mngr_prefix_add(&lpm_mngr, vrf, &key, dest, is_pending_op, ARAD_PP_FRWRD_IP_ROUTE_LOCATION_LPM, &success_bool));

  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

  if (*success == SOC_SAND_SUCCESS) {
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit, vrf, TRUE));
  }

exit:
  SOCDNX_FUNC_RETURN;
}


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
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    uint32  entry_payload[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)];
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    entry_payload[0] = *((uint32*)payload); /* payload in SEM is uint32 */

    sal_memset(entry_key,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(entry_mask,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry_key, entry_mask ));
    /* we use the stage of the first program because we assume that all the programs that use the DB are from the same stage.*/

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&(table->db_prefix), ARAD_PP_ISEM_ACCESS_LSB_PREFIX, ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX, entry_key));

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
    
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint8 is_in_vtt_stage = ((table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));

    SOCDNX_INIT_FUNC_DEFS;

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    /* recieve hash table */
    hash_tbl = arad_sw_db_entry_key_to_entry_id_hash_get(unit);
    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table_id), 1);

#ifdef DBAL_ADVANCED_PRINTS_ENABLED
    arad_pp_dbal_print_buffer(&stream[0], ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2+1, "Entry stream for hash");        
#endif

    /* SOCDNX_SAND_IF_ERR_EXIT(soc_sand_U32_to_U8(entry.value, ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES,stream)); */    
  
    if (is_in_vtt_stage) {
        action.value[0] = (*(ARAD_PP_ISEM_ACCESS_ENTRY*)payload).sem_result_ndx;
    } else { /* in FLP stage the result is uint32 */
        action.value[0] = (*(uint32*)payload);
    }
    
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(hash_tbl, stream, &data_indx, &found));

    if (!found) {
        /* Insert the rule to the route_key -> entry_id table*/
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_add(hash_tbl, stream, &data_indx, &entry_added ));
    } else {
        entry_added = TRUE;
    }

    if (entry_added) { /* Remove the old entry, if applicable */
        if (is_in_vtt_stage) {
            tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table->additional_table_info);
        } else {
            tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table->additional_table_info);
        }

        SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_remove_unsafe(unit, FALSE /* is_bank_freed_if_no_entry */, tcam_db_id, data_indx));
        entry.valid = TRUE;
        /* Insert the new rule to the TCAM */
        if (!is_in_vtt_stage) {
            entry.is_for_update = found;               
        }

        SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_add_unsafe(unit, tcam_db_id, data_indx, FALSE, priority, &entry, &action, success ));

        if ((*success != SOC_SAND_SUCCESS) && (!found)) {
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(hash_tbl, data_indx));
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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint32  prefix_len;
    
    SOCDNX_INIT_FUNC_DEFS;

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table->db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(unit, table_size_in_bytes, entry_key, entry_mask, elk_data, elk_mask));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_to_kbp_payload_buffer_encode(unit, table_payload_in_bytes, ((ARAD_TCAM_ACTION *)payload)->value, elk_ad_value));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(elk_mask, table_size_in_bytes, &prefix_len));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_lpm_route_add(unit, table->db_prefix, 0, prefix_len, elk_data, elk_ad_value, success));
        /* entry Id in this case is has no significance  */

#ifdef KBP_COMPARE_DEBUG
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_kbp_compare(unit, table->db_prefix, elk_data));
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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1/*private_table*/, &kaps_table_id));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_route_to_kaps_payload_buffer_encode(unit, payload, ad_bytes));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_add_hw(unit, kaps_table_id, prefix_len, data_bytes, ad_bytes, success));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_add(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, void* payload, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add - table not initiated")));
    }

    switch(table->physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_lem(unit, table_id, qual_vals, payload, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_add_lpm(unit, table_id, qual_vals, payload, success));
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
    LOG_CLI((BSL_META("\nEntry added to table %s \n"), table->table_name));    
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
    LOG_CLI((BSL_META("\nLIF: %d \n"), *(uint32*)payload));
#endif

    table->nof_entries_added_to_table++;

exit:
    SOCDNX_FUNC_RETURN;
}







STATIC uint32
    arad_pp_dbal_entry_delete_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(entry_key,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(entry_mask,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry_key, entry_mask ));
    /* we use the stage of the first program because we assume that all the programs that use the DB are from the same stage.*/

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&(table->db_prefix), ARAD_PP_ISEM_ACCESS_LSB_PREFIX, ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX, entry_key));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_remove_unsafe(unit, entry_key, sem_id));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_delete_lpm(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
  ARAD_PP_IPV4_LPM_MNGR_INFO  lpm_mngr;
  uint8                       is_pending_op,success_bool;
  SOC_SAND_PP_IPV4_SUBNET     key;
  uint32                      prefix = 0, vrf = 0, prefix_len = 0;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lpm_buffer(unit, qual_vals, table_id, &prefix, &vrf, &prefix_len));

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_lpm_mngr_get(unit, &lpm_mngr));

  key.ip_address = prefix;
  key.prefix_len = prefix_len;

  /* is pedning depends on IPV4 routing status */
  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get(unit, ARAD_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM, &is_pending_op));

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ipv4_lpm_mngr_prefix_remove(&lpm_mngr, vrf, &key, is_pending_op, &success_bool));

  *success = (success_bool == TRUE) ? SOC_SAND_SUCCESS : SOC_SAND_FAILURE_OUT_OF_RESOURCES;

  if (*success == SOC_SAND_SUCCESS) {
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_cache_vrf_modified_set(unit, vrf, TRUE));
  }

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

    uint32 data_indx, tcam_db_id;
    uint8  stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2 + 1]; /* the stream consist of value mask table Id */
    SOC_SAND_HASH_TABLE_INFO *hash_tbl;
    uint8 found;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;

    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint8 is_in_vtt_stage = ((table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));

    SOCDNX_INIT_FUNC_DEFS;

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);

    /* recieve hash table */
    hash_tbl = arad_sw_db_entry_key_to_entry_id_hash_get(unit);
    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table_id), 1);

#ifdef DBAL_ADVANCED_PRINTS_ENABLED
    arad_pp_dbal_print_buffer(&stream[0], ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2+1, "Entry stream for hash");
#endif

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(hash_tbl, stream, &data_indx, &found));

    if (!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("entry doesn't exists")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_remove_by_index(hash_tbl,data_indx));

    if (is_in_vtt_stage) {
        tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table->additional_table_info);
    } else {
        tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table->additional_table_info);
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit,tcam_db_id,data_indx));

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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint32  prefix_len;
    
    SOCDNX_INIT_FUNC_DEFS;

    (*success) = SOC_SAND_FAILURE_INTERNAL_ERR;

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table->db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(unit, table_size_in_bytes, entry_key, entry_mask, elk_data, elk_mask));*/

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(elk_mask, table_size_in_bytes, &prefix_len));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_lpm_route_remove(unit, table->db_prefix, prefix_len, elk_data));
        /* entry Id in this case is has no significance  */

    (*success) = SOC_SAND_SUCCESS;

#ifdef KBP_COMPARE_DEBUG
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_kbp_compare(unit, table->db_prefix, elk_data ));
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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    (*success) = SOC_SAND_FAILURE_INTERNAL_ERR;

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1/*private_table*/, &kaps_table_id));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_entry_remove_hw(unit, kaps_table_id, prefix_len, data_bytes));

    (*success) = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_delete(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete - table not initiated")));
    }

    switch(table->physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_lem(unit, table_id, qual_vals, success));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_delete_lpm(unit, table_id, qual_vals, success));
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

    table->nof_entries_added_to_table--;

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry deleted from table %s \n"),table->table_name));    
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC uint32
    arad_pp_dbal_entry_get_lpm(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint8 *found)
{
  ARAD_PP_IPV4_LPM_MNGR_INFO  lpm_mngr;
  SOC_SAND_PP_IPV4_SUBNET     key;
  uint32                      prefix = 0, vrf = 0, prefix_len = 0;

  SOC_PPC_FRWRD_IP_ROUTE_STATUS                 route_status;
  SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_LOCATION  hw_target_type;
  SOC_SAND_PP_SYSTEM_FEC_ID                     fec_id;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lpm_buffer(unit, qual_vals, table_id, &prefix, &vrf, &prefix_len));

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_sw_db_ipv4_lpm_mngr_get(unit, &lpm_mngr));

  key.ip_address = prefix;
  key.prefix_len = prefix_len;

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ipv4_lpm_mngr_sys_fec_get(&lpm_mngr, vrf, &key, TRUE, &fec_id, &route_status, &hw_target_type,found));

  *((uint32*)payload) = fec_id;

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

    request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_lem_access_entry_by_key_get_unsafe(unit, &request.key, (ARAD_PP_LEM_ACCESS_PAYLOAD*)(payload), found));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_get_sem(int unit, uint32 sem_id, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint8 *found)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE];
    uint32  entry_payload[SOC_DPP_DEFS_MAX(ISEM_PAYLOAD_NOF_UINT32)];
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(entry_key,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);
    sal_memset(entry_mask,0x0, sizeof(uint32) * ARAD_PP_FP_TCAM_ENTRY_SIZE);    
	
	entry_payload[0] = 0;
                                                                                                                    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry_key, entry_mask ));
    /* we use the stage of the first program because we assume that all the programs that use the DB are from the same stage.*/

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&(table->db_prefix), ARAD_PP_ISEM_ACCESS_LSB_PREFIX, ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX, entry_key));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_isem_entry_get_unsafe(unit, entry_key, entry_payload, sem_id, found));

    *((uint32*)payload) = entry_payload[0]; /* payload in SEM is uint32 */

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_entry_get_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    uint32 data_indx, tcam_db_id;
    uint8  stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2 + 1]; /* the stream consist of value mask table Id */
    SOC_SAND_HASH_TABLE_INFO *hash_tbl;
    ARAD_TCAM_ENTRY  entry;
    ARAD_TCAM_ACTION action;    

    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint8 is_in_vtt_stage = ((table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));

    SOCDNX_INIT_FUNC_DEFS;

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
    ARAD_TCAM_ACTION_clear(&action);
    
    /* recieve hash table */
    hash_tbl = arad_sw_db_entry_key_to_entry_id_hash_get(unit);
                                                                                                                
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));

    sal_memcpy(&(stream[0]), &(entry.value[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES]), &(entry.mask[0]), ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES);
    sal_memcpy(&(stream[ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2]), &(table_id), 1);

#ifdef DBAL_ADVANCED_PRINTS_ENABLED
    arad_pp_dbal_print_buffer(&stream[0], ARAD_PP_ISEM_ACCESS_TCAM_KEY_LEN_BYTES*2+1, "Entry stream for hash");
#endif

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_hash_table_entry_lookup(hash_tbl, stream, &data_indx, found));

    if (is_in_vtt_stage) {
        tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table->additional_table_info);
    } else {
        tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table->additional_table_info);
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_get_unsafe(unit, tcam_db_id, data_indx, TRUE /*hit_bit_clear*/,  priority, &entry, &action, found, hit_bit ));

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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint32  prefix_len;
    
    SOCDNX_INIT_FUNC_DEFS;

    (*priority) = 0;
                                                                                                                
    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_kbp_table_size_get(unit, table->db_prefix, &table_size_in_bytes, &table_payload_in_bytes));

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_buffer_to_kbp_buffer_encode(unit, table_size_in_bytes, entry_key, entry_mask, elk_data, elk_mask));*/

    /*SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_lpm_prefix_len_get(elk_mask, table_size_in_bytes, &prefix_len));*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, table, table_size_in_bytes, qual_vals, &prefix_len, elk_data));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_lpm_route_get(unit, table->db_prefix, prefix_len, elk_data, elk_ad_value, found));

    if (*found) {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_route_from_kbp_payload_buffer_decode(unit, table->db_prefix, elk_ad_value,((ARAD_TCAM_ACTION*)payload)->value));
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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    JER_KAPS_IP_TBL_ID kaps_table_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(jer_pp_kaps_dbal_table_id_translate(unit, table_id, qual_vals, -1/*private_table*/, &kaps_table_id));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_kbp_buffer(unit, table, JER_KAPS_KEY_BUFFER_NOF_BYTES, qual_vals, &prefix_len, data_bytes));

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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get - table not initiated %d"), table_id));
    }

    switch(table->physical_db_type)
    {
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_lem(unit, table_id, qual_vals, payload, found));
        break;

    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM:
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_get_lpm(unit, table_id, qual_vals, payload, found));
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
    LOG_CLI((BSL_META("Entry searched in table %s, found=%d\n"), table->table_name, (*found)));
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_add_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32 priority, SOC_SAND_SUCCESS_FAILURE* success)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint8 is_in_vtt_stage = ((table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));
    uint32 tcam_db_id;
    ARAD_TCAM_ENTRY  entry;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_TCAM_ENTRY_clear(&entry);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, entry.value, entry.mask ));
    
    if (is_in_vtt_stage) {
        tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table->additional_table_info);
    } else {
        tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table->additional_table_info);
    }
    entry.valid = TRUE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_add_unsafe(unit, tcam_db_id, entry_id, FALSE, priority, &entry, (ARAD_TCAM_ACTION*)payload, success ));

exit:
    SOCDNX_FUNC_RETURN;
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_add_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 payload[ARAD_TCAM_ACTION_MAX_LEN], uint32 priority, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE* success)
{
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, entry_key, entry_mask ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_tcam_entry_add(unit, table->db_prefix, entry_id, is_for_update,
                                                               priority, entry_key, entry_mask, payload, success));    
exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_add_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 priority, ARAD_TCAM_ACTION* payload, uint8 is_for_update, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - table not initiated")));
    }

    switch(table->physical_db_type)
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
     /* SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:*/
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_add_id - ilegal physical DB type")));
        break;
    }

#ifdef DBAL_PRINTS_ENABLED
    LOG_CLI((BSL_META("\nEntry added to table %s \n"), table->table_name));   
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);
#endif

    table->nof_entries_added_to_table++;

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_entry_delete_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    uint32 tcam_db_id;

    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint8 is_in_vtt_stage = ((table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));

    SOCDNX_INIT_FUNC_DEFS;

    if (is_in_vtt_stage) {
        tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table->additional_table_info);
    } else {
        tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table->additional_table_info);
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_managed_db_entry_remove_unsafe(unit, tcam_db_id, entry_id));

    *success = SOC_SAND_SUCCESS;

exit:
    SOCDNX_FUNC_RETURN;
}
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_delete_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_route_remove(unit, table->db_prefix, entry_id));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_delete_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, SOC_SAND_SUCCESS_FAILURE *success)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    
    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - table not initiated")));
    }

    switch(table->physical_db_type)
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
    
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:        
        break;

    default:
     /* SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:*/
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_delete_id - ilegal physical DB type")));
        break;
    }


    table->nof_entries_added_to_table--;

#ifdef DBAL_PRINTS_ENABLED
    /*LOG_CLI((BSL_META("\nEntry deleted from table \n"),table->table_name));
    arad_pp_dbal_entry_dump(unit, table_id, qual_vals);*/
#endif

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC uint32    
    arad_pp_dbal_entry_get_id_tcam(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    uint32 tcam_db_id;    
    ARAD_TCAM_ENTRY  entry;

    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint8 is_in_vtt_stage = ((table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_TT) || (table->table_programs[0].stage == ARAD_FP_DATABASE_STAGE_INGRESS_VT));

    SOCDNX_INIT_FUNC_DEFS;

    /* TCAM */
    ARAD_TCAM_ENTRY_clear(&entry);
                                                                                                                    
    if (is_in_vtt_stage) {
        tcam_db_id = arad_pp_isem_access_tcam_db_id_get(table->additional_table_info);
    } else {
        tcam_db_id = arad_pp_frwrd_ip_tcam_ip_db_id_get(table->additional_table_info);
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_tcam_db_entry_get_unsafe(unit, tcam_db_id, entry_id, TRUE /*hit_bit_clear*/,  priority, &entry, (ARAD_TCAM_ACTION*)payload, found, hit_bit ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 0, 1, qual_vals, entry.value, entry.mask ));

exit:
    SOCDNX_FUNC_RETURN;
}


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_entry_get_id_kbp(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8* found)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    uint32  entry_key[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    uint32  entry_mask[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_frwrd_ip_tcam_kbp_tcam_entry_get( 
           unit, 
           table->db_prefix, 
           entry_id,
           entry_key,
           entry_mask,
           payload,
           priority,
           found,
           hit_bit
         ));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, 1, 1, qual_vals, entry_key, entry_mask ));

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

uint32
    arad_pp_dbal_entry_get_id(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, uint32 entry_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], void* payload, uint32* priority, uint8* hit_bit, uint8 *found)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if(!table->is_table_initiated) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_entry_get_id - table not initiated")));
    }

    switch(table->physical_db_type)
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
    
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS:        
        break;

    default:
     /* SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B:*/
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
/********* SERVICE FUNCTIONS *********/

/*
   looking the next free instruction in the program and use it.
   strats from the 32bit instructions.
 
   two modes of operation:
   must use 16 instruction for qualifiers that are less than 16 bits.
   use the first available CE (can use 32 bit instructions for qualifiers that are less than 16 bit)
*/
STATIC uint32    arad_pp_dbal_next_available_ce_get(int unit, int qual_nof_bits, uint8 use_ce_32, uint32 last_unchecked_ce, ARAD_FP_DATABASE_STAGE stage, uint32 ce_rsrc, SOC_DPP_DBAL_CE_SEARCH_MODE mode, int is_msb, ARAD_PP_KEY_CE_ID* free_ce)
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
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("arad_pp_dbal_program_to_tables_associate - no available CE last_unchecked_ce = %d"
                                                              "ce_rsrc = %u, is_msb = %d, qual_nof_bits = %d "), last_unchecked_ce, ce_rsrc, is_msb, qual_nof_bits));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

           
STATIC uint32
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
  arad_pp_dbal_entry_key_to_lem_buffer(int unit, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, ARAD_PP_LEM_ACCESS_KEY *key)
{
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_KEY_clear(key);

    key->type = table->additional_table_info;

    key->nof_params = table->nof_qualifiers;

    for (i = 0; i < table->nof_qualifiers; i++) {        
        if(i > 0){
            if(table->qual_info[i].qual_type == table->qual_info[i-1].qual_type) {
                key->param[i-1].nof_bits += table->qual_info[i].qual_nof_bits;
                key->param[i-1].value[1] = qual_vals[i].val.arr[0];
                key->nof_params -= 1;
            }else {
                key->param[i].nof_bits = table->qual_info[i].qual_nof_bits;
                key->param[i].value[0] = qual_vals[i].val.arr[0];
            }
        }else {
                key->param[i].nof_bits = table->qual_info[i].qual_nof_bits;
                key->param[i].value[0] = qual_vals[i].val.arr[0];
        }
    }

    key->prefix.nof_bits = table->db_prefix_len;
    key->prefix.value = table->db_prefix;

    SOCDNX_FUNC_RETURN;
}


STATIC uint32
  arad_pp_dbal_entry_key_to_lpm_buffer(int unit, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX], uint32 table_id, uint32* prefix, uint32* vrf, uint32* prefix_len)
{
    int i;
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    *prefix_len = 0;
    
    for (i = 0; i < table->nof_qualifiers; i++) {

        if(qual_vals[i].type == SOC_PPC_FP_QUAL_IRPP_VRF){
            *vrf = qual_vals[i].val.arr[0];
        } else {
            *prefix = qual_vals[i].val.arr[0];            
        }
        *prefix_len = _shr_ip_mask_length(qual_vals[i].is_valid.arr[0]);
    }        

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
            /*qual_offset += iter_bit_len;*/

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
#endif




/****************** DIAGNOSTICS FUNCTIONS ******************/
uint32
    arad_pp_dbal_entry_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX])
{
    SOC_DPP_DBAL_TABLE_INFO*    table = &(dbal_tables[unit][table_id]);
    uint32                      data[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] = {0};
    uint32                      mask[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] = {0};
    uint8* data_as_uint8;
    int i;

    SOCDNX_INIT_FUNC_DEFS;
    
    for (i = 0; i < table->nof_qualifiers; i++) {
        /* mask can also be printed if needed (but disabled because of last packet diagnostics)
           LOG_CLI((BSL_META("\t Qualifier %s value (0x%x) mask (0x%x)\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(qual_vals[i].type)), qual_vals[i].val.arr[0], qual_vals[i].is_valid.arr[0]));*/
        LOG_CLI((BSL_META("\t Qualifier %s value (0x%x)\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(qual_vals[i].type)), qual_vals[i].val.arr[0]));
    }    

    LOG_CLI((BSL_META("\t DB prefix (%d)"), table->db_prefix));

    /* all should work with buffer */
    if (table->physical_db_type != SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM && table->physical_db_type != SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM) {

        if (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 1, 1, qual_vals, data, mask));
        } else{
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, table->table_programs[0].stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_KEY_TO_BUFFER, 0, 1, qual_vals, data, mask));
        }
        if (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A || table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B) {
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_set_any_field(&(table->db_prefix), ARAD_PP_ISEM_ACCESS_LSB_PREFIX, ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX, data));        
        }

        data_as_uint8 = (uint8*)(&(data[0]));
        if(table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM)
        {
            arad_pp_dbal_print_buffer(data_as_uint8, 16, "\n\t Full tcam key as buffer");
        }else
        {
            arad_pp_dbal_print_buffer(data_as_uint8, ARAD_PP_ISEM_ACCESS_KEY_SIZE*4, "\n\t Full key (include prefix) as buffer");
        }

    } else if(table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM){
        uint32  prefix = 0, vrf = 0, prefix_len = 0;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_entry_key_to_lpm_buffer(unit, qual_vals, table_id, &prefix, &vrf, &prefix_len));
        LOG_CLI((BSL_META("\nEntry vrf = %d, prefix =%d, prefix length = %d\n"), vrf, (prefix), prefix_len));

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
    SOC_DPP_DBAL_TABLE_INFO* table = &(dbal_tables[unit][table_id]);

    SOCDNX_INIT_FUNC_DEFS;

    if (!table->is_table_initiated) {
        goto exit;
    }

    /* dump information about the table */
    LOG_CLI((BSL_META("\nTable: %s, ID (%d)\n"), (table->table_name), table_id));
    LOG_CLI((BSL_META("  Physical db type %s\n"), (arad_pp_dbal_physical_db_to_string(table->physical_db_type) )));
    if (table->db_prefix == DBAL_PREFIX_NOT_DEFINED) {
        LOG_CLI((BSL_META("  DB prefix %s\n"), (" not defined")));
    }else {
        LOG_CLI((BSL_META("  DB prefix %d, prefix length (%d)\n"), (table->db_prefix), (table->db_prefix_len)));
    }
    LOG_CLI((BSL_META("  Entries in table %d\n"), (table->nof_entries_added_to_table)));
    LOG_CLI((BSL_META("  Additional table info %d\n"), (table->additional_table_info)));
    LOG_CLI((BSL_META("  Number of qualifiers %d\n"), (table->nof_qualifiers)));
    for (i = 0; i < table->nof_qualifiers; i++) {
        LOG_CLI((BSL_META("  \tQualifier: %s, offset (%d), size (%d)\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(table->qual_info[i].qual_type)), (table->qual_info[i].qual_offset), (table->qual_info[i].qual_nof_bits)));        
    }
    LOG_CLI((BSL_META("  Number of programs associated to table (%d)\n"), (table->nof_table_programs)));
    for (i = 0; i < table->nof_table_programs; i++) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_program_to_string(unit, table->table_programs[i].stage, table->table_programs[i].program_id, &str));
        LOG_CLI((BSL_META("  \tProgram %s ID %d\n"), str, (table->table_programs[i].program_id)));
        for (j = 0; j < table->nof_qualifiers; j++) {
            LOG_CLI((BSL_META("  \tCE assigned for qualifier %s - %d\n"), (ARAD_PP_FP_QUAL_TYPE_to_string(table->qual_info[j].qual_type)), (table->table_programs[i].ce_assigned[j])));            
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    arad_pp_dbal_tables_dump(int unit, int is_full_info)
{
    int table_id;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META("\ndbal tables dump\n----------------\n")));
    
    for (table_id = 0; table_id < SOC_DPP_DBAL_SW_NOF_TABLES; table_id++) {
        if (is_full_info) {
            SOCDNX_SAND_IF_ERR_EXIT(arad_pp_dbal_table_info_dump(unit, table_id));
        }else {
            if (dbal_tables[unit][table_id].is_table_initiated) {
                /* dump information about the table */
                LOG_CLI((BSL_META("\nTable: %s, ID (%d)\n"), (dbal_tables[unit][table_id].table_name), table_id));
            }
        }    
     }
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

    SOCDNX_IF_ERR_EXIT(arad_sw_db_pgm_ce_rsrc_get(unit, stage, program_id, 0, &(ce_rsrc[0])));

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
    arad_pp_dbal_key_dump(int unit, SOC_DPP_DBAL_SW_TABLE_IDS table_id, int lookup_number, ARAD_FP_DATABASE_STAGE  stage)
{
    uint32 ind, qual_val_ndx, res;
    uint32 val[ARAD_PP_DIAG_DBG_VAL_LEN];
    SOC_DPP_DBAL_TABLE_INFO* table;
    uint8   is_for_kbp = 0;
    ARAD_PP_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 mask_in[ARAD_PP_FP_TCAM_ENTRY_SIZE] = {0};
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE key_signal_id[5];/* max signals per key */
    SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE hit_signal_id = 0, res_signal_id = 0, opcode_signal_id = 0;
    int nof_key_signals = 1;
    int size_of_signal_in_bits;

    SOCDNX_INIT_FUNC_DEFS;

    table = &(dbal_tables[unit][table_id]);

    for(qual_val_ndx = 0; qual_val_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; ++qual_val_ndx) {
        ARAD_PP_FP_QUAL_VAL_clear(&qual_vals[qual_val_ndx]);
    }

    LOG_CLI((BSL_META("\n   A lookup was made in table: %s, lookup information:\n\n"), table->table_name ));
    
    SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_ids_get(unit, table->physical_db_type, lookup_number, stage, key_signal_id, &res_signal_id, &hit_signal_id, &opcode_signal_id, &nof_key_signals));

    if(table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
        is_for_kbp = 1;
    }

    LOG_CLI((BSL_META("    KEY:\n")));

    SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, key_signal_id[0], val, &size_of_signal_in_bits));
    if (nof_key_signals > 1) {
        uint32 segment_val[ARAD_PP_DIAG_DBG_VAL_LEN];
        int last_pos = size_of_signal_in_bits;        
        for (ind = 1; ind < nof_key_signals; ind++) {
            SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, key_signal_id[ind], segment_val, &size_of_signal_in_bits));
            SHR_BITCOPY_RANGE(/*dest*/&val[0],/*pos is dest*/ last_pos, /*src*/segment_val, 0, size_of_signal_in_bits);
            last_pos += size_of_signal_in_bits;
        }        
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_fp_key_value_buffer_mapping(unit, stage, table_id, ARAD_PP_FP_KEY_BUFFER_DIRECTION_BUFFER_TO_KEY, is_for_kbp, 1, qual_vals, val, mask_in));
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_entry_dump(unit, table_id, qual_vals));
    
    if (is_for_kbp) {
        LOG_CLI((BSL_META("    OPCODE:\n")));
        SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, opcode_signal_id, val, &size_of_signal_in_bits));
        LOG_CLI((BSL_META("0x%08x "), val[0]));
    }    

	
	LOG_CLI((BSL_META("    RESULT:\n")));
    res = arad_pp_signal_mngr_signal_get(unit, hit_signal_id, val, &size_of_signal_in_bits);
    if ((val[0] != 0) || (res == -1)) {
        int num_of_int; 
        SOCDNX_IF_ERR_EXIT(arad_pp_signal_mngr_signal_get(unit, res_signal_id, val, &size_of_signal_in_bits));
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
    arad_pp_dbal_last_packet_dump(int unit)
{
#define NUM_OF_STAGES  3
    int prog_id[NUM_OF_STAGES], i, stage, j;
    int first_table_id[NUM_OF_STAGES];
    int last_table_id[NUM_OF_STAGES];
    SOC_DPP_DBAL_TABLE_INFO* table;
    int lookup_number = 1;
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

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_isem_access_print_last_vtt_program_data(unit,0, &prog_id[stage_vt], &prog_id[stage_tt]));
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_flp_access_print_last_programs_data(unit, 0, &prog_id[stage_flp]));        

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
            table = &(dbal_tables[unit][i]);

            if (!(table->is_table_initiated)) {
                continue;
            }        
            for (j = 0; j < table->nof_table_programs; j++) {
                if ((table->table_programs[j].program_id == prog_id[stage]) && (table->table_programs[j].stage == actual_stages[stage])) {
                    if (table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM) {
                        ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
                        arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, table->table_programs[j].program_id, &flp_lookups_tbl);
                        if (flp_lookups_tbl.lem_2nd_lkp_valid == 1 && flp_lookups_tbl.lem_2nd_lkp_key_select == table->table_programs[j].key_id) {
                            lookup_number = 2;
                        }
                    } else if(table->physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM){
                        ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA flp_lookups_tbl;
                        arad_pp_ihb_flp_lookups_tbl_get_unsafe(unit, table->table_programs[j].program_id, &flp_lookups_tbl);
                        if (flp_lookups_tbl.lpm_2nd_lkp_valid == 1 && flp_lookups_tbl.lpm_2nd_lkp_key_select == table->table_programs[j].key_id) {
                            lookup_number = 2;
                        }
                    }
                    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_key_dump(unit, i, lookup_number, actual_stages[stage]));
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


const char*
    arad_pp_dbal_physical_db_to_string(SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type)
{
    const char* str = NULL;

    switch(physical_db_type)
    {
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM:
            str = "LEM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM:
            str = "LPM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM:
            str = "TCAM";
            break;
        case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP:
            str = "EXTERNAL TCAM";
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
        *str = "program unknown";
    }


exit:
    SOCDNX_FUNC_RETURN;
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




uint32
    arad_pp_dbal_flp_tables_create(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_kbp_tables_create(unit)); 
    if(JER_KAPS_ENABLE(unit)) {
       SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_kaps_tables_create(unit));
    }    
#endif

    if (SOC_DPP_CONFIG(unit)->pp.oam_statistics == 1) {
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_flp_oam_tables_create(unit)); 
    }
     
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint32
    arad_pp_dbal_flp_oam_tables_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_OPCODE;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_STATISTICS, qual_info, "oam statistics"));
    
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_IPV4;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "BFD statistics"));

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD; 
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_UNTAGGED;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOWN_UNTAGGED_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_DOWN_UNTAGGED_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_DOWN_UNTAGGED_STATISTICS, qual_info, "OAM down untagged statistics"));
    
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD; 
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_SINGLE_TAG;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_SINGLE_TAG_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_SINGLE_TAG_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 3, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_SINGLE_TAG_STATISTICS, qual_info, "OAM sinngle tag statistics"));

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD; 
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_OAM_MD_LEVEL_DOUBLE_TAG;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_TM_OUTER_TAG;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_TM_INNER_TAG;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_OAM_DOUBLE_TAG_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_OAM_DOUBLE_TAG_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 4, ARAD_PP_LEM_ACCESS_KEY_TYPE_OAM_DOUBLE_TAG_STATISTICS, qual_info, "OAM double tag statistics"));
    
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_MPLS;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_MPLS_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "BFD MPLS statistics"));

    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_MY_DISCR_PWE;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_BFD_PWE_STATISTICS, ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_BFD_STATISTICS, 4,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 1, ARAD_PP_LEM_ACCESS_KEY_TYPE_BFD_STATISTICS, qual_info, "BFD PWE statistics"));
exit:
     SOCDNX_FUNC_RETURN;    
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
STATIC uint32
    arad_pp_dbal_flp_kbp_tables_create(int unit)
{
    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];

    SOCDNX_INIT_FUNC_DEFS;

    if(ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED){
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD; /* 1 bit */
        qual_info[2].qual_nof_bits = 1;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD; /* 4 bit */
        qual_info[3].qual_nof_bits = 4;
        qual_info[3].qual_offset = 16;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD; /* 16 bit */        
        qual_info[4].qual_nof_bits = 16;
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;
        qual_info[5].qual_nof_bits = 3;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_MPLS_EXTENDED, DBAL_PREFIX_NOT_DEFINED, 0,
                                                    SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 6, 0, qual_info, "IPv4 MPLS extended UC KBP"));
    }

    if(ARAD_KBP_ENABLE_IPV6_EXTENDED){
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);

        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        qual_info[0].qual_offset = 32;        
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        qual_info[1].qual_nof_bits = 32;
        qual_info[1].qual_nof_bits = 32;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
        qual_info[2].qual_offset = 32;        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
        qual_info[3].qual_nof_bits = 32;
        qual_info[3].qual_nof_bits = 32;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;        
                
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_EXTENDED, DBAL_PREFIX_NOT_DEFINED, 0,
                                             SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 5, 0, qual_info, "IPv6 extended UC KBP"));        
    }

    if(ARAD_KBP_ENABLE_P2P_EXTENDED){
         DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
         qual_info[0].qual_type = SOC_PPC_FP_QUAL_IN_LIF_PROFILE;        
         SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_P2P_EXTENDED, DBAL_PREFIX_NOT_DEFINED, 0,
                                                      SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 1, 0, qual_info, "P2P extended UC KBP"));
    }
 
    if(ARAD_KBP_ENABLE_IPV4_MC && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mc_flexible_fwd_table", 0)){
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
        qual_info[0].qual_nof_bits = 27;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[3].qual_nof_bits = 4;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
        SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FLEXIBLE, DBAL_PREFIX_NOT_DEFINED, 0,
                                                     SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP, 5, 0, qual_info, "IPv4 MC flexible KBP"));
    }
exit:
     SOCDNX_FUNC_RETURN;
}


STATIC uint32
    arad_pp_dbal_flp_kaps_tables_create(int unit)
{
     SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
 
     SOCDNX_INIT_FUNC_DEFS;
 
    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[0].qual_nof_bits = 16;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_KAPS, JER_KAPS_IPV4_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv4 UC KAPS"));
 
    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[0].qual_nof_bits = 16;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_KAPS, JER_KAPS_IPV4_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv4 UC RPF KAPS"));
 
    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    /* LSB */
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[0].qual_nof_bits = 4;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
    qual_info[3].qual_nof_bits = 12;
 
    /* MSB */
    qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
    qual_info[4].qual_offset = 12;
    qual_info[4].qual_nof_bits = 3;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[5].qual_nof_bits = 1;
        qual_info[6].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
        qual_info[7].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
		qual_info[7].qual_offset = 16;
    qual_info[7].qual_nof_bits = 16;
        qual_info[8].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
        qual_info[8].qual_offset = 4;
    qual_info[8].qual_nof_bits = 12;
    qual_info[9].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
 
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS, JER_KAPS_IPV4_MC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 10, 0, qual_info, "IPv4 MC KAPS"));
 
    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    /* LSB */
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[0].qual_nof_bits = 16;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info[1].qual_nof_bits = 32;
    qual_info[1].qual_offset = 32;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info[2].qual_nof_bits = 32;
 
    /* MSB */
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info[3].qual_nof_bits = 32;
    qual_info[3].qual_offset = 32;
    qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info[4].qual_nof_bits = 32;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
 
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_KAPS, JER_KAPS_IPV6_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv6 UC KAPS"));
 
    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    /* LSB */
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[0].qual_nof_bits = 16;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info[1].qual_nof_bits = 32;
    qual_info[1].qual_offset = 32;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW;
    qual_info[2].qual_nof_bits = 32;
 
    /* MSB */
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info[3].qual_nof_bits = 32;
    qual_info[3].qual_offset = 32;        
    qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH;
    qual_info[4].qual_nof_bits = 32;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
 
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6UC_RPF_KAPS, JER_KAPS_IPV6_UC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 6, 0, qual_info, "IPv6 UC RPF KAPS"));
 
    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    /* LSB */
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
    qual_info[0].qual_nof_bits = 1;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_RIF;
    qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info[2].qual_nof_bits = 32;
    qual_info[2].qual_offset = 32;
    qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW;
    qual_info[3].qual_nof_bits = 32;
 
    /* MSB */
    qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info[4].qual_nof_bits = 32;
    qual_info[4].qual_offset = 32;
    qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH;
    qual_info[5].qual_nof_bits = 32;
    qual_info[6].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
 
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KAPS, JER_KAPS_IPV6_MC_TABLE_PREFIX, JER_KAPS_TBL_PREFIX_NOF_BITS,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS, 7, 0, qual_info, "IPv6 MC KAPS"));

    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM ************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_LEM, DBAL_PREFIX_NOT_DEFINED, 0,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, 0, qual_info, "IPv4 UC LEM"));

    /************************************************* SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM *************************************************/
    DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
    qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP;
    qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_VRF;
    SOCDNX_IF_ERR_EXIT(arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_IPV4UC_RPF_LEM, DBAL_PREFIX_NOT_DEFINED, 0,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LEM, 2, 0, qual_info, "IPv4 UC LEM RPF"));
												 
exit:
    SOCDNX_FUNC_RETURN;
}
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030)*/




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
    arad_pp_signal_mngr_signal_get(int unit, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id, uint32 val[24], int* size_of_signal_in_bits)
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
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_diag_dbg_val_get_unsafe(unit, (sig_info.prm_blk), &prm_fld, val));
        
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
    arad_pp_signal_mngr_signal_print(int unit, SOC_DPP_SIGNAL_MNGR_SIGNAL_TYPE signal_id)
{
    int num_of_int = 0;
    int ind, size_of_signal_in_bits;
    uint32 val[ARAD_PP_DIAG_DBG_VAL_LEN];
    int res = 0;        

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_signal_mngr_signal_get(unit, signal_id, val, &size_of_signal_in_bits);
    
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
    case SOC_DPP_DBAL_PHYSICAL_DB_TYPE_LPM:
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

