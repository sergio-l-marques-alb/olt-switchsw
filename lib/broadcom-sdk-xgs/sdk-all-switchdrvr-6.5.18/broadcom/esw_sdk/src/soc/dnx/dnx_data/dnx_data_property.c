/** \file dnx_data_property.c
 *
 * MODULE DATA CUSTOME PROPRTY -
 * Includes all custom functions implementations
 *
 * Device Data
 * SW component that maintains per device data
 * The data is static and won't be changed after device initialization.
 *
 * Supported data types:
 *     - Define             - a 'uint32' number (a max value for all devices is maintained)
 *     - feature            - 1 bit per each feature (supported/not supported) - support soc properties
 *     - table              - the data is accessed with keys and/or can maintain multiple values and/or set by soc property
 *     - numeric            - a 'uint32' number that support soc properties
 *
 * User interface for DNX DATA component can be found in "dnx_data_if.h" and "dnx_data_if_#module#.h"
 *
 * Adding the data is done via XMLs placed in "tools/autocoder/DeviceData/dnx/.."
 * "How to" User Guide can be found in confluence.
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
#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/* #include <soc/drv.h> */
   /*
    * needed for soc_property_s 
    */

#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <sal/appl/sal.h>
#include <soc/dnxc/dnxc_data_property.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_str.h>
#include <shared/pbmp.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <src/bcm/dnx/algo/port/algo_port_internal.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*
 * }
 */

/*
 * Defines:
 * {
 */
#define SOC_DNX_PROPERTY_UNAVAIL (-1)
/*
 * }
 */

/*
 * MODULE: FABRIC:
 * {
 */

/*
 * submodule pipes
 */
/*
 * See .h file
 */
shr_error_e
dnx_data_property_fabric_pipes_map_read(
    int unit,
    dnx_data_fabric_pipes_map_t * map)
{
    uint32 nof_pipes = 0;
    int i_prio = 0;
    int i_config = 0;
    const dnxc_data_table_info_t *table_info = NULL;
    int is_valid = 0;

    SHR_FUNC_INIT_VARS(unit);
    nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    /*
     * Single pipe - take default configuration.
     * Multiple pipes - validate user's configuration is valid.
     */
    if (nof_pipes > 1)
    {
        /*
         * Get user's mapping configurations.
         */
        for (i_prio = 0; i_prio < DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES; ++i_prio)
        {
            map->uc[i_prio] =
                dnx_drv_soc_property_suffix_num_get(unit, i_prio, spn_FABRIC_PIPE_MAP, "uc", SOC_DNX_PROPERTY_UNAVAIL);
            if (map->uc[i_prio] == SOC_DNX_PROPERTY_UNAVAIL)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "unicast priority %d isn't configured. If number of pipes > 1, all priorities must be configured",
                             i_prio);
            }
            map->mc[i_prio] =
                dnx_drv_soc_property_suffix_num_get(unit, i_prio, spn_FABRIC_PIPE_MAP, "mc", SOC_DNX_PROPERTY_UNAVAIL);
            if (map->mc[i_prio] == SOC_DNX_PROPERTY_UNAVAIL)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "multicast priority %d isn't configured. If number of pipes > 1, all priorities must be configured",
                             i_prio);
            }
        }
        /*
         * check if invalid pipe was configured (more than number of pipes)
         */
        for (i_prio = 0; i_prio < DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES; ++i_prio)
        {
            if ((map->uc[i_prio] >= nof_pipes) || (map->mc[i_prio] >= nof_pipes))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid pipe number - more than configured");
            }
        }
        /*
         * check if the configuration is valid
         */
        table_info = dnx_data_fabric.pipes.valid_map_config_info_get(unit);
        for (i_config = 0; i_config < table_info->key_size[0]; ++i_config)
        {
            if (nof_pipes == dnx_data_fabric.pipes.valid_map_config_get(unit, i_config)->nof_pipes)
            {
                is_valid = 1;
                for (i_prio = 0; i_prio < DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES; ++i_prio)
                {
                    is_valid = (is_valid
                                && (map->uc[i_prio] ==
                                    dnx_data_fabric.pipes.valid_map_config_get(unit, i_config)->uc[i_prio])
                                && (map->mc[i_prio] ==
                                    dnx_data_fabric.pipes.valid_map_config_get(unit, i_config)->mc[i_prio]));
                }
                /*
                 * If config is valid, update all other table variables
                 */
                if (is_valid)
                {
                    /** Get name */
                    map->name = dnx_data_fabric.pipes.valid_map_config_get(unit, i_config)->name;

                    /** Get type */
                    map->type = dnx_data_fabric.pipes.valid_map_config_get(unit, i_config)->type;

                    /** Get min HP MC */
                    map->min_hp_mc = dnx_data_fabric.pipes.valid_map_config_get(unit, i_config)->min_hp_mc;

                    break;
                }
            }
        }
        if (!is_valid)
        {
            LOG_ERROR(BSL_LOG_MODULE, ("\nInvalid configuration for fabric pipes mapping:"));
            LOG_ERROR(BSL_LOG_MODULE, (("\nNumber of pipes - %d"), nof_pipes));
            LOG_ERROR(BSL_LOG_MODULE, ("\nUC priority to pipe mapping -"));
            for (i_prio = 0; i_prio < DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES; ++i_prio)
            {
                LOG_ERROR(BSL_LOG_MODULE, ((" %d"), map->uc[i_prio]));
            }
            LOG_ERROR(BSL_LOG_MODULE, (("\nMC priority to pipe mapping -")));
            for (i_prio = 0; i_prio < DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES; ++i_prio)
            {
                LOG_ERROR(BSL_LOG_MODULE, ((" %d"), map->mc[i_prio]));
            }

            SHR_IF_ERR_EXIT(_SHR_E_CONFIG);
        }
    }
    /*
     * Get TDM pipe id.
     * TDM pipe can also be on single pipe mode, and it depends on whether
     * the device works in tdm mode.
     */
    map->tdm_pipe =
        (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_NONE) ? -1 : dnx_data_fabric.pipes.valid_map_config_get(unit,
                                                                                                                i_config)->tdm_pipe;
    {
        /*
         * Not all configurations are valid for MESH mode.
         */
        if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_MESH)
        {
            int is_tdm_supported = dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE;

            /*
             * Mesh with Multicast
             */
            if (dnx_data_fabric.mesh.multicast_enable_get(unit) == 1)
            {
                if (is_tdm_supported && (map->type != soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm)
                    && (map->type != soc_dnxc_fabric_pipe_map_dual_uc_mc))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "MESH with MC must be configured along with UC/MC 2-pipes configuration or UC/MC/TDM 3-pipes configuration");
                }
                else if (map->type != soc_dnxc_fabric_pipe_map_dual_uc_mc)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "MESH with MC must be configured along with UC/MC 2-pipes configuration");
                }
            }
            /*
             * Mesh without Multicast
             */
            else
            {
                if (is_tdm_supported && (map->type != soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm)
                    && (map->type != soc_dnxc_fabric_pipe_map_single))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "MESH must be configured along with single pipe configuration or TDM/non-TDM 2-pipes configuration");
                }
                else if (map->type != soc_dnxc_fabric_pipe_map_single)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "MESH must be configured along with single_pipe configuration");
                }
            }
        }
        else if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
        {
            if (map->type != soc_dnxc_fabric_pipe_map_single)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "FAP stand-alone must be configured along with single_pipe configuration");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * submodule mesh
 */
/*
 * See .h file
 */
