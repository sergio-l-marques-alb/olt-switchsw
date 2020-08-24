/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2019 Broadcom.
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
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_loopback)
    {
        case BCM_PORT_LOOPBACK_NONE:
            *portmod_loopback = portmodLoopbackCount;
            break;
        case BCM_PORT_LOOPBACK_MAC:
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
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
        portmod_abilities[i].an_mode = (portmod_port_phy_control_autoneg_mode_t) bcm_abilities[i].an_mode;
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
        bcm_abilities[i].an_mode = (bcm_port_autoneg_mode_t) portmod_abilities[i].an_mode;
    }
}

/**
 * \brief - Translate from BCM autoneg abilities to legacy PORTMOD abilities
 * 
 * See .h file
 */
void
imb_portmod_legacy_an_ability_from_bcm_an_ability_get(
    int num_abilities,
    const bcm_port_speed_ability_t * bcm_abilities,
    portmod_port_ability_t * portmod_abilities)
{
    int i;

    for (i = 0; i < num_abilities; ++i)
    {
        /** speeds */
        if (10000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        else if (25000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_25GB;
        }
        else if (40000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_40GB;
        }
        else if (50000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_50GB;
        }
        else if (100000 == bcm_abilities[i].speed)
        {
            portmod_abilities->speed_full_duplex |= SOC_PA_SPEED_100GB;
        }

        /** fec type */
        if (bcmPortPhyFecBaseR == bcm_abilities[i].fec_type)
        {
            portmod_abilities->fec |= SOC_PA_FEC_CL74;
        }
        else if (bcmPortPhyFecRsFec == bcm_abilities[i].fec_type)
        {
            portmod_abilities->fec |= SOC_PA_FEC_CL91;
        }
        else if (bcmPortPhyFecNone == bcm_abilities[i].fec_type)
        {
            portmod_abilities->fec |= SOC_PA_FEC_NONE;
        }
    }

    /** can be only one medium type */
    if (BCM_PORT_MEDIUM_COPPER == bcm_abilities[0].medium)
    {
        portmod_abilities->medium = _SHR_PA_MEDIUM_COPPER;
    }
    else if (BCM_PORT_MEDIUM_FIBER == bcm_abilities[0].medium)
    {
        portmod_abilities->medium = _SHR_PA_MEDIUM_FIBER;
    }
    else if (BCM_PORT_MEDIUM_BACKPLANE == bcm_abilities[0].medium)
    {
        portmod_abilities->medium = _SHR_PA_MEDIUM_BACKPLANE;
    }

    /** can be only one pause type */
    if (bcmPortPhyPauseTx == bcm_abilities[0].pause)
    {
        portmod_abilities->pause = _SHR_PA_PAUSE_TX;
    }
    else if (bcmPortPhyPauseRx == bcm_abilities[0].pause)
    {
        portmod_abilities->pause = _SHR_PA_PAUSE_RX;
    }
    else if (bcmPortPhyPauseSymm == bcm_abilities[0].pause)
    {
        portmod_abilities->pause = _SHR_PA_PAUSE_RX | _SHR_PA_PAUSE_TX;
    }

    /** can be only one channel type */
    if (bcmPortPhyChannelShort == bcm_abilities[0].channel)
    {
        portmod_abilities->channel = _SHR_PA_CHANNEL_SHORT;
    }
    else if (bcmPortPhyChannelLong == bcm_abilities[0].channel)
    {
        portmod_abilities->channel = _SHR_PA_CHANNEL_LONG;
    }
}

/**
 * \brief - Translate from PORTMOD legacy autoneg abilities to BCM abilities
 * 
 * See .h file
 */
int
imb_bcm_an_ability_from_portmod_legacy_an_ability_get(
    int unit,
    bcm_port_t port,
    int max_num_abilities,
    const portmod_port_ability_t * portmod_abilities,
    int *actual_num_ability,
    bcm_port_speed_ability_t * bcm_abilities)
{
    int i;
    phymod_autoneg_control_t an;
    bcm_port_autoneg_mode_t an_mode = phymod_AN_MODE_NONE;
    bcm_port_medium_t medium = _SHR_PORT_MEDIUM_NONE;
    bcm_port_phy_pause_t pause = bcmPortPhyPauseNone;
    bcm_port_phy_fec_t fec = bcmPortPhyFecInvalid;
    bcm_port_phy_channel_t channel = bcmPortPhyChannelCount;
    uint32 speed = 0;
    int nof_lanes, cnt_abilities = 0;
    const int fec_portmod_to_bcm[] = { bcmPortPhyFecInvalid, bcmPortPhyFecNone, bcmPortPhyFecBaseR,
        bcmPortPhyFecInvalid, bcmPortPhyFecRsFec
    };
    const int channel_portmod_to_bcm[] = { bcmPortPhyChannelAll, bcmPortPhyChannelLong, bcmPortPhyChannelShort };

    SHR_FUNC_INIT_VARS(unit);

    /** resolve AN mode */
    SHR_IF_ERR_EXIT(portmod_port_autoneg_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &an));
    if (phymod_AN_MODE_CL73 == an.an_mode)
    {
        an_mode = bcmPortAnModeCL73;
    }
    else if (phymod_AN_MODE_CL73BAM == an.an_mode)
    {
        an_mode = bcmPortAnModeCL73BAM;
    }

    /** resolve medium type */
    if (_SHR_PA_MEDIUM_COPPER == portmod_abilities->medium)
    {
        medium = BCM_PORT_MEDIUM_COPPER;
    }
    else if (_SHR_PA_MEDIUM_FIBER == portmod_abilities->medium)
    {
        medium = BCM_PORT_MEDIUM_FIBER;
    }
    else if (_SHR_PA_MEDIUM_BACKPLANE == portmod_abilities->medium)
    {
        medium = BCM_PORT_MEDIUM_BACKPLANE;
    }

    /** resolve pause type */
    if ((portmod_abilities->pause & _SHR_PA_PAUSE_RX) && (portmod_abilities->pause & _SHR_PA_PAUSE_TX))
    {
        pause = bcmPortPhyPauseSymm;
    }
    else if (portmod_abilities->pause & _SHR_PA_PAUSE_RX)
    {
        pause = bcmPortPhyPauseRx;
    }
    else if (portmod_abilities->pause & _SHR_PA_PAUSE_TX)
    {
        pause = bcmPortPhyPauseTx;
    }

    /** retrieve number of lane */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));

    /** iterate supported AN abilities table */
    for (i = 0; i < dnx_data_nif.eth.supported_an_abilities_info_get(unit)->key_size[0]; ++i)
    {
        if (dnx_data_nif.eth.supported_an_abilities_get(unit, i)->is_valid)
        {
            if ((an_mode == dnx_data_nif.eth.supported_an_abilities_get(unit, i)->an_mode)
                && (nof_lanes == dnx_data_nif.eth.supported_an_abilities_get(unit, i)->nof_lanes)
                && (medium == dnx_data_nif.eth.supported_an_abilities_get(unit, i)->medium))
            {
                fec = dnx_data_nif.eth.supported_an_abilities_get(unit, i)->fec_type;
                if ((fec == fec_portmod_to_bcm[(portmod_abilities->fec & _SHR_PA_FEC_NONE ? _SHR_PA_FEC_NONE : 0)])
                    ||
                    (fec == fec_portmod_to_bcm[(portmod_abilities->fec & _SHR_PA_FEC_CL74 ? _SHR_PA_FEC_CL74 : 0)])
                    || (fec == fec_portmod_to_bcm[(portmod_abilities->fec & _SHR_PA_FEC_CL91 ? _SHR_PA_FEC_CL91 : 0)]))
                {
                    speed = dnx_data_nif.eth.supported_an_abilities_get(unit, i)->speed;
                    if (((speed == 10000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_10GB))
                        || ((speed == 20000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_20GB))
                        || ((speed == 25000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_25GB))
                        || ((speed == 40000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_40GB))
                        || ((speed == 50000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_50GB))
                        || ((speed == 100000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_100GB)))
                    {
                        channel = dnx_data_nif.eth.supported_an_abilities_get(unit, i)->channel;
                        if (((portmod_abilities->channel & _SHR_PA_CHANNEL_LONG)
                             && (channel == channel_portmod_to_bcm[_SHR_PA_CHANNEL_LONG]))
                            || ((portmod_abilities->channel & _SHR_PA_CHANNEL_SHORT)
                                && (channel == channel_portmod_to_bcm[_SHR_PA_CHANNEL_SHORT]))
                            || ((portmod_abilities->channel & (_SHR_PA_CHANNEL_LONG | _SHR_PA_CHANNEL_SHORT))
                                && (channel == bcmPortPhyChannelAll)))
                        {
                            if (cnt_abilities < max_num_abilities)
                            {
                                bcm_abilities[cnt_abilities].an_mode = an_mode;
                                bcm_abilities[cnt_abilities].speed = speed;
                                bcm_abilities[cnt_abilities].resolved_num_lanes = nof_lanes;
                                bcm_abilities[cnt_abilities].medium = medium;
                                bcm_abilities[cnt_abilities].fec_type = fec;
                                bcm_abilities[cnt_abilities].channel =
                                    channel_portmod_to_bcm[portmod_abilities->channel];
                                bcm_abilities[cnt_abilities].pause = pause;
                                ++cnt_abilities;
                            }
                            else
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM, "max_num_abilities %d is too small.\r\n", max_num_abilities);
                            }
                        }
                    }
                }
            }
        }
    }

    *actual_num_ability = cnt_abilities;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate from PORTMOD legacy autoneg abilities to BCM local abilities
 * 
 * See .h file
 */
