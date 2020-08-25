/** \file appl_ref_dynamic_port.c
 * $Id$
 *
 * Dynamic port application for DNX. 
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_PORT

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/link.h>
#include <bcm/switch.h>

#include <appl/reference/dnx/appl_ref_dynamic_port.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/reference/dnx/appl_ref_sniff_ports.h>
#include "appl_ref_compensation_init.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

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

/*
 * See .h files
 */
shr_error_e
appl_dnx_dynamic_port_create(
    int unit,
    int port)
{
    bcm_port_config_t *port_config = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** create system port for the new port */
    SHR_IF_ERR_EXIT(appl_dnx_sys_ports_port_create(unit, port));

    /** create scheduling scheme for the new port */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_port_create(unit, port));

    {
        /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
        SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));

        /** map port (if ethernet) to mirror recycle port */
        if (BCM_PBMP_MEMBER(port_config->e, port) && !BCM_PBMP_MEMBER(port_config->rcy_mirror, port))
        {
            SHR_IF_ERR_EXIT(appl_dnx_sniff_ports_recycle_port_map(unit, port));
        }

        /** add NIF port to linkscan */
        if (BCM_PBMP_MEMBER(port_config->nif, port))
        {
            SHR_IF_ERR_EXIT(bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW));
        }

        /** set ingress compensation per port (used by crps and stif) */
        SHR_IF_ERR_EXIT(appl_dnx_compensation_port_delta_set(unit, port, APPL_DNX_REF_COMPENSATION_ING_PORT_DELTA));
    }

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_dynamic_port_destroy(
    int unit,
    int port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Destroy scheduling scheme for the port */
    SHR_IF_ERR_EXIT(appl_dnx_e2e_scheme_port_destroy(unit, port));

    /** unmap from recycle mirror port */
    SHR_IF_ERR_EXIT(appl_dnx_sniff_ports_recycle_port_unmap(unit, port));

    /** remove ingress compensation per port (used by crps and stif) */
    SHR_IF_ERR_EXIT(appl_dnx_compensation_port_delta_set(unit, port, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set dynamic port eth initial settings for ETH ports
 *
 * \param [in] unit - Unit ID
 * \param [in] port - Port
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
appl_dnx_dyn_port_pp_eth_initial_settings_set(
    int unit,
    bcm_port_t port)
{
    uint32 flags;
    uint32 class_id;
    bcm_port_config_t *port_config = NULL;
    SHR_FUNC_INIT_VARS(unit);

    flags = BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD;
    class_id = port;

    /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
    SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Set VLAN domain
     */
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, port, bcmPortClassId, class_id));
    /*
     * Set VLAN membership-namespace per port
     */
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, port, bcmPortClassVlanMember, class_id));
    /*
     * Add the port to BCM_VLAN_DEFAULT
     */
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, BCM_VLAN_DEFAULT, port, 0));
    /*
     *  Enable Same interface Filter per port
     */
    SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlBridge, TRUE));
    /*
     * Set untagged packet default VLAN per port
     */
    SHR_IF_ERR_EXIT(bcm_port_untagged_vlan_set(unit, port, BCM_VLAN_DEFAULT));
    /*
     * Enable learning
     */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, port, flags));

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    /*
     * Configure default stp state as FORWARD
     */
    if (BCM_PBMP_MEMBER(port_config->nif, port))
    {
        SHR_IF_ERR_EXIT(bcm_port_stp_set(unit, port, BCM_STG_STP_FORWARD));
    }

    /*
     * Set port default-LIF to simple bridge
     */
    SHR_IF_ERR_EXIT(bcm_vlan_control_port_set(unit, port, bcmVlanTranslateIngressMissDrop, 0));

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_dyn_ports_packet_processing_eth_init(
    int unit)
{
    bcm_port_t port;
    bcm_port_config_t *port_config = NULL;
    bcm_pbmp_t pbmp_eth;

    SHR_FUNC_INIT_VARS(unit);

    /**  alloc port config on heap to overcome stack overflow since it's a huge struct */
    SHR_ALLOC(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    BCM_PBMP_CLEAR(pbmp_eth);
    BCM_PBMP_OR(pbmp_eth, port_config->e);
    BCM_PBMP_REMOVE(pbmp_eth, port_config->rcy_mirror);
    /*
     * Take the ETH port
     */
    BCM_PBMP_ITER(pbmp_eth, port)
    {
        /*
         * Set port pp ETH initial settings
         */
        SHR_IF_ERR_EXIT(appl_dnx_dyn_port_pp_eth_initial_settings_set(unit, port));
    }

exit:
    SHR_FREE(port_config);
    SHR_FUNC_EXIT;
}
