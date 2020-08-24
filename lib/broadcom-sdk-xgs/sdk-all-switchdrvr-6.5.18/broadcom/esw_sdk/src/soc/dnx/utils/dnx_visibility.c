/** \file dnx_visibility.c
 *  Miscellaneous routines for managing visibility capabilities in JR2 and further
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/types.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/sand/sand_signals.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_visibility.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/drv.h>

/* *INDENT-OFF* */
static const dnx_visibility_block_map_t dnx_visibility_block_map[DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES] = {
    /*
     * Block index NAME VALIDS DBAL Field ID Direction NOF Lines,SIZE
     */
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA]  = {"IPPA",  DBAL_TABLE_IPPA_SIGNAL_MEMORY,  DBAL_FIELD_IPPA_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPB]  = {"IPPB",  DBAL_TABLE_IPPB_SIGNAL_MEMORY,  DBAL_FIELD_IPPB_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPC]  = {"IPPC",  DBAL_TABLE_IPPC_SIGNAL_MEMORY,  DBAL_FIELD_IPPC_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPD]  = {"IPPD",  DBAL_TABLE_IPPD_SIGNAL_MEMORY,  DBAL_FIELD_IPPD_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPE]  = {"IPPE",  DBAL_TABLE_IPPE_SIGNAL_MEMORY,  DBAL_FIELD_IPPE_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF]  = {"IPPF",  DBAL_TABLE_IPPF_SIGNAL_MEMORY,  DBAL_FIELD_IPPF_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP]  = {"ERPP",  DBAL_TABLE_ERPP_SIGNAL_MEMORY,  DBAL_FIELD_ERPP_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPA] = {"ETPPA", DBAL_TABLE_ETPPA_SIGNAL_MEMORY, DBAL_FIELD_ETPPA_SAMPLE_VALIDS, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPB] = {"ETPPB", DBAL_TABLE_ETPPB_SIGNAL_MEMORY, DBAL_FIELD_ETPPB_SAMPLE_VALIDS, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC] = {"ETPPC", DBAL_TABLE_ETPPC_SIGNAL_MEMORY, DBAL_FIELD_ETPPC_SAMPLE_VALIDS, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
};
/* *INDENT-ON* */
static dnx_visibility_block_info_t *dnx_visibility_block_info[SOC_MAX_NUM_DEVICES][DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES];

/*
 * }
 * End of static variables
 */

/*
 * See dnx_visibility.h
 */
char *
dnx_debug_mem_name(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block)
{
    if ((asic_block < DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA) || (asic_block > DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC))
    {
        return "Unknown block";
    }
    return dnx_visibility_block_map[asic_block].name;
}

/**
 * \brief - Init Debug Signals Cache memory, essentially allocate the memory to debug signals after fetching from HW
 *
 * \param [in] unit - Unit id
 *
 * \return
 *      Standard shr_error_e
 *
 * \remark
 *      Only reason for failure is memory allocation one
 */