int
imb_bcm_local_an_ability_from_portmod_legacy_an_ability_get(
    int unit,
    bcm_port_t port,
    int max_num_abilities,
    const portmod_port_ability_t * portmod_abilities,
    int *actual_num_ability,
    bcm_port_speed_ability_t * bcm_abilities)
{
    int i;
    uint32 speed = 0;
    int nof_lanes, cnt_abilities = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** retrieve number of lane */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_lanes));

    /** iterate supported AN abilities table */
    for (i = 0; i < dnx_data_nif.eth.supported_an_abilities_info_get(unit)->key_size[0]; ++i)
    {
        if (dnx_data_nif.eth.supported_an_abilities_get(unit, i)->is_valid)
        {
            if (nof_lanes == dnx_data_nif.eth.supported_an_abilities_get(unit, i)->nof_lanes)
            {
                speed = dnx_data_nif.eth.supported_an_abilities_get(unit, i)->speed;
                if (((speed == 10000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_10GB))
                    || ((speed == 20000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_20GB))
                    || ((speed == 25000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_25GB))
                    || ((speed == 40000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_40GB))
                    || ((speed == 50000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_50GB))
                    || ((speed == 100000) && (portmod_abilities->speed_full_duplex & _SHR_PA_SPEED_100GB)))
                {
                    if (cnt_abilities < max_num_abilities)
                    {
                        if (cnt_abilities > 0)
                        {
                            if ((bcm_abilities[cnt_abilities - 1].an_mode ==
                                 dnx_data_nif.eth.supported_an_abilities_get(unit, i)->an_mode)
                                && (bcm_abilities[cnt_abilities - 1].speed == speed)
                                && (bcm_abilities[cnt_abilities - 1].resolved_num_lanes == nof_lanes)
                                && (bcm_abilities[cnt_abilities - 1].fec_type ==
                                    dnx_data_nif.eth.supported_an_abilities_get(unit, i)->fec_type))
                            {
                                continue;
                            }
                        }

                        bcm_abilities[cnt_abilities].an_mode =
                            dnx_data_nif.eth.supported_an_abilities_get(unit, i)->an_mode;
                        bcm_abilities[cnt_abilities].speed = speed;
                        bcm_abilities[cnt_abilities].resolved_num_lanes = nof_lanes;
                        if (speed == 10000)
                        {
                            bcm_abilities[cnt_abilities].medium =
                                dnx_data_nif.eth.supported_an_abilities_get(unit, i)->medium;
                        }
                        else
                        {
                            bcm_abilities[cnt_abilities].medium = _SHR_PORT_MEDIUM_ALL;
                        }
                        bcm_abilities[cnt_abilities].fec_type =
                            dnx_data_nif.eth.supported_an_abilities_get(unit, i)->fec_type;
                        bcm_abilities[cnt_abilities].channel = bcmPortPhyChannelAll;
                        bcm_abilities[cnt_abilities].pause = bcmPortPhyPauseALL;
                        ++cnt_abilities;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "max_num_abilities %d is too small.\r\n", max_num_abilities);
                    }
                }
            }
        }
    }

    *actual_num_ability = cnt_abilities;

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