shr_error_e
dnx_data_property_fabric_mesh_multicast_enable_read(
    int unit,
    uint32 *multicast_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The soc property fabric_mesh_multicast_enable is relevant only
     * when device is in MESH mode.
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        *multicast_enable = dnx_drv_soc_property_get(unit, spn_FABRIC_MESH_MULTICAST_ENABLE, *multicast_enable);
        if (*multicast_enable > 1)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Invalid value %u for fabric_mesh_multicast_enable. Value must be only 0 or 1.",
                         *multicast_enable);
        }
    }
    else
    {
        *multicast_enable = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * submodule links
 */
/*
 * See .h file
 */
shr_error_e
dnx_data_property_fabric_links_polarity_tx_polarity_read(
    int unit,
    int link,
    uint32 *tx_polarity)
{
    soc_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    port = link + dnx_data_port.general.fabric_port_base_get(unit);
    *tx_polarity = dnx_drv_soc_property_port_get(unit, port, spn_PHY_TX_POLARITY_FLIP, *tx_polarity);

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_fabric_links_polarity_rx_polarity_read(
    int unit,
    int link,
    uint32 *rx_polarity)
{
    soc_port_t port;
    SHR_FUNC_INIT_VARS(unit);

    port = link + dnx_data_port.general.fabric_port_base_get(unit);
    *rx_polarity = dnx_drv_soc_property_port_get(unit, port, spn_PHY_RX_POLARITY_FLIP, *rx_polarity);

    SHR_FUNC_EXIT;
}

/*
 * submodule dtqs
 */
/**
 * See .h file
 */
shr_error_e
dnx_data_property_fabric_dtqs_nof_active_dtqs_read(
    int unit,
    uint32 *nof_active_dtqs)
{
    uint32 is_single_dtq;
    SHR_FUNC_INIT_VARS(unit);

    is_single_dtq = dnx_drv_soc_property_get(unit, spn_SINGLE_DTQ, *nof_active_dtqs);

    if (is_single_dtq == 1)
    {
        if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "fabric_connect_mode=SINGLE_FAP/MESH mode can't be configured along with single DTQ");
        }

        *nof_active_dtqs = 1;
    }
    else
    {
        if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
        {
            /*
             * For Mesh mode, nof DTQs is always maximum, since it depends
             * on mesh configurations and not on pipes configuration.
             */
            *nof_active_dtqs = dnx_data_fabric.pipes.max_nof_contexts_get(unit);
        }
        else
        {
            /*
             * Number of active DTQs is according to pipes mapping.
             * Note that TDM traffic goes straight to FDT (bypass) without
             * entering DTQs, so nof_active_dtqs ignores TDM.
             */
            switch (dnx_data_fabric.pipes.map_get(unit)->type)
            {
                case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
                {
                    *nof_active_dtqs = 3;
                    break;
                }
                case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
                case soc_dnxc_fabric_pipe_map_dual_uc_mc:
                {
                    *nof_active_dtqs = 2;
                    break;
                }
                case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
                case soc_dnxc_fabric_pipe_map_single:
                {
                    *nof_active_dtqs = 1;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * MODULE: PORT:
 * {
 */

/*
 * submodule static_add
 */

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_fabric_fw_load_method_read(
    int unit,
    uint32 *fabric_fw_load_method)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_load_method_read(unit, fabric_fw_load_method, "fabric"));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_fabric_fw_crc_check_read(
    int unit,
    uint32 *fabric_fw_crc_check)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_crc_check_read(unit, fabric_fw_crc_check, "fabric"));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_fabric_fw_load_verify_read(
    int unit,
    uint32 *fabric_fw_load_verify)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_load_verify_read(unit, fabric_fw_load_verify, "fabric"));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_nif_fw_load_method_read(
    int unit,
    uint32 *nif_fw_load_method)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_load_method_read(unit, nif_fw_load_method, "nif"));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_property_port_static_add_nif_fw_crc_check_read(
    int unit,
    uint32 *nif_fw_crc_check)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_crc_check_read(unit, nif_fw_crc_check, "nif"));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_property_port_static_add_nif_fw_load_verify_read(
    int unit,
    uint32 *nif_fw_load_verify)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_data_property_fw_load_verify_read(unit, nif_fw_load_verify, "nif"));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_fabric_quad_info_quad_enable_read(
    int unit,
    int quad,
    int *quad_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * For emulation use only 1 quad, since number of quads can vary between compilations and emulations.
     */
    if (soc_sand_is_emulation_system(unit))
    {
        *quad_enable = (quad == 0) ? 1 : 0;
    }
    else
    {
        *quad_enable = dnx_drv_soc_property_suffix_num_get(unit, quad, spn_SERDES_QRTT_ACTIVE, "", 1);
    }

    SHR_FUNC_EXIT;
}

/*
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
dnx_data_property_module_testing_property_methods_custom_read(
    int unit,
    int link,
    int pipe,
    dnx_data_module_testing_property_methods_custom_t * custom)
{
    SHR_FUNC_INIT_VARS(unit);

    if (pipe == 1)
    {
        custom->val = dnx_drv_soc_property_suffix_num_get(unit, link, "dnx_data_custom", "link", custom->val);
    }

    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * MODULE: NIF
 * {
 */
/*
 * submodule ports
 */

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_general_fabric_port_base_read(
    int unit,
    uint32 *fabric_port_base)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Read SoC property */
    *fabric_port_base = dnx_drv_soc_property_get(unit, spn_FABRIC_LOGICAL_PORT_BASE, *fabric_port_base);
    /** Verify SoC property */
    if (*fabric_port_base > (SOC_MAX_NUM_PORTS - dnx_data_fabric.links.nof_links_get(unit) + 1))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "fabric logical port base value not supported - [0 - %d] \n",
                     SOC_MAX_NUM_PORTS - dnx_data_fabric.links.nof_links_get(unit));
    }

    /*
     * Special handling!
     * Set port name -
     * Required in this stage to be able to use soc_porperty_port_get
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_fabric_default_port_names_set(unit, *fabric_port_base));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * required info per interface type in order parse ucode_port SoC property
 */
typedef struct
{
    /**
     * interface type name as expected in ucode_port val
     */
    char *interface_type_name;
    /**
     * Matching Interface type define
     */
    bcm_port_if_t interface;
    /**
     * port name - used to read soc properties using soc_property_port_get
     */
    char *port_name;
    /**
     * nof_lanes - Matching number of lanes for this interface
     */
    int nof_lanes;
    /**
     * flags - additional info - see ucode_port_info_flags
     * see DNX_DATA_PROPERTY_UCODE_FLAG_*
     */
    uint32 flags;
} dnx_data_property_ucode_port_info_t;

/**
 * \brief - flags for ucode_ports_info
 */
#define DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX (0x1)
#define DNX_DATA_PROPERTY_UCODE_FLAG_OPTIONAL_INDEX     (0x2)

/* *INDENT-OFF* */
/**
 * \brief
 * holds info per interface type required to parse ucode_port SoC property
 */
