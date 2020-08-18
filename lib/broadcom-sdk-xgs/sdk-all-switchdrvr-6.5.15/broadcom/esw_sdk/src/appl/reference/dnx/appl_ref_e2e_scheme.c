/** \file appl_ref_e2e_scheme.c
 * $Id$
 *
 * System ports level application procedures for DNX. 
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/stack.h>
#include <bcm_int/dnx_dispatch.h>

#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>

#include <appl/reference/sand/appl_ref_sand.h>

#include "appl_ref_sys_device.h"

#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/**
 * \brief - credit request profile id allocated for 10G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_10G_SLOW         (0)
/**
 * \brief - credit request profile id allocated for 40G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_40G_SLOW         (1)
/**
 * \brief - credit request profile id allocated for 100G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_100G_SLOW        (2)
/**
 * \brief - credit request profile id allocated for 200G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_200G_SLOW        (3)
/**
 * \brief - credit request profile id allocated for 400G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_400G_SLOW        (4)
/**
 * \brief - credit request profile id allocated for FMQs 2400G SLOW ports
 */
#define APPL_DNX_E2E_CREDIT_PROFILE_MC_2400G_SLOW      (5)

/**
 * \brief - rate class profile id allocated for 10G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_10G_SLOW         (0)
/**
 * \brief - rate class profile id allocated for 40G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_40G_SLOW         (1)
/**
 * \brief - rate class profile id allocated for 100G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_100G_SLOW        (2)
/**
 * \brief - rate class profile id allocated for 200G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_200G_SLOW        (3)
/**
 * \brief - rate class profile id allocated for 400G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_400G_SLOW        (4)
/**
 * \brief - rate class profile id allocated for FMQs 2400G SLOW ports
 */
#define APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_2400G_SLOW    (5)

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/**
 * \brief - Convert FAP index + core id to 
 * modid
 *
 */
shr_error_e
appl_dnx_fap_and_core_to_modid_convert(
    int unit,
    int fap_index,
    int core_id,
    int *modid)
{
    appl_dnx_sys_device_t *sys_params;
    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    *modid = fap_index * APPL_DNX_NOF_MODIDS_PER_DEVICE + sys_params->base_modid + core_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
int
appl_dnx_logical_port_to_base_voq_connector_id_get(
    int unit,
    bcm_port_t port,
    bcm_module_t remote_modid,
    int *base_voq_connector)
{
    int modid_index, remote_core_id;
    appl_dnx_sys_device_t *sys_params;
    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert remote modid to remote core id */
    remote_core_id = remote_modid % APPL_DNX_NOF_MODIDS_PER_DEVICE;

    /** convert modid to modid index (fap index) */
    modid_index = (remote_modid - sys_params->base_modid) / APPL_DNX_NOF_MODIDS_PER_DEVICE;

    /** call internal function to get voq_connector id */
    SHR_IF_ERR_EXIT(appl_sand_modid_to_base_voq_connector_id_convert(unit, port, remote_core_id, modid_index,
                                                                     sys_params->nof_devices, base_voq_connector));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_voq_profiles_get(
    int unit,
    int speed,
    int *delay_tolerance_level,
    int *rate_class)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get credit request profile according to port's speed
     */
    if (speed <= 20000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_10G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_10G_SLOW;
    }
    else if (speed <= 70000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_40G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_40G_SLOW;
    }
    else if (speed <= 150000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_100G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_100G_SLOW;
    }
    else if (speed <= 300000)
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_200G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_200G_SLOW;
    }
    else
    {
        *delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_400G_SLOW;
        *rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_400G_SLOW;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Create VOQ
 *
 */
shr_error_e
appl_dnx_e2e_scheme_voq_create(
    int unit,
    int port,
    int modid_idx)
{
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_config;
    bcm_gport_t sysport_gport, voq_gport;
    int sys_port, voq_base_id;
    int cos;
    int delay_tolerance_level, rate_class;
    bcm_port_resource_t port_resource;
    int speed;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;

    SHR_FUNC_INIT_VARS(unit);

    /** required delay tolerance profile */

    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &port_resource));
    speed = port_resource.speed;

    /*
     * Set credit request profile according to port's speed 
     */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_voq_profiles_get(unit, speed, &delay_tolerance_level, &rate_class));

    /** Convert modid_idx and logical port to system port id */
    SHR_IF_ERR_EXIT(appl_sand_modid_to_sysport_convert(unit, modid_idx, port, &sys_port));
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, sys_port);

    /** convert system port id to base voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, sys_port, &voq_base_id));

    /** Allocate the queue bundle and map system port to the base voq */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_base_id);

    sal_memset(&queue_bundle_config, 0, sizeof(bcm_cosq_ingress_queue_bundle_gport_config_t));
    queue_bundle_config.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
    queue_bundle_config.local_core_id = BCM_CORE_ALL; /** Irrelevant in case of WITH_ID allocation */
    queue_bundle_config.numq = APPL_SAND_E2E_SCHEME_COS;
    queue_bundle_config.port = sysport_gport;
    for (cos = 0; cos < queue_bundle_config.numq; cos++)
    {
        queue_bundle_config.queue_atrributes[cos].delay_tolerance_level = delay_tolerance_level;
        queue_bundle_config.queue_atrributes[cos].rate_class = rate_class;
    }

    /** allocate voqs bundle */
    SHR_IF_ERR_EXIT(bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_config, &voq_gport));

    for (cos = 0; cos < queue_bundle_config.numq; ++cos)
    {
        /** Configure Compensation - constant value of 24 bytes (Preamable, IFG, CRC) assuming regular NIF port */
        adjust_info.gport = voq_gport;
        adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;         /** compensation type*/
        adjust_info.flags = 0;
        adjust_info.cosq = cos;
        adjust_info.source_info.source_id = 0;         /** irrelevant */

        /** Set per queue compensation */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info, 24));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Destroy VOQ
 *
 */
