/** \file dnxf_lane_map_db.h
 *  Manages lane map (swap) database stored in SW state.
 *  APIs:
 *  1) Set/get the whole database
 *  2) Get a part of the database to configure a single port macro
 *  3) Get per lane whether it's mapped or not
 *
 *  A user configures the following lane map for all fabric ports: lane -> mapped to {serdes rx, serdes tx}
 *  A lane can be either mapped or unmapped for both rx and tx.
 *
 *  Each lane and serdes is considered to have rx and tx side.
 *  The SW state database is bidirectional:
 *  1) lane2serdes: holds configuration as given by the user: lane -> {serdes rx, serdes tx}
 *  2) serdes2lane: holds the reversed configuration of 1): serdes -> {lane rx, lane tx}
 *
 *  Example:
 *  --------
 *  1) Configuration from user:
 *      lane              serdes rx          serdes tx
 *     ------             ---------          ---------
 *       0                    0                  1
 *       1                NOT_MAPPED (-1)    NOT_MAPPED (-1)
 *       2                NOT_MAPPED (-1)    NOT_MAPPED (-1)
 *       3                    3                  2
 *
 *  2) lane2serdes configuration in SW database:
 *  dnxf_state.lane_map_db.lane2serdes [0] -> {0, 1}
 *  dnxf_state.lane_map_db.lane2serdes [1] -> {-1, -1}
 *  dnxf_state.lane_map_db.lane2serdes [2] -> {-1, -1}
 *  dnxf_state.lane_map_db.lane2serdes [3] -> {3, 2}
 *
 *  2) serdes2lane configuration in SW database:
 *  dnxf_state.lane_map_db.serdes2lane [0] -> {0, -1}
 *  dnxf_state.lane_map_db.serdes2lane [1] -> {-1, 0}
 *  dnxf_state.lane_map_db.serdes2lane [2] -> {-1, 3}
 *  dnxf_state.lane_map_db.serdes2lane [3] -> {3, -1}
 *
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */


#ifndef _SOC_DNXF_LANE_MAP_DB_H_
#define _SOC_DNXF_LANE_MAP_DB_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF generation only!"
#endif

/*
 * include files
 * {
 */
#include <soc/dnxc/error.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
/*
 * }
 */


/*
 * defines
 * {
 */
#define SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED (-1)
/*
 * }
 */


/**
 * \brief
 *    Fill the WHOLE lane map SW state database for a given unit.
 *    Performs input legality validation.
 *    The input given is lane-> {serdes rx, serdes tx} mapping
 *    The SW state database is bidirectional:
 *    1. lane -> serdes rx, serdes tx
 *    2. serdes -> lane rx, lane tx
 *
 * \param [in] unit -
 *    unit #
 * \param [in] map_size -
 *    number of lanes-
 *            1. size of SW state database for the unit
 *            2. size of lane2serdes array
 * \param [in] lane2serdes -
 *    array of a new lane mapping to set to SW state
 * \param [in] is_init_sequence -
 *    whether this function is called for the first time (e.g. in init sequence)
 *    in that case no need to check if a mapping of an active port has changed,
 *    because ports are not active and also this is the first time mapping is configured
 * \return
 *   see shr_error_e
 * \remark
 *  The SW state database was allocated outside of this module
 * \see
 *  Example of SW state database configuration at file header above
 */
shr_error_e
soc_dnxf_lane_map_db_map_set(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t* lane2serdes,
    int is_init_sequence);

/**
 * \brief
 *    Get the WHOLE lane map SW state database for a given unit in the following form:
 *    lane -> {serdes rx, serdes tx}
 *
 * \param [in] unit -
 *    unit #
 * \param [in] map_size -
 *    number of lanes-
 *            1. size of SW state database for the unit
 *            2. size of lane2serdes array
 * \param [out] lane2serdes -
 *    lane map: lane2serdes[lane] -> {serdes rx, serdes tx} as stored in SW state database.
 * \return
 *    see shr_error_e
 * \remark
 *    * None
 * \see
 *    Example of SW state database configuration at file header above
 */
shr_error_e
soc_dnxf_lane_map_db_map_get(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t* lane2serdes);

/**
 * \brief
 *    Get a part of lane map SW state database for a single portmacro, for a given unit.
 *    Get the lane map in the following form:
 *    lane2serdes[lane] -> {serdes rx, serdes tx}
 * \param [in] unit -
 *    unit #
 * \param [in] pm_index -
 *    zero-based index of the fabric portmacro
 * \param [in] nof_serdeses -
 *    number of serdeses represented by the portmacro and size of lane2serdes array
 * \param [out] lane2serdes -
 *    array to get lane mapping, relevant to a specific pm, as stored in SW state.
 *    Unmapped lanes entries in SW state will be filled with "dummy" mapping (unmapped serdeses) in output array.
 * \return
 *    see shr_error_e
 * \remark
 *    * None
 * \see
 *    * None
 */
shr_error_e
soc_dnxf_lane_map_db_pm_map_get(
    int unit,
    int pm_index,
    int nof_ports,
    soc_dnxc_lane_map_db_map_t* lane2serdes);


/**
 * \brief
 *    For a given lane- get if it's mapped or unmapped (for both rx and tx).
 *
 * \param [in] unit -
 *    unit #
 * \param [in] lane -
 *    lane index
 * \param [out] is_mapped -
 *    1 if lane is mapped, 0 if not mapped (SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)
 * \return
 *    see shr_error_e
 * \remark
 *    * None
 * \see
 *    * None
 */
shr_error_e
soc_dnxf_lane_map_db_is_lane_mapped_get(
    int unit,
    int lane,
    int* is_lane_mapped);

#endif /*_SOC_DNXF_LANE_MAP_DB_H_*/
