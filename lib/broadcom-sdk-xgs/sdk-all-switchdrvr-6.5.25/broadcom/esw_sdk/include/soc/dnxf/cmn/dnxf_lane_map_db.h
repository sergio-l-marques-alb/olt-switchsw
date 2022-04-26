/** \file dnxf_lane_map_db.h
 *  Manages lane map (swap) database stored in SW state.
 *  APIs:
 *  1) Get per lane whether it's mapped or not
 *  2) Get per link the mapped fmac lane
 *  3) Get per fmac lane the mapped link
 *
 *  NOTE: Additional more device specific information can be found in the SOC layer header
 *
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
 *  dnxf_state.lane_map_db.serdes_map.lane2serdes [0] -> {0, 1}
 *  dnxf_state.lane_map_db.serdes_map.lane2serdes [1] -> {-1, -1}
 *  dnxf_state.lane_map_db.serdes_map.lane2serdes [2] -> {-1, -1}
 *  dnxf_state.lane_map_db.serdes_map.lane2serdes [3] -> {3, 2}
 *
 *  2) serdes2lane configuration in SW database:
 *  dnxf_state.lane_map_db.serdes_map.serdes2lane [0] -> {0, -1}
 *  dnxf_state.lane_map_db.serdes_map.serdes2lane [1] -> {-1, 0}
 *  dnxf_state.lane_map_db.serdes_map.serdes2lane [2] -> {-1, 3}
 *  dnxf_state.lane_map_db.serdes_map.serdes2lane [3] -> {3, -1}
 *
 */

/*
 * $Copyright: (c) 2021 Broadcom.
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
 *    Validate input given to soc_dnxf_lane_map_db_map_set/get functions
 *    1. lane2serdes ptr isn't NULL
 *    2. map_size parameter (size of lane2serdes) is the same as SW state array size
 * \param [in] unit -
 *    unit #
 * \param [in] map_size - number of lanes-
 *    1. size of SW state database of the given type for the unit
 *    2. size of lane2serdes array
 * \param [in] lane2serdes -
 *    array to set/get lane mapping stored in SW state
 * \return
 *    see shr_error_e
 * \remark
 *    * None
 * \see
 *    * None
 */
shr_error_e soc_dnxf_lane_map_db_map_input_check(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

/**
 * \brief
 * To return the fmac_lane corresponding to the logical link
 *
 * The opposite function of soc_dnxf_lane_map_db_fmac_lane_to_link_get
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] link-
 *   logical port or link.
 * \param [out] fmac_lane
 *   fmac_lane corresponding to the logical link
 * \return
 * \remark
 * \see
 */
shr_error_e soc_dnxf_lane_map_db_link_to_fmac_lane_get(
    int unit,
    bcm_port_t link,
    int *fmac_lane);

/**
 * \brief
 * To return the logical link corresponding to the fmac_lane
 *
 * The opposite function of soc_dnxf_lane_map_db_link_to_fmac_lane_get
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmac_lane
 *   fmac_lane
 * \param [out] link-
 *   logical port corresponding to the fmac_lane
 * \return
 * \remark
 * \see
 */
shr_error_e soc_dnxf_lane_map_db_fmac_lane_to_link_get(
    int unit,
    int fmac_lane,
    bcm_port_t * link);

#endif /*_SOC_DNXF_LANE_MAP_DB_H_*/
