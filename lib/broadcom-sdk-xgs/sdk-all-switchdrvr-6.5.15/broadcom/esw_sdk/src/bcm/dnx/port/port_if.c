/**
 * \file port_if.c $Id$ PORT Interfaces procedures for DNX. 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_ilkn_reg_access.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/failover/failover.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/legacy/mbcm.h>
#include <src/bcm/dnx/tune/ecgm_tune.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <soc/sand/sand_aux_access.h>
#include "port_utils.h"
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/counter.h>

/*
 * }
 */

/*
 * Structs
 * {
 */

/**
 * \brief
 *   Information needed for handling bcm_port_resource_multi_set
 */
typedef struct
{
    /** Original user's port */
    bcm_gport_t original_port;
    /** type of the port */
    dnx_algo_port_type_e port_type;
    /** is port has its speed configured */
    int is_has_speed;
    /** is port enabled */
    int is_port_enable;
} dnx_port_resource_port_info_t;

/*
 * }
 */

/*
 * Macros.
 * {
 */
/**
 * \brief - supported flags for API bcm_dnx_lane_to_serdes_map_set()
 */
#define DNX_PORT_LANE_MAP_SUPPORTED_FLAGS  (BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE)
/*
 * }
 */

static int dnx_port_phy_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value);

static int dnx_port_phy_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 value);

static void dnx_port_custom_reg_access(
    int unit);

/**
 * \brief - initialize the Port module -not used
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * This method should be called only during init sequence,
 *     not to be used dynamically by the user.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_init API is not supported.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve autonegotiation abilities advertised for
 *        the port - not used
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [out] ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_advert_get(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_advert_get API is not supported. Please use bcm_port_autoneg_ability_advert_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set autonegotiation abilities adverised for the
 *        port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_advert_set(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_advert_set API is not supported. Please use bcm_port_autoneg_ability_advert_set instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the autonegotiation abilities for the
 *        remote side of the port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_remote_get(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_remote_get API is not supported. Please use bcm_port_autoneg_ability_remote_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the autonegotiation abilities for the local
 *        side of the port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] local_ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_local_get(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * local_ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_local_get API is not supported. Please use bcm_port_speed_ability_local_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the autonegotiation abilities for the local
 *        side of the port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] local_ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_ability_get(
    int unit,
    bcm_port_t port,
    bcm_port_abil_t * local_ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_get API is not supported. Please use bcm_port_speed_ability_local_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - INTERNAL FUNCTION initialize the Port module
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e
dnx_port_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** init IMB  module*/
    SHR_IF_ERR_EXIT(imb_init_all(unit));

    /*
     * init ilk reg access
     */
    dnx_port_custom_reg_access(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize the Port module
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * This method will be called only during init sequence.
 * \see
 *   * None
 */
shr_error_e
dnx_port_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

        /** deinit IMB  module*/
    SHR_IF_ERR_EXIT(imb_deinit_all(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize the port subsystem without affecting the current state of
 * stack ports.
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_clear API is not supported.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_autoneg_ability_advert_get
 *     bcm_dnx_port_autoneg_ability_advert_set
 *     bcm_dnx_port_speed_ability_local_get
 *     bcm_dnx_port_autoneg_ability_remote_get
 */

static shr_error_e
dnx_port_autoneg_ability_verify(
    int unit,
    bcm_port_t port,
    int num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability,
    int is_get)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(abilities, _SHR_E_PARAM, "speed abilities");

    /*
     * For GET API, need to verify the actual_num_ability parameter.
     */
    if (is_get)
    {
        SHR_NULL_CHECK(actual_num_ability, _SHR_E_PARAM, "actual_num_ability");
    }

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    if (!is_get && (num_ability > dnx_data_nif.eth.an_max_nof_abilities_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max auto-negotiation abilities is %d.\r\n",
                     dnx_data_nif.eth.an_max_nof_abilities_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_port_autoneg_ability_advert_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      max_num_ability  -- the max num of autoneg ability this port can advertise
 *      ability - (OUT) values indicating the ability this port advertise
 *      actual_num_ability - (OUT) the actual num of ability that this port advertise
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
shr_error_e
bcm_dnx_port_autoneg_ability_advert_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_autoneg_ability_verify
                          (unit, port, max_num_ability, abilities, actual_num_ability, 1));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_autoneg_ability_advert_get
                    (unit, logical_port, max_num_ability, abilities, actual_num_ability));
exit:
    SHR_FUNC_EXIT;
}
/*
 * Function:
 *      bcm_port_autoneg_ability_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      num_ability - number of ability the port will advertise
 *      ability - Local advertisement for each ability.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call WILL NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

shr_error_e
bcm_dnx_port_autoneg_ability_advert_set(
    int unit,
    bcm_port_t port,
    int num_ability,
    bcm_port_speed_ability_t * abilities)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_autoneg_ability_verify(unit, port, num_ability, abilities, NULL, 0));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
    SHR_IF_ERR_EXIT(imb_port_autoneg_ability_advert_set(unit, logical_port, num_ability, abilities));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_port_speed_ability_local_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      max_num_ability  -- the max num of speed ability this port support
 *      ability - (OUT) values indicating the ability of the MAC/PHY
 *      actual_num_ability - (OUT) the actual num of ability that this port can support
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
shr_error_e
bcm_dnx_port_speed_ability_local_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_autoneg_ability_verify
                          (unit, port, max_num_ability, abilities, actual_num_ability, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_speed_ability_local_get
                    (unit, logical_port, max_num_ability, abilities, actual_num_ability));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_port_autoneg_ability_advert_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit #.
 *      port - port #.
 *      max_num_ability  -- the max num of speed ability remote partner can advertise
 *      ability - (OUT) values indicating the ability of the MAC/PHY
 *      actual_num_ability - (OUT) the actual num of ability that remote partner advertise
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
shr_error_e
bcm_dnx_port_autoneg_ability_remote_get(
    int unit,
    bcm_port_t port,
    int max_num_ability,
    bcm_port_speed_ability_t * abilities,
    int *actual_num_ability)
{
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_autoneg_ability_verify
                          (unit, port, max_num_ability, abilities, actual_num_ability, 1));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_autoneg_ability_remote_get
                    (unit, logical_port, max_num_ability, abilities, actual_num_ability));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_autoneg_get
 *     bcm_dnx_port_autoneg_set
 */

static shr_error_e
dnx_port_autoneg_verify(
    int unit,
    bcm_port_t port,
    int *autoneg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(autoneg, _SHR_E_PARAM, "autoneg");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve autoneg enable indication
 * 
 * \param [in] unit - chip unit id 
 * \param [in] port - logical port
 * \param [out] autoneg - enable indication
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_autoneg_get(
    int unit,
    bcm_port_t port,
    int *autoneg)
{
    bcm_port_t logical_port = 0;
    phymod_autoneg_control_t an;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_autoneg_verify(unit, port, autoneg));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    phymod_autoneg_control_t_init(&an);

    /*
     *Not supported for Fabric ports - IMB will return an error. 
     */
    SHR_IF_ERR_EXIT(imb_port_autoneg_get(unit, logical_port, 0 /* flags */ , &an));
    *autoneg = an.enable;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable autoneg 
 * 
 * \param [in] unit - chip unit id 
 * \param [in] port - logical port
 * \param [in] autoneg - enable indication
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_autoneg_set(
    int unit,
    bcm_port_t port,
    int autoneg)
{
    bcm_port_t logical_port = 0;
    phymod_autoneg_control_t an;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_autoneg_verify(unit, port, &autoneg));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    phymod_autoneg_control_t_init(&an);
    /*
     *Not supported for Fabric ports - IMB will return an error. 
     */
    SHR_IF_ERR_EXIT(imb_port_autoneg_get(unit, logical_port, 0 /* flags */ , &an));
    an.enable = autoneg;
    SHR_IF_ERR_EXIT(imb_port_autoneg_set(unit, logical_port, 0 /* flags */ , &an));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_config_get
 */

static shr_error_e
dnx_port_config_get_verify(
    int unit,
    bcm_port_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "port_config");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve all ports configuration info.
 * 
 * \param [in] unit - chip unit id.
 * \param [out] config - configuration info.
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_config_get(
    int unit,
    bcm_port_config_t * config)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_config_get_verify(unit, config));

    bcm_port_config_t_init(config);

    {
        config->il = PBMP_IL_ALL(unit);
        config->xl = PBMP_XL_ALL(unit);
        config->ce = PBMP_CE_ALL(unit);
        config->xe = PBMP_XE_ALL(unit);
        config->hg = PBMP_HG_ALL(unit);
        config->sfi = PBMP_SFI_ALL(unit);
        config->port = PBMP_PORT_ALL(unit);
        config->cpu = PBMP_CMIC(unit);
        config->rcy = PBMP_RCY_ALL(unit);
        config->e = PBMP_E_ALL(unit);
        config->all = PBMP_ALL(unit);
        config->cd = SOC_PORT_BITMAP(unit, cde);
        config->le = SOC_PORT_BITMAP(unit, le);
        config->cc = SOC_PORT_BITMAP(unit, cc);
    }

    /** Special ports that are not included in SOC PBMP, require iteration over all ports */
    {
        _SHR_PBMP_CLEAR(config->erp);
        _SHR_PBMP_CLEAR(config->olp);
        _SHR_PBMP_CLEAR(config->oamp);
        _SHR_PBMP_CLEAR(config->eventor);
        _SHR_PBMP_CLEAR(config->sat);
        _SHR_PBMP_CLEAR(config->rcy_mirror);

        /** Get all ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0     /* flags 
                                                                                                                 */ , &pbmp));

        /** iterate over the ports bitmap and search for the right port types */
        _SHR_PBMP_ITER(pbmp, port)
        {
            /** get current port type */
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

            /** ERP port */
            if (DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type)) /** ERP port */
            {
                _SHR_PBMP_PORT_ADD(config->erp, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_OLP(unit, port_type)) /** OLP port*/
            {
                _SHR_PBMP_PORT_ADD(config->olp, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_OAMP(unit, port_type)) /** OAMP port */
            {
                _SHR_PBMP_PORT_ADD(config->oamp, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_EVENTOR(unit, port_type)) /** EVENTOR port */
            {
                _SHR_PBMP_PORT_ADD(config->eventor, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_SAT(unit, port_type)) /** SAT port */
            {
                _SHR_PBMP_PORT_ADD(config->sat, port);
            }
            else if (DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_type)) /** RCY_MIRROR port */
            {
                _SHR_PBMP_PORT_ADD(config->rcy_mirror, port);
            }
        }
    }

    /** NIF ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK | DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF        /* flags 
                                                                                                                                                                                         */ , &config->nif));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_internal_get() API
 */