static shr_error_e
dnx_debug_cache_init(
    int unit)
{
    int i_bl, i_line;
    int dbal_field_size;
    int buffer_size;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC; i_bl++)
    {
        if (dnx_visibility_block_info[unit][i_bl] != NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of info for block:%s was not cleared properly\n",
                         dnx_visibility_block_map[i_bl].name);
        }
        if ((dnx_visibility_block_info[unit][i_bl] = utilex_alloc(sizeof(dnx_visibility_block_info_t))) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of info for block:%s failed\n",
                         dnx_visibility_block_map[i_bl].name);
        }

        /*
         * Fetch number of lines and width from DNX DATA
         */
        dnx_visibility_block_info[unit][i_bl]->line_nof = dnx_data_debug.mem.params_get(unit, i_bl)->line_nof;
        dnx_visibility_block_info[unit][i_bl]->width = dnx_data_debug.mem.params_get(unit, i_bl)->width;
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_IPPA_SIGNAL_MEMORY, DBAL_FIELD_SIGDATA,
                                               FALSE, 0, 0, &dbal_field_size));

    /*
     * Each core requests memory of its own
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC; i_bl++)
        {
            /*
             * Calculate line size in bytes, rounded to DBAL field max size
             */
            dnx_visibility_block_info[unit][i_bl]->line_size =
                UTILEX_ALIGN_UP(dnx_visibility_block_info[unit][i_bl]->width, dbal_field_size) / 8;
            /*
             * Number of DBAL reads for specific ASIC block per line
             */
            dnx_visibility_block_info[unit][i_bl]->data_nof =
                dnx_visibility_block_info[unit][i_bl]->line_size / BITS2BYTES(dbal_field_size);
            /*
             * Memory size to allocate per block
             */
            buffer_size = dnx_visibility_block_info[unit][i_bl]->line_nof *
                dnx_visibility_block_info[unit][i_bl]->line_size;

            if ((dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx] = utilex_alloc(buffer_size)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of %d for %s block failed\n", buffer_size,
                             dnx_visibility_block_map[i_bl].name);
            }
            for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
            {
                dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core_idx][i_line] = FALSE;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - DeInit Debug Signals Cache memory, essentially allocate the memory to debug signals after fetching from HW
 *
 * \param [in] unit - Unit id
 *
 * \return
 *      Standard shr_error_e
 *
 * \remark
 *      Only reason for failure is memory allocation one
 */
static shr_error_e
dnx_debug_cache_deinit(
    int unit)
{
    int i_bl;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC; i_bl++)
    {
        /*
         * Each core_idx requests memory of its own
         */
        if (dnx_visibility_block_info[unit][i_bl] != NULL)
        {
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
            {
                if (dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx] != NULL)
                {
                    utilex_free(dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx]);
                    dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx] = NULL;
                }
            }
            utilex_free(dnx_visibility_block_info[unit][i_bl]);
            dnx_visibility_block_info[unit][i_bl] = NULL;
        }
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_debug_pema_read(
    int unit,
    int core,
    int direction,
    dbal_enum_value_field_pema_id_e pema_id,
    int window,
    uint32 *value_array)
{
    uint32 entry_handle_id;
    int size;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * First set the window and direction
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_WINDOW_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_ID, pema_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_DIRECTION, INST_SINGLE, direction);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_WINDOW, INST_SINGLE, window);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_DATA, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_ID, pema_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_DATA, DBAL_FIELD_PEMA_DATA,
                                               FALSE, 0, 0, &size));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PEMA_DATA, 0,
                                                            &value_array[0]));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PEMA_DATA, 1,
                                                            &value_array[BITS2WORDS(size)]));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h
 */
shr_error_e
dnx_debug_reg_read(
    int unit,
    int core,
    char *hw_name,
    int size,
    uint32 *value_array)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;
    char **tokens = NULL;
    uint32 realtokens = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify input
     */
    SHR_NULL_CHECK(value_array, _SHR_E_PARAM, "value_array");
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    /*
     * input name and fetch DBAL table and field
     */
    tokens = utilex_str_split(hw_name, ".", 2, &realtokens);
    if (realtokens != 2 || (tokens == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong dbal format:%s for Debug Signal\n", hw_name);
    }

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, tokens[0], &table_id));
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, tokens[1], &field_id));
    /*
     * Now we have table: table_id and field:field_id
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Use the best method per size
     */
    if (size > 32)
    {
        int dbal_field_size;
        /*
         * Max Register field is 2 DBAL fields - consider replacing by loop on size
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, field_id, FALSE, 0, 0, &dbal_field_size));

        if (size > dbal_field_size)
        {   /** Multi-Instance DBAL field size should be always rounded to 32b */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, 0, value_array));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, 1, value_array
                                                                    + BITS2WORDS(dbal_field_size)));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                                    value_array));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, value_array));
    }
exit:
    utilex_str_split_free(tokens, realtokens);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief       IF cache is invalid, read from ASIC data per core per ASIC block
 * \param [in]  unit        - Identifier of HW platform.
 * \param [in]  core        - core ID
 * \param [in]  asic_block  - block id for with we are fetching signals
 * \param [in]  line_index  - number of memory line inside block
 * \return
 *      Standard shr_error
 * \remark
 *      No input verification, because it is internal function, verification is done in dbx_debug_mem_read
 *      Cache is invalidated by dbx_debug_cache_clear
 * \see
 *      dnx_debug_cache_init, dbx_debug_cache_clear, dbx_debug_mem_read
 */
