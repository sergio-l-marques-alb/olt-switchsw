/** \file init_time_analyzer.h
 * 
 * Define the modules running time to be analyzed in order to get better decisions when working on time optimization.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DNX_INIT_TIME_ANALYZER_H_INCLUDED
#define DNX_INIT_TIME_ANALYZER_H_INCLUDED
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/time.h>
#include <shared/utilex/utilex_time_analyzer.h>

/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief - set to 1 to enable. disabled by a compilation to optimize running time
 */
#define DNX_INIT_TIME_ANALYZER  0
#define DNX_ERROR_RECOVERY_TIME_DIAGNOSTICS 0

/*
 * regular init time analyzer diagnostics are disabled when error recovery diagnostics are enabled
 */
#if (DNX_INIT_TIME_ANALYZER && !DNX_ERROR_RECOVERY_TIME_DIAGNOSTICS)
#define DNX_INIT_TIME_ANALYZER_START(unit, module_id)       utilex_time_analyzer_module_start(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_STOP(unit, module_id)        utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_INIT_TIME_ANALYZER_START(unit, module_id)
#define DNX_INIT_TIME_ANALYZER_STOP(unit, module_id)
#endif

/*
 * error recovery diagnostics require init time analyzer diagnostics
 */
#if (DNX_INIT_TIME_ANALYZER && DNX_ERROR_RECOVERY_TIME_DIAGNOSTICS)
#define DNX_ERROR_RECOVERY_TIME_DIAG_START(unit, module_id)     utilex_time_analyzer_module_start(unit, module_id)
#define DNX_ERROR_RECOVERY_TIME_DIAG_END(unit, module_id)       utilex_time_analyzer_module_stop(unit, module_id)
#else
#define DNX_ERROR_RECOVERY_TIME_DIAG_START(unit, module_id)
#define DNX_ERROR_RECOVERY_TIME_DIAG_END(unit, module_id)
#endif

/*
 * }
 */

/*
 * TYPEDEFs
 * {
 */
/**
 * \brief - all the modules / operation tracked
 * Note: must be synced with the switch case in dnx_init_time_analyzer_module_name_get()
 */
typedef enum
{
    DNX_INIT_TIME_ANALYZER_DBAL_SET,
    DNX_INIT_TIME_ANALYZER_DBAL_GET,
    DNX_INIT_TIME_ANALYZER_DBAL_CLEAR,
    DNX_INIT_TIME_ANALYZER_DBAL_ENTRY_CLEAR,
    DNX_INIT_TIME_ANALYZER_DBAL_ITER,
    DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET,
    DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET,
    DNX_INIT_TIME_ANALYZER_DBAL_FIELD_REQUEST,
    DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP,
    DNX_INIT_TIME_ANALYZER_DNX_DATA,
    DNX_INIT_TIME_ANALYZER_SW_STATE,
    DNX_INIT_TIME_ANALYZER_RES_MNGR,
    DNX_INIT_TIME_ANALYZER_TEMPLATE_MNGR,
    DNX_INIT_TIME_ANALYZER_MEM_INIT,
    DNX_INIT_TIME_ANALYZER_MEM_ZEROS,
    DNX_INIT_TIME_ANALYZER_ER_API_FULL,
    DNX_INIT_TIME_ANALYSER_DBAL_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_VALIDATION, 
    DNX_INIT_TIME_ANALYSER_DBAL_FIELD_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_MDB_ACCESS_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_SW_ACCESS_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_ACCESS_INIT, 
    
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_ALLOC_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_HL_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_SW_STATE_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_TCAM_CONTEXT_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_PEMLA_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_MDB_INIT, 
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_FREE_INIT, 

    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_CLEAR,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_INTERACE_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_APP_TO_PHY_INIT,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE,

    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_INTERFACE,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG,

    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_REG_ACCESS,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_MEM_ACCESS,
    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_PARSING_OFFSETS_PARSE,

    DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_HW_ENTITY_STR,


    DNX_INIT_TIME_ANALYZER_NOF
} dnx_init_time_analyzer_module_e;
/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */
/**
 * \brief - allocate memory, initialize time analyzer tool and adds the required modules to be tracked
 * \param [in] unit - unit #.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_time_analyzer_init(
    int unit);
/**
 * \brief - free time analyzer tool resources
 * \param [in] unit - unit #.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_init_time_analyzer_deinit(
    int unit);
/**
 * \brief - flag cb for init sequence that allows to skip the module
 * \param [in] unit - unit #.
 * * \param [out] flags - init sequnce flags
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_time_analyzer_flag_cb(
    int unit,
    int *flags);
/*
 * }
 */

#endif /* DNX_INIT_TIME_ANALYZER_H_INCLUDED */
