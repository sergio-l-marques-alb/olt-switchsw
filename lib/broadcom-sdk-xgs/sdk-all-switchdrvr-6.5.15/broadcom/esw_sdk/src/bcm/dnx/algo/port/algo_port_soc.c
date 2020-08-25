/** \file algo_port_soc.c
 *  
 *  PORT manager, Port utilities, and Port verifiers.
 *  
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *  
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/drv.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>

#include <bcm/types.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "algo_port_internal.h"

/*
 * }
 */
/*
 * Macros
 * {
 */
/** 
 * \brief - macro used to add port to soc info port bitmaps 
 *          ptype - See SOC_INFO list of port types  
 */
#define DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, ptype, logical_port) \
            do {\
                BCM_PBMP_PORT_ADD(SOC_INFO(unit).ptype.bitmap, logical_port);\
                if (SOC_PORT_MIN(unit,ptype) > logical_port || SOC_PORT_MIN(unit,ptype) == -1) SOC_PORT_MIN(unit,ptype) = logical_port;\
                if (SOC_PORT_MAX(unit,ptype) < logical_port) SOC_PORT_MAX(unit,ptype) = logical_port;\
            } while(0)

/** 
 * \brief - macro used to remove port to soc info port bitmaps 
 *          ptype - See SOC_INFO list of port types  
 */
#define DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, ptype, logical_port) \
            do {\
                BCM_PBMP_PORT_REMOVE(SOC_INFO(unit).ptype.bitmap, logical_port);\
                if (logical_port == SOC_PORT_MIN(unit,ptype)) _SHR_PBMP_FIRST(SOC_INFO(unit).ptype.bitmap, SOC_PORT_MIN(unit,ptype));\
                if (logical_port == SOC_PORT_MAX(unit,ptype)) _SHR_PBMP_LAST(SOC_INFO(unit).ptype.bitmap, SOC_PORT_MAX(unit,ptype));\
            } while(0)
/*
 * }
 */

/*
 * Local functions (documantation in function implementation)
 * {
 */
static shr_error_e dnx_algo_port_soc_info_port_name_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type);