static shr_error_e
dnx_port_internal_get_verify(
    int unit,
    uint32 flags,
    int internal_ports_max,
    bcm_gport_t * internal_gport,
    int *internal_ports_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(internal_gport, _SHR_E_PARAM, "internal_gport");
    SHR_NULL_CHECK(internal_ports_count, _SHR_E_PARAM, "internal_ports_count");

    /** Supported flags check */
    if (flags & BCM_PORT_INTERNAL_RECYCLE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Recycle ports are no longer internal - they are available in pbmp.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - returns gport of type "local" or "mod:port"
 * This returns handle to port. Not to Interface. From a port the interface handle is retrieved
 * via bcm_petra_fabric_port_get() API.
 * Following internal functionality required by other modules (internal functions)
 *    - convert the port to "fap port"
 *    - get the port type (e.g. to indicate that some operations cannot occur on some port types)
 * 
 * \param [in] unit - Unit ID
 * \param [in] flags -  Flags that determine the requested port type. See flags starting with BCM_PORT_INTERNAL_CONF...
 * \param [in] internal_ports_max - Number of ports to get
 * \param [in] internal_gport - Array of returned gports
 * \param [in] internal_ports_count - Number of actual returned ports
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * This API is legacy, there is a better API that returns all port types bitmaps.
 * \see
 *   * bcm_port_config_get API
 */
int
bcm_dnx_port_internal_get(
    int unit,
    uint32 flags,
    int internal_ports_max,
    bcm_gport_t * internal_gport,
    int *internal_ports_count)
{
    int count = 0;
    bcm_gport_t gport;
    int core, core_i, nof_cores;
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_internal_get_verify(unit, flags, internal_ports_max, internal_gport,
                                                       internal_ports_count));

    /** Initialize */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    /** Determine the requested core */
    if ((flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE0) && (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE1))
    {
        core = BCM_CORE_ALL;
    }
    else if (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE0)
    {
        core = 0;
    }
    else if (flags & BCM_PORT_INTERNAL_CONF_SCOPE_CORE1)
    {
        if (nof_cores < 2)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Requested Invalid core\n");
        }
        core = 1;
    }
    else
    {
        core = BCM_CORE_ALL;
    }

    /** Return the requested ports */
    DNXCMN_CORES_ITER(unit, core, core_i)
    {
        /** get tm ports on the requested core */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core_i, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0      /* flags 
                                                                                                                 */ , &pbmp));

        /** iterate over the ports bitmap and search for the right port types */
        _SHR_PBMP_ITER(pbmp, port)
        {
            /** get current port type */
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

            /** ERP port */
            if ((flags & BCM_PORT_INTERNAL_EGRESS_REPLICATION) && DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type))
            {
                if (count >= internal_ports_max)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "internal_ports_max < number of internal ports (now adding ERP)\n");
                }
                BCM_GPORT_LOCAL_SET(gport, port);
                internal_gport[count++] = gport;
            }

            /** OLP port*/
            if ((flags & BCM_PORT_INTERNAL_OLP) && DNX_ALGO_PORT_TYPE_IS_OLP(unit, port_type))
            {
                if (count >= internal_ports_max)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "internal_ports_max < number of internal ports (now adding OLP)\n");
                }
                BCM_GPORT_LOCAL_SET(gport, port);
                internal_gport[count++] = gport;
            }

            /** OAMP port */
            if ((flags & BCM_PORT_INTERNAL_OAMP) && DNX_ALGO_PORT_TYPE_IS_OAMP(unit, port_type))
            {
                if (count >= internal_ports_max)
                {
                    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "internal_ports_max < number of internal ports (now adding OAMP)\n");
                }
                BCM_GPORT_LOCAL_SET(gport, port);
                internal_gport[count++] = gport;
            }
        }
    }

    *internal_ports_count = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_detach
 */

static shr_error_e
dnx_port_detach_verify(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached)
{
    int fabric_port_base;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(detached, _SHR_E_PARAM, "detached_ports");

    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);

    BCM_PBMP_ITER(pbmp, port)
    {
        if (port < fabric_port_base)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "bcm_port_detach API is only supported for Fabric ports. please see bcm_port_remove.\n");
        }

        if (!IS_SFI_PORT(unit, port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is not probed. Can't detach ports that aren't probed.\n", port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Detach all ports in pbmp bitmap. 
 * 
 * \param [in] unit - chip unit id.
 * \param [in] pbmp - bitmap of ports to be detached
 * \param [out] detached - ports actually detached
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * This method is not supported for NIF ports,
 *   see bcm_port_remove instead.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_detach_verify(unit, pbmp, detached));

    SHR_IF_ERR_EXIT(dnx_fabric_if_port_detach(unit, pbmp, detached));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_duplex_set
 *     bcm_dnx_port_duplex_get
 */

static shr_error_e
dnx_port_duplex_verify(
    int unit,
    bcm_port_t port,
    int *duplex)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(duplex, _SHR_E_PARAM, "duplex_mode");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the port duplex mode. 
 * 0 => Half duplex 
 * 1 => Full duplex 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] duplex - duplex mode.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * BCM_PORT_DUPLEX_*
 */
shr_error_e
bcm_dnx_port_duplex_get(
    int unit,
    bcm_port_t port,
    int *duplex)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_duplex_verify(unit, port, duplex));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /*
     *Not supported for Fabric ports - IMB will return an error. 
     */
    SHR_IF_ERR_EXIT(imb_port_duplex_get(unit, logical_port, duplex));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the port duplex mode. 
 * 0 => Half duplex 
 * 1 => Full duplex 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] duplex - duplex mode.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * BCM_PORT_DUPLEX_*
 */
shr_error_e
bcm_dnx_port_duplex_set(
    int unit,
    bcm_port_t port,
    int duplex)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_duplex_verify(unit, port, &duplex));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /*
     *Not supported for Fabric ports - IMB will return an error. 
     */
    SHR_IF_ERR_EXIT(imb_port_duplex_set(unit, logical_port, duplex));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_enable_set
 *     bcm_dnx_port_enable_get
 */

static shr_error_e
dnx_port_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve enable status for the port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_enable_verify(unit, port, enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable a port
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in] enable - enable indication
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_enable_verify(unit, port, &enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, logical_port, enable));

    /*
     * Note: in Jericho, if MAC loopback is enabled, linkscan would force the link up. 
     * from now on, linkscan should not force the link in case of MAC loopback. 
     */
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_frame_max_set
 *     bcm_dnx_port_frame_max_get
 */

