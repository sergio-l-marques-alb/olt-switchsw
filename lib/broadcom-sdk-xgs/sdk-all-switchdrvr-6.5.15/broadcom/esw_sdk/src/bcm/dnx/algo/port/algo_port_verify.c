/** \file algo_port_verify.c
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
#include <bcm/types.h>

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>

#include "algo_port_internal.h"
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_free_verify(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /** verify port range */
    if (logical_port < 0 || logical_port >= SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port is out of range %d. Max is %d.\n", logical_port, SOC_MAX_NUM_PORTS - 1);
    }

    /** Verify that port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state != DNX_ALGO_PORT_STATE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port expected to be free %d.\n", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_valid_verify(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /** verify port range */
    if (logical_port < 0 || logical_port >= SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port is out of range %d. Max is %d.\n", logical_port, SOC_MAX_NUM_PORTS - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state == DNX_ALGO_PORT_STATE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Logical port expected to be valid %d.\n", logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type)
{
    dnx_algo_port_type_e actual_port_type;
    char port_type_exp[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    char port_type_got[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    SHR_FUNC_INIT_VARS(unit);

    /** get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &actual_port_type));

    /** compare port type */
    if (actual_port_type != port_type)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, port_type, port_type_exp));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, actual_port_type, port_type_got));
        SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected port type Expect %s, got %s.\n", port_type_exp, port_type_got);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_port_valid_verify(
    int unit,
    bcm_core_t core,
    bcm_port_t tm_port)
{
    int valid;
    int nof_cores;
    int nof_tm_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify tm port */
    nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
    if (tm_port < 0 || tm_port >= nof_tm_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port is out of range %d. Max is %d.\n", core, nof_tm_ports - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.valid.get(unit, core, tm_port, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port expected to be valid (core %d, tm port %d) .\n", core, tm_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_port_free_verify(
    int unit,
    dnx_algo_port_type_e port_type,
    bcm_core_t core,
    bcm_port_t tm_port)
{
    int valid;
    int nof_cores;
    int nof_tm_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify tm port */
    nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
    if (tm_port < 0 || tm_port >= nof_tm_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port is out of range %d. Max is %d.\n", core, nof_tm_ports - 1);
    }

    /** verify tm port is not reserved tm port */
    /** reserved for ERP */
    if (!DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type))
    {
        if (tm_port == dnx_data_port.general.erp_tm_port_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "TM port reserved for ERP %d.\n", tm_port);
        }
    }
    else
    {
        if (tm_port != dnx_data_port.general.erp_tm_port_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "ERP TM port was set to %d, should be %d.\n", tm_port,
                         dnx_data_port.general.erp_tm_port_get(unit));
        }
    }
    /** reserved TM port (used for returned credits in LAG SCH)*/
    if (tm_port == dnx_data_port.general.reserved_tm_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port %d is reserved and cannot be used.\n", tm_port);
    }

    /** Verify that port is not valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.valid.get(unit, core, tm_port, &valid));
    if (valid)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "TM port expected to be free (core %d, tm port %d) .\n", core, tm_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_free_verify(
    int unit,
    bcm_core_t core,
    int base_hr,
    int nof_sch_priorities)
{
    int nof_cores;
    int nof_hrs;
    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    nof_hrs = dnx_data_sch.flow.nof_hr_get(unit);

    /** avoid verify if without ID */
    if (base_hr != nof_hrs)
    {
        /** verify tm port */
        if (base_hr < 0 || base_hr + nof_sch_priorities > nof_hrs)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "HR is out of range %d:%d. Max is %d.\n", base_hr,
                         base_hr + nof_sch_priorities - 1, nof_hrs - 1);
        }

        /** verify HR is not reserved */
        if (base_hr <= dnx_data_sch.flow.reserved_hr_get(unit) &&
            base_hr + nof_sch_priorities > dnx_data_sch.flow.reserved_hr_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "HR %d is reserved and cannot be used.\n",
                         dnx_data_sch.flow.reserved_hr_get(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_valid_verify(
    int unit,
    bcm_core_t core,
    bcm_port_t pp_port)
{
    int valid;
    int nof_cores;
    int nof_pp_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** verify core range */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    if (core < 0 || core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core is out of range %d. Max is %d.\n", core, nof_cores - 1);
    }

    /** verify pp port */
    nof_pp_ports = dnx_data_port.general.nof_pp_ports_get(unit);
    if (pp_port < 0 || pp_port >= nof_pp_ports)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "PP port is out of range %d. Max is %d.\n", core, nof_pp_ports - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.valid.get(unit, core, pp_port, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "PP port expected to be valid (core %d, pp port %d) .\n", core, pp_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_link_valid_verify(
    int unit,
    int fabric_link)
{
    int valid;
    int nof_fabric_links;
    SHR_FUNC_INIT_VARS(unit);

    /** verify fabric link */
    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    if (fabric_link < 0 || fabric_link >= nof_fabric_links)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fabric link is out of range %d. Max is %d.\n", fabric_link, nof_fabric_links - 1);
    }

    /** Verify that port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.valid.get(unit, fabric_link, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fabric link expected to be valid (%d) .\n", fabric_link);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_phy_verify(
    int unit,
    int fabric_phy)
{
    SHR_FUNC_INIT_VARS(unit);

    if (fabric_phy < 0 || fabric_phy >= dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "phy is out of bound - %d \n", fabric_phy);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_supported_verify(
    int unit,
    dnx_algo_port_type_e port_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that port type is supported. 
     * When new device. more sophisticated verify will be requried (using dnx data). 
     */
    if (port_type < 0 || port_type >= DNX_ALGO_PORT_TYPE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port type is not supported - %d.\n", port_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_over_fabric_phys_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t phys)
{
    int pm_id = 0;
    int link_id = 0;
    uint32 pm_bypassed_links = 0;
    uint32 is_bypassed = 0;
    int nof_links_in_pm = 0;
    int nof_fabric_pms = 0;
    bcm_pbmp_t bypassed_phys_bmp;
    bcm_pbmp_t illegal_phys_bmp;
    bcm_pbmp_t ilkn_port_bmp;
    bcm_pbmp_t fabric_link_bmp;
    bcm_port_t port;
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_pm = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);

    /*
     *  Make sure all requested links are in bypass mode
     */
    BCM_PBMP_CLEAR(bypassed_phys_bmp);

    for (pm_id = 0; pm_id < nof_fabric_pms; pm_id++)
    {
        pm_bypassed_links = dnx_data_fabric.ilkn.bypass_info_get(unit, pm_id)->links;

        for (link_id = 0; link_id < nof_links_in_pm; link_id++)
        {
            is_bypassed = SHR_BITGET(&pm_bypassed_links, link_id) ? 1 : 0;
            if (is_bypassed)
            {
                BCM_PBMP_PORT_ADD(bypassed_phys_bmp, pm_id * nof_links_in_pm + link_id);
            }
        }
    }

    BCM_PBMP_ASSIGN(illegal_phys_bmp, phys);
    BCM_PBMP_REMOVE(illegal_phys_bmp, bypassed_phys_bmp);
    if (!BCM_PBMP_IS_NULL(illegal_phys_bmp))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ILKN port %d is assigned to phys which are not in bypass mode\n", logical_port);
    }

    /*
     * Make sure no ILKN ports using those phys
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                     &ilkn_port_bmp));

    BCM_PBMP_PORT_REMOVE(ilkn_port_bmp, logical_port);
    BCM_PBMP_ITER(ilkn_port_bmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
        if (is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &fabric_link_bmp));

            BCM_PBMP_AND(fabric_link_bmp, phys);
            if (!BCM_PBMP_IS_NULL(fabric_link_bmp))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d and port %d share phys.\n", logical_port, port);
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
dnx_algo_port_nif_phys_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t phys)
{
    const dnx_data_nif_phys_general_t *phys_info;
    bcm_pbmp_t unsupported_phys;
    char phys_str[_SHR_PBMP_FMT_LEN];
    bcm_port_t active_nif_port;
    int phy_id, is_active = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get supported phys per device and throw an error if phys include unsupported phy
     */
    /** get phys info from dnx data */
    phys_info = dnx_data_nif.phys.general_get(unit);

    /** calc bitmap of unsupported phys */
    BCM_PBMP_ASSIGN(unsupported_phys, phys);
    BCM_PBMP_REMOVE(unsupported_phys, phys_info->supported_phys);

    /** if this bitmap is not empty throw an error */
    if (!BCM_PBMP_IS_NULL(unsupported_phys))
    {
        _SHR_PBMP_FMT(unsupported_phys, phys_str);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported nif phys for port %d ( %s ).\n", logical_port, phys_str);
    }

    /*
     * Make sure no one else using those phys
     */
    BCM_PBMP_ITER(phys, phy_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
        if (is_active)
        {
            /*
             * ILKN over fabric port will not go into this API, hence setting
             * "is_over_fabric" flag to "0".
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, phy_id, 0, 0, &active_nif_port));
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d and port %d share phys.\n", logical_port, active_nif_port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phy_verify(
    int unit,
    int nif_phy)
{
    bcm_pbmp_t supported_phys;

    SHR_FUNC_INIT_VARS(unit);

    if (nif_phy < 0 || nif_phy >= dnx_data_nif.phys.nof_phys_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "phy is out of bound - %d \n", nif_phy);
    }
    /*
     * Verify if phy is supported on SKU
     */
    supported_phys = dnx_data_nif.phys.general_get(unit)->supported_phys;
    if (!BCM_PBMP_MEMBER(supported_phys, nif_phy))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "phy is not supported on this SKU - %d \n", nif_phy);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the ILKN lanes assignment is legal
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_lanes_bmp - ILKN lanes bitmap
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_nif_ilkn_lanes_verify(
    int unit,
    bcm_pbmp_t ilkn_lanes_bmp)
{
    int lane_id;
    uint32 ilkn_lanes[1], allowed_lanes;

    SHR_FUNC_INIT_VARS(unit);

    ilkn_lanes[0] = 0;
    _SHR_PBMP_ITER(ilkn_lanes_bmp, lane_id)
    {
        SHR_BITSET(ilkn_lanes, lane_id);
    }
    if (ilkn_lanes[0] == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No lane is assigned for this ILKN port! \n");
    }
    /*
     * Verify if all the lanes are located in the allowed lanes
     */
    allowed_lanes = (1 << dnx_data_nif.ilkn.lanes_allowed_nof_get(unit)) - 1;
    if ((ilkn_lanes[0] & ~allowed_lanes) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ILKN lane assignment is incorrect, the allowed lanes should be in bitmap 0x%x! \n",
                     allowed_lanes);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type,
    dnx_algo_port_if_add_t * info)
{
    int first_phy;
    int nif_handle;
    int valid;
    bcm_port_t master_port;
    dnx_algo_port_type_e master_port_type;
    int master_interface_offset;
    bcm_pbmp_t master_phys;
    int phy;
    uint32 is_over_fabric = 0;
    dnx_algo_lane_map_type_e lane_map_type;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        is_over_fabric = info->ilkn_info.is_ilkn_over_fabric;
    }

    /*
     * Check if master port or not
     */

    _SHR_PBMP_FIRST(info->phys, first_phy);
    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_phy_verify(unit, first_phy));

        /** Add fabric links offset for NIF DB handle */
        first_phy += dnx_data_port.general.fabric_phys_offset_get(unit);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_verify(unit, first_phy));
    }

    nif_handle = first_phy;

    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, nif_handle, &valid));

    if (valid)
    {   /* i.e. channelized port */
        /** make sure that channel is defined */
        if (info->tm_info.channel == -1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port (%d) is channelized. expecting channel != -1 .\n", logical_port);
        }

        /*
         *  Make sure that indeed the already defined port is identical
         */

        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, nif_handle, &master_port));

        /** port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, master_port, &master_port_type));
        if (port_type != master_port_type)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "port (%d) is channelized. expecting identical port type to master port (%d).\n",
                         logical_port, master_port);
        }
        /** interface offset */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.get(unit, master_port, &master_interface_offset));
        if (info->interface_offset != master_interface_offset)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "port (%d) is channelized. expecting identical interface offset to master port (%d).\n",
                         logical_port, master_port);
        }
        /** phys */

        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, &master_phys));

        if (!BCM_PBMP_EQ(info->phys, master_phys))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "port (%d) is channelized. expecting identical phys to master port (%d).\n",
                         logical_port, master_port);
        }
    }
    else
    { /** i.e. master port */

        /*
         * Make sure that no one else is using those phys
         */
        if (!is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_verify(unit, logical_port, info->phys));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_over_fabric_phys_verify(unit, logical_port, info->phys));
        }

        /** Lane Map verify, make sure all the lanes have been mapped and Lane map boundaries are correct*/

        lane_map_type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;

        
        if (!dnx_data_dev_init.general.access_only_get(unit) && !SOC_IS_J2C(unit))
        {
            BCM_PBMP_ITER(info->phys, phy)
            {
                SHR_IF_ERR_EXIT(dnx_algo_lane_map_port_add_verify
                                (unit, lane_map_type, port_type, is_over_fabric, phy));
            }
        }
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_lanes_verify(unit, info->ilkn_info.ilkn_lanes));
        }
        /*
         * Make sure interface and interface offset are valid, and match number of phys
         */

        
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_mode_supported_verify(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that tdm mode is supported. 
     * When new device. more sophisticated verify will be requried (using dnx data). 
     */
    if (tdm_mode < 0 || tdm_mode >= DNX_ALGO_PORT_TDM_MODE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TDM mode is not supported - %d.\n", tdm_mode);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_id_verify(
    int unit,
    int if_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that egress interface id is valid. 
     * Assuming there are no overlaps (skip this check)
     */
    if (if_id < 0 || if_id >= dnx_data_port.egress.nof_ifs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EGR interface id is not supported - %d.\n", if_id);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channel_free_verify(
    int unit,
    bcm_port_t logical_port,
    int tm_interface_handle,
    int channel)
{
    int tm_interface_valid;
    bcm_port_t master_logical_port, logical_port_channel;
    int channel_id;
    bcm_pbmp_t logical_port_channels;
    SHR_FUNC_INIT_VARS(unit);

    /** verify handle */
    DNX_ALGO_PORT_HANDLE_VERIFY(tm_interface_handle);
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.valid.get(unit, tm_interface_handle, &tm_interface_valid));
    if (tm_interface_valid)
    {
        /** get master port of the same interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.master_non_tdm_logical_port.get(unit, tm_interface_handle,
                                                                                      &master_logical_port));

        /** get all channels */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, master_logical_port, 0, &logical_port_channels));
        /** remove new port */
        BCM_PBMP_PORT_REMOVE(logical_port_channels, logical_port);

        /** iterate over ports and make sure channels is not used */
        BCM_PBMP_ITER(logical_port_channels, logical_port_channel)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port_channel, &channel_id));
            if (channel_id == channel)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Adding port %d channel %d is already used by logical port %d.\n",
                             logical_port, channel, logical_port_channel);
            }
        }
    }
    else
    {
        /** do nothing the interface is not used */
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_special_interface_offset_verify(
    int unit,
    dnx_algo_port_type_e port_type,
    int interface_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_type))
    {
        SHR_RANGE_VERIFY(interface_offset, -1, dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit) - 1,
                         _SHR_E_PARAM, "interface offset %d is not expected.\n", interface_offset);
    }
    else
    {
        SHR_RANGE_VERIFY(interface_offset, -1, -1,
                         _SHR_E_PARAM, "interface offset %d is not expected. (should be set to -1)\n",
                         interface_offset);

    }

exit:
    SHR_FUNC_EXIT;

}
