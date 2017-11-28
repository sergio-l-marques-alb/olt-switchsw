/*
 *
 * $Id:$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_CPM4X25_SUPPORT


#define CPM4x25_WB_BUFFER_VERSION        (8)

#define CPM4x25_IS_ACTIVE_SET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], &is_active)
#define CPM4x25_IS_ACTIVE_GET(unit, pm_info, is_active) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isActive], is_active)

#define CPM4x25_IS_INITIALIZED_SET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], &is_initialized)
#define CPM4x25_IS_INITIALIZED_GET(unit, pm_info, is_initialized) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInitialized], is_initialized)

#define CPM4x25_IS_BYPASSED_SET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], &is_bypass)
#define CPM4x25_IS_BYPASSED_GET(unit, pm_info, is_bypass) \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isBypassed], is_bypass)

#define CPM4x25_LANE2PORT_SET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], &port, lane)
#define CPM4x25_LANE2PORT_GET(unit, pm_info, lane, port) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], port, lane)

#define CPM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[maxSpeed], &max_speed, port_index)
#define CPM4x25_MAX_SPEED_GET(unit, pm_info, max_speed, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[maxSpeed], max_speed, port_index)

#define CPM4x25_INTERFACE_TYPE_SET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceType], &interface_type, port_index)
#define CPM4x25_INTERFACE_TYPE_GET(unit, pm_info, interface_type, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceType], interface_type, port_index)

#define CPM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index) \
    SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceModes], &interface_modes, port_index)
#define CPM4x25_INTERFACE_MODES_GET(unit, pm_info, interface_modes, port_index) \
    SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceModes], interface_modes, port_index)

typedef enum cpm4x25_wb_vars{
    isActive = 0, /* specify the PM4x25 core is active (at least one port has been attached) */
    isBypassed, /*specify the PM is in Bypass mode - the MAC is not in use, some configurations do not apply*/
    isInitialized, /* specify initialization of all SerDes have been done */
    lane2portMap, /* map each lane in the PM to a logical port */
    maxSpeed, /* Max Port Speed Supported */
    interfaceType, /* interface of type soc_port_if_t */
    interfaceModes /* see PHYMOD_INTF_MODES_ */
}cpm4x25_wb_vars_t;


extern int pm4x25_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data);

static int _cprimod_port_speed_id_to_vco(int speed_id);


int cpm4x25_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int port_index = -1;
    int rv = 0;
    int phy, ii;
    int phys_count = 0, first_phy = -1;
    int tmp_port, first_phy_in_core;
    uint32 pm_is_bypassed = 0;
    portmod_pbmp_t port_phys_in_pm;
    phymod_phy_access_t phy_access;
    phymod_core_init_config_t init_cfg;
    phymod_core_status_t core_sts;
    cprimod_port_init_config_t port_config;
    uint32 core_is_initialized;


    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&init_cfg));
    /* when first time initilizing the core firmware loaded will not be set */
    if(!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized)) {
        /* First time make all ports as cpri ports that will avoid using clmac
         * reset for all cpri port */
        first_phy_in_core = (((SOC_INFO(unit).port_l2p_mapping[port]-1)>>2)<<2)+1;
        for(ii=0; ii<CPRIMOD_MAX_LANES_PER_CORE; ii++) {
            tmp_port = (SOC_INFO(unit).port_p2l_mapping[first_phy_in_core+ii]);
            _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_set(unit, tmp_port, 1));
        }
    }
    if((add_info->interface_config.encap_mode != _SHR_PORT_ENCAP_CPRI) && 
      (add_info->interface_config.encap_mode != _SHR_PORT_ENCAP_RSVD4)){
        _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_set(unit, port, 0));
        _rv = pm4x25_port_attach(unit, port, pm_info, add_info);
        SOC_FUNC_RETURN;
    }
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_set(unit, port, 1));
    rv = CPM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);
    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);
    ii = 0;
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy){
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = CPM4x25_LANE2PORT_SET(unit, pm_info, ii, port);
            _SOC_IF_ERR_EXIT(rv);
            first_phy = (first_phy == -1) ? phy : first_phy;
            port_index = (port_index == -1 ? ii : port_index);
        }
        ii++;
    }
    PM_4x25_INFO(pm_info)->first_phy = first_phy;
    if(port_index<0 || port_index>=PM4X25_LANES_PER_CORE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("can't attach port %d. can't find relevant phy"), port));
    }
    /* init data for port configuration */
    /*
    rv = CPM4x25_MAX_SPEED_SET(unit, pm_info, add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = CPM4x25_INTERFACE_TYPE_SET(unit, pm_info, add_info->interface_config.interface, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = CPM4x25_INTERFACE_MODES_SET(unit, pm_info, add_info->interface_config.interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);
    */
    if (add_info->init_config.polarity_overwrite == 1) {
        sal_memcpy(&PM_4x25_INFO(pm_info)->polarity,
            &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
    }
    if (add_info->init_config.lane_map_overwrite == 1) {
        sal_memcpy(&PM_4x25_INFO(pm_info)->lane_map,
            &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
    }
    if (add_info->init_config.fw_load_method_overwrite == 1) {
        PM_4x25_INFO(pm_info)->fw_load_method =
            add_info->init_config.fw_load_method[0];
    }
    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_4x25_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }
    /*
    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;
    */
    /* No probe for CPM4x25 */
    if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info)) {
        return (rv);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)){
        CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&init_cfg);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)){
        CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&init_cfg);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        if (add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
            init_cfg.pll0_div_init_value = _cprimod_port_speed_id_to_vco(add_info->interface_config.speed);
            init_cfg.pll1_div_init_value = 0;
        }
    }

    rv = CPM4x25_IS_INITIALIZED_GET(unit, pm_info, &core_is_initialized);
    _SOC_IF_ERR_EXIT(rv);

    if (!PORTMOD_CORE_INIT_FLAG_CPRI_CORE_INITIALZIED_GET(core_is_initialized)) {
        if(!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized) ||
            PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
                init_cfg.tx_input_voltage=cprimodTxInputVoltageDefault;
                init_cfg.firmware_loader = pm4x25_default_fw_loader;
                init_cfg.lane_map.num_of_lanes = PM_4x25_INFO(pm_info)->lane_map.num_of_lanes;
                for(ii=0; ii<CPRIMOD_MAX_LANES_PER_CORE; ii++) {
                    init_cfg.lane_map.lane_map_rx[ii] = PM_4x25_INFO(pm_info)->lane_map.lane_map_rx[ii];
                    init_cfg.lane_map.lane_map_tx[ii] = PM_4x25_INFO(pm_info)->lane_map.lane_map_tx[ii];
                }
            }
            _SOC_IF_ERR_EXIT(cprimod_core_init(unit, port, &PM_4x25_INFO(pm_info)->int_core_access,
                                           &init_cfg, &core_sts));

            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
                PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
            if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                PORTMOD_CORE_INIT_FLAG_CPRI_CORE_INITIALZIED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
        }
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (rv);
    }
    /* initalize port */

    /* rv = _pm4x25_pm_port_init(unit, port, pm_info, port_index, add_info, 1); */
    if(add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_CPRI) {
        switch(add_info->interface_config.speed) {
        case 0:
            port_config.tx_cpri_speed  = cprimodSpd1228p8;
            port_config.rx_cpri_speed  = cprimodSpd1228p8;
            break;
        case 1:
            port_config.tx_cpri_speed  = cprimodSpd2457p6;
            port_config.rx_cpri_speed  = cprimodSpd2457p6;
            break;
        case 2:
            port_config.tx_cpri_speed  = cprimodSpd3072p0;
            port_config.rx_cpri_speed  = cprimodSpd3072p0;
            break;
        case 3:
            port_config.tx_cpri_speed  = cprimodSpd4915p2;
            port_config.rx_cpri_speed  = cprimodSpd4915p2;
            break;
        case 4:
            port_config.tx_cpri_speed  = cprimodSpd6144p0;
            port_config.rx_cpri_speed  = cprimodSpd6144p0;
            break;
        case 5:
            port_config.tx_cpri_speed  = cprimodSpd9830p4;
            port_config.rx_cpri_speed  = cprimodSpd9830p4;
            break;
        case 6:
            port_config.tx_cpri_speed  = cprimodSpd10137p6;
            port_config.rx_cpri_speed  = cprimodSpd10137p6;
            break;
        case 7:
            port_config.tx_cpri_speed  = cprimodSpd12165p12;
            port_config.rx_cpri_speed  = cprimodSpd12165p12;
            break;
        case 8:
            port_config.tx_cpri_speed  = cprimodSpd24330p24;
            port_config.rx_cpri_speed  = cprimodSpd24330p24;
            break;
        default:
            port_config.tx_cpri_speed  = cprimodSpd1228p8;
            port_config.rx_cpri_speed  = cprimodSpd1228p8;
            break;
        }
        port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult4X;
        port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult4X;
    } else if (add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_RSVD4) {
        switch(add_info->interface_config.speed) {
        case 4:
            port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            break;
        case 8:
            port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult8X;
            port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult8X;
            break;
        default:
            port_config.tx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            port_config.rx_rsvd4_speed = cprimodRsvd4SpdMult4X;
            break;
        }
        port_config.tx_cpri_speed  = cprimodSpd1228p8;
        port_config.rx_cpri_speed  = cprimodSpd1228p8;
    }
    if (add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_RSVD4) {
        port_config.interface = cprimodRsvd4;
    } else {
        port_config.interface = cprimodCpri;
    }
    port_config.roe_stuffing_bit = add_info->init_config.roe_stuffing_bit;
    port_config.roe_reserved_bit = add_info->init_config.roe_reserved_bit;

    cprimod_port_init(unit, port,  &phy_access, &port_config);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;
    _rv = pm4x25_pm_init(unit, pm_add_info, wb_buffer_index, pm_info);

    SOC_FUNC_RETURN;

}