static dnx_data_property_ucode_port_info_t ucode_ports_info[] = {
    /*if type name  if typedef           port name   nof_lanes   flags*/
    {"GE",          BCM_PORT_IF_NIF_ETH,    "ge",       1,       0},
    {"XE",          BCM_PORT_IF_NIF_ETH,    "xe",       1,       0},
    {"10GBase-R",   BCM_PORT_IF_NIF_ETH,    "xe",       1,       0}, /*same as XE, legacy name*/
    {"XLGE2_",      BCM_PORT_IF_NIF_ETH,    "xl",       2,       0},
    {"XLGE",        BCM_PORT_IF_NIF_ETH,    "xl",       4,       0},
    {"CGE2_",       BCM_PORT_IF_NIF_ETH,    "ce",       2,       0},
    {"CGE",         BCM_PORT_IF_NIF_ETH,    "ce",       4,       0},
    {"ILKN",        BCM_PORT_IF_ILKN,       "il",       0,       0},/*nof_lanes not relevant for ILKN*/
    {"LGE",         BCM_PORT_IF_NIF_ETH,    "le",       1,       0},
    {"CCGE",        BCM_PORT_IF_NIF_ETH,    "cc",       4,       0},
    {"CDGE",        BCM_PORT_IF_NIF_ETH,    "cd",       8,       0},
    {"ERP",         BCM_PORT_IF_ERP,        "erp",      0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"CPU",         BCM_PORT_IF_CPU,        "cpu",      0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"RCY_MIRROR",  BCM_PORT_IF_RCY_MIRROR, "mirror",   0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"RCY",         BCM_PORT_IF_RCY,        "rcy",      0,       DNX_DATA_PROPERTY_UCODE_FLAG_OPTIONAL_INDEX},
    {"OAMP",        BCM_PORT_IF_OAMP,       "oamp",     0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"OLP",         BCM_PORT_IF_OLP,        "olp",      0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"SAT",         BCM_PORT_IF_SAT,        "sat",      0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"EVENTOR",     BCM_PORT_IF_EVENTOR,    "eventor",  0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {"SCH",         BCM_PORT_IF_SCH,        "sch",      0,       DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX},
    {NULL,          BCM_PORT_IF_NULL,       NULL,       0,       0}  /*last*/
};
/* *INDENT-ON* */

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_ucode_port_read(
    int unit,
    int port,
    dnx_data_port_static_add_ucode_port_t * ucode_port)
{
    char *propval;
    char *str_pointer, *str_pointer_end;
    char *interface_type_str;
    int interface_type_str_length;
    int idx;
    int channel;
    char *prefix;
    int prefix_len;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Read SoC property
     */
    propval = dnx_drv_soc_property_port_get_str(unit, port, spn_UCODE_PORT);

    /*
     * If set - parse SoC property
     * Expected format:
     * "#interface_type##interface_id#[.#channel_num#]:core#core-id#.#tm-port#[:#stat#][:#kbp#][:#tdm#]"
     */
    if (propval)
    {
        str_pointer = propval;
        idx = 0;

        /*
         * Find interface type and interface offset
         * Iterate over ucode_port_info entries
         */
        while (ucode_ports_info[idx].interface_type_name)
        {
            interface_type_str = ucode_ports_info[idx].interface_type_name;
            interface_type_str_length = sal_strlen(interface_type_str);

            /*
             * compare interface type to current ucode_port_info entry
             */
            if (!sal_strncasecmp(str_pointer, interface_type_str, interface_type_str_length))
            {
                str_pointer += interface_type_str_length;
                ucode_port->interface = ucode_ports_info[idx].interface;
                ucode_port->nof_lanes = ucode_ports_info[idx].nof_lanes;

                /*
                 * If interface type requires interface offset - parse it
                 */
                if ((ucode_ports_info[idx].flags & DNX_DATA_PROPERTY_UCODE_FLAG_NOT_REQUIRED_INDEX) == 0 ||
                    (ucode_ports_info[idx].flags & DNX_DATA_PROPERTY_UCODE_FLAG_OPTIONAL_INDEX))
                {
                    /*
                     * Parse interface offset
                     */
                    ucode_port->interface_offset = sal_ctoi(str_pointer, &str_pointer_end);
                    /** Make sure that interface offset found */
                    if (str_pointer == str_pointer_end)
                    {
                        if (ucode_ports_info[idx].flags & DNX_DATA_PROPERTY_UCODE_FLAG_OPTIONAL_INDEX)
                        {
                            /** in a case that the index is optinal - just set to 0*/
                            ucode_port->interface_offset = 0;
                        }
                        else
                        {
                            /** throw an error */
                            SHR_ERR_EXIT(_SHR_E_FAIL, "No interface offset in (\"%s\") for %s\n", propval,
                                         spn_UCODE_PORT);
                        }
                    }
                    str_pointer = str_pointer_end;
                }

                break;
            }

            /** increment to next ucode_port_info entry */
            idx++;
        }

        /*
         * Make sure that interace type found
         */
        if (ucode_ports_info[idx].interface_type_name == NULL /* not found */ )
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Interface name not found 0 - (\"%s\") for %s\n", propval, spn_UCODE_PORT);
        }

        /*
         * Parse channel - optional param
         */
        if (str_pointer && (*str_pointer == '.'))
        {
            ++str_pointer;
            channel = sal_ctoi(str_pointer, &str_pointer_end);
            if (str_pointer != str_pointer_end)
            {
                ucode_port->channel = channel;
            }
            str_pointer = str_pointer_end;
        }

        /*
         * Parse additional params: core, is_stif, is_kbp
         */
        if (str_pointer && (*str_pointer == ':'))
        {
            ++str_pointer;

            /*
             * Core + TM port
             */
            prefix = "core_";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                str_pointer += prefix_len;

                ucode_port->core = sal_ctoi(str_pointer, &str_pointer_end);
                if (str_pointer == str_pointer_end)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Core not specified for (\"%s\") for %s", propval, spn_UCODE_PORT);
                }
                str_pointer = str_pointer_end;

                /** parse tm port */
                if (str_pointer && (*str_pointer == '.'))
                {
                    ++str_pointer;
                    ucode_port->tm_port = sal_ctoi(str_pointer, &str_pointer_end);
                    if (str_pointer == str_pointer_end)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "TM port not specify for (\"%s\") for %s", propval, spn_UCODE_PORT);
                    }
                    str_pointer = str_pointer_end;
                }
            }
        }

        /*
         * Parse is_stif and is_kbp
         */
        while (str_pointer && (*str_pointer == ':'))
        {
            ++str_pointer;

            /** Parse stat interface */
            prefix = "stat";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_stif = 1;
                str_pointer += prefix_len;
                continue;
            }

            /** Parse kbp interface */
            prefix = "kbp";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_kbp = 1;
                str_pointer += prefix_len;
                continue;
            }

            /** Parse flexe interface */
            prefix = "flexe";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_flexe_phy = 1;
                str_pointer += prefix_len;
                continue;
            }

            /** Parse kbp interface */
            prefix = "cross_connect";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_cross_connect = 1;
                str_pointer += prefix_len;
                continue;
            }
            /** Parse egress interleave */
            prefix = "egr_ilv";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_egress_interleave = 1;
                str_pointer += prefix_len;
                continue;
            }
            /** Parse tdm */
            prefix = "tdm";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_tdm = 1;
                str_pointer += prefix_len;
                continue;
            }
            /** Parse interface tdm only */
            prefix = "if_tdm_only";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_if_tdm_only = 1;
                str_pointer += prefix_len;
                continue;
            }
            /** Parse interface tdm hybrid*/
            prefix = "if_tdm_hybrid";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->is_if_tdm_hybrid = 1;
                str_pointer += prefix_len;
                continue;
            }
            /** Parse sch port priority propagation*/
            prefix = "sch_priority_propagation";
            prefix_len = sal_strlen(prefix);
            if (!sal_strncasecmp(str_pointer, prefix, prefix_len))
            {
                ucode_port->sch_priority_propagation_en = 1;
                str_pointer += prefix_len;
                continue;
            }
        }
        if (ucode_port->interface == BCM_PORT_IF_ILKN)
        {
            /*
             * Read is ILKN interleaved
             */
            ucode_port->is_ingress_interleave = dnx_drv_soc_property_suffix_num_get(unit, ucode_port->interface_offset,
                                                                                    spn_ILKN_IS_BURST_INTERLEAVING,
                                                                                    "", 0);

        }

        {
            /*
             * Special handling
             * Set port name -
             * Required in this stage to be able to use soc_porperty_port_get
             */
            char *port_name_p;
            int sizeof_port_name;

            SHR_IF_ERR_EXIT(soc_info_char_2dimensions_address_size_get
                            (unit, PORT_NAME, (int) port, &port_name_p, &sizeof_port_name));
            sal_snprintf(port_name_p, sizeof_port_name, "%s%d", ucode_ports_info[idx].port_name, port);
        }
        /*
         * Read number of port priorities
         */
        ucode_port->num_priorities =
            dnx_drv_soc_property_port_get(unit, port, spn_PORT_PRIORITIES, ucode_port->num_priorities);

        /*
         * Read Port Base Queue Pair
         */
        ucode_port->base_q_pair =
            dnx_drv_soc_property_port_get(unit, port, spn_OTM_BASE_Q_PAIR, ucode_port->base_q_pair);

        /*
         * Read number of SCH port priorities. Number of (EGQ) port priorities is the default number
         */
        ucode_port->num_sch_priorities = dnx_drv_soc_property_port_get(unit, port, spn_PORT_PRIORITIES_SCH, 0);

        /*
         * Read Port Base HR. Base q-pair is the default value
         */
        ucode_port->base_hr = dnx_drv_soc_property_port_get(unit, port, spn_PORT_BASE_HR, -1);

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_ext_stat_speed_read(
    int unit,
    int ext_stat_port,
    dnx_data_port_static_add_ext_stat_speed_t * ext_stat_speed)
{
    char *propkey, *propval;
    char **tokens = NULL;
    uint32 nof_tokens;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_PORT_INIT_SPEED;

    /*
     * read the key string and get the value string 
     */
    propval = dnx_drv_soc_property_suffix_num_only_suffix_str_get(unit, ext_stat_port, propkey, "ext_stat");
    if (propval == NULL)
    {
        ext_stat_speed->speed = DNXC_PORT_INVALID_SPEED;
        ext_stat_speed->nof_lanes = 0;
    }
    else
    {
        tokens = utilex_str_split(propval, ".", 2, &nof_tokens);
        if (tokens == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "utilex_str_split return NULL pointer \n");
        }
        if (nof_tokens != 2)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Soc property - port_init_speed_ext_stat%d=%s. Invalid value format \n",
                         ext_stat_port, propval);
        }
        ext_stat_speed->speed = sal_atoi(tokens[0]);
        /** support one digit nof_lanes */
        ext_stat_speed->nof_lanes = utilex_char_to_num(tokens[1][0]);
        if (sal_strcmp(&tokens[1][1], "lanes") != 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Soc property - port_init_speed_ext_stat%d=%s. missing word 'lanes' in the value string \n",
                         ext_stat_port, propval);
        }
    }