static shr_error_e
dnx_debug_mem_fetch(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index)
{
    int i_data;
    char *data_ptr;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Now we know that cash is invalid and there is new data
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dnx_visibility_block_map[asic_block].table, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINE, line_index);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * data_ptr points to the beginning of line data, inside the loop several dbal fields will be fetched
     */
    data_ptr = dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core] +
        line_index * dnx_visibility_block_info[unit][asic_block]->line_size;
    for (i_data = 0; i_data < dnx_visibility_block_info[unit][asic_block]->data_nof; i_data++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_SIGDATA, i_data,
                                                                (uint32 *) data_ptr));
        /*
         * Progress data pointer to the next portion
         */
        data_ptr += dnx_visibility_block_info[unit][asic_block]->line_size /
            dnx_visibility_block_info[unit][asic_block]->data_nof;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h
 */
/**
 * \brief - Clear memory status, allowing to recognize further changes
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - core ID
 * \param [in] asic_block - ASIC Block ID
 * \param [in] line_index - Memory line inside ASIC block
 *
 * \return
 *      error - per standard shr_error_e
 */
static shr_error_e
dnx_debug_mem_clear(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dnx_visibility_block_map[asic_block].table, &entry_handle_id));

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINE, line_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGRESET, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_line_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index)
{
    uint32 entry_handle_id;
    uint32 block_valids[2] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);
    /*
     * Check that the memory is ready. No need to check readiness in ALWAYS mode, but it does not heart
     * Allows to understand whether there was a packet or not
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (dnx_visibility_block_info[unit][asic_block]->line_nof > 32)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                dnx_visibility_block_map[asic_block].valids_field,
                                                                INST_SINGLE, block_valids));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            dnx_visibility_block_map[asic_block].valids_field,
                                                            INST_SINGLE, block_valids));
    }

    if (((line_index < 32) && ((block_valids[0] & SAL_BIT(line_index)) == 0)) ||
        ((line_index >= 32) && ((block_valids[1] & SAL_BIT(line_index - 32)) == 0)))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    uint32 **data_ptr)
{
    shr_error_e ready_status = _SHR_E_EMPTY;
    bcm_instru_vis_mode_control_t mode;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input
     */
    SHR_NULL_CHECK(data_ptr, _SHR_E_PARAM, "value_array");
    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    if (line_index >= dnx_visibility_block_info[unit][asic_block]->line_nof)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Line Index:%d is out of range(%d) for:%s\n", line_index,
                     dnx_visibility_block_info[unit][asic_block]->line_nof, dnx_debug_mem_name(unit, asic_block));
    }

    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &mode, FALSE));

    /*
     * Check if the cache is already valid
     */
    if (dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] == TRUE)
    {
        /*
         * In "Always" mode we need always to check if memory is ready, may be there were new packets
         * if it is ready we re-fetch - cash is filled with new data
         */
        if ((mode == bcmInstruVisModeAlways) &&
            ((ready_status = dnx_debug_mem_line_is_ready(unit, core, asic_block, line_index)) == _SHR_E_NONE))
        {
            SHR_IF_ERR_EXIT(dnx_debug_mem_fetch(unit, core, asic_block, line_index));
        }
    }
    else if ((ready_status = dnx_debug_mem_line_is_ready(unit, core, asic_block, line_index)) == _SHR_E_NONE)
    {
        /*
         * If cash was empty and now the memory is ready re-fetch the signals
         */
        SHR_IF_ERR_EXIT(dnx_debug_mem_fetch(unit, core, asic_block, line_index));
        dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] = TRUE;
    }

    /*
     * In "Always" mode after ready we always clean the memory to allow us indication
     * whether next time it will be next packet or not
     */
    if ((mode == bcmInstruVisModeAlways) && (ready_status == _SHR_E_NONE))
    {
        SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core, asic_block, line_index));
    }

    if (dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] == TRUE)
    {
        /*
         * Now we just return the pointer and the caller will extract needed information
         */
        *data_ptr = (uint32 *) (dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core] +
                                line_index * dnx_visibility_block_info[unit][asic_block]->line_size);
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_lines(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int *size_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);

    *size_p = dnx_visibility_block_info[unit][asic_block]->line_nof;

exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_cache(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    int *status_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);

    *status_p = dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index];

exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block)
{
    uint32 entry_handle_id;
    uint32 block_valids[2] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);
    /*
     * Check that the memory is ready. No need to check readiness in ALWAYS mode, but it does not heart
     * Allows to understand whether there was a packet or not
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (dnx_visibility_block_info[unit][asic_block]->line_nof > 32)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                dnx_visibility_block_map[asic_block].valids_field,
                                                                INST_SINGLE, block_valids));
        if ((block_valids[0] == 0) && (block_valids[1] == 0))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            dnx_visibility_block_map[asic_block].valids_field,
                                                            INST_SINGLE, block_valids));
        if (block_valids[0] == 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_debug_block_is_ready(
    int unit,
    int core,
    dnx_pp_block_e pp_block)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    {
        match_t match_m;
        rhlist_t *dsig_list = NULL;

        sal_memset(&match_m, 0, sizeof(match_t));

        if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
        }

        match_m.block = dnx_pp_block_name(unit, pp_block);

        SHR_SET_CURRENT_ERR(sand_signal_get(unit, 0, &match_m, dsig_list, NULL));
        if (!SHR_FUNC_VAL_IS(_SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
        sand_signal_list_free(dsig_list);
    }
#else
    {
        dbal_enum_value_field_pp_asic_block_e asic_block;

        switch (pp_block)
        {
            case DNX_PP_BLOCK_IRPP:
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
                break;
            case DNX_PP_BLOCK_ITPP:
                
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
                break;
            case DNX_PP_BLOCK_ERPP:
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
                break;
            case DNX_PP_BLOCK_ETPP:
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPA;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pp block:%d\n", pp_block);
                break;
        }
        SHR_SET_CURRENT_ERR(dnx_debug_mem_is_ready(unit, core, asic_block));
    }
#endif
exit:
    SHR_FUNC_EXIT;
}
/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_resume(
    int unit,
    int core,
    uint32 flags)
{
    int core_idx;
    bcm_instru_vis_mode_control_t mode = -1;
    dbal_enum_value_field_pp_asic_block_e first_block = 0, last_block = 0, i_bl;
    int i_line;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verification is done by underlying routines
     */
    /*
     * Step 1 - obtain current mode
     *          if the mode is not selective - HW resume has no meaning, but we'll proceed anyway to clear cache
     */
    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &mode, FALSE));
    /*
     * Step 2 - Set mode to none if it was selective
     */
    if (mode == bcmInstruVisModeSelective)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, bcmInstruVisModeNone));
    }

    /** Set Range Key Fields */
    if ((flags & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS) && (flags & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS))
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }
    else if (flags & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF;
    }
    else if (flags & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        /*
         * Step 3 - Clear the memory per core
         */
        for (i_bl = first_block; i_bl <= last_block; i_bl++)
        {
            for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
            {
                SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core_idx, i_bl, i_line));
            }
        }
        /*
         * Step 4 - Clear valids and and clear SW cache per core
         */
        for (i_bl = first_block; i_bl <= last_block; i_bl++)
        {
            for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
            {
                dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core_idx][i_line] = FALSE;
            }
            if (flags & DNX_VISIBILITY_RESUME_ZERO)
            {
                sal_memset(dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx], 0,
                           dnx_visibility_block_info[unit][i_bl]->line_nof *
                           dnx_visibility_block_info[unit][i_bl]->line_size);
            }
        }
    }
