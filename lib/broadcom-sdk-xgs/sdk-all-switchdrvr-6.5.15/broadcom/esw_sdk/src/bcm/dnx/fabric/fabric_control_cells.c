/** \file fabric_control_cells.c
 * $Id$
 *
 * Fabric control cells functions for DNX. \n
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

/*
 * Includes:
 * {
 */

#include <sal/types.h>

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/legacy/JER2/jer2_regs.h>
#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>
#include <soc/drv.h> 
#include <soc/dnx/dbal/dbal.h>

/*
 * } 
 */

/*
 * Defines:
 * {
 */

/*
 * Mesh Topology Threshold
 */
#define DNX_FABRIC_CTRL_CELLS_MESH_TOPOLOGY_STATUS_TH (256)

#define DNX_FABRIC_CTRL_CELLS_INIT_TIMER_ITERATIONS      (1500)
#define DNX_FABRIC_CTRL_CELLS_INIT_FIFO_EMPTY_ITERATIONS (24)
#define DNX_FABRIC_CTRL_CELLS_INIT_RCH_STATUS_ITERATIONS (24)
#define DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS (8)
#define DNX_FABRIC_CTRL_CELLS_INIT_TIMER_DELAY_MSEC      (32)

/*
 * } 
 */

/*
 * Type defs:
 * {
 */

/**
 * \brief
 *   Pointers to functions that being used by dnx_fabric_ctrl_cells_polling_t.
 */
typedef int (*dnx_ctrl_cells_poll_func_f) (
    int unit,
    uint32 iter_index,
    int *success);
typedef int (*dnx_ctrl_cells_fnly_func_f) (
    int unit);

/**
 * \brief
 *   Handle polling functions calling information
 */
typedef struct
{
    /** What the polling function do. We'll be printed in verbose */
    char *polling_name;
    /** Polling function */
    dnx_ctrl_cells_poll_func_f polling_func;
    /** Function to run in case of polling function failed */
    dnx_ctrl_cells_fnly_func_f failure_func;
    /**
     *  This is the number of iterations (at least one is performed).
     */
    uint32 nof_polling_iters;
    /** If TRUE, need to return error when polling function failed */
    uint32 err_on_fail;
} dnx_fabric_ctrl_cells_polling_t;

/*
 * } 
 */

/*
 * Functions:
 * {
 */

/**
 * \brief
 *   Polling function that return success when all links are down.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_all_links_disable_polling(
    int unit,
    uint32 iter_index,
    int *all_down)
{
    uint32 entry_handle_id;
    int link_id = 0;
    uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
    uint32 is_link_active = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *all_down = TRUE;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LINK_ACTIVE, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (link_id = 0; link_id < nof_links; ++link_id)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IS_LINK_ACTIVE, link_id, &is_link_active));
        if (is_link_active == TRUE)
        {
            *all_down = FALSE;
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Polling function that return success when CPU cell FIFO is empty.
 */