exit:
    utilex_str_split_free(tokens, 2);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_erp_exist_read(
    int unit,
    int core,
    dnx_data_port_static_add_erp_exist_t * erp_exist)
{
    int port_index;
    const dnx_data_port_static_add_ucode_port_t *ucode_port;

    SHR_FUNC_INIT_VARS(unit);

    erp_exist->exist = 0;
    for (port_index = 0; port_index < dnx_data_port.general.fabric_port_base_get(unit); port_index++)
    {
        ucode_port = dnx_data_port.static_add.ucode_port_get(unit, port_index);
        if (ucode_port->interface == BCM_PORT_IF_ERP && ucode_port->core == core)
        {
            erp_exist->exist = 1;
            break;
        }
    }

    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * MODULE: ELK
 * {
 */
/*
 * submodule connectivity
 */

/*
 * See .h file
 */

shr_error_e
dnx_data_property_elk_connectivity_topology_read(
    int unit,
    dnx_data_elk_connectivity_topology_t * topology)
{
    char *propkey, *propval, *sub_str, *sub_str_end;
    int kbp_port;
    int temp_mapping[DNX_DATA_MAX_ELK_GENERAL_MAX_PORTS];

    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_EXT_TCAM_START_LANE;
    for (kbp_port = 0; kbp_port < dnx_data_elk.general.max_ports_get(unit); ++kbp_port)
    {
        propval = dnx_drv_soc_property_suffix_num_str_get(unit, kbp_port, propkey, "port");
        sub_str = propval;

        if (propval == NULL)
        {
            topology->port_core_mapping[kbp_port] = kbp_port % 2;
            switch (kbp_port)
            {
                case 0:
                    topology->start_lane[kbp_port] = 0;
                    break;
                case 1:
                    /*
                     * Connect mode: 0 - SINGLE, 1 - DUAL_SHARED, 2 - DUAL_SMT
                     * Device type : 1 - BCM52311, 2 - BCM52321
                     */
                    topology->start_lane[kbp_port] = (dnx_data_elk.connectivity.connect_mode_get(unit) == 0) ?
                        ((dnx_data_elk.general.device_type_get(unit) == 2) ? 16 : 20) : 8;
                    break;
                case 2:
                    topology->start_lane[kbp_port] = 16;
                    break;
                case 3:
                    topology->start_lane[kbp_port] = 24;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Bad KBP port ID %d!\n", kbp_port);
            }
        }
        else
        {

            /**
             *Get the start lane number
             */
            topology->start_lane[kbp_port] = sal_ctoi(sub_str, &sub_str_end);
            sub_str = sub_str_end;
            if (*sub_str != '\0')
            {
                if (*sub_str != ':')
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP Port %d: Bad config string \"%s\"\n", kbp_port, propval);
                }
                /**
                 *Skip the ":" indicator to get the core ID
                 */
                sub_str++;
                if (sal_strcmp(sub_str, "core0") == 0)
                {
                    topology->port_core_mapping[kbp_port] = 0;
                }
                else if (sal_strcmp(sub_str, "core1") == 0)
                {
                    topology->port_core_mapping[kbp_port] = 1;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP Port %d: Bad core ID \"%s\"\n", kbp_port, propval);
                }
            }
            else
            {
                /**
                 *No core ID, assign a default core ID
                 */
                topology->port_core_mapping[kbp_port] = kbp_port % 2;
            }
        }
        temp_mapping[kbp_port] = topology->port_core_mapping[kbp_port];

        if ((kbp_port % 2 == 1) && (temp_mapping[kbp_port] == temp_mapping[kbp_port - 1]))
        {
            SHR_ERR_EXIT(SOC_E_CONFIG, "Cannot attach two KBP ports in the same core\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#define DNX_DATA_PROPERTY_MAX_KBP_TX_TAPS    6

shr_error_e
dnx_data_property_elk_connectivity_kbp_serdes_tx_taps_read(
    int unit,
    dnx_data_elk_connectivity_kbp_serdes_tx_taps_t * kbp_serdes_tx_taps)
{
    char *propval = NULL;
    bcm_port_phy_tx_t tx;
    enum kbp_blackhawk_tsc_txfir_tap_enable_enum txfir_tap_enable;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * read SoC property, which should be in this format:
     * signaling_mode:pre:main:post:pre2:post2:post3
     * the [:pre2:post2:post3] part is optional.
     */
    propval = dnx_drv_soc_property_get_str(unit, spn_EXT_TCAM_SERDES_TX_TAPS);

    SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_read(unit, -1, propval, &tx));
    /*
     * if the SoC property was given - copy the given values into serdes_tx_taps struct. if SoC property wasn't given - 
     * tx.main will be INVALID. In that case serdes_tx_taps struct will have the defaults set in dnx_data. 
     */
    if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
    {
        kbp_serdes_tx_taps->pre = tx.pre;
        kbp_serdes_tx_taps->main = tx.main;
        kbp_serdes_tx_taps->post = tx.post;

        /*
         * signalling_mode is bcmPortPhySignallingModeNRZ 
         */
        switch (tx.signalling_mode)
        {
            case bcmPortPhySignallingModeNRZ:
                txfir_tap_enable = (tx.tx_tap_mode == bcmPortPhyTxTapMode3Tap) ? NRZ_LP_3TAP : NRZ_6TAP;
                break;
            case bcmPortPhySignallingModePAM4:
                txfir_tap_enable = (tx.tx_tap_mode == bcmPortPhyTxTapMode3Tap) ? PAM4_LP_3TAP : PAM4_6TAP;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_FAIL, "Bad tx signalling_mode %d!\n", tx.signalling_mode);
        }

        kbp_serdes_tx_taps->txfir_tap_enable = txfir_tap_enable;

        /*
         * copy the optional taps if they were given 
         */
        if (tx.tx_tap_mode == bcmPortPhyTxTapMode6Tap)
        {
            kbp_serdes_tx_taps->pre2 = tx.pre2;
            kbp_serdes_tx_taps->post2 = tx.post2;
            kbp_serdes_tx_taps->post3 = tx.post3;
        }
        /*
         * clear default value in case tap mode is 3Tap 
         */
        else
        {
            kbp_serdes_tx_taps->pre2 = 0;
            kbp_serdes_tx_taps->post2 = 0;
            kbp_serdes_tx_taps->post3 = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_property_elk_connectivity_kbp_lane_serdes_tx_taps_read(
    int unit,
    int ext_lane,
    dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t * kbp_serdes_tx_taps)
{
    char *propkey, *propval = NULL;
    bcm_port_phy_tx_t tx;
    enum kbp_blackhawk_tsc_txfir_tap_enable_enum txfir_tap_enable;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * read SoC property, which should be in this format:
     * signaling_mode:pre:main:post:pre2:post2:post3
     * the [:pre2:post2:post3] part is optional.
     */
    propkey = spn_EXT_TCAM_SERDES_TX_TAPS;
    propval = dnx_drv_soc_property_suffix_num_only_suffix_str_get(unit, ext_lane, propkey, "lane");

    if (propval != NULL)
    {
        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_read(unit, -1, propval, &tx));
        /*
         * if the SoC property was given - copy the given values into serdes_tx_taps struct. if SoC property wasn't given - 
         * tx.main will be INVALID. In that case serdes_tx_taps struct will have the defaults set in dnx_data. 
         */
        if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
        {
            kbp_serdes_tx_taps->pre = tx.pre;
            kbp_serdes_tx_taps->main = tx.main;
            kbp_serdes_tx_taps->post = tx.post;

            /*
             * signalling_mode is bcmPortPhySignallingModeNRZ 
             */
            switch (tx.signalling_mode)
            {
                case bcmPortPhySignallingModeNRZ:
                    txfir_tap_enable = (tx.tx_tap_mode == bcmPortPhyTxTapMode3Tap) ? NRZ_LP_3TAP : NRZ_6TAP;
                    break;
                case bcmPortPhySignallingModePAM4:
                    txfir_tap_enable = (tx.tx_tap_mode == bcmPortPhyTxTapMode3Tap) ? PAM4_LP_3TAP : PAM4_6TAP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Bad tx signalling_mode %d!\n", tx.signalling_mode);
            }

            kbp_serdes_tx_taps->txfir_tap_enable = txfir_tap_enable;

            /*
             * copy the optional taps if they were given 
             */
            if (tx.tx_tap_mode == bcmPortPhyTxTapMode6Tap)
            {
                kbp_serdes_tx_taps->pre2 = tx.pre2;
                kbp_serdes_tx_taps->post2 = tx.post2;
                kbp_serdes_tx_taps->post3 = tx.post3;
            }
            /*
             * clear default value in case tap mode is 3Tap 
             */
            else
            {
                kbp_serdes_tx_taps->pre2 = 0;
                kbp_serdes_tx_taps->post2 = 0;
                kbp_serdes_tx_taps->post3 = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/*
 * MODULE: FC:
 * {
 */
/*
 * See .h file
 */
/**
 * \brief Function which read the SoC property and modify the data to the required value
 * Module - 'fc', Submodule - 'coe', table - 'mac_address'
 * MAC address of COE FC
 *
 * \param [in] unit - Unit #
 * \param [out] mac_address - data type already set to default and should be modified according to property
 *
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e
dnx_data_property_fc_coe_mac_address_read(
    int unit,
    dnx_data_fc_coe_mac_address_t * mac_address)
{
    uint8 prop_val[6] = { 0 };
    char *propkey = NULL;
    char *propval = NULL;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_FC_COE_MAC_ADDRESS;
    propval = dnx_drv_soc_property_get_str(unit, propkey);
    if (propval != NULL)
    {
        if (_shr_parse_macaddr(propval, &prop_val[0]) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INIT, "Unexpected property value (\"%s\") for %s\n", propval, propkey);
        }

        sal_memcpy(&mac_address->mac, prop_val, sizeof(dnx_data_fc_coe_mac_address_t));
    }
    else
    {
        sal_memset(mac_address->mac, 0, sizeof(uint8) * 6);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_property_fc_oob_calender_length_read(
    int unit,
    int oob_id,
    dnx_data_fc_oob_calender_length_t * calender_length)
{
    char *propkey = NULL;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_FC_OOB_CALENDER_LENGTH;
    /** Reading the SOC property using soc_property_port_get is a bug at legacy, which was fixed by overriding with
      * soc_property_suffix_num_get. We are keeping the same 4 lines here as in legacy in order to ensure we have the
      * the exact same behavior */
    calender_length->rx = dnx_drv_soc_property_port_get(unit, oob_id, propkey, calender_length->rx);
    calender_length->tx = dnx_drv_soc_property_port_get(unit, oob_id, propkey, calender_length->tx);
    calender_length->rx = dnx_drv_soc_property_suffix_num_get(unit, oob_id, propkey, "rx", calender_length->rx);
    calender_length->tx = dnx_drv_soc_property_suffix_num_get(unit, oob_id, propkey, "tx", calender_length->tx);

    if ((calender_length->rx > 512) || (calender_length->tx > 512))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "\nSOC Property %s value too large, max value is (%d)", propkey, 512);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_property_fc_oob_calender_rep_count_read(
    int unit,
    int oob_id,
    dnx_data_fc_oob_calender_rep_count_t * calender_rep_count)
{
    char *propkey = NULL;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_FC_OOB_CALENDER_REP_COUNT;
    /** Reading the SOC property using soc_property_port_get is a bug at legacy, which was fixed by overriding with
      * soc_property_suffix_num_get. We are keeping the same 4 lines here as in legacy in order to ensure we have the
      * the exact same behavior */
    calender_rep_count->rx = dnx_drv_soc_property_port_get(unit, oob_id, propkey, calender_rep_count->rx);
    calender_rep_count->tx = dnx_drv_soc_property_port_get(unit, oob_id, propkey, calender_rep_count->tx);
    calender_rep_count->rx = dnx_drv_soc_property_suffix_num_get(unit, oob_id, propkey, "rx", calender_rep_count->rx);
    calender_rep_count->tx = dnx_drv_soc_property_suffix_num_get(unit, oob_id, propkey, "tx", calender_rep_count->tx);

    if ((calender_rep_count->rx < 1) || (calender_rep_count->rx > 15)
        || (((dnx_data_fc.oob.calender_length_get(unit, oob_id)->rx) * calender_rep_count->rx) > 512))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "\nUnexpected property value (\"%d\") for %s, setting to min value (%d).",
                     calender_rep_count->rx, propkey, 1);
    }

    if ((calender_rep_count->tx < 1) || (calender_rep_count->tx > 15)
        || (((dnx_data_fc.oob.calender_length_get(unit, oob_id)->tx) * calender_rep_count->tx) > 512))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "\nUnexpected property value (\"%d\") for %s, setting to min value (%d).",
                     calender_rep_count->tx, propkey, 1);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_property_fc_inband_calender_length_read(
    int unit,
    int ilkn_id,
    dnx_data_fc_inband_calender_length_t * calender_length)
{
    char *propkey = NULL;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_FC_INBAND_INTLKN_CALENDER_LENGTH;
    /** Reading the SOC property using soc_property_port_get is a bug at legacy, which was fixed by overriding with
      * soc_property_suffix_num_get. We are keeping the same 4 lines here as in legacy in order to ensure we have the
      * the exact same behavior */
    calender_length->rx = dnx_drv_soc_property_port_get(unit, ilkn_id, propkey, calender_length->rx);
    calender_length->tx = dnx_drv_soc_property_port_get(unit, ilkn_id, propkey, calender_length->tx);
    calender_length->rx = dnx_drv_soc_property_suffix_num_get(unit, ilkn_id, propkey, "rx", calender_length->rx);
    calender_length->tx = dnx_drv_soc_property_suffix_num_get(unit, ilkn_id, propkey, "tx", calender_length->tx);

    if ((calender_length->rx > 256) || (calender_length->tx > 256))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "\nSOC Property %s value too large, max value is (%d)", propkey, 256);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * MODULE: SCH:
 * {
 */

typedef enum
{
    DNX_DATA_SCH_REGION_TYPE_CON = DNX_SCH_REGION_TYPE_CON,
    DNX_DATA_SCH_REGION_TYPE_INTERDIG_1 = DNX_SCH_REGION_TYPE_INTERDIG_1,
    DNX_DATA_SCH_REGION_TYPE_INTERDIG_2 = 2,
    DNX_DATA_SCH_REGION_TYPE_SE_DIRECTION_LOW_TO_HIGH = DNX_SCH_REGION_TYPE_SE,
    DNX_DATA_SCH_REGION_TYPE_SE_DIRECTION_HIGH_TO_LOW = 0x4 | DNX_SCH_REGION_TYPE_SE,
    DNX_DATA_SCH_REGION_INVALID_MAPPING_MODE = DNX_SCH_REGION_INVALID_MAPPING_MODE
} dnx_data_sch_region_type_e;

 /*
  * See .h file
  */
shr_error_e
dnx_data_property_sch_flow_region_type_read(
    int unit,
    int core,
    int region,
    dnx_data_sch_flow_region_type_t * region_type)
{
    char *propkey;
    int user_region;
    dnx_data_sch_region_type_e type;

    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_DTM_FLOW_MAPPING_MODE_REGION;
    region_type->type = DNX_SCH_REGION_INVALID_MAPPING_MODE;
    region_type->propagation_direction = 0;

    user_region = region + 1; /** user region starts from 1 */
    {
        /** zero-based calculation -- use region and not user_region */
        if (region < dnx_data_sch.flow.nof_connectors_only_regions_get(unit))
        {
            /*
             * set region type for Flow-only regions
             */
            region_type->type = DNX_SCH_REGION_TYPE_CON;
        }
        /** zero-based calculation -- use region and not user_region */
        else if (region >= (dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_hr_regions_get(unit)))
        {
            /*
             * set region type for HR regions
             */
            region_type->type = DNX_SCH_REGION_TYPE_SE_HR;
        }
        else
        {
            {
                if (core == 0)
                {
                    type =
                        dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core0_",
                                                                        DNX_SCH_REGION_INVALID_MAPPING_MODE);
                    if (type == DNX_DATA_SCH_REGION_INVALID_MAPPING_MODE)
                    {
                        type =
                            dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core0",
                                                                            DNX_SCH_REGION_INVALID_MAPPING_MODE);
                    }
                }
                else if (core == 1)
                {
                    type =
                        dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core1_",
                                                                        DNX_SCH_REGION_INVALID_MAPPING_MODE);
                    if (type == DNX_DATA_SCH_REGION_INVALID_MAPPING_MODE)
                    {
                        type =
                            dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core1",
                                                                            DNX_SCH_REGION_INVALID_MAPPING_MODE);
                    }
                }
                else
                {
                    SHR_ERR_EXIT(SOC_E_INTERNAL, "Unsupported number of cores\n");
                }
            }
            /**
             * Try without core id
             * Set default region type to be DNX_DATA_SCH_REGION_TYPE_SE_DIRECTION_LOW_TO_HIGH
             */
            if (type == DNX_DATA_SCH_REGION_INVALID_MAPPING_MODE)
            {
                type =
                    dnx_drv_soc_property_suffix_num_get(unit, user_region, propkey, "",
                                                        DNX_DATA_SCH_REGION_TYPE_SE_DIRECTION_LOW_TO_HIGH);
            }
            if (type == DNX_DATA_SCH_REGION_TYPE_CON || type == DNX_DATA_SCH_REGION_TYPE_INTERDIG_1)
            {
                /** valid type */
                region_type->type = type;
            }
            else if (type == DNX_DATA_SCH_REGION_TYPE_SE_DIRECTION_LOW_TO_HIGH)
            {
                /** valid type */
                region_type->type = DNX_SCH_REGION_TYPE_SE;
                region_type->propagation_direction = DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_LOW_TO_HIGH;
            }
            else if (type == DNX_DATA_SCH_REGION_TYPE_SE_DIRECTION_HIGH_TO_LOW)
            {
                /** valid type */
                region_type->type = DNX_SCH_REGION_TYPE_SE;
                region_type->propagation_direction = DBAL_ENUM_FVAL_SHARED_SHAPER_PROP_ORDER_HIGH_TO_LOW;
            }
            else if (type == DNX_DATA_SCH_REGION_TYPE_INTERDIG_2)
            {
                SHR_ERR_EXIT(SOC_E_PARAM, "Region type %d is not supported on this device\n", type);
            }
            else
            {
                SHR_ERR_EXIT(SOC_E_PARAM, "Invalid region type %d for region %d\n", type, user_region);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#define DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES 0xffffffff

/*
  * See .h file
  */
shr_error_e
dnx_data_property_sch_flow_nof_remote_cores_read(
    int unit,
    int core,
    int region,
    dnx_data_sch_flow_nof_remote_cores_t * nof_remote_cores)
{
    char *propkey;
    int user_region;

    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_DTM_FLOW_NOF_REMOTE_CORES_REGION;
    nof_remote_cores->val = DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES;

    user_region = region + 1; /** user region starts from 1 */
    {
        if (core == 0)
        {
            nof_remote_cores->val =
                dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core0_",
                                                                DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES);

            if (nof_remote_cores->val == DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES)
            {
                nof_remote_cores->val =
                    dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core0",
                                                                    DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES);
            }
        }
        else if (core == 1)
        {
            nof_remote_cores->val =
                dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core1_",
                                                                DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES);

            if (nof_remote_cores->val == DNX_SCH_REGION_INVALID_MAPPING_MODE)
            {
                nof_remote_cores->val =
                    dnx_drv_soc_property_suffix_num_get_only_suffix(unit, user_region, propkey, "core1",
                                                                    DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES);
            }
        }
        else
        {
            SHR_ERR_EXIT(SOC_E_INTERNAL, "Unsupported number of cores\n");
        }
    }
    /*
     * try without core id
     */
    if (nof_remote_cores->val == DNX_DATA_PROPERTY_INVALID_NOF_REMOTE_CORES)
    {
        nof_remote_cores->val = dnx_drv_soc_property_suffix_num_get(unit, user_region, propkey, "", 1);
    }

    /*
     * set nof_remote_cores->val=1 for HR regions
     */
    /** zero-based calculation -- use region and not user_region */
    if (region >= (dnx_data_sch.flow.nof_regions_get(unit) - dnx_data_sch.flow.nof_hr_regions_get(unit)))
    {
        nof_remote_cores->val = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Parse the common lane map property for NIF and Fabric lane swapping
 * Property Name: lane_to_serdes_map
 */

static shr_error_e
dnx_data_property_port_lane_to_serdes_mapping_parse(
    int unit,
    int lane_id,
    char *propval,
    int *serdes_rx,
    int *serdes_tx)
{
    char *prefix, *propval_tmp;
    int len;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the default value.
     */
    *serdes_rx = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    *serdes_tx = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;

    prefix = "NOT_MAPPED";
    len = strlen(prefix);

    /*
     * lane is mapped.
     */
    if (propval && sal_strncasecmp(propval, prefix, len))
    {
        prefix = "rx";
        len = strlen(prefix);

        if (sal_strncasecmp(propval, prefix, len))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - \"rx\" is missing",
                         lane_id);
        }
        propval += len;

        *serdes_rx = sal_ctoi(propval, &propval_tmp);
        if (propval == propval_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - srd_rx_id is missing",
                         lane_id);
        }

        propval = propval_tmp;

        prefix = ":";
        len = strlen(prefix);
        if (sal_strncasecmp(propval, prefix, len))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - \":\" is missing", lane_id);
        }
        propval += len;

        prefix = "tx";
        len = strlen(prefix);
        if (sal_strncasecmp(propval, prefix, len))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - \"tx\" is missing",
                         lane_id);
        }
        propval += len;

        *serdes_tx = sal_ctoi(propval, &propval_tmp);
        if (propval == propval_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid lane_to_serdes_map configuration lane %d - srd_tx_id is missing",
                         lane_id);
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/*
 * MODULE: LANE_MAP
 * {
 */
