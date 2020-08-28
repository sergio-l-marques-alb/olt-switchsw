/** \file dnxf_lane_map_db.c
 *  See h file
 */

/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT

#ifdef PORTMOD_SUPPORT

/*
 * include files.
 * {
 */
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxc/dnxc_port.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
/*
 * }
 */


/*
 * defines
 * {
 */

/*
 * macros to get first and last port in pm, based on pm index.
 * needed to get the relevant entries from lane map SW state database, which holds data for all ports.
 */
#define SOC_DNXF_LANE_MAP_DB_PM_LOWER_BOUNDRY_GET(unit, pm_index, ports_per_pm) \
        (pm_index * ports_per_pm)

#define SOC_DNXF_LANE_MAP_DB_PM_UPPER_BOUNDRY_GET(unit, pm_index, ports_per_pm) \
        ((pm_index+1) * ports_per_pm - 1)

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
static shr_error_e
soc_dnxf_lane_map_db_map_input_check(
        int unit,
        int map_size,
        soc_dnxc_lane_map_db_map_t* lane2serdes)
{
    int sw_state_map_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lane2serdes, _SHR_E_PARAM, "lane2serdes");

    /** Validate given map_size is correct- same as stored in SW state */
    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.map_size.get(unit, &sw_state_map_size));
    if (map_size != sw_state_map_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid map_size parameter %d. map_size is supposed to be %d\n", map_size, sw_state_map_size);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
soc_dnxf_lane_map_db_map_set(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t* lane2serdes,
    int is_init_sequence)
{
    int lane, i;
    soc_dnxc_lane_map_db_map_t serdes2lane[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];
    int lane_swap_bound;
    int lane_core, serdes_tx_core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_map_input_check(unit, map_size, lane2serdes));

    /** get nof lanes within which swap is allowed */
    lane_swap_bound = dnxf_data_port.lane_map.lane_map_bound_get(unit);

    /** set all serdes_rx and serdes_tx to be unmapped first */
    for (i = 0; i < DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS; ++i)
    {
        serdes2lane[i].rx_id = SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED;
        serdes2lane[i].tx_id = SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED;
    }

    /*
     * validate lane2serdes input array values are legal
     */
    for (lane = 0; lane < map_size; ++lane)
    {
        int serdes_rx = lane2serdes[lane].rx_id;
        int serdes_tx = lane2serdes[lane].tx_id;

        /** for fabric mapped lane: lane and serdes rx are always 1:1 */
        if ((serdes_rx != lane) && (serdes_rx != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: rx lane mapping should be one-to-one. Invalid serdes rx id %d\n", lane, serdes_rx);
        }

        if((serdes_rx == SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED && serdes_tx != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED) ||
                (serdes_rx != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED && serdes_tx == SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "lane %d: a lane must be mapped/umapped for BOTH rx and tx. Serdes rx id is %d, serdes tx id is %d\n", lane, serdes_rx, serdes_tx);
        }

        /** check that serdes rx and tx values are allowed */
        if ((serdes_rx >= map_size) || ((serdes_rx < 0) && (serdes_rx != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: invalid serdes rx id %d\n", lane, serdes_rx);
        }
        if ((serdes_tx >= map_size) || ((serdes_tx < 0) && (serdes_tx != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: invalid serdes tx id %d\n", lane, serdes_tx);
        }

        if (serdes_rx != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)
        {
            /** if the tx of this serdes was already mapped to another lane */
            if (serdes2lane[serdes_tx].tx_id != SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: double mapping: serdes tx id %d is already mapped to lane %d\n", lane, serdes_tx, serdes2lane[serdes_tx].tx_id);
            }

            /** validate swap is done within allowed nof lanes (quad) */
            lane_core = lane / lane_swap_bound;
            serdes_tx_core = serdes_tx / lane_swap_bound;

            if (serdes_tx_core != lane_core)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Swap is out of bounds: lane %d, serdes tx id %d. Swap is allowed only within quad bounds\n", lane, serdes_tx);
            }

            serdes2lane[serdes_rx].rx_id = lane;
            serdes2lane[serdes_tx].tx_id = lane;
        }

        /** if this function is called via API, need to make sure that the mapping of active ports remains unchanged.
         *  there can be a further PM-specific restriction */
        if (!is_init_sequence)
        {
            soc_dnxc_lane_map_db_map_t prev_lane2serdes;
            SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.lane2serdes.get(unit, lane, &prev_lane2serdes));
            /** if tx swap of an active lane is changed */
            if ((serdes_tx != prev_lane2serdes.tx_id) && SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), lane))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Mapping of active port is forbidden. Lane %d is active\n", lane);
            }
        }
    }

    /*
     * Configure SW state database
     */
    for (i = 0; i < map_size; ++i)
    {
        /** lane to serdes mapping - rx, tx */
        SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.lane2serdes.set(unit, i, &lane2serdes[i]));
        /** serdes to lane mapping - rx, tx */
        SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.serdes2lane.set(unit, i, &serdes2lane[i]));
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * See .h file
 */
