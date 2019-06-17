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

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/* *INDENT-OFF* */
static const dnx_visibility_block_map_t dnx_visibility_block_map[DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES] = {
    /*
     * Block index NAME VALIDS DBAL Field ID Direction NOF Lines,SIZE
     */
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA]  = {"IPPA",  DBAL_FIELD_IPPA_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPB]  = {"IPPB",  DBAL_FIELD_IPPB_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPC]  = {"IPPC",  DBAL_FIELD_IPPC_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPD]  = {"IPPD",  DBAL_FIELD_IPPD_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPE]  = {"IPPE",  DBAL_FIELD_IPPE_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF]  = {"IPPF",  DBAL_FIELD_IPPF_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP]  = {"ERPP",  DBAL_FIELD_ERPP_SAMPLE_VALIDS,  BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPA] = {"ETPPA", DBAL_FIELD_ETPPA_SAMPLE_VALIDS, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPB] = {"ETPPB", DBAL_FIELD_ETPPB_SAMPLE_VALIDS, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
    [DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC] = {"ETPPC", DBAL_FIELD_ETPPC_SAMPLE_VALIDS, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS},
};
/* *INDENT-ON* */
static dnx_visibility_block_info_t *dnx_visibility_block_info[SOC_MAX_NUM_DEVICES][DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES];

/*
 * Mode is set to DNX_VISIBILITY_ALWAYS on init and modified by application
 */
static dnx_visibility_status_t dnx_visibility_status[SOC_MAX_NUM_DEVICES] =
    { {bcmInstruVisModeNone, {FALSE, FALSE}, {FALSE, FALSE}}, };
/*
 * }
 * End of static variables
 */

/**
 * \brief       Set cache status
 * \param [in]  unit           - device ID
 * \param [in]  mc_core        - core ID
 * \param [in]  mc_direction   - Cache status may be set per direction or both
 * \param [in]  mc_status      - status to be set
 * \see
 *      dnx_debug_cache_init, dbx_debug_mem_fetch
 */
#define DEBUG_CACHE_STATUS_SET(unit, mc_core, mc_direction, mc_status)                  \
    DNX_PP_VERIFY_DIRECTION(unit, direction);                                           \
    if (mc_direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)                       \
    {                                                                                   \
        dnx_visibility_status[unit].ing_sig_cash_status[mc_core] = mc_status;           \
    }                                                                                   \
    if (mc_direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)                        \
    {                                                                                   \
        dnx_visibility_status[unit].eg_sig_cash_status[mc_core] = mc_status;            \
    }

/**
 * \brief       Get cache status
 * \param [in]  unit           - device ID
 * \param [in]  mc_core        - core ID
 * \param [in]  mc_direction   - Cache status may be fetched per direction only
 * \param [in]  mc_status      - Cache status variable to be assigned with the requested status
 * \see
 *      dnx_debug_cache_init, dbx_debug_mem_fetch
 */
#define DEBUG_CACHE_STATUS_GET(unit, mc_core, mc_direction, mc_status)                  \
    DNX_PP_VERIFY_DIRECTION(unit, direction);                                           \
    if (mc_direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)                       \
    {                                                                                   \
        mc_status = dnx_visibility_status[unit].ing_sig_cash_status[mc_core];           \
    }                                                                                   \
    else if (mc_direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)                   \
    {                                                                                   \
        mc_status = dnx_visibility_status[unit].eg_sig_cash_status[mc_core];            \
    }                                                                                   \

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
    int i_bl;
    int buffer_size;
    int core;
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

    /*
     * Each core requests memory of its own
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC; i_bl++)
        {
            /*
             * Calculate line size in bytes, rounded to DBAL field max size
             */
            dnx_visibility_block_info[unit][i_bl]->line_size =
                UTILEX_ALIGN_UP(dnx_visibility_block_info[unit][i_bl]->width, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS) / 8;
            /*
             * Memory size to allocate per block
             */
            buffer_size = dnx_visibility_block_info[unit][i_bl]->line_nof *
                dnx_visibility_block_info[unit][i_bl]->line_size;

            if ((dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core] = utilex_alloc(buffer_size)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of %d for %s block failed\n", buffer_size,
                             dnx_visibility_block_map[i_bl].name);
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
    int core;
    SHR_FUNC_INIT_VARS(unit);

    for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC; i_bl++)
    {
        /*
         * Each core requests memory of its own
         */
        if (dnx_visibility_block_info[unit][i_bl] != NULL)
        {
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                if (dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core] != NULL)
                {
                    utilex_free(dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core]);
                    dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core] = NULL;
                }
            }
            utilex_free(dnx_visibility_block_info[unit][i_bl]);
            dnx_visibility_block_info[unit][i_bl] = NULL;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Nullify Debug Signals Cache memory
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] direction - PP Direction (Ingree/Egress or both)
 *
 * \return
 *      Standard shr_error_e
 *
 * \remark
 *      Only reason for failure is memory allocation one
 */
