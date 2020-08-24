/** \file dnxf_data_property.c
 * 
 * MODULE DATA CUSTOME PROPRTY - 
 * Includes all custom functions implementations 
 *  
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

/*
 * INCLUDE FILES:
 * {
 */
#include <soc/cmic.h>
#include <soc/property.h>

#include <soc/dnxc/dnxc_data_property.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxc/fabric.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/cmn/dnxf_drv.h>


#include <sal/core/boot.h>
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * INCLUDE FILES:
 * {
 */

/*
 * }
 */
 /*
* {  LOCAL DEFINES and STRUCTURES
*/
#define DNXF_DATA_FW_LOAD_VERIFY_BITS_OFFSET                (8)
#define DNXF_DATA_FW_LOAD_METHOD_BITS_MASK                  0xFF
#define DNXF_DATA_FABRIC_FIFO_SIZE_FE1_SUFFIX               "fe1"
#define DNXF_DATA_FABRIC_FIFO_SIZE_FE3_SUFFIX               "fe3"
#define DNXF_DATA_FABRIC_FIFO_SIZE_FE3_LOCAL_ROUTE_SUFFIX   "fe3_local_route"

#define DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(size)  ((size / dnxf_data_fabric.fifos.granularity_get(unit)) * dnxf_data_fabric.fifos.granularity_get(unit))
/*
* {  LOCAL DEFINES and STRUCTURES - end
*/
/*
* {  LOCAL FUNCTIONS
*/

static
shr_error_e dnxf_data_property_fabric_fifos_depth_parse_propval(int unit, int nof_pipes, char *propval, int *sizes);

static
shr_error_e dnxf_data_property_fabric_fifos_depth_validate(int unit, int nof_pipes, int fifo_resource, int *sizes, int *lr_sizes);

/*
* }  LOCAL FUNCTIONS - end
*/
/*
 * MODULE: DEVICE
 * {
 */
shr_error_e dnxf_data_property_device_access_schan_timeout_usec_read(
    int unit,
    uint32 *schan_timeout_usec)
{
    uint32 default_value;
    SHR_FUNC_INIT_VARS(unit);

    /*SCHAN*/
    /*Choose default value in dependance of the boot*/
    if (SAL_BOOT_QUICKTURN) {
        default_value = SCHAN_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
        default_value = SCHAN_TIMEOUT_PLI;
    } else {
        default_value = SCHAN_TIMEOUT;
    }

    *schan_timeout_usec = soc_property_get(unit, spn_SCHAN_TIMEOUT_USEC, default_value);

    SHR_FUNC_EXIT;
}

 /*
 * MODULE: DEVICE - end
 * }
 */
/*
 * MODULE: FABRIC
 * {
 */
shr_error_e dnxf_data_property_fabric_pipes_map_read(
    int unit,
    dnxf_data_fabric_pipes_map_t *map)
{
    uint32 nof_pipes = 0;
    int i_prio = 0;
    int i_config = 0;
    const dnxc_data_table_info_t *table_info = NULL;
    int is_valid = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);

    /*
     * Single pipe - take default configuration.
     * Multiple pipes - validate user's configuration is valid.
     */
    if (nof_pipes > 1)
    {
        /*
         * Get user's mapping configurations.
         */
        for (i_prio = 0; i_prio < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES; ++i_prio)
        {
            map->uc[i_prio] =
                soc_property_suffix_num_get(unit, i_prio, spn_FABRIC_PIPE_MAP, "uc", SOC_DNXF_PROPERTY_UNAVAIL);
            if (map->uc[i_prio] == SOC_DNXF_PROPERTY_UNAVAIL)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "unicast priority %d isn't configured. If number of pipes > 1, all priorities must be configured",
                             i_prio);
            }

            map->mc[i_prio] =
                soc_property_suffix_num_get(unit, i_prio, spn_FABRIC_PIPE_MAP, "mc", SOC_DNXF_PROPERTY_UNAVAIL);
            if (map->mc[i_prio] == SOC_DNXF_PROPERTY_UNAVAIL)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "multicast priority %d isn't configured. If number of pipes > 1, all priorities must be configured",
                             i_prio);
            }
        }

        /*
         * check if invalid pipe was configured (more than number of pipes)
         */
        for (i_prio = 0; i_prio < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES; ++i_prio)
        {
            if ((map->uc[i_prio] >= nof_pipes) || (map->mc[i_prio] >= nof_pipes))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid pipe number - more than configured");
            }
        }

        /*
         * check if the configuration is valid
         */
        table_info = dnxf_data_fabric.pipes.valid_map_config_info_get(unit);
        for (i_config = 0; i_config < table_info->key_size[0]; ++i_config)
        {
            if (nof_pipes == dnxf_data_fabric.pipes.valid_map_config_get(unit, i_config)->nof_pipes)
            {
                is_valid = 1;
                for (i_prio = 0; i_prio < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES; ++i_prio)
                {
                    is_valid = (is_valid
                                && (map->uc[i_prio] ==
                                    dnxf_data_fabric.pipes.valid_map_config_get(unit, i_config)->uc[i_prio])
                                && (map->mc[i_prio] ==
                                    dnxf_data_fabric.pipes.valid_map_config_get(unit, i_config)->mc[i_prio]));
                }

                /*
                 * If config is valid, update all other table variables
                 */
                if (is_valid)
                {
                    /** Get name */
                    map->name = dnxf_data_fabric.pipes.valid_map_config_get(unit, i_config)->name;

                    /** Get type */
                    map->type = dnxf_data_fabric.pipes.valid_map_config_get(unit, i_config)->type;

                    if (map->type == soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc
                        && dnxf_data_fabric.multicast.priority_map_enable_get(unit))
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid Configuration - cannot configure triple pipe uc,hp-mc,lp-mc mode & mc priority map");
                    }

                    /** Get min HP MC */
                    map->min_hp_mc = dnxf_data_fabric.pipes.valid_map_config_get(unit, i_config)->min_hp_mc;

                    break;
                }
            }
        }

        if (!is_valid)
        {
            LOG_ERROR(BSL_LOG_MODULE, ("\nInvalid configuration for fabric pipes mapping:"));
            LOG_ERROR(BSL_LOG_MODULE, (("\nNumber of pipes - %d"), nof_pipes));

            LOG_ERROR(BSL_LOG_MODULE, ("\nUC priority to pipe mapping -"));
            for (i_prio = 0; i_prio < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES; ++i_prio)
            {
                LOG_ERROR(BSL_LOG_MODULE, ((" %d"), map->uc[i_prio]));
            }
            LOG_ERROR(BSL_LOG_MODULE, (("\nMC priority to pipe mapping -")));
            for (i_prio = 0; i_prio < DNXF_DATA_MAX_FABRIC_CONGESTION_NOF_THRESHOLD_PRIORITIES; ++i_prio)
            {
                LOG_ERROR(BSL_LOG_MODULE, ((" %d"), map->mc[i_prio]));
            }

            SHR_IF_ERR_EXIT(_SHR_E_CONFIG);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_fabric_fifos_rx_depth_per_pipe_read(
    int unit,
    int pipe_id,
    dnxf_data_fabric_fifos_rx_depth_per_pipe_t *rx_depth_per_pipe)
{

    char *propval;
    int nof_pipes;
    int fifo_resource;
    int sizes[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_fabric_device_mode_t device_mode;
    SHR_FUNC_INIT_VARS(unit);

    device_mode = dnxf_data_fabric.general.device_mode_get(unit);
    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    fifo_resource = dnxf_data_fabric.fifos.rx_resources_get(unit);

    /*
     * Set Defaults
     */
    rx_depth_per_pipe->fe1 = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource / nof_pipes);
    rx_depth_per_pipe->fe3 = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource / nof_pipes);

    /*
     * Parse SoC property
     */
    if (device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13)
    {

        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_RX, DNXF_DATA_FABRIC_FIFO_SIZE_FE1_SUFFIX);

        /* if set - parse the SoC property and set it to the struct returned */
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
            rx_depth_per_pipe->fe1 = sizes[pipe_id];
        }

        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_RX, DNXF_DATA_FABRIC_FIFO_SIZE_FE3_SUFFIX);

        /* if set - parse the SoC property and set it to the struct returned */
        if (propval)
        {
             SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
             SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
             rx_depth_per_pipe->fe3 = sizes[pipe_id];
        }

    }
    else
    {
        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_RX, "");

        /* if set - parse the SoC property and set it to the struct returned */
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
            rx_depth_per_pipe->fe1 = sizes[pipe_id];
            rx_depth_per_pipe->fe3 = sizes[pipe_id];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_fabric_fifos_mid_depth_per_pipe_read(
    int unit,
    int pipe_id,
    dnxf_data_fabric_fifos_mid_depth_per_pipe_t *mid_depth_per_pipe)
{

    char *propval;
    int nof_pipes;
    int fifo_resource;
    int local_routing_enable_uc, local_routing_enable_mc;
    int sizes[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_fabric_device_mode_t device_mode;
    SHR_FUNC_INIT_VARS(unit);

    local_routing_enable_uc = dnxf_data_fabric.general.local_routing_enable_uc_get(unit);
    local_routing_enable_mc = dnxf_data_fabric.general.local_routing_enable_mc_get(unit);
    device_mode = dnxf_data_fabric.general.device_mode_get(unit);
    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    fifo_resource = dnxf_data_fabric.fifos.mid_resources_get(unit);

    /*
     * Set Defaults
     */
    mid_depth_per_pipe->fe1_nlr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource / nof_pipes);
    mid_depth_per_pipe->fe1_lr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource / nof_pipes);

    mid_depth_per_pipe->fe3_nlr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource / nof_pipes);
    mid_depth_per_pipe->fe3_lr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource / nof_pipes);

    /*
     * Parse SoC property
     */
    if (device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13)
    {
        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_MID, DNXF_DATA_FABRIC_FIFO_SIZE_FE1_SUFFIX);
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
            mid_depth_per_pipe->fe1_nlr = sizes[pipe_id];

        }

        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_MID, DNXF_DATA_FABRIC_FIFO_SIZE_FE3_SUFFIX);
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
            mid_depth_per_pipe->fe3_nlr = sizes[pipe_id];
        }

        if (local_routing_enable_uc || local_routing_enable_mc)
        {
            propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_MID, DNXF_DATA_FABRIC_FIFO_SIZE_FE3_LOCAL_ROUTE_SUFFIX);
            if (propval)
            {
                SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
                SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
                mid_depth_per_pipe->fe3_lr  = sizes[pipe_id];
            }

        }
    }
    else
    {
        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_MID, "");

        /* if set - parse the SoC property and set it to the struct returned */
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, fifo_resource, sizes, NULL));
            mid_depth_per_pipe->fe1_nlr = sizes[pipe_id];
            mid_depth_per_pipe->fe3_nlr = sizes[pipe_id];
        }

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_fabric_fifos_tx_depth_per_pipe_read(
    int unit,
    int pipe_id,
    dnxf_data_fabric_fifos_tx_depth_per_pipe_t *tx_depth_per_pipe)
{
    char *propval;
    int pipe_index;
    int nof_pipes;
    int fifo_resource;
    int sizes[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    int sizes_fe3_nlr[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    int sizes_fe3_lr[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES];
    soc_dnxf_fabric_device_mode_t device_mode;
    int local_routing_enable_uc, local_routing_enable_mc;
    SHR_FUNC_INIT_VARS(unit);

    local_routing_enable_uc = dnxf_data_fabric.general.local_routing_enable_uc_get(unit);
    local_routing_enable_mc = dnxf_data_fabric.general.local_routing_enable_mc_get(unit);
    device_mode = dnxf_data_fabric.general.device_mode_get(unit);
    nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
    fifo_resource = dnxf_data_fabric.fifos.tx_resources_get(unit);

    /*
     * Set Defaults
     */
    if (local_routing_enable_uc || local_routing_enable_mc)
    {
        tx_depth_per_pipe->fe3_nlr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED( (fifo_resource/2) / nof_pipes) - 4;
        tx_depth_per_pipe->fe3_lr  = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED( (fifo_resource/2) / nof_pipes) - 4;

        tx_depth_per_pipe->fe1_nlr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource/nof_pipes) - 4;
        tx_depth_per_pipe->fe1_lr  = 0;
    }
    else
    {
        tx_depth_per_pipe->fe3_nlr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource/nof_pipes) - 4;
        tx_depth_per_pipe->fe3_lr  = 0;

        tx_depth_per_pipe->fe1_nlr = DNXF_DATA_FABRIC_FIFO_SIZE_ALIGNED(fifo_resource/nof_pipes) - 4;
        tx_depth_per_pipe->fe1_lr  = 0;
    }

    /*
     * Parse SoC property
     */
    if (device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13)
    {
        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_TX, DNXF_DATA_FABRIC_FIFO_SIZE_FE1_SUFFIX);
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, (fifo_resource - 4), sizes, NULL));
            tx_depth_per_pipe->fe1_nlr = sizes[pipe_id];
        }

        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_TX, DNXF_DATA_FABRIC_FIFO_SIZE_FE3_SUFFIX);
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes_fe3_nlr));
            if (!local_routing_enable_uc && !local_routing_enable_mc)
            {
                SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, (fifo_resource - 4), sizes_fe3_nlr, NULL));
            }
            tx_depth_per_pipe->fe3_nlr = sizes_fe3_nlr[pipe_id];
        }
        else
        {
            /*
             * Fill sizes_fe3_nlr[] with default values for local route validation
             */

            for (pipe_index = 0 ; pipe_index < nof_pipes ; pipe_index++)
            {
                sizes_fe3_nlr[pipe_id] = tx_depth_per_pipe->fe3_nlr;
            }
        }

        if (local_routing_enable_uc || local_routing_enable_mc)
        {
            propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_TX, DNXF_DATA_FABRIC_FIFO_SIZE_FE3_LOCAL_ROUTE_SUFFIX);
            if (propval)
            {
                SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes_fe3_lr));
                SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, (fifo_resource - 4), sizes_fe3_nlr, sizes_fe3_lr));
                tx_depth_per_pipe->fe3_lr  = sizes_fe3_lr[pipe_id];
            }
        }
    }
    else
    {
        propval = soc_property_suffix_num_str_get(unit, 0, spn_FABRIC_LINK_FIFO_SIZE_TX, "");

        /* if set - parse the SoC property and set it to the struct returned */
        if (propval)
        {
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_parse_propval(unit, nof_pipes, propval, sizes));
            SHR_IF_ERR_EXIT(dnxf_data_property_fabric_fifos_depth_validate(unit, nof_pipes, (fifo_resource - 4), sizes, NULL));
            tx_depth_per_pipe->fe1_nlr = sizes[pipe_id];
            tx_depth_per_pipe->fe3_nlr = sizes[pipe_id];
        }

    }