static shr_error_e
dnx_fabric_ctrl_cells_fifo_empty_polling(
    int unit,
    uint32 iter_index,
    int *is_empty)
{
    uint32 entry_handle_id;
    uint32 buffer[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = {0};
    uint32 is_fifo_not_empty = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Read cell from FIFO buffer
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RECEIVE_DATA, &entry_handle_id));
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_CONTROL_CELL_DATA, INST_SINGLE, buffer);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Check if CPU-Control-FIFO is empty
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RECEIVE_CONTROL, entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONTROL_CELL_DATA_READY, INST_SINGLE, &is_fifo_not_empty);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_empty = !is_fifo_not_empty;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set whether the device is in stand-alone mode.
 * \param [in] unit -
 *   The unit number.
 * \param [in] is_single_fap_mode -
 *   1 - stand-alone mode.
 *   0 - not stand-alone mode.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(
    int unit,
    int is_single_fap_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_SINGLE_FAP, INST_SINGLE, is_single_fap_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether the device is in stand-alone mode.
 * \param [in] unit -
 *   The unit number.
 * \param [out] is_single_fap_mode -
 *   1 - stand-alone mode.
 *   0 - not stand-alone mode.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_ctrl_cells_stand_alone_fap_mode_get(
    int unit,
    int *is_single_fap_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_SINGLE_FAP, INST_SINGLE, (uint32*)&is_single_fap_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   The following functions:
 *      dnx_fabric_ctrl_cells_standalone_polling
 *      dnx_fabric_ctrl_cells_standalone_failure
 *   works together.
 *   The 'polling' function read control cells counter.
 *   If control cells detected, that means we should set the device
 *   NOT in stand-alone mode.
 *   If control cells weren't detected, that means we should set the
 *   device in stand-alone mode, and we do it by calling the 'failure'
 *   function.
 *
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_standalone_polling(
    int unit,
    uint32 iter_index,
    int *success)
{
    uint32 entry_handle_id;
    uint32 nof_control_cells = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE, &nof_control_cells);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * ignore first iteration in order to clear counter
     */
    *success = (nof_control_cells ? 1 : 0) && (iter_index != 0);

    /*
     * cfg not stand alone in success case 
     */
    if (*success)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, FALSE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   See dnx_fabric_ctrl_cells_standalone_polling comment.
 * \see
 *   * dnx_fabric_ctrl_cells_standalone_polling.
 */
static shr_error_e
dnx_fabric_ctrl_cells_standalone_failure(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check Mesh Topology status
 */
static shr_error_e
dnx_fabric_ctrl_cells_mesh_topology_check(
    int unit,
    int stand_alone,
    int *success)
{
    uint32 entry_handle_id;
    uint32 threshold, pos_watermark, neg_watermark;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1, INST_SINGLE, &pos_watermark);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2, INST_SINGLE, &neg_watermark);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    threshold = DNX_FABRIC_CTRL_CELLS_MESH_TOPOLOGY_STATUS_TH;

    if (((pos_watermark < threshold) && (neg_watermark < threshold)) || (stand_alone == TRUE))
    {
        *success = TRUE;
    }
    else
    {
        *success = FALSE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Polling function for control cells status.
 *   If control cells init didn't finished, read control cells counter
 *   and check maybe we should be in stand-alone mode.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_status_polling(
    int unit,
    uint32 iter_index,
    int *success)
{
    uint32 entry_handle_id;
    uint32 nof_control_cells = 0;
    int stand_alone = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_get(unit, &stand_alone));

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_mesh_topology_check(unit, stand_alone, success));

    if (*success == FALSE)
    {
        /*
         * recheck stand alone once every DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS
         */
        if ((iter_index % DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS == 0) && (iter_index != 0))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE, &nof_control_cells);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * cfg stand alone when the counter is zero
             */
            if (nof_control_cells == 0)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_stand_alone_fap_mode_set(unit, TRUE));

                *success = TRUE;

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Control cells enable: Stand-alone mode detected on the %u iteration.\n"),
                             iter_index));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Polling function that returns whether there is any link up.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_links_enable_polling(
    int unit,
    uint32 iter_index,
    int *any_up)
{
    int all_down;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_all_links_disable_polling(unit, iter_index, &all_down));

    *any_up = all_down ? 0 : 1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Clear control cells counters.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_counter_clear(
    int unit)
{
    uint32 entry_handle_id;
    uint32 control_cells_type1 = 0;
    uint32 status_1 = 0;
    uint32 status_2 = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_STATUS, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_CONTROL_CELLS_TYPE1, INST_SINGLE, &control_cells_type1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_1, INST_SINGLE, &status_1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATUS_2, INST_SINGLE, &status_2);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get array of enabled FMAC blocks
 */
