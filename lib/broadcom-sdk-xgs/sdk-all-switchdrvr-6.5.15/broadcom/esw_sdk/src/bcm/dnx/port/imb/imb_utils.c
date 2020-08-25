/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 * \brief - Translate from BCM loopback to Portmod loopback
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] bcm_loopback - BCM loopback type
 * \param [out] portmod_loopback - Portmod loopback type
 *   
 * \return
 *   int - see _SHR_E_ * 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_portmod_loopback_from_bcm_loopback_get(
    int unit,
    bcm_port_t port,
    int bcm_loopback,
    portmod_loopback_mode_t * portmod_loopback)
{
    dnx_algo_port_type_e port_type = DNX_ALGO_PORT_TYPE_INVALID;
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_loopback)
    {
        case BCM_PORT_LOOPBACK_NONE:
            *portmod_loopback = portmodLoopbackCount;
            break;
        case BCM_PORT_LOOPBACK_MAC:
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type))
            {
                *portmod_loopback = portmodLoopbackMacAsyncFifo;
            }
            else
            {
                *portmod_loopback = portmodLoopbackMacOuter;
            }
            break;
        }
        case BCM_PORT_LOOPBACK_PHY:
            *portmod_loopback = portmodLoopbackPhyGloopPMD;
            break;
        case BCM_PORT_LOOPBACK_PHY_REMOTE:
            *portmod_loopback = portmodLoopbackPhyRloopPMD;
            break;
        case BCM_PORT_LOOPBACK_MAC_REMOTE:
            *portmod_loopback = portmodLoopbackMacRloop;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "BCM loopback type %d did not match any portmod loopback type", bcm_loopback);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate from Portmod loopback to BCM loopback
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] portmod_loopback - Portmod loopback type
 * \param [out] bcm_loopback - BCM loopback type
 *   
 * \return
 *   int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_bcm_loopback_from_portmod_loopback_get(
    int unit,
    bcm_port_t port,
    portmod_loopback_mode_t portmod_loopback,
    int *bcm_loopback)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (portmod_loopback)
    {
        case portmodLoopbackCount:
            *bcm_loopback = BCM_PORT_LOOPBACK_NONE;
            break;
        case portmodLoopbackMacOuter:
        case portmodLoopbackMacAsyncFifo:
            *bcm_loopback = BCM_PORT_LOOPBACK_MAC;
            break;
        case portmodLoopbackPhyGloopPCS:
        case portmodLoopbackPhyGloopPMD:
            *bcm_loopback = BCM_PORT_LOOPBACK_PHY;
            break;
        case portmodLoopbackPhyRloopPMD:
            *bcm_loopback = BCM_PORT_LOOPBACK_PHY_REMOTE;
            break;
        case portmodLoopbackMacRloop:
            *bcm_loopback = BCM_PORT_LOOPBACK_MAC_REMOTE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                         "Portmod loopback type %d did not match any BCM loopback type", portmod_loopback);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Translate from BCM autoneg abilities to PORTMOD abilities
 * 
 * See .h file
 */
void
imb_portmod_an_ability_from_bcm_an_ability_get(
    int num_abilities,
    const bcm_port_speed_ability_t * bcm_abilities,
    portmod_port_speed_ability_t * portmod_abilities)
{
    int i;
    for (i = 0; i < num_abilities; ++i)
    {
        portmod_abilities[i].speed = bcm_abilities[i].speed;
        portmod_abilities[i].num_lanes = bcm_abilities[i].resolved_num_lanes;
        portmod_abilities[i].fec_type = bcm_abilities[i].fec_type;
        portmod_abilities[i].medium = bcm_abilities[i].medium;
        portmod_abilities[i].pause = bcm_abilities[i].pause;
        portmod_abilities[i].channel = bcm_abilities[i].channel;
        portmod_abilities[i].an_mode = bcm_abilities[i].an_mode;
    }
}

/**
 * \brief - Translate from PORTMOD autoneg abilities to BCM abilities
 * 
 * See .h file
 */
void
imb_bcm_an_ability_from_portmod_an_ability_get(
    int num_abilities,
    portmod_port_speed_ability_t * portmod_abilities,
    bcm_port_speed_ability_t * bcm_abilities)
{
    int i;
    for (i = 0; i < num_abilities; ++i)
    {
        bcm_abilities[i].speed = portmod_abilities[i].speed;
        bcm_abilities[i].resolved_num_lanes = portmod_abilities[i].num_lanes;
        bcm_abilities[i].fec_type = portmod_abilities[i].fec_type;
        bcm_abilities[i].medium = portmod_abilities[i].medium;
        bcm_abilities[i].pause = portmod_abilities[i].pause;
        bcm_abilities[i].channel = portmod_abilities[i].channel;
        bcm_abilities[i].an_mode = portmod_abilities[i].an_mode;
    }
}

#undef BSL_LOG_MODULE