int cpm4x25_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int val;
    int phy_enable;
    int actual_flags = flags;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_enable_get(unit, port, pm_info, flags, enable);
        SOC_FUNC_RETURN;
    }
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }
    *enable = 0;
    if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_rx_enable_get(unit, port, &phy_enable));
        *enable |= phy_enable;
    }
    if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_tx_enable_get(unit, port, &phy_enable));
        *enable |= phy_enable;
    }

exit:
    SOC_FUNC_RETURN;
}


int cpm4x25_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int val;
    int actual_flags = flags;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_enable_set(unit, port, pm_info, flags, enable);
        SOC_FUNC_RETURN;
    }
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }
    if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_rx_enable_set(unit, port, enable));
    }
    if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
        _SOC_IF_ERR_EXIT(cprimod_cpri_port_tx_enable_set(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    int val;
    uint32 los;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_link_get(unit, port, pm_info, flags, link);
        SOC_FUNC_RETURN;
    }
    _SOC_IF_ERR_EXIT(cprimod_port_rx_pcs_status_get(unit, port,
                                                    cprimodRxPcsStatusLosLive,
                                                    &los));
    *link = (los&1);


exit:

    SOC_FUNC_RETURN;

}

int cpm4x25_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_loopback_set(unit, port, pm_info, loopback_type, enable);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_loopback_get(int unit, int port, pm_info_t pm_info,
                 portmod_loopback_mode_t loopback_type, int* enable)
{   
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_loopback_get(unit, port, pm_info, loopback_type, enable);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;

}
int cpm4x25_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, 
                            const phymod_autoneg_control_t* an) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_autoneg_set(unit, port, pm_info, phy_flags, an);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an) {
    int val;
    SOC_INIT_FUNC_DEFS;
    an->enable = 0;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_autoneg_get(unit, port, pm_info, phy_flags, an);
        SOC_FUNC_RETURN;
    }

exit:

    SOC_FUNC_RETURN;
}
int cpm4x25_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_ability_advert_get(unit, port, pm_info, phy_flags, ability);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, 
                                 portmod_port_ability_t* ability) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_ability_advert_set(unit, port, pm_info, phy_flags, ability);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_autoneg_status_get (int unit, int port, pm_info_t pm_info, 
                                    phymod_autoneg_status_t* an_status) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_autoneg_status_get(unit, port, pm_info, an_status);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_ability_remote_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, 
                                       portmod_port_ability_t* ability) {
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_ability_remote_get(unit, port, pm_info, phy_flags, ability);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_encap_set(int unit, int port, pm_info_t pm_info, int flags, portmod_encap_t encap)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_encap_set(unit, port, pm_info, flags, encap);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_encap_get(int unit, int port, pm_info_t pm_info, int* flags, portmod_encap_t* encap)
{
    int val;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_encap_get(unit, port, pm_info, flags, encap);
        SOC_FUNC_RETURN;
    }

exit:
    SOC_FUNC_RETURN;

}

static int
_cprimod_port_speed_t_to_int (
    cprimod_port_speed_t speedi)
{
    int speedo;

    switch(speedi) {
    case cprimodSpd1228p8:
        speedo = 1228;
        break;
    case cprimodSpd2457p6:
        speedo = 2457;
        break;
    case cprimodSpd3072p0:
        speedo = 3072;
        break;
    case cprimodSpd4915p2:
        speedo = 4915;
        break;
    case cprimodSpd6144p0:
        speedo = 6144;
        break;
    case cprimodSpd9830p4:
        speedo = 9830;
        break;
    case cprimodSpd10137p6:
        speedo = 10137;
        break;
    case cprimodSpd12165p12:
        speedo = 12165;
        break;
    case cprimodSpd24330p24:
        speedo = 24330;
        break;
    default:
        speedo = 1228;
        break;
    }

    return speedo;
}