exit:
    SHR_FUNC_EXIT;
}

 /*
 * MODULE: FABRIC - end
 * }
 */

/*
 * MODULE: PORT
 * {
 */
shr_error_e dnxf_data_property_port_general_fw_load_method_read(
    int unit,
    uint32 *fabric_fw_load_method)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_load_method_read(unit, fabric_fw_load_method, "fabric"));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_port_general_fw_crc_check_read(
    int unit,
    uint32 *fabric_fw_crc_check)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_crc_check_read(unit, fabric_fw_crc_check, "fabric"));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_port_general_fw_load_verify_read(
    int unit,
    uint32 *fabric_fw_load_verify)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_load_verify_read(unit, fabric_fw_load_verify, "fabric"));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    dnxf_data_port_static_add_serdes_tx_taps_t *serdes_tx_taps)
{
    bcm_port_phy_tx_t tx;
    char *propval = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * read SoC property, which should be in this format:
     * signaling_mode:pre:main:post:pre2:post2:post3
     * the [:pre2:post2:post3] part is optional.
     */
     propval = soc_property_port_get_str(unit, port, spn_SERDES_TX_TAPS);
     SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_read(unit, port, propval, &tx));

     /* * if the SoC property was given - copy the given values into serdes_tx_taps struct.
      * if SoC property wasn't given - tx.main will be INVALID. In that case serdes_tx_taps struct will have the defaults set in dnxf_data. */
     if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
     {
         serdes_tx_taps->pre = tx.pre;
         serdes_tx_taps->main = tx.main;
         serdes_tx_taps->post = tx.post;
         serdes_tx_taps->tx_tap_mode = tx.tx_tap_mode;
         serdes_tx_taps->signalling_mode = tx.signalling_mode;

         /* copy the optional taps if they were given */
         if (serdes_tx_taps->tx_tap_mode ==  bcmPortPhyTxTapMode6Tap)
         {
             serdes_tx_taps->pre2 = tx.pre2;
             serdes_tx_taps->post2 = tx.post2;
             serdes_tx_taps->post3 = tx.post3;
         }
     }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_port_lane_map_info_read(
    int unit,
    int lane,
    dnxf_data_port_lane_map_info_t *info)
{
    char *propval, *prefix, *propval_tmp;
    int len, legacy_quad;
    int serdes_rx, serdes_tx;
    SHR_FUNC_INIT_VARS(unit);

    /* check that the legacy soc property wasn't configured. check once per quad */
    if (lane % 4 == 0)
    {
        legacy_quad = lane / 4;
        if (soc_property_suffix_num_only_suffix_str_get(unit, legacy_quad, spn_PHY_RX_LANE_MAP, "quad") ||
            (soc_property_suffix_num_get(unit, legacy_quad, spn_PHY_RX_LANE_MAP, "fabric_quad", -1) != -1) ||
            soc_property_suffix_num_only_suffix_str_get(unit, legacy_quad, spn_PHY_TX_LANE_MAP, "quad") ||
            (soc_property_suffix_num_get(unit, legacy_quad, spn_PHY_TX_LANE_MAP, "fabric_quad", -1) != -1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "legacy SOC property configuration phy_tx/rx_lane_map_quad<> not supported for this device");
        }
    }

    /*get a string of this format (the right side of the soc property): rx<srd_rx_id>:tx<srd_tx_id> or UNMAPPED*/
    propval = soc_property_suffix_num_only_suffix_str_get(unit, lane, spn_LANE_TO_SERDES_MAP, "fabric_lane");

    prefix = "NOT_MAPPED";
    len = strlen(prefix);

    /* lane is mapped */
    if (propval && sal_strncasecmp(propval, prefix, len))
    {
        prefix = "rx";
        len = strlen(prefix);

        if (sal_strncasecmp(propval, prefix, len))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - \"rx\" is missing", lane);
        }
        propval += len;

        serdes_rx = sal_ctoi(propval, &propval_tmp);
        if (propval == propval_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - srd_rx_id is missing", lane);
        }

        propval = propval_tmp;

        prefix = ":";
        len = strlen(prefix);
        if (sal_strncasecmp(propval, prefix, len))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - \":\" is missing", lane);
        }
        propval += len;

        prefix = "tx";
        len = strlen(prefix);
        if (sal_strncasecmp(propval, prefix, len))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - \"tx\" is missing", lane);
        }
        propval += len;

        serdes_tx = sal_ctoi(propval, &propval_tmp);
        if (propval == propval_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - srd_tx_id is missing", lane);
        }
        /* put recovered values to the array */
        info->serdes_rx_id = serdes_rx;
        info->serdes_tx_id = serdes_tx;
    }
    else
    {
        /* lane is unmapped */
        info->serdes_rx_id = SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED;
        info->serdes_tx_id = SOC_DNXF_LANE_MAP_DB_LANE_TO_SERDES_NOT_MAPPED;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
* MODULE: PORT - end
* }
*/

