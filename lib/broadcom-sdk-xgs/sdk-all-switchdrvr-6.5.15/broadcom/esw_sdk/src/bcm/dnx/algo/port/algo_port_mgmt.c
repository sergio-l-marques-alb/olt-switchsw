/** \file algo_port_mgmt.c
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
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnxc/swstate/types/sw_state_res_mngr.h>

#include <bcm/types.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "algo_port_internal.h"

/*
 * }
 */
/*
 * Macros
 * {
 */

/*
 * }
 */

/*
 * Local functions (documentation in function implementation)
 * {
 */

static shr_error_e dnx_algo_port_tm_add(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_tm_add_t * info,
    int tm_interface_id);
/*
 * }
 */

/*
 * Module Init / Deinit  
 * { 
 */
/**
 * \brief - set default values for element in logical DB.
 */
static shr_error_e
dnx_algo_port_db_logical_init(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_db_logical_t logical_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&logical_db, 0, sizeof(dnx_algo_port_db_logical_t));
    logical_db.fabric_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.ilkn_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.nif_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.pp_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.pp_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.tm_interface_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.interface_handle = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_if_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_if_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_handle.h0 = DNX_ALGO_PORT_HANDLE_INVALID;
    logical_db.sch_handle.h1 = DNX_ALGO_PORT_HANDLE_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.set(unit, logical_port, &logical_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in fabric DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_fabric_init(
    int unit,
    int fabric_handle)
{
    dnx_algo_port_db_fabric_t fabric_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&fabric_db, 0, sizeof(dnx_algo_port_db_fabric_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.set(unit, fabric_handle, &fabric_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in nif DB. 
 *  This function is not called during init since the current default is 0.
 *  In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init() 
 */
static shr_error_e
dnx_algo_port_db_nif_init(
    int unit,
    int nif_handle)
{
    int rmc_index;
    dnx_algo_port_db_nif_t nif_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&nif_db, 0, sizeof(dnx_algo_port_db_nif_t));
    for (rmc_index = 0; rmc_index < BCM_PORT_MAX_NOF_PRIORITY_GROUPS; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_rmc_info_t_init(unit, &nif_db.logical_fifos[rmc_index]));
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.set(unit, nif_handle, &nif_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in ilkn DB. 
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init() 
 */
static shr_error_e
dnx_algo_port_db_ilkn_init(
    int unit,
    int ilkn_handle)
{
    dnx_algo_port_db_ilkn_t ilkn_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&ilkn_db, 0, sizeof(dnx_algo_port_db_ilkn_t));
    ilkn_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.set(unit, ilkn_handle, &ilkn_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in tm interface DB. 
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init() 
 */
static shr_error_e
dnx_algo_port_db_tm_interface_init(
    int unit,
    int tm_interface_handle)
{
    dnx_algo_port_db_tm_interface_t tm_interface_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tm_interface_db, 0, sizeof(dnx_algo_port_db_tm_interface_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.set(unit, tm_interface_handle, &tm_interface_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in interface DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_interface_init(
    int unit,
    int interface_handle)
{
    dnx_algo_port_db_interface_t interface_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&interface_db, 0, sizeof(dnx_algo_port_db_interface_t));
    interface_db.master_logical_port = DNX_ALGO_PORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.set(unit, interface_handle, &interface_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in scheduler DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_sch_if_init(
    int unit,
    dnx_algo_port_db_2d_handle_t sch_if_handle)
{
    dnx_algo_port_db_sch_if_t sch_if_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sch_if_db, 0, sizeof(dnx_algo_port_db_sch_if_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.set(unit, sch_if_handle.h0, sch_if_handle.h1, &sch_if_db));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in tm DB. 
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init() 
 */
static shr_error_e
dnx_algo_port_db_tm_init(
    int unit,
    dnx_algo_port_db_2d_handle_t tm_handle)
{
    dnx_algo_port_db_tm_t tm_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tm_db, 0, sizeof(dnx_algo_port_db_tm_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.set(unit, tm_handle.h0, tm_handle.h1, &tm_db));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set default values for element in pp DB. 
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init() 
 */
static shr_error_e
dnx_algo_port_db_pp_init(
    int unit,
    dnx_algo_port_db_2d_handle_t pp_handle)
{
    dnx_algo_port_db_pp_t pp_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&pp_db, 0, sizeof(dnx_algo_port_db_pp_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.set(unit, pp_handle.h0, pp_handle.h1, &pp_db));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp_alloc.free_single(unit, pp_handle.h0, pp_handle.h1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default values for element in scheduler DB.
 *          This function is not called during init since the current default is 0.
 *          In a case the default should be changed to be different from 0, call this function from dnx_algo_port_init()
 */
static shr_error_e
dnx_algo_port_db_sch_init(
    int unit,
    dnx_algo_port_db_2d_handle_t sch_handle)
{
    dnx_algo_port_db_sch_t sch_db;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&sch_db, 0, sizeof(dnx_algo_port_db_sch_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.set(unit, sch_handle.h0, sch_handle.h1, &sch_db));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_init(
    int unit)
{
    bcm_port_t logical_port;
    int nof_fabric_links;
    int nof_nif_phys;
    int nof_tm_ports, nof_pp_ports, nof_hrs, nof_sch_ifs;
    int nof_cores;
    int nof_ilkn_if;
    int nof_interfaces;
    int phy;
    int if_id;
    bcm_core_t core_id;
    sw_state_algo_res_create_data_t create_data;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_WARM_BOOT(unit))
    {
       /** restore soc info data */
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_restore(unit));
    }
    else
    {
        /*
         * Create SW State instance
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.init(unit));

        /*
         * Allocate SW STATE DBs  
         */
        {
            /*
             *  pp alloc init
             */
            sal_memset(&create_data, 0, sizeof(create_data));
            create_data.first_element = 0;
            create_data.nof_elements = dnx_data_port.general.nof_pp_ports_get(unit);
            sal_strncpy(create_data.name, "PP_PORT_ALLOC", SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp_alloc.alloc(unit, SW_STATE_ALGO_RES_NOF_CORES_GET(unit)));
            for (core_id = 0; core_id < SW_STATE_ALGO_RES_NOF_CORES_GET(unit); core_id++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db.pp_alloc.create(unit, core_id, &create_data, NULL));
            }

            /*
             *  fabric init
             *  allocate db for each fabric link
             */
            nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.alloc(unit, nof_fabric_links));

            /*
             * nif init 
             * allocate db for each nif phy
             * allocate entries for fabric phys for ILKN over fabric
             */
            nof_nif_phys = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.alloc(unit, nof_nif_phys));

            /*
             * ILKN init
             * allocate db for each ILKN id
             */
            nof_ilkn_if = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.alloc(unit, nof_ilkn_if));

            /*
             *  tm ports init
             *  allocate db for each core x tm port
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_tm_ports = dnx_data_port.general.nof_tm_ports_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.alloc(unit, nof_cores, nof_tm_ports));

            /*
             *  pp ports init
             *  allocate db for each core x pp port
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_pp_ports = dnx_data_port.general.nof_pp_ports_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.alloc(unit, nof_cores, nof_pp_ports));
            /*
             *  sch init
             *  allocate db for each core x HR
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_hrs = dnx_data_sch.flow.nof_hr_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.alloc(unit, nof_cores, nof_hrs));
            /*
             *  sch IF init
             *  allocate db for each core x HR
             */
            nof_cores = dnx_data_device.general.nof_cores_get(unit);
            nof_sch_ifs = dnx_data_sch.interface.nof_sch_interfaces_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.alloc(unit, nof_cores, nof_sch_ifs));

            /*
             * tm interface init 
             * allocate tm interface for each nif phy, special tm interfaces. 
             */
            nof_interfaces = DNX_ALGO_PORT_INTERFACE_HANDLE_NOF(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.alloc(unit, nof_interfaces));

            /*
             * Interface init
             * allocate tm interface for each nif phy, special tm interfaces.
             */
            nof_interfaces = DNX_ALGO_PORT_INTERFACE_HANDLE_NOF(unit);
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.alloc(unit, nof_interfaces));

        }

        if (!SOC_WARM_BOOT(unit))
        {
            /*
             * Set default values for DBs
             */
            /** logicals init - set handles to -1 */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db_logical_init(unit, logical_port));
            }
            /** NIF init - init the NIF info DB */
            for (phy = 0; phy < nof_nif_phys; ++phy)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db_nif_init(unit, phy));
            }
        }
        /** Interface init - init the NIF info DB */
        for (if_id = 0; if_id < nof_interfaces; ++if_id)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_init(unit, if_id));
        }
        /*
         * Init SOC INFO
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_init(unit));

    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_init(
    int unit,
    int *is_init)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.is_init(unit, (uint8 *) is_init));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Add / Remove Port
 * {
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_allocate(
    int unit,
    uint32 flags,
    bcm_core_t core,
    bcm_trunk_t lag_id,
    uint32 *pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    uint32 supported_flags;
    uint32 res_mngr_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    pp_handle.h0 = core;
    pp_handle.h1 = DNX_ALGO_PORT_INVALID;

    /*
     * Verify
     */
    /** verify flags */
    supported_flags = DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG | DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_WITH_ID;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "pp port allocate does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);
    /*
     * verify core 
     */
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    /** Find new free pp port */
    if (flags & DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_WITH_ID)
    {
        res_mngr_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_port_db.
                             pp_alloc.allocate_single(unit, core, res_mngr_flags, NULL, (int *) pp_port),
                             "Free PP port not found in core %d\n%s%s", core, EMPTY, EMPTY);
    pp_handle.h1 = *pp_port;

    /*
     * allocate the port
     */
    /** mark as valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.valid.set(unit, pp_handle.h0, pp_handle.h1, 1));
    /** Set pp port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.set(unit, pp_handle.h0, pp_handle.h1, *pp_port));
    /** Set core */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.core.set(unit, pp_handle.h0, pp_handle.h1, core));
    if (flags & DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG)
    {
        /** Set lag pp port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.set(unit, pp_handle.h0, pp_handle.h1, 1));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.lag_id.set(unit, pp_handle.h0, pp_handle.h1, lag_id));
    }

    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, core, *pp_port, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_free(
    int unit,
    bcm_core_t core,
    uint32 pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle, other_pp_handle;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    /** create handle */
    pp_handle.h0 = core;
    pp_handle.h1 = pp_port;

    /*
     * Verify
     */
    /** verify pp port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));

    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, core, pp_port, FALSE));

    /** Make sure that there are no logical ports with the same handle */
    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &other_pp_handle));

        /** Stop when found a free port */
        if (DNX_ALGO_PORT_2D_HANDLE_EQ(pp_handle, other_pp_handle))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "pp port used by logical port %d\n", logical_port);
        }
    }

   /** free resource */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_pp_init(unit, pp_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create and set general interface database
 */
static int
dnx_algo_port_db_interface_config_set(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    int interface_id)
{
    int valid_interface;
    SHR_FUNC_INIT_VARS(unit);

    /** Assign logical port to general interface database */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.set(unit, logical_port, interface_id));

    /*
     * If  interface_id is allocated -
     * assume that it's channelized interface with already configured interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.valid.get(unit, interface_id, &valid_interface));
    if (!valid_interface) /** i.e. master port */
    {
        /** set as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.valid.set(unit, interface_id, 1));
        /** Set interface core */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.set(unit, interface_id, core));
        /** set master logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.master_logical_port.set(unit, interface_id, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Add new NIF port
 * Relevant for both ethrnet port and interlaken ports.
 * (Configure NIF and ILKN data bases only)
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - required logical port.
 *   \param [in] nif_handle - nif DB handle
 *   \param [in] port_type - see dnx_algo_port_type_e
 *   \param [in] interface_offset - interface offset #
 *   \param [in] phys - bitmap of phy ports
 *   \param [in] phy_offset - offset for phys bitmap (used for ILKN over fabric)
 *   \param [in] ilkn_lanes - for ILKN only. bitmap of ILKN lanes (bits 0-23)
 *   \param [in] is_ilkn_over_fabric - for ILKN only. specify whether ILKN is over fabric.
 *
 *
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_nif_internal_add(
    int unit,
    bcm_port_t logical_port,
    int nif_handle,
    dnx_algo_port_type_e port_type,
    int interface_offset,
    bcm_pbmp_t phys,
    int phy_offset,
    bcm_pbmp_t ilkn_lanes,
    int is_ilkn_over_fabric)
{
    int valid_nif_port;
    int phy;
    int first_phy;
    SHR_FUNC_INIT_VARS(unit);

    /** set logical port as valid and set interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_ADDED));

    /** set port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.set(unit, logical_port, port_type));

    /** update ilkn pbmp for reg access */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        BCM_PBMP_PORT_ADD(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, logical_port);
    }

    /** Assign logical port to nif */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.set(unit, logical_port, nif_handle));

    /** Assign logical port to ilkn-id (just for ilkn of course) */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.set(unit, logical_port, interface_offset));
    }

    /*
     * If nif_handle_port is allocated -
     * assume that it's channelized interface with already configured NIF
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, nif_handle, &valid_nif_port));
    if (!valid_nif_port) /** i.e. master port */
    {
        /** set interface offset */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.set(unit, logical_port, interface_offset));
        /** set phys bitmap */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.set(unit, nif_handle, phys));
        /** set first phy */
        _SHR_PBMP_FIRST(phys, first_phy);
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.first_phy.set(unit, nif_handle, first_phy));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
        {
            /** set ILKN lanes **/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.lanes.set(unit, interface_offset, ilkn_lanes));

            /** set ILKN over fabric indication **/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_over_fabric.set(unit, interface_offset, is_ilkn_over_fabric));

            /** ILKN master and valid */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.valid.set(unit, interface_offset, 1));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.set(unit, interface_offset, logical_port));
        }

        /*
         * set valid and master logical port for all phys
         * Relevant fields for non master phys are: valid, master_logical_port.
         */
        BCM_PBMP_ITER(phys, phy)
        {
            /** set as valid */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.set(unit, phy + phy_offset, 1));

            /** set master port */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.set(unit, phy + phy_offset, logical_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_interface_handle_allocate(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type,
    bcm_pbmp_t phys,
    int interface_offset,
    int *tm_interface)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        *tm_interface = dnx_data_nif.phys.nof_phys_get(unit) + interface_offset;
    }
    else
    {
        _SHR_PBMP_FIRST(phys, *tm_interface);
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phy_active_get(
    int unit,
    int logical_phy,
    int *is_active)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, logical_phy, is_active));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_if_add_t * info)
{
    int first_phy_port;
    int phy_offset = 0;
    int interface_id, tm_interface_id;
    int is_master_port;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** verify logical port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, logical_port));

    /** add verify  */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_type_get
                    (unit, info->interface, info->ilkn_info.is_elk_if, info->eth_info.is_stif, &port_type));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_add_verify(unit, logical_port, port_type, info));

    _SHR_PBMP_FIRST(info->phys, first_phy_port);

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        if (info->ilkn_info.is_ilkn_over_fabric)
        {
            phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
            /** Add fabric links offset for NIF DB handle */
            first_phy_port += phy_offset;
        }
    }

    /** Verify channels just for TM ports */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        /** tm port Verify */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_free_verify(unit, port_type, info->core, info->tm_info.tm_port));

        SHR_IF_ERR_EXIT(dnx_algo_port_tm_interface_handle_allocate
                        (unit, logical_port, port_type, info->phys, info->interface_offset, &tm_interface_id));

        /** channel verify */
        if (info->tm_info.is_channelized)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_free_verify
                            (unit, logical_port, tm_interface_id, info->tm_info.channel));
        }
        if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type))
        {
            /** Verify HR */
            SHR_IF_ERR_EXIT(dnx_algo_port_hr_free_verify
                            (unit, info->core, info->tm_info.base_hr, info->tm_info.sch_priorities));
        }
    }

    /*
     * Add to data base
     */
    /** Add general interface DataBase */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_interface_handle_allocate
                    (unit, logical_port, port_type, info->phys, info->interface_offset, &interface_id));

    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_config_set(unit, logical_port, info->core, interface_id));
    /** NIF PORT*/
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_internal_add
                    (unit, logical_port, first_phy_port, port_type, info->interface_offset, info->phys,
                     phy_offset, info->ilkn_info.ilkn_lanes, info->ilkn_info.is_ilkn_over_fabric));

    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {
        /** TM PORT */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_add(unit, logical_port, info->core, &info->tm_info, interface_id));
    }

    /*
     * Set SOC INFO.
     * Until TDM mode will be set assume the port is not TDM.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add
                    (unit, logical_port, is_master_port, port_type, info->phys, DNX_ALGO_PORT_TDM_MODE_NONE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - 
 * Add new TM port
 * Relevant for any port type which has tm port (nif, cpu, ..)
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port.
 *   \param [in] core - core ID.
 *   \param [in] info - Switch port info.
 *   \param [in] interface_id - See macros DNX_ALGO_PORT_TM_INTERFACE_*
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port data base
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_tm_add(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    dnx_algo_port_tm_add_t * info,
    int interface_id)
{
    int valid_tm_interface;
    uint32 pp_port = DNX_ALGO_PORT_TYPE_INVALID;
    dnx_algo_port_db_2d_handle_t tm_handle, pp_handle, sch_handle;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));

    /** Assign logical port to tm port */
    tm_handle.h0 = core;
    tm_handle.h1 = info->tm_port;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.set(unit, logical_port, &tm_handle));
    /** Assign logical port to sch */
    sch_handle.h0 = core;
    sch_handle.h1 = info->base_hr;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.set(unit, logical_port, &sch_handle));
    /** Assign logical port to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.set(unit, logical_port, interface_id));

    /*
     * Configure TM port
     */
    /** mark as valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.valid.set(unit, tm_handle.h0, tm_handle.h1, 1));
    /** Set tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tm_port.set(unit, tm_handle.h0, tm_handle.h1, info->tm_port));
    /** Set core */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.core.set(unit, tm_handle.h0, tm_handle.h1, core));
    /** Set channel_id */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.channel.set(unit, tm_handle.h0, tm_handle.h1, info->channel));
    /** set master port (of tm port!)*/
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.set(unit, tm_handle.h0, tm_handle.h1, logical_port));

    /** Egress queuing port only */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        /** Set base_q_pair */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.base_q_pair.set(unit, tm_handle.h0, tm_handle.h1, info->base_q_pair));
        /** Set number of port priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.priorities.set(unit, tm_handle.h0, tm_handle.h1, info->num_priorities));

    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.
                        base_q_pair.set(unit, tm_handle.h0, tm_handle.h1, DNX_ALGO_PORT_TYPE_INVALID));
    }

    /*
     * If tm_interface_id is allocated -
     * assume that it's channelized interface with already configured interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.valid.get(unit, interface_id, &valid_tm_interface));
    if (!valid_tm_interface) /** i.e. master port */
    {
        /** set as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.valid.set(unit, interface_id, 1));
        /** Set is channelized */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_channelized.set(unit, interface_id, info->is_channelized));
        /** Set egress interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.egr_if.set(unit, interface_id, info->egress_interface));
        /** set master logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.master_logical_port.set(unit, interface_id, logical_port));
        /** set tdm master port to be invalid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_tdm_logical_port.set(unit, interface_id, DNX_ALGO_PORT_INVALID));
        /** set non-tdm master port to be this port - assuming it's non tdm port for now */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_non_tdm_logical_port.set(unit, interface_id, logical_port));
    }

    /*
     * Configure SCH
     */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type))
    {
        /** mark as valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.valid.set(unit, sch_handle.h0, sch_handle.h1, 1));
        /** Set base_hr */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.base_hr.set(unit, sch_handle.h0, sch_handle.h1, info->base_hr));
        /** Set number of sch priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.set(unit, sch_handle.h0, sch_handle.h1,
                                                                info->sch_priorities));
        /** set master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.master_logical_port.set(unit, sch_handle.h0, sch_handle.h1, logical_port));
    }

    /*
     * Configure PP port
     */
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type))
    {
        /*
         * Allocate new pp port
         */
        
#if FIXME_PP_PORT
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, 0, info->core, &pp_port));
#else
        pp_port = info->tm_port;
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate
                        (unit, DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_WITH_ID, core, 0, &pp_port));