static cprimod_port_speed_t
_int_to_cprimod_port_speed_t (
    int speedi)
{
    cprimod_port_speed_t speedo;
    switch(speedi) {
    case 1228:
        speedo = cprimodSpd1228p8;
        break;
    case 2457:
        speedo = cprimodSpd2457p6;
        break;
    case 3072:
        speedo = cprimodSpd3072p0;
        break;
    case 4915:
        speedo = cprimodSpd4915p2;
        break;
    case 6144:
        speedo = cprimodSpd6144p0;
        break;
    case 9830:
        speedo = cprimodSpd9830p4;
        break;
    case 10137:
        speedo = cprimodSpd10137p6;
        break;
    case 12165:
        speedo = cprimodSpd12165p12;
        break;
    case 24330:
        speedo = cprimodSpd24330p24;
        break;
    default:
        speedo = cprimodSpd1228p8;
        break;
    }

    return speedo;
}

static int
_cprimod_port_speed_id_to_vco (
   int speed_id)
{
    int vco_value;
    switch(speed_id) {
    case 0:
        vco_value = phymod_TSCF_PLL_DIV160;
        break;
    case 1:
        vco_value = phymod_TSCF_PLL_DIV160;
        break;
    case 2:
        vco_value = phymod_TSCF_PLL_DIV200;
        break;
    case 3:
        vco_value = phymod_TSCF_PLL_DIV160;
        break;
    case 4:
        vco_value = phymod_TSCF_PLL_DIV200;
        break;
    case 5:
        vco_value = phymod_TSCF_PLL_DIV160;
        break;
    case 6:
        vco_value = phymod_TSCF_PLL_DIV165;
        break;
    case 7:
        vco_value = phymod_TSCF_PLL_DIV198;
        break;
    case 8:
        vco_value = phymod_TSCF_PLL_DIV198;
        break;
    default:
        vco_value = phymod_TSCF_PLL_DIV160;
        break;
    }

    return vco_value;
}

int cpm4x25_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    int val;
    cprimod_port_interface_config_t config_t;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
        _rv = pm4x25_port_interface_config_set(unit, port, pm_info, config, phy_init_flags);
        SOC_FUNC_RETURN;
    }
    config_t.speed = _int_to_cprimod_port_speed_t(config->speed);
    config_t.interface = cprimodCpri;
    if (phy_init_flags == PORTMOD_INIT_F_TX_ONLY) {
        _SOC_IF_ERR_EXIT(cprimod_port_tx_interface_config_set(unit, port,
                                                              &config_t));
    }
    if (phy_init_flags == PORTMOD_INIT_F_RX_ONLY) {
        _SOC_IF_ERR_EXIT(cprimod_port_rx_interface_config_set(unit, port,
                                                              &config_t));
    }

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    int val;
    cprimod_port_interface_config_t config_t;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_cpri_or_enet_port_get(unit, port, &val));
    if(val == 0) {
       _rv = pm4x25_port_interface_config_get(unit, port, pm_info, config, phy_init_flags);
        SOC_FUNC_RETURN;
    }
    if (phy_init_flags == PORTMOD_INIT_F_TX_ONLY) {
        _SOC_IF_ERR_EXIT(cprimod_port_tx_interface_config_get(unit, port,
                                                              &config_t));
    } else {
        _SOC_IF_ERR_EXIT(cprimod_port_rx_interface_config_get(unit, port,
                                                              &config_t));
    }
    config->speed = _cprimod_port_speed_t_to_int (config_t.speed);


exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */


    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_frame_optional_config_set(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_frame_optional_config_get(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_tx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_frame_optional_config_set(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_tx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_frame_optional_config_get(unit, port,
                                                                     field, value));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rx_frame_optional_config_set(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rx_frame_optional_config_get(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_frame_optional_config_set(int unit, int port, pm_info_t pm_info, cprimod_tx_config_field_t field, uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_tx_frame_optional_config_set(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_frame_optional_config_get(int unit, int port, pm_info_t pm_info, cprimod_tx_config_field_t field, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_tx_frame_optional_config_get(unit, port, field,
                                                               value));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_stage_activate(int unit, int port, pm_info_t pm_info, portmod_cpri_stage_t stage)
{
    cprimod_basic_frame_table_id_t table;
    SOC_INIT_FUNC_DEFS;

    if (stage == portmodCpriStageRx) {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_active_table_get(unit, port,&table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_1));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_0));
        }
    } else {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_active_table_get(unit, port,&table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_1));
        } else {
            _SOC_IF_ERR_EXIT
                (cprimod_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_0));
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_speed_set(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_speed_set(unit, port, speed));
    _SOC_IF_ERR_EXIT(cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageRx));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_rx_speed_get(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_speed_get(unit, port, speed));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_port_rsvd4_tx_speed_set(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_speed_set(unit, port, speed));
    _SOC_IF_ERR_EXIT(cpm4x25_cpri_port_stage_activate(unit, port, pm_info, portmodCpriStageTx));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_port_rsvd4_tx_speed_get(int unit, int port, pm_info_t pm_info, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_speed_get(unit, port, speed));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_tx_fsm_state_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_fsm_state_set(unit, port, state));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_fsm_state_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_fsm_state_set(unit, port, state));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_tx_override_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_tx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_tx_overide_set(unit, port, parameter,
                                                        enable, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_override_set(int unit, int port, pm_info_t pm_info, cprimod_rsvd4_rx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprimod_port_rsvd4_rx_overide_set(unit, port, parameter,
                                                        enable, value));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_axc_frame_add(int unit, int port, pm_info_t pm_info, uint32 axc_id, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_axc_basic_frame_add(unit, port, axc_id, frame_info->start_bit, frame_info->num_bits));
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_axc_container_config_set(unit, port, axc_id, &frame_info->container_info));
    _SOC_IF_ERR_EXIT
        (cprimod_rx_roe_payload_size_set(unit, port, axc_id, &frame_info->roe_payload_info));
    /*
     * In data AxC, the AxC number and Queue number are identical.
     */
    queue_num = axc_id;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, queue_num, &frame_info->encap_data_info ));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_axc_frame_delete(int unit, int port, pm_info_t pm_info, uint32 axc_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_axc_basic_frame_delete(unit, port, axc_id));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rx_axc_frame_get(int unit, int port, pm_info_t pm_info, uint32 axc_id, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    cprimod_basic_frame_table_id_t active_table;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    int num_entries;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_parser_active_table_get(unit, port, &active_table));


    usage_table = (cprimod_basic_frame_usage_entry_t *) sal_alloc
                                   ((sizeof(cprimod_basic_frame_table_id_t)*CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY),
                                    "cpm4x25");

    SOC_NULL_CHECK(usage_table);
    num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;


    _SOC_IF_ERR_EXIT
        (cprimod_rx_axc_basic_frame_get(unit, port, axc_id, active_table, usage_table, &num_entries));
    

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_axc_container_config_get(unit, port, axc_id, &frame_info->container_info));

    _SOC_IF_ERR_EXIT
        (cprimod_rx_roe_payload_size_get(unit, port, axc_id, &frame_info->roe_payload_info));
    /*
     * In data AxC, the AxC number and Queue number are identical.
     */
    queue_num = axc_id;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_get(unit, port, queue_num, &frame_info->encap_data_info ));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_axc_frame_add(int unit, int port, pm_info_t pm_info, uint32 axc_id, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_axc_basic_frame_add(unit, port, axc_id, frame_info->start_bit, frame_info->num_bits));
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_axc_container_config_set(unit, port, axc_id, &frame_info->container_info));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_set(unit, port, axc_id, &frame_info->roe_payload_info));
    /*
     * In data AxC, the AxC number and Queue number are identical.
     */
    queue_num = axc_id;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, queue_num, &frame_info->encap_data_info ));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_axc_frame_delete(int unit, int port, pm_info_t pm_info, uint32 axc_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_axc_basic_frame_delete(unit, port, axc_id));