/*
 * MODULE: MODULE_TESTING
 * {
 */

/*
 * submodule property_methods
 */

/*
 * TBD: it's example only - should be rewritten
 */
/*
 * See .h file
 */
shr_error_e
dnxf_data_property_module_testing_property_methods_custom_val_read(
    int unit,
    int link,
    int pipe,
    int *val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (pipe == 1)
    {
        *val = soc_property_suffix_num_get(unit, link, "dnxf_data_custom", "link", *val);
    }

    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
* {  LOCAL FUNCTIONS
*/

static
shr_error_e dnxf_data_property_fabric_fifos_depth_parse_propval(int unit, int nof_pipes, char *propval, int *sizes)
{
    char *prefix, *propval_tmp;
    int pipe_index;
    int len;
    SHR_FUNC_INIT_VARS(unit);

    prefix = ":";
    len = strlen(prefix);
    for (pipe_index = 0; pipe_index < nof_pipes; ++pipe_index)
    {

        sizes[pipe_index] = sal_ctoi(propval, &propval_tmp);
        if (propval == propval_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Size is missing after \":\"");
        }
        propval = propval_tmp;

        if (sal_strncasecmp(propval, prefix, len))
        {
            if ( pipe_index == (nof_pipes-1) )
            {
                break;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "number of pipes configured %d, should be %d", (pipe_index+1), nof_pipes);
            }
        }
        propval += len;
    }

exit:
    SHR_FUNC_EXIT;
}

