/** \file src/bcm/dnx/field/field_init.c
 * $Id$
 *
 * Field init procedures for DNX.
 *
 * The file contains all procedures needed for field initialization.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_init.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_key.h>

#include <bcm_int/dnx/field/tcam/field_tcam.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>

#include <bcm_int/dnx/algo/field/algo_field.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#ifdef DNX_EMULATION_1_CORE
#include <soc/sand/sand_aux_access.h>
#endif

/* { */
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_prefix_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
/* } */
/*
 * }
 */

/*
 * See header on fiewld_init.h
 */
shr_error_e
dnx_field_init_sw_state(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     *  No SW state initialization when we are in WARM BOOT mode.
     */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))
    {
#endif /* BCM_WARM_BOOT_SUPPORT */

        /**Init SW of Field MAP module*/
        SHR_IF_ERR_EXIT(dnx_field_map_sw_state_init(unit));

        /**Init SW of Field Context module*/
        SHR_IF_ERR_EXIT(dnx_field_context_sw_state_init(unit));

        /**Init SW of Field Apptype module*/
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_sw_state_init(unit));

        /**Init SW of Field Group Database module*/
        SHR_IF_ERR_EXIT(dnx_field_group_sw_state_init(unit));

        /** Init SW state of TCAM */
        SHR_IF_ERR_EXIT(dnx_field_tcam_sw_state_init(unit));

        /** Init SW state of KEY */
        SHR_IF_ERR_EXIT(dnx_field_key_sw_state_init(unit));

        /** Init SW state of FEM */
        SHR_IF_ERR_EXIT(dnx_field_all_fems_sw_state_init(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header on fiewld_init.h
 */
shr_error_e
dnx_field_deinit_sw_state(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#if (DNX_FIELD_INIT_FEM_TABLES_FAST)
/* { */
/**
 * \brief
 *  Init 'action' on all IPPC_FEM_*_MAP tables
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This initialization loads all FEM actions by DNX_FIELD_ACTION_ID_INVALID
 *     Strictly, this may not be required since, by default, all
 *     FEMs are set to 'IPPC_FEM_MAP_INDEX_TABLE.VALID = 0'
 * \see
 *   * None
 */
static shr_error_e
dnx_field_init_fem_action_type_map_table(
    int unit)
{
    dbal_tables_e table_name;
    uint32 entry_handle_id;
    unsigned int fem_id_max;
    unsigned int fem_index_max;
    unsigned int action_init_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_TABLE_FIELD_PMF_A_FEM_ACTION_TYPE_MAP;
    /*
     * Maximum number of FEMs in iPMF1/2 == 16
     * The value returned by dbal_tables_field_predefine_value_get() is the maximal value
     * which is 15, in this case.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_name, DBAL_FIELD_FIELD_FEM_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &fem_id_max));
    /*
     * Maximum number of FEM index in iPMF1/2 == 4
     * The value returned by dbal_tables_field_predefine_value_get() is the maximal value
     * which is 3, in this case.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_name, DBAL_FIELD_FIELD_FEM_MAP_INDEX, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     &fem_index_max));
    /*
     * We initialize the 'action valid' flag to 'invalid'
     * The actual value written into the HW is DNX_FIELD_ACTION_ID_INVALID
     */
    action_init_val = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    {
        /*
         * key construction
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, 0, fem_id_max);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_MAP_INDEX, 0, fem_index_max);
        /*
         * Set the values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPMF1_ACTION, INST_SINGLE, action_init_val);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRUN));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Init FEM mapIndex table.
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This initialization may not be required since, by default, all
 *     FEMs are set to 'IPPC_FEM_MAP_INDEX_TABLE.VALID = 0'
 * \see
 *   * None
 */
static shr_error_e
dnx_field_init_fem_map_index_table(
    int unit)
{
    dbal_tables_e table_name;
    uint32 entry_handle_id;
    unsigned int fem_id_max;
    unsigned int fem_pgm_max;
    uint32 fem_condition_ndx_max;
    dnx_field_fem_action_valid_t valid_init_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_TABLE_FIELD_PMF_A_FEM_MAP_INDEX;
    /*
     * Maximum number of FEMs in iPMF1/2 == 16
     * The value returned by dbal_tables_field_predefine_value_get() is the maximal value
     * which is 15, in this case.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_name, DBAL_FIELD_FIELD_FEM_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &fem_id_max));
    /*
     * Maximum number of FEM programs in iPMF1/2 == 4
     * The value returned by dbal_tables_field_predefine_value_get() is the maximal value
     * which is 3, in this case.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_name, DBAL_FIELD_FIELD_FEM_PGM_ID, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     &fem_pgm_max));
    /*
     * Maximum number of combinations for 'condition' in iPMF1/2 == 4
     * The value returned by dbal_tables_field_predefine_value_get() is the maximal value
     * which is 15, in this case.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_name, DBAL_FIELD_FIELD_FEM_CONDITION, TRUE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &fem_condition_ndx_max));
    /*
     * We initialize the 'action valid' flag to 'invalid'
     */
    valid_init_val = (dnx_field_fem_action_valid_t) DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    {
        /*
         * key construction
         */
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, 0, fem_id_max);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID, 0, fem_pgm_max);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_CONDITION, 0,
                                         fem_condition_ndx_max);
        /*
         * Set the values
         */
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ACTION_VALID, INST_SINGLE,
                                    valid_init_val);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRUN));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Init FEM bit select table.
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_init_fem_condition_ms_bit_table(
    int unit)
{
    dbal_tables_e table_name;
    uint32 entry_handle_id;
    unsigned int fem_id_max;
    unsigned int fem_pgm_max;
    dnx_field_fem_condition_ms_bit_t bit_select_init_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_TABLE_FIELD_PMF_A_FEM_CONDITION_MS_BIT;
    /*
     * Maximum number of FEMs in iPMF1/2 == 16
     */
    fem_id_max = SAL_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1))->nof_bits_in_fem_id;
    /*
     * Maximum number of FEM programs in iPMF1/2 == 4
     */
    fem_pgm_max = SAL_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_programs);
    /*
     * We initialize the bit_select to its minimal value
     */
    bit_select_init_val =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1) _fem_condition_ms_bit_min_value;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_ID, 0, fem_id_max - 1);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_PGM_ID, 0, fem_pgm_max - 1);
    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_FEM_CONDITION_MS_BIT, INST_SINGLE,
                                 bit_select_init_val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Clear current entry to config the next one.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_name, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* } */
