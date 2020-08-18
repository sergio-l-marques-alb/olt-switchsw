/** \file init_time_analyzer.c
 *
 * Define the modules running time to be analyzed in order to get better decisions when working on time optimization.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>

#include <shared/utilex/utilex_time_analyzer.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
/*
 * }
 */

/**
 * \brief - module_id to name string
 */
shr_error_e
dnx_init_time_analyzer_module_name_get(
    int unit,
    int module_id,
    char **module_name)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (module_id)
    {
        case DNX_INIT_TIME_ANALYZER_DBAL_SET:
            *module_name = "DBAL_SET";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_GET:
            *module_name = "DBAL_GET";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_CLEAR:
            *module_name = "DBAL_CLEAR";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_ENTRY_CLEAR:
            *module_name = "DBAL_ENTRY_CLEAR";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_ITER:
            *module_name = "DBAL_ITER";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET:
            *module_name = "DBAL_FIELD_SET";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET:
            *module_name = "DBAL_FIELD_GET";
            break;
        case DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST:
            *module_name = "DBAL_FIELD_REQ";
            break;
        case DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP:
            *module_name = "PORT_BITMAP";
            break;
        case DNX_INIT_TIME_ANALYZER_DNX_DATA:
            *module_name = "DNX_DATA";
            break;
        case DNX_INIT_TIME_ANALYZER_SW_STATE:
            *module_name = "SW_STATE";
            break;
        case DNX_INIT_TIME_ANALYZER_RES_MNGR:
            *module_name = "RES_MNGR";
            break;
        case DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR:
            *module_name = "TEMP_MNGR";
            break;
        case DNX_INIT_TIME_ANALYZER_MEM_INIT:
            *module_name = "MEM_INIT";
            break;
        case DNX_INIT_TIME_ANALYZER_MEM_ZEROS:
            *module_name = "MEM_ZEROS";
            break;
        case DNX_INIT_TIME_ANALYZER_ER_API_FULL:
            *module_name = "ER_API_FULL";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_INIT:
            *module_name = "DBAL_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT:
            *module_name = "DBAL_LOGICAL_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_VALIDATION:
            *module_name = "DBAL_LOGICAL_INIT_VALIDATION";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_FIELD_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_FIELD_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_MDB_ACCESS_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_MDB_ACCESS_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_SW_ACCESS_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_SW_ACCESS_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_ACCESS_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_ACCESS_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_ALLOC_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_ALLOC_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_HL_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_HL_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_SW_STATE_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_SW_STATE_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_TCAM_CONTEXT_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_TCAM_CONTEXT_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_PEMLA_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_PEMLA_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_MDB_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_MDB_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_FREE_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_FREE_INIT";
            break;

        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_CLEAR:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_CLEAR";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_INTERACE_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_INTERACE_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_APP_TO_PHY_INIT:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_APP_TO_PHY_INIT";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE:
            *module_name = "DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE";
            break;

        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_INTERFACE:
            *module_name = "DBAL_TABLE_ADD_INTERFACE";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL:
            *module_name = "DBAL_TABLE_MAPPING_HL";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG:
            *module_name = "DBAL_TABLE_MAPPING_HL_MEM_REG";
            break;

        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_REG_ACCESS:
            *module_name = "DBAL_TABLE_MAPPING_HL_MEM_REG_REG_ACCESS";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_MEM_ACCESS:
            *module_name = "DBAL_TABLE_MAPPING_HL_MEM_REG_MEM_ACCESS";
            break;
        case DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_PARSING_OFFSETS_PARSE:
            *module_name = "DBAL_TABLE_MAPPING_HL_MEM_REG_PARSING_OFFSETS_PARSE";
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "unknown module id\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_init_time_analyzer_init(
    int unit)
{
#if DNX_INIT_TIME_ANALYZER
    int module_id;
    char *name;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_time_analyzer_init(unit));

    for (module_id = 0; module_id < DNX_INIT_TIME_ANALYZER_NOF; module_id++)
    {
        SHR_IF_ERR_EXIT(dnx_init_time_analyzer_module_name_get(unit, module_id, &name));
        SHR_IF_ERR_EXIT(utilex_time_analyzer_module_add(unit, module_id, name));
    }
exit:
    SHR_FUNC_EXIT;
#else
    return _SHR_E_NONE;
#endif
}

/*
 * See .h file
 */
shr_error_e
dnx_init_time_analyzer_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_time_analyzer_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_time_analyzer_flag_cb(
    int unit,
    int *flags)
{
    SHR_FUNC_INIT_VARS(unit);

#if DNX_INIT_TIME_ANALYZER
    *flags = 0;
    if (dnx_data_dev_init.time.analyze_get(unit) == UTILEX_TIME_ANALYZER_MODE_OFF)
    {
        *flags = DNX_INIT_STEP_F_SKIP | DNX_INIT_STEP_F_VERBOSE;
    }
#else
    *flags = DNX_INIT_STEP_F_SKIP | DNX_INIT_STEP_F_VERBOSE;
#endif

    SHR_FUNC_EXIT;
}