static
shr_error_e dnxf_data_property_fabric_fifos_depth_validate(int unit, int nof_pipes, int fifo_resource, int *sizes, int *lr_sizes)
{
    int pipe_index;
    int total_size;
    SHR_FUNC_INIT_VARS(unit);

    total_size=0;
    for (pipe_index = 0; pipe_index < nof_pipes; ++pipe_index)
    {
        /* Validate FIFO granularity */
        if (sizes[pipe_index] % dnxf_data_fabric.fifos.granularity_get(unit) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "FIFO size %d for pipe %d is not divisible by %d", sizes[pipe_index], pipe_index, dnxf_data_fabric.fifos.granularity_get(unit));
        }

        /* Validate FIFO minimal value */
        if (sizes[pipe_index] < dnxf_data_fabric.fifos.min_depth_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "FIFO size %d for pipe %d is below minimal value (%d)", sizes[pipe_index], pipe_index, dnxf_data_fabric.fifos.min_depth_get(unit));
        }

        total_size+=sizes[pipe_index];

        if (lr_sizes)
        {
            /* Validate FIFO granularity */
            if (lr_sizes[pipe_index] % dnxf_data_fabric.fifos.granularity_get(unit) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "FIFO size %d for pipe %d is not divisible by %d", lr_sizes[pipe_index], pipe_index, dnxf_data_fabric.fifos.granularity_get(unit));
            }

            /* Validate FIFO minimal value */
            if (lr_sizes[pipe_index] < dnxf_data_fabric.fifos.min_depth_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "FIFO size %d for pipe %d is below minimal value (%d)", lr_sizes[pipe_index], pipe_index, dnxf_data_fabric.fifos.min_depth_get(unit));
            }

            total_size+=lr_sizes[pipe_index];
        }


    }

    if (total_size > fifo_resource)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Total resource per FIFO %d is larger than available resource %d", total_size, fifo_resource);
    }

    if ( (fifo_resource - total_size) > dnxf_data_fabric.fifos.max_unused_resources_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "unused resources %d is larger then allowed (%d)", (fifo_resource - total_size), dnxf_data_fabric.fifos.max_unused_resources_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* }  LOCAL FUNCTIONS - end
*/