shr_error_e
appl_dnx_e2e_scheme_voq_destroy(
    int unit,
    int port,
    int modid_idx)
{
    bcm_gport_t voq_gport;
    int sys_port, voq_base_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Convert modid_idx and logical port to system port id */
    SHR_IF_ERR_EXIT(appl_sand_modid_to_sysport_convert(unit, modid_idx, port, &sys_port));

    /** convert system port id to base voq id */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, sys_port, &voq_base_id));

    /** Destroy the queue bundle */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_base_id);
    SHR_IF_ERR_EXIT(bcm_cosq_gport_delete(unit, voq_gport));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Create VOQ connector
 *
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connector_create(
    int unit,
    int port,
    int ingress_modid_idx,
    int remote_core_id)
{
    appl_dnx_sys_device_t *sys_params;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_gport_t local_gport, voq_connector_gport;
    bcm_cosq_gport_info_t gport_info;
    int cos, nof_connections_per_hr;

    int voq_connector_id;
    bcm_cosq_voq_connector_gport_t voq_connector_config;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Set local port gport */
    BCM_GPORT_LOCAL_SET(local_gport, port);

    /** Get voq connector id */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(appl_sand_modid_to_base_voq_connector_id_convert(unit, port, remote_core_id,
                                                                     ingress_modid_idx, sys_params->nof_devices,
                                                                     &voq_connector_id));
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(voq_connector_gport, voq_connector_id, mapping_info.core);

    voq_connector_config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;
    voq_connector_config.nof_remote_cores = APPL_DNX_NOF_MODIDS_PER_DEVICE;
    voq_connector_config.numq = APPL_SAND_E2E_SCHEME_COS;

    SHR_IF_ERR_EXIT(appl_dnx_fap_and_core_to_modid_convert
                    (unit, ingress_modid_idx, remote_core_id, &voq_connector_config.remote_modid));
    BCM_COSQ_GPORT_E2E_PORT_SET(voq_connector_config.port, port);

    /** allocate voq connectors */
    SHR_IF_ERR_EXIT(bcm_cosq_voq_connector_gport_add(unit, &voq_connector_config, &voq_connector_gport));

    
    /** Each HR should be attached to euqal number of connectors  according to amount of HRs */
    nof_connections_per_hr = voq_connector_config.numq / mapping_info.num_priorities;

    /** Each VOQ connector attach to suitable HR */
    for (cos = 0; cos < voq_connector_config.numq; cos++)
    {
        /** Connect the voq connector to strict priority 0 leg */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_set(unit, voq_connector_gport, cos, BCM_COSQ_SP0, 0));

        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, local_gport);

        /** Connect voq to HR according to the amount of HRs (span the connectors over the HRs) */
        gport_info.cosq = cos / nof_connections_per_hr;

        /** get gport that represents port's HR */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));

        /** attach HR SPi to connecter */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_attach(unit, gport_info.out_gport, voq_connector_gport, cos));

        /** Set Slow Rate to be bcmCosqControlFlowSlowRate1 */
        SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, voq_connector_gport, cos, bcmCosqControlFlowSlowRate, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy VOQ connector
 *
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connector_destroy(
    int unit,
    int port,
    int ingress_modid_idx,
    int remote_core_id)
{

    appl_dnx_sys_device_t *sys_params;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_gport_t local_gport, voq_connector_gport;
    bcm_cosq_gport_info_t gport_info;
    int cos;
    int voq_connector_id;
    int nof_connections_per_hr;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Set local port gport */
    BCM_GPORT_LOCAL_SET(local_gport, port);

    /** Get voq connector id */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(appl_sand_modid_to_base_voq_connector_id_convert(unit, port, remote_core_id,
                                                                     ingress_modid_idx, sys_params->nof_devices,
                                                                     &voq_connector_id));
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(voq_connector_gport, voq_connector_id, mapping_info.core);

    
    /** Each HR should be attached to euqal number of connectors  according to amount of HRs */
    nof_connections_per_hr = APPL_SAND_E2E_SCHEME_COS / mapping_info.num_priorities;

    /** Each VOQ connector detach from suitable HR */
    for (cos = 0; cos < APPL_SAND_E2E_SCHEME_COS; cos++)
    {
        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, local_gport);
        /** detach voq connectors from HRs according to the amount of HRs */
        gport_info.cosq = cos / nof_connections_per_hr;

        /** get gport that represents port's HR */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));

        /** detach HR SPi to connecter */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_detach(unit, gport_info.out_gport, voq_connector_gport, cos));
    }

    /** deallocate voq connectors */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_delete(unit, voq_connector_gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Connect VOQ to Voq connector
 * on ingress side
 *
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connect(
    int unit,
    int port,
    int egress_modid_idx,
    int core_id,
    int is_disconnect)
{
    appl_dnx_sys_device_t *sys_params;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_cosq_gport_connection_t connection;
    int voq_base_id, voq_connector_id;
    int ingress_modid_idx;
    uint32 dummy_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** get port info */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info));

    /** Fill connections attributes */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    if (is_disconnect)
    {
        connection.flags |= BCM_COSQ_GPORT_CONNECTION_INVALID;
    }

    SHR_IF_ERR_EXIT(appl_dnx_fap_and_core_to_modid_convert
                    (unit, egress_modid_idx, mapping_info.core, &connection.remote_modid));

    /** set voq id */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, connection.remote_modid,
                                                                     port, &voq_base_id));
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, core_id, voq_base_id);

    /** set voq connector id */
    ingress_modid_idx = (sys_params->modid - sys_params->base_modid) / APPL_DNX_NOF_MODIDS_PER_DEVICE;
    SHR_IF_ERR_EXIT(appl_sand_modid_to_base_voq_connector_id_convert(unit, port, core_id,
                                                                     ingress_modid_idx, sys_params->nof_devices,
                                                                     &voq_connector_id));

    /** set voq connector gport */
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, voq_connector_id, mapping_info.core);

    /** set the 'voq to voq_connector' connection */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_set(unit, &connection));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Connect Voq connector to VOQ 
 * on egress side
 *
 */
