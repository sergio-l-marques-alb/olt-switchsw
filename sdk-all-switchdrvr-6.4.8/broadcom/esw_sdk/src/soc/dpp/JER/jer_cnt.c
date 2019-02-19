/*
 * $Id: jer_cnt.c Exp $
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
#define _ERR_MSG_MODULE_NAME BSL_SOC_CNT

/* 
 * Includes
 */ 

#include <shared/bsl.h>
#include <bcm/stat.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/JER/jer_cnt.h>



/*
 * Functions
 */

/*
 * Function:
 *      soc_jer_filter_config_egress_receive_set_get
 * Purpose:
 *      set and get filters criteria configurations in egress receive,
 *      eccentialy decides which filters criteria the CRPS will filter in and out
 * Parameters:
 *      unit -  unit number
 *      relevant_bit - the bit to set/reset in the register
 *      command_id - the CRPS support few types of command: 0,1, and special cases which has a uniqe command id. In the egress receive 0,1 and BCM_STAT_COUNTER_TM_COMMAND command IDs are supported.
 *      get - decides if get or set value
 *      value - value to set or returned value from get
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_filter_config_egress_receive_set_get(int unit, int relevant_bit, int command_id, int get, int* value)
{
    uint32 reg32_val;
    uint32 mask;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(value);

    /* create a mask for relevant bit */
    mask = 1U << relevant_bit;

    /* handle each command ID */
    if (command_id == BCM_STAT_COUNTER_TM_COMMAND) {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CRPS_TM_PQP_DROP_REASONS_MASKr(unit, SOC_CORE_ALL, &reg32_val));
        field32_val = soc_reg_field_get(unit, CGM_CRPS_TM_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_TM_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, CGM_CRPS_TM_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_TM_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_CRPS_TM_PQP_DROP_REASONS_MASKr(unit, SOC_CORE_ALL, reg32_val));
        }        
    } else if (command_id == 0) {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr(unit, SOC_CORE_ALL, &reg32_val));
        field32_val = soc_reg_field_get(unit, CGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_PP_1_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, CGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_PP_1_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_CRPS_PP_1_PQP_DROP_REASONS_MASKr(unit, SOC_CORE_ALL, reg32_val));
        }
    } else if (command_id == 1) {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr(unit, SOC_CORE_ALL, &reg32_val));
        field32_val = soc_reg_field_get(unit, CGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr, reg32_val, CRPS_PP_2_PQP_DROP_REASONS_MASKf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg_field_set(unit, CGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr, &reg32_val, CRPS_PP_2_PQP_DROP_REASONS_MASKf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_CRPS_PP_2_PQP_DROP_REASONS_MASKr(unit, SOC_CORE_ALL, reg32_val));
        }        
    } else {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "egress receive counter doesn't support command id %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_filter_config_ingress_set_get
 * Purpose:
 *      set and get filters criteria configurations in ingress,
 *      eccentialy decides which filters criteria the CRPS will filter in or out
 * Parameters:
 *      unit -  unit number
 *      relevant_bit - the bit to set/reset in the register
 *      command_id - the CRPS support few types of command, 0,1, and special cases which has a uniqe command id, here only 0 and 1 are supported.
 *      get - decides if get or set
 *      value - value to set, or returned value from get
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_filter_config_ingress_set_get(int unit, int relevant_bit, int command_id, int get, int* value)
{
    uint64 reg64_val;
    uint32 mask;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;

    /* create a mask for relevant bit */
    mask = 1U << relevant_bit;

    /* handle each command ID */
    if (command_id == 0) {
        SOCDNX_IF_ERR_EXIT(READ_IQM_IQM_CNT_CMD_CONFIG_Ar(unit, SOC_CORE_ALL, &reg64_val));
        field32_val = soc_reg64_field32_get(unit, IQM_IQM_CNT_CMD_CONFIG_Ar, reg64_val, IQM_CNT_CMD_ERRORS_FILTER_Af);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg64_field32_set(unit, IQM_IQM_CNT_CMD_CONFIG_Ar, &reg64_val, IQM_CNT_CMD_ERRORS_FILTER_Af, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_CNT_CMD_CONFIG_Ar(unit, SOC_CORE_ALL, reg64_val));
        }
    } else if (command_id == 1) {
        SOCDNX_IF_ERR_EXIT(READ_IQM_IQM_CNT_CMD_CONFIG_Br(unit, SOC_CORE_ALL, &reg64_val));
        field32_val = soc_reg64_field32_get(unit, IQM_IQM_CNT_CMD_CONFIG_Br, reg64_val, IQM_CNT_CMD_ERRORS_FILTER_Bf);
        if (get) {
            *value = (field32_val & mask) ? 1 : 0;
        } else {
            field32_val = (*value) ? field32_val | mask : field32_val & (~mask) ;
            soc_reg64_field32_set(unit, IQM_IQM_CNT_CMD_CONFIG_Br, &reg64_val, IQM_CNT_CMD_ERRORS_FILTER_Bf, field32_val);
            SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_CNT_CMD_CONFIG_Br(unit, SOC_CORE_ALL, reg64_val));
        }        
    } else {
        LOG_ERROR(BSL_LS_SOC_CNT, (BSL_META_U(unit, "ingress counter doesn't support command id %d\n"), command_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_map_filter_to_bit_and_source_type
 * Purpose:
 *      map a filter criterion to the relevant bit that controls it, and find the relevant source types for it
 * Parameters:
 *      unit -  unit number
 *      filter - filter criterion to map
 *      relevant_bit - the bit to set/reset in the register
 *      engine_source_max_count - size of array
 *      engine_source_array - array to store matching engine sources for drop reason
 *      engine_source_count - amount of found matches
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_map_filter_to_bit_and_source_type(int                               unit, 
                                              SOC_TMC_CNT_FILTER_TYPE           filter, 
                                              int*                              relevant_bit, 
                                              int                               engine_source_max_count, 
                                              SOC_TMC_CNT_SRC_TYPE*             engine_source_array, 
                                              int*                              engine_source_count)
{
    int is_egq = 0;
    int is_iqm = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(relevant_bit);
    SOCDNX_NULL_CHECK(engine_source_array);
    SOCDNX_NULL_CHECK(engine_source_count);

    /* set relevant info for given filter */
    switch (filter) {
    case SOC_TMC_CNT_TOTAL_PDS_THRESHOLD_VIOLATED:
        *relevant_bit = 0;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_TOTAL_PDS_UC_POOL_SIZE_THRESHOLD_VIOLATED:
        *relevant_bit = 1;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_PORT_UC_PDS_THRESHOLD_VIOLATED:
        *relevant_bit = 2;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_QUEUE_UC_PDS_THRESHOLD_VIOLATED:
        *relevant_bit = 3;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_PORT_UC_DBS_THRESHOLD_VIOLATED:
        *relevant_bit = 4;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_QUEUE_UC_DBS_THRESHOLD_VIOLATED:
        *relevant_bit = 5;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_QUEUE_DISABLE_BIT:
        *relevant_bit = 6;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_TOTAL_PDS_MC_POOL_SIZE_THRESHOLD_VIOLATED:
        *relevant_bit = 8;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_INTERFACE_PDS_THREHOLD_VIOLATED:
        *relevant_bit = 9;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_MC_SP_THRESHOLD_VIOLATED:
        *relevant_bit = 10;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_PER_MC_TC_THRESHOLD_VIOLATED:
        *relevant_bit = 11;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_MC_PDS_PER_PORT_THRESHOLD_VIOLATED:
        *relevant_bit = 12;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_MC_PDS_PER_QUEUE_THRESHOLD_VIOLATED:
        *relevant_bit = 13;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_MC_PER_PORT_SIZE_THRESHOLD_VIOLATED:
        *relevant_bit = 14;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_MC_PER_QUEUE_SIZE_THRESHOLD_VIOLATED:
        *relevant_bit = 15;
        is_egq = 1;
        break;
    case SOC_TMC_CNT_GLOBAL_REJECT_DISCARDS:
        *relevant_bit = 0;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_DRAM_REJECT_DISCARDS:
        *relevant_bit = 1;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_VOQ_TAIL_DROP_DISCARDS:
        *relevant_bit = 2;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_VOQ_STATISTICS_DISCARDS:
        *relevant_bit = 3;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_VSQ_TAIL_DROP_DISCARDS:
        *relevant_bit = 4;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_VSQ_STATISTICS_DISCARDS:
        *relevant_bit = 5;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_QUEUE_NOT_VALID_DISCARD:
        *relevant_bit = 6;
        is_iqm = 1;
        break;
    case SOC_TMC_CNT_OTHER_DISCARDS:
        *relevant_bit = 7;
        is_iqm = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "filter criterion %d is invalid\n"), filter));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    /* set possible engine source types accoridng to info(egq/iqm...) */
    if (engine_source_max_count < 1){
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "insufficant size in engine_source_array\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_FULL);
    }

    if (is_egq) {
        engine_source_array[0] = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
        *engine_source_count = 1;
    } else if (is_iqm) {
        engine_source_array[0] = SOC_TMC_CNT_SRC_TYPE_VOQ;
        *engine_source_count = 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_stat_counter_filter_set
 * Purpose:
 *      set filters (values are filtered out or in to the count) for source (source_type and command_id).
 * Parameters:
 *      unit -  unit number
 *      source - engine source to set filter for (command_id and source type)
 *      filter_array - filters to filter
 *      filter_count - nof members in filter array
 *      is_active - if set activate filters (filter out), else deactivate filters (filter in)
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_stat_counter_filter_set(SOC_SAND_IN int                             unit, 
                                    SOC_SAND_IN SOC_TMC_CNT_SOURCE*             source, 
                                    SOC_SAND_IN SOC_TMC_CNT_FILTER_TYPE*        filter_array, 
                                    SOC_SAND_IN int                             filter_count,
                                    SOC_SAND_IN int                             is_active )
{

    int filter_iter = 0;
    int engine_source_iter = 0;
    int engine_source_count = 0;
    int relevant_bit = 0;
    int found_match = 0;
    int value = is_active ? 0 : 1;
    int command_id = source->command_id;
    SOC_TMC_CNT_SRC_TYPE engine_source = source->source_type;
    SOC_TMC_CNT_SRC_TYPE engine_source_array[SOC_TMC_CNT_NOF_SRC_TYPES_JERICHO];

    SOCDNX_INIT_FUNC_DEFS;

    /* iterate over all filter types */
    for (filter_iter = 0; filter_iter < filter_count; ++filter_iter) 
    {
        /* map filter to relevant engines and bits to change */
        SOCDNX_IF_ERR_EXIT(soc_jer_map_filter_to_bit_and_source_type(unit, filter_array[filter_iter], &relevant_bit, SOC_TMC_CNT_NOF_SRC_TYPES_JERICHO, engine_source_array, &engine_source_count));
        /* iterate over found engines, and try to match to input engine */
        for (engine_source_iter = 0, found_match = 0; engine_source_iter < engine_source_count; ++engine_source_iter) 
        {
            /* continue if not match */
            if (engine_source_array[engine_source_iter] != engine_source) 
            {
                continue;
            }

            found_match = 1;
            /* when finding a match, activate per engine type function */
            if (engine_source == SOC_TMC_CNT_SRC_TYPE_EGR_PP){
                SOCDNX_IF_ERR_EXIT(soc_jer_filter_config_egress_receive_set_get(unit, relevant_bit, command_id, 0, &value));
            } else if (engine_source == SOC_TMC_CNT_SRC_TYPE_VOQ) {
                SOCDNX_IF_ERR_EXIT(soc_jer_filter_config_ingress_set_get(unit, relevant_bit, command_id, 0, &value));
            } else {
                LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "engine source %d doesn't support any filters\n"), engine_source));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);        
            }
            break;
        }

        if (found_match != 1) 
        {
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "engine type %d and filter criterion %d doesn't match invalid\n"), engine_source, filter_array[filter_iter]));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_stat_counter_filter_get
 * Purpose:
 *      get active filters (values are filtered out of the count) for source (source_type and command_id).
 * Parameters:
 *      unit -  unit number
 *      source - engine source to set filter for (command_id and source type)
 *      filter_max_count - size of filter array
 *      filter_array - returned found filters
 *      filter_count - nof members found
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_stat_counter_filter_get(SOC_SAND_IN  int                            unit, 
                                    SOC_SAND_IN  SOC_TMC_CNT_SOURCE*            source, 
                                    SOC_SAND_IN  int                            filter_max_count,
                                    SOC_SAND_OUT SOC_TMC_CNT_FILTER_TYPE*       filter_array, 
                                    SOC_SAND_OUT int*                           filter_count )
{
    int filter_iter = 0;
    int engine_source_iter = 0;
    int engine_source_count = 0;
    int relevant_bit = 0;
    int value;
    int found_filter_index = 0;
    int command_id = source->command_id;
    SOC_TMC_CNT_SRC_TYPE engine_source = source->source_type;
    SOC_TMC_CNT_SRC_TYPE engine_source_array[SOC_TMC_CNT_NOF_SRC_TYPES_JERICHO];

    SOCDNX_INIT_FUNC_DEFS;

    /* iterate over all filter types */
    for (filter_iter = 0; filter_iter < SOC_TMC_CNT_DROP_REASON_COUNT; ++filter_iter) 
    {
        /* map filter to relevant engines and bits to change */
        SOCDNX_IF_ERR_EXIT(soc_jer_map_filter_to_bit_and_source_type(unit, filter_iter, &relevant_bit, SOC_TMC_CNT_NOF_SRC_TYPES_JERICHO, engine_source_array, &engine_source_count));
        /* iterate over found engines, and try to match to input engine */
        for (engine_source_iter = 0; engine_source_iter < engine_source_count; ++engine_source_iter) 
        {
            /* continue if not match */
            if (engine_source_array[engine_source_iter] != engine_source) 
            {
                continue;
            }

            /* when finding a match, activate per engine type function */
            if (engine_source == SOC_TMC_CNT_SRC_TYPE_EGR_PP){
                SOCDNX_IF_ERR_EXIT(soc_jer_filter_config_egress_receive_set_get(unit, relevant_bit, command_id, 1, &value));
            } else if (engine_source == SOC_TMC_CNT_SRC_TYPE_VOQ) {
                SOCDNX_IF_ERR_EXIT(soc_jer_filter_config_ingress_set_get(unit, relevant_bit, command_id, 1, &value));
            } else {
                LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "engine source %d doesn't support any filters\n"), engine_source));
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }

            /* fill returned filter array with filter criterion which is filtered out */
            if (!value) 
            {
                if (found_filter_index < filter_max_count) 
                {
                    filter_array[found_filter_index] = filter_iter;
                    ++found_filter_index;
                } else {
                    LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "insufficant size of filter_array\n")));
                    SOCDNX_IF_ERR_EXIT(SOC_E_FULL);
                }
            }
            break;
        }
    }

    /* update amount of found members */
    *filter_count = found_filter_index;
    
exit:
    SOCDNX_FUNC_RETURN;
}