/*
 * submodule nif
 */

/*
 * See .h file
 */

shr_error_e
dnx_data_property_lane_map_nif_mapping_read(
    int unit,
    int lane_id,
    dnx_data_lane_map_nif_mapping_t * mapping)
{
    char *propval;
    int serdes_rx, serdes_tx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get a string of this format (the right side of the soc property): rx<srd_rx_id>:tx<srd_tx_id> or NOT_MAPPED
     */

    propval = dnx_drv_soc_property_suffix_num_only_suffix_str_get(unit, lane_id, spn_LANE_TO_SERDES_MAP, "nif_lane");

    SHR_IF_ERR_EXIT(dnx_data_property_port_lane_to_serdes_mapping_parse
                    (unit, lane_id, propval, &serdes_rx, &serdes_tx));

    mapping->serdes_rx_id = serdes_rx;
    mapping->serdes_tx_id = serdes_tx;
exit:
    SHR_FUNC_EXIT;

}
/*
 * }
 */

/*
 * MODULE: LANE_MAP
 * {
 */
/*
 * submodule fabric
 */

/*
 * See .h file
 */

shr_error_e
dnx_data_property_lane_map_fabric_mapping_read(
    int unit,
    int lane_id,
    dnx_data_lane_map_fabric_mapping_t * mapping)
{
    char *propval;
    int serdes_rx, serdes_tx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get a string of this format (the right side of the soc property): rx<srd_rx_id>:tx<srd_tx_id> or NOT_MAPPED
     */

    propval = dnx_drv_soc_property_suffix_num_only_suffix_str_get(unit, lane_id, spn_LANE_TO_SERDES_MAP, "fabric_lane");

    SHR_IF_ERR_EXIT(dnx_data_property_port_lane_to_serdes_mapping_parse
                    (unit, lane_id, propval, &serdes_rx, &serdes_tx));

    mapping->serdes_rx_id = serdes_rx;
    mapping->serdes_tx_id = serdes_tx;

exit:
    SHR_FUNC_EXIT;

}
/*
 * }
 */

