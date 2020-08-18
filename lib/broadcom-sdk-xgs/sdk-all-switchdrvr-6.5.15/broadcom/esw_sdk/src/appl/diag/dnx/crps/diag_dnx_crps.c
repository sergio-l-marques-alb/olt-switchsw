/** \file diag_dnx_crps.c
 *
 * Main diagnostics for crps applications
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <soc/dnx/swstate/auto_generated/access/crps_access.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <src/bcm/dnx/stat/crps/crps_eviction.h>
#include <src/bcm/dnx/stat/crps/crps_engine.h>
#include <bcm_int/dnx/stat/crps/crps_src_interface.h>
/** sal */
#include <sal/appl/sal.h>
#include "diag_dnx_crps.h"

/*************
 * TYPEDEFS  *
 *************/
typedef struct diag_dnx_crps_engine_source_s
{
    bcm_stat_counter_interface_type_t source_idx;
    char *source_str;
} diag_dnx_crps_engine_source_t;

static const diag_dnx_crps_engine_source_t source_enum_to_char_binding_table[] = {
    {bcmStatCounterInterfaceIngressReceivePp, "bcmStatCounterInterfaceIngressReceivePp"},
    {bcmStatCounterInterfaceIngressOam, "bcmStatCounterInterfaceIngressOam"},
    {bcmStatCounterInterfaceIngressVoq, "bcmStatCounterInterfaceIngressVoq"},
    {bcmStatCounterInterfaceIngressTransmitPp, "bcmStatCounterInterfaceIngressTransmitPp"},
    {bcmStatCounterInterfaceEgressReceivePp, "bcmStatCounterInterfaceEgressReceivePp"},
    {bcmStatCounterInterfaceEgressReceiveQueue, "bcmStatCounterInterfaceEgressReceiveQueue"},
    {bcmStatCounterInterfaceEgressTransmitPp, "bcmStatCounterInterfaceEgressTransmitPp"},
    {bcmStatCounterInterfaceEgressOam, "bcmStatCounterInterfaceEgressOam"},
    {bcmStatCounterInterfaceOamp, "bcmStatCounterInterfaceOamp"}
};

typedef struct diag_dnx_crps_engine_format_s
{
    bcm_stat_counter_format_type_t format_idx;
    char *format_str;
} diag_dnx_crps_engine_format_t;

static const diag_dnx_crps_engine_format_t format_enum_to_char_binding_table[] = {
    {bcmStatCounterFormatPacketsAndBytes, "bcmStatCounterFormatPacketsAndBytes"},
    {bcmStatCounterFormatPackets, "bcmStatCounterFormatPackets"},
    {bcmStatCounterFormatBytes, "bcmStatCounterFormatBytes"},
    {bcmStatCounterFormatDoublePackets, "bcmStatCounterFormatDoublePackets"},
    {bcmStatCounterFormatMaxSize, "bcmStatCounterFormatMaxSize"},
    {bcmStatCounterFormatIngressLatency, "bcmStatCounterFormatIngressLatency"}
};

typedef struct diag_dnx_crps_expansion_type_s
{
    bcm_stat_counter_format_type_t type_idx;
    char *exp_type_str;
} diag_dnx_crps_expansion_type_t;

static const diag_dnx_crps_expansion_type_t exp_type_enum_to_char_binding_table[] = {
    {bcmStatExpansionTypeTmDropReason, "TmDropReason"},
    {bcmStatExpansionTypeDispositionIsDrop, "DispositionIsDrop"},
    {bcmStatExpansionTypeDropPrecedenceMeter0Valid, "DropPrecedenceMeter0Valid"},
    {bcmStatExpansionTypeDropPrecedenceMeter0Value, "DropPrecedenceMeter0Value"},
    {bcmStatExpansionTypeDropPrecedenceMeter1Valid, "DropPrecedenceMeter1Valid"},
    {bcmStatExpansionTypDropPrecedenceMeter1Value, "DropPrecedenceMeter1Value"},
    {bcmStatExpansionTypeDropPrecedenceMeter2Valid, "DropPrecedenceMeter2Valid"},
    {bcmStatExpansionTypeDropPrecedenceMeter2Value, "DropPrecedenceMeter2Value"},
    {bcmStatExpansionTypeTrafficClass, "TrafficClass"},
    {bcmStatExpansionTypeDropPrecedenceMeterResolved, "DropPrecedenceMeterResolved"},
    {bcmStatExpansionTypeDropPrecedenceInput, "DropPrecedenceInput"},
    {bcmStatExpansionTypeSystemMultiCast, "SystemMultiCast"},
    {bcmStatExpansionTypeEcnEligibleAndCni, "EcnEligibleAndCni"},
    {bcmStatExpansionTypePortMetaData, "PortMetaData"},
    {bcmStatExpansionTypePpDropReason, "PpDropReason"},
    {bcmStatExpansionTypeMetaData, "MetaData"},
    {bcmStatExpansionTypeDiscardPp, "DiscardPp"},
    {bcmStatExpansionTypeDiscardTm, "DiscardTm"},
    {bcmStatExpansionTypeEgressTmActionType, "EgressTmActionType"},
    {bcmStatExpansionTypeLatencyBin, "LatencyBin"},
    {bcmStatExpansionTypeLatencyFlowProfile, "LatencyFlowProfile"},
};