exit:
    /*
     * Step 4 - Set mode back to what it was if it was SELECTIVE and routine succeeded or failed
     */
    if (mode == bcmInstruVisModeSelective)
    {
        SHR_SET_CURRENT_ERR(dnx_visibility_mode_set(unit, mode));
    }
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_mode_set(
    int unit,
    bcm_instru_vis_mode_control_t mode)
{
    uint32 entry_handle_id;
    int always_status, selective_status;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set the value, pay attention that mode will be always global per device
     */
    switch (mode)
    {
        case bcmInstruVisModeAlways:
            always_status = TRUE;
            selective_status = FALSE;
            break;
        case bcmInstruVisModeSelective:
            always_status = FALSE;
            selective_status = TRUE;
            break;
        case bcmInstruVisModeNone:
            always_status = FALSE;
            selective_status = FALSE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown visibility mode:%d\n", mode);
            break;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_ENABLE,
                                 INST_SINGLE, selective_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPA_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPB_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPC_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPD_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPE_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPF_ALWAYS_ENABLE, INST_SINGLE, always_status);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE,
                                 INST_SINGLE, selective_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE,
                                 INST_SINGLE, selective_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPPA_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPPB_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPPC_ALWAYS_ENABLE, INST_SINGLE, always_status);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_mode_get(
    int unit,
    bcm_instru_vis_mode_control_t * mode_p,
    int full_flag)
{
    uint32 entry_handle_id;
    uint32 always_status = SAL_UINT32_MAX, selective_status = SAL_UINT32_MAX, tmp_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mode_p, _SHR_E_PARAM, "mode_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Fetch separately all the data per ASIC block and verify that it is consistent over all of them,
     * Any inconsistency will result in error
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_ENABLE,
                                                        INST_SINGLE, &tmp_status));
    DISCERN_STATUS(selective_status, tmp_status, "Ingress Selective Status");
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPPA_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
    DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
    /*
     * Now we have samples for both mode. If full was not requested we may skip the rest
     */
    if (full_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPB_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPC_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPD_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPE_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPF_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(selective_status, tmp_status, "Egress Selective Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(selective_status, tmp_status, "Egress Selective Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ERPP_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPPA_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPPB_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPPC_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
    }
    if (selective_status == 1)
    {
        if (always_status == 1)
        {
            /*
             * Both modes enabled is forbidden state and is the result of mis-configuration
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Both Always and Selective Modes are Set\n");
        }
        *mode_p = bcmInstruVisModeSelective;
    }
    else if (always_status == 1)
    {
        *mode_p = bcmInstruVisModeAlways;
    }
    else
    {
        *mode_p = bcmInstruVisModeNone;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_sampling_set(
    int unit,
    uint32 direction,
    uint32 period)
{
    uint32 entry_handle_id;
    int enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_PP_VERIFY_DIRECTION(unit, direction);
    /*
     * Period value 0 serves as implicit sampling disable
     */
    if (period != 0)
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    /*
     * Sampling may be configured differently per direction, just as port may be configured differently
     */
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_ENABLE,
                                     INST_SINGLE, enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_PERIOD,
                                     INST_SINGLE, period);
    }

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_ENABLE,
                                     INST_SINGLE, enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_PERIOD,
                                     INST_SINGLE, period);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_ENABLE,
                                     INST_SINGLE, enable);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_PERIOD,
                                     INST_SINGLE, period);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_sampling_get(
    int unit,
    uint32 direction,
    uint32 *period_p)
{
    uint32 entry_handle_id;
    uint32 status = SAL_UINT32_MAX, tmp_status;
    uint32 period = SAL_UINT32_MAX, tmp_period;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(period_p, _SHR_E_PARAM, "period_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * We verify that sampling is unified over the direction or both directions
     * The request for both direction means that they both should be configured the same
     */
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(status, tmp_status, "Sampling Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_PERIOD, INST_SINGLE, &tmp_period));
        DISCERN_STATUS(period, tmp_period, "Sampling Period");
    }

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(status, tmp_status, "Sampling Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_PERIOD, INST_SINGLE, &tmp_period));
        DISCERN_STATUS(period, tmp_period, "Sampling Period");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(status, tmp_status, "Sampling Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_PERIOD, INST_SINGLE, &tmp_period));
        DISCERN_STATUS(period, tmp_period, "Sampling Period");
    }

    if (status == 0)
        *period_p = 0;
    else
        *period_p = period;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_pp_port_enable_set(
    int unit,
    int core,
    int pp_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verify enable
     */
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Enable visibility for egress port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_pp_port_enable_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Get all field */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get type from entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE,
                                                        INST_SINGLE, &enable));
    *enable_p = enable;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable port visibility per tm_port - Egress
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] tm_port - pp_port id 8b
 * \param [in] enable - command to enable(TRUE) or disable(FALSE) port
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_port_enable_get
 */
static shr_error_e
dnx_visibility_tm_port_enable_set(
    int unit,
    int core,
    int tm_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verify enable
     */
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }
    /*
     * Write to EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Enable visibility for egress port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable port visibility per tm_port - Egress
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] tm_port - pp_port id 8b
 * \param [in] enable_p - command to enable(TRUE) or disable(FALSE) port
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_port_enable_get
 */
static shr_error_e
dnx_visibility_tm_port_enable_get(
    int unit,
    int core,
    int tm_port,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 erpp_enable = 0, etpp_enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Read from EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Enable visibility for egress port */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, &erpp_enable));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, &etpp_enable));

    DISCERN_STATUS(etpp_enable, erpp_enable, "ERPP and ETPP visibility Enable statues are Inconsistent");

    *enable_p = erpp_enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_port_enable_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        /** Egress call cannot be Trunk, as the API configure a single tm port for Egress direction */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        uint32 pp_port_index;
        /*
         * For ingress we fetch pp port
         */
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit,
                                                              gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                              gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                              enable));
        }
    }
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        uint32 tm_port;
        dnx_algo_port_info_s port_info;
        bcm_core_t core;
        /*
         * for egress we fetch tm_port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core, &tm_port));
            SHR_IF_ERR_EXIT(dnx_visibility_tm_port_enable_set(unit, core, tm_port, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_port_enable_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int enable = SAL_UINT32_MAX, cur_enable;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        /** Egress call cannot be Trunk, as the API configure a single tm port for Egress direction */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));

    /*
     * Get INGRESS PORT visibility status
     */
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        uint32 pp_port_index;
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_get(unit,
                                                              gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                              gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                              &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress Port Enable Status");
        }
    }
    /*
     * Get EGRESS PORT visibility status
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        uint32 tm_port;
        dnx_algo_port_info_s port_info;
        bcm_core_t core;
        /*
         * for egress we fetch tm_port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core, &tm_port));
            SHR_IF_ERR_EXIT(dnx_visibility_tm_port_enable_get(unit, core, tm_port, &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress and Egress visibility enable Status are inconsistent");
        }
    }
    *enable_p = enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_pp_port_force_set(
    int unit,
    int core,
    int pp_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verify enable
     */
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Enable visibility for egress port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_pp_port_force_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Get all field */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Get type from entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE,
                                                        INST_SINGLE, &enable));

    *enable_p = enable;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