#endif

/**
 * \brief
 *  Init all relevant information for Field FEM related tables
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_init_fem_tables(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

#if (DNX_FIELD_INIT_FEM_TABLES_FAST)
/* { */
    /*
     * Directly access DBAL tables and write without first reading.
     * This is fast but suitable for init stage only.
     */
    /*
     * Initialize FIELD_PMF_A_FEM_CONDITION_MS_BIT table.
     */
    SHR_IF_ERR_EXIT(dnx_field_init_fem_condition_ms_bit_table(unit));
    /*
     * Initialize FIELD_PMF_A_FEM_MAP_INDEX table.
     */
    SHR_IF_ERR_EXIT(dnx_field_init_fem_map_index_table(unit));
    /*
     * Initialize FIELD_PMF_A_FEM_ACTION_TYPE_MAP table.
     */
    SHR_IF_ERR_EXIT(dnx_field_init_fem_action_type_map_table(unit));
/* } */
#else
/* { */
    {
        dnx_field_fem_id_t fem_id_index, fem_id_max;
        dnx_field_fem_program_t fem_program_index, fem_program_max;
        uint8 ignore_actions;
        /*
         * Initialize FEM tables which do not involve 'context'. Set all
         * tables to NOT carry out any action.
         * Not that 'bit_select' (The index which indicates the MS bit of the 4 'condition' bits) must
         * be set to '3' (dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_condition_ms_bit_min_value) and NOT less!
         * The following tables are initialized:
         * - FIELD_PMF_A_FEM_CONDITION_MS_BIT table.
         * - FIELD_PMF_A_FEM_MAP_INDEX table.
         * - FIELD_PMF_A_FEM_ACTION_TYPE_MAP table.
         */
        fem_id_max = SAL_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_id);
        fem_program_max =
            SAL_BIT(dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_programs);
        ignore_actions = DNX_FIELD_IGNORE_NO_ACTIONS;
        for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
        {
            for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fem_defaults_set
                                (unit, fem_id_index, fem_program_index, ignore_actions));
            }
        }
    }