shr_error_e
soc_dnxf_lane_map_db_map_get(
    int unit,
    int map_size,
    soc_dnxc_lane_map_db_map_t* lane2serdes)
{
    int lane;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_map_input_check(unit, map_size, lane2serdes));

    for (lane = 0; lane < map_size; ++lane)
    {
        SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.lane2serdes.get(unit, lane, &lane2serdes[lane]));
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * See .h file
 */
shr_error_e
soc_dnxf_lane_map_db_pm_map_get(
    int unit,
    int pm_index,
    int nof_ports,
    soc_dnxc_lane_map_db_map_t* lane2serdes)
{
    int pm_lower_boundry, pm_upper_boundry;
    int lane, serdes;
    int unmapped_tx_serdeses_arr[DNXC_PORT_FABRIC_LANES_PER_CORE] = {0};
    int nof_unmapped_tx_serdeses = 0, unmapped_tx_ind = 0;
    soc_dnxc_lane_map_db_map_t serdes2lane_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** input check */
    SHR_NULL_CHECK(lane2serdes, _SHR_E_PARAM, "lane2serdes");
    if (nof_ports != DNXC_PORT_FABRIC_LANES_PER_CORE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_ports != DNXC_PORT_FABRIC_LANES_PER_CORE\n");
    }

    /** get first and last port = [lane or serdes] of the pm */
    pm_lower_boundry = SOC_DNXF_LANE_MAP_DB_PM_LOWER_BOUNDRY_GET(unit, pm_index, nof_ports);
    pm_upper_boundry = SOC_DNXF_LANE_MAP_DB_PM_UPPER_BOUNDRY_GET(unit, pm_index, nof_ports);

    /** construct an array holding all unmapped serdeses numbers of the pm (zero-based)-
     * to be later used to fill holes where lane is not mapped */
    for (serdes = pm_lower_boundry; serdes <= pm_upper_boundry; ++serdes)
    {
        SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.serdes2lane.get(unit, serdes, &serdes2lane_entry));
        /** this lane has to be unmapped for both rx and tx */
        if(serdes2lane_entry.tx_id == SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)
        {
            unmapped_tx_serdeses_arr[nof_unmapped_tx_serdeses] = serdes % nof_ports;
            ++nof_unmapped_tx_serdeses;
        }
    }

    for (lane = pm_lower_boundry; lane <= pm_upper_boundry; ++lane)
    {
        int lane_in_pm = lane % nof_ports;
        SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.lane2serdes.get(unit, lane, &lane2serdes[lane_in_pm]));

        /** if a lane is not mapped:
         * on rx side- use the index of the lane itself to fill the hole (because [lane<->serdes rx] mapping is 1:1)
         * on tx side- use the array of unmapped serdeses of the pm to fill the hole */
        if ((lane2serdes[lane_in_pm].rx_id == SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED) &&
                (lane2serdes[lane_in_pm].tx_id == SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED))
        {
            lane2serdes[lane_in_pm].rx_id = lane_in_pm;

            lane2serdes[lane_in_pm].tx_id = unmapped_tx_serdeses_arr[unmapped_tx_ind];
            ++unmapped_tx_ind;
        } else { /* lane is mapped */
            lane2serdes[lane_in_pm].rx_id %= nof_ports;
            lane2serdes[lane_in_pm].tx_id %= nof_ports;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * See .h file
 */
shr_error_e
soc_dnxf_lane_map_db_is_lane_mapped_get(
    int unit,
    int lane,
    int* is_lane_mapped)
{
    soc_dnxc_lane_map_db_map_t lane2serdes_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.lane_map_db.lane2serdes.get(unit, lane, &lane2serdes_entry));

    if(lane2serdes_entry.rx_id == SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED)
    {
        *is_lane_mapped = 0;
    }
    else
    {
        *is_lane_mapped = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

#endif /*PORTMOD_SUPPORT*/

#undef BSL_LOG_MODULE