/*************
 * DEFINES   *
 *************/
#define DIAG_DNX_CRPS_NO_OPTION     -1

static shr_error_e
sh_dnx_get_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int engine_id, valid;
    int core_id, core_idx, type_id;
    int counter_set;
    int database_id;
    int flags = 0;
    int nstat, offset_id;
    int stat_arr[DNX_DATA_MAX_CRPS_ENGINE_MAX_COUNTER_SET_SIZE * 2];
    int is_bytes = 0;
    int found, counter_id, sub_count;
    int base_engine_id;
    bcm_stat_engine_t engine;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t config;
    bcm_stat_counter_input_data_t implicit_input_data;
    bcm_stat_counter_explicit_input_data_t explicit_input_data;
    bcm_stat_counter_output_data_t output_data;
    bcm_stat_counter_value_t value_arr[DNX_DATA_MAX_CRPS_ENGINE_MAX_COUNTER_SET_SIZE * 2];

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("engine", engine_id);
    SH_SAND_GET_INT32("database", database_id);
    SH_SAND_GET_INT32("set", counter_set);
    SH_SAND_GET_INT32("type", type_id);
    SH_SAND_GET_INT32("core", core_id);

    /** check chosen options   */
    if ((engine_id != DIAG_DNX_CRPS_NO_OPTION && database_id != DIAG_DNX_CRPS_NO_OPTION)
        || (engine_id == DIAG_DNX_CRPS_NO_OPTION && database_id == DIAG_DNX_CRPS_NO_OPTION))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d for options give engine OR database \n", unit);
    }

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        database.core_id = core_idx;
        if (database_id != DIAG_DNX_CRPS_NO_OPTION)
        {
            database.database_id = database_id;
            if (_SHR_E_NONE != dnx_crps_mgmt_database_structure_verify(unit, &database, FALSE))
            {
                LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"),
                         unit, database.core_id, database.database_id));
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database_id, &engine_id));
            if (engine_id == DNX_CRPS_MGMT_ENGINE_INVALID)
            {
                LOG_CLI((BSL_META("No engines attached to database : unit=%d core_id=%d database_id=%d\n"),
                         unit, database.core_id, database.database_id));
                continue;
            }
        }
        /** engine key - find the database it belong to */
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get
                            (unit, core_idx, engine_id, &database.database_id));
        }
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid);
        if (valid == FALSE)
        {
            LOG_CLI((BSL_META("engine_id=%d / database_id=%d, core_id=%d is not active \n"),
                     engine_id, database_id, core_idx));
            continue;
        }
        engine.engine_id = engine_id;
        engine.core_id = core_idx;

        PRT_TITLE_SET("Counter values for unit=%d  core=%d engine_id=%d / database_id=%d", unit,
                      database.core_id, engine_id, database.database_id);

        SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &config));
        if (!config.type_config[type_id].valid)
        {
            PRT_INFO_ADD("for engine_id=%d type_id=%d is not valid\n", engine_id, type_id);
        }
        else
        {
            PRT_COLUMN_ADD("Offset_ID");
            PRT_COLUMN_ADD("Counter Value");
            PRT_COLUMN_ADD("Format");
            PRT_COLUMN_ADD("Counter ID");
            PRT_COLUMN_ADD("Engine ID");
            if (config.format_type == bcmStatCounterFormatPacketsAndBytes)
            {
                nstat = config.counter_set_size * 2;
            }
            else
            {
                nstat = config.counter_set_size;
            }
            output_data.value_arr = value_arr;
            /** set array */
            for (offset_id = 0; offset_id < nstat; offset_id++)
            {
                if (config.format_type == bcmStatCounterFormatPacketsAndBytes)
                {
                    /** first packets, then bytes */
                    if (offset_id >= config.counter_set_size)
                    {
                        is_bytes = 1;
                    }
                    else
                    {
                        is_bytes = 0;
                    }
                }
                stat_arr[offset_id] = BCM_STAT_COUNTER_STAT_SET(is_bytes, (offset_id % config.counter_set_size));
            }
            if (database_id == DIAG_DNX_CRPS_NO_OPTION)
            {
                explicit_input_data.engine = engine;
                explicit_input_data.object_stat_id = counter_set;
                explicit_input_data.type_id = type_id;
                explicit_input_data.nstat = nstat;
                explicit_input_data.stat_arr = stat_arr;
                SHR_IF_ERR_EXIT(bcm_stat_counter_explicit_get(unit, flags, &explicit_input_data, &output_data));
            }
            else
            {
                implicit_input_data.core_id = core_idx;
                implicit_input_data.database_id = database_id;
                implicit_input_data.command_id = config.command_id;
                implicit_input_data.counter_source_id = counter_set;
                implicit_input_data.type_id = type_id;
                implicit_input_data.nstat = nstat;
                implicit_input_data.stat_arr = stat_arr;
                SHR_IF_ERR_EXIT(bcm_stat_counter_get(unit, flags, &implicit_input_data, &output_data));
            }

            base_engine_id = engine_id; /** save the base engine id*/
            /** print*/
            for (offset_id = 0; offset_id < nstat; offset_id++)
            {
                /** First calculate the counter id and the engine id it
                 *  belongs to */
                 /** In case of  given database_id - need to calculate the
                  *  counter_id and the engine_id - do this in the offset loop
                  *  as the set might be split in two engines so the check
                  *  will be done for each offset */
                if (database_id != DIAG_DNX_CRPS_NO_OPTION)
                {
                    engine_id = base_engine_id; /** per each offset iteration start from the base engine */
                    /** to find to which engine the counters belong to need to go
                    *  over the database */
                    while (engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
                    {
                        SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_id_calc
                                        (unit, core_idx, engine_id, type_id, counter_set, stat_arr[offset_id], &found,
                                         &counter_id, &sub_count));
                        if (found)
                        {
                            break;
                        }
                        SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                        next_engine_id.get(unit, core_idx, engine_id, &engine_id));
                    }
                }
                /** In case of given engine_id - only need to calculate the counter_id */
                else
                {
                    SHR_IF_ERR_EXIT(dnx_crps_mgmt_counter_id_calc
                                    (unit, core_idx, engine_id, type_id, counter_set, stat_arr[offset_id], &found,
                                     &counter_id, &sub_count));
                }

                /** Once the calculation is ready - print the table */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                if (config.format_type == bcmStatCounterFormatPacketsAndBytes)
                {
                    if (offset_id >= config.counter_set_size)
                    {
                        PRT_CELL_SET("%d", (offset_id % config.counter_set_size));
                        PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                     COMPILER_64_LO(output_data.value_arr[offset_id].value));
                        PRT_CELL_SET("Bytes");
                    }
                    else
                    {
                        PRT_CELL_SET("%d", (offset_id % config.counter_set_size));
                        PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                     COMPILER_64_LO(output_data.value_arr[offset_id].value));
                        PRT_CELL_SET("Packets");
                    }
                }
                else if (config.format_type == bcmStatCounterFormatBytes)
                {
                    PRT_CELL_SET("%d", offset_id);
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                 COMPILER_64_LO(output_data.value_arr[offset_id].value));
                    PRT_CELL_SET("Bytes");
                }
                else if (config.format_type == bcmStatCounterFormatMaxSize)
                {
                    PRT_CELL_SET("%d", offset_id);
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                 COMPILER_64_LO(output_data.value_arr[offset_id].value));
                    PRT_CELL_SET(" -- ");
                }
                else
                {
                    PRT_CELL_SET("%d", offset_id);
                    PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(output_data.value_arr[offset_id].value),
                                 COMPILER_64_LO(output_data.value_arr[offset_id].value));
                    PRT_CELL_SET("Packets");
                }
                /** print counter id and engine id */
                PRT_CELL_SET("%d", counter_id);
                PRT_CELL_SET("%d", engine_id);
            }
        }
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_attach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    /** attach DMA FIFO */
    result = dnx_crps_dma_fifo_full_attach(unit, BCM_CORE_ALL);
    if (_SHR_E_NONE != result)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor DMA FIFO failed to attach: %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

    /** attach BG thread */
    SHR_IF_ERR_EXIT(bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, TRUE));
    LOG_CLI((BSL_META("unit %d counter processor DMA FIFO is attached and background access resumed\n"), unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_detach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** detach BG thread */
    SHR_IF_ERR_EXIT(bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, FALSE));
    result = dnx_crps_eviction_dma_fifo_full_detach(unit, BCM_CORE_ALL);
    if (BCM_E_NONE == result)
    {
        LOG_CLI((BSL_META("unit %d counter processor background access suspended and DMA FIFO is detached \n"), unit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor DMA FIFO detach failed %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_suspend_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    result = bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, FALSE);
    if (BCM_E_NONE == result)
    {
        LOG_CLI((BSL_META("unit %d counter processor background accesses suspended \n"), unit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor background suspend failed: %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_resume_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int result;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    result = bcm_stat_control_set(unit, flags, bcmStatControlCounterThreadEnable, TRUE);
    if (BCM_E_NONE == result)
    {
        LOG_CLI((BSL_META("unit %d counter processor background accesses resumed \n"), unit));
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "unit %d counter processor background accesses resume failed: %d (%s)", unit, result,
                     _SHR_ERRMSG(result));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 engine_id, flags = 0;
    int core_id, core_idx;
    int valid_engine = 0, type_id, valid_type;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    bcm_stat_engine_t engine;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t interface;
    bcm_stat_eviction_t eviction;
    dnx_engine_log_obj_id_range_t range;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("engine", engine_id);
    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid_engine);
        if (!valid_engine)
        {
            LOG_CLI((BSL_META("unit %d core=%d - engine_id=%d is not active \n"), unit, core_idx, engine_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.database_id.get
                            (unit, core_idx, engine_id, &database.database_id));
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.log_obj_id_range.get(unit, core_idx, engine_id, &range));

            database.core_id = core_idx;
            PRT_TITLE_SET(" Interface configuration");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Configuration");
            PRT_COLUMN_ADD("Value");
            SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &interface));
            engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
            /** interface*/
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("engine size");
            PRT_CELL_SET("%d", engine_info->nof_counters);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("source");
            PRT_CELL_SET("%s", source_enum_to_char_binding_table[interface.source].source_str);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("command_id");
            PRT_CELL_SET("%d", interface.command_id);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("counter_set_size");
            PRT_CELL_SET("%d", interface.counter_set_size);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("format");
            PRT_CELL_SET("%s", format_enum_to_char_binding_table[interface.format_type].format_str);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("logical_range_start");
            PRT_CELL_SET("%d", range.logical_object_id_first);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("logical_range_end");
            PRT_CELL_SET("%d", range.logical_object_id_last);

            PRT_COMMITX;
            PRT_TITLE_SET(" valid Types for the engine");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Type_ID");
            PRT_COLUMN_ADD("valid_type");
           /**  type config */
            for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", type_id);
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.type_hw_valid.get
                                (unit, core_idx, engine_id, type_id, &valid_type));
                PRT_CELL_SET("%d", valid_type);
            }
            PRT_COMMITX;
            /** data mapping is in diag "info counterset" */
            /** eviction*/
            engine.core_id = core_idx;
            engine.engine_id = engine_id;
            SHR_IF_ERR_EXIT(bcm_stat_counter_eviction_get(unit, flags, &engine, &eviction));
            PRT_TITLE_SET(" Eviction configuration");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Configuration");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("dma_fifo");
            PRT_CELL_SET("%d", eviction.dma_fifo_select);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("destination_type");
            PRT_CELL_SET("%d", eviction.type);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("record_format");
            PRT_CELL_SET("%d", eviction.record_format);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("eviction_event_id");
            PRT_CELL_SET("%d", eviction.eviction_event_id);
            PRT_COMMITX;
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_info_ctr_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 database_id, flags = 0;
    int core_id, core_idx, ind, i;
    int type_id;
    bcm_stat_engine_t engine;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t interface;
    bcm_stat_counter_interface_key_t interface_key;
    bcm_stat_expansion_select_t expansion;
    int expansion_array[DNX_DATA_MAX_CRPS_EXPANSION_EXPANSION_SIZE][2] = { {0} };
    int array_nof_elements = 0;
    int array_row_i, array_row_j, slap, temp;
    uint32 metadata_bitmap, size, offset;
    int bitmap_set_bits;
    int mem_idx, offset_id, type_index = 0;
    char data_mapping_buf[20] = "";
    char data_mapping_str[DNX_DATA_MAX_CRPS_ENGINE_DATA_MAPPING_TABLE_SIZE * 2] = "";
    bcm_stat_expansion_data_value_t value;
    const dnx_data_crps_expansion_itm_types_t *itm_types;
    const dnx_data_crps_expansion_etm_types_t *etm_types;
    const bcm_stat_expansion_types_t *valid_types;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("database", database_id);
    SH_SAND_GET_INT32("type", type_id);
    SH_SAND_GET_INT32("core", core_id);

    database.database_id = database_id;

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        database.core_id = core_idx;
        /** check that database created and specific core_id */
        if (_SHR_E_NONE != dnx_crps_mgmt_database_structure_verify(unit, &database, FALSE))
        {
            LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"),
                     unit, database.core_id, database.database_id));
            continue;
        }

        SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &interface));
        if (interface.type_config[type_id].valid != TRUE)
        {
            LOG_CLI((BSL_META
                     ("type id verification failed: unit=%d core_id=%d database_id=%d type_id=%d is not valid\n"), unit,
                     database.core_id, database.database_id, type_id));
            continue;
        }
        itm_types = dnx_data_crps.expansion.itm_types_get(unit);
        etm_types = dnx_data_crps.expansion.etm_types_get(unit);
         /** expansion is not valid for source  OAMP, so for it
         *  only the datamapping will be printed, otherwise - first print
         *  the expansion configuration */
        if (interface.source != bcmStatCounterInterfaceOamp)
        {
            PRT_TITLE_SET(" Expansion format");
            PRT_INFO_ADD("unit=%d database_id=%d core_id=%d", unit, database_id, core_idx);

            for (ind = dnx_data_crps.expansion.expansion_size_get(unit) - 1; ind >= 0; ind--)
            {
                PRT_COLUMN_ADD("Bit%d", ind);
            }
            interface_key.core_id = core_idx;
            interface_key.interface_source = interface.source;
            interface_key.command_id = interface.command_id;
            interface_key.type_id = type_id;
            if (interface.source == bcmStatCounterInterfaceIngressVoq
                || interface.source == bcmStatCounterInterfaceEgressReceiveQueue)
            {
                 /** Get the valid expansion types for source ITM/ETM */
                valid_types =
                    (interface.source ==
                     bcmStatCounterInterfaceIngressVoq) ? itm_types->valid_types : etm_types->valid_types;
                /** Go over the valid expansion types and fill the expansion
                 *  structure */
                while (valid_types[type_index] != bcmStatExpansionTypeMax)
                {
                    expansion.expansion_elements[type_index].type = valid_types[type_index];
                    type_index++;
                }
                expansion.nof_elements = type_index;
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_stat_counter_expansion_select_get(unit, flags, &interface_key, &expansion));
            }
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            array_nof_elements = 0;
            /** get total bits */
            for (ind = 0; ind < expansion.nof_elements; ind++)
            {
                /** get field size */
                SHR_IF_ERR_EXIT(dnx_crps_src_interface_data_field_params_get
                                (unit, core_idx, interface.source, interface.command_id,
                                 expansion.expansion_elements[ind].type, &size, &offset));
                /** for metadata metadata_bitmap holds the relevant metadata
                 *  bitmap, for other types it is returned from the function
                 *  together with the offset */
                if (expansion.expansion_elements[ind].type == bcmStatExpansionTypeMetaData
                    || expansion.expansion_elements[ind].type == bcmStatExpansionTypePortMetaData)
                {
                    metadata_bitmap = expansion.expansion_elements[ind].bitmap;
                    SHR_BITCOUNT_RANGE(&metadata_bitmap, bitmap_set_bits, 0, (sizeof(uint32) * 8));
                    size = bitmap_set_bits;
                }
                for (i = 0; i < size; i++)
                {
                    expansion_array[array_nof_elements][0] = offset;
                    expansion_array[array_nof_elements][1] = expansion.expansion_elements[ind].type;
                    array_nof_elements++;
                }
            }
            /** sort the array according to its offset in descending order
             *  lowest offset is in the expansion LSB */
            for (array_row_i = 0; array_row_i < array_nof_elements; array_row_i++)
            {
                for (array_row_j = array_row_i + 1; array_row_j < array_nof_elements; array_row_j++)
                {
                    if (expansion_array[array_row_i][0] < expansion_array[array_row_j][0])
                    {
                        for (slap = 0; slap < 2; slap++)
                        {
                            temp = expansion_array[array_row_i][slap];
                            expansion_array[array_row_i][slap] = expansion_array[array_row_j][slap];
                            expansion_array[array_row_j][slap] = temp;
                        }
                    }
                }
            }
            i = 0;
            for (ind = DNX_DATA_MAX_CRPS_EXPANSION_EXPANSION_SIZE; ind > 0; ind--)
            {
                if (ind > array_nof_elements)
                {
                    PRT_CELL_SET(" -- ");
                }
                else
                {
                    PRT_CELL_SET("%s", exp_type_enum_to_char_binding_table[expansion_array[i][1]].exp_type_str);
                    i++;
                }
            }
            PRT_COMMITX;
        }
        /** data mapping*/
        engine.core_id = core_idx;
        SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database_id, &engine.engine_id));

        PRT_TITLE_SET(" Data mapping configuration");
        PRT_INFO_ADD(" counter_set_size = %d", interface.counter_set_size);
        PRT_COLUMN_ADD("Counter_set offset");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Expansion Values according the Expansion format table");
        for (offset_id = 0; offset_id < interface.counter_set_size; offset_id++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", offset_id);
            /** make the row string */
            sal_memset(data_mapping_str, '\0', sizeof(data_mapping_str));
            for (mem_idx = 0; mem_idx < DNX_DATA_MAX_CRPS_ENGINE_DATA_MAPPING_TABLE_SIZE; mem_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_crps_engine_data_mapping_hw_get(unit, &engine, mem_idx, &value));
                if (value.valid == 1 && value.counter_set_offset == offset_id)
                {
                    sal_snprintf(data_mapping_buf, sizeof(data_mapping_buf), "%d ", mem_idx);
                    sal_strcat(data_mapping_str, data_mapping_buf);
                }
            }
            PRT_CELL_SET("%s", data_mapping_str);
        }
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_last_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 engine_id;
    int core_id, core_idx;
    int valid_engine = 0;
    uint32 last_address_value;
    uint64 last_data_value;
    bcm_stat_engine_t engine;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("engine", engine_id);
    SH_SAND_GET_INT32("core", core_id);

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        DNX_CRPS_MGMT_ENGINE_VALID_GET(unit, core_idx, engine_id, valid_engine);
        if (!valid_engine)
        {
            LOG_CLI((BSL_META("unit %d core=%d - engine_id=%d is not active \n"), unit, core_idx, engine_id));
        }
        else
        {
            engine.engine_id = engine_id;
            engine.core_id = core_idx;
            SHR_IF_ERR_EXIT(dnx_crps_engine_last_command_hw_get(unit, &engine, &last_address_value, &last_data_value));
            PRT_TITLE_SET(" LAST COMMAND");
            PRT_INFO_ADD("unit=%d engine_id=%d core_id=%d", unit, engine_id, core_idx);
            PRT_COLUMN_ADD("Field");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Last address admitted");
            PRT_CELL_SET("%d", last_address_value);
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Last data admitted");
            PRT_CELL_SET("0x%08x%08x", COMPILER_64_HI(last_data_value), COMPILER_64_LO(last_data_value));
            PRT_COMMITX;
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_show_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int database_id, engine_id = 0;
    int current_engine_id, base_engine_id;
    int core_id, core_idx, type_id;
    uint32 flags = 0;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int total_size = 0;
    bcm_stat_counter_database_t database;
    bcm_stat_counter_interface_t interface;
    int first_database, last_database;
    uint8 is_created;
    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("database", database_id);
    SH_SAND_GET_INT32("core", core_id);

    if (database_id == -1)
    {
        first_database = 0;
           /** nof databases is the same as nof engines */
        last_database = dnx_data_crps.engine.nof_engines_get(unit) - 1;
    }
    else
    {
        first_database = database_id;
        last_database = database_id;
    }

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        for (database_id = first_database; database_id <= last_database; database_id++)
        {
            database.database_id = database_id;
            database.core_id = core_idx;

            SHR_IF_ERR_EXIT(dnx_crps_db.database_res.is_allocated
                            (unit, database.core_id, database.database_id, &is_created));
            if (is_created == FALSE)
            {
                LOG_CLI((BSL_META("No content for database : unit=%d core_id=%d database_id=%d\n"),
                         unit, database.core_id, database.database_id));
                continue;
            }

            PRT_TITLE_SET(" Engines in the database");
            PRT_INFO_ADD("unit=%d database_id=%d core_id=%d", unit, database_id, core_idx);
            PRT_COLUMN_ADD("Engine_id");
            PRT_COLUMN_ADD("Engine size");

            SHR_IF_ERR_EXIT(dnx_crps_db.database.base_engine.get(unit, core_idx, database_id, &base_engine_id));
            current_engine_id = base_engine_id;

            /**    go over the linked list of the database */
            while (current_engine_id != DNX_CRPS_MGMT_ENGINE_INVALID)
            {
                engine_id = current_engine_id;
                engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", engine_id);
                PRT_CELL_SET("%d", engine_info->nof_counters);
                total_size += engine_info->nof_counters;
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.interface.
                                next_engine_id.get(unit, core_idx, engine_id, &current_engine_id));
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Total size: ");
            PRT_CELL_SET("%d", total_size);
            PRT_COMMITX;

            SHR_IF_ERR_EXIT(bcm_stat_counter_interface_get(unit, flags, &database, &interface));
            /** interface*/
            PRT_TITLE_SET(" Type configuration");
            PRT_INFO_ADD("unit=%d database_id=%d core_id=%d", unit, database_id, core_idx);
            PRT_INFO_ADD("counter_set_size=%d", interface.counter_set_size);
            PRT_COLUMN_ADD("Type_ID");
            PRT_COLUMN_ADD("Type_Offset");
            PRT_COLUMN_ADD("Start");
            PRT_COLUMN_ADD("End");
            /**  type config */
            for (type_id = 0; type_id < BCM_STAT_MAX_NUMBER_OF_OBJECT_TYPES; type_id++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", type_id);
                if (interface.type_config[type_id].valid)
                {
                    PRT_CELL_SET("%d", interface.type_config[type_id].object_type_offset);
                    PRT_CELL_SET("%d", interface.type_config[type_id].start);
                    PRT_CELL_SET("%d", interface.type_config[type_id].end);
                }
                else
                {
                    PRT_CELL_SET(" -- ");
                    PRT_CELL_SET(" -- ");
                    PRT_CELL_SET(" -- ");
                }
            }
            PRT_COMMITX;
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sh_enum_crps_engine[] = {
    {"max", 0, "Maximum engine number", "DNX_DATA.crps.engine.nof_engines-1"},
    {NULL}
};

static sh_sand_option_t dnx_crps_get_arguments[] = {
    {"engine", SAL_FIELD_TYPE_INT32, "engine_id", "-1"},
    {"database", SAL_FIELD_TYPE_INT32, "database_id", "-1"},
    {"set", SAL_FIELD_TYPE_INT32, "counter set index", NULL},
    {"type", SAL_FIELD_TYPE_INT32, "type id", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_get_man = {
    .brief = "get values of counters in a specific sw counter set.",
    .full = "'Get' command gets for a specific counter set all uin64 counter values in it. \n",
    .synopsis = NULL,
    .examples = "engine=2 set=2 type=2 core=1\n" "database=2 set=2 type=3 core=0\n" "engine=5 set=55 type=3 core=1"
};

static sh_sand_man_t sh_dnx_crps_attach_man = {
    .brief = "attach the DMA FIFO and BG thread \n",
    .full = "first attach all connected dma fifos, then attach background thread"
};

static sh_sand_man_t sh_dnx_crps_detach_man = {
    .brief = "detach the DMA FIFO and BG thread \n",
    .full = "first detach the background thread, then detach the connected dma fifos"
};

static sh_sand_man_t sh_dnx_crps_suspend_man = {
    .brief = "suspend the BG thread operation \n",
    .full = "suspend the background thread operation"
};

static sh_sand_man_t sh_dnx_crps_resume_man = {
    .brief = "resume the BG thread operation \n",
    .full = "resume the background thread operation"
};
static sh_sand_option_t dnx_crps_info_arguments[] = {
    {"engine", SAL_FIELD_TYPE_UINT32, "engine_id", NULL, sh_enum_crps_engine, "0-max"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_info_man = {
    .brief = "info - show engine configuration \n",
    .full = "with option engine - 3 tables - interface/type config/eviction \n"
        "command id, set size, source type, format type, database id, next engine \n"
        "valid types with offset, start, end \n" "dma fifo, event id, record format, destination"
        "with option counter_set - 2 tables - expnasion/ data mapping"
        "with option database - info for the database range",
    .synopsis = NULL,
    .examples = "crps info database database=15 core=1"
};

static sh_sand_man_t sh_dnx_crps_info_engine_man = {
    .brief = "info - show engine configuration \n",
    .full = "show engine configuration - 3 tables - interface/type config/eviction \n"
        "command id, set size, source type, format type, database id, next engine \n"
        "valid types with offset, start, end \n" "dma fifo, event id, record format, destination",
    .synopsis = NULL,
    .examples = "engine=2 core=0"
};

static sh_sand_option_t dnx_crps_info_ctr_set_arguments[] = {
    {"database", SAL_FIELD_TYPE_UINT32, "database_id", NULL},
    {"type", SAL_FIELD_TYPE_INT32, "type_id", NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_info_ctr_set_man = {
    .brief = "show database configuration - expansion and data mapping \n",
    .full = "show data mapping configuration \n",
    .synopsis = NULL,
    .examples = "database=2 type=3 core=0\n" "database=2 type=1 core=1\n" "database=7 type=2 core=0"
};

static sh_sand_option_t dnx_crps_last_options[] = {
    {"engine", SAL_FIELD_TYPE_UINT32, "engine_id", NULL, sh_enum_crps_engine, "0-max"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_last_man = {
    .brief = "last - show last address and data admitted per engine \n",
    .full = "",
    .synopsis = NULL,
    .examples = "engine=2 core=0\n" "engine=21 core=1"
};

static sh_sand_option_t dnx_crps_show_options[] = {
    {"database", SAL_FIELD_TYPE_INT32, "database_id", "-1"},
    {NULL}
};

static sh_sand_man_t sh_dnx_crps_show_man = {
    .brief = "show all engines in database and the types range \n",
    .full =
        "show all engines in database and the types range. \nif no databse_id give, it prsent the information for all configured databased",
    .synopsis = NULL,
    .examples = "database=21 core=0\n" "core=1"
};

static sh_sand_cmd_t sh_dnx_crps_info_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"engine", sh_dnx_info_cmd, NULL, dnx_crps_info_arguments, &sh_dnx_crps_info_engine_man},
    {"counterSet", sh_dnx_info_ctr_set_cmd, NULL, dnx_crps_info_ctr_set_arguments, &sh_dnx_crps_info_ctr_set_man},
    {"database", sh_dnx_show_cmd, NULL, dnx_crps_show_options, &sh_dnx_crps_show_man, NULL, NULL},
    {NULL}
};

/**
 * \brief DNX CRPS diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for CRPS diagnostic commands 
 */
sh_sand_cmd_t sh_dnx_crps_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"Get", sh_dnx_get_counter_cmd, NULL, dnx_crps_get_arguments, &sh_dnx_crps_get_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {"ATTach", sh_dnx_attach_cmd, NULL, NULL, &sh_dnx_crps_attach_man},
    {"DETach", sh_dnx_detach_cmd, NULL, NULL, &sh_dnx_crps_detach_man},
    {"SuSPenD", sh_dnx_suspend_cmd, NULL, NULL, &sh_dnx_crps_suspend_man},
    {"ReSuMe", sh_dnx_resume_cmd, NULL, NULL, &sh_dnx_crps_resume_man},
    {"INFO", NULL, sh_dnx_crps_info_cmds, NULL, &sh_dnx_crps_info_man},
    {"Last", sh_dnx_last_cmd, NULL, dnx_crps_last_options, &sh_dnx_crps_last_man},
    {NULL}
};

sh_sand_man_t sh_dnx_crps_man = {
    .brief = "CRPS diagnostics and commands \n"
};