static shr_error_e
dnx_debug_cache_nullify(
    int unit,
    int core,
    uint32 direction)
{
    dbal_enum_value_field_pp_asic_block_e first_block = 0, last_block = 0, i_bl;

    SHR_FUNC_INIT_VARS(unit);

    /** Set Range Key Fields */
    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
        && (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS))
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }
    else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF;
    }
    else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }

    /*
     * Each core requests memory of its own
     */
    for (i_bl = first_block; i_bl <= last_block; i_bl++)
    {
        SHR_NULL_CHECK(dnx_visibility_block_info[unit][i_bl], _SHR_E_PARAM, "dnx_visibility_block_info");

        sal_memset(dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core], 0,
                   dnx_visibility_block_info[unit][i_bl]->line_nof * dnx_visibility_block_info[unit][i_bl]->line_size);
    }

exit:
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
        /*
         * Max Register field is 2 DBAL fields - consider replacing by loop on size
         */
        if (size > DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, 0, value_array));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, 1, value_array
                                                                    + DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, field_id, INST_SINGLE, value_array));
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
 * \param [in]  direction   - direction for with we are fetching signals
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
    uint32 direction)
{
    int i_data, data_nof, i_line;
    dbal_enum_value_field_pp_asic_block_e i_bl;
    char *data_ptr;
    uint32 entry_handle_id;
    int retry_count;
    int status = FALSE;
    bcm_instru_vis_mode_control_t mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &mode, FALSE));
    /*
     * Check if the cache is already valid
     */

    DEBUG_CACHE_STATUS_GET(unit, core, direction, status);
    if (status == TRUE)
    {
        /*
         * If the mode is always we need to check if memory is ready -
         * if it is ready we invalidate relevant cache and re-fetch
         */
        if ((mode == bcmInstruVisModeAlways) &&
            (dnx_debug_direction_is_ready(unit, core, direction, DNX_VISIBILITY_BLOCK_UNDEFINED) == _SHR_E_NONE))
        {
            DEBUG_CACHE_STATUS_SET(unit, core, direction, FALSE);
        }
        else
        {
            /*
             * Data cached
             */
            SHR_EXIT();
        }
    }
    else if ((dnx_debug_direction_is_ready(unit, core, direction, DNX_VISIBILITY_BLOCK_UNDEFINED) != _SHR_E_NONE))
    {
        /*
         * Nothing changes if there is no cache and direction is not ready
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DEBUG_SIGNAL_MEMORY, &entry_handle_id));

    for (i_bl = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; i_bl < DBAL_NOF_ENUM_PP_ASIC_BLOCK_VALUES; i_bl++)
    {
        if (direction != dnx_visibility_block_map[i_bl].direction)
            continue;

        SHR_NULL_CHECK(dnx_visibility_block_info[unit][i_bl], _SHR_E_PARAM, "dnx_visibility_block_info");

        retry_count = DNX_VISIBILITY_RETRY_READY_COUNT;

        while (retry_count--)
        {
            SHR_SET_CURRENT_ERR(dnx_debug_mem_is_ready(unit, core, i_bl));
            if (!SHR_FUNC_VAL_IS(_SHR_E_EMPTY))
                break;
            else
            {
                sal_usleep(1000);
            }
        }

        if (SHR_FUNC_VAL_IS(_SHR_E_EMPTY))
        {
            /*
             * LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Memory for %s is not ready on %d retries\n"),
             * dnx_visibility_block_info[asic_block].name, DNX_VISIBILITY_RETRY_READY_COUNT)); Ignore this error for now
             */
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ASIC_BLOCK, i_bl);
        /*
         * All the line are read with the same max width, signal mechanism will know the real data places
         */
        for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINE, i_line);

            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            /*
             * data_ptr points to the beginning of line data, inside the loop several dbal fields will be fetched
             */
            data_ptr = dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core] +
                i_line * dnx_visibility_block_info[unit][i_bl]->line_size;
            data_nof = dnx_visibility_block_info[unit][i_bl]->line_size / DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES;
            for (i_data = 0; i_data < data_nof; i_data++)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_SIGDATA, i_data,
                                 (uint32 *) (data_ptr + i_data * DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES)));
            }
        }
    }

    if (mode == bcmInstruVisModeAlways)
    {
        SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core, direction));
    }
    DEBUG_CACHE_STATUS_SET(unit, core, direction, TRUE);

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
    /*
     * This call will check if memory is cached will return promptly, if not fetch the memory
     * Actual per signal read is always from cache
     */
    SHR_IF_ERR_EXIT(dnx_debug_mem_fetch(unit, core, dnx_visibility_block_map[asic_block].direction));
    /*
     * Now we just return the pointer and the caller will extract needed information
     */
    if (((line_index < 32) && ((dnx_visibility_block_info[unit][asic_block]->valids_data[core][0] &
                                SAL_BIT(line_index)) == 0))
        || ((line_index >= 32)
            && (dnx_visibility_block_info[unit][asic_block]->valids_data[core][1] & SAL_BIT(line_index - 32)) == 0))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
    }
    else
    {
        *data_ptr = (uint32 *) (dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core] +
                                line_index * dnx_visibility_block_info[unit][asic_block]->line_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_clear(
    int unit,
    int core,
    uint32 direction)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_pp_asic_block_e first_block = 0, last_block = 0, i_bl;
    int i_line;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_PP_VERIFY_DIRECTION(unit, direction);

    /** Set Range Key Fields */
    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
        && (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS))
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }
    else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF;
    }
    else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DEBUG_SIGNAL_MEMORY, &entry_handle_id));

    if (!SOC_WARM_BOOT(unit))
    {
        for (i_bl = first_block; i_bl <= last_block; i_bl++)
        {
            SHR_NULL_CHECK(dnx_visibility_block_info[unit][i_bl], _SHR_E_PARAM, "dnx_visibility_block_info");

            for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ASIC_BLOCK, i_bl);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINE, i_line);

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SIGRESET, INST_SINGLE, 0);

                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DEBUG_SIGNAL_MEMORY, entry_handle_id));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See dnx_visibility.h
 */
