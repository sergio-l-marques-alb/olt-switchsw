/**
 * \file diag_dnx_switch.h
 *
 * External declarations for 'switch-related' command functions and
 * their associated usage strings.
 * We currently have, under switch,
 *   load_balancing
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNX_SWITCH_H_INCLUDED
/* { */
#define DIAG_DNX_SWITCH_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_switch.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/switch/switch_load_balancing.h>
/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROS  *
 *************/

/**************
 * Prototypes *
 **************/
/* 
 * {
 */
/**
 * \brief
 *   Level of detail of display level on 'switch' menus.
 */
typedef enum
{
    FIRST_DISPLAY_LEVEL = 0,
  /**
   * Display most details but drop legends.
   */
    DISPLAY_LEVEL_MEDIUM = FIRST_DISPLAY_LEVEL,
  /**
   * Display maximal detail level. Nothing is missing.
   */
    DISPLAY_LEVEL_HIGH,
    NUM_DIAPLY_LEVELS
} display_level_e;
/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
extern sh_sand_cmd_t Sh_dnx_switch_cmds[];
extern sh_sand_man_t Sh_dnx_switch_man;
extern sh_sand_enum_t Switch_load_balancing_enum_table_for_display_level[];

/**************
 * Prototypes *
 **************/
/* 
 * {
 */
/**
 * \brief
 *  This diagnostics function will display information on reserved labels,
 *  as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
shr_error_e diag_dnx_switch_lb_display_reserved_labels(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control);
/**
 * \brief
 *  This diagnostics function will display information on MPLS protocol identification,
 *  as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
shr_error_e diag_dnx_switch_lb_display_mpls_identification(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control);
/**
 * \brief
 *  This diagnostics function will display information on load balancing clients and the corresponding
 *  crc functions, as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
shr_error_e diag_dnx_switch_lb_display_clients_and_crc_functions(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control);
/**
 * \brief
 *  This diagnostics function will display information on crc functions (8) and the corresponding
 *  '16-bits seed', that is used for their calculations, as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
shr_error_e diag_dnx_switch_lb_display_crc_seed_per_function(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control);
/**
 * \brief
 *  This diagnostics function will display information on general crc seeds used for data collection
 *  and compression for MPLS stacks and for parser, as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
shr_error_e diag_dnx_switch_lb_display_crc_general_seeds(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control);
/*
 * }
 */
/* } */
#endif /* DIAG_DNX_SWITCH_H_INCLUDED */