static shr_error_e
dnx_fabric_ctrl_cells_fmac_get(
    int unit,
    int *nof_enabled_fmacs,
    int fmacs_array[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC])
{
    uint32 fmac = 0;
    uint32 nof_instances_fmac = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    int count = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (fmac = 0; fmac < nof_instances_fmac; ++fmac)
    {
        int enable;
        SHR_IF_ERR_EXIT(soc_jer2_fmac_block_enable_get(unit, fmac, &enable));
        if (enable)
        {
            fmacs_array[count++] = fmac;
        }
    }

    *nof_enabled_fmacs = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Actions are taken according to the 'data' param:
 *      If data includes register param - write it to HW.
 *      If data includes polling funtion - run it until success.
 *      If data includes delay - wait that time before continuing execution.
 * \see
 *   * dnx_fabric_ctrl_cells_enable_set.
 */
static shr_error_e
dnx_fabric_ctrl_cells_enable_polling(
    int unit,
    dnx_fabric_ctrl_cells_polling_t * data)
{
    int success = TRUE;
    int is_sim = FALSE;
    uint32 iter_index = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        is_sim = TRUE;
    }
#endif

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Control cells enable: Start polling stage - %s.\n"), data->polling_name));

    /*
     * If there polling field exists, refer to delay as the expected value 
     */
    for (iter_index = 0; iter_index < data->nof_polling_iters; ++iter_index)
    {
        if (is_sim)
        {
            success = (data->failure_func && !data->err_on_fail) ? FALSE : TRUE;
        }
        else
        {
            SHR_IF_ERR_EXIT(data->polling_func(unit, iter_index, &success));
        }

        if (success)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Control cells enable: Succeeded polling stage - %s after %u times.\n"),
                         data->polling_name, iter_index));
            SHR_EXIT();
        }
        sal_msleep(DNX_FABRIC_CTRL_CELLS_INIT_TIMER_DELAY_MSEC);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U
                 (unit, "Control cells enable: Failed polling stage - %s. Polling function run %u times.\n"),
                 data->polling_name, iter_index));

    if (data->failure_func)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Control cells enable: Run failure function stage - %s.\n"),
                     data->polling_name));
        SHR_IF_ERR_EXIT(data->failure_func(unit));
    }

    if (data->err_on_fail)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Control cells enable: Failed polling stage - %s.\n", data->polling_name);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_ctrl_cells_standalone_state_modify(
    int unit)
{
    dnx_fabric_ctrl_cells_polling_t data;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Polling on recieving control cells (to determine stand-alone mode)
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "recieving control cells (to determine stand-alone mode)";
    data.polling_func = dnx_fabric_ctrl_cells_standalone_polling;
    data.failure_func = dnx_fabric_ctrl_cells_standalone_failure;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_STANDALONE_ITERATIONS;
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

    /*
     * Polling to see whether control cells init is done
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "whether control cells init is done";
    data.polling_func = dnx_fabric_ctrl_cells_status_polling;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_TIMER_ITERATIONS;
    data.err_on_fail = TRUE; /** Throw error in case polling failed */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Calculate rmgr_global HW value.
 *   The period is in HW resolution.
 */
static shr_error_e
dnx_fabric_ctrl_cells_reachability_global_rmgr_period_get(
    int unit,
    uint32 *period)
{
    uint32 nof_clock_cycles = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);

    time.time_units = DNXCMN_TIME_UNIT_USEC;
    COMPILER_64_SET(time.time, 0, dnx_data_fabric.reachability.full_cycle_period_usec_get(unit));
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));

    *period = UTILEX_DIV_ROUND_DOWN(nof_clock_cycles, dnx_data_fabric.reachability.resolution_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Calculate reachability watchdog HW value.
 *   The period is in HW resolution.
 */
static shr_error_e
dnx_fabric_ctrl_cells_reachability_watchdog_period_get(
    int unit,
    uint32 *period)
{
    uint32 nof_clock_cycles = 0;
    dnxcmn_time_t time;
    SHR_FUNC_INIT_VARS(unit);

    time.time_units = DNXCMN_TIME_UNIT_USEC;
    COMPILER_64_SET(time.time, 0, dnx_data_fabric.reachability.watchdog_period_usec_get(unit));
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));

    *period = UTILEX_DIV_ROUND_UP(nof_clock_cycles, dnx_data_fabric.reachability.watchdog_resolution_get(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get which value should be configured in the following field:
 *   MESH_TOPOLOGY_INITr CONFIG_1f
 *
 * \see
 *  * dnx_fabric_ctrl_cells_enable_set
 */
static shr_error_e
dnx_fabric_ctrl_cells_mesh_topology_init_val_get(
    int unit,
    uint32 *field_val)
{
    uint32 entry_handle_id;
    uint32 nof_links = dnx_data_fabric.links.nof_links_get(unit);
    uint32 link_id = 0;
    int is_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_MESH)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, &entry_handle_id));
        for (link_id = 0; link_id < nof_links; ++link_id)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32*)&is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (is_enabled)
            {
                break;
            }
        }

        *field_val = (is_enabled) ? 0x2 : 0x0;
    }
    else if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
    {
        *field_val = 0;
    }
    else
    {
        *field_val = 0x2;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable control cells
 * \remark
 *   dnx_fabric_all_ctrl_cells_disable_set and dnx_fabric_all_ctrl_cells_enable_set
 *   are coupled, which means that is something is added/removed from one of them
 *   there is a good chance that it will be needed to be done also to the second
 *   function.
 */
static shr_error_e
dnx_fabric_all_ctrl_cells_enable_set(
    int unit,
    int is_soft_reset)
{
    uint32 entry_handle_id;
    uint32 rate = 0;
    uint32 global_rate = 0;
    uint32 watchdog_threshold = 0;
    uint32 field_val = 0;
    int link_id = 0;
    dnx_fabric_ctrl_cells_polling_t data;
    int nof_enabled_fmacs = 0;
    int fmacs_array[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC] = {0};
    int fmac_index = 0;
    uint32 nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initialize enabled FMACs array
     */
    if (soc_sand_is_emulation_system(unit) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_fmac_get(unit, &nof_enabled_fmacs, fmacs_array));
    }

    /*
     * Clear control cells counters
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_counter_clear(unit));

    /*
     * Enable reachability messages
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_REACHABILITY, &entry_handle_id));
    rate = dnx_data_fabric.reachability.gen_period_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, rate);
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_reachability_global_rmgr_period_get(unit, &global_rate));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_RATE, INST_SINGLE, global_rate);
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_reachability_watchdog_period_get(unit, &watchdog_threshold));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WATCHDOG_THRESHOLD, INST_SINGLE, watchdog_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_MASK_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_MSG_UPDATE_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Enable reachability control cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_REACHABILITY_DISCARD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Make sure that incoming control cells are not trapped to the CPU
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TRAP_ALL, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Polling until there will be any fabric link up
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "any fabric link active";
    data.polling_func = dnx_fabric_ctrl_cells_links_enable_polling;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_RCH_STATUS_ITERATIONS;
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

    /*
     * Polling until FCR fifo will be empty
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "whether control cells FCR FIFO is empty";
    data.polling_func = dnx_fabric_ctrl_cells_fifo_empty_polling;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_FIFO_EMPTY_ITERATIONS;
    data.err_on_fail = TRUE; /** Throw error in case polling failed */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));


    /*
     * Set Mesh Topology link configurations
     */
    /** FMAC Does not work in emulation */
    if (soc_sand_is_emulation_system(unit) == 0)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            int base_link_id = fmacs_array[fmac_index]*nof_links_in_fmac;
            for (link_id = base_link_id; link_id < base_link_id + nof_links_in_fmac; ++link_id)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
                if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_FE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_0, INST_SINGLE, 1);
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_2, INST_SINGLE, 1);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    /*
     * Set Mesh Topology configurations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, 0x14);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Do not config as part of soft reset sequance */
    if (!is_soft_reset)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST, INST_SINGLE, dnx_data_fabric.debug.mesh_topology_fast_get(unit) ? 0x1 : 0x0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_6, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_mesh_topology_init_val_get(unit, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_1, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Determine whether the device is standalone or not according to
     * received control cells
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_standalone_state_modify(unit));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    if (!is_soft_reset)
    {
        /** Do not config as part of soft reset sequance */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FAST, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_5, INST_SINGLE, 0x7);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Start sending control cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FLOW_STATUS_DISCARD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CREDIT_DISCARD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Enable LLFC control cells
     */
    if (soc_sand_is_emulation_system(unit) == 0)
    {   /* FMAC Does not work in emulation */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LLFC, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmacs_array[fmac_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        sal_msleep(16);
    }

    /*
     * Disable packets discard
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALL_PACKETS_DISCARD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Disable control cells
 * \remark
 *   dnx_fabric_all_ctrl_cells_disable_set and dnx_fabric_all_ctrl_cells_enable_set
 *   are coupled, which means that is something is added/removed from one of them
 *   there is a good chance that it will be needed to be done also to the second
 *   function.
 *   Note also that the sequence is essentialy backwards of the 'enable' function.
 */
static shr_error_e
dnx_fabric_all_ctrl_cells_disable_set(
    int unit,
    int is_soft_reset)
{
    uint32 entry_handle_id;
    dnx_fabric_ctrl_cells_polling_t data;
    int link_id = 0;
    int nof_enabled_fmacs = 0;
    int fmacs_array[DNX_DATA_MAX_FABRIC_BLOCKS_NOF_INSTANCES_FMAC] = {0};
    int fmac_index = 0;
    uint32 nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initialize enabled FMACs array
     */
    if (soc_sand_is_emulation_system(unit) == 0)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_fmac_get(unit, &nof_enabled_fmacs, fmacs_array));
    }

    /*
     * Clear control cells counters
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_counter_clear(unit));

    /*
     * Enable packets discard
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALL_PACKETS_DISCARD, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Disable LLFC control cells
     */
    if (soc_sand_is_emulation_system(unit) == 0)
    {   /* FMAC Does not work in emulation */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_LLFC, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC, fmacs_array[fmac_index]);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 0);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        sal_msleep(16);
    }

    /*
     * Discard all control cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_TX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_FLOW_STATUS_DISCARD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CREDIT_DISCARD, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_REACHABILITY_DISCARD, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Trap incoming control cells to the CPU
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_RX_DISCARD, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TRAP_ALL, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Polling until all fabric links will be down
     */
    sal_memset(&data, 0, sizeof(data));
    data.polling_name = "all fabric links are down";
    data.polling_func = dnx_fabric_ctrl_cells_all_links_disable_polling;
    data.nof_polling_iters = DNX_FABRIC_CTRL_CELLS_INIT_TIMER_ITERATIONS;
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_polling(unit, &data));

    /*
     * Clear Mesh Topology configurations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_5, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MESH_CONFIG_1, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Do not config as part of soft reset sequance */
    if (!is_soft_reset)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONFIG_6, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Clear Mesh Topology link configurations
     */
    /** FMAC Does not work in emulation */
    if (soc_sand_is_emulation_system(unit) == 0)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_LINKS, entry_handle_id));
        for (fmac_index = 0; fmac_index < nof_enabled_fmacs; ++fmac_index)
        {
            int base_link_id = fmacs_array[fmac_index]*nof_links_in_fmac;
            for (link_id = base_link_id; link_id < base_link_id + nof_links_in_fmac; ++link_id)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link_id);
                if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_FE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_0, INST_SINGLE, 0);
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MODE_2, INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

    /*
     * Set Mesh Topology configurations
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Disable reachability messages
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CONTROL_CELLS_REACHABILITY, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_RATE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WATCHDOG_THRESHOLD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LINK_STATUS_MASK_ENABLE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_MSG_UPDATE_ENABLE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_fabric_ctrl_cells_enable_set(
    int unit,
    int enable,
    int is_soft_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_all_ctrl_cells_enable_set(unit, is_soft_reset));
    }
    else
    {
        int is_control_cells_enabled = 0;

        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, &is_control_cells_enabled));

        /*
         * Disable control cells only if they are enabled.
         * No need to disable them if they are already disabled (and it also takes too
         * much time to double disable the control cells).
         */
        if (is_control_cells_enabled)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_all_ctrl_cells_disable_set(unit, is_soft_reset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_fabric_ctrl_cells_enable_get(int unit, int *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *  Just read one register as a representative
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONFIG_6, INST_SINGLE, (uint32*)enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * } 
 */