shr_error_e
dnx_debug_mem_valids_clear(
    int unit,
    int core,
    uint32 direction)
{
    dbal_enum_value_field_pp_asic_block_e first_block = 0, last_block = 0, i_bl;
    SHR_FUNC_INIT_VARS(unit);

    DNX_PP_VERIFY_DIRECTION(unit, direction);

    /** Set Range Key Fields */
    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
        && (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS))
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }
    else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF;
    }
    else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
        last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
    }

    if (!SOC_WARM_BOOT(unit))
    {
        for (i_bl = first_block; i_bl <= last_block; i_bl++)
        {
            SHR_NULL_CHECK(dnx_visibility_block_info[unit][i_bl], _SHR_E_PARAM, "dnx_visibility_block_info");

            dnx_visibility_block_info[unit][i_bl]->valids_data[core][0] = 0;
            dnx_visibility_block_info[unit][i_bl]->valids_data[core][0] = 1;
        }
    }

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
    uint32 field_size;
    uint32 entry_handle_id;
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

    field_size = dnx_visibility_block_info[unit][asic_block]->line_nof;

    if (field_size > 32)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                dnx_visibility_block_map[asic_block].valids_field,
                                                                INST_SINGLE,
                                                                dnx_visibility_block_info[unit][asic_block]->valids_data
                                                                [core]));
        if ((dnx_visibility_block_info[unit][asic_block]->valids_data[core][0] == 0)
            && (dnx_visibility_block_info[unit][asic_block]->valids_data[core][1] == 0))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            dnx_visibility_block_map[asic_block].valids_field,
                                                            INST_SINGLE,
                                                            dnx_visibility_block_info[unit][asic_block]->valids_data
                                                            [core]));
        if (dnx_visibility_block_info[unit][asic_block]->valids_data[core][0] == 0)
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
dnx_debug_direction_is_ready(
    int unit,
    int core,
    uint32 direction,
    dbal_enum_value_field_pp_asic_block_e block)
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

        if (direction == BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
        {
            match_m.block = "IRPP";
        }
        else if (direction == BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
        {
            match_m.block = "ERPP";
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Direction:%d readiness cannot be stated\n", direction);
        }
        SHR_SET_CURRENT_ERR(sand_signal_list_get(unit, 0, &match_m, dsig_list));
        if (!SHR_FUNC_VAL_IS(_SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }
#else
    {
        /*
         * For ASIC the criteria whether direction is ready or not is based on valid bits for debug memory
         * If all bits are 0 - direction is invalid, if even some of them are not null - certain signals were recorded
         * If the entire block is empty - direction is not ready
         */
        dbal_enum_value_field_pp_asic_block_e i_bl, first_block, last_block;
        if (block != DNX_VISIBILITY_BLOCK_UNDEFINED)
        {
            first_block = last_block = block;
        }
        else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
        {
            
            first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF;
            last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPF;
        }
        else if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
        {
            /*
             * We'll consider direction ready even if only ERPP will be ready, the fact that ETPPx are not ready will
             * be consider as if some memory access is not ready.
             * Obviously there is no option for ETPP to be ready if ERPP is not, so we are safe here
             */
            first_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
            last_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
        }
        else
        {
            /*
             * Else is register virtual block that is always ready
             */
            SHR_EXIT();
        }
        for (i_bl = first_block; i_bl <= last_block; i_bl++)
        {
            SHR_SET_CURRENT_ERR(dnx_debug_mem_is_ready(unit, core, i_bl));
            if (!SHR_FUNC_VAL_IS(_SHR_E_NONE))
                break;
        }
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
    uint32 direction,
    int flags)
{
    int core_idx;
    bcm_instru_vis_mode_control_t mode = -1;
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

    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        /*
         * Step 3 - Clear the memory per core - only in selective mode it is critical - in always it is indicative
         */
        if ((flags & DNX_VISIBILITY_RESUME_MEM) && (mode == bcmInstruVisModeSelective))
        {
            SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core_idx, direction));
        }
        SHR_IF_ERR_EXIT(dnx_debug_mem_valids_clear(unit, core_idx, direction));
        /*
         * Step 4 - Clear SW cache per core
         */
        DEBUG_CACHE_STATUS_SET(unit, core_idx, direction, FALSE);
        if (flags & DNX_VISIBILITY_RESUME_ZERO)
        {
            SHR_IF_ERR_EXIT(dnx_debug_cache_nullify(unit, core_idx, direction));
        }

    }
exit:
    /*
     * Step 4 - Set mode back to what it was if it was SELECTIVE and routine succeeded or failed
     */
    if (mode == bcmInstruVisModeSelective)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, mode));
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

    if (!SOC_WARM_BOOT(unit))
    {
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
    }
    dnx_visibility_status[unit].mode = mode;

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
    bcm_gport_t gport = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mode_p, _SHR_E_PARAM, "mode_p");

    if (!SOC_WARM_BOOT(unit))
    {
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
    bcm_gport_t gport = 0;

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
    uint32 *enable_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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
                                                        INST_SINGLE, enable_p));

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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));

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
        bcm_port_t logical_port;
        uint32 tm_port;
        dnx_algo_port_type_e port_type;
        bcm_core_t core;
        /*
         * for egress we fetch tm_port 
         */
        BCM_PBMP_ITER(gport_info.local_port_bmp, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
            if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
                SHR_IF_ERR_EXIT(dnx_visibility_tm_port_enable_set(unit, core, tm_port, enable));
            }
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
    uint32 enable = SAL_UINT32_MAX, cur_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));
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
        bcm_port_t logical_port;
        uint32 tm_port;
        dnx_algo_port_type_e port_type;
        bcm_core_t core;
        /*
         * for egress we fetch tm_port 
         */
        BCM_PBMP_ITER(gport_info.local_port_bmp, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
            if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
            }

            /** Set key fields - port and core*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            /*
             * Get field values from entry
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE,
                                                                &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "ERPP Port Enable Status");
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE,
                                                                &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "ETPP Port Enable Status");
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
    uint32 *enable_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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
                                                        INST_SINGLE, enable_p));

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
shr_error_e
dnx_visibility_port_force_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));

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
        bcm_port_t logical_port;
        uint32 tm_port;
        dnx_algo_port_type_e port_type;
        bcm_core_t core;
        /*
         * for egress we fetch tm_port 
         */
        BCM_PBMP_ITER(gport_info.local_port_bmp, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
            if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
                SHR_IF_ERR_EXIT(dnx_visibility_tm_port_force_set(unit, core, tm_port, enable));
            }
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
    uint32 enable = SAL_UINT32_MAX, cur_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify input
     */
    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &gport_info));
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
            DISCERN_STATUS(enable, cur_enable, "Ingress Port Force Status");
        }
    }
    /*
     * Get EGRESS PORT visibility status
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, &entry_handle_id));
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        bcm_port_t logical_port;
        uint32 tm_port;
        dnx_algo_port_type_e port_type;
        bcm_core_t core;
        /*
         * for egress we fetch tm_port 
         */
        BCM_PBMP_ITER(gport_info.local_port_bmp, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
            if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, &tm_port));
            }
            /** Set key fields - port and core*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            /*
             * Get field values from entry
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE,
                                                                &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "ERPP Port Force Status");
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                                DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE,
                                                                &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "ETPP Port Force Status");
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
    int prt_qualifier,
    int *enable_p)
{
    uint32 enable_data;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_QUALIFIER_INFO, &entry_handle_id));
    /** Set key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, prt_qualifier);
    /** Set vlan_domain */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE,
                                                        INST_SINGLE, &enable_data));
    *enable_p = enable_data;