shr_error_e
appl_dnx_e2e_scheme_voq_connector_connect(
    int unit,
    int port,
    int modid_idx,
    int remote_core_id,
    int is_disconnect)
{
    appl_dnx_sys_device_t *sys_params;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_cosq_gport_connection_t connection;
    int voq_base_id, voq_connector_id;
    uint32 dummy_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** get port info */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info));

    /** get voq id */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, sys_params->modid + mapping_info.core,
                                                                     port, &voq_base_id));

    /** fill connection attributes */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    if (is_disconnect)
    {
        connection.flags |= BCM_COSQ_GPORT_CONNECTION_INVALID;
    }

    /** set voq connector id */
    SHR_IF_ERR_EXIT(appl_sand_modid_to_base_voq_connector_id_convert(unit, port, remote_core_id,
                                                                     modid_idx, sys_params->nof_devices,
                                                                     &voq_connector_id));
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, voq_connector_id, mapping_info.core);

    /** set voq id */
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, remote_core_id, voq_base_id);

    /** set remote modid */
    SHR_IF_ERR_EXIT(appl_dnx_fap_and_core_to_modid_convert(unit, modid_idx, 0, &connection.remote_modid));

    /** set the 'voq_connector to voq' connection */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_set(unit, &connection));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_scheme_port_create(
    int unit,
    int port)
{
    appl_dnx_sys_device_t *sys_params;
    int modid_idx;

    uint32 flags;
    bcm_gport_t e2e_gport, local_gport;
    int nof_active_cores, sch_priority;
    int remote_core_id;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /*
     * [Egress] Configure HR mode (change to enhanced mode). 
     *  Before creating the scheme, need to configure the existing HRs mode.
     */
    {
        
        SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));

        BCM_GPORT_LOCAL_SET(local_gport, port);
        flags = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED | BCM_COSQ_GPORT_REPLACE);
        for (sch_priority = 0; sch_priority < mapping_info.num_priorities; sch_priority++)
        {
            BCM_COSQ_GPORT_E2E_PORT_TC_SET(e2e_gport, port);
            SHR_IF_ERR_EXIT(bcm_cosq_gport_add(unit, local_gport, sch_priority, flags, &e2e_gport));
        }
    }

    /*
     * Create scheduling scheme for a given port:
     *   1. [Egress] Create Connectors for all VOQs pointing to the port on remote devices.
     *   2. [Ingress] Create VOQs pointing to the port on remote devices.
     *   3. [Egress] Connect the created connectors to the VOQs of the remote devices.
     *   4. [Ingress] Connect the created VOQs to the connectors of the port of each remote device. 
     */

    /*
     * Stage 1: [Egress] Create VOQ connectors for all VOQs pointing to the port on remote devices.
     */

    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        remote_core_id = 0;

        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_create(unit, port, modid_idx, remote_core_id));
    }

    /*
     * Stage 2: [Ingress] Create VOQs pointing to the port on remote devices: 
     *   - Create VOQs
     *   - Set Compensation for the created VOQs
     */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_create(unit, port, modid_idx));
    }

    /*
     * Stage 3: Connect Ingress VOQs <=> Egress VOQ connectors.
     */

    /** get number of cores on the device */
    SHR_IF_ERR_EXIT(bcm_stk_modid_count(unit, &nof_active_cores));

    /** [Ingress] Connect VOQs to VOQ connectors of the port of each remote device. */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        /** Connect voq to voq_connector */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connect(unit, port, modid_idx, BCM_CORE_ALL, 0 /** is_disconnect */ ));
    }

    /** [Egress] Connect VOQ connectors to VOQs of the remote devices. */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        /** Connect voq_connector to voq */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_connect
                        (unit, port, modid_idx, BCM_CORE_ALL, 0 /** is_disconnect */ ));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_scheme_port_destroy(
    int unit,
    int port)
{
    appl_dnx_sys_device_t *sys_params;
    int modid_idx;

    int remote_core_id;
    int nof_active_cores;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /*
     * Destroy scheduling scheme for a given port:
     *   1. [Ingress] Disconect VOQ pointing to the port on remote devices from all VOQ connectors
     *   1. [Egress]  Disconnect Connectors for all VOQs pointing to the port on remote devices.
     *   2. [Ingress] Destroy VOQs pointing to the port on remote devices.
     *   3. [Egress] Destroy Connectors for all VOQs pointing to the port on remote devices.
     */

    /*
     * Stage 1 [Ingress]: DisConnect Ingress VOQs <=> Egress VOQ connectors.
     */

    /** get number of cores on the device */
    SHR_IF_ERR_EXIT(bcm_stk_modid_count(unit, &nof_active_cores));

    /** [Ingress] DisConnect VOQs to VOQ connectors of the port of each remote device. */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        /** Disconnect voq to voq_connector */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connect(unit, port, modid_idx, BCM_CORE_ALL, 1 /** is_disconnect */ ));
    }

    /** Stage 1: [Egress] Disconnect VOQ connectors to VOQs of the remote devices. */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        /** Disconnect voq_connector to voq */
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_connect
                        (unit, port, modid_idx, BCM_CORE_ALL, 1 /** is_disconnect */ ));
    }
    /*
     * Stage 2: [Ingress] Destroy VOQs pointing to the port on remote devices: 
     *   - Destroy VOQs
     */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_destroy(unit, port, modid_idx));
    }

    /*
     * Stage 3: [Egress] Destroy voq connectors for all voqs pointing to the port on remote devices.
     */
    for (modid_idx = 0; modid_idx < sys_params->nof_devices; modid_idx++)
    {
        remote_core_id = 0;

        SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_voq_connector_destroy(unit, port, modid_idx, remote_core_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create multicast scheduling scheme 
 * Before creating VOQs, User must specify the range of the FMQs in the device.
 * In most cases, where fabric multicast is only defined by packet tc, range should set between 0-3. 
 * Set range that is different than 0-3, need to change fabric scheduler mode. 
 * Please see more details in the UM, Cint example: 
 * cint_enhance_application.c and API: 
 * bcm_fabric_control_set type: bcmFabricMulticastSchedulerMode.  
 */
static shr_error_e
appl_dnx_e2e_scheme_mcast_create(
    int unit)
{
    bcm_gport_t gport_mcast_queue_group;
    bcm_cosq_ingress_queue_bundle_gport_config_t queue_bundle_config;
    int numq;
    SHR_FUNC_INIT_VARS(unit);

    /** by default set simple FMQ mode */
    SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricMulticastSchedulerMode, 0));

    /** Set voq range for FMQs */
    SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricMulticastQueueMax, 3));

    /** Create 4 FMQs (0...3) */
    sal_memset(&queue_bundle_config, 0, sizeof(bcm_cosq_ingress_queue_bundle_gport_config_t));
    queue_bundle_config.flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID;
    queue_bundle_config.local_core_id = BCM_CORE_ALL; /** Irrelevant in case of WITH_ID allocation */
    queue_bundle_config.numq = 4;
    queue_bundle_config.port = 0; /** Irrelevant in case of FMQs */
    for (numq = 0; numq < queue_bundle_config.numq; numq++)
    {
        queue_bundle_config.queue_atrributes[numq].rate_class = APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_2400G_SLOW;
        queue_bundle_config.queue_atrributes[numq].delay_tolerance_level = APPL_DNX_E2E_CREDIT_PROFILE_MC_2400G_SLOW;
    }
    BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_mcast_queue_group, BCM_CORE_ALL, 0);
    SHR_IF_ERR_EXIT(bcm_cosq_ingress_queue_bundle_gport_add(unit, &queue_bundle_config, &gport_mcast_queue_group));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_rate_class_profiles_create(
    int unit)
{
    int profiles[] = { APPL_DNX_E2E_RATE_CLASS_PROFILE_10G_SLOW, APPL_DNX_E2E_RATE_CLASS_PROFILE_40G_SLOW,
        APPL_DNX_E2E_RATE_CLASS_PROFILE_100G_SLOW, APPL_DNX_E2E_RATE_CLASS_PROFILE_200G_SLOW,
        APPL_DNX_E2E_RATE_CLASS_PROFILE_400G_SLOW, APPL_DNX_E2E_RATE_CLASS_PROFILE_MC_2400G_SLOW
    };
    int profile_rates[] = { 10, 40, 100, 200, 400, 2400 };      /* rates in gbps */
    int profile_index;
    int flags = 0;
    bcm_gport_t rate_class_gport;
    bcm_cosq_rate_class_create_info_t create_info;
    SHR_FUNC_INIT_VARS(unit);

    for (profile_index = 0; profile_index < COUNTOF(profiles); profile_index++)
    {
        create_info.attributes = 0;

        /** set multicast indication for FMQ profile */
        if (profiles[profile_index] == APPL_DNX_E2E_CREDIT_PROFILE_MC_2400G_SLOW)
        {
            create_info.attributes |= BCM_COSQ_RATE_CLASS_CREATE_ATTR_MULTICAST;
        }

        BCM_GPORT_PROFILE_SET(rate_class_gport, profile_index);
        create_info.rate = profile_rates[profile_index];
        create_info.attributes |= BCM_COSQ_RATE_CLASS_CREATE_ATTR_SLOW_ENABLED;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_rate_class_create(unit, rate_class_gport, flags, &create_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_credit_request_profiles_create(
    int unit)
{
    bcm_cosq_delay_tolerance_t credit_request_profile;
    bcm_cosq_delay_tolerance_preset_attr_t credit_request_attr;
    int profiles[] = { APPL_DNX_E2E_CREDIT_PROFILE_10G_SLOW, APPL_DNX_E2E_CREDIT_PROFILE_40G_SLOW,
        APPL_DNX_E2E_CREDIT_PROFILE_100G_SLOW, APPL_DNX_E2E_CREDIT_PROFILE_200G_SLOW,
        APPL_DNX_E2E_CREDIT_PROFILE_400G_SLOW, APPL_DNX_E2E_CREDIT_PROFILE_MC_2400G_SLOW
    };
    int profile_rates[] = { 10, 40, 100, 200, 400, 2400 };      /* rates in gbps */
    int profile_index, my_modid;
    uint32 credit_size;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create credit request profiles
     */
    bcm_cosq_delay_tolerance_preset_attr_t_init(&credit_request_attr);

    /** assume symmetric system */
    SHR_IF_ERR_EXIT(bcm_stk_modid_get(unit, &my_modid));
    SHR_IF_ERR_EXIT(bcm_cosq_dest_credit_size_get(unit, my_modid, &credit_size));
    credit_request_attr.credit_size = credit_size;
    credit_request_attr.flags = BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_SLOW_ENABLED;
    for (profile_index = 0; profile_index < COUNTOF(profiles); profile_index++)
    {
        /** get set of recommended values */
        credit_request_attr.rate = profile_rates[profile_index];

        /** set multicast indication for FMQ profile */
        if (profiles[profile_index] == APPL_DNX_E2E_CREDIT_PROFILE_MC_2400G_SLOW)
        {
            credit_request_attr.flags |= BCM_COSQ_DELAY_TOLERANCE_PRESET_ATTR_F_MULTICAST;
        }

        SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_preset_get(unit, &credit_request_attr, &credit_request_profile));

        /** set the threshold */
        SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_set(unit, profiles[profile_index], &credit_request_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create unicast scheduling scheme for each port. 
 * For more info \see appl_dnx_e2e_scheme_port_create().
 */
static shr_error_e
appl_dnx_e2e_scheme_ucast_create(
    int unit)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /** get all ports */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_ports_get(unit, &pbmp));

    /** create scheduling scheme for each port */
    BCM_PBMP_ITER(pbmp, port)
    {
        rv = appl_dnx_e2e_scheme_port_create(unit, port);
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed to create e2e scheme for port: %d.%s%s", port, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_scheme_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** create credit request profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_credit_request_profiles_create(unit));

    /** create rate class profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_rate_class_profiles_create(unit));

    /** Create FMQs (fabric multicast queues) e2e scheduling scheme before allocating any VoQs */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_mcast_create(unit));

    /** Create unicast e2e scheduling scheme */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_ucast_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_e2e_profiles_create(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** create credit request profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_credit_request_profiles_create(unit));

    /** create rate class profiles - to be used when creating ingress queues*/
    SHR_IF_ERR_EXIT(appl_dnx_e2e_rate_class_profiles_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
int
appl_dnx_e2e_scheme_logical_port_to_base_voq_get(
    int unit,
    bcm_module_t egress_modid,
    bcm_port_t logical_port,
    int *base_voq)
{
    int modid_index;
    int sysport;
    appl_dnx_sys_device_t *sys_params;
    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert modid to modid index (fap index) */
    modid_index = (egress_modid - sys_params->base_modid) / APPL_DNX_NOF_MODIDS_PER_DEVICE;
    SHR_IF_ERR_EXIT(appl_sand_modid_to_sysport_convert(unit, modid_index, logical_port, &sysport));
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, sysport, base_voq));

exit:
    SHR_FUNC_EXIT;
}