static shr_error_e dnx_algo_port_soc_info_port_restore(
    int unit,
    bcm_port_t logical_port);
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_port_bitmaps_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    int nof_lanes;
    int port_has_speed;
    int speed;
    SHR_FUNC_INIT_VARS(unit);

    /** first, clear logical port from all bitmaps*/
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, xe, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, xl, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, il, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, ce, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, sfi, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, le, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, cc, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, cde, logical_port);
    BCM_PBMP_PORT_REMOVE((SOC_INFO(unit).cmic_bitmap), logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, rcy, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, tdm, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, port, logical_port);
    DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, all, logical_port);

    /** set appropriate bitmaps */
    if (port_type == DNX_ALGO_PORT_TYPE_INVALID)
    {
        /** do nothing */
    }
    else
    {
        DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, all, logical_port);
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type))
        {
            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, port, logical_port);
        }
        if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
        {
            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, tdm, logical_port);
        }

        if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type))
        {

            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, sfi, logical_port);
        }
        else if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
        {
            BCM_PBMP_PORT_ADD((SOC_INFO(unit).cmic_bitmap), logical_port);
        }
        else if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_type))
        {
            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, rcy, logical_port);
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, 1))
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, &port_has_speed));
            if (port_has_speed)
            {
                /*
                 * figure out the port type by number of lanes and interface speed
                 */

                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &nof_lanes));
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));

                switch (speed)
                {
                    case 400000:
                        DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, cde, logical_port);
                        break;

                    case 200000:
                        DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, cc, logical_port);
                        break;

                    case 100000:
                        DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, ce, logical_port);
                        break;

                    case 50000:
                    case 40000:
                        if (nof_lanes == 1)
                        {
                            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, le, logical_port);
                        }
                        else
                        {
                            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, xl, logical_port);
                        }
                        break;

                    default:
                    /** speed range 10000 - 25000 */
                        DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, xe, logical_port);
                }
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
        {
            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, il, logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register access per port is used for NIF ports and CPU ports.
 *          The SW configuration located in SOC INFO
 *          The function configure / removes it
 *  
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] is_master_port - see function dnx_algo_port_master_get for more information
 *   \param [in] port_type - port type of the added port
 *   \param [in] phy_ports - relevant just for nif ports - physical port bitmaps (one based)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO members which relevant for port access
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_soc_info_port_access_add(
    int unit,
    bcm_port_t logical_port,
    int is_master_port,
    dnx_algo_port_type_e port_type,
    bcm_pbmp_t phy_ports)
{
    int first_phy_port;
    int port_block, port_block_type;
    int lane_num;
    bcm_port_t phy_port, phy_port_i;
    int is_ilkn_over_fabric = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (port_type)
    {
            /*
             * NIF block access
             */
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
        {

            if ((port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN) || (port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK))
            {
                is_ilkn_over_fabric = 0;
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, logical_port, &is_ilkn_over_fabric));
            }

            if (!is_ilkn_over_fabric)
            {
                /** get first phy port */
                _SHR_PBMP_FIRST(phy_ports, first_phy_port);

                /** get physical block */
                port_block = SOC_PORT_IDX_INFO(unit, first_phy_port, 0).blk;
                if (port_block == -1)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d  first_phy_port %d is not supported\n", logical_port,
                                 first_phy_port);
                }

                /** if master port set as master block port*/
                if (is_master_port)
                {
                    SOC_INFO(unit).block_port[port_block] = logical_port;
                }

                /** add logical_port to block bitmap */
                SOC_PBMP_PORT_ADD(SOC_INFO(unit).block_bitmap[port_block], logical_port);

                /** Set port block type */
                port_block_type = SOC_BLOCK_INFO(unit, port_block).type;
                SOC_INFO(unit).port_type[logical_port] = port_block_type;

                /** set logical-to-physical*/
                SOC_INFO(unit).port_l2p_mapping[logical_port] = first_phy_port;

                /** set port lane num*/
                BCM_PBMP_COUNT(phy_ports, lane_num);
                SOC_INFO(unit).port_num_lanes[logical_port] = lane_num;

                /** set physical-to-logical */
                if (is_master_port)
                {
                    BCM_PBMP_ITER(phy_ports, phy_port_i)
                    {
                        SOC_INFO(unit).port_p2l_mapping[phy_port_i] = logical_port;
                    }
                }
            }
            else
            {
                /*
                 * ILKN over fabric case 
                 */
                _SHR_PBMP_FIRST(phy_ports, first_phy_port);

                SOC_INFO(unit).port_type[logical_port] = SOC_BLK_FSRD;

                /** set logical-to-physical*/
                phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + first_phy_port;
                SOC_INFO(unit).port_l2p_mapping[logical_port] = phy_port;

                /** set port lane num*/
                BCM_PBMP_COUNT(phy_ports, lane_num);
                SOC_INFO(unit).port_num_lanes[logical_port] = lane_num;

                /** set physical-to-logical */
                if (is_master_port)
                {
                    BCM_PBMP_ITER(phy_ports, phy_port_i)
                    {
                        phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + phy_port_i;
                        SOC_INFO(unit).port_p2l_mapping[phy_port] = logical_port;
                    }
                }
            }

            break;
        }
        case DNX_ALGO_PORT_TYPE_FABRIC:
        {
            int fabric_link = -1;

            /*
             * Take info from logical_port to fabric_link mapping instead of
             * phy_ports bitmap, because getting the first port in the bitmap
             * is not efficient.
             */

            /*
             * Set logical-to-physical.
             * We give fabric phisical ports an offest, so they will be
             * separated from NIF ports.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &fabric_link));
            phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + fabric_link;

            /** set logical-to-physical*/
            SOC_INFO(unit).port_l2p_mapping[logical_port] = phy_port;
            /** set physical-to-logical */
            SOC_INFO(unit).port_p2l_mapping[phy_port] = logical_port;
            /** set port lane num*/
            SOC_INFO(unit).port_num_lanes[logical_port] = 1;

            SOC_INFO(unit).port_type[logical_port] = SOC_BLK_FSRD;
            break;
        }
        default:
        {
            SOC_INFO(unit).port_type[logical_port] = SOC_BLK_ECI;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register access per port is used for NIF ports and CPU ports.
 *          The SW configuration located in SOC INFO
 *          The function configure / removes it
 *  
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] port_type - port type of the removed port
 *   \param [in] new_master_port - logical port of interface master port (after port remove), -1 if no master port
 *   \param [in] phy_ports - relevant just for nif ports - physical port bitmaps (one based)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO members which relevant for port access
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_soc_info_port_access_remove(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type,
    bcm_port_t new_master_port,
    bcm_pbmp_t phy_ports)
{
    int first_phy_port;
    int port_block;
    bcm_port_t phy_port, phy_port_i;
    int is_ilkn_over_fabric = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (port_type)
    {
            /*
             * NIF block access
             */
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
        {

            if ((port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN) || (port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK))
            {
                is_ilkn_over_fabric = 0;
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, logical_port, &is_ilkn_over_fabric));
            }

            if (!is_ilkn_over_fabric)
            {
                /** reset physical to logical */
                BCM_PBMP_ITER(phy_ports, phy_port_i)
                {
                    SOC_INFO(unit).port_p2l_mapping[phy_port_i] = new_master_port;
                }

                /** clear to block bitmap */
                _SHR_PBMP_FIRST(phy_ports, first_phy_port);
                port_block = SOC_PORT_IDX_INFO(unit, first_phy_port, 0).blk;

                BCM_PBMP_PORT_REMOVE(SOC_INFO(unit).block_bitmap[port_block], logical_port);

                /** reset block port */
                SOC_INFO(unit).block_port[port_block] = new_master_port;
            }
            else
            {
                /** reset physical to logical */
                BCM_PBMP_ITER(phy_ports, phy_port_i)
                {
                    phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + phy_port_i;
                    SOC_INFO(unit).port_p2l_mapping[phy_port] = new_master_port;
                }
            }

            /** remove port block type */
            SOC_INFO(unit).port_type[logical_port] = SOC_BLK_NONE;

            /** clear logical-to-physical */
            SOC_INFO(unit).port_l2p_mapping[logical_port] = -1;

            break;
        }
        default:
        {
            /** Do nothing */
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_add(
    int unit,
    bcm_port_t logical_port,
    int is_master_port,
    dnx_algo_port_type_e port_type,
    bcm_pbmp_t phy_ports,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add port access configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_access_add(unit, logical_port, is_master_port, port_type, phy_ports));

    /*
     * Add to port bitmap 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set(unit, logical_port, port_type, tdm_mode));

    /*
     * Set port name
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_name_set(unit, logical_port, port_type));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_remove(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type,
    bcm_port_t new_master_port,
    bcm_pbmp_t phy_ports)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Remove port access configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_access_remove
                    (unit, logical_port, port_type, new_master_port, phy_ports));

    /*
     * Remove port from soc info bitmaps
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set
                    (unit, logical_port, DNX_ALGO_PORT_TYPE_INVALID, DNX_ALGO_PORT_TDM_MODE_NONE));

    /*
     * Remove port name
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_name_set(unit, logical_port, DNX_ALGO_PORT_TYPE_INVALID));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_init(
    int unit)
{
    bcm_port_t logical_port;
    bcm_port_t phy_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Init physical port info */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++)
    {
        SOC_INFO(unit).port_p2l_mapping[phy_port] = -1;
    }

    /** Init logical port info */
    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {
        SOC_INFO(unit).port_l2p_mapping[logical_port] = -1;
        SOC_INFO(unit).port_type[logical_port] = SOC_BLK_NONE;
        SOC_INFO(unit).port_num_lanes[logical_port] = 0;
    }

    /** Init fabric logical port base info */
    SOC_INFO(unit).fabric_logical_port_base = dnx_data_port.general.fabric_port_base_get(unit);

/** exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Set port name and alternative port name - used mostly be soc property read. 
 * The name set according to port type and port interface type (read from dnx algo port DB) 
 * This function will be called after port add and remove to update port name. 
 *  
 * to remove set port_type to DNX_ALGO_PORT_TYPE_INVALID
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - required logical port.
 *   \param [in] port_type - port type of the added /removed port
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO port name
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_soc_info_port_name_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type)
{
    char *port_name;
    char *port_name_alter;
    int port_name_alter_valid;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port name - according to port type
     */
    /** set default to without alternative name */
    port_name_alter = "";
    port_name_alter_valid = 0;

    switch (port_type)
    {
        case DNX_ALGO_PORT_TYPE_INVALID:
        {
            port_name = "";
            port_name_alter = "";
            port_name_alter_valid = 0;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FABRIC:
        {
            port_name = "sfi";
            port_name_alter = "fabric";
            port_name_alter_valid = 1;
            break;
        }
        case DNX_ALGO_PORT_TYPE_CPU:
        {
            port_name = "cpu";
            break;
        }
        case DNX_ALGO_PORT_TYPE_OAMP:
        {
            port_name = "oamp";
            break;
        }
        case DNX_ALGO_PORT_TYPE_OLP:
        {
            port_name = "olp";
            break;
        }
        case DNX_ALGO_PORT_TYPE_SAT:
        {
            port_name = "sat";
            break;
        }
        case DNX_ALGO_PORT_TYPE_EVENTOR:
        {
            port_name = "eventor";
            break;
        }
        case DNX_ALGO_PORT_TYPE_ERP:
        {
            port_name = "erp";
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY:
        {
            port_name = "rcy";
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        {
            port_name = "rcy_mirror";
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
            port_name = "il";
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
            port_name = "eth";
            break;

        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port type for port %d (%d).\n", logical_port, port_type);
            break;
        }
    }

    /*
     * Set port name 
     */
    /** Set port name in SOC CONTROL*/
    sal_snprintf(SOC_INFO(unit).port_name[logical_port], sizeof(SOC_INFO(unit).port_name[logical_port]), "%s%d",
                 port_name, logical_port);
    sal_snprintf(SOC_INFO(unit).port_name_alter[logical_port], sizeof(SOC_INFO(unit).port_name_alter[logical_port]),
                 "%s%d", port_name_alter, logical_port);
    SOC_INFO(unit).port_name_alter_valid[logical_port] = port_name_alter_valid;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_port_eth_bitmap_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_header_mode_e header_mode)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, logical_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if (!SOC_WARM_BOOT(unit))
    {
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            dnx_algo_port_db.pp.header_mode.set(unit, gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                gport_info.internal_port_pp_info.pp_port[pp_port_index], header_mode);
        }
    }

    switch (header_mode)
    {
        case DNX_ALGO_PORT_HEADER_MODE_ETH:
        case DNX_ALGO_PORT_HEADER_MODE_MPLS_RAW:
        case DNX_ALGO_PORT_HEADER_MODE_INJECTED_2_PP:
        {
            DNX_ALGO_PORT_SOC_INFO_PORT_ADD(unit, ether, logical_port);
            break;
        }
        case DNX_ALGO_PORT_HEADER_MODE_NON_PP:
            DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE(unit, ether, logical_port);
            break;
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Header_mode %d isn't supported (port %d)\n", header_mode, logical_port);
            break;
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Restore information stored in soc info per logical port.
 * This API should called only upon warm reboot
 */
static shr_error_e
dnx_algo_port_soc_info_port_restore(
    int unit,
    bcm_port_t logical_port)
{
    bcm_pbmp_t phy_ports;
    dnx_algo_port_type_e port_type;
    int is_master_port;
    dnx_algo_port_tdm_mode_e tdm_mode;
    dnx_algo_port_header_mode_e header_mode;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int is_lag = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Collect data to restore soc info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
    BCM_PBMP_CLEAR(phy_ports);
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phy_ports));
    }
    /** Until TDM mode will be set assume the port is not TDM */
    tdm_mode = DNX_ALGO_PORT_TDM_MODE_NONE;
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tdm_mode));
    }

    /*
     * Config SoC Info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add(unit, logical_port, is_master_port, port_type, phy_ports, tdm_mode));

    /*
     * Restoring Headers and Ethernet bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, logical_port, &is_lag));
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type) && !is_lag)
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, logical_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.header_mode.get(unit, gport_info.internal_port_pp_info.core_id[0],
                                                            gport_info.internal_port_pp_info.pp_port[0], &header_mode));

        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_eth_bitmap_set(unit, logical_port, header_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_restore(
    int unit)
{
    bcm_pbmp_t valid_logical_ports;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Warm boot restore sequence. 
     * Sync soc_control port related data with the relevant data in sw state
     */
    /** Init SOC INFO */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_init(unit));

    /** restore each valid port soc info */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0, &valid_logical_ports));
    BCM_PBMP_ITER(valid_logical_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_restore(unit, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_ref_clk_get(
    int unit,
    bcm_port_t logical_port,
    int *ref_clk)
{
    int pll_index = -1;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type))
    {
        int link_id = SOC_DNX_FABRIC_PORT_TO_LINK(unit, logical_port);

        pll_index = link_id / (dnx_data_fabric.links.nof_links_get(unit) / 2);
        *ref_clk = dnx_data_pll.general.fabric_pll_cfg_get(unit, pll_index)->out_freq;
        if (*ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
        {
            *ref_clk = dnx_data_pll.general.fabric_pll_cfg_get(unit, pll_index)->in_freq;
        }
    }
    else
    {
        dnx_algo_port_cdu_access_info_t cdu_info;
        SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, logical_port, &cdu_info));

        pll_index = cdu_info.cdu_id >= dnx_data_nif.eth.cdu_nof_per_core_get(unit) ? 1 : 0;
        *ref_clk = dnx_data_pll.general.nif_pll_cfg_get(unit, pll_index)->out_freq;
        if (*ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
        {
            *ref_clk = dnx_data_pll.general.nif_pll_cfg_get(unit, pll_index)->in_freq;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