static shr_error_e
dnx_visibility_tm_port_force_set(
    int unit,
    int core,
    int tm_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verify enable
     */
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }
    /*
     * Write to EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /** Enable visibility for egress port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
static shr_error_e
dnx_visibility_tm_port_force_get(
    int unit,
    int core,
    int tm_port,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 erpp_force = 0, etpp_force = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Read from EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    /** Set key fields - port and core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Enable visibility for egress port */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, &erpp_force));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, &etpp_force));

    DISCERN_STATUS(etpp_force, erpp_force, "ERPP and ETPP visibility Force statues are Inconsistent");

    *enable_p = erpp_force;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_port_force_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        /** Egress call cannot be Trunk, as the API configure a single tm port for Egress direction */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        uint32 pp_port_index;
        /*
         * For ingress we fetch pp port
         */
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_force_set(unit,
                                                             gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                             gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                             enable));
        }
    }
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        uint32 tm_port;
        dnx_algo_port_info_s port_info;
        bcm_core_t core;

        /*
         * for egress we fetch tm_port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core, &tm_port));
            SHR_IF_ERR_EXIT(dnx_visibility_tm_port_force_set(unit, core, tm_port, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_port_force_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    int enable = SAL_UINT32_MAX, cur_enable = SAL_UINT32_MAX;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        /** Egress call cannot be Trunk, as the API configure a single tm port for Egress direction */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));
    /*
     * Get INGRESS PORT visibility status
     */
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_force_get(unit,
                                                             gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                             gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                             &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress and Egress visibility enable Status are inconsistent");
        }
    }
    /*
     * Get EGRESS PORT visibility status
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        bcm_core_t core;
        uint32 tm_port;
        dnx_algo_port_info_s port_info;
        /*
         * for egress we fetch tm_port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core, &tm_port));
            SHR_IF_ERR_EXIT(dnx_visibility_tm_port_force_get(unit, core, tm_port, &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress Port Force Status");
        }
    }
    *enable_p = enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
/**
 * \brief - Enable visibility for the packets with specific PRT qualifier
 *
 * \param [in] unit - Unit id
 * \param [in] prt_qualifier - prt qualifier enabling visibility
 * \param [in] enable - command to enable(TRUE) or disable(FALSE) prt qualifier
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_prt_qualifier_get
 */