exit:
    DBAL_FUNC_FREE_VARS;
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
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

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
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    SHR_IF_ERR_EXIT(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, port, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_visibility_port_init_all(
    int unit)
{
    uint32 entry_handle_id;
    bcm_core_t core_id;
    uint32 pp_port;
    bcm_pbmp_t logical_pbmp;
    bcm_pbmp_t pbmp_pp_arr[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(logical_pbmp);
    /*
     * Write to INGRESS PORT table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    /** Set key fields - port range and core*/
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 0, 255);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Disable visibility and unforce for ingress port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE, INST_SINGLE, FALSE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Write to EGRESS PORT table
     */

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &logical_pbmp));
    SHR_IF_ERR_EXIT(dnx_algo_port_logical_pbmp_to_pp_pbmp_get(unit, logical_pbmp, pbmp_pp_arr));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_TM_PORT_TABLE, entry_handle_id));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_ITER(pbmp_pp_arr[core_id], pp_port)
        {
            /** Set key fields - port and core */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, pp_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            /** Disable visibility and unforce for egress port */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, FALSE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, FALSE);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
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
dnx_visibility_init(
    int unit)
{
    int i_qual;
    int enable;

    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing visibility in emulation to make init work%s%s%s", EMPTY, EMPTY,
                          EMPTY);
    }
#endif
    /*
     * Allocate and init memory for signal cache
     */
    SHR_IF_ERR_EXIT(dnx_debug_cache_init(unit));
    /*
     * Establish ALWAYS mode for both directions
     */
    SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, bcmInstruVisModeSelective));
    /*
     * Disable sampling - set zero period
     */
    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dnx_visibility_sampling_set(unit,
                                                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS |
                                                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, 0));
        /*
         * Disable all qualifiers
         */
        for (i_qual = 0; i_qual < DBAL_NOF_ENUM_PRT_QUALIFIER_VALUES; i_qual++)
        {
            if ((i_qual & DNX_VISIBILITY_PRT_QUAL_MASK) == DNX_VISIBILITY_PRT_QUAL_MASK)
                enable = TRUE;
            else
                enable = FALSE;

            SHR_IF_ERR_EXIT(dnx_visibility_prt_qualifier_set(unit, i_qual, enable));
        }
    }
    /*
     * Clear memory
     */
    SHR_IF_ERR_EXIT(dnx_visibility_resume(unit, _SHR_CORE_ALL,
                                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS |
                                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, DNX_VISIBILITY_RESUME_MEM));

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