/* } */
#endif
    /*
     * There is no 'fast' varsion for this table (of 'FEM contexts'). If required,
     * this may be easily added here.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_all_fem_all_context_defaults_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init all tables related to Preselection
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_presel_tables(
    int unit)
{
    dbal_tables_e dbal_table_id;
    int stage;
    int max_capacity;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Initialize relevant tables for each PMF stage
     */
    DNX_FIELD_STAGE_CS_QUAL_ITERATOR(stage)
    {
        /*
         * Get CS TCAM table handle
         */
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, stage, &dbal_table_id));

        /*
         * Verify that the capacity matches the FP DNX data.
         */
        SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, dbal_table_id, &max_capacity));
        if (max_capacity != dnx_data_field.stage.stage_info_get(unit, stage)->nof_cs_lines)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "number of entries in table %s (%d) does not match the number of entries for "
                         "CS TCAM of stage %s (%d). Entries in DBAL %d, in DNX DATA %d.\n",
                         dbal_logical_table_to_string(unit, dbal_table_id), dbal_table_id,
                         dnx_field_stage_text(unit, stage), stage,
                         max_capacity, dnx_data_field.stage.stage_info_get(unit, stage)->nof_cs_lines);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init single Field iPMF1/2 General table
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_ipmf1_ipmf2_general_table(
    int unit)
{
    uint32 entry_handle_id;
    int core_id, nof_cores;
    int exem_key_select_1;
    int exem_key_select_2;
    int cmp_selection;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Key used by EXEM. Note any number larger than 3 is equivalent to 4.
     */
    exem_key_select_1 = dnx_data_field.exem.large_ipmf1_key_get(unit);
    exem_key_select_2 = dnx_data_field.exem.small_ipmf2_key_hw_value_get(unit);
    cmp_selection = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->cmp_selection;

    /*
     * Get max number of cores
     */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF1_IPMF2_GENERAL, &entry_handle_id));

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

        if (dnx_data_field.exem.large_ipmf1_key_configurable_get(unit))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPMF1_EXEM_KEY_SELECT, INST_SINGLE,
                                         exem_key_select_1);
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPMF2_EXEM_KEY_SELECT, INST_SINGLE,
                                     exem_key_select_2);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPMF1_CMP_SELECTION, INST_SINGLE, cmp_selection);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, core_id);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DESTINATION_ACTION_TRAP_ENCODING,
                                     INST_SINGLE, TRUE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Clear current entry to config other next entries */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FIELD_IPMF1_IPMF2_GENERAL, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init single Field iPMF3 General table
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_ipmf3_general_table(
    int unit)
{
    uint32 entry_handle_id;
    int core_id, nof_cores;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get max number of cores
     */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF3_GENERAL, &entry_handle_id));

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE, INST_SINGLE, core_id);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DESTINATION_ACTION_TRAP_ENCODING,
                                     INST_SINGLE, TRUE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Clear current entry to config other next entries */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FIELD_IPMF3_GENERAL, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Init single CPU trap code profile table
* \param [in] unit  - Device ID
* \param [in] table_name  - Table ID used for handle allocation
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_single_cpu_trap_code_profile_table(
    int unit,
    dbal_tables_e table_name)
{
    uint32 entry_handle_id;
    uint32 table_field_name;
    /** Set all fields in the CPU_TRAP_CODE_PROFILE table to all ones. */
    uint32 cpu_trap_code_profiles_all_ones[16];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(cpu_trap_code_profiles_all_ones, 0xFF, sizeof(cpu_trap_code_profiles_all_ones));

    switch (table_name)
    {
        case DBAL_TABLE_FIELD_PMF_A_CPU_TRAP_CODE_PROFILE:
        {
            /*
             * Relevant field, of the table, for which we should set a proper value.
             */
            table_field_name = DBAL_FIELD_FIELD_PMF_A_CPU_TRAP_CODE_PROFILE;
            break;
        }
        case DBAL_TABLE_FIELD_PMF_B_CPU_TRAP_CODE_PROFILE:
        {
            /*
             * Relevant field, of the table, for which we should set a proper value.
             */
            table_field_name = DBAL_FIELD_FIELD_PMF_B_CPU_TRAP_CODE_PROFILE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %d is not valid CPU TRAP CODE table or not ported to DBAL!",
                         table_name);
            break;
        }
    }

    /*
     * Initialize relevant values about cpu trap code profile tables
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /*
     * Set the values
     */
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, table_field_name, INST_SINGLE,
                                     cpu_trap_code_profiles_all_ones);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Init L4 Protocol table