static shr_error_e
dnx_port_frame_max_verify(
    int unit,
    bcm_port_t port,
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(size, _SHR_E_PARAM, "max_packet_size");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    if (*size < 0 || *size > dnx_data_nif.eth.packet_size_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong max frame size %d for port %d\r\n", *size, port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve max packet size supported 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] size - max packet size
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_frame_max_get(
    int unit,
    bcm_port_t port,
    int *size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_frame_max_verify(unit, port, size));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_max_packet_size_get(unit, logical_port, size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set max packet size to support 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] size - max packet size
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_frame_max_set(
    int unit,
    bcm_port_t port,
    int size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_frame_max_verify(unit, port, &size));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_max_packet_size_set(unit, logical_port, size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_ifg_set
 *     bcm_dnx_port_ifg_get
 */

static shr_error_e
dnx_port_ifg_verify(
    int unit,
    bcm_port_t port,
    bcm_port_duplex_t duplex,
    int *bit_times)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bit_times, _SHR_E_PARAM, "bit_times");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    if (duplex != BCM_PORT_DUPLEX_FULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only Full Duplex Mode supported");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve inter-frame gap for the port.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] speed - port speed
 * \param [in] duplex - port duplex mode (half / full)
 * \param [out] bit_times - inter-frame gap in bit-times
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ifg_get(
    int unit,
    bcm_port_t port,
    int speed,
    bcm_port_duplex_t duplex,
    int *bit_times)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_ifg_verify(unit, port, duplex, bit_times));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_tx_average_ipg_get(unit, logical_port, bit_times));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set inter-frame gap for the port.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] speed - port speed
 * \param [in] duplex - port duplex mode (half / full)
 * \param [in] bit_times - inter-frame gap in bit-times
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ifg_set(
    int unit,
    bcm_port_t port,
    int speed,
    bcm_port_duplex_t duplex,
    int bit_times)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_ifg_verify(unit, port, duplex, &bit_times));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_tx_average_ipg_set(unit, logical_port, bit_times));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_link_state_get
 */

static shr_error_e
dnx_port_link_state_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_link_state_t * state)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(state, _SHR_E_PARAM, "link_state");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve port link state
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags (currently not in use)
 * \param [in] state - link state info.
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_link_state_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_port_link_state_t * state)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input parameters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_link_state_get_verify(unit, port, state));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_link_state_get(unit, logical_port, 1, state));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_loopback_get
 *     bcm_dnx_port_loopback_set
 */

static shr_error_e
dnx_port_loopback_verify(
    int unit,
    bcm_port_t port,
    int *loopback,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(loopback, _SHR_E_PARAM, "loopback");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    if (is_set)
    {
        /*
         * Verify the Loopback Mode
         */
        switch (*loopback)
        {
            case BCM_PORT_LOOPBACK_NONE:
            case BCM_PORT_LOOPBACK_MAC:
            case BCM_PORT_LOOPBACK_PHY:
            case BCM_PORT_LOOPBACK_PHY_REMOTE:
            case BCM_PORT_LOOPBACK_MAC_REMOTE:
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Loopback type %d is not supported.\r\n", *loopback);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve loopback status for the port
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] loopback - loopback mode. see 
 *        bcm_port_loopback_t
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_loopback_verify(unit, port, loopback, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, logical_port, loopback));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set loopback for the port
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] loopback - loopback mode. see bcm_port_loopback_t
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    bcm_port_t logical_port = 0;
    bcm_port_resource_t resource;
    int current_loopback = BCM_PORT_LOOPBACK_NONE;
    /*
     * uint32 cl72_start;
     */
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_loopback_verify(unit, port, &loopback, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /*
     * Disable CL72 when closing loopback 
     */
    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, logical_port, &current_loopback));
    if ((loopback != BCM_PORT_LOOPBACK_NONE) && (current_loopback == BCM_PORT_LOOPBACK_NONE))
    {
        /*
         * Store original link training state
         */
        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, logical_port, &resource));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_loopback_original_link_training_set
                        (unit, logical_port, resource.link_training));

        /*
         * Disable link training
         */
        resource.link_training = 0;
        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_set(unit, logical_port, &resource));
    }

    SHR_IF_ERR_EXIT(imb_port_loopback_set(unit, logical_port, loopback));

    /*
     * Note: in Jericho, if MAC loopback is enabled, linkscan would force the link up. 
     * from now on, linkscan should not force the link in case of MAC loopback. 
     */

    if ((loopback == BCM_PORT_LOOPBACK_NONE) && (current_loopback != BCM_PORT_LOOPBACK_NONE))
    {
        /*
         * Restore original link training state
         */
        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, logical_port, &resource));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_loopback_original_link_training_get
                        (unit, logical_port, &resource.link_training));
        SHR_IF_ERR_EXIT(bcm_dnx_port_resource_set(unit, logical_port, &resource));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Auto MDIX mode for a port. only relevant for 
 *        external phy, hence not supported.
 */
shr_error_e
bcm_dnx_port_mdix_get(
    int unit,
    bcm_port_t port,
    bcm_port_mdix_t * mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_mdix_get API is only supported for external Phys.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Auto MDIX mode for a port. only relevant for 
 *        external phy, hence not supported.
 */
shr_error_e
bcm_dnx_port_mdix_set(
    int unit,
    bcm_port_t port,
    bcm_port_mdix_t mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_mdix_set API is only supported for external Phys.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Auto MDIX status for a port. only relevant for 
 *        external phy, hence not supported.
 */
shr_error_e
bcm_dnx_port_mdix_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_mdix_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_mdix_status_gete API is only supported for external Phys.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_port_probe() API
 */
static shr_error_e
dnx_port_probe_verify(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp)
{
    bcm_port_t logical_port = 0;
    int fabric_port_base = 0;
    int nof_fabric_links = 0;
    bcm_pbmp_t fabric_ports_bitmap;

    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(okay_pbmp, _SHR_E_PARAM, "okay_pbmp");

    if (BCM_PBMP_IS_NULL(pbmp))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "pbmp is empty. Need to have at least 1 port to probe.\n");
    }

    BCM_PBMP_CLEAR(fabric_ports_bitmap);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));

    /** Fabric ports range check */
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);

    BCM_PBMP_ITER(pbmp, logical_port)
    {
        if (logical_port < fabric_port_base)
        {
            SHR_ERR_EXIT(_SHR_E_PORT,
                         "bcm_port_probe API is only supported for Fabric ports. please see bcm_port_add.\n");
        }

        if (logical_port >= fabric_port_base + nof_fabric_links)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Fabric logical port is out of range %d. Max is %d.\n", logical_port,
                         fabric_port_base + nof_fabric_links - 1);
        }

        /** Validate port is not already probed */
        if (BCM_PBMP_MEMBER(fabric_ports_bitmap, logical_port))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d is already probed.\n", logical_port);
        }

        /** Validate lane map */
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_port_add_verify
                        (unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, DNX_ALGO_PORT_TYPE_FABRIC, 0,
                         (logical_port - fabric_port_base)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Attach all ports in pbmp bitmap
 * 
 * \param [in] unit - chip unit id.
 * \param [in] pbmp - bitmap of ports to be added 
 * \param [in] okay_pbmp - ports actually added
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * This method is not supported for NIF ports,
 *   see bcm_port_add instead.
 * \see
 *   * bcm_port_add
 */
shr_error_e
bcm_dnx_port_probe(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_probe_verify(unit, pbmp, okay_pbmp));

    SHR_IF_ERR_EXIT(dnx_fabric_if_port_probe(unit, pbmp, okay_pbmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following function:
 *     bcm_dnx_port_priority_config_get
 *     bcm_dnx_port_priority_config_set
 */

shr_error_e
dnx_port_priority_config_verify(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config,
    int is_set)
{
    bcm_port_t logical_port = 0;
    int enable;
    uint8 is_fixed_priority;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(priority_config, _SHR_E_PARAM, "priority_config");

    /*
     * Get local port from port
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    if (is_set)
    {
        /*
         * check if the port is disabled
         */
        SHR_IF_ERR_EXIT(imb_port_enable_get(unit, logical_port, &enable));
        if (enable)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "bcm_port_priority_config_set API should be called only for disabled ports. please disable port %d.\n",
                         logical_port);
        }

        /*
         * check if there is a limitaion on changing priority 
         */
        SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_get(unit, logical_port, &is_fixed_priority));
        if (is_fixed_priority)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Priority config can't be changed for port %d. Priority can be change only before creating src VSQ and/or setting port compensation\n",
                         logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Returns per_priority_exist array 
 * such that per_priority_exist[i] = 1 iff priority "i" exists  in the priority_config
 * 
 */
static int
dnx_port_priority_config_priority_exist_get(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config,
    int per_priority_exist[])
{
    int ii, nof_prio_groups;

    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups = priority_config->nof_priority_groups;

    for (ii = 0; ii < DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF; ++ii)
    {
        /** initialize exist array -- all priorities do not exist */
        per_priority_exist[ii] = 0;
    }

    for (ii = 0; ii < nof_prio_groups; ++ii)
    {
        /** set priority, if exists in the user config */
        per_priority_exist[priority_config->priority_groups[ii].sch_priority] = 1;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the priority groups for the port. each 
 *        priority group represent a logical FIFO.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups info
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_priority_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    bcm_port_t logical_port = 0;
    int per_priority_exist[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF];

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_priority_config_verify(unit, port, priority_config, 1 /* is_set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_priority_config_set(unit, logical_port, priority_config));

    /*
     * configure reassembly context for the new / changed priorities
     */

    SHR_IF_ERR_EXIT(dnx_port_priority_config_priority_exist_get
                    (unit, logical_port, priority_config, per_priority_exist));

    /** set context for existing priorities */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_per_priority_context_set(unit, logical_port, per_priority_exist));
    /** unset context for non existing priorities */
    SHR_IF_ERR_EXIT(dnx_port_ingr_reassembly_per_priority_context_unset(unit, logical_port, per_priority_exist));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get priority groups information for the port.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups info
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_priority_config_verify(unit, port, priority_config, 0 /* is_set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_priority_config_get(unit, logical_port, priority_config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_nif_priority_get(
    int unit,
    bcm_gport_t local_port,
    uint32 flags,
    bcm_port_nif_prio_t * priority,
    bcm_pbmp_t * affected_ports)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_nif_priority_get API is no longer supported. Please see bcm_port_priority_config_get instead.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_nif_priority_set(
    int unit,
    bcm_gport_t local_port,
    uint32 flags,
    bcm_port_nif_prio_t * priority,
    bcm_pbmp_t * affected_ports)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_nif_priority_set API is no longer supported. Please see bcm_port_priority_config_set instead.\n");
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_resource_lane_config_soc_properties_get(
    int unit,
    bcm_port_t port,
    bcm_port_resource_t * port_resource)
{
    soc_dnxc_port_dfe_mode_t dfe;
    int media_type;
    int unreliable_los;
    int cl72_auto_polarity_enable;
    int cl72_restart_timeout_enable;
    soc_dnxc_port_channel_mode_t channel_mode;
    SHR_FUNC_INIT_VARS(unit);

    dfe = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->dfe;
    /** if dfe SoC property was configured */
    if (dfe != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (dfe)
        {
            case soc_dnxc_port_dfe_on:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(port_resource->phy_lane_config);
                break;
            }
            case soc_dnxc_port_dfe_off:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_CLEAR(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(port_resource->phy_lane_config);
                break;
            }
            case soc_dnxc_port_lp_dfe:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_SET(port_resource->phy_lane_config);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid serdes_lane_config_dfe SoC property value. Allowed values are on|off|lp",
                             port);
            }
        }
    }

    media_type = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->media_type;
    /*
     * if media type SoC property was configured
     */
    if (media_type != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (media_type)
        {
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE:
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE:
            case BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(port_resource->phy_lane_config, media_type);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid serdes_lane_config_media_type SoC property value. Allowed values are backplane|copper|optics",
                             port);
            }
        }
    }

    unreliable_los = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->unreliable_los;
    /*
     * if unreliable los SoC property was configured
     */
    if (unreliable_los != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (unreliable_los)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(port_resource->phy_lane_config);
        }
    }

    cl72_auto_polarity_enable = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->cl72_auto_polarity_enable;
    /*
     * if cl72 auto polarity SoC property was configured
     */
    if (cl72_auto_polarity_enable != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (cl72_auto_polarity_enable)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(port_resource->phy_lane_config);
        }
    }

    cl72_restart_timeout_enable =
        dnx_data_port.static_add.serdes_lane_config_get(unit, port)->cl72_restart_timeout_enable;
    /*
     * if cl72 restart timeout SoC property was configured
     */
    if (cl72_restart_timeout_enable != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        if (cl72_restart_timeout_enable)
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(port_resource->phy_lane_config);
        }
        else
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_CLEAR(port_resource->phy_lane_config);
        }
    }

    channel_mode = dnx_data_port.static_add.serdes_lane_config_get(unit, port)->channel_mode;
    /*
     * if channel mode SoC property was configured
     */
    if (channel_mode != DNXC_PORT_SERDES_LANE_CONFIG_INVALID_VAL)
    {
        switch (channel_mode)
        {
            case soc_dnxc_port_force_nr:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(port_resource->phy_lane_config);
                break;
            }
            case soc_dnxc_port_force_er:
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(port_resource->phy_lane_config);
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_SET(port_resource->phy_lane_config);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "port %d: Invalid serdes_lane_config_channel_mode SoC property value. Allowed values are force_nr|force_er",
                             port);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the parameters for the following functions:
 *     bcm_dnx_port_resource_set
 *     bcm_dnx_port_resource_get
 *     bcm_dnx_port_resource_default_get
 */