#endif

        /** Assign logical port to pp port */
        pp_handle.h0 = core;
        pp_handle.h1 = pp_port;
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.set(unit, logical_port, &pp_handle));
        /** set master port (of pp port!)*/
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.set(unit, pp_handle.h0, pp_handle.h1, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_algo_port_db_interface_id_get(
    int unit,
    dnx_algo_port_type_e port_type,
    bcm_core_t core,
    int interface_offset,
    int *interface_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get tm interface id and port type
     */
    switch (port_type)
    {
        case DNX_ALGO_PORT_TYPE_CPU:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_CPU(unit, core);
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_RCY(unit, core, interface_offset);
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_RCY_MIRROR(unit, core);
            break;
        }
        case DNX_ALGO_PORT_TYPE_ERP:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_ERP(unit, core);
            break;
        }
        case DNX_ALGO_PORT_TYPE_OLP:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_OLP(unit, core);
            break;
        }
        case DNX_ALGO_PORT_TYPE_OAMP:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_OAMP(unit, core);
            break;
        }
        case DNX_ALGO_PORT_TYPE_SAT:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_SAT(unit, core);
            break;
        }
        case DNX_ALGO_PORT_TYPE_EVENTOR:
        {
            *interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_EVENTOR(unit, core);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported interface type for port\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - input verification for special interface adding
 * 
 */
static shr_error_e
dnx_algo_port_special_interface_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_special_interface_add_t * info)
{
    bcm_core_t core;
    int cpu_interface_id, rcy_interface_id;
    int interface_offset;
    int interface_id;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get tm interface id and port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_type_get(unit, info->interface, 0, 0, &port_type));
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                    (unit, port_type, info->core, info->interface_offset, &interface_id));

    /*
     * Verify
     */
    /** Verify logical port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, logical_port));
    /** Core verify*/
    DNXCMN_CORE_VALIDATE(unit, info->core, 0);
    /** Verify tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_free_verify(unit, port_type, info->core, info->tm_info.tm_port));
    /** Verify interface offset */
    SHR_IF_ERR_EXIT(dnx_algo_port_special_interface_offset_verify(unit, port_type, info->interface_offset));
    /** Verify channel */
    if (info->tm_info.is_channelized)
    {
        if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
        {
            /** special check for cpu - make sure that the channel is not used on all cores */
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                cpu_interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_CPU(unit, core);
                SHR_IF_ERR_EXIT(dnx_algo_port_channel_free_verify
                                (unit, logical_port, cpu_interface_id, info->tm_info.channel));
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_type))
        {
            /** special check for recycle - make sure that the channel is not used on all interface offsets */
            for (interface_offset = 0;
                 interface_offset < dnx_data_ingr_reassembly.priority.rcy_priorities_nof_get(unit); interface_offset++)
            {
                rcy_interface_id = DNX_ALGO_PORT_INTERFACE_HANDLE_FOR_RCY(unit, info->core, interface_offset);
                SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_port_channel_free_verify
                                         (unit, logical_port, rcy_interface_id, info->tm_info.channel),
                                         "Port %d channel %d is already used by one of the RCY interfaces. (RCY interfaces shared the channel ids).\n%s",
                                         logical_port, info->tm_info.channel, EMPTY);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_free_verify(unit, logical_port, interface_id, info->tm_info.channel));
        }
    }
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type))
    {
        /** Verify HR */
        SHR_IF_ERR_EXIT(dnx_algo_port_hr_free_verify
                        (unit, info->core, info->tm_info.base_hr, info->tm_info.sch_priorities));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_special_interface_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_special_interface_add_t * info)
{
    int interface_id;
    dnx_algo_port_type_e port_type;
    bcm_pbmp_t dummy_pbmp;
    int is_master_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_algo_port_special_interface_add_verify(unit, logical_port, info));

    /*
     * Get interface id and port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_type_get(unit, info->interface, 0, 0, &port_type));;
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_id_get
                    (unit, port_type, info->core, info->interface_offset, &interface_id));

    /*
     * Add to data base
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_ADDED));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.set(unit, logical_port, port_type));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.set(unit, logical_port, info->interface_offset));

    /** Add general interface Data Base */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_config_set(unit, logical_port, info->core, interface_id));
    /** add tm info */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_add(unit, logical_port, info->core, &info->tm_info, interface_id));

    /*
     *  Set SOC INFO
     */
    BCM_PBMP_CLEAR(dummy_pbmp);
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add
                    (unit, logical_port, is_master_port, port_type, dummy_pbmp, DNX_ALGO_PORT_TDM_MODE_NONE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_and_channel_to_logical_get(
    int unit,
    int core_id,
    dnx_algo_port_type_e port_type,
    int channel_id,
    bcm_port_t * logical_port)
{
    dnx_algo_port_type_e port_type_i;
    bcm_port_t logical_port_i;
    bcm_pbmp_t logical_ports;
    int channel_i;
    bcm_core_t core_i;
    SHR_FUNC_INIT_VARS(unit);

    *logical_port = DNX_ALGO_PORT_INVALID;

    /*
     * iterate over valid logical ports bitmap and compare channel and type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0,
                                               &logical_ports));
    BCM_PBMP_ITER(logical_ports, logical_port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port_i, &port_type_i));
        if (port_type_i == port_type)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port_i, &core_i));
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port_i, &channel_i));
            if ((core_id == core_i) && (channel_id == channel_i))
            {
                *logical_port = logical_port_i;
                break;
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
dnx_algo_port_fabric_phy_active_get(
    int unit,
    int fabric_phy,
    int *is_active)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.valid.get(unit, fabric_phy, is_active));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_add(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_fabric_add_t * info)
{
    bcm_pbmp_t dummy_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is free */
    SHR_IF_ERR_EXIT(dnx_algo_port_free_verify(unit, logical_port));

    /** Verify that fabric link id is supported by device*/
    /** TBD SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_supported_verify(unit, info->interface, info->interface_offset)); */

    /*
     * Add to data base
     */
    /** Logical DB - port type and interface type*/
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_ADDED));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.set(unit, logical_port, DNX_ALGO_PORT_TYPE_FABRIC));

    /** Fabric data base (handle, mark as valid and link id */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.set(unit, logical_port, info->link_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.valid.set(unit, info->link_id, 1));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.link.set(unit, info->link_id, info->link_id));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.master_logical_port.set(unit, info->link_id, logical_port));

    /*
     *  Set SOC INFO
     */
    BCM_PBMP_CLEAR(dummy_pbmp);
    /** Set logical_port in bitmap, even though we currently don't use it */
    BCM_PBMP_PORT_ADD(dummy_pbmp, logical_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add
                    (unit, logical_port, 1, DNX_ALGO_PORT_TYPE_FABRIC, dummy_pbmp, DNX_ALGO_PORT_TDM_MODE_NONE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_remove(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_state_e state;
    dnx_algo_port_type_e port_type;
    int is_master_port;
    int nif_handle;
    int ilkn_handle;
    int fabric_handle;
    dnx_algo_port_db_2d_handle_t tm_handle, pp_handle, sch_handle, sch_if_handle;
    int tm_interface_handle, interface_handle;
    bcm_port_t next_master_port = DNX_ALGO_PORT_INVALID;
    bcm_pbmp_t phy_ports;
    int is_lag;
    int phy;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));

    /*
     * If port is valid (i.e. removing existing port)
     * Remove non-logical data base in case it's the master port  
     */
    if (state != DNX_ALGO_PORT_STATE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));
        /*
         * if it is tm port 
         * first set tdm mode to be invalid, which will update tdm and non-tdm master ports 
         */
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_set(unit, logical_port, DNX_ALGO_PORT_TDM_MODE_INVALID));
        }

        /*
         * Remove port from SOC INFO 
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
        BCM_PBMP_CLEAR(phy_ports);
        if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phy_ports));
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_remove(unit, logical_port, port_type, next_master_port, phy_ports));

        /*
         * Release data base which are one per logical ports
         */
        /** Fabric Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        if (fabric_handle != DNX_ALGO_PORT_HANDLE_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_fabric_init(unit, fabric_handle));
        }

        /** Release TM port Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(tm_handle))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_tm_init(unit, tm_handle));
        }

        /** Release PP port Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(pp_handle))
        {
            /** release just in case pp ported wasn't allocated for LAG */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
            if (!is_lag)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_db_pp_init(unit, pp_handle));
            }
        }

        /** Release SCH Data Base */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(sch_handle))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db_sch_init(unit, sch_handle));
        }

        /*
         * For data bases which might be relevant more then one logical port. 
         * Release it only it there is no other master port.
         * If it master port and there are additional ports - just reset the pointers to the new master port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
        if (is_master_port)
        {
            /** get next master */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                            (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));

            /** if no next master - release all data bases */
            if (next_master_port == DNX_ALGO_PORT_INVALID)
            {
                /** Release NIF Data Base - all related phys */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
                if (nif_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, &phy_ports));
                    BCM_PBMP_ITER(phy_ports, phy)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_port_db_nif_init(unit, phy));
                    }
                }

                /** Release ILKN Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
                if (ilkn_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_ilkn_init(unit, ilkn_handle));
                }

                /** Release TM interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                                tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
                if (tm_interface_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_tm_interface_init(unit, tm_interface_handle));
                }

                /** Release interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
                if (interface_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_interface_init(unit, interface_handle));
                }

                /** Release scheduler interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_if_handle));
                if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(sch_if_handle))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db_sch_if_init(unit, sch_if_handle));
                }

            }
            else
            {
                /** update next master port */
                /** NIF Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
                if (nif_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, &phy_ports));
                    BCM_PBMP_ITER(phy_ports, phy)
                    {
                        /** set master port */
                        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.set(unit, phy, next_master_port));
                    }

                }

                /** ILKN Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
                if (ilkn_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.set(unit, ilkn_handle, next_master_port));
                }

                /** TM interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                                tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
                if (tm_interface_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_logical_port.set(unit, tm_interface_handle, next_master_port));
                }

                /** Interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                                interface_handle.get(unit, logical_port, &tm_interface_handle));
                if (interface_handle != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    master_logical_port.set(unit, tm_interface_handle, next_master_port));
                }

                /** Scheduler interface Data Base */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_if_handle));
                if (sch_if_handle.h0 != DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                                    master_logical_port.set(unit, sch_if_handle.h0, sch_if_handle.h1,
                                                            next_master_port));
                }
            }
        }
    }

    /*
     * Init logical DB
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db_logical_init(unit, logical_port));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_remove_process_start(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Mark as port under remove process
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_REMOVED));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_removed_port_get(
    int unit,
    bcm_port_t * logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over all ports and return the first port in removed state. 
     * (Assuming just one port is in this state) 
     */
    for ((*logical_port) = 0; (*logical_port) < SOC_MAX_NUM_PORTS; (*logical_port)++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, *logical_port, &state));
        if (state == DNX_ALGO_PORT_STATE_REMOVED)
        {
            break;
        }
    }

    /** if not found throw an error */
    if (*logical_port == SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "removed port wasn't found");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_added_port_get(
    int unit,
    bcm_port_t * logical_port)
{
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterate over all ports and return the first port in added state. 
     * (Assuming just one port is in this state) 
     */
    for ((*logical_port) = 0; (*logical_port) < SOC_MAX_NUM_PORTS; (*logical_port)++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, *logical_port, &state));
        if (state == DNX_ALGO_PORT_STATE_ADDED)
        {
            break;
        }
    }

    /** if not found throw an error */
    if (*logical_port == SOC_MAX_NUM_PORTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "added port wasn't found");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_add_process_done(
    int unit,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify (i.e. make sure the port is not free)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Mark as valid port - added process completed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.set(unit, logical_port, DNX_ALGO_PORT_STATE_VALID));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * Mapping from / to logical port 
 * { 
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_port_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core,
    uint32 *tm_port)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /** Get core and tm_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.core.get(unit, tm_handle.h0, tm_handle.h1, core));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tm_port.get(unit, tm_handle.h0, tm_handle.h1, tm_port));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_core_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core)
{
    int interface_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to interface DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(interface_handle);

    /** Get core and tm_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, interface_handle, core));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_in_lag(
    int unit,
    bcm_port_t logical_port,
    int *port_in_lag)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    SHR_FUNC_INIT_VARS(unit);

    *port_in_lag = 0;

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to PP DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
    if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(pp_handle))
    {
        /** set port_in_lag if port is part of a lag */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, port_in_lag));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_get(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t * core,
    uint32 *pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    int is_lag;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to PP DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(pp_handle);

    /** Don't allow getting pp port for LAG member */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, pp_handle.h0, pp_handle.h1, &is_lag));
    if (is_lag)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "getting pp port for a LAG member %d is not allowed.\n"
                     "Use LAG gport to get the required pp port\n", logical_port);
    }

    /** Get core and pp_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.core.get(unit, pp_handle.h0, pp_handle.h1, core));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.get(unit, pp_handle.h0, pp_handle.h1, pp_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_port_modify(
    int unit,
    bcm_port_t logical_port,
    bcm_core_t core,
    uint32 pp_port)
{
    dnx_algo_port_db_2d_handle_t pp_handle;
    uint32 tm_port;
    bcm_core_t tm_core;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** Verify pp port */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));
    /** Verify that tm core equals to pp core */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &tm_core, &tm_port));
    if (tm_core != core)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "logical port %d core must not changed dynamically\n", core);
    }

    /*
     * Set handle to PP DB
     */
    pp_handle.h0 = core;
    pp_handle.h1 = pp_port;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.set(unit, logical_port, &pp_handle));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_link_get(
    int unit,
    bcm_port_t logical_port,
    int *fabric_link)
{
    int fabric_handle;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to FABRIC DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

    /** Get fabric link */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.link.get(unit, fabric_handle, fabric_link));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_prbs_mode_set(
    int unit,
    bcm_port_t logical_port,
    portmod_prbs_mode_t prbs_mode)
{
    int fabric_handle = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to FABRIC DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));

    /*
     * For non Fabric ports, prbs mode is always portmodPrbsModePhy.
     * For Fabric ports, prbs mode can be either portmodPrbsModePhy or portmodPrbsModeMac.
     */
    if (fabric_handle == DNX_ALGO_PORT_HANDLE_INVALID)
    {
        /** Verify prbs mode */
        SHR_VAL_VERIFY(prbs_mode, portmodPrbsModePhy, _SHR_E_PARAM, "PRBS mode must be PHY for this port");
    }
    else
    {
        /** Verify prbs mode */
        SHR_RANGE_VERIFY(prbs_mode, portmodPrbsModePhy, portmodPrbsModeMac, _SHR_E_PARAM,
                         "prbs mode %d is not supported", prbs_mode);

        /** Set PRBS mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.prbs_mode.set(unit, fabric_handle, prbs_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_prbs_mode_get(
    int unit,
    bcm_port_t logical_port,
    portmod_prbs_mode_t * prbs_mode)
{
    int fabric_handle = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to FABRIC DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));

    /*
     * For non Fabric ports, prbs mode is always portmodPrbsModePhy.
     * For Fabric ports, prbs mode can be either portmodPrbsModePhy or portmodPrbsModeMac.
     */
    if (fabric_handle == DNX_ALGO_PORT_HANDLE_INVALID)
    {
        *prbs_mode = portmodPrbsModePhy;
    }
    else
    {
        /** Get PRBS mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.prbs_mode.get(unit, fabric_handle, (int *) prbs_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 tm_port,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_valid_verify(unit, core, tm_port));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.master_logical_port.get(unit, core, tm_port, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_to_logical_get(
    int unit,
    bcm_core_t core,
    uint32 pp_port,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify pp port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.get(unit, core, pp_port, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_fabric_link_to_logical_get(
    int unit,
    int fabric_link,
    bcm_port_t * logical_port)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify fabric link valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_valid_verify(unit, fabric_link));

    /** Get logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.master_logical_port.get(unit, fabric_link, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_to_gport_get(
    int unit,
    bcm_core_t core,
    uint32 pp_port,
    bcm_gport_t * gport)
{

    int is_lag = 0;
    bcm_trunk_t lag_id;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify pp port valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_valid_verify(unit, core, pp_port));

    /** Check if PP port was allocated for LAG */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.is_lag.get(unit, core, pp_port, &is_lag));
    if (is_lag)
    {
        /** Get LAG ID*/
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.lag_id.get(unit, core, pp_port, &lag_id));
        BCM_GPORT_TRUNK_SET(*gport, lag_id);
    }
    else
    {
        /** Get logical port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.master_logical_port.get(unit, core, pp_port, &logical_port));
        *gport = logical_port;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Set / Get  APIs 
 * { 
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_master_get(
    int unit,
    bcm_port_t logical_port,
    int *is_master_port)
{
    bcm_port_t master_port;
    SHR_FUNC_INIT_VARS(unit);

    /** get master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));

    /** return true iff master port is logical port */
    *is_master_port = (logical_port == master_port);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_master_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_port_t * master_logical_port)
{
    int tm_interface_handle;
    uint32 supported_flags;
    bcm_port_t logical_port_i;
    dnx_algo_port_type_e port_type;
    bcm_port_t cur_master_logical_port;
    bcm_pbmp_t logical_port_channels;
    SHR_FUNC_INIT_VARS(unit);

    /** Init default */
    *master_logical_port = DNX_ALGO_PORT_INVALID;

    /*
     * Verify
     */
    /** Verify that the required flags are supported */
    supported_flags = DNX_ALGO_PORT_MASTER_F_MASK;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "get master flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * get port type - different behavior per port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));
    switch (port_type)
    {
        /** ELK port shouldn't have channels */
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
        /** Every logical port is the master port*/
        case DNX_ALGO_PORT_TYPE_FABRIC:
        {

            if (flags & DNX_ALGO_PORT_MASTER_F_NEXT)
            { /** no next master*/
                *master_logical_port = DNX_ALGO_PORT_INVALID;
            }
            else
            { /** the logical port is the master port */
                *master_logical_port = logical_port;
            }
            break;
        }
        default:
        {
            /** get current master port */
            /** Get handle to tm interface */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
            DNX_ALGO_PORT_HANDLE_VERIFY(tm_interface_handle);

            /** iterate the port to find the requested port */
            if (flags & DNX_ALGO_PORT_MASTER_F_NEXT)
            {
                if (flags & DNX_ALGO_PORT_MASTER_F_TDM)
                {
                    /*
                     * TDM master port
                     */
                    /** get channels ports bitmap */
                    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                                    (unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_TDM_ONLY, &logical_port_channels));
                    /** get current master port */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_tdm_logical_port.get(unit, tm_interface_handle, &cur_master_logical_port));
                    BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                    {
                        /** filter master port */
                        if (logical_port_i == cur_master_logical_port)
                        {
                            continue;
                        }
                        *master_logical_port = logical_port_i;
                        break;
                    }
                }
                else if (flags & DNX_ALGO_PORT_MASTER_F_NON_TDM)
                {
                    /*
                     * NON TDM master port
                     */
                    /** get channels ports bitmap */
                    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get
                                    (unit, logical_port, DNX_ALGO_PORT_CHANNELS_F_NON_TDM_ONLY,
                                     &logical_port_channels));
                    /** get current master port */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_non_tdm_logical_port.get(unit, tm_interface_handle,
                                                                    &cur_master_logical_port));
                    BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                    {
                        /** filter master port */
                        if (logical_port_i == cur_master_logical_port)
                        {
                            continue;
                        }
                        *master_logical_port = logical_port_i;
                        break;
                    }
                }
                else
                {
                    /*
                     * Regular master port
                     */
                    /** get channels ports bitmap */
                    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, 0, &logical_port_channels));
                    /** get current master port */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_logical_port.get(unit, tm_interface_handle, &cur_master_logical_port));
                    BCM_PBMP_ITER(logical_port_channels, logical_port_i)
                    {
                        /** filter master port */
                        if (logical_port_i == cur_master_logical_port)
                        {
                            continue;
                        }
                        *master_logical_port = logical_port_i;
                        break;
                    }
                }
            }
            else
            {   /** current master port */

                /** read master, tdm master, non-tdm master */
                if (flags & DNX_ALGO_PORT_MASTER_F_TDM)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_tdm_logical_port.get(unit, tm_interface_handle, master_logical_port));
                }
                else if (flags & DNX_ALGO_PORT_MASTER_F_NON_TDM)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_non_tdm_logical_port.get(unit, tm_interface_handle, master_logical_port));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                                    master_logical_port.get(unit, tm_interface_handle, master_logical_port));
                }
            }
            break;
        }

    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_if_id_to_master_get(
    int unit,
    int interface_id,
    int *master_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.master_logical_port.get(unit, interface_id, master_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e * port_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port exist */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, port_type));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_offset_get(
    int unit,
    bcm_port_t logical_port,
    int *interface_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port exist */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_offset.get(unit, logical_port, interface_offset));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channel_set(
    int unit,
    bcm_port_t logical_port,
    int channel_id)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);
    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.channel.set(unit, tm_handle.h0, tm_handle.h1, channel_id));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channel_get(
    int unit,
    bcm_port_t logical_port,
    int *channel_id)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    *channel_id = 0;

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
    {

        /** Get handle to TM DB */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
        DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

        /*
         * Get data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.channel.get(unit, tm_handle.h0, tm_handle.h1, channel_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_app_flags_set(
    int unit,
    bcm_port_t logical_port,
    uint32 app_flags)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);
    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.port_app_flags.set(unit, tm_handle.h0, tm_handle.h1, app_flags));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_app_flags_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *app_flags)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    *app_flags = 0;

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.port_app_flags.get(unit, tm_handle.h0, tm_handle.h1, app_flags));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_base_q_pair_get(
    int unit,
    bcm_port_t logical_port,
    int *base_q_pair)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.base_q_pair.get(unit, tm_handle.h0, tm_handle.h1, base_q_pair));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_q_pair_port_cosq_get(
    int unit,
    int q_pair,
    bcm_core_t core,
    bcm_port_t * logical_port,
    bcm_cos_queue_t * cosq)
{
    bcm_port_t port;
    bcm_pbmp_t port_bm;
    int num_priorities, base_q_pair;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF, &port_bm));
    BCM_PBMP_ITER(port_bm, port)
    {
        /*
         * Get the Q-pair
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
        /*
         * Get number of priorities for current port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
        /*
         * If q_pair is in the range of Q-pairs for this port, return the information and break the loop
         */
        if ((q_pair >= base_q_pair) && (q_pair < (base_q_pair + num_priorities)))
        {
            *logical_port = port;
            *cosq = q_pair - base_q_pair;
            /** If Port and COSQ are found go directly to "exit" and skip NOT_FOUND error */
            SHR_EXIT();
        }
    }
    /** If we reach this point it means that Port and COSQ were not found */
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Port and COSQ for specified Q-pair not found.");
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_base_hr_get(
    int unit,
    bcm_port_t logical_port,
    int *base_hr)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.base_hr.get(unit, sch_handle.h0, sch_handle.h1, base_hr));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_hr_get(
    int unit,
    bcm_port_t logical_port,
    int sch_priority,
    int *hr)
{
    int nof_priorities, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    /** verify valid scheduler priority is given */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (sch_priority > nof_priorities)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given Schedule priority (%d) is bigger than number of scheduler priorities (%d)",
                     sch_priority, nof_priorities);
    }

    /** get base hr */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** the returned HR ID is computed by (base_hr + sch_priority) */
    *hr = base_hr + sch_priority;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_priorities_nof_set(
    int unit,
    bcm_port_t logical_port,
    int num_priorities)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.priorities.set(unit, tm_handle.h0, tm_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_priorities_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *num_priorities)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.priorities.get(unit, tm_handle.h0, tm_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_priorities_nof_set(
    int unit,
    bcm_port_t logical_port,
    int num_priorities)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_handle);

    /*
     * Set data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.set(unit, sch_handle.h0, sch_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_priorities_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *num_priorities)
{
    dnx_algo_port_db_2d_handle_t sch_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_handle.get(unit, logical_port, &sch_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch.sch_priorities.get(unit, sch_handle.h0, sch_handle.h1, num_priorities));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_is_channelized_get(
    int unit,
    bcm_port_t logical_port,
    int *is_channelized)
{
    int tm_interface_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to tm interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(tm_interface_handle);

    /*
     * Get data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.is_channelized.get(unit, tm_interface_handle, is_channelized));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_speed_set(
    int unit,
    bcm_port_t logical_port,
    int speed)
{
    int interface_handle;
    dnx_algo_port_type_e port_type;
    int fabric_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type))
    {
        /** Get handle to fabric link */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /*
         * Set data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.speed.set(unit, fabric_handle, speed));
    }
    else
    {

        /** Get handle to tm interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(interface_handle);

        /*
         * Set data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.speed.set(unit, interface_handle, speed));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * internal implementation of speed_get
 */