* \param [in] unit  - Device ID
* \param [in] table_name  - Table ID used for handle allocation
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_is_L4_profile_table(
    int unit,
    dbal_tables_e table_name)
{
    uint32 entry_handle_id;
    uint32 table_field_name;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint32 l4_ops_init_val = DNX_FIELD_INIT_L4_OPS_INIT_VAL(unit);
    switch (table_name)
    {
        case DBAL_TABLE_FIELD_IPMF1_L4_PROTOCOL:
        {
            /*
             * Relevant field, of the table, for which we should set a proper value.
             */
            table_field_name = DBAL_FIELD_IPMF1_PROTOCOL_IS_L_4;
            break;
        }
        case DBAL_TABLE_FIELD_EPMF_L4_PROTOCOL:
        {
            /*
             * Relevant field, of the table, for which we should set a proper value.
             */
            table_field_name = DBAL_FIELD_EPMF_PROTOCOL_IS_L_4;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %d is not valid IS L4 OPS table or not ported to DBAL!", table_name);
            break;
        }
    }
    /*
     * Initialize relevant values about L4 profile tables
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));
    /*
     * Set the values
     */
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, table_field_name, INST_SINGLE, &l4_ops_init_val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init CS Inlif profile mapping
* \param [in] unit  - Device ID
* \param [in] table_name  - Table ID used for handle allocation
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_cs_inlif_profile_map(
    int unit,
    dbal_tables_e table_name)
{
    int cs_entry_iter, mask;
    unsigned int start_bit, nof_bits;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the relevant bits from inlif profile which are used by PMF 
     */
    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, &start_bit, &nof_bits));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));
    mask = (((1 << nof_bits) - 1) << start_bit);
    for (cs_entry_iter = 0; cs_entry_iter < dnx_data_field.preselector.num_cs_inlif_profile_entries_get(unit);
         cs_entry_iter++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CS_INLIF_PROFILE_ENTRY, cs_entry_iter);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_0_PROFILE, INST_SINGLE,
                                     ((cs_entry_iter & mask) >> start_bit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INLIF_1_PROFILE, INST_SINGLE,
                                     ((cs_entry_iter & mask) >> start_bit));

        /** writing the entry */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init Range qualifier table fields with 0