static shr_error_e
dnx_port_resource_verify(
    int unit,
    bcm_port_t port,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate input params for bcm_dnx_port_resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_verify(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * validation
     */
    if (nport < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "nport %d is invalid", nport);
    }

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, resource[resource_index].port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate if the FEC + Speed combination is supported.
 *   This function only validates part of FEC + Speed combinations.
 *   portmod will validate the rest FEC + Speed combinations.
 */
static shr_error_e
dnx_port_resource_fec_validate(
    int unit,
    bcm_port_resource_t * resource)
{
    int nof_lanes;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, resource->port, &nof_lanes));

    if ((resource->fec_type == bcmPortPhyFecRs544 && resource->speed == 200000) ||
        (resource->fec_type == bcmPortPhyFecNone
         && (resource->speed == 200000 || (resource->speed == 100000 && nof_lanes == 2))))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: The combination of FEC type %d, speed %d and lane numbers %d is not supported! \n ",
                     resource->port, resource->fec_type, resource->speed, nof_lanes);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the 'resource' struct members for bcm_dnx_port_resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_resource_verify(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    dnx_algo_port_type_e port_type = -1;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert user's gports to logical ports.
     * User's gports will be restored at the end of bcm_dnx_port_resource_multi_set.
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource[resource_index].port, &resource[resource_index].port));
    }

    /*
     * basic validation that given resource configurations are legal per port
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, resource[resource_index].port, &port_type));
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type))
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_resource_validate(unit, &resource[resource_index]));
            /*
             * Validate the FEC+SPEED combinations for NIF ETH ports
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE))
            {
                SHR_IF_ERR_EXIT(dnx_port_resource_fec_validate(unit, &resource[resource_index]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Fill ports_info according to resource.
 *   ports_info.original_port was already filled in
 *   bcm_dnx_port_resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_port_info_get(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int resource_index = 0;
    int port_enable = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        /*
         * Per port type handling
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get
                        (unit, resource[resource_index].port, &ports_info[resource_index].port_type));

        /*
         * Check if this is the first invocation of resource_set for this interface
         */
        if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, ports_info[resource_index].port_type))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed
                            (unit, resource[resource_index].port, &ports_info[resource_index].is_has_speed));
        }

        /*
         * Save all enabled ports.
         * The enabled ports will be disabled during the resource set,
         * and will be re-enabled at the end.
         */
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, ports_info[resource_index].port_type))
        {
            /** store enable state of the port */
            SHR_IF_ERR_EXIT(imb_port_enable_get(unit, resource[resource_index].port, &port_enable));
            ports_info[resource_index].is_port_enable = port_enable;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure needed modules after resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_set_modules_config_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int resource_index = 0;
    dnx_algo_port_type_e port_type;
    dnx_algo_port_tdm_mode_e tdm_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Set Port Bitmaps
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        tdm_mode = DNX_ALGO_PORT_TDM_MODE_NONE;
        if (DNX_ALGO_PORT_TYPE_IS_TM(unit, ports_info[resource_index].port_type))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, resource[resource_index].port, &tdm_mode));
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, resource[resource_index].port, &port_type));
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set
                        (unit, resource[resource_index].port, port_type, tdm_mode));
    }

    /*
     * E2E SCH ports config
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, ports_info[resource_index].port_type))
        {
            /*
             * if this is first resource_set on the interface
             * color all ports under the interface according to interface speed
             */
            if (ports_info[resource_index].is_has_speed == 0)
            {
                SHR_IF_ERR_EXIT(dnx_sch_port_interface_ports_color(unit, resource[resource_index].port));
            }
        }
    }

    /*
     * EGR TM ports config
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_type))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_if_thr_set(unit, resource[resource_index].port));
        }
    }
    /*
     * dnx_egr_queuing_nif_calendar_set is a configuration for all EGR TM
     * ports, and not per port.
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_type))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_calendar_set(unit, BCM_CORE_ALL));
            break;
        }
    }

    /*
     * Fabric ports config
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, ports_info[resource_index].port_type))
        {
            /*
             * ALDWP should be configured after init time for every fabric speed change.
             * Avoiding ALDWP config at init time.
             */
            if (dnx_init_is_init_done_get(unit) == TRUE)
            {
                SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_aldwp_config, (unit)));
            }
            break;
        }
    }

    /*
     *  Config start TX threshold
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_type, FALSE))
        {
            SHR_IF_ERR_EXIT(imb_port_tx_start_thr_set
                            (unit, resource[resource_index].port, resource[resource_index].speed));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Tune needed modules after resource_multi_set.
 */