static shr_error_e
dnx_algo_port_speed_get_impl(
    int unit,
    bcm_port_t logical_port,
    int *speed)
{
    int interface_handle;
    dnx_algo_port_type_e port_type;
    int fabric_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type))
    {
        /** Get handle to fabric link */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /*
         * Set data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.speed.get(unit, fabric_handle, speed));

    }
    else
    {

        /** Get handle to tm interface */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &interface_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(interface_handle);

        /*
         * Get data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.speed.get(unit, interface_handle, speed));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_has_speed(
    int unit,
    bcm_port_t logical_port,
    int *has_speed)
{
    int speed;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get_impl(unit, logical_port, &speed));

    *has_speed = (speed != 0);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_speed_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *speed)
{
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify that the required flags are supported */
    supported_flags = DNX_ALGO_PORT_SPEED_F_MBPS | DNX_ALGO_PORT_SPEED_F_KBPS;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "get speed flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);
    /*
     * Get speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get_impl(unit, logical_port, speed));

    if (*speed == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "speed wasn't set for port %d.\n", logical_port);
    }

    if (flags & DNX_ALGO_PORT_SPEED_F_KBPS)
    {
        *speed = *speed * 1000;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phys_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * phys)
{
    int nif_handle;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.phys.get(unit, nif_handle, phys));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_first_phy_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *first_phy_port)
{
    int nif_handle;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.first_phy.get(unit, nif_handle, first_phy_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_lanes_get(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t * ilkn_lanes)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.lanes.get(unit, ilkn_handle, ilkn_lanes));

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_nof_segments_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_segments)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.nof_segments.get(unit, ilkn_handle, nof_segments));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_nof_segments_set(
    int unit,
    bcm_port_t logical_port,
    int nof_segments)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.nof_segments.set(unit, ilkn_handle, nof_segments));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_is_over_fabric_get(
    int unit,
    bcm_port_t logical_port,
    int *is_over_fabric)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_over_fabric.get(unit, ilkn_handle, is_over_fabric));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_ilkn_is_over_fabric_set(
    int unit,
    bcm_port_t logical_port,
    int is_over_fabric)
{
    int ilkn_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &ilkn_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(ilkn_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.is_over_fabric.set(unit, ilkn_handle, is_over_fabric));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_latch_down_get(
    int unit,
    bcm_port_t logical_port,
    int *is_latch_down)
{
    int nif_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.latch_down.get(unit, nif_handle, is_latch_down));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_latch_down_set(
    int unit,
    bcm_port_t logical_port,
    int is_latch_down)
{
    int nif_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Set data
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** set data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.latch_down.set(unit, nif_handle, is_latch_down));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_loopback_original_link_training_get(
    int unit,
    bcm_port_t logical_port,
    int *link_training_enable)
{
    int fabric_handle;
    int nif_handle;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    if (port_type == DNX_ALGO_PORT_TYPE_FABRIC)
    {
        /*
         * Get data
         */
        /** get fabric handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.
                        loopback_original_link_training.get(unit, fabric_handle, link_training_enable));
    }
    else
    {
        /*
         * Get data
         */
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** get data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                        loopback_original_link_training.get(unit, nif_handle, link_training_enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_loopback_original_link_training_set(
    int unit,
    bcm_port_t logical_port,
    int link_training_enable)
{
    int fabric_handle;
    int nif_handle;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    if (port_type == DNX_ALGO_PORT_TYPE_FABRIC)
    {
        /*
         * Set data
         */
        /** get fabric handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &fabric_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(fabric_handle);

        /** set data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.fabric.
                        loopback_original_link_training.set(unit, fabric_handle, link_training_enable));
    }
    else
    {
        /*
         * Set data
         */
        /** get nif handle */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
        DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

        /** set data */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                        loopback_original_link_training.set(unit, nif_handle, link_training_enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phy_to_logical_get(
    int unit,
    int phy,
    int is_over_fabric,
    uint32 flags,
    bcm_port_t * logical_port)
{
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_phy_verify(unit, phy));
        phy += dnx_data_port.general.fabric_phys_offset_get(unit);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_verify(unit, phy));
    }

    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, phy, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_logical_fifo_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int logical_fifo_index,
    dnx_algo_port_rmc_info_t * logical_fifo_info)
{
    int nif_handle;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.logical_fifos.get(unit, nif_handle, logical_fifo_index, logical_fifo_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_logical_fifo_set(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int logical_fifo_index,
    dnx_algo_port_rmc_info_t * logical_fifo_info)
{
    int nif_handle;
    uint32 supported_flags;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    /** verify flags */
    supported_flags = 0;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "function flags does not support the requested flags 0x%x %s%s.\n", flags, EMPTY, EMPTY);

    /*
     * Get data 
     */
    /** get nif handle */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &nif_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(nif_handle);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.logical_fifos.set(unit, nif_handle, logical_fifo_index, *logical_fifo_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_rmc_info_t_init(
    int unit,
    dnx_algo_port_rmc_info_t * rmc_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize rmc info to invalid values
     */
    sal_memset(rmc_info, 0, sizeof(dnx_algo_port_rmc_info_t));
    rmc_info->rmc_id = -1;
    rmc_info->sch_priority = bcmPortNifSchedulerLow;
    rmc_info->prd_priority = -1;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_logical_fifo_pbmp_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_fifo_pbmp)
{
    int ii, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*logical_fifo_pbmp);

    nof_priority_groups = dnx_data_nif.eth.priority_groups_nof_get(unit);
    /*
     * iterate over all logical fifos of the port
     */
    for (ii = 0; ii < nof_priority_groups; ++ii)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, logical_port, 0, ii, &rmc));
        if (rmc.rmc_id != DNX_ALGO_PORT_INVALID)
        {
            /*
             * add each logical fifo id to bitmap
             */
            _SHR_PBMP_PORT_ADD(*logical_fifo_pbmp, rmc.rmc_id);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_to_port_type_get(
    int unit,
    bcm_port_if_t interface_type,
    int is_elk,
    int is_stif,
    dnx_algo_port_type_e * port_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (interface_type)
    {
        case BCM_PORT_IF_NULL:
        {
            *port_type = DNX_ALGO_PORT_TYPE_INVALID;
            break;
        }
        case BCM_PORT_IF_SFI:
        {
            *port_type = DNX_ALGO_PORT_TYPE_FABRIC;
            break;
        }
        case BCM_PORT_IF_RCY:
        {
            *port_type = DNX_ALGO_PORT_TYPE_RCY;
            break;
        }
        case BCM_PORT_IF_RCY_MIRROR:
        {
            *port_type = DNX_ALGO_PORT_TYPE_RCY_MIRROR;
            break;
        }
        case BCM_PORT_IF_CPU:
        {
            *port_type = DNX_ALGO_PORT_TYPE_CPU;
            break;
        }
        case BCM_PORT_IF_ERP:
        {
            *port_type = DNX_ALGO_PORT_TYPE_ERP;
            break;
        }
        case BCM_PORT_IF_OLP:
        {
            *port_type = DNX_ALGO_PORT_TYPE_OLP;
            break;
        }
        case BCM_PORT_IF_OAMP:
        {
            *port_type = DNX_ALGO_PORT_TYPE_OAMP;
            break;
        }
        case BCM_PORT_IF_EVENTOR:
        {
            *port_type = DNX_ALGO_PORT_TYPE_EVENTOR;
            break;
        }
        case BCM_PORT_IF_SAT:
        {
            *port_type = DNX_ALGO_PORT_TYPE_SAT;
            break;
        }
        case BCM_PORT_IF_ILKN:
        {
            *port_type = DNX_ALGO_PORT_TYPE_NIF_ILKN;
            if (is_elk)
            {
                *port_type = DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK;
            }
            break;
        }
        case BCM_PORT_IF_XFI:
        case BCM_PORT_IF_XLAUI:
        case BCM_PORT_IF_XLAUI2:
        case BCM_PORT_IF_CAUI:
        case BCM_PORT_IF_NIF_ETH:
        {
            *port_type = DNX_ALGO_PORT_TYPE_NIF_ETH;
            if (is_stif)
            {
                *port_type = DNX_ALGO_PORT_TYPE_NIF_ETH_STIF;
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported interface type (%d).\n", interface_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_to_interface_type_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int *is_elk,
    int *is_stif,
    bcm_port_if_t * interface_type)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_elk = 0;
    *is_stif = 0;
    switch (port_type)
    {
        case DNX_ALGO_PORT_TYPE_INVALID:
        {
            *interface_type = BCM_PORT_IF_NULL;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FABRIC:
        {
            *interface_type = BCM_PORT_IF_SFI;
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY:
        {
            *interface_type = BCM_PORT_IF_RCY;
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        {
            *interface_type = BCM_PORT_IF_RCY_MIRROR;
            break;
        }
        case DNX_ALGO_PORT_TYPE_CPU:
        {
            *interface_type = BCM_PORT_IF_CPU;
            break;
        }
        case DNX_ALGO_PORT_TYPE_ERP:
        {
            *interface_type = BCM_PORT_IF_ERP;
            break;
        }
        case DNX_ALGO_PORT_TYPE_OLP:
        {
            *interface_type = BCM_PORT_IF_OLP;
            break;
        }
        case DNX_ALGO_PORT_TYPE_OAMP:
        {
            *interface_type = BCM_PORT_IF_OAMP;
            break;
        }
        case DNX_ALGO_PORT_TYPE_EVENTOR:
        {
            *interface_type = BCM_PORT_IF_EVENTOR;
            break;
        }
        case DNX_ALGO_PORT_TYPE_SAT:
        {
            *interface_type = BCM_PORT_IF_SAT;
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
        {
            *interface_type = BCM_PORT_IF_ILKN;
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
        {
            *interface_type = BCM_PORT_IF_ILKN;
            *is_elk = 1;
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
        {
            *interface_type = BCM_PORT_IF_NIF_ETH;
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
        {
            *interface_type = BCM_PORT_IF_NIF_ETH;
            *is_stif = 1;
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port type (%d).\n", port_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_legacy_interface_to_nof_lanes_get(
    int unit,
    bcm_port_if_t interface_type,
    int *nof_lanes)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (interface_type)
    {
        case BCM_PORT_IF_XFI:
        {
            *nof_lanes = 1;
            break;
        }
        case BCM_PORT_IF_XLAUI:
        case BCM_PORT_IF_CAUI:
        {
            *nof_lanes = 4;
            break;
        }
        case BCM_PORT_IF_XLAUI2:
        {
            *nof_lanes = 2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported interface type (%d).\n", interface_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_set(
    int unit,
    bcm_port_t logical_port,
    int if_id)
{
    int tm_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Verify interface id */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_id_verify(unit, if_id));

    /** Get handle to TM IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_if_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(tm_if_handle);

    /*
     * Set Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.egr_if.set(unit, tm_if_handle, if_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_unset(
    int unit,
    bcm_port_t logical_port)
{
    int tm_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_if_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(tm_if_handle);

    /*
     * Set Data to default
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    egr_if.set(unit, tm_if_handle, DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_EGR_INTERFACES));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_egr_if_get(
    int unit,
    bcm_port_t logical_port,
    int *if_id)
{
    int tm_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_if_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(tm_if_handle);

    /*
     * Get Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.egr_if.get(unit, tm_if_handle, if_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_if_set(
    int unit,
    bcm_port_t logical_port,
    int if_id)
{
    dnx_algo_port_db_2d_handle_t sch_if_handle;
    int sch_if_valid;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Verify interface id */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_id_verify(unit, if_id));

    /** Get handle to SCH IF DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &sch_if_handle.h0));
    sch_if_handle.h1 = if_id;

    /*
     * Set Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.valid.get(unit, sch_if_handle.h0, sch_if_handle.h1, &sch_if_valid));
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.set(unit, logical_port, &sch_if_handle));
    if (!sch_if_valid)
    {
        /** set valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.valid.set(unit, sch_if_handle.h0, sch_if_handle.h1, 1));
        /** set scheduler interface id */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.sch_if.set(unit, sch_if_handle.h0, sch_if_handle.h1, if_id));
        /** set master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.
                        master_logical_port.set(unit, sch_if_handle.h0, sch_if_handle.h1, logical_port));
    }

exit:
    SHR_FUNC_EXIT;

}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_sch_if_get(
    int unit,
    bcm_port_t logical_port,
    int *if_id)
{
    dnx_algo_port_db_2d_handle_t sch_if_handle;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to SCH IF DB */ ;
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.sch_if_handle.get(unit, logical_port, &sch_if_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(sch_if_handle);

    /*
     * Get Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.sch_interface.sch_if.get(unit, sch_if_handle.h0, sch_if_handle.h1, if_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    dnx_algo_port_type_e port_type;
    int tm_interface_handle;
    bcm_port_t master_tdm_logical_port, master_non_tdm_logical_port;
    bcm_port_t next_tdm_master_port, next_non_tdm_master_port;
    dnx_algo_port_state_e state;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Verify TDM mode */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
    if (state != DNX_ALGO_PORT_STATE_REMOVED)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_mode_supported_verify(unit, tdm_mode));
    }

    /** Get handle to TM DB and TM interface DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_interface_handle.get(unit, logical_port, &tm_interface_handle));
    DNX_ALGO_PORT_HANDLE_VERIFY(tm_interface_handle);

    /*
     * Set Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tdm_mode.set(unit, tm_handle.h0, tm_handle.h1, tdm_mode));

    /*
     * Update tdm master port
     */
    /** get tdm master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    master_tdm_logical_port.get(unit, tm_interface_handle, &master_tdm_logical_port));
    /** if first tdm port added - define as master tdm port */
    if (master_tdm_logical_port == DNX_ALGO_PORT_INVALID && tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_tdm_logical_port.set(unit, tm_interface_handle, logical_port));
    }

    /** if master tdm port removed / changed - define as master tdm port */
    if (master_tdm_logical_port == logical_port && tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_TDM,
                         &next_tdm_master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_tdm_logical_port.set(unit, tm_interface_handle, next_tdm_master_port));
    }

    /*
     * Update Non TDM master port
     */
     /** get non tdm master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                    master_non_tdm_logical_port.get(unit, tm_interface_handle, &master_non_tdm_logical_port));
    /** if first non-tdm port added - define next one as master non-tdm port */
    if (master_non_tdm_logical_port == DNX_ALGO_PORT_INVALID && tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_non_tdm_logical_port.set(unit, tm_interface_handle, logical_port));
    }

    /** if master non-tdm port removed / changed - define next one as master non-tdm port */
    if (master_non_tdm_logical_port == logical_port
        && (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS || tdm_mode == DNX_ALGO_PORT_TDM_MODE_INVALID))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NEXT | DNX_ALGO_PORT_MASTER_F_NON_TDM,
                         &next_non_tdm_master_port));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.tm_interface.
                        master_non_tdm_logical_port.set(unit, tm_interface_handle, next_non_tdm_master_port));
    }

    /*
     * Set SoC INFO
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set(unit, logical_port, port_type, tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_tdm_mode_e * tdm_mode)
{
    dnx_algo_port_db_2d_handle_t tm_handle;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** Get handle to TM DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.tm_handle.get(unit, logical_port, &tm_handle));
    DNX_ALGO_PORT_2D_HANDLE_VERIFY(tm_handle);

    /*
     * Get Data
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.tm.tdm_mode.get(unit, tm_handle.h0, tm_handle.h1, tdm_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