static shr_error_e
dnx_visibility_prt_qualifier_set(
    int unit,
    int prt_qualifier,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Verify enable
     */
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_QUALIFIER_INFO, &entry_handle_id));
    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, prt_qualifier);
    /** Set vlan_domain */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_prt_qualifier_get(
    int unit,
    uint32 *prt_qualifier_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_PRT_QUALIFIER,
                                                  DBAL_ENUM_FVAL_PRT_QUALIFIER_SET_VISIBILITY, prt_qualifier_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h file
 */
shr_error_e
dnx_visibility_port_egress_enable(
    int unit)
{
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, port, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h file
 */
shr_error_e
dnx_visibility_port_egress_disable(
    int unit)
{
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, port, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_visibility_port_init_all(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to INGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields - port range and core*/
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Disable visibility and unforce for all ingress ports */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE, INST_SINGLE, FALSE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Write to EGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Disable visibility and unforce for all egress ports */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, FALSE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - global visibility set
 *
 * \param [in] unit - unit id
  * \param [in] is_enable - enable/disable visibility
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_visibility_global_set(
    int unit,
    int is_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to GLOBAL_VISIBILITY_SET table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /*
     * enable visibility
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_GLOBAL_ENABLE, INST_SINGLE,
                                 is_enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing visibility in emulation to make init work%s%s%s", EMPTY, EMPTY,
                          EMPTY);
    }
#endif
    /*
     * Allocate and init memory for signal cache - only operation to be done always even for WB - the rest only for cold
     */
    SHR_IF_ERR_EXIT(dnx_debug_cache_init(unit));
    /*
     * Disable sampling - set zero period
     */
    if (!SOC_WARM_BOOT(unit))
    {
        int i_qual;
        int enable;
        int core_idx;
        /*
         * Make sure all ports are disabled on cold boot
         */
        SHR_IF_ERR_EXIT(dnx_visibility_port_init_all(unit));
        /*
         * Establish Selective mode as default for both directions
         */
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, bcmInstruVisModeSelective));

        SHR_IF_ERR_EXIT(dnx_visibility_sampling_set(unit,
                                                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS |
                                                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, 0));
        /*
         * Disable all qualifiers besides visbility one
         */
        for (i_qual = 0; i_qual < DBAL_NOF_ENUM_PRT_QUALIFIER_VALUES; i_qual++)
        {
            if (i_qual == DBAL_ENUM_FVAL_PRT_QUALIFIER_SET_VISIBILITY)
                enable = TRUE;
            else
                enable = FALSE;

            SHR_IF_ERR_EXIT(dnx_visibility_prt_qualifier_set(unit, i_qual, enable));
        }

        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            /*
             * Step 3 - Clear the memory per core, per block
             */
            int i_bl;
            for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC; i_bl++)
            {
                int i_line;
                for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
                {
                    SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core_idx, i_bl, i_line));
                }
            }
        }

        if (dnx_data_debug.general.global_visibility_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_visibility_global_set(unit, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see dnx_visibility.h
 */
shr_error_e
dnx_visibility_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_debug_cache_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
