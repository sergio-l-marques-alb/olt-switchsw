/** \file algo_port_utils.c
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
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>

#include "algo_port_internal.h"
/*
 * }
 */
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_logicals_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_logicals_type_e logicals_type,
    uint32 flags,
    bcm_pbmp_t * logicals)
{
    dnx_algo_port_state_e state;
    int handle;
    bcm_port_t logical_port, master_logical_port;
    uint32 supported_flags;
    bcm_core_t logical_port_core;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP);

    /** clear bitmap */
    BCM_PBMP_CLEAR(*logicals);

    /** per type implementation */
    switch (logicals_type)
    {
            /*
             *  bitmap of free ports
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_INVALID:
        {
            /** This port type can be supported only for all cores */
            if (core != BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL .\n");
            }

            /** throw an error if unsupported flag requested */
            supported_flags = 0;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get invalid logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter valid ports to bitmap */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state != DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             *  bitmap of valid (used) ports
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_VALID:
        {
            /** This port type can be supported only for all cores */
            if (core != BCM_CORE_ALL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Core must be BCM_CORE_ALL .\n");
            }

            /** throw an error if unsupported flag requested */
            supported_flags = 0;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get valid logicals flags does not support the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter invalid ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of fabric ports 
             * (logical ports with fabric handles) 
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = 0;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get fabric logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter invalid fabric handles*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.fabric_handle.get(unit, logical_port, &handle));
                if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    continue;
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif ports 
             * (logical ports with valid nif handle) 
             * Additonal options: master only flag 
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF:
        {
            /** throw an error if unsupported flag requested */
            supported_flags =
                DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK |
                DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just add just the relevant to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter invalid nif handles*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &handle));
                if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));
                /** filter port types ILKN ELK if EXCLUDE_ELK flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK)
                    && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type))
                {
                    continue;
                }
                /** filter port types ETH_STIF  if EXCLUDE_STIF flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF)
                    && DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type))
                {
                    continue;
                }

                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }
                                        /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, handle, &logical_port_core));

                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }
                /*
                 * filter non-master port (if requested).
                 */
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif ethrnet ports 
             * (logical ports with port type == DNX_ALGO_PORT_TYPE_NIF_ETH) 
             * Additonal options: master only flag 
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter port types different from NIF ETH*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));
                if (port_type != DNX_ALGO_PORT_TYPE_NIF_ETH)
                {
                    continue;
                }

                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, handle, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested)*/
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.nif_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        /** NIF is not defined as master */
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }
            /*
             * bitmap of nif ILKN ports
             * If the logicals_type is DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN:
             *     (logical ports with port type == DNX_ALGO_PORT_TYPE_NIF_ILKN || DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK)
             *     Additonal options: master only flag and exclude ELK flag
             * If the logicals_type is DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK:
             *     (logical ports with port type == DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK)
             *     Additonal options: None
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN:
        case DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK:
        {
            /** throw an error if unsupported flag requested */
            supported_flags =
                (logicals_type ==
                 DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK) ? 0 : (DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY |
                                                                  DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK);
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get nif logicals flags does not supported the requested flags 0x%x %s%s.\n",
                                     flags, EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));
                /** filter port types different from ILKN and ILKN ELK */
                if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1 /* include elk */ ))
                {
                    continue;
                }
                /** filter port types ILKN ELK if EXCLUDE_ELK flag is set */
                if ((flags & DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK)
                    && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type))
                {
                    continue;
                }
                /** filter port types NON-ILKN ELK if logicals type is ILKN ELK */
                if ((logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK)
                    && !DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type))
                {
                    continue;
                }

                /** in case specific core asked - filter according to core */
                if (core != BCM_CORE_ALL)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }
                                        /** get core id */
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, handle, &logical_port_core));
                    /** filter the un-wanted core */
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested) */
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.ilkn_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.ilkn.master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }
                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }
            break;
        }

            /*
             * bitmap of tm ports 
             * (logical ports with valid tm handle) 
             * Additonal options: master only flag. 
             * Assuming that every tm port is pp port and vice versa 
             */
        case DNX_ALGO_PORT_LOGICALS_TYPE_PP:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH:
        case DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING:
        case DNX_ALGO_PORT_LOGICALS_TYPE_CPU:
        {
            /** throw an error if unsupported flag requested */
            supported_flags = DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY;
            SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                                     "Get tm logicals flags does not supported the requested flags 0x%x %s%s.\n", flags,
                                     EMPTY, EMPTY);

            /** Iterate all ports and just the relevants to bitmap */
            for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
            {
                /** filter free ports */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
                if (state == DNX_ALGO_PORT_STATE_INVALID)
                {
                    continue;
                }

                /** filter invalid tm handles*/
                SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port, &port_type));
                if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type))
                {
                    continue;
                }

                 /** in case specific core asked - filter acorrding to core */
                if (core != BCM_CORE_ALL)
                {
                    /** get core  and filter */
                    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &logical_port_core));
                    if (core != logical_port_core)
                    {
                        continue;
                    }
                }

                /** filter non-master port (if requested)*/
                if (flags & DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port, &handle));
                    if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.
                                    master_logical_port.get(unit, handle, &master_logical_port));
                    if (master_logical_port != logical_port)
                    {
                        continue;
                    }
                }

                /** filter ports according to port type */
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_type))
                    {
                        continue;
                    }
                }
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
                    {
                        continue;
                    }
                }
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type))
                    {
                        continue;
                    }
                }
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_PP)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type))
                    {
                        continue;
                    }
                }
                if (logicals_type == DNX_ALGO_PORT_LOGICALS_TYPE_CPU)
                {
                    if (!DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
                    {
                         continue;
                    }
                }
                /** add to bitmap */
                BCM_PBMP_PORT_ADD(*logicals, logical_port);
            }

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported logicals type %d .\n", logicals_type);
            break;
        }
    }