* \param [in] unit  - Device ID
* \param [in] table_name  - Table ID used for handle allocation
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_range_qualifier_value(
    int unit,
    dbal_tables_e table_name)
{
    uint32 entry_handle_id;
    uint32 range_id_iterator, range_id_max;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /**
     * Initialize relevant values about L4 profile tables
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));
    switch (table_name)
    {
        case DBAL_TABLE_FIELD_IPMF1_L4_OPS_RANGES:
        {
            range_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_l4_ops_ranges;
            break;
        }
        case DBAL_TABLE_FIELD_EPMF_L4_OPS_RANGES:
        {
            range_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_l4_ops_ranges;
            break;
        }
        case DBAL_TABLE_FIELD_IPMF1_OUT_LIF_RANGES:
        {
            range_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_out_lif_ranges;
            break;
        }
        case DBAL_TABLE_FIELD_IPMF3_OUT_LIF_RANGES:
        {
            range_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_out_lif_ranges;
            break;
        }
        case DBAL_TABLE_FIELD_IPMF1_PKT_HDR_SIZE_RANGE:
        {
            range_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_pkt_hdr_ranges;
            break;
        }
        default:
        {
            /**
             * None of the supported tables
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Table name:%d \n", table_name);
            break;
        }
    }

    for (range_id_iterator = 0; range_id_iterator < range_id_max; range_id_iterator++)
    {
        /**
         * Setting the DBAL table key.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_RANGE_ID, range_id_iterator);
#ifdef ADAPTER_SERVER_MODE
            /** clearing the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
#endif
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init context profile mapping register based on PMF contexts.
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_context_profile_map_pmf_context(
    int unit)
{
    uint32 entry_handle_id;
    dnx_field_context_t context_id;
    dnx_field_context_t context_id_max;
    unsigned int stage_idx;
    /*
     * The list of stages that have CS profile from PMF contexts. 
     * Must end with DBAL_NOF_ENUM_FIELD_PMF_STAGE_VALUES
     */
    dbal_enum_value_field_field_pmf_stage_e dbal_pmf_stage_ar[] = {
        DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF3,
        DBAL_NOF_ENUM_FIELD_PMF_STAGE_VALUES
    };
    /*
     * The list of stages that come before the corresponding stage in stage_ar.
     * Must end with DBAL_NOF_ENUM_FIELD_PMF_STAGE_VALUES
     */
    dnx_field_stage_e prev_stage_ar[] = { DNX_FIELD_STAGE_IPMF1,
        DNX_FIELD_STAGE_INVALID
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));

    for (stage_idx = 0;
         stage_idx < (sizeof(dbal_pmf_stage_ar) / sizeof(dbal_pmf_stage_ar[0])) &&
         dbal_pmf_stage_ar[stage_idx] < DBAL_NOF_ENUM_FIELD_PMF_STAGE_VALUES; stage_idx++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, dbal_pmf_stage_ar[stage_idx]);

        if (stage_idx >= (sizeof(prev_stage_ar) / sizeof(prev_stage_ar[0])) ||
            prev_stage_ar[stage_idx] == DNX_FIELD_STAGE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size mismatch between list of stages and list of prev stages at %d.\n",
                         stage_idx);
        }

        context_id_max = dnx_data_field.stage.stage_info_get(unit, prev_stage_ar[stage_idx])->nof_contexts;
        for (context_id = 0; context_id < context_id_max; context_id++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);

            /*
             * When using a profile based on a PMF ocntext, we use a 1:1 mapping of context Id to Context profile
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE,
                                         context_id);

            /** writing the entry */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init MDB intefarce MUX register
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_mux_mdb_interfaces(
    int unit)
{
    uint32 entry_handle_id;
    dnx_field_stage_e sexem_stage;
    int sexem_mux_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * The value of the SEXM MUX is dependant on the SOC property pmf_sexem3_stage.
     */
    sexem_stage = dnx_data_field.profile_bits.pmf_sexem3_stage_get(unit);
    if (sexem_stage == DNX_FIELD_STAGE_IPMF2)
    {
        sexem_mux_value = 1;
    }
    else if (sexem_stage == DNX_FIELD_STAGE_IPMF3)
    {
        sexem_mux_value = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The value of pmf_sexem3_stage SOC property is %d, unrecognized value.\n", sexem_stage);
    }

    /**
     * writing data foe relevant table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_MUX_MDB_INTERFACES, &entry_handle_id));
    /**
     * Setting the DBAL table key.
     */

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MUX_MAPS_INTERFACE, INST_SINGLE, 0);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MUX_LEXEM_INTERFACE, INST_SINGLE, 0);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MUX_SEXEM_INTERFACE, INST_SINGLE, sexem_mux_value);

    /** writing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init all Field GENERAL values
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_general_tables(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize relevant field for PMF_GENERAL tables. */
    SHR_IF_ERR_EXIT(dnx_field_init_ipmf1_ipmf2_general_table(unit));
    SHR_IF_ERR_EXIT(dnx_field_init_ipmf3_general_table(unit));
    SHR_IF_ERR_EXIT(dnx_field_init_mux_mdb_interfaces(unit));
    SHR_IF_ERR_EXIT(dnx_field_init_single_cpu_trap_code_profile_table
                    (unit, DBAL_TABLE_FIELD_PMF_A_CPU_TRAP_CODE_PROFILE));
    SHR_IF_ERR_EXIT(dnx_field_init_single_cpu_trap_code_profile_table
                    (unit, DBAL_TABLE_FIELD_PMF_B_CPU_TRAP_CODE_PROFILE));
    SHR_IF_ERR_EXIT(dnx_field_init_is_L4_profile_table(unit, DBAL_TABLE_FIELD_IPMF1_L4_PROTOCOL));
    SHR_IF_ERR_EXIT(dnx_field_init_is_L4_profile_table(unit, DBAL_TABLE_FIELD_EPMF_L4_PROTOCOL));
    SHR_IF_ERR_EXIT(dnx_field_init_range_qualifier_value(unit, DBAL_TABLE_FIELD_IPMF1_L4_OPS_RANGES));
    SHR_IF_ERR_EXIT(dnx_field_init_range_qualifier_value(unit, DBAL_TABLE_FIELD_EPMF_L4_OPS_RANGES));
    SHR_IF_ERR_EXIT(dnx_field_init_range_qualifier_value(unit, DBAL_TABLE_FIELD_IPMF1_OUT_LIF_RANGES));
    SHR_IF_ERR_EXIT(dnx_field_init_range_qualifier_value(unit, DBAL_TABLE_FIELD_IPMF3_OUT_LIF_RANGES));
    SHR_IF_ERR_EXIT(dnx_field_init_range_qualifier_value(unit, DBAL_TABLE_FIELD_IPMF1_PKT_HDR_SIZE_RANGE));
    SHR_IF_ERR_EXIT(dnx_field_context_profile_map_kbp_context_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_init_context_profile_map_pmf_context(unit));
    SHR_IF_ERR_EXIT(dnx_field_init_cs_inlif_profile_map(unit, DBAL_TABLE_FIELD_IPMF1_CS_INLIF_PROFILE_MAP));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_system_headers_init(
    int unit)
{
    uint32 entry_handle_id;
    int ii;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE, &entry_handle_id));

    for (ii = 0; ii < DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_NOF; ii++)
    {
        /*
         * Get the profile data from DNX_DATA
         */
        const dnx_data_field_system_headers_system_header_profiles_t *profile;
        profile = dnx_data_field.system_headers.system_header_profiles_get(unit, ii);

        /*
         * Fill in DBAL values from the DNX_DATA struct
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_PROFILE, ii);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_FTMH, INST_SINGLE, profile->build_ftmh);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_OAM_TS_HEADER,
                                     INST_SINGLE, profile->build_tsh);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_PPH, INST_SINGLE, profile->build_pph);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_UDH, INST_SINGLE, profile->build_udh);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEVER_ADD_PPH_LEARN_EXT,
                                     INST_SINGLE, profile->never_add_pph_learn_ext);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Clear entry before configuring J1 system header mode fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE, entry_handle_id));

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        /*
         * In case of J1 system header mode, we add ftmh dest system port to all ethernet packet by
         * setting ENABLE_STACKING_UC and ENABLE_STACKING_MC to all system profiles when ftmh_add_dsp_ext
         * SOC property is set.
         */
        if (dnx_data_headers.ftmh.add_dsp_ext_get(unit))
        {
            
            for (ii = 0; ii < dnx_data_field.system_headers.nof_profiles_get(unit); ii++)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_PROFILE, ii);
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_STACKING_UC, INST_SINGLE, 1);
                dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_STACKING_MC, INST_SINGLE, 1);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_udh_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 udh_type_len[DNX_DATA_MAX_FIELD_UDH_TYPE_COUNT] = {
        DNX_DATA_MAX_FIELD_UDH_TYPE_0_LENGTH,
        DNX_DATA_MAX_FIELD_UDH_TYPE_1_LENGTH,
        DNX_DATA_MAX_FIELD_UDH_TYPE_2_LENGTH,
        DNX_DATA_MAX_FIELD_UDH_TYPE_3_LENGTH
    };
    int udh_index;
    int system_headers_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        /**
         * Fill in UDH size for J1 mode
         */
        udh_type_len[0] =
            BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit) +
                       dnx_data_field.udh.field_class_id_size_2_get(unit));
        udh_type_len[1] =
            BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit) +
                       dnx_data_field.udh.field_class_id_size_3_get(unit));
        udh_type_len[2] = 0;
        udh_type_len[3] = 0;
    }
    /*
     * Enable UDH
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_ALL, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Fill in UDH map
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG_UDH, &entry_handle_id));

    for (udh_index = 0; udh_index < DNX_DATA_MAX_FIELD_UDH_TYPE_COUNT; udh_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_TYPE, udh_index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_MAP_TYPE_TO_LENGTH, INST_ALL,
                                     udh_type_len[udh_index]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* /brief
*  Converts DBAL stage to its TCAM counterpart.
*  Returns error for unsupported stages for static tables init.
*
* \param [in] unit         - Device ID
* \param [in] dbal_stage   - DBAL stage to convert
* \param [out] tcam_stage  - The converted TCAM stage for the given DBAL stage
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_init_dbal_stage_to_field_convert(
    int unit,
    dbal_stage_e dbal_stage,
    dnx_field_tcam_stage_e * tcam_stage)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_stage)
    {
        case DBAL_STAGE_VT1:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_VTT1;
            break;
        }
        case DBAL_STAGE_VT2:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_VTT2;
            break;
        }
        case DBAL_STAGE_VT3:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_VTT3;
            break;
        }
        case DBAL_STAGE_VT4:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_VTT4;
            break;
        }
        case DBAL_STAGE_VT5:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_VTT5;
            break;
        }
        case DBAL_STAGE_FWD1:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_FWD1;
            break;
        }
        case DBAL_STAGE_FWD2:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_FWD2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Unsupported dbal stage %d for Field static tables init, stage should be FLP or VT\n",
                         dbal_stage);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_init.h
 */