/*
 * MODULE: LANE_MAP
 * {
 */
/*
 * submodule ilkn
 */

/*
 * See .h file
 */

shr_error_e
dnx_data_property_lane_map_ilkn_remapping_read(
    int unit,
    int lane_id,
    int ilkn_id,
    dnx_data_lane_map_ilkn_remapping_t * remapping)
{
    char buf[10];
    soc_pbmp_t ilkn_lanes;
    int lane_index, count, is_over_fabric, interface_base_phy;
    int active_lanes[DNX_DATA_MAX_NIF_PHYS_NOF_PHYS];
    const dnx_data_nif_ilkn_supported_phys_t *interface_supported_phys;

    SHR_FUNC_INIT_VARS(unit);

    is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->is_over_fabric;
    interface_supported_phys = dnx_data_nif.ilkn.supported_phys_get(unit, ilkn_id);

    if (is_over_fabric)
    {
        _SHR_PBMP_FIRST(interface_supported_phys->fabric_phys, interface_base_phy);
    }
    else
    {
        _SHR_PBMP_FIRST(interface_supported_phys->nif_phys, interface_base_phy);
    }
    /*
     * Initialize the dnx data value.
     */
    remapping->ilkn_lane_id = -1;
    /*
     * If the logical lane id is smaller that the base phy id, return directly.
     */
    if (lane_id < interface_base_phy)
    {
        SHR_EXIT();
    }

    ilkn_lanes = dnx_data_nif.ilkn.phys_get(unit, ilkn_id)->bitmap;

    /*
     * Store the lane index by order
     */
    count = 0;
    _SHR_PBMP_ITER(ilkn_lanes, lane_index)
    {
        /*
         * coverity protection 
         */
        if (lane_index > dnx_data_nif.phys.nof_phys_get(unit) - 1)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "ILKN lane bitmap is out of bound \n");
        }
        active_lanes[lane_index] = count;
        count++;
    }

    if (_SHR_PBMP_MEMBER(ilkn_lanes, lane_id))
    {
        /*
         * The Soc property format is ilkn_lane_map_<ilkn_id>_lane<num>=<ilkn lane id>
         */
        sal_snprintf(buf, sizeof(buf), "%d_lane", ilkn_id);
        remapping->ilkn_lane_id =
            dnx_drv_soc_property_suffix_num_get(unit, lane_id, spn_ILKN_LANE_MAP, buf, active_lanes[lane_id]);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * MODULE: NIF
 * {
 */
/*
 * submodule phys
 */

/*
 * See .h file
 */

shr_error_e
dnx_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    dnx_data_port_static_add_serdes_tx_taps_t * serdes_tx_taps)
{
    bcm_port_phy_tx_t tx;
    char *propval = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * read SoC property, which should be in this format:
     * signaling_mode:pre:main:post:pre2:post2:post3
     * the [:pre2:post2:post3] part is optional.
     */
    propval = dnx_drv_soc_property_port_get_str(unit, port, spn_SERDES_TX_TAPS);

    SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_read(unit, port, propval, &tx));
    /*
     * If serdes_tx_taps is not configured, tx.main should be equal with DNXC_PORT_TX_FIR_INVALID_MAIN_TAP,
     * in that case the SDK will get default tx parameters from Phymod.
     */
    if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
    {
        serdes_tx_taps->pre = tx.pre;
        serdes_tx_taps->main = tx.main;
        serdes_tx_taps->post = tx.post;
        serdes_tx_taps->tx_tap_mode = tx.tx_tap_mode;
        serdes_tx_taps->signalling_mode = tx.signalling_mode;

        if (serdes_tx_taps->tx_tap_mode == bcmPortPhyTxTapMode6Tap)
        {
            serdes_tx_taps->pre2 = tx.pre2;
            serdes_tx_taps->post2 = tx.post2;
            serdes_tx_taps->post3 = tx.post3;
        }
        else
        {
            serdes_tx_taps->pre2 = 0;
            serdes_tx_taps->post2 = 0;
            serdes_tx_taps->post3 = 0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */

shr_error_e
dnx_data_property_nif_ilkn_phys_read(
    int unit,
    int ilkn_id,
    dnx_data_nif_ilkn_phys_t * ilkn_phys)
{
    char *propval;
    bcm_pbmp_t tmp_pbmp;
    uint8 is_hex = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(tmp_pbmp);
    /*
     * Read SoC property
     */
    propval = dnx_drv_soc_property_suffix_num_str_get(unit, ilkn_id, spn_ILKN_LANES, "");
    if (propval)
    {
        if (_shr_pbmp_parse(propval, &tmp_pbmp, ilkn_phys->lanes_array, &is_hex) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "ILKN lane bitmap parsing failed \n");
        }

       /** if it's an old presentation of 24 bit bitmap, convert the bitmap to nif phys bitmap */
        if (is_hex)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_lanes_to_nif_phys_get(unit, ilkn_id, &tmp_pbmp, &ilkn_phys->bitmap));
        }
        else
        {
            BCM_PBMP_ASSIGN(ilkn_phys->bitmap, tmp_pbmp);
        }
        ilkn_phys->array_order_valid = (is_hex) ? FALSE : TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/*
 * }
 */

/*
 * MODULE: L3:
 * {
 */
/*
 * submodule rif
 */

shr_error_e
dnx_data_property_l3_rif_nof_rifs_read(
    int unit,
    uint32 *nof_rifs)
{
    int rif_id_max, outlif_physical_bank_size, max_nof_rifs;
    SHR_FUNC_INIT_VARS(unit);

    rif_id_max = dnx_drv_soc_property_get(unit, spn_RIF_ID_MAX, *nof_rifs);

    
    outlif_physical_bank_size = 4096;

    max_nof_rifs = dnx_data_l3.rif.max_nof_rifs_get(unit);
    /*
     * The number of rifs must be divisable by the size of an outlif physical bank.
     */
    if (rif_id_max % outlif_physical_bank_size == (outlif_physical_bank_size - 1))
    {
        /*
         * We also accept a legal value - 1, because the soc property's name is misleading.
         */
        rif_id_max++;
    }
    if (rif_id_max % outlif_physical_bank_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Soc property rif_id_max must be a multiple of %d. Given 0x%08x.",
                     outlif_physical_bank_size, rif_id_max);
    }
    if (rif_id_max > max_nof_rifs)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Soc property rif_id_max must be at most 0x%08x. Given 0x%08x.", max_nof_rifs,
                     rif_id_max);
    }

    *nof_rifs = rif_id_max;

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * MODULE: MDB:
 * {
 */
/*
 * submodule eedb
 */

/*
 * See .h file
 */
shr_error_e
dnx_data_property_mdb_eedb_outlif_physical_phase_granularity_data_granularity_read(
    int unit,
    int outlif_physical_phase,
    uint32 *data_granularity)
{
    uint32 data_granularity_temp, data_granularity_pair;
    uint32 mdb_basic_size;
    uint32 default_granularity;
    char *physical_phase_names[DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES] =
        { "XL", "S2", "L1", "S1", "L2", "M3", "M1", "M2" };

    SHR_FUNC_INIT_VARS(unit);

    if (outlif_physical_phase >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected outlif_physical_phase %d, max expected phase %d.",
                     outlif_physical_phase, DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES - 1);
    }

    mdb_basic_size = dnx_data_mdb.dh.data_out_granularity_get(unit);
    default_granularity = 2 * mdb_basic_size;

    data_granularity_temp =
        dnx_drv_soc_property_suffix_num_get_only_suffix(unit, 0, spn_OUTLIF_PHYSICAL_PHASE_DATA_GRANULARITY,
                                                        physical_phase_names[outlif_physical_phase],
                                                        default_granularity);

    if ((data_granularity_temp != mdb_basic_size) &&
        (data_granularity_temp != mdb_basic_size * 2) && (data_granularity_temp != mdb_basic_size * 4))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Unsupported data granularity (%d) for physical phase soc property %s_%s, supported data granularities are 30/60/120 bits.",
                     data_granularity_temp, spn_OUTLIF_PHYSICAL_PHASE_DATA_GRANULARITY,
                     physical_phase_names[outlif_physical_phase]);
    }

    if (outlif_physical_phase % dnx_data_mdb.eedb.nof_phase_per_mag_get(unit) != 0)
    {
        data_granularity_pair =
            dnx_drv_soc_property_suffix_num_get_only_suffix(unit, 0, spn_OUTLIF_PHYSICAL_PHASE_DATA_GRANULARITY,
                                                            physical_phase_names[outlif_physical_phase - 1],
                                                            default_granularity);
        if (data_granularity_temp != data_granularity_pair)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "soc properties %s_%s and %s_%s must have the same data granularity.",
                         spn_OUTLIF_PHYSICAL_PHASE_DATA_GRANULARITY, physical_phase_names[outlif_physical_phase - 1],
                         spn_OUTLIF_PHYSICAL_PHASE_DATA_GRANULARITY, physical_phase_names[outlif_physical_phase]);
        }
    }

    *data_granularity = data_granularity_temp;

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * MODULE: SBUSDMA_DESC:
 * {
 */
/*
 * submodule global
 */
/**
 * \brief Function which read the SoC property and modify the data to the required value
 * Module - 'sbusdma_desc', Submodule - 'global', table - 'enable_module_desc_dma'
 * Enable descriptor DMA per module
 *
 * \param [in] unit - Unit #
 * \param [in] module_enum_val - The module name.
 * \param [out] enable - data type already set to default and should be modified according to property
 *
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e
dnx_data_property_sbusdma_desc_global_enable_module_desc_dma_enable_read(
    int unit,
    int module_enum_val,
    uint32 *enable)
{
    uint32 enable_temp = 0;
    /*
     * This array must be aligned to sbusdma_desc_module_enable_e
     */
    char *module_names[SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES] =
        { "MDB_LPM", "MDB_EEDB", "MDB_INLIF", "MDB_VSI", "MDB_FEC", "MDB_MAP",
        "MDB_ISEM", "MDB_LEM", "MDB_OEM", "MDB_PPMC", "MDB_GLEM", "MDB_ESEM", "MDB_EXEM", "MDB_RMEP_EM"
    };

    SHR_FUNC_INIT_VARS(unit);

    if (module_enum_val >= SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected module_enum_val %d, max expected phase %d.",
                     module_enum_val, SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES - 1);
    }

    enable_temp =
        dnx_drv_soc_property_suffix_num_get_only_suffix(unit, 0, spn_DMA_DESC_AGGREGATOR_ENABLE_SPECIFIC,
                                                        module_names[module_enum_val], 0);

    if (enable_temp == FALSE)
    {
        *enable = FALSE;
    }
    else
    {
        *enable = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * MODULE: DRAM
 * {
 */
/*
 * submodule gddr6
 */

/** see header file */
shr_error_e
dnx_data_property_dram_gddr6_dq_map_read(
    int unit,
    int dram_index,
    int byte,
    dnx_data_dram_gddr6_dq_map_t * dq_map)
{
    int bit;
    char suffix[GDDR6_SWAP_SUFFIX_BUFFER_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * ext_ram_dq_swap_dramX_byteY_bitZ=M
     */
    for (bit = 0; bit < 8; bit++)
    {
        sal_snprintf(suffix, GDDR6_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_byte%d_bit", dram_index, byte);
        dq_map->dq_map[bit] = dnx_drv_soc_property_suffix_num_get(unit, bit, spn_EXT_RAM_DQ_SWAP, suffix, bit);
    }

    SHR_FUNC_EXIT;
}
/** see header file */
shr_error_e
dnx_data_property_dram_gddr6_dq_byte_map_read(
    int unit,
    int dram_index,
    int byte,
    dnx_data_dram_gddr6_dq_byte_map_t * dq_byte_map)
{
    char suffix[GDDR6_SWAP_SUFFIX_BUFFER_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * ext_ram_dq_swap_dramX_byteY=M 
     */
    sal_snprintf(suffix, GDDR6_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_byte", dram_index);
    dq_byte_map->dq_byte_map = dnx_drv_soc_property_suffix_num_get(unit, byte, spn_EXT_RAM_DQ_SWAP, suffix, byte);

    SHR_FUNC_EXIT;
}
/** see header file */
shr_error_e
dnx_data_property_dram_gddr6_ca_map_read(
    int unit,
    int dram_index,
    int channel,
    dnx_data_dram_gddr6_ca_map_t * ca_map)
{
    int ca_bit;
    char suffix[GDDR6_SWAP_SUFFIX_BUFFER_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * ext_ram_addr_bank_swap_dramX_channelY_bitZ=M 
     */
    /** CA have 12, nof_ca_bits+cabi */
    for (ca_bit = 0; ca_bit <= dnx_data_dram.gddr6.nof_ca_bits_get(unit); ca_bit++)
    {
        sal_snprintf(suffix, GDDR6_SWAP_SUFFIX_BUFFER_SIZE, "dram%d_channel%d_bit", dram_index, channel);
        ca_map->ca_map[ca_bit] =
            dnx_drv_soc_property_suffix_num_get(unit, ca_bit, spn_EXT_RAM_ADDR_BANK_SWAP, suffix, ca_bit);
    }

    SHR_FUNC_EXIT;
}
/** see header file */
shr_error_e
dnx_data_property_dram_gddr6_dq_channel_swap_read(
    int unit,
    int dram_index,
    dnx_data_dram_gddr6_dq_channel_swap_t * dq_channel_swap)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * ext_ram_channel_swap_en_dramX=M 
     */
    dq_channel_swap->dq_channel_swap =
        dnx_drv_soc_property_suffix_num_get(unit, dram_index, "ext_ram_channel_swap_en", "dram", 0);
    SHR_FUNC_EXIT;
}

/**
 * \brief - get timing parameter from soc property
 *
 * \param [in] unit - unit number
 * \param [in] prop - soc property name
 * \param [out] p_val - value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_gddr6_dram_timing_param_get(
    int unit,
    char *prop,
    uint32 *p_val)
{
    char *propval, *s;
    int val;
    SHR_FUNC_INIT_VARS(unit);

    propval = dnx_drv_soc_property_get_str(unit, prop);
    if (propval)
    {
        val = sal_ctoi(propval, &s);
        if ((*s == 'c') && (*(s + 1) == '\0'))
        {
            /** Property is XXXXc, so directly get clocks */
            *p_val = val;
        }
        else if (*s == '\0')
        {
            uint32 units_transformation_coefficient = 1000000;
            /** Property is a simple number, picoseconds. need transfer to clock */
            /** frequency is Mhz */
            /** calculation should be (property [ps] * 10^(-12)) * (frequency [MHz] * 10^6 = clocks) */
            /** this is the same as (property [ps]) * (frequency [MHz]) * 10^(-6) = clocks) */
            *p_val =
                UTILEX_DIV_ROUND_UP(val * dnx_data_dram.general_info.frequency_get(unit),
                                    units_transformation_coefficient);
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM,
                                     "Property %s is in a unknown format. Should be a number, or a number followed by c (e.g. 1000c)%s%s\n",
                                     prop, EMPTY, EMPTY);
        }
    }
    else
    {
        /*
         * Loading the device with DRAM requires the user to define the
         * timing params in soc properties, or having a default value in
         * dnx_data that is not -1.
         * We enforce this to make sure the user puts all DRAM needed
         * configuration.
         */
        if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0) && (*p_val == -1))
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "Property %s must be defined in order for the DRAM to work properly%s%s\n", prop,
                                     EMPTY, EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_general_info_timing_params_read(
    int unit,
    dnx_data_dram_general_info_timing_params_t * timing_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Value Property - twr */
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_WR, &timing_params->twr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RP, &timing_params->trp));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RTP_S, &timing_params->trtps));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RTP_L, &timing_params->trtpl));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RRD_S, &timing_params->trrds));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RRD_L, &timing_params->trrdl));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_FAW, &timing_params->tfaw));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RCD_WR, &timing_params->trcdwr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RCD_RD, &timing_params->trcdrd));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RAS, &timing_params->tras));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RC, &timing_params->trc));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_WTR_L, &timing_params->twtrl));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_WTR_S, &timing_params->twtrs));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_RTW, &timing_params->trtw));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_CCD_S, &timing_params->tccds));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, spn_EXT_RAM_T_CCD_L, &timing_params->tccdl));
    /** refresh interval, */
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rrefd", &timing_params->trrefd));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rfcsb", &timing_params->trfcsb));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rfc", &timing_params->trfc));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_mod", &timing_params->tmod));
    /** training timing */
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_wtrtr", &timing_params->twtrtr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_wrwtr", &timing_params->twrwtr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_reftr", &timing_params->treftr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rdtlt", &timing_params->trdtlt));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rcdwtr", &timing_params->trcdwtr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rcdrtr", &timing_params->trcdrtr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_rcdltr", &timing_params->trcdltr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_ltrtr", &timing_params->tltrtr));
    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_ltltr", &timing_params->tltltr));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_gddr6_refresh_intervals_trefiab_read(
    int unit,
    uint32 *trefiab)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Loading the device with DRAM requires the user to define the
     * timing params in soc properties, or having a default value in
     * dnx_data that is not -1.
     * We enforce this to make sure the user puts all DRAM needed
     * configuration.
     */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0)
    {
        *trefiab = dnx_drv_soc_property_get(unit, "ext_ram_t_abref_in_ns", *trefiab);

        if (*trefiab == -1)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "Property ext_ram_t_abref_in_ns must be defined in order for the DRAM to work properly%s%s%s\n",
                                     EMPTY, EMPTY, EMPTY);
        }

        SHR_MAX_VERIFY(*trefiab, 1024 * 1024 * 16 - 1, _SHR_E_PARAM, "ext_ram_t_abref_in_ns is out of bounds.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_gddr6_refresh_intervals_trefisb_read(
    int unit,
    uint32 *trefisb)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Loading the device with DRAM requires the user to define the
     * timing params in soc properties, or having a default value in
     * dnx_data that is not -1.
     * We enforce this to make sure the user puts all DRAM needed
     * configuration.
     */
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0)
    {
        *trefisb = dnx_drv_soc_property_get(unit, "ext_ram_t_refi_sb_in_ns", *trefisb);

        if (*trefisb == -1)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND,
                                     "Property ext_ram_t_refi_sb_in_ns must be defined in order for the DRAM to work properly%s%s%s\n",
                                     EMPTY, EMPTY, EMPTY);
        }

        SHR_MAX_VERIFY(*trefisb, 4095, _SHR_E_PARAM, "ext_ram_t_refi_sb_in_ns is out of bounds.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_general_info_write_latency_read(
    int unit,
    uint32 *write_latency)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_write_latency", write_latency));
    if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0))
    {
        SHR_MAX_VERIFY(*write_latency, 15, _SHR_E_PARAM, "ext_ram_write_latency is out of bounds.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_general_info_read_latency_read(
    int unit,
    uint32 *read_latency)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_read_latency", read_latency));
    if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0))
    {
        SHR_MAX_VERIFY(*read_latency, 30, _SHR_E_PARAM, "ext_ram_read_latency is out of bounds.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_general_info_crc_write_latency_read(
    int unit,
    uint32 *crc_write_latency)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_crc_wr_latency", crc_write_latency));
    if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0))
    {
        SHR_MAX_VERIFY(*crc_write_latency, 15, _SHR_E_PARAM, "ext_ram_t_crc_wr_latency is out of bounds.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_data_property_dram_general_info_crc_read_latency_read(
    int unit,
    uint32 *crc_read_latency)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_gddr6_dram_timing_param_get(unit, "ext_ram_t_crc_rd_latency", crc_read_latency));
    if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap != 0))
    {
        SHR_MAX_VERIFY(*crc_read_latency, 3, _SHR_E_PARAM, "ext_ram_t_crc_rd_latency is out of bounds.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_ext_stat_global_serdes_tx_taps_read(
    int unit,
    dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t * ext_stat_serdes_tx_taps)
{
    char *propkey, *propval;
    bcm_port_phy_tx_t tx;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_SERDES_TX_TAPS;

    propval = dnx_drv_soc_property_suffix_num_only_suffix_str_get(unit, -1, propkey, "ext_stat");
    if (propval != NULL)
    {
        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_read(unit, -1, propval, &tx));
        /*
         * if the SoC property was given - copy the given values into serdes_tx_taps struct. if SoC property wasn't given - 
         * tx.main will be INVALID. In that case serdes_tx_taps struct will have the defaults set in dnx_data. 
         */
        if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
        {
            ext_stat_serdes_tx_taps->pre = tx.pre;
            ext_stat_serdes_tx_taps->main = tx.main;
            ext_stat_serdes_tx_taps->post = tx.post;
            /** copy the optional taps if they were given */
            if (tx.tx_tap_mode == bcmPortPhyTxTapMode6Tap)
            {
                ext_stat_serdes_tx_taps->pre2 = tx.pre2;
                ext_stat_serdes_tx_taps->post2 = tx.post2;
                ext_stat_serdes_tx_taps->post3 = tx.post3;
            }
            /** clear default value in case tap mode is 3Tap */
            else
            {
                ext_stat_serdes_tx_taps->pre2 = 0;
                ext_stat_serdes_tx_taps->post2 = 0;
                ext_stat_serdes_tx_taps->post3 = 0;
            }

            /** must be 6TAP mode. choose between NRX or PAM4 */
            switch (tx.signalling_mode)
            {
                case bcmPortPhySignallingModeNRZ:
                    ext_stat_serdes_tx_taps->txfir_tap_enable = NRZ_6TAP;
                    break;
                case bcmPortPhySignallingModePAM4:
                    ext_stat_serdes_tx_taps->txfir_tap_enable = PAM4_6TAP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Bad tx signalling_mode %d!\n", tx.signalling_mode);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_port_static_add_ext_stat_lane_serdes_tx_taps_read(
    int unit,
    int ext_stat_lane,
    dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t * ext_stat_serdes_tx_taps)
{
    char *propkey, *propval;
    bcm_port_phy_tx_t tx;
    SHR_FUNC_INIT_VARS(unit);

    propkey = spn_SERDES_TX_TAPS;

    propval = dnx_drv_soc_property_suffix_num_only_suffix_str_get(unit, ext_stat_lane, propkey, "ext_stat_lane");
    if (propval != NULL)
    {
        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_read(unit, ext_stat_lane, propval, &tx));
        /*
         * if the SoC property was given - copy the given values into serdes_tx_taps struct. if SoC property wasn't given - 
         * tx.main will be INVALID. In that case serdes_tx_taps struct will have the defaults set in dnx_data. 
         */
        if (tx.main != DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
        {
            ext_stat_serdes_tx_taps->pre = tx.pre;
            ext_stat_serdes_tx_taps->main = tx.main;
            ext_stat_serdes_tx_taps->post = tx.post;
            /** copy the optional taps if they were given */
            if (tx.tx_tap_mode == bcmPortPhyTxTapMode6Tap)
            {
                ext_stat_serdes_tx_taps->pre2 = tx.pre2;
                ext_stat_serdes_tx_taps->post2 = tx.post2;
                ext_stat_serdes_tx_taps->post3 = tx.post3;
            }
            /** clear default value in case tap mode is 3Tap */
            else
            {
                ext_stat_serdes_tx_taps->pre2 = 0;
                ext_stat_serdes_tx_taps->post2 = 0;
                ext_stat_serdes_tx_taps->post3 = 0;
            }

            /** must be 6TAP mode. choose between NRX or PAM4 */
            switch (tx.signalling_mode)
            {
                case bcmPortPhySignallingModeNRZ:
                    ext_stat_serdes_tx_taps->txfir_tap_enable = NRZ_6TAP;
                    break;
                case bcmPortPhySignallingModePAM4:
                    ext_stat_serdes_tx_taps->txfir_tap_enable = PAM4_6TAP;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Bad tx signalling_mode %d!\n", tx.signalling_mode);
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_data_property_ingr_congestion_vsq_vsq_f_default_read(
    int unit,
    uint32 *vsq_f_default)
{
    uint32 reduced_lane;
    SHR_FUNC_INIT_VARS(unit);

    reduced_lane =
        dnx_drv_soc_property_get(unit, spn_REDUCED_PRIORITY_GROUPS_LANE,
                                 dnx_data_nif.phys.nof_phys_per_core_get(unit) - 1);
    *vsq_f_default = (reduced_lane * BCM_COSQ_VSQ_NOF_PG) + BCM_COSQ_VSQ_NOF_PG - 1;

    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * }
 */