static shr_error_e
dnx_port_resource_multi_tune(
    int unit,
    int nport,
    bcm_port_resource_t * resource,
    dnx_port_resource_port_info_t * ports_info)
{
    int resource_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * EGR TM ports tune
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, ports_info[resource_index].port_type))
        {
            SHR_IF_ERR_EXIT(dnx_ecgm_tune_port_init
                            (unit, resource[resource_index].port, resource[resource_index].speed));
        }
    }

    /*
     * Configure CDU scheduler
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index].port_type, FALSE))
        {
            SHR_IF_ERR_EXIT(imb_port_scheduler_config_set(unit, resource[resource_index].port, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_resource_multi_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    int resource_index = 0;
    dnx_port_resource_port_info_t *ports_info = NULL;
    int counter_interval;
    uint32 counter_flags;
    bcm_pbmp_t counter_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Input validation
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_resource_multi_set_verify(unit, nport, resource));

    /*
     * Save user's ports which reside in the 'resource' struct.
     * These ports will be overriden by their logical ports, and will
     * be restored back to the 'resource' struct at the end of this function.
     *
     * This solution is quite ugly, but it needed since we shouldn't change the
     * user's input 'resource' param.
     */
    SHR_ALLOC_SET_ZERO(ports_info, nport * sizeof(dnx_port_resource_port_info_t), "User resource ports info",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        ports_info[resource_index].original_port = resource[resource_index].port;
    }

    /*
     * Validate 'resource' members
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_resource_multi_set_resource_verify(unit, nport, resource));

    /*
     * Get port info prior to calling resource_multi_set 
     */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_port_info_get(unit, nport, resource, ports_info));

    /*
     * Validate PHY loopback is not set on fabric ports
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, ports_info[resource_index].port_type))
        {
            int loopback;
            SHR_IF_ERR_EXIT(bcm_dnx_port_loopback_get(unit, resource[resource_index].port, &loopback));

            if (loopback == BCM_PORT_LOOPBACK_PHY)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Can't configure resource while PHY loopback is set on port %d.",
                             resource[resource_index].port);
            }
        }
    }
    /*
     * Get counter pbmp,
     */
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
    /*
     * Disable ports. All enabled ports will be re-enabled at the end
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, ports_info[resource_index].port_type))
        {
            /*
             * Remove port from counter thread.
             */
            if (BCM_PBMP_MEMBER(counter_pbmp, resource[resource_index].port))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, resource[resource_index].port, 0));
            }
            if (ports_info[resource_index].is_port_enable == 1)
            {
                /** disable port */
                SHR_IF_ERR_EXIT(imb_port_enable_set(unit, resource[resource_index].port, 0));
            }
        }
    }

    /*
     * Configure all resources.
     */
    SHR_IF_ERR_EXIT(imb_common_port_resource_multi_set(unit, nport, resource));

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        /** set speed in mgmt db */
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_set(unit, resource[resource_index].port, resource[resource_index].speed));
    }

    /*
     * Run per module configurations that are needed after calling resource_multi_set
     */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_set_modules_config_set(unit, nport, resource, ports_info));

    /*
     * Run per module tunnings that are needed after calling resource_multi_set
     */
    SHR_IF_ERR_EXIT(dnx_port_resource_multi_tune(unit, nport, resource, ports_info));

    /*
     * Re-enable ports that was previously enabled
     */
    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, ports_info[resource_index].port_type))
        {
            if (ports_info[resource_index].is_port_enable == 1)
            {
                /** enable port */
                SHR_IF_ERR_EXIT(imb_port_enable_set(unit, resource[resource_index].port, 1));
            }
            /*
             * Restore port to counter thread.
             */
            if (BCM_PBMP_MEMBER(counter_pbmp, resource[resource_index].port))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, resource[resource_index].port, 1));
            }
        }
    }

exit:
    if (ports_info != NULL)
    {
        /** Restore user's ports configurations */
        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            resource[resource_index].port = ports_info[resource_index].original_port;
        }
    }

    SHR_FREE(ports_info);
    SHR_FUNC_EXIT;
}

/** See .h file */
int
bcm_dnx_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    bcm_gport_t resource_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Input validation
     */
    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if (resource->port != port && resource->port != 0 && resource->port != -1)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "resource->port must be equal to port");
    }

    /*
     * 'port' appear twice - in 'port' param and in resource->port.
     * They should be identical, but in case they not we change resource->port
     * to be the same as 'port'. At the end of the function we restore it since
     * we shouldn't change user's params.
     */
    resource_port = resource->port;

    /** Override resource->port (will be restored at end of function) */
    resource->port = port;

    /*
     * Call resource_multi_set with 1 port.
     * All further validations will be done inside.
     */
    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, 1, resource));