shr_error_e
dnx_field_init_tcam_fwd_tables(
    int unit)
{
    CONST dbal_logical_table_t *table;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_TCAM, &table_id));
    while (table_id != DBAL_TABLE_EMPTY)
    {
        dnx_field_tcam_database_t db;
        uint32 handler_id;
        int actual_action_size = 0;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        SHR_IF_ERR_EXIT(dnx_field_tcam_database_t_init(unit, &db));

                /**
         *  according to the key size 80\160\320 and according to the result size 32\64\128
         * this is a simple implementation that the key size and result size need to be aligned it means that 32 goes to 80, 64
         * to 160 and 128 to 320.
         * 80b key-size can only be used when flip ECO is present.
         */
        if (table->key_size < dnx_data_field.tcam.key_size_half_get(unit)
            && (dnx_data_field.minor_changes_j2_a0_b0.tcam_result_flip_eco_get(unit)))
        {
                         /***/
            if (table->multi_res_info[0].entry_payload_size < dnx_data_field.tcam.action_size_half_get(unit))
            {
                db.key_size = DNX_FIELD_KEY_LENGTH_TYPE_HALF;
                db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF;
            }
            else if (table->multi_res_info[0].entry_payload_size < dnx_data_field.tcam.action_size_single_get(unit))
            {
                db.key_size = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;
                db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE;
            }
            else
            {
                db.key_size = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;
                db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE;
            }
        }
        else if (table->key_size < dnx_data_field.tcam.key_size_single_get(unit))
        {
            if (table->multi_res_info[0].entry_payload_size < dnx_data_field.tcam.action_size_single_get(unit))
            {
                db.key_size = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;
                db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE;
            }
            else
            {
                db.key_size = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;
                db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE;
            }
        }
        else
        {
            db.key_size = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;
            db.action_size = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE;
        }

        SHR_IF_ERR_EXIT(dnx_field_init_dbal_stage_to_field_convert(unit, table->dbal_stage, &(db.stage)));

                /** Automatically allocate prefix of size 5 */
        db.prefix_size = MIN(dnx_data_field.tcam.max_prefix_size_get(unit), table->app_id_size);
        db.prefix_value = DNX_FIELD_TCAM_PREFIX_VAL_AUTO;

        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, table_id, 0, &actual_action_size));
        db.actual_action_size = actual_action_size;

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_create
                        (unit, &db, table->app_id, DNX_FIELD_TCAM_HANDLER_MODE_DEFAULT, &handler_id));

        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_set(unit, table_id, handler_id));

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_TCAM, &table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_init_tcam_tables(
    int unit)
{
    int nof_banks = dnx_data_field.tcam.nof_banks_get(unit);
    int nof_big_banks = dnx_data_field.tcam.nof_big_banks_get(unit);
    int nof_entries;
    int entry_index;
    int bank_index;
    int bank_first_entry_position;
    int position;
    int first_action_table;
    int second_action_table;
    uint32 data[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE) + 1] = { 0 };
    uint32 payload_data[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_DOUBLE)] = { 0 };
    soc_reg_above_64_val_t mem_val;
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    /*
     * Set write command 
     */
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);

    for (bank_index = 0; bank_index < nof_banks; bank_index++)
    {
        /*
         * We avoid calling DNX DATA functions for each entry, so we find the poisiton of the first entry in the bank.
         */
        bank_first_entry_position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_index, 0);
        nof_entries = FIELD_TCAM_BANK_NOF_ENTRIES(bank_index) / 2;
        first_action_table = bank_index * 2;
        second_action_table = bank_index * 2 + 1;
        for (entry_index = 0; entry_index < nof_entries; entry_index++)
        {
            /*
             * Calculate the position of the entry within the TCAM
             */
            position = bank_first_entry_position + (entry_index * 2);

            /*
             * This writes both key and mask 
             */
            SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, position, mem_val));

            /*
             * Now we write action according to big/small banks indication 
             */
            if (bank_index < nof_big_banks)
            {
                action_tbl = TCAM_TCAM_ACTIONm;
            }
            else
            {
                action_tbl = TCAM_TCAM_ACTION_SMALLm;
            }
            soc_mem_field_set(unit, action_tbl, payload_data, ACTIONf, data);
            /*
             * Set both action tables 
             */
            SHR_IF_ERR_EXIT(soc_mem_array_write
                            (unit, action_tbl, first_action_table, MEM_BLOCK_ALL, entry_index, payload_data));
            SHR_IF_ERR_EXIT(soc_mem_array_write
                            (unit, action_tbl, second_action_table, MEM_BLOCK_ALL, entry_index, payload_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_init.h
 */
shr_error_e
dnx_field_init_tables(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_TIME_MEASURE_INIT(unit);

    /**
     * FES and KEY are initialized by HW init mechanism!
     * Please look jer2_a0_dev_init.xml and init_custom_funcs.c.
     *
     * FFC should be all zeros hence nothing to initialize.
     */

#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit) <= 1)
    {   /* save init time in emulation by not executing the slow table inits */
#endif
        SHR_IF_ERR_EXIT(dnx_field_init_fem_tables(unit));
        SHR_TIME_MEASURE();
#ifdef DNX_EMULATION_1_CORE
    }
#endif

    SHR_IF_ERR_EXIT(dnx_field_init_presel_tables(unit));
    SHR_TIME_MEASURE();

    SHR_IF_ERR_EXIT(dnx_field_init_general_tables(unit));
    SHR_TIME_MEASURE();

    SHR_IF_ERR_EXIT(dnx_field_init_tcam_tables(unit));
    SHR_TIME_MEASURE();

    SHR_TIME_MEASURE_CLOSE(unit);
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header on field_init.h
 */
shr_error_e
dnx_field_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**VERY IMPORTENT NOTE:
     * This function is not invoked in init
     * If you want to add more init staged
     * 1)go explicitly to init_seq.c
     * 2) find dnx_init_deinit_field_seq
     * 3) add another line of init de-init
     */

    SHR_IF_ERR_EXIT(dnx_field_map_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_init_sw_state(unit));

    SHR_IF_ERR_EXIT(dnx_field_init_tables(unit));

    SHR_IF_ERR_EXIT(dnx_field_init_tcam_fwd_tables(unit));

    SHR_IF_ERR_EXIT(dnx_field_context_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_presel_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_udh_init(unit));

    SHR_IF_ERR_EXIT(dnx_field_system_headers_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header on field_init.h
 */
shr_error_e
dnx_field_init_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**VERY IMPORTENT NOTE:
     * This function is not invoked in deinit
     * If you want to add more init staged
     * 1)go explicitly to init_seq.c
     * 2) find dnx_init_deinit_field_seq
     * 3) add another line of init de-init
     */

    SHR_IF_ERR_EXIT(dnx_field_map_deinit(unit));

    SHR_IF_ERR_EXIT(dnx_field_context_deinit(unit));

    SHR_IF_ERR_EXIT(dnx_field_presel_deinit(unit));

    SHR_IF_ERR_EXIT(dnx_field_deinit_sw_state(unit));

exit:
    SHR_FUNC_EXIT;
}
#if DBX_FIELD_BRUTE_FORCE_DEINIT_INIT
/* { */
/**
 * See header on field_init.h
 */
shr_error_e
dnx_field_init_reinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
#endif
/**
 * \brief
 *  Init all relevant information for Field module
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   This functions is BCM API.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_field_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function,  can be init on device init or by API*/
    SHR_IF_ERR_EXIT(dnx_field_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  De-Init all relevant information for Field module
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   This functions is BCM API.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_field_detach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Call internal function,  can be init on device init or by API*/
    SHR_IF_ERR_EXIT(dnx_field_init_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