exit:
    SOC_FUNC_RETURN;

}



int cpm4x25_cpri_port_tx_axc_frame_get(int unit, int port, pm_info_t pm_info, uint32 axc_id, portmod_cpri_axc_frame_info_t* frame_info)
{
    uint32 queue_num = 0xFF;
    cprimod_basic_frame_table_id_t active_table;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    int num_entries;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_basic_frame_assembly_active_table_get(unit, port, &active_table));

    usage_table = (cprimod_basic_frame_usage_entry_t *) sal_alloc
                                   ((sizeof(cprimod_basic_frame_table_id_t)*CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY),
                                    "cpm4x25");

    SOC_NULL_CHECK(usage_table);
    num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;


    _SOC_IF_ERR_EXIT
        (cprimod_tx_axc_basic_frame_get(unit, port, axc_id, active_table, usage_table, &num_entries));
    

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_tx_axc_container_config_get(unit, port, axc_id, &frame_info->container_info));

    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_get(unit, port, axc_id, &frame_info->roe_payload_info));
    /*
     * In data AxC, the AxC number and Queue number are identical.
     */
    queue_num = axc_id;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_get(unit, port, queue_num, &frame_info->encap_data_info ));
exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_encap_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_encap_header_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_header_config_set(unit, port, queue_num, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_encap_header_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_header_config_get(unit, port, queue_num, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_roe_ordering_info_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_encap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_entry_set(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_roe_ordering_info_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_encap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_entry_get(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}



int cpm4x25_cpri_port_encap_roe_ordering_sequence_offset_set(int unit, int port, pm_info_t pm_info,
                                                             uint32 queue_num,
                                                             uint32 control,
                                                             cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_sequence_offset_set(unit, port, queue_num, control, config));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_roe_ordering_sequence_offset_get(int unit, int port, pm_info_t pm_info,
                                                             uint32 queue_num,
                                                             uint32 control,
                                                             cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ordering_info_sequence_offset_get(unit, port, queue_num, control, config));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_macda_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_da_entry_set(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_macda_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_da_entry_get(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_roe_encap_macsa_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_sa_entry_set(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_macsa_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_mac_addr_t mac_addr)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_mac_sa_entry_get(unit, port, index, mac_addr));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_vlan_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_vlan_table_id_t table_id, uint32 vlan_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_vlan_id_entry_set(unit, port, table_id, index, vlan_id));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_roe_encap_vlan_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_vlan_table_id_t table_id, uint32* vlan_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_vlan_id_entry_get(unit, port, table_id, index, vlan_id));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_ethertype_config_set(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16 ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ethertype_config_set(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_ethertype_config_get(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16* ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ethertype_config_get(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_ethertype_config_set(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16 ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ethertype_config_set(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_ethertype_config_get(int unit, int port, pm_info_t pm_info,
                                                cprimod_ethertype_t pkt_type,
                                                uint16* ethertype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_ethertype_config_get(unit, port, pkt_type, ethertype));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_config_set(int unit, int port, pm_info_t pm_info,
                                            uint8 roe_subtype,
                                            uint32 queue_num,
                                            cprimod_cls_option_t cls_option,
                                            cprimod_cls_flow_type_t flow_type,
                                            uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_classification_config_set(unit, port, roe_subtype, queue_num, cls_option, flow_type));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_config_get(int unit, int port, pm_info_t pm_info,
                                            uint8 roe_subtype,
                                            uint32* queue_num,
                                            cprimod_cls_option_t* cls_option,
                                            cprimod_cls_flow_type_t* flow_type,
                                            uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_classification_config_get(unit, port, roe_subtype, queue_num, cls_option, flow_type));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_queue_to_ordering_info_index_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_queue_to_ordering_info_index_mapping_set(unit, port, queue_num, ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_queue_to_ordering_info_index_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, uint32* ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_queue_to_ordering_info_index_mapping_get(unit, port, queue_num, ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_decap_flow_to_queue_mapping_set(int unit, int port, pm_info_t pm_info, uint32 flow_id, uint32 queue_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_to_queue_mapping_set(unit, port, flow_id, queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_flow_to_queue_mapping_get(int unit, int port, pm_info_t pm_info, uint32 flow_id, uint32* queue_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_flow_to_queue_mapping_get(unit, port, flow_id, queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_roe_ordering_info_set(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_decap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_entry_set(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_roe_ordering_info_get(int unit, int port, pm_info_t pm_info, uint32 index, cprimod_decap_ordering_info_entry_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_entry_get(unit, port, index, config));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_decap_roe_ordering_sequence_offset_set(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_sequence_offset_set(unit, port, queue_num, config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_decap_roe_ordering_sequence_offset_get(int unit, int port, pm_info_t pm_info, uint32 queue_num, cprimod_cpri_ordering_info_offset_t* config)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_decap_ordering_info_sequence_offset_get(unit, port, queue_num, config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_config_set(int unit, int port, pm_info_t pm_info, portmod_tx_frame_tgen_config_t* config)
{
    uint8 tgen_hfn = 0;
    uint16 tgen_bfn= 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_select_counter_set(unit, port, config->tx_gen_bfn_hfn_sel));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_set(unit, port, config->tx_tgen_ts_offset));

    tgen_hfn = config->tx_tgen_hfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_set(unit, port, tgen_hfn));

    tgen_bfn = config->tx_tgen_bfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_set(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_config_get(int unit, int port, pm_info_t pm_info, portmod_tx_frame_tgen_config_t* config)
{
    uint8 tgen_hfn = 0;
    uint16 tgen_bfn= 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_select_counter_get(unit, port, &config->tx_gen_bfn_hfn_sel));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_get(unit, port, &config->tx_tgen_ts_offset));

    tgen_hfn = config->tx_tgen_hfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_get(unit, port, &tgen_hfn));
    config->tx_tgen_hfn = tgen_hfn;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_get(unit, port, &tgen_bfn));
    config->tx_tgen_bfn = tgen_bfn;

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_frame_tgen_enable(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_enable(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_frame_tgen_ts_offset_set(int unit, int port, pm_info_t pm_info, uint64 offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_set(unit, port, offset));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_ts_offset_get(int unit, int port, pm_info_t pm_info, uint64* offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_offset_get(unit, port, offset));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_next_hfn_set(int unit, int port, pm_info_t pm_info, uint32 hfn)
{
    uint8 tgen_hfn;
    SOC_INIT_FUNC_DEFS;

    tgen_hfn = hfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_set(unit, port, tgen_hfn));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_next_hfn_get(int unit, int port, pm_info_t pm_info, uint32* hfn)
{
    uint8 tgen_hfn;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_get(unit, port, &tgen_hfn));
    *hfn = tgen_hfn;

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_frame_tgen_next_bfn_set(int unit, int port, pm_info_t pm_info, uint32 bfn)
{
    uint16 tgen_bfn;
    SOC_INIT_FUNC_DEFS;

    tgen_bfn = bfn;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_set(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_frame_tgen_next_bfn_get(int unit, int port, pm_info_t pm_info, uint32* bfn)
{
    uint16 tgen_bfn;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_get(unit, port, &tgen_bfn));

    *bfn = tgen_bfn;
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_flow_add(int unit, int port, pm_info_t pm_info,
                                        uint32 flow_id,
                                        portmod_rsvd4_rx_flow_info_t* info)
{
    cprimod_header_compare_entry_t compare_entry;
    cprimod_rsvd4_container_config_t container_info;
    cprimod_rsvd4_encap_data_config_t encap_data_info;
    cprimod_payload_size_info_t  payload_info;
    uint32 queue_num=0;

    SOC_INIT_FUNC_DEFS;

    cprimod_header_compare_entry_t_init(&compare_entry);
    compare_entry.valid         = 1;
    compare_entry.mask          = info->mask;
    compare_entry.match_data    = info->match_data;
    compare_entry.flow_id       = flow_id;
    compare_entry.flow_type     = cprimodFlowTypeData;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_add(unit, port, &compare_entry));

    cprimod_rsvd4_container_config_t_init(&container_info);
    container_info.axc_id                   = info->axc_id;
    container_info.stuffing_cnt             = info->stuffing_cnt;
    container_info.msg_ts_mode              = info->msg_ts_mode;
    container_info.msg_ts_cnt               = info->msg_ts_count;
    container_info.frame_sync_mode          = info->frame_sync_mode;
    container_info.master_frame_offset      = info->master_frame_offset;
    container_info.message_number_offset    = info->message_number_offset;
    container_info.container_block_cnt      = info->container_block_count;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_axc_container_config_set(unit, port, flow_id, &container_info));

    cprimod_payload_size_info_t_init(&payload_info);
    payload_info.packet_size        = info->packet_size ;
    payload_info.last_packet_num    = info->last_packet_num ;
    payload_info.last_packet_size   = info->last_packet_size ;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_roe_payload_size_set(unit, port, flow_id, &payload_info));
    /*
     * Queue number and flow id are same for data flow.
     */
    queue_num = container_info.axc_id;

    cprimod_rsvd4_encap_data_config_t_init(&encap_data_info);

    encap_data_info.buffer_size             = info->queue_size;
    encap_data_info.priority                = info->priority ;
    encap_data_info.gsm_pad_size            = info->pad_size;
    encap_data_info.gsm_extra_pad_size      = info->extra_pad_size ;
    encap_data_info.gsm_pad_enable          = info->pad_enable ;
    encap_data_info.gsm_control_location    = info->control_location ;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_encap_data_config_set(unit, port, queue_num, &encap_data_info));

    _SOC_IF_ERR_EXIT
        (cprimod_rx_rsvd4_gsm_tsn_bitmap_set(unit, port, queue_num, info->tsn_bitmap));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_tx_flow_add(int unit, int port, pm_info_t pm_info, uint32 flow_id, portmod_rsvd4_tx_flow_info_t* info)
{
    cprimod_rsvd4_container_config_t container_info;
    cprimod_payload_size_info_t payload_info;
    cprimod_rsvd4_decap_data_config_t decap_data_info;
    uint32 queue_num;

    SOC_INIT_FUNC_DEFS;

    cprimod_rsvd4_container_config_t_init(&container_info);
    container_info.axc_id                   = info->axc_id;
    container_info.stuffing_cnt             = info->stuffing_cnt ;
    container_info.msg_ts_mode              = info->msg_ts_mode ;
    container_info.msg_ts_cnt               = info->msg_ts_count ;
    container_info.frame_sync_mode          = info->frame_sync_mode ;
    container_info.master_frame_offset      = info->master_frame_offset ;
    container_info.message_number_offset    = info->message_number_offset ;
    container_info.container_block_cnt      = info->container_block_count ;
    container_info.use_ts_dbm               = info->use_ts_dbm ;
    container_info.ts_dbm_prof_num          = info->ts_dbm_profile_num  ;
    container_info.num_active_slots         = info->num_active_slot ;
    container_info.msg_addr                 = info->msg_addr ;
    container_info.msg_type                 = info->msg_type ;
    container_info.msg_ts_offset            = info->msg_ts_offset ;
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_axc_container_config_set(unit, port, flow_id , &container_info));

    cprimod_payload_size_info_t_init(&payload_info);
    payload_info.packet_size        = info->packet_size ;
    payload_info.last_packet_num    = info->last_packet_num ;
    payload_info.last_packet_size   = info->last_packet_size ;
    _SOC_IF_ERR_EXIT
        (cprimod_tx_roe_payload_size_set(unit, port, flow_id, &payload_info));

    /*
     * Queue number and flow id are same for data flow.
     */
    queue_num = container_info.axc_id;

    cprimod_rsvd4_decap_data_config_t_init(&decap_data_info);
    decap_data_info.buffer_size = info->buffer_size ;
    decap_data_info.cycle_size = info->cycle_size ;
    decap_data_info.gsm_pad_size = info->pad_size ;
    decap_data_info.gsm_extra_pad_size = info->extra_pad_size ;
    decap_data_info.gsm_pad_enable = info->pad_enable ;
    decap_data_info.gsm_control_location = info->control_location ;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_decap_data_config_set(unit, port, queue_num, &decap_data_info));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_dbm_entry_set(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dual_bitmap_rule_entry_set(unit, port, profile_id, profile));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_dbm_entry_get(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dual_bitmap_rule_entry_get(unit, port, profile_id, profile));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_secondary_dbm_entry_set(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_secondary_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_secondary_dual_bitmap_rule_entry_set(unit, port, profile_id, profile));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_secondary_dbm_entry_get(int unit, int port, pm_info_t pm_info, uint32 profile_id, cprimod_secondary_dbm_rule_entry_t* profile)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_secondary_dual_bitmap_rule_entry_get(unit, port, profile_id, profile));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_modulo_rule_entry_set(int unit, int port, pm_info_t pm_info,
                                                  uint32 modulo_rule_num,
                                                  cprimod_transmission_rule_type_t modulo_rule_type,
                                                  const cprimod_modulo_rule_entry_t* modulo_rule)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_entry_set(unit, port, modulo_rule_num, modulo_rule_type, modulo_rule));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_modulo_rule_entry_get(int unit, int port, pm_info_t pm_info,
                                                  uint32 modulo_rule_num,
                                                  cprimod_transmission_rule_type_t modulo_rule_type,
                                                  cprimod_modulo_rule_entry_t* modulo_rule)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_modulo_rule_entry_get(unit, port, modulo_rule_num, modulo_rule_type, modulo_rule));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_rsvd4_dbm_position_entry_set(int unit, int port, pm_info_t pm_info,
                                                   uint32 index,
                                                   cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_entry_set(unit, port, index, pos_entry));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_dbm_position_entry_get(int unit, int port, pm_info_t pm_info,
                                                   uint32 index,
                                                   cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_dbm_position_entry_get(unit, port, index, pos_entry));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_master_frame_sync_config_set(int unit, int port, pm_info_t pm_info, uint32 master_frame_number, uint64 master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_master_frame_sync_config_set(unit, port, master_frame_number, master_frame_start_time));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rsvd4_rx_master_frame_sync_config_get(int unit, int port, pm_info_t pm_info, uint32* master_frame_number, uint64* master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_master_frame_sync_config_get(unit, port, master_frame_number, master_frame_start_time));
exit:
    SOC_FUNC_RETURN;
}

/*
 * Control Word Processing.
 */

int cpm4x25_cpri_port_rx_cw_sync_info_get(int unit, int port, pm_info_t pm_info, uint32* hyper_frame_num, uint32* radio_frame_num)
{
    uint32 basic_frame_num=0;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_sync_info_get(unit, port, &basic_frame_num, hyper_frame_num,radio_frame_num));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_cw_l1_signal_info_get(int unit, int port, pm_info_t pm_info, portmod_cpri_cw_l1_protocol_info_t* l1_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_l1_signal_info_get(unit, port, l1_info));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_cw_l1_signal_signal_protection_set(int unit, int port, pm_info_t pm_info, uint32 signal_map, uint32 enable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_l1_signal_signal_protection_set(unit, port, signal_map,enable));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_cw_l1_signal_signal_protection_get(int unit, int port, pm_info_t pm_info, uint32 signal_map, uint32* enable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_rx_control_word_l1_signal_signal_protection_get(unit, port, signal_map,enable));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_cw_slow_hdlc_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_slow_hdlc_config_info_t* config_info)
{
    cprimod_slow_hdlc_config_info_t hdlc_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = config_info->cycle_size;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    hdlc_config.tx_crc_mode     = config_info->tx_crc_mode;
    hdlc_config.tx_flag_size    = config_info->tx_flag_size;
    hdlc_config.crc_init_val    = config_info->crc_init_val;
    hdlc_config.use_fe_mac      = config_info->use_fe_mac;
    hdlc_config.crc_byte_swap   = config_info->crc_byte_swap;
    hdlc_config.cw_sel          = config_info->cw_sel;
    hdlc_config.cw_size         = config_info->cw_size;
    hdlc_config.fcs_size        = config_info->fcs_size;
    hdlc_config.runt_frame_drop = config_info->runt_frame_drop;
    hdlc_config.long_frame_drop = config_info->long_frame_drop ;
    hdlc_config.min_frame_size  = config_info->min_frame_size;
    hdlc_config.max_frame_size  = config_info->max_frame_size;
    hdlc_config.queue_num       = config_info->queue_num ;

    hdlc_config.tx_filling_flag_pattern = config_info->tx_filling_flag_pattern ;
    hdlc_config.no_fcs_err_check        = config_info->no_fcs_err_check ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_cw_slow_hdlc_config_set(unit, port, &hdlc_config));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_cw_slow_hdlc_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_slow_hdlc_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_cw_fast_eth_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_fast_eth_config_info_t* config_info)
{
    cprimod_fast_eth_config_info_t eth_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = config_info->cycle_size;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;
    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    eth_config.tx_crc_mode      = config_info->tx_crc_mode;
    eth_config.schan_start      = config_info->schan_start;
    eth_config.schan_size       = config_info->schan_size;
    eth_config.cw_sel           = config_info->cw_sel;
    eth_config.cw_size          = config_info->cw_size;
    eth_config.min_packet_size  = config_info->min_packet_size;
    eth_config.max_packet_size  = config_info->max_packet_size;
    eth_config.min_packet_drop  = config_info->min_packet_drop;
    eth_config.max_packet_drop  = config_info->max_packet_drop;
    eth_config.strip_crc        = config_info->strip_crc;
    eth_config.min_ipg          = config_info->min_ipg;
    eth_config.queue_num        = config_info->queue_num ;
    eth_config.no_fcs_err_check = config_info->no_fcs_err_check ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_cw_fast_eth_config_set(unit, port, &eth_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_cw_fast_eth_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_fast_eth_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_rx_vsd_config_info_t* config_info)
{
    cprimod_cpri_rx_vsd_config_info_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    vsd_config.schan_start = config_info->schan_start ;
    vsd_config.schan_size  = config_info->schan_size ;
    vsd_config.schan_bytes = config_info->schan_bytes ;
    vsd_config.schan_steps = config_info->schan_steps ;
    vsd_config.flow_bytes  = config_info->flow_bytes ;
    vsd_config.queue_num   = config_info->queue_num ;
    vsd_config.rsvd_sector_mask[0] = config_info->rsvd_sector_mask[0] ;
    vsd_config.rsvd_sector_mask[1] = config_info->rsvd_sector_mask[1] ;
    vsd_config.rsvd_sector_mask[2] = config_info->rsvd_sector_mask[2] ;
    vsd_config.rsvd_sector_mask[3] = config_info->rsvd_sector_mask[3] ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_cw_config_set(unit, port, &vsd_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_rx_vsd_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;



    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_tx_vsd_config_info_t* config_info)
{
    cprimod_cpri_tx_vsd_config_info_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.priority             = 0; /* not used for decap */

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    vsd_config.schan_start = config_info->schan_start ;
    vsd_config.schan_size  = config_info->schan_size ;
    vsd_config.schan_bytes = config_info->schan_bytes ;
    vsd_config.schan_steps = config_info->schan_steps ;
    vsd_config.flow_bytes  = config_info->flow_bytes ;
    vsd_config.queue_num   = config_info->queue_num ;

    vsd_config.rsvd_sector_mask[0] = config_info->rsvd_sector_mask[0] ;
    vsd_config.rsvd_sector_mask[1] = config_info->rsvd_sector_mask[1] ;
    vsd_config.rsvd_sector_mask[2] = config_info->rsvd_sector_mask[2] ;
    vsd_config.rsvd_sector_mask[3] = config_info->rsvd_sector_mask[3] ;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_cw_config_set(unit, port, &vsd_config));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_tx_vsd_config_info_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_flow_add(int unit, int port, pm_info_t pm_info, uint32 group_id, const portmod_cpri_rx_vsd_flow_info_t* flow_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_ctrl_flow_add(unit, port, group_id, flow_info));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_vsd_flow_add(int unit, int port, pm_info_t pm_info, uint32 group_id, const portmod_cpri_tx_vsd_flow_info_t* flow_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_ctrl_flow_add(unit, port, group_id, flow_info));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_encap_queue_control_tag_to_flow_map_set(int unit, int port, pm_info_t pm_info, uint32 tag_id, uint32 flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_control_queue_tag_to_flow_id_map_set(unit, port, tag_id, flow_id));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_encap_queue_control_tag_to_flow_map_get(int unit, int port, pm_info_t pm_info, uint32 tag_id, uint32* flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_encap_control_queue_tag_to_flow_id_map_get(unit, port, tag_id, flow_id));
exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_rx_vsd_raw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprimod_cpri_rx_vsd_raw_config_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));


    vsd_config.schan_start          = config_info->schan_start ;
    vsd_config.schan_size           = config_info->schan_size ;
    vsd_config.cw_sel               = config_info->cw_sel;
    vsd_config.cw_size              = config_info->cw_size;
    vsd_config.filter_mode          = config_info->filter_mode;
    vsd_config.hyper_frame_index    = config_info->hyper_frame_index;
    vsd_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;
    vsd_config.match_offset         = config_info->match_offset;
    vsd_config.match_value          = config_info->match_value;
    vsd_config.match_mask           = config_info->match_mask;
    vsd_config.queue_num            = config_info->queue_num;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_vsd_raw_config_set(unit, port, index, &vsd_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_vsd_raw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_rx_vsd_raw_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */


    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_vsd_raw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_tx_vsd_raw_config_t* config_info)
{
    cprimod_cpri_tx_vsd_raw_config_t vsd_config;
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.priority             = 0; /* not used for decap */

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));


    vsd_config.schan_start          = config_info->schan_start ;
    vsd_config.schan_size           = config_info->schan_size ;
    vsd_config.cw_sel               = config_info->cw_sel;
    vsd_config.cw_size              = config_info->cw_size;
    vsd_config.map_mode             = config_info->map_mode;
    vsd_config.hyper_frame_index    = config_info->hyper_frame_index;
    vsd_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;
    vsd_config.repeat_enable        = config_info->repeat_enable;
    vsd_config.bfn0_filter_enable   = config_info->bfn0_filter_enable;
    vsd_config.bfn1_filter_enable   = config_info->bfn1_filter_enable;
    vsd_config.queue_num            = config_info->queue_num;
    vsd_config.idle_value           = config_info->idle_value;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_raw_config_set(unit, port, index, &vsd_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_raw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_tx_vsd_raw_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_vsd_raw_filter_set(int unit, int port, pm_info_t pm_info, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_vsd_raw_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_brcm_rsvd5_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_rx_brcm_rsvd5_config_t* config_info)
{
    cprimod_encap_decap_data_config_t encap_decap_data;

    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config_info->queue_size;
    encap_decap_data.priority             = config_info->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_brcm_rsvd5_control_config_set(unit, port,
                                                          config_info->schan_start,
                                                          config_info->schan_size,
                                                          config_info->queue_num,
                                                          config_info->parity_disable));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_brcm_rsvd5_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_rx_brcm_rsvd5_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */


    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_brcm_rsvd5_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_tx_brcm_rsvd5_config_t* config_info)
{
    cprimod_encap_decap_data_config_t encap_decap_data;
    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.cycle_size           = config_info->cycle_size;
    encap_decap_data.priority             = 0; /* not used for decap */

    /* added 16 bytes to the buffer.
     * The extra space is required to store the RoE Header for Tx Framer
     */
    encap_decap_data.buffer_size          = config_info->buffer_size + 16;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_decap_data_config_set(unit, port, config_info->queue_num, &encap_decap_data));

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_brcm_rsvd5_control_config_set(unit, port,
                                                          config_info->schan_start,
                                                          config_info->schan_size,
                                                          config_info->queue_num,
                                                          config_info->crc_enable));

exit:
    SOC_FUNC_RETURN;
}

int cpm4x25_cpri_port_tx_brcm_rsvd5_config_get(int unit, int port, pm_info_t pm_info, portmod_cpri_tx_brcm_rsvd5_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_gcw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_rx_gcw_config_t* config_info)
{
    cprimod_cpri_rx_gcw_config_t gcw_config;
    SOC_INIT_FUNC_DEFS;

    gcw_config.Ns   = config_info->Ns;
    gcw_config.Xs   = config_info->Xs;
    gcw_config.Y    = config_info->Y;
    gcw_config.mask = config_info->mask;

    gcw_config.filter_mode          = config_info->filter_mode;
    gcw_config.hyper_frame_index    = config_info->hyper_frame_index;;
    gcw_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;;
    gcw_config.match_mask           = config_info->match_mask;
    gcw_config.match_value          = config_info->match_value;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_rx_gcw_config_set(unit, port, index, &gcw_config));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_rx_gcw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_tx_gcw_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */


    SOC_FUNC_RETURN;

}


int cpm4x25_cpri_port_tx_gcw_config_set(int unit, int port, pm_info_t pm_info, uint32 index, const portmod_cpri_tx_gcw_config_t* config_info)
{
    cprimod_cpri_tx_gcw_config_t gcw_config;
    SOC_INIT_FUNC_DEFS;

    gcw_config.Ns   = config_info->Ns;
    gcw_config.Xs   = config_info->Xs;
    gcw_config.Y    = config_info->Y;
    gcw_config.mask = config_info->mask;

    gcw_config.hyper_frame_index    = config_info->hyper_frame_index;;
    gcw_config.hyper_frame_modulo   = config_info->hyper_frame_modulo;;
    gcw_config.repeat_enable        = config_info->repeat_enable;
    gcw_config.bfn0_filter_enable   = config_info->bfn0_filter_enable;
    gcw_config.bfn1_filter_enable   = config_info->bfn1_filter_enable;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_config_set(unit, port, index, &gcw_config));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_gcw_config_get(int unit, int port, pm_info_t pm_info, uint32 index, portmod_cpri_tx_gcw_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    /* Place your code here */

    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_gcw_filter_set(int unit, int port, pm_info_t pm_info, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_gcw_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_cw_sync_info_set(int unit, int port, pm_info_t pm_info, const uint32 hyper_frame_num, const uint32 radio_frame_num)
{
    uint8 hfn=0;
    uint16 bfn=0;
    SOC_INIT_FUNC_DEFS;

    hfn = hyper_frame_num & 0xFF;
    bfn = radio_frame_num & 0xFFFF;

    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_hfn_set(unit,port,hfn));
    _SOC_IF_ERR_EXIT
        (cprimod_tx_framer_tgen_next_bfn_set(unit,port,bfn));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_cpri_port_tx_cw_l1_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_cw_l1_config_info_t* control_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_port_tx_control_word_set(unit, port, control_info));

exit:
    SOC_FUNC_RETURN;

}

/* BRCM_RSVD4 */
int cpm4x25_rsvd4_rx_control_flow_add (int unit, int port, pm_info_t pm_info, uint32 flow_id, const portmod_control_flow_config_t* config)
{
    cprimod_control_flow_config_t flow_config;
    cprimod_encap_decap_data_config_t encap_decap_data;
    cprimod_header_compare_entry_t hdr_entry;
    cprimod_cpri_tag_option_t tag_option;
    SOC_INIT_FUNC_DEFS;

    encap_decap_data.sample_size          = 16;
    encap_decap_data.truncation_enable    = FALSE;
    encap_decap_data.truncation_type      = 0;
    encap_decap_data.compression_type     = cprimod_no_compress;
    encap_decap_data.mux_enable           = 0;
    encap_decap_data.bit_reversal         = FALSE;
    encap_decap_data.buffer_size          = config->queue_size;
    encap_decap_data.priority             = config->priority;
    encap_decap_data.cycle_size           = 0; /* not used for encap */

    _SOC_IF_ERR_EXIT
        (cprimod_cpri_encap_data_config_set(unit, port, config->queue_num, &encap_decap_data));

    flow_config.proc_type       = config->proc_type;
    flow_config.queue_num       = config->queue_num;
    flow_config.sync_profile    = config->sync_profile;
    flow_config.sync_enable     = config->sync_enable;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_control_flow_config_set(unit, port, flow_id, &flow_config));

    /*
     *  Set tag option to use default tag or look up.
     */
    if ((config->proc_type == PORTMOD_CONTROL_MSG_PROC_TYPE_PAYLOAD) ||
        (config->proc_type == PORTMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE_W_TAG)) {
        tag_option = CPRIMOD_CPRI_TAG_LOOKUP;
    } else {
        tag_option = CPRIMOD_CPRI_TAG_DEFAULT;
    }

    _SOC_IF_ERR_EXIT
        (cprimod_rx_control_flow_tag_option_set(unit, port, flow_id, tag_option));

    hdr_entry.valid = 1;
    hdr_entry.match_data = config->match_data;
    hdr_entry.mask = config->match_mask;
    hdr_entry.flow_id = flow_id;
    hdr_entry.flow_type = cprimodFlowTypeCtrl;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_add(unit, port,
                                        &hdr_entry));

exit:
    SOC_FUNC_RETURN;

}
int cpm4x25_rsvd4_rx_control_flow_delete(int unit, int port, pm_info_t pm_info, uint32 flow_id,const portmod_control_flow_config_t* config)
{
    cprimod_header_compare_entry_t hdr_entry;

    SOC_INIT_FUNC_DEFS;

    hdr_entry.valid = 1;
    hdr_entry.match_data = config->match_data;
    hdr_entry.mask = config->match_mask;
    hdr_entry.flow_id = flow_id;
    hdr_entry.flow_type = cprimodFlowTypeCtrl;

    _SOC_IF_ERR_EXIT
        (cprimod_header_compare_entry_delete(unit, port,
                                           &hdr_entry));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_fast_eth_config_set(int unit, int port, pm_info_t pm_info, const portmod_cpri_fast_eth_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_fast_eth_config_set(unit, port, config_info));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_rx_sync_profile_entry_set(int unit, int port, pm_info_t pm_info, uint32 profile_id, const portmod_sync_profile_entry_t* entry)
{
    cprimod_sync_profile_entry_t sync_profile;
    SOC_INIT_FUNC_DEFS;

    sync_profile.count_cycle            = entry->count_cycle;
    sync_profile.message_offset         = entry->message_offset;
    sync_profile.master_frame_offset    = entry->master_frame_offset;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_sync_profile_entry_get(unit, port, profile_id, &sync_profile));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_rx_sync_profile_entry_get(int unit, int port, pm_info_t pm_info, uint32 profile_id, portmod_sync_profile_entry_t* entry)
{
    cprimod_sync_profile_entry_t sync_profile;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_rx_sync_profile_entry_get(unit, port, profile_id, &sync_profile));

    entry->count_cycle          =  sync_profile.count_cycle;
    entry->message_offset       = sync_profile.message_offset;
    entry->master_frame_offset  =  sync_profile.master_frame_offset;

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rx_tag_config_set(int unit, int port, pm_info_t pm_info, uint32 default_tag, uint32 no_match_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_tag_config_set(unit, port, default_tag, no_match_tag));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rx_tag_gen_entry_add(int unit, int port, pm_info_t pm_info, const portmod_tag_gen_entry_t* entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_tag_gen_entry_add(unit, port, entry));
exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rx_tag_gen_entry_delete(int unit, int port, pm_info_t pm_info, const portmod_tag_gen_entry_t* entry)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rx_tag_gen_entry_delete(unit, port, entry));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_group_member_add(int unit, int port, pm_info_t pm_info,
                                                   uint32 group_id, uint32 priority,
                                                   uint32 queue_num,
                                                   portmod_control_msg_proc_type_t proc_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_group_member_add(unit , port, group_id,
                                                      priority, queue_num,
                                                      proc_type));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rsvd4_tx_control_flow_group_member_delete(int unit, int port, pm_info_t pm_info,
                                                      uint32 group_id,
                                                      uint32 priority)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_group_member_delete(unit , port, group_id, priority));
exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_eth_message_config_set(int unit, int port, pm_info_t pm_info, uint32 msg_node, uint32 msg_subnode, uint32 msg_type, uint32 msg_padding)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_eth_message_config_set(unit, port, msg_node, msg_subnode, msg_type, msg_padding));

exit:
    SOC_FUNC_RETURN;

}