exit:
    resource->port = resource_port;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_type_e port_type;
    int has_speed = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_resource_verify(unit, port, resource));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    bcm_port_resource_t_init(resource);

    /*
     * Get resource info only if speed has been configured.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, &has_speed));
    if (has_speed)
    {
        /*
         * Per port type handling
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));

        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type))
        {
            SHR_IF_ERR_EXIT(imb_common_port_resource_get(unit, logical_port, resource));

            /*
             * The params 'port' and 'resource->port' should be equal,
             * so if the user gave fabric gport as input - we should
             * return a fabric gport too.
             */
            if (BCM_GPORT_IS_LOCAL_FABRIC(port))
            {
                int link_id;

                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));
                BCM_GPORT_LOCAL_FABRIC_SET(resource->port, link_id);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &resource->speed));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify input paramters
     */
    SHR_INVOKE_VERIFY_DNX(dnx_port_resource_verify(unit, port, resource));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_resource_default_get(unit, logical_port, flags, resource));

    /*
     * The params 'port' and 'resource->port' should be equal,
     * so if the user gave fabric gport as input - we should
     * return a fabric gport too.
     */
    if (BCM_GPORT_IS_LOCAL_FABRIC(port))
    {
        int link_id;

        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));
        BCM_GPORT_LOCAL_FABRIC_SET(resource->port, link_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_remote_fault_status_clear()/get() and
 *          dnx_port_local_fault_status_clear()/get()API
 */
static shr_error_e
dnx_port_remote_local_fault_verify(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get remote fault status
 */
static shr_error_e
dnx_port_remote_fault_status_get(
    int unit,
    int port,
    int *value)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_status_get(unit, logical_port, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get remote fault status
 */
static shr_error_e
dnx_port_local_fault_status_get(
    int unit,
    int port,
    int *value)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_status_get(unit, logical_port, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear remote fault status
 */
static shr_error_e
dnx_port_remote_fault_status_clear(
    int unit,
    int port)
{
    bcm_port_t logical_port = 0;
    int value;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, &value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_status_clear(unit, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear local fault status
 */
static shr_error_e
dnx_port_local_fault_status_clear(
    int unit,
    int port)
{
    bcm_port_t logical_port = 0;
    int value;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, &value));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_status_clear(unit, logical_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get local fault enable value
 */
static shr_error_e
dnx_port_local_fault_enable_get(
    int unit,
    int port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set local fault enable value
 */
static shr_error_e
dnx_port_local_fault_enable_set(
    int unit,
    int port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, &enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_local_fault_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get remote fault enable value
 */
static shr_error_e
dnx_port_remote_fault_enable_get(
    int unit,
    int port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set remote fault enable value
 */
static shr_error_e
dnx_port_remote_fault_enable_set(
    int unit,
    int port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_remote_local_fault_verify(unit, port, &enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_remote_fault_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_remote_fault_status_clear()/get() and
 *          dnx_port_local_fault_status_clear()/get()API
 */
static shr_error_e
dnx_port_pad_size_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    if ((*value < dnx_data_nif.eth.pad_size_min_get(unit) || *value > dnx_data_nif.eth.pad_size_max_get(unit))
        && (*value != 0) && (is_set))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong padding size %d for port 0x%x\r\n", *value, port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set pad size, TX pad packets to the specified size
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pad_size - pad size (min 64 - max 96)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_pad_size_set(
    int unit,
    bcm_port_t port,
    int pad_size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_pad_size_verify(unit, port, &pad_size, 1));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_pad_size_set(unit, logical_port, pad_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TX pad size value.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] pad_size - pad size (min 64 - max 96)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
dnx_port_pad_size_get(
    int unit,
    bcm_port_t port,
    int *pad_size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_pad_size_verify(unit, port, pad_size, 0));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** call imb layer  */
    SHR_IF_ERR_EXIT(imb_port_pad_size_get(unit, logical_port, pad_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the lane gport is legal
 *
 * \param [in] unit - chip unit id
 * \param [in] port - lane gport
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
dnx_port_lane_gport_verify(
    int unit,
    bcm_port_t port)
{
    int nof_phys;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    /** verify port */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport_info.local_port));
    /** verify lane */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, gport_info.local_port, &nof_phys));
    SHR_RANGE_VERIFY(gport_info.lane, 0, nof_phys - 1, _SHR_E_PARAM, "Lane ID is out of range.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the port type is supported by Prbs function.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
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
dnx_port_prbs_port_verify(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        SHR_IF_ERR_EXIT(dnx_port_lane_gport_verify(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the prbs polynomial value is acceptable.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] value - polynomial value
 * \param [in] is_set - if this fuction is called by set function
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
dnx_port_prbs_polynomial_verify(
    int unit,
    bcm_port_t port,
    bcm_port_prbs_t value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify port */
    SHR_IF_ERR_EXIT(dnx_port_prbs_port_verify(unit, port));

    /** for set API, verify if the poly vaule is correct. */
    if (is_set)
    {
        switch (value)
        {
            case BCM_PORT_PRBS_POLYNOMIAL_X7_X6_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X15_X14_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X23_X18_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X9_X5_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X11_X9_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X58_X31_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X49_X40_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X20_X3_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X13_X12_X2_1:
            case BCM_PORT_PRBS_POLYNOMIAL_X10_X7_1:
            case BCM_PORT_PRBS_POLYNOMIAL_PAM4_13Q:
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Poly value %d is not supported\r\n", value);
        }
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - Verify if the parameters for prbs data inversion
 *    function is acceptable.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [in] invert - enable or disable inversion
 * \param [in] is_set - if this fuction is called by set function
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
dnx_port_prbs_invert_data_verify(
    int unit,
    bcm_port_t port,
    int type,
    int invert,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify port */
    SHR_IF_ERR_EXIT(dnx_port_prbs_port_verify(unit, port));

    /** verify type */
    switch (type)
    {
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Prbs invert type %d is not supported\r\n", type);
    }

    /** for set API, verify if the value is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(invert, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if the parameters for prbs enable
 *    function is acceptable.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [in] value - enable or disable prbs
 * \param [in] is_set - if this fuction is called by set function
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
dnx_port_prbs_enable_verify(
    int unit,
    bcm_port_t port,
    int type,
    int value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify port */
    SHR_IF_ERR_EXIT(dnx_port_prbs_port_verify(unit, port));

    /** verify type */
    switch (type)
    {
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Prbs enable type %d is not supported\r\n", type);
    }

    /** for set API, verify if the value is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(value, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify if the parameters for 'fabric only' enable controls.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable
 * \param [in] is_set - if this fuction is called by set function
 */
static shr_error_e
dnx_port_fabric_control_enable_verify(
    int unit,
    bcm_port_t port,
    int enable,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    /** for set API, verify if the 'enable' is correct. */
    if (is_set)
    {
        SHR_RANGE_VERIFY(enable, 0, 1, _SHR_E_PARAM, "Enable should be 0 or 1");
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set prbs mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port
 * \param [in] prbs_mode - prbs mode value
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
dnx_port_prbs_mode_set(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_set(unit, logical_port, prbs_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port
 * \param [out] prbs_mode - prbs mode value
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
dnx_port_prbs_mode_get(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t * prbs_mode)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, prbs_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set prbs polynomial
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] value - prbs polynomial value
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
dnx_port_prbs_polynomial_set(
    int unit,
    bcm_port_t port,
    bcm_port_prbs_t value)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_polynomial_verify(unit, port, value, 1 /* is set */ ));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL, value));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_polynomial_set(unit, logical_port, prbs_mode, value));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs polynomial
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [out] value - prbs polynomial value
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
dnx_port_prbs_polynomial_get(
    int unit,
    bcm_port_t port,
    bcm_port_prbs_t * value)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_polynomial_verify(unit, port, *value, 0 /* is set */ ));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL, value));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_polynomial_get(unit, logical_port, prbs_mode, value));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Whether enable prbs data inversion
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [in] invert - enable or disable data inversion
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
dnx_port_prbs_invert_data_set(
    int unit,
    bcm_port_t port,
    int type,
    int invert)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_invert_data_verify(unit, port, type, invert, 1 /* is set */ ));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_set
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxInvertData) ? BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA, invert));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_invert_data_set(unit, logical_port, type, prbs_mode, invert));
    }
exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - Get the prbs data inversion status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [out] invert - enable or disable data inversion
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
dnx_port_prbs_invert_data_get(
    int unit,
    bcm_port_t port,
    int type,
    int *invert)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_invert_data_verify(unit, port, type, *invert, 0 /* is set */ ));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxInvertData) ? BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA, (uint32 *) invert));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_invert_data_get(unit, logical_port, type, prbs_mode, invert));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Enable or disable Prbs
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [in] enable - enable or disable prbs
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
dnx_port_prbs_enable_set(
    int unit,
    bcm_port_t port,
    int type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_enable_verify(unit, port, type, enable, 1 /* is set */ ));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_set
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxEnable) ? BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE, enable));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_enable_set(unit, logical_port, type, prbs_mode, enable));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get prbs enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [out] enable - enable or disable prbs
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
dnx_port_prbs_enable_get(
    int unit,
    bcm_port_t port,
    int type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_enable_verify(unit, port, type, *enable, 0 /* is set */ ));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get
                        (unit, port,
                         (type ==
                          bcmPortControlPrbsRxEnable) ? BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE :
                         BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE, (uint32 *) enable));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_enable_get(unit, logical_port, type, prbs_mode, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [out] status - prbs status
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
dnx_port_prbs_rx_status_get(
    int unit,
    bcm_port_t port,
    int *status)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_prbs_port_verify(unit, port));

    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        /** For lane port, use phy control to handle it */
        SHR_IF_ERR_EXIT(dnx_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS, (uint32 *) status));
    }
    else
    {
        bcm_port_t logical_port = 0;
        portmod_prbs_mode_t prbs_mode = portmodPrbsModeCount;

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        /** For local port, call imb API, and then PortMod. For NIF port */
        SHR_IF_ERR_EXIT(dnx_algo_port_prbs_mode_get(unit, logical_port, &prbs_mode));
        SHR_IF_ERR_EXIT(imb_port_prbs_rx_status_get(unit, logical_port, prbs_mode, status));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable or disable low latency llfc.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable low latency llfc
 */
static shr_error_e
dnx_port_low_latency_llfc_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_low_latency_llfc_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get whether low latency llfc is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether low latency llfc is enabled or disabled
 */
static shr_error_e
dnx_port_low_latency_llfc_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_low_latency_llfc_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Enable or disable 'FEC error detect'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'FEC error detect'
 */
static shr_error_e
dnx_port_fec_error_detect_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_fec_error_detect_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get whether 'FEC error detect' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether 'Fec error detect' is enabled or disabled
 */
static shr_error_e
dnx_port_fec_error_detect_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_fec_error_detect_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Enable or disable 'extract LLFC cells congestion indication'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'extract LLFC cells
 *                      congestion indication'
 */
static shr_error_e
dnx_port_llfc_cells_congestion_ind_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_extract_congestion_indication_from_llfc_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get whether 'extract LLFC cells congestion indication' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether 'extract LLFC cells congestion indication'
 *                       is enabled or disabled
 */
static shr_error_e
dnx_port_llfc_cells_congestion_ind_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_extract_congestion_indication_from_llfc_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Enable or disable 'LLFC after FEC'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'LLFC after FEC'
 */
static shr_error_e
dnx_port_llfc_after_fec_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_llfc_after_fec_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get whether 'LLFC after FEC' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - whether 'LLFC after FEC' is enabled or disabled
 */
static shr_error_e
dnx_port_llfc_after_fec_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_llfc_after_fec_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Enable or disable 'FEC Bypass of Control Cells'.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable 'FEC Bypass of Control Cells'
 */
static shr_error_e
dnx_port_control_cells_fec_bypass_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, enable, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_control_cells_fec_bypass_enable_set(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Get whether 'FEC Bypass of Control Cells' is enabled/disabled.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - is 'FEC Bypass of Control Cells' enabled/disabled
 */
static shr_error_e
dnx_port_control_cells_fec_bypass_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_fabric_control_enable_verify(unit, port, *enable, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_control_cells_fec_bypass_enable_get(unit, logical_port, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Verify function for dnx_port_control_stat_oversize_set()/get() API
 */
static shr_error_e
dnx_port_stat_max_size_verify(
    int unit,
    bcm_port_t port,
    int *value,
    int is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));

    if ((*value < 0 || *value > dnx_data_nif.eth.packet_size_max_get(unit)) && (is_set))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, wrong max size %d for port 0x%x\r\n", *value, port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the threshold above which packet will be counted as oversized.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] size - threshold above which packet will be counted as oversized
 */
static shr_error_e
dnx_port_control_stat_oversize_set(
    int unit,
    bcm_port_t port,
    int size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_stat_max_size_verify(unit, port, &size, 1 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_cntmaxsize_set(unit, logical_port, size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the threshold above which packet will be counted as oversized.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] size - threshold above which packet will be counted as oversized
 */
static shr_error_e
dnx_port_control_stat_oversize_get(
    int unit,
    bcm_port_t port,
    int *size)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_stat_max_size_verify(unit, port, size, 0 /* is set */ ));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(imb_port_cntmaxsize_get(unit, logical_port, size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_control_rx_enable_set()/get() API
 */
static shr_error_e
dnx_port_rx_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "Port %d must be of GPORT format. Please use BCM_GPORT_LOCAL_SET for a specific port or BCM_ GPORT_LOCAL_INTERFACE_SET for the entire interface.\r\n",
                     port);
    }

    if (BCM_GPORT_IS_LOCAL(port) && !BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "bcmPortControlRxEnable with BCM_GPORT_LOCAL is for channelized port while RX channels are not supported in this device.\r\n");
    }

    /** NULL checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, gport_info.local_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable/Disable a port's RX direction only.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 */
static shr_error_e
dnx_port_control_rx_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_rx_enable_verify(unit, port, &enable));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, logical_port, PORTMOD_PORT_ENABLE_RX, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve enabling status of a port's RX direction.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 */
static shr_error_e
dnx_port_control_rx_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_rx_enable_verify(unit, port, enable));

    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, logical_port, PORTMOD_PORT_ENABLE_RX, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for dnx_port_control_tx_enable_set()/get() API
 */
static shr_error_e
dnx_port_tx_enable_verify(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT,
                     "Port %d must be in GPORT format. Please use BCM_GPORT_LOCAL_SET for a specific port or BCM_ GPORT_LOCAL_INTERFACE_SET for the entire interface\r\n",
                     port);
    }

    /** NULL checks */
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, gport_info.local_port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable/Disable a port's TX direction only.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 */
static shr_error_e
dnx_port_control_tx_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_tx_enable_verify(unit, port, &enable));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if (BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, gport_info.local_port, PORTMOD_PORT_ENABLE_TX, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, gport_info.local_port, enable, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve enabling status of a port's TX direction.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable indication
 */
static shr_error_e
dnx_port_control_tx_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    bcm_core_t core;
    int base_q_pair;
    uint32 is_queue_disabled;
    SHR_FUNC_INIT_VARS(unit);

    /** verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_port_tx_enable_verify(unit, port, enable));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    if (BCM_GPORT_IS_LOCAL_INTERFACE(port))
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, gport_info.local_port, PORTMOD_PORT_ENABLE_TX, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_get(unit, core, base_q_pair, &is_queue_disabled));
        *enable = (is_queue_disabled ? 0 : 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set various features at the port NIF level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_if_control_get
 */
int
dnx_port_if_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortControlLinkFaultLocal:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_status_clear(unit, port));
            break;
        }
        case bcmPortControlLinkFaultRemote:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_status_clear(unit, port));
            break;
        }
        case bcmPortControlLinkFaultRemoteEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultLocalEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlPadToSize:
        {
            SHR_IF_ERR_EXIT(dnx_port_pad_size_set(unit, port, value));
            break;
        }
        case bcmPortControlPCS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlPCS is no longer supported. Please use bcm_port_resource_set to control FEC\n");
            break;
        }
        case bcmPortControlLinkDownPowerOn:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlLinkDownPowerOn is no longer supported. Please use bcm_port_enable_set instead\n");
            break;
        }
        case bcmPortControlPrbsMode:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_mode_set(unit, port, value));
            break;
        }
        case bcmPortControlPrbsPolynomial:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_polynomial_set(unit, port, value));
            break;
        }
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_invert_data_set(unit, port, type, value));
            break;
        }
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_enable_set(unit, port, type, value));
            break;
        }
        case bcmPortControlLowLatencyLLFCEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_low_latency_llfc_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlFecErrorDetectEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_fec_error_detect_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlLlfcCellsCongestionIndEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_cells_congestion_ind_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlLLFCAfterFecEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_after_fec_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlControlCellsFecBypassEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_cells_fec_bypass_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlStatOversize:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_stat_oversize_set(unit, port, value));
            break;
        }
        case bcmPortControlRxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_rx_enable_set(unit, port, value));
            break;
        }
        case bcmPortControlTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_tx_enable_set(unit, port, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_control_t type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get various features at the port NIF level.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - Device or logical port number.
 * \param [in] type - Port feature enumerator, see /bcm_port_control_t
 * \param [in] *value - Value of the bit field in port table
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 * \see
 *   dnx_port_if_control_set
 */
int
dnx_port_if_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortControlLinkFaultLocal:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_status_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultRemote:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_status_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultRemoteEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_remote_fault_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlLinkFaultLocalEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_local_fault_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlPadToSize:
        {
            SHR_IF_ERR_EXIT(dnx_port_pad_size_get(unit, port, value));
            break;
        }
        case bcmPortControlPCS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlPCS is no longer supported. Please use bcm_port_resource_get to control FEC\n");
            break;
        }
        case bcmPortControlLinkDownPowerOn:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "bcmPortControlLinkDownPowerOn is no longer supported. Please use bcm_port_enable_get instead\n");
            break;
        }
        case bcmPortControlPrbsMode:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_mode_get(unit, port, (portmod_prbs_mode_t *) value));
            break;
        }
        case bcmPortControlPrbsPolynomial:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_polynomial_get(unit, port, (bcm_port_prbs_t *) value));
            break;
        }
        case bcmPortControlPrbsRxInvertData:
        case bcmPortControlPrbsTxInvertData:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_invert_data_get(unit, port, type, value));
            break;
        }
        case bcmPortControlPrbsRxEnable:
        case bcmPortControlPrbsTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_enable_get(unit, port, type, value));
            break;
        }
        case bcmPortControlPrbsRxStatus:
        {
            SHR_IF_ERR_EXIT(dnx_port_prbs_rx_status_get(unit, port, value));
            break;
        }
        case bcmPortControlLowLatencyLLFCEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_low_latency_llfc_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlFecErrorDetectEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_fec_error_detect_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlLlfcCellsCongestionIndEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_cells_congestion_ind_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlLLFCAfterFecEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_llfc_after_fec_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlControlCellsFecBypassEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_cells_fec_bypass_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlStatOversize:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_stat_oversize_get(unit, port, value));
            break;
        }
        case bcmPortControlRxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_rx_enable_get(unit, port, value));
            break;
        }
        case bcmPortControlTxEnable:
        {
            SHR_IF_ERR_EXIT(dnx_port_control_tx_enable_get(unit, port, value));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "dnx_port_if_control_get type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_fault_get(
    int unit,
    bcm_port_t port,
    uint32 *flags)
{
    bcm_port_t logical_port = 0;
    int value;
    SHR_FUNC_INIT_VARS(unit);

    /** reset flags */
    *flags = 0;

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

    /** verify function is called insde dnx_port_if_control */

    SHR_IF_ERR_EXIT(dnx_port_if_control_get(unit, logical_port, bcmPortControlLinkFaultRemote, &value));
    if (value)
    {
        *flags |= BCM_PORT_FAULT_REMOTE;
    }

    SHR_IF_ERR_EXIT(dnx_port_if_control_get(unit, logical_port, bcmPortControlLinkFaultLocal, &value));
    if (value)
    {
        *flags |= BCM_PORT_FAULT_LOCAL;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_lane_to_serdes_map_get and
 *       bcm_dnx_port_lane_to_serdes_map_set API.
 * 
 * \param [in] unit - Unit ID
 * \param [in] flags - see BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE \n
 *          Supported flags:
 *                - 0  -  configure lane map for NIF side.
 *                - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE  -  configure lane map for Fabric side. 
 *
 * \param [in] map_size - Max lane numbers for NIF and Fabric.
 * \param [in] serdes_map - lane to serdes mapping info.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_lane_to_serdes_map_verify(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Null checks */
    SHR_NULL_CHECK(serdes_map, _SHR_E_PARAM, "serdes_map");

    /** Flags */
    SHR_MASK_VERIFY(flags, DNX_PORT_LANE_MAP_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    /** Verify the map size */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_size_verify(unit, DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags), map_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Lane to Serdes Mapping for NIF and Fabric.
 *         Each lane should be both Rx and Tx mapped, or NOT mapped.
 *
 *         For NiF ports, the lane mapping is allowed within a same octet. 
 *         For Fabric ports, the lane mapping is allowed within a same quartet,
 *         The Rx lane mapping should be one to one mapping for Fabric ports.
 * 
 * \param [in] unit - Unit ID
 * \param [in] flags - see BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE \n
 *          Supported flags:
 *                - 0  -  configure lane map for NIF side.
 *                - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE  -  configure lane map for Fabric side. 
 *
 * \param [in] map_size - Max lane numbers for NIF and Fabric.
 * \param [in] serdes_map - lane to serdes mapping info.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_lane_to_serdes_map_set(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int lane_id;
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_lane_to_serdes_map_verify(unit, flags, map_size, serdes_map));

    SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (lane_id = 0; lane_id < map_size; ++lane_id)
    {
        lane2serdes[lane_id].rx_id = serdes_map[lane_id].serdes_rx_id;
        lane2serdes[lane_id].tx_id = serdes_map[lane_id].serdes_tx_id;
    }

    type = DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_set(unit, type, map_size, lane2serdes));

exit:
    SHR_FREE(lane2serdes);
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Lane to Serdes Mapping for NIF and Fabric.
 * 
 * \param [in] unit - Unit ID
 * \param [in] flags - see BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE \n
 *          Supported flags:
 *                - 0  -  configure lane map for NIF side.
 *                - BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE  -  configure lane map for Fabric side. 
 *
 * \param [in] map_size - Max lane numbers for NIF and Fabric.
 * \param [out] serdes_map - lane to serdes mapping info.
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_lane_to_serdes_map_get(
    int unit,
    int flags,
    int map_size,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int lane_id;
    dnx_algo_lane_map_type_e type;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_lane_to_serdes_map_verify(unit, flags, map_size, serdes_map));

    SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    type = DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit, type, map_size, lane2serdes));

    for (lane_id = 0; lane_id < map_size; ++lane_id)
    {
        serdes_map[lane_id].serdes_rx_id = lane2serdes[lane_id].rx_id;
        serdes_map[lane_id].serdes_tx_id = lane2serdes[lane_id].tx_id;
    }

exit:
    SHR_FREE(lane2serdes);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_ilkn_lane_id_map_set and
 *       bcm_dnx_port_ilkn_lane_id_map_get API.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] num_of_lanes - lane numbers for the ilkn port
 * \param [in] ilkn_lane_id - logical lane remapping info.
 * \param [in] is_set - if the verify function is called by set API
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_ilkn_lane_id_map_verify(
    int unit,
    bcm_port_t port,
    int num_of_lanes,
    int *ilkn_lane_id,
    int is_set)
{
    bcm_pbmp_t lane_id_bmp;
    int nof_phys, i;

    SHR_FUNC_INIT_VARS(unit);

    /** Null checks */
    SHR_NULL_CHECK(ilkn_lane_id, _SHR_E_PARAM, "ilkn_lane_id");

    /** Verify the port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    /** Verify lane numbers */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));
    SHR_VAL_VERIFY(num_of_lanes, nof_phys, _SHR_E_PARAM, "Expect num_of_lanes is equal to the phys in this ILKN port");

    /*
     * Verify the ILKN lane id value for set function
     */
    if (is_set)
    {
        BCM_PBMP_CLEAR(lane_id_bmp);
        for (i = 0; i < num_of_lanes; ++i)
        {
            /** Verify lane id is not out of range */
            SHR_RANGE_VERIFY(ilkn_lane_id[i], 0, num_of_lanes - 1, _SHR_E_PARAM, "ILKN id is out of range.\n");
            /** Verify there is no double mapping */
            if (BCM_PBMP_MEMBER(lane_id_bmp, ilkn_lane_id[i]))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "there is double mapping for lane %d\n", i);
            }
            BCM_PBMP_PORT_ADD(lane_id_bmp, ilkn_lane_id[i]);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remap the logical lane order for ILKN ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - support flags
 * \param [in] num_of_lanes - lane numbers for the ilkn port
 * \param [in] ilkn_lane_id - logical lane remapping info.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_ilkn_lane_id_map_set(
    int unit,
    int flags,
    bcm_port_t port,
    int num_of_lanes,
    int *ilkn_lane_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ilkn_lane_id_map_verify(unit, port, num_of_lanes, ilkn_lane_id, 1 /* is_set */ ));
    SHR_IF_ERR_EXIT(imb_port_logical_lane_order_set(unit, port, ilkn_lane_id, num_of_lanes));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the logical lane order for ILKN ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - support flags
 * \param [in] num_of_lanes - lane numbers for the ilkn port
 * \param [in] ilkn_lane_id - logical lane remapping info.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_ilkn_lane_id_map_get(
    int unit,
    int flags,
    bcm_port_t port,
    int num_of_lanes,
    int *ilkn_lane_id)
{
    int actual_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_ilkn_lane_id_map_verify(unit, port, num_of_lanes, ilkn_lane_id, 0 /* is_set */ ));
    SHR_IF_ERR_EXIT(imb_port_logical_lane_order_get(unit, port, num_of_lanes, ilkn_lane_id, &actual_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_phy_control_get/set.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - phy control types
 * \param [in] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_port_phy_control_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Null checks */
    SHR_NULL_CHECK(value, _SHR_E_PARAM, "PHY control value");

    /** verify port. supported port type is lane port or logical local port */
    if (BCM_GPORT_IS_SET(port))
    {
        if (!BCM_PHY_GPORT_IS_LANE(port) && !BCM_GPORT_IS_LOCAL_FABRIC(port))
        {
            SHR_ERR_EXIT(_SHR_E_PORT, "Port: 0x%x:port type is not supported.\r\n", port);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
    }
    /*
     * Sepcial handle for PRBs function, as port control PRBs function needs to
     * call the soc layer PRBs function to deal with Per Lane Gport.
     * Therefore we must ensure the PRBs is supported by soc layer common API.
     */
    switch (type)
    {
        case BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "PHY control type %d is not supported, please use bcm_port_control_set/get instead.\r\n",
                         type);
            break;
        }
        /** The soc layer will verify the rest types */
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Common PHY control handle function.
 *    It can be shared by PHY control and Port contorl
 *    API.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [out] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

static int
dnx_port_phy_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info));
    /*
     * Coverity note: The verify function dnx_port_phy_control_verify can ensure the local_port here is
     * a valid port.
     */
    /*
     * coverity[overrun-call] 
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_get(unit, gport_info.local_port, -1, gport_info.lane,
                                                  gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE,
                                                  (soc_phy_control_t) type, value));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Common PHY control handle function.
 *    It can be shared by PHY control and Port contorl
 *    API.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [in] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
dnx_port_phy_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 value)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                    &gport_info));
    /*
     * Coverity note: The verify function dnx_port_phy_control_verify can ensure the local_port here is
     * a valid port.
     */
    /*
     * coverity[overrun-call] 
     */
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_set(unit, gport_info.local_port, -1, gport_info.lane,
                                                  gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE,
                                                  (soc_phy_control_t) type, value));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get the phy contrl values according to
 *    phy control types
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [out] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_port_phy_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_phy_control_verify(unit, port, type, value));

    SHR_IF_ERR_EXIT(dnx_port_phy_control_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set the phy contrl values according to
 *    phy control types
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port or lane port
 * \param [in] type - phy control types
 * \param [in] value - phy control value
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_phy_control_verify(unit, port, type, &value));

    SHR_IF_ERR_EXIT(dnx_port_phy_control_set(unit, port, type, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parameters for bcm_dnx_port_phy_tx_set and
 *       bcm_dnx_port_phy_tx_get API.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] tx - tx parameters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_port_phy_tx_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "PHY tx parameters");

    /** verify port. supported port type is lane port or logical local port */
    if (BCM_PHY_GPORT_IS_LANE(port))
    {
        SHR_IF_ERR_EXIT(dnx_port_lane_gport_verify(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set PHY tx parameters, including tx taps,
 *    tx tap mode and signalling mode.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] tx - tx parameters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    int is_init_sequence;
    bcm_port_resource_t resource;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_phy_tx_verify(unit, port, tx));
    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, gport_info.local_port, &resource));

    if (resource.link_training == 1)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: setting TX FIR parameters when link training is enabled is not allowed. First disable link training and then set TX FIR parameters.",
                     gport_info.local_port);
    }

    is_init_sequence = !dnx_init_is_init_done_get(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_set
                    (unit, gport_info.local_port, -1, gport_info.lane, 0, is_init_sequence, tx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get PHY tx parameters, including tx taps,
 *    tx tap mode and signalling mode.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx - tx parameters
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_tx_t * tx)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_phy_tx_verify(unit, port, tx));
    /** get logical port*/
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_tx_get(unit, gport_info.local_port, -1, gport_info.lane, 0, tx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - ilk reg access init
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_port_custom_reg_access(
    int unit)
{
    soc_custom_reg_access_t *reg_access;

    reg_access = &(SOC_INFO(unit).custom_reg_access);

    reg_access->custom_reg32_get = soc_jer2_ilkn_reg32_get;
    reg_access->custom_reg32_set = soc_jer2_ilkn_reg32_set;

    reg_access->custom_reg64_get = soc_jer2_ilkn_reg64_get;
    reg_access->custom_reg64_set = soc_jer2_ilkn_reg64_set;

    reg_access->custom_reg_above64_get = soc_jer2_ilkn_reg_above_64_get;
    reg_access->custom_reg_above64_set = soc_jer2_ilkn_reg_above_64_set;

    BCM_PBMP_ASSIGN(reg_access->custom_port_pbmp, PBMP_IL_ALL(unit));

}

static shr_error_e
dnx_port_phy_measure_verify(
    int unit,
    bcm_port_t port,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(phy_measure, _SHR_E_PARAM, "phy_measure");

    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - PHY Measure for all the IMB ports.
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] is_rx - indicate the FIFO used to measure
 * \param [out] phy_measure - phy measure output
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_port_phy_measure_verify(unit, port, phy_measure));

    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    if (!DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type))
    {
        phy_measure->valid = 0;
        SHR_EXIT();
    }
    sal_memset(phy_measure, 0, sizeof(soc_dnxc_port_phy_measure_t));
    SHR_IF_ERR_EXIT(imb_port_phy_measure_get(unit, port, is_rx, phy_measure));
    phy_measure->valid = 1;
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - sync the MAC status according to the PHY status.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] link - phy link status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_update(
    int unit,
    bcm_port_t port,
    int link)
{
    bcm_port_t logical_port = 0;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    /** facility protection case */
    if (BCM_GPORT_IS_SYSTEM_PORT(port) || BCM_GPORT_IS_TRUNK(port))
    {
        SHR_IF_ERR_EXIT(dnx_failover_facility_port_status_set(unit, port, link));
    }
    else
    {
        SHR_INVOKE_VERIFY_DNX(dnx_port_logical_verify(unit, port));

        SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));

        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, logical_port, &port_type));

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE))
        {
            SHR_IF_ERR_EXIT(imb_port_link_up_mac_update(unit, logical_port, link));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
