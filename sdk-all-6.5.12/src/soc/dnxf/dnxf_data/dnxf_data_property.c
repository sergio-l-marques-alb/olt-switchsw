/** \file dnxf_data_property.c
 * 
 * MODULE DATA CUSTOME PROPRTY - 
 * Includes all custom functions implementations 
 *  
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 */
/*
 * $Copyright: (c) 2017 Broadcom.
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

#include <soc/dnxc/legacy/dnxc_data_property.h>
#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnxc/legacy/fabric.h>

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
#define DNXF_DATA_FW_LOAD_VERIFY_BITS_OFFSET        (8)
#define DNXF_DATA_FW_LOAD_METHOD_BITS_MASK          0xFF

/*
* {  LOCAL DEFINES and STRUCTURES - end
*/
/*
* {  LOCAL FUNCTIONS
*/

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

    SHR_IF_ERR_EXIT(dnxc_data_property_fabric_fw_load_method_read(unit, fabric_fw_load_method));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e dnxf_data_property_port_general_fw_load_verify_read(
    int unit,
    uint32 *fabric_fw_load_verify)
{
    SHR_FUNC_INIT_VARS(unit);

    dnxc_data_property_fabric_fw_load_verify_read(unit, fabric_fw_load_verify);

    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_port_static_add_serdes_lane_config_read(
    int unit,
    int port,
    dnxf_data_port_static_add_serdes_lane_config_t *serdes_lane_config)
{
    char* propval_str;
    uint32 propval_num;
    SHR_FUNC_INIT_VARS(unit);

    /* 1. dfe */
    propval_str = soc_property_port_suffix_num_get_str(unit, port, 0, spn_SERDES_LANE_CONFIG, "dfe");

    if (propval_str)
    {
        if (sal_strcasecmp(propval_str, "on") == 0)
        {
            serdes_lane_config->dfe = soc_dnxc_port_dfe_on;
        }
        else if (sal_strcasecmp(propval_str, "off") == 0)
        {
            serdes_lane_config->dfe = soc_dnxc_port_dfe_off;
        }
        else if (sal_strcasecmp(propval_str, "lp") == 0)
        {
            serdes_lane_config->dfe = soc_dnxc_port_lp_dfe;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_dfe SoC property value %s. Allowed values are on|off|lp", port, propval_str);
        }
    }

    /* 2. media type */
    propval_str = soc_property_port_suffix_num_get_str(unit, port, 0, spn_SERDES_LANE_CONFIG, "media_type");

    if (propval_str)
    {
        if (sal_strcasecmp(propval_str, "backplane") == 0)
        {
            serdes_lane_config->media_type = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE;
        }
        else if (sal_strcasecmp(propval_str, "copper") == 0)
        {
            serdes_lane_config->media_type = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE;
        }
        else if (sal_strcasecmp(propval_str, "optics") == 0)
        {
            serdes_lane_config->media_type = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_media_type SoC property value %s. Allowed values are backplane|copper|optics", port, propval_str);
        }
    }

    /* 3. unreliable_los */
    propval_num = soc_property_port_suffix_num_get(unit, port, 0, spn_SERDES_LANE_CONFIG, "unreliable_los", 0xffffffff);

    if (propval_num != 0xffffffff)
    {
        if ((propval_num != 0) && (propval_num != 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_unreliable_los SoC property value %u. Allowed values are 0 or 1", port, propval_num);
        }
        serdes_lane_config->unreliable_los = propval_num;
    }

    /* 4. cl72_auto_polarity_en */
    propval_num = soc_property_port_suffix_num_get(unit, port, 0, spn_SERDES_LANE_CONFIG, "cl72_auto_polarity_en", 0xffffffff);

    if (propval_num != 0xffffffff)
    {
        if ((propval_num != 0) && (propval_num != 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_cl72_auto_polarity_en SoC property value %u. Allowed values are 0 or 1", port, propval_num);
        }
        serdes_lane_config->cl72_auto_polarity_enable = propval_num;
    }

    /* 5. cl72_restart_timeout_en */
    propval_num = soc_property_port_suffix_num_get(unit, port, 0, spn_SERDES_LANE_CONFIG, "cl72_restart_timeout_en", 0xffffffff);

    if (propval_num != 0xffffffff)
    {
        if ((propval_num != 0) && (propval_num != 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_cl72_restart_timeout_en SoC property value %u. Allowed values are 0 or 1", port, propval_num);
        }
        serdes_lane_config->cl72_restart_timeout_enable = propval_num;
    }

    /* 6. channel_mode */
    propval_str = soc_property_port_suffix_num_get_str(unit, port, 0, spn_SERDES_LANE_CONFIG, "channel_mode");

    if (propval_str)
    {
        if (sal_strcasecmp(propval_str, "force_nr") == 0)
        {
            serdes_lane_config->channel_mode = soc_dnxc_port_force_nr;
        }
        else if (sal_strcasecmp(propval_str, "force_er") == 0)
        {
            serdes_lane_config->channel_mode = soc_dnxc_port_force_er;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: Invalid serdes_lane_config_channel_mode SoC property value %s. Allowed values are force_nr|force_er", port, propval_str);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e dnxf_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    dnxf_data_port_static_add_serdes_tx_taps_t *serdes_tx_taps)
{
    bcm_port_phy_tx_t tx;
    SHR_FUNC_INIT_VARS(unit);

     SHR_IF_ERR_EXIT(dnxc_data_property_port_static_add_serdes_tx_taps_read(unit, port, &tx));

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
dnxf_data_property_module_testing_property_methods_custom_read(
    int unit,
    int link,
    int pipe,
    dnxf_data_module_testing_property_methods_custom_t * custom)
{
    SHR_FUNC_INIT_VARS(unit);

    if (pipe == 1)
    {
        custom->val = soc_property_suffix_num_get(unit, link, "dnxf_data_custom", "link", custom->val);
    }

    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
* {  LOCAL FUNCTIONS
*/

/*
* }  LOCAL FUNCTIONS - end
*/