int cpm4x25_rsvd4_tx_single_raw_message_config_set(int unit, int port, pm_info_t pm_info, uint32 msg_id, uint32 msg_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_single_raw_message_config_set(unit, port, msg_id, msg_type));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_config_set(int unit, int port, pm_info_t pm_info, uint32 flow_id, uint32 queue_num, portmod_control_msg_proc_type_t proc_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_config_set(unit, port, flow_id, queue_num, proc_type));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_config_get(int unit, int port, pm_info_t pm_info, uint32 flow_id, uint32* queue_num, portmod_control_msg_proc_type_t* proc_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_config_get(unit, port, flow_id, queue_num, proc_type));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_header_index_set(int unit, int port, pm_info_t pm_info, uint32 roe_flow_id, uint32 index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_header_index_set(unit, port, roe_flow_id, index));


exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_flow_header_index_get(int unit, int port, pm_info_t pm_info, uint32 roe_flow_id, uint32* index)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_flow_header_index_get(unit, port, roe_flow_id, index));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_header_entry_set(int unit, int port, pm_info_t pm_info, uint32 index, uint32 header_node, uint32 header_subnode, uint32 payload_node)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_header_entry_set(unit, port, index, header_node, header_subnode, payload_node));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_control_header_entry_get(int unit, int port, pm_info_t pm_info, uint32 index, uint32* header_node, uint32* header_subnode, uint32* payload_node)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_control_header_entry_get(unit, port, index, header_node, header_subnode, payload_node));

exit:
    SOC_FUNC_RETURN;

}

int cpm4x25_rsvd4_tx_single_tunnel_message_config_set(int unit, int port, pm_info_t pm_info, cprimod_cpri_crc_option_t crc_option)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprimod_rsvd4_tx_single_tunnel_message_config_set(unit, port, crc_option));

exit:
    SOC_FUNC_RETURN;

}


#endif /* PORTMOD_CPM4X25_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