exit:
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_ALGO_PORT_BITMAP);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channels_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_port_channnels)
{
    uint32 supported_flags;
    bcm_port_t master_port, master_valid_port;
    bcm_pbmp_t valid_ports;
    bcm_port_t valid_port;
    dnx_algo_port_tdm_mode_e tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    supported_flags = DNX_ALGO_PORT_CHANNELS_F_MASK;
    SHR_IF_ERR_EXIT_WITH_LOG(utilex_bitstream_mask_verify(&flags, &supported_flags, 1),
                             "Get channels flags does not support the requested flags 0x%x %s%s.\n",
                             flags, EMPTY, EMPTY);

    /*
     * Get logical port master
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &master_port));

    /*
     * Iter all valid ports and add to the bitmap
     */
    BCM_PBMP_CLEAR(*logical_port_channnels);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0, &valid_ports));
    BCM_PBMP_ITER(valid_ports, valid_port)
    {
        /** filter ports with different master port*/
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, valid_port, 0, &master_valid_port));
        if (master_valid_port != master_port)
        {
            continue;
        }

        /** filter non tdm ports according to flags */
        if (flags & DNX_ALGO_PORT_CHANNELS_F_TDM_ONLY)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, valid_port, &tdm_mode));
            if (tdm_mode != DNX_ALGO_PORT_TDM_MODE_BYPASS)
            {
                continue;
            }
        }

        /** filter tdm ports according to flags */
        if (flags & DNX_ALGO_PORT_CHANNELS_F_NON_TDM_ONLY)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, valid_port, &tdm_mode));
            if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
            {
                continue;
            }
        }

        BCM_PBMP_PORT_ADD(*logical_port_channnels, valid_port);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_channels_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_channels)
{
    bcm_pbmp_t channels;
    SHR_FUNC_INIT_VARS(unit);

    /** Get bitmap of all channels */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, 0, &channels));

    /** Count the number of the members */
    BCM_PBMP_COUNT(channels, *nof_channels);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_logical_pbmp_to_tm_pbmp_get(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * pbmp_tm_arr)
{
    bcm_core_t core_id;
    bcm_port_t logical_port;
    uint32 tm_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear output TM ports bitmap */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(pbmp_tm_arr[core_id]);
    }

    /*
     * convert logical port bmp(bitmap) to tm port bmp, required per core
     */
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        /** Get Port + Core */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core_id, &tm_port));

        /** Add to bitmap*/
        BCM_PBMP_PORT_ADD(pbmp_tm_arr[core_id], tm_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tm_pbmp_to_logical_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t tm_ports,
    bcm_pbmp_t * logicals_ports)
{
    bcm_port_t tm_port;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Clear output bitmap */
    BCM_PBMP_CLEAR(*logicals_ports);

    /*
     * Iterate over each tm port: 
     * * verify the port is valid 
     * * convert to logical port
     */
    BCM_PBMP_ITER(tm_ports, tm_port)
    {
        /** Verify tm port valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));

        BCM_PBMP_PORT_ADD(*logicals_ports, logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_interface_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *rate)
{
    int num_of_lanes;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port rate (might be per rate per lane)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, flags, rate));

    /*
     * Modify the rate just for ilkn
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1 /* include elk */ ))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &num_of_lanes));
        *rate *= num_of_lanes;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_nif_phys_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_phys)
{
    bcm_pbmp_t phys;
    SHR_FUNC_INIT_VARS(unit);

    /** get bitmap of all phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phys));

    /** count the number of the members in the bitmap */
    BCM_PBMP_COUNT(phys, *nof_phys);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_logical_pbmp_to_pp_pbmp_get(
    int unit,
    bcm_pbmp_t logical_pbmp,
    bcm_pbmp_t * pbmp_pp_arr)
{
    bcm_core_t core_id;
    bcm_port_t logical_port;
    uint32 pp_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear output PP ports bitmap */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        BCM_PBMP_CLEAR(pbmp_pp_arr[core_id]);
    }

    /*
     * convert logical port bmp(bitmap) to pp port bmp, required per core
     */
    BCM_PBMP_ITER(logical_pbmp, logical_port)
    {
        /** Get Port + Core */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, logical_port, &core_id, &pp_port));

        /** Add to bitmap*/
        BCM_PBMP_PORT_ADD(pbmp_pp_arr[core_id], pp_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_pbmp_to_logical_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t pp_ports,
    bcm_pbmp_t * logicals_ports)
{
    bcm_port_t pp_port;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Clear output bitmap */
    BCM_PBMP_CLEAR(*logicals_ports);

    /*
     * Iterate over each pp port: 
     * * verify the port is valid 
     * * convert to logical port
     */
    BCM_PBMP_ITER(pp_ports, pp_port)
    {
        /** Verify pp port valid */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_to_logical_get(unit, core, pp_port, &logical_port));

        BCM_PBMP_PORT_ADD(*logicals_ports, logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_pp_to_tm_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_port_t pp_port,
    bcm_pbmp_t * pbmp_tm_ports)
{
    bcm_port_t logical_port;
    bcm_core_t core_i;
    uint32 pp_port_i;
    dnx_algo_port_db_2d_handle_t pp_handle;
    bcm_core_t tm_core_id;
    uint32 tm_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Clear output TM ports bitmap */
    BCM_PBMP_CLEAR(*pbmp_tm_ports);

    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));

        if (DNX_ALGO_PORT_IS_2D_HANDLE_VALID(pp_handle))
        {
            /** Get core and pp_port */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.core.get(unit, pp_handle.h0, pp_handle.h1, &core_i));
            SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.get(unit, pp_handle.h0, pp_handle.h1, &pp_port_i));
            if ((pp_port_i == pp_port) && (core_i == core))
            {
               /** Get tm port and core id */
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &tm_core_id, &tm_port));
               /** Add the tm_port to tm_ports pbmp */
                BCM_PBMP_PORT_ADD(*pbmp_tm_ports, tm_port);
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
dnx_algo_port_diag_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_t * diag_info)
{
    bcm_pbmp_t phy_ports;
    SHR_FUNC_INIT_VARS(unit);

    /** Init output */
    sal_memset(diag_info, 0, sizeof(dnx_algo_port_diag_info_t));

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /** logical port */
    diag_info->logical_port = logical_port;

    /** port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &diag_info->port_type));

    /** tm attributes */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, diag_info->port_type))
    {
        /** core and tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &diag_info->core_id, &diag_info->tm_port));

        /** channel id */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &diag_info->channel_id));
    }
    else
    {
        /** mark the tm port and channel as invalid */
        diag_info->tm_port = DNX_ALGO_PORT_INVALID;

        /** mark channel_id as invalid */
        diag_info->channel_id = -1;
    }

    /** phy id */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, diag_info->port_type, TRUE, TRUE))
    {
        /** first phy for nif */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phy_ports));
        _SHR_PBMP_FIRST(phy_ports, diag_info->phy_id);
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, diag_info->port_type))
    {
        /** link id for fabric */
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &diag_info->phy_id));
    }
    else
    {
        diag_info->phy_id = -1;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_diag_info_all_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_all_t * diag_info_all)
{
    dnx_algo_port_diag_info_logical_t *logical_info;
    dnx_algo_port_diag_info_nif_t *nif_info;
    dnx_algo_port_diag_info_tm_pp_t *tm_pp_info;
    dnx_algo_port_diag_info_fabric_t *fabric_info;
    dnx_algo_port_diag_info_ilkn_t *ilkn_info;
    dnx_algo_port_db_2d_handle_t pp_handle;
    dnx_algo_port_diag_info_interface_t *interface_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Init output */
    sal_memset(diag_info_all, 0, sizeof(dnx_algo_port_diag_info_all_t));

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    /*
     * Logical port info
     */
    logical_info = &diag_info_all->logical_port_info;
    /** logical port */
    logical_info->logical_port = logical_port;
    /** port type */
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &logical_info->port_type));
    /** get interface offset */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &logical_info->interface_offset));
    /** master port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, logical_port, 0, &logical_info->master_port));

    /*
     * NIF info
     */
    nif_info = &diag_info_all->nif_info;
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, logical_info->port_type, TRUE, TRUE))
    {
        /** mark data as valid */
        nif_info->valid = 1;
        /** get phy ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &nif_info->first_phy));
        /** get phy ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &nif_info->phy_ports));
        /** get logical fifos for the port */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_pbmp_get(unit, logical_port, 0, &nif_info->logical_fifos));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, logical_info->port_type, 1 /* include elk */ ))
        {
            ilkn_info = &diag_info_all->ilkn_info;
            /** mark data as valid */
            ilkn_info->valid = 1;
            /** get nof segments  */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, logical_port, &ilkn_info->nof_segments));
        }
    }

    /*
     * Interface info
     */
    interface_info = &diag_info_all->interface_info;
    if (!DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, logical_info->port_type))
    {
        interface_info->valid = 1;
        /** core */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &interface_info->core_id));
        /** interface speed */
        {
            int has_speed;

            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, &has_speed));
            if (has_speed == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &interface_info->interface_speed));
            }
            else
            {
                interface_info->interface_speed = -1;
            }
        }

    }
    /*
     * TM & PP info
     */
    tm_pp_info = &diag_info_all->tm_pp_info;
    /** tm attributes */
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, logical_info->port_type))
    {
        int dummy_core;

        /** mark data as valid */
        tm_pp_info->valid = 1;
        /** tm interface - internal data (read directly from data base) */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                        tm_interface_handle.get(unit, logical_port, &tm_pp_info->tm_interface));
        /** core and tm port */
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &dummy_core, &tm_pp_info->tm_port));
        /** channel id */
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &tm_pp_info->channel_id));

        /** is channelized */
        SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, logical_port, &tm_pp_info->is_channelized));

        /** channels */
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, logical_port, 0, &tm_pp_info->channels_ports));
        /** tdm mode */
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tm_pp_info->tdm_mode));
        /** tdm master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_TDM, &tm_pp_info->tdm_master));
        /** non tdm master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get
                        (unit, logical_port, DNX_ALGO_PORT_MASTER_F_NON_TDM, &tm_pp_info->non_tdm_master));
    }

    /** TM Egress scheduler */
    if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, logical_info->port_type))
    {
        /** base hr */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &tm_pp_info->base_hr));
        /** sch priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &tm_pp_info->sch_priorities));
        /** scheduler interface id */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &tm_pp_info->sch_if_id));
    }

    /** TM Egress scheduler */
    if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, logical_info->port_type))
    {
        /** egress interface id */
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, logical_port, &tm_pp_info->egr_if_id));
        /** base queue pair */
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &tm_pp_info->base_q_pair));
        /** port priorities */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &tm_pp_info->num_priorities));
    }

    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, logical_info->port_type))
    {
        /** pp port - read directly to be able to read for LAG members too */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.pp_handle.get(unit, logical_port, &pp_handle));
        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.pp_port.get(unit, pp_handle.h0, pp_handle.h1, &tm_pp_info->pp_port));
    }

    /*
     * Fabric info
     */
    fabric_info = &diag_info_all->fabric_info;
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, logical_info->port_type))
    {
        /** mark data as valid */
        fabric_info->valid = 1;
        /** link id for fabric */
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &fabric_info->link_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * To String / from String 
 * { 
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_type_str_get(
    int unit,
    dnx_algo_port_type_e port_type,
    char port_type_str[DNX_ALGO_PORT_TYPE_STR_LENGTH])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (port_type)
    {
        case DNX_ALGO_PORT_TYPE_INVALID:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "INVALID");
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "NIF_ETH");
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "NIF_ILKN");
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "NIF_ETH_STIF");
            break;
        case DNX_ALGO_PORT_TYPE_CPU:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "CPU");
            break;
        case DNX_ALGO_PORT_TYPE_RCY:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "RCY");
            break;
        case DNX_ALGO_PORT_TYPE_ERP:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "ERP");
            break;
        case DNX_ALGO_PORT_TYPE_OLP:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "OLP");
            break;
        case DNX_ALGO_PORT_TYPE_OAMP:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "OAMP");
            break;
        case DNX_ALGO_PORT_TYPE_SAT:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "SAT");
            break;
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "RCY_MIRROR");
            break;
        case DNX_ALGO_PORT_TYPE_EVENTOR:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "EVENTOR");
            break;
        case DNX_ALGO_PORT_TYPE_FABRIC:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "FABRIC");
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%s", "ILKN_ELK");
            break;
        /** In case mapping will be missed - return int value instead */
        default:
            sal_snprintf(port_type_str, DNX_ALGO_PORT_TYPE_STR_LENGTH, "%d", port_type);
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_tdm_mode_str_get(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode,
    char tdm_mode_str[DNX_ALGO_PORT_TDM_MODE_STR_LENGTH])
{
    SHR_FUNC_INIT_VARS(unit);

    switch (tdm_mode)
    {
        case DNX_ALGO_PORT_TDM_MODE_INVALID:
            sal_sprintf(tdm_mode_str, "%s", "INVALID");
            break;
        case DNX_ALGO_PORT_TDM_MODE_NONE:
            sal_sprintf(tdm_mode_str, "%s", "NONE");
            break;
        case DNX_ALGO_PORT_TDM_MODE_BYPASS:
            sal_sprintf(tdm_mode_str, "%s", "BYPASS");
            break;
        case DNX_ALGO_PORT_TDM_MODE_PACKET:
            sal_sprintf(tdm_mode_str, "%s", "PACKET");
            break;
        /** In case mapping will be missed - return int value instead */
        default:
            sal_sprintf(tdm_mode_str, "%d", tdm_mode);
            break;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_cdu_access_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_cdu_access_info_t * cdu_access_info)
{
    bcm_core_t core;
    int first_phy_port, nof_phys_per_cdu, nof_cdus_per_core, nof_lanes_per_mac;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy_port));

    /*
     * get dnx_dat information
     */
    nof_cdus_per_core = dnx_data_nif.eth.cdu_nof_per_core_get(unit);
    nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);
    nof_lanes_per_mac = dnx_data_nif.eth.mac_lanes_nof_get(unit);

    /*
     * calculate port attributes
     */
    cdu_access_info->core = core;
    cdu_access_info->cdu_id = (first_phy_port / nof_phys_per_cdu);
    cdu_access_info->inner_cdu_id = cdu_access_info->cdu_id % nof_cdus_per_core;
    cdu_access_info->cdu_first_phy = (first_phy_port / nof_phys_per_cdu) * nof_phys_per_cdu;
    cdu_access_info->first_lane_in_cdu = first_phy_port % nof_phys_per_cdu;
    cdu_access_info->mac_id = cdu_access_info->first_lane_in_cdu / nof_lanes_per_mac;

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_first_port_type_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_type_e port_type,
    bcm_port_t * logical_port)
{
    bcm_port_t logical_port_i;
    dnx_algo_port_type_e port_type_i;
    bcm_core_t core_i;
    dnx_algo_port_state_e state_i;
    int handle;
    SHR_FUNC_INIT_VARS(unit);
    *logical_port = DNX_ALGO_PORT_INVALID;

    for (logical_port_i = 0; logical_port_i < SOC_MAX_NUM_PORTS; logical_port_i++)
    {
        /** filter valid ports to bitmap */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port_i, &state_i));
        if (state_i == DNX_ALGO_PORT_STATE_INVALID)
        {
            continue;
        }

        /** in case specific core asked - filter acorrding to core */
        if (core != BCM_CORE_ALL)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.interface_handle.get(unit, logical_port_i, &handle));
            if (handle == DNX_ALGO_PORT_HANDLE_INVALID)
            {
                continue;
            }

            /** get core  and filter */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.interface.core.get(unit, handle, &core_i));
            if (core != core_i)
            {
                continue;
            }
        }

        /** look for the first requested port_type*/
        SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.type.get(unit, logical_port_i, &port_type_i));
        if (port_type_i == port_type)
        {
            *logical_port = logical_port_i;
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
dnx_algo_port_pll_index_get(
    int unit,
    bcm_port_t logical_port,
    int *pll_index)
{
    dnx_algo_port_type_e port_type;
    int first_phy, pll_found = 0;
    int is_over_fabric = 0, interface_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, logical_port, &interface_offset));
        is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, interface_offset)->is_over_fabric;
    }
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type) || is_over_fabric)
    {
        if (is_over_fabric)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &first_phy));
        }
        for (*pll_index = 0; *pll_index < dnx_data_fabric.general.nof_lcplls_get(unit); (*pll_index)++)
        {
            if ((first_phy >= dnx_data_fabric.general.pll_phys_get(unit, *pll_index)->min_phy_id) &&
                (first_phy <= dnx_data_fabric.general.pll_phys_get(unit, *pll_index)->max_phy_id))
            {
                pll_found = 1;
                break;
            }
        }
    }
    /** if nif port include elk and stif */
    else if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, logical_port, 0, &first_phy));
        for (*pll_index = 0; *pll_index < dnx_data_nif.global.nof_lcplls_get(unit); (*pll_index)++)
        {
            if ((first_phy >= dnx_data_nif.global.pll_phys_get(unit, *pll_index)->min_phy_id) &&
                (first_phy <= dnx_data_nif.global.pll_phys_get(unit, *pll_index)->max_phy_id))
            {
                pll_found = 1;
                break;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Port type %d\n", port_type);
    }
    if (!pll_found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No PLL index is found for port %d\n", logical_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_ilkn_lanes_to_nif_phys_get(
    int unit,
    int interface_id,
    bcm_pbmp_t * ilkn_lanes,
    bcm_pbmp_t * nif_phys)
{
    bcm_port_t phy_iter, interface_base_phy;
    const dnx_data_nif_ilkn_supported_phys_t *interface_supported_phys;
    int is_over_fabric;
    SHR_FUNC_INIT_VARS(unit);

    is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, interface_id)->is_over_fabric;
    interface_supported_phys = dnx_data_nif.ilkn.supported_phys_get(unit, interface_id);

    if (is_over_fabric)
    {
        _SHR_PBMP_FIRST(interface_supported_phys->fabric_phys, interface_base_phy);
    }
    else
    {
        _SHR_PBMP_FIRST(interface_supported_phys->nif_phys, interface_base_phy);
    }

    BCM_PBMP_CLEAR(*nif_phys);
    BCM_PBMP_ITER(*ilkn_lanes, phy_iter)
    {
        BCM_PBMP_PORT_ADD(*nif_phys, phy_iter + interface_base_phy);
    }

    SHR_FUNC_EXIT;
}

/*
 * }
 */
