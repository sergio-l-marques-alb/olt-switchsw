/* 
 * $Id:$
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        portctrl.c
 * Purpose:     SDK Port Control Layer
 *
 *              The purpose is to encapsulate port functionality
 *              related to the xxPORT block (i.e. XLPORT, CPORT)
 *              MAC and PHY.
 *
 *              Currently, only the PortMod library is being supported.
 *              The PortMod library provides support for the MAC, PHY,
 *              and xxPORT registers.
 *
 */

#include <soc/error.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/port.h>

#ifdef PORTMOD_SUPPORT
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/phy/phymod_port_control.h>


/* Indicates if the Port Control module has been initalized */
static int portctrl_init[SOC_MAX_NUM_DEVICES];

/*
 * Define:
 *      PORTCTRL_PORT_RESOLVE
 * Purpose:
 *      Converts a SOC port into port type used in PortMod functions '_pport'
 */
#define PORTCTRL_PORT_TO_PPORT(_port, _pport) do {  \
        _pport = _port;                             \
    } while (0)

static phymod_bus_t portmod_ext_default_bus = {
    "MDIO Bus",
    portmod_common_phy_mdio_c45_reg_read,
    portmod_common_phy_mdio_c45_reg_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};


extern uint32_t phymod_dbg_mask;
extern uint32_t phymod_dbg_addr;
extern uint32_t phymod_dbg_lane;

/*
 * Function:
 *      soc_esw_portctrl_init_check
 * Purpose:
 *      check if portctrl is already initialized.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE  : Initialized
 *      SOC_E_INIT  : Not Initialized.
 */
int
soc_esw_portctrl_init_check(int unit)
{
    if (SOC_PORTCTRL_NOT_INITIALIZED == portctrl_init[unit]) {
        return SOC_E_INIT;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_portctrl_pm_user_access_alloc
 * Purpose:
 *      Allocate SW data structure for the port macros (PM).
 * Parameters:
 *      unit        - (IN) Unit number.
 *      num         - (IN) Number of PM port macros in device.
 *      user_acc    - (OUT) Returns allocated PMs structures.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_pm_user_access_alloc(int unit, int num,
                                      portmod_default_user_access_t **user_acc)
{
    if (*user_acc == NULL) {
        *user_acc = sal_alloc(sizeof(portmod_default_user_access_t) * num,
                             "PortMod PM");
        if (*user_acc == NULL) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(*user_acc, 0, sizeof(portmod_default_user_access_t) * num);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_portctrl_dump_txrx_lane_map
 * Purpose:
 *      Utility function to dump tx, rx lane map
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
void
soc_esw_portctrl_dump_txrx_lane_map(int unit, int first_port, int logical_port,
              int core_num, uint32 txlane_map_b, uint32 rxlane_map_b,
              uint32 txlane_map, uint32 rxlane_map)
{
    static int hdr_flag =0;
    if (!hdr_flag) {
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_UP(unit, logical_port,
            "+-------+-------+-------+-------+------+--------+--------+\n")));
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_UP(unit, logical_port,
            "| PPORT | LPORT | TXMAP | RXMAP | CORE | TXSWAP | RXSWAP |\n")));
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_UP(unit, logical_port,
            "+-------+-------+-------+-------+------+--------+--------+\n")));
        hdr_flag=1;
    }
    LOG_ERROR(BSL_LS_SOC_PORT,
        (BSL_META_UP(unit, logical_port,
         "| %04d  | %04d  | %04X  | %04X  | %02d   | %04X   | %04X   |\n"),
         first_port, logical_port, txlane_map_b, rxlane_map_b, core_num,
         txlane_map, rxlane_map));

    LOG_ERROR(BSL_LS_SOC_PORT,
        (BSL_META_UP(unit, logical_port,
        "+-------+-------+-------+-------+------+--------+--------+\n")));
    return;
}

/*
 * Function:
 *      soc_esw_portctrl_ext_phy_config_parameter_get
 * Purpose:
 *      To extract external phy parameter from config.bcm.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Logical Port Number 
 *      phy_addr         - (OUT) Ext Phy MDIO address. 0xFF mean no ext phy    
 *      shift            - (IN)  Phy Port Map Shift from internal map. ( default 0) 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
soc_esw_portctrl_ext_phy_config_parameter_get (int unit, int port,
                   uint32 *phy_addr, uint32 *num_int_cores, uint32 shift[3])
{
    char *config_str, *sub_str, *sub_str_end;
    int idx;

    /* set default return values. */
    *num_int_cores  = 1;
    shift[0]        = 0;
    shift[1]        = 0;
    shift[2]        = 0;

    config_str = soc_property_port_get_str(unit, port, spn_PORT_PHY_ADDR);
    if (config_str == NULL) {
        *phy_addr = 0xFF; /* NO PHY */
        return SOC_E_NONE;        
    }

    /*
    * port_phy_addr_<port>=<phy addr>:<num_int_cores>:<shift0>:<shift1>:<shift2>
    */
    sub_str = config_str;

    /* Parse phy address  number */
    *phy_addr  = sal_ctoi(sub_str, &sub_str_end);
    
    if (*sub_str_end == '\0')
    {
        return SOC_E_NONE ;
    }
 
    /* Skip ':' between physical port number and num_internal_cores */
    sub_str = sub_str_end;
    if (*sub_str != '\0') {
        if (*sub_str != ':') {
            LOG_CLI((BSL_META_U(unit, "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                return SOC_E_FAIL;
        }
        sub_str++;
    } else {
        return SOC_E_NONE;
    }

    /* parse num of cores. */
    *num_int_cores = sal_ctoi(sub_str, &sub_str_end);
    if (*num_int_cores > SOC_PM12X10_PM4X10_COUNT) {
        LOG_CLI((BSL_META_U(unit, 
                "Port %d: Bad config string bad num of cores \"%s\" %d \n"),
                port, config_str, *num_int_cores));
        return SOC_E_FAIL;

    }

    /* check for end of string. */
    if( *sub_str_end == '\0' )
    {
        return SOC_E_NONE ;
    }

    for (idx=0; idx < *num_int_cores; idx++) {
        /* Skip ':' between physical port number and num_internal_cores */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit, "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                return SOC_E_FAIL;
            }
            sub_str++;
        }
        shift[idx] = sal_ctoi(sub_str, &sub_str_end);
        /* check for end of string. */
        if( *sub_str_end == '\0' )
        {
            return SOC_E_NONE ;
        }
    } 
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_portctrl_setup_ext_phy_add(int unit, int first_port)
 * Purpose:
 *      Add External PHY info.
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      first_port       - (IN)  First port
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_setup_ext_phy_add(int unit, int first_port, portmod_dispatch_type_t type)
{
    int    rv, idx;
    int    xphy_logical_port;
    uint32 ext_phy_addr;
    uint32 num_int_cores;
    uint32 first_phy_lane[3];
    int phy_chain_length;
    phymod_core_access_t tmp_ext_phy_access;
    portmod_default_user_access_t* local_user_access;
    /* add Ext Phy to portmod. */
    xphy_logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];
    rv = soc_esw_portctrl_ext_phy_config_parameter_get(unit, xphy_logical_port,
                &ext_phy_addr, &num_int_cores,  first_phy_lane);
    phy_chain_length = soc_property_port_get(unit, xphy_logical_port, spn_PHY_CHAIN_LENGTH, 0);  
    if((PORTMOD_SUCCESS(rv)) && ((ext_phy_addr != 0xFF) || phy_chain_length)) {
 
#ifdef PORTMOD_PM4X10_SUPPORT
        if(portmodDispatchTypePm4x10 == type) {
            cli_out(" EXT PHY Add 4x10 for port %d  ext_phy 0x%x num_of_cores %d shift %d %d %d.\n",
                      xphy_logical_port,ext_phy_addr, num_int_cores,
                      first_phy_lane[0],first_phy_lane[1],first_phy_lane[2]);
        }
#endif
#ifdef PORTMOD_PM12X10_SUPPORT
        else if(portmodDispatchTypePm12x10 == type) {
            cli_out(" EXT PHY Add 12x10 for port %d  ext_phy 0x%x num_of_cores %d shift %d %d %d.\n",
                      xphy_logical_port,ext_phy_addr, num_int_cores,
                      first_phy_lane[0],first_phy_lane[1],first_phy_lane[2]);
        }
#endif
        phymod_core_access_t_init(&tmp_ext_phy_access);
        tmp_ext_phy_access.access.bus = &portmod_ext_default_bus;
        tmp_ext_phy_access.access.addr = ext_phy_addr; 
        tmp_ext_phy_access.type = phymodDispatchTypeCount; /* Make sure it is invalid. */
        local_user_access = sal_alloc(sizeof(portmod_default_user_access_t),
                                      "pm4x10_specific_db");
        sal_memset(local_user_access, 0, sizeof(portmod_default_user_access_t));
        local_user_access->unit = unit;
        tmp_ext_phy_access.access.user_acc = local_user_access;
        for(idx = 0 ; idx < num_int_cores;idx++) {
#ifdef PORTMOD_PM4X10_SUPPORT
            if(portmodDispatchTypePm4x10 == type) {
                rv = portmod_ext_phy_attach(unit, first_port,
                                            &tmp_ext_phy_access, first_phy_lane[idx]);
            }
#endif
#ifdef PORTMOD_PM12X10_SUPPORT
            else if(portmodDispatchTypePm12x10 == type) {
                rv = portmod_ext_phy_attach(unit, first_port+(idx*SOC_PM4X10_NUM_LANES),
                                            &tmp_ext_phy_access, first_phy_lane[idx]);
            }
#endif
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_esw_portctrl_reset_tsc0_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 0 means TSC core index0, first TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC0_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc0_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 0) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}

/*
 * Function:
 *      soc_esw_portctrl_reset_tsc1_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 1 means TSC core index1, 2nd TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC1_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc1_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 1) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}
 
/*
 * Function:
 *      soc_esw_portctrl_reset_tsc2_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 2 means TSC core index2, 3rd TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC2_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc2_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 2) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}
 
/*
 * Function:
 *      soc_esw_portctrl_reset_tsc3_cb
 * Purpose:
 *      Reset TSC by setting TSC control register.
 *      Index 3 means TSC core index3, 4th TSC ctrl register index,
 *      which resides in PGW block.
 *      For TD2+ case, the register is PGW_TSC3_CTRL_REG
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      port             - (IN)  Port number.
 *      in_reset         - (IN)  reset status
 * Returns:
 *      BCM_E_XXX
 */
int
soc_esw_portctrl_reset_tsc3_cb(int unit, int port, uint32 in_reset)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    return (!in_reset? soc_tsc_xgxs_reset(unit, port, 3) : SOC_E_NONE);
#else
    return SOC_E_NONE;
#endif
}


/*
 * Function:
 *      soc_esw_portctrl_config_get
 * Purpose:
 *      Get interface and initialization configuration
 *      for given logical port.
 * Parameters:
 *      unit              - (IN) Unit number.
 *      port              - (IN) Logical port number.
 *      interface_config  - (IN/OUT) Port interface configuration.
 *      init_config       - (IN/OUT) Port initialization configuration.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_config_get(int unit, soc_port_t port, 
                            portmod_port_interface_config_t* interface_config,
                            portmod_port_init_config_t* init_config)
{
    /* soc_port_if_t interface_type; */
    int is_hg = 0, port_num_lanes, fiber_pref = 0, is_scrambler = 0;
    int an_cl37 = 0, an_cl73=0, fs_cl72=0, max_speed ;
    /* phymod_tx_t *p_tx; */
    uint32  preemphasis, driver_current;
    /*int rxaui_mode = 0; */
 
    /* Initialize both interface config and init config */ 
    portmod_port_init_config_t_init(unit, init_config);
    portmod_port_interface_config_t_init(unit,interface_config);
    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];
    interface_config->port_num_lanes = port_num_lanes;

    is_hg = (PBMP_MEMBER(SOC_HG2_PBM(unit), port) ||
             PBMP_MEMBER(PBMP_HG_ALL(unit), port));
    if (is_hg) {
        PHYMOD_INTF_MODES_HIGIG_SET(interface_config);
    }

    fiber_pref = soc_property_port_get(unit, port,
                                       spn_SERDES_FIBER_PREF, fiber_pref);
    if (fiber_pref) {
        PHYMOD_INTF_MODES_FIBER_SET(interface_config);
    }  

    is_scrambler = soc_property_port_get(unit, port,
                                         spn_SERDES_SCRAMBLER_ENABLE, 0);
    if (is_scrambler) {
        PHYMOD_INTF_MODES_SCR_SET(interface_config);
    }  

    
#if 0 
    if (SOC_IS_FE1600_B0_AND_ABOVE(unit) &&
        SOC_DFE_CONFIG(unit).serdes_mixed_rate_enable) {
        /* Dual rate support */
        PHYMOD_INTF_MODES_OS2_SET(interface_config);
    }
#endif    
    if (IS_C_PORT(unit, port)) {
        PHYMOD_INTF_MODES_TRIPLE_CORE_SET(interface_config);

        switch(SOC_INFO(unit).port_100g_lane_config[port]) {
            case SOC_LANE_CONFIG_100G_4_4_2:
                PHYMOD_INTF_MODES_TC_442_SET(interface_config);
                break ;
            case SOC_LANE_CONFIG_100G_2_4_4:
                PHYMOD_INTF_MODES_TC_244_SET(interface_config);
                break ;
            case SOC_LANE_CONFIG_100G_3_4_3:
                PHYMOD_INTF_MODES_TC_343_SET(interface_config);
                break ;
            default:
                PHYMOD_INTF_MODES_TC_343_SET(interface_config);
                break ;
        }
        init_config->port_fallback_lane =
            SOC_INFO(unit).port_fallback_lane[port];
    }


    /* Should be changed based on the speed later in the flow  */
    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port))
        interface_config->interface =  SOC_PORT_IF_NULL;
    else 
        interface_config->interface = soc_property_port_get(unit, port,
                                                        spn_SERDES_IF_TYPE,
                                                        SOC_PORT_IF_XFI);

    max_speed = SOC_INFO(unit).port_speed_max[port];
    /*
    if(is_hg) {
        switch(max_speed) {
        case 10000: max_speed=11000 ; break ;
        case 20000: max_speed=21000 ; break ;
        case 40000: max_speed=42000 ; break ;
        case 100000: max_speed=107000 ; break ;
        case 120000: max_speed=127000 ; break ;
        default:  break ;
        }
    }
    */
    interface_config->max_speed = max_speed ;

    /* rxaui_mode = soc_property_port_get(unit, port,
                                          spn_SERDES_RXAUI_MODE, 1);*/


    /* Fill in init config */
    an_cl37 = soc_property_port_get(unit, port,
                                    spn_PHY_AN_C37, an_cl37);
    an_cl73 = soc_property_port_get(unit, port,
                                    spn_PHY_AN_C73, an_cl73);
    if(is_hg) {
        init_config->an_mode = phymod_AN_MODE_CL37BAM ;
    } else {
        init_config->an_mode = phymod_AN_MODE_CL73 ;
        init_config->an_cl72 = 1 ;
    }
    if(an_cl73) {
        init_config->an_mode = an_cl73;
        init_config->an_cl72 = 1 ;
    } else if(an_cl37) {
        init_config->an_mode = an_cl37;
        init_config->an_cl72 = 0 ;
    }
    
    init_config->an_cl72 = soc_property_port_get(unit, port,
                                                 spn_PHY_AN_C72,
                                                 init_config->an_cl72);
    init_config->an_fec = soc_property_port_get(unit, port,
                                                spn_PHY_AN_FEC,
                                                init_config->an_fec);
    fs_cl72             = soc_property_port_get(unit, port,
                                                spn_PORT_INIT_CL72,
                                                fs_cl72);
    if(fs_cl72) {
        init_config->fs_cl72 = PHYMOD_INTF_F_CL72_REQUESTED_BY_CNFG ;
    }

    interface_config->speed =
        soc_property_port_get(unit, port,
                              spn_PORT_INIT_SPEED,
                              interface_config->max_speed);

    init_config->pll_divider_req      =
        soc_property_port_get(unit, port, spn_XGXS_PHY_PLL_DIVIDER, 0xA);
    
    init_config->serdes_1000x_at_6250_vco =
        soc_property_port_get(unit, port,
                              spn_SERDES_1000X_AT_6250_VCO,
                              init_config->serdes_1000x_at_6250_vco);

    init_config->cx4_10g = soc_property_port_get(unit, port,
                                                 spn_10G_IS_CX4, TRUE);

    /*
     * Update this to add all possible interfaces here 
     */
    if (port_num_lanes == 1) {
        if (interface_config->speed >= 10000) {
            interface_config->interface = fiber_pref ?
                SOC_PORT_IF_SFI : SOC_PORT_IF_XFI;
        }
    } else if (port_num_lanes == 2) {
        /* Add the interface later for 20G ports*/
        interface_config->interface = SOC_PORT_IF_RXAUI;
    } else if (port_num_lanes == 3) {
        /* Add the interface later (if valid )*/
    } else if (port_num_lanes == 4) {
        if (interface_config->speed >= 40000) {
            interface_config->interface = SOC_PORT_IF_XLAUI;
        } else if (interface_config->speed >= 10000) {
            interface_config->interface = 
                  fiber_pref? SOC_PORT_IF_SFI : SOC_PORT_IF_XFI;
        }
    } else if (port_num_lanes == 10) {  /* 100G */
            interface_config->interface = SOC_PORT_IF_CAUI;
    } else if (port_num_lanes == 12) {  /* 120G */
            interface_config->interface = SOC_PORT_IF_CAUI;
    } else {
        /* Do Nothing - let it be default */
    }
    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port))
        interface_config->interface =  SOC_PORT_IF_NULL;

    preemphasis = 0x0;
    driver_current = 0x0;
    preemphasis = soc_property_port_get(unit, port,
                                        spn_SERDES_PREEMPHASIS, preemphasis);
    driver_current = soc_property_port_get(unit, port,
                                           spn_SERDES_DRIVER_CURRENT,
                                           driver_current);
    init_config->tx_params_user_flag = 0;
    if(preemphasis) {
        init_config->tx_params_user_flag |=
            PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG ;
        init_config->tx_params.pre  = preemphasis & 0xff;
        init_config->tx_params.main = (preemphasis & 0xff00) >> 8;
        init_config->tx_params.post = (preemphasis & 0xff0000) >> 16;
    }
    if(driver_current){
        init_config->tx_params_user_flag |= PORTMOD_USER_SET_TX_AMP_BY_CONFIG;
        init_config->tx_params.amp  = driver_current;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_portctrl_pm_portmod_init
 *
 * Purpose:
 *      Call device specific portmod init function
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_UNAVAIL : if fuction vector is not correctly set
 *      rv            : return rv
 */
STATIC int
soc_portctrl_pm_portmod_init(int unit)
{
    int rv = SOC_E_NONE;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_init)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_init(unit);
    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv; 
}

/*
 * Function:
 *      soc_portctrl_pm_portmod_deinit
 *
 * Purpose:
 *      Call device specific portmod deinit function
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_UNAVAIL : if fuction vector is not correctly set
 *      rv            : return rv
 */
STATIC int
soc_portctrl_pm_portmod_deinit(int unit)
{
    int rv = SOC_E_NONE;

    if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
        (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_deinit)) {
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_deinit(unit);
    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv; 
}

extern soc_portctrl_functions_t soc_td2p_portctrl_func;
extern soc_portctrl_functions_t soc_th_portctrl_func;
extern soc_portctrl_functions_t soc_gh_portctrl_func;


/*
 * Function:
 *      soc_portctrl_functions_register
 *
 * Purpose:
 *      This will set given function vector to soc_control->soc_portctrl_functions
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      func     - (IN) device specific portctrl function vectors
 * Returns:
 *      SOC_E_NONE
 */
int
soc_esw_portctrl_functions_register(int unit, soc_driver_t *drv)
{
    switch (drv->type) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    case SOC_CHIP_BCM56860_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_td2p_portctrl_func;
        break;
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    case SOC_CHIP_BCM56960_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_th_portctrl_func;
        break;
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    case SOC_CHIP_BCM53400_A0:
        SOC_PORTCTRL_FUNCTIONS(unit) = &soc_gh_portctrl_func;
        break;
#endif
    default:
        break;
    }
    return SOC_E_NONE;
}
#endif /* PORTMOD_SUPPORT */

/*
 * Function:
 *      soc_esw_portctrl_init
 * Purpose:
 *      Initialize the Port Control component and
 *      corresponding library (PortMod library).
 *      1. call soc_portctrl_pm_portmod_init
 *      2. set portctrl_init status to SOC_PORTCTRL_INITIALIZED.
 *
 *      This is to be called by the SOC Port module initialization
 *      function soc_misc_init().
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_init(int unit)
{
#ifdef PORTMOD_SUPPORT
    int rv;

    /* Call Device specific PortMod library initialization */
    rv = soc_portctrl_pm_portmod_init(unit);
    /* Successful */
    if (SOC_SUCCESS(rv)) {
        portctrl_init[unit] = SOC_PORTCTRL_INITIALIZED;
    }
    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_deinit
 * Purpose:
 *      Uninitialize the Port Control component and
 *      corresponding library (PortMod library).
 *      1. call soc_portctrl_pm_portmod_deinit
 *      2. set portctrl_init status to SOC_PORTCTRL_NOT_INITIALIZED.
 *
 *      This function must be called by the BCM Port module
 *      deinit (or detach) function.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_deinit(int unit)
{
#ifdef PORTMOD_SUPPORT
    int rv;

    if (SOC_PORTCTRL_NOT_INITIALIZED == portctrl_init[unit]) {
        return SOC_E_NONE;
    }

    /* Call Device specific PortMod library uninitialization */
    rv = soc_portctrl_pm_portmod_deinit(unit);
    portctrl_init[unit] = SOC_PORTCTRL_NOT_INITIALIZED;

    return rv;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_portctrl_software_deinit
 * Purpose:
 *      Release portmod software resource
 * Parameters:
 *      unit - SOC Unit #.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_software_deinit(int unit)
{
#ifdef PORTMOD_SUPPORT
    return portmod_destroy(unit);

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_add
 * Purpose:
 *      Add given logical port to PM (Port Macro).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Port number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assume port is disabled.
 *      Assume port has been removed from PM.
 *      Assume caller has checked that PortCtrl was initialized.
 *      SOC_INFO SW data structure has been updated.
 */
int
soc_esw_portctrl_add(int unit, soc_port_t port)
{
#ifdef PORTMOD_SUPPORT
    portctrl_pport_t pport;
    portmod_port_add_info_t add_info;
    int phy_port;
    int num_lanes;
    int lane;
    int encap_mode;

    /* Convert port to PortMod port */
    PORTCTRL_PORT_TO_PPORT(port, pport);

    PORTMOD_IF_ERROR_RETURN
        (portmod_port_add_info_t_init(unit, &add_info));

    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_config_get(unit, port,
                                     &(add_info.interface_config),
                                     &(add_info.init_config)));

    add_info.phy_op_mode = soc_property_port_get(unit, port,
                                                 spn_PHY_PCS_REPEATER, 
                                                 phymodOperationModeRetimer);

    add_info.phy_op_datapath= soc_property_port_get(unit, port,
                                                    spn_PHY_ULL_DATAPATH,
                                                    phymodDatapathNormal);

    add_info.sys_interface= soc_property_port_get(unit, port,
                                                  spn_PHY_SYS_INTERFACE, 
                                                  phymodInterfaceXFI);

    if (IS_HG_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_no_higig_plus)) {
            encap_mode = SOC_ENCAP_HIGIG2;
        } else {
            encap_mode = soc_property_port_get(unit, port,
                         spn_HIGIG2_HDR_MODE, 0) ?
                         SOC_ENCAP_HIGIG2 : SOC_ENCAP_HIGIG;
        }
        add_info.interface_config.encap_mode = encap_mode;
    }
    /*
     * Add port to PM
     */
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    num_lanes = SOC_INFO(unit).port_num_lanes[port];
    if (num_lanes == 10) {
        num_lanes = 12;
    }
    /*
     * Set lane information
     * Assume lanes are arranged consecutively with respect to
     * the physical port number.
     */
    for (lane = 0 ; lane < num_lanes; lane++) { 
        SOC_PBMP_PORT_ADD(add_info.phys, phy_port + lane);
    }
    if (SAL_BOOT_SIMULATION) {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&add_info);
    }

    PORTMOD_IF_ERROR_RETURN(portmod_port_add(unit, pport, &add_info));

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_delete
 * Purpose:
 *      Remove given logical port from PM (Port Macro).
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Port number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assume port is disabled.
 *      Assume caller has checked that PortCtrl was initialized.
 *      SOC_INFO SW data structure has been updated.
 */
int
soc_esw_portctrl_delete(int unit, soc_port_t port)
{
#ifdef PORTMOD_SUPPORT
    portctrl_pport_t pport;
    int valid;

    /* Convert port to PortMod port */
    PORTCTRL_PORT_TO_PPORT(port, pport);

    /* Remove port */
    PORTMOD_IF_ERROR_RETURN(portmod_port_is_valid(unit, pport, &valid));
    if (valid) {
        PORTMOD_IF_ERROR_RETURN(portmod_port_remove(unit, pport));
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}


/*
 * Function:
 *      soc_esw_portctrl_port_resource_configure
 * Purpose:
 *      Configure the relevant Port Macros for the given
 *      the FlexPort configuration.
 *
 *      This function reconfigures all the blocks inside
 *      the PortMod library, this is xxPORT, MAC, and PHY.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 *      - Assumes SOC_INFO has been updated.
 */
int
soc_esw_portctrl_port_resource_configure(int unit,
                                         int nport,
                                         soc_port_resource_t *resource)
{
#ifdef PORTMOD_SUPPORT
    int i;
    soc_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);

    /* Check that PortCtrl has been initialized */
    SOC_IF_ERROR_RETURN(soc_esw_portctrl_init_check(unit));

    /* Configure port in PortMod */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Remove port */
        if (pr->physical_port == -1) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "Delete port: logical=%d physical=%d\n"),
                         pr->logical_port, pr->physical_port));
            SOC_IF_ERROR_RETURN
                (soc_esw_portctrl_delete(unit, pr->logical_port));

            /*
             * In some cases, ports to be deleted need to be inactive
             * which requires the PortMod NULL driver to be installed.
             * In this case, the port needs to be added back.
             * The function soc_esw_portctrl_add() sets the right
             * information for the PortMod to install the correct
             * driver.
             */

            /*
             * If this deleted port is not an inactive port, move on
             * to next port, else, continue with flow and add port.
             */
            if (!SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
                continue;
            }
        }

        /* Add port */
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Add port: logical=%d physical=%d\n"),
                     pr->logical_port, pr->physical_port));
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_add(unit, pr->logical_port));
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_pgw_reconfigure
 * Purpose:
 *      This function is called during reconfiguration of PGW during
 *      flexing operation.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Port number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes port is disabled.
 *      Assumes caller has checked that PortCtrl was initialized.
 *      SOC_INFO SW data structure has been updated.
 */
int
soc_esw_portctrl_pgw_reconfigure(int unit, soc_port_t port,
                                 soc_esw_portctrl_pgw_t *pgw_data)
{
#ifdef PORTMOD_SUPPORT
    portmod_port_mode_info_t info;

    portmod_port_mode_info_t_init(unit, &info);

    info.cur_mode = pgw_data->mode;
    info.lanes = pgw_data->lanes;
    info.port_index = pgw_data->port_index;

    /* Note this portmod API requires logical port */
    PORTMOD_IF_ERROR_RETURN(portmod_port_pgw_reconfig(unit, port, &info,
                                                      pgw_data->flags));

    return SOC_E_NONE;
#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_led_chain_config
 * Purpose:
 *      Set the port mode
 * Parameters:
 *      unit - SOC Unit #
 *      port - Port #
 *      value - Intra-Port delay for each subports in PORT block
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_led_chain_config(int unit, int port, int value)
{
#ifdef PORTMOD_SUPPORT
    return portmod_port_led_chain_config(unit, port, value);

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_name_get
 * Purpose:
 *      Get the phy name of the given port
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port #.
 * Returns:
 *      The PHY name string or NULL when error
 */
char *
soc_portctrl_phy_name_get(int unit, int port)
{
#ifdef PORTMOD_SUPPORT
    phymod_core_access_t core_acc;
    phymod_core_info_t core_info;
    int nof_cores = 0;
    char *enum_str;

    portmod_port_core_access_get(unit, port, -1, 1, &core_acc, &nof_cores, NULL);
    if (nof_cores == 0) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                 (BSL_META_UP(unit, port,
                              "ERROR: getting port%d information.\n"),
                  port));
        return "<nophy>";
    }

    phymod_core_info_get(&core_acc, &core_info);
    enum_str = phymod_core_version_t_mapping[core_info.core_version].key;
    return &enum_str[PHYMOD_STRLEN("phymodCoreVersion")];

#else  /* PORTMOD_SUPPORT */
    return "<unavail>";
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_port_mode_set
 * Purpose:
 *      Set the port mode
 * Parameters:
 *      unit - SOC Unit #
 *      port - Port #
 *      mode - port mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_port_mode_set(int unit, int port, int mode)
{
#ifdef PORTMOD_SUPPORT
    portmod_port_mode_info_t port_mode_info;

    portmod_port_mode_info_t_init(unit, &port_mode_info);

    port_mode_info.cur_mode = mode;

    return portmod_port_mode_set(unit, port, &port_mode_info);

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_port_mode_get
 * Purpose:
 *      Get the port mode
 * Parameters:
 *      unit - SOC Unit #
 *      port - Port #
 *      mode - port mode
 *      lanes - lanes of the port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_port_mode_get(int unit, int port, int *mode, int *lanes)
{
#ifdef PORTMOD_SUPPORT
    int rv;
    portmod_port_mode_info_t port_mode_info;

    portmod_port_mode_info_t_init(unit, &port_mode_info);

    rv = portmod_port_mode_get(unit, port, &port_mode_info);

    if (SOC_SUCCESS(rv)) {
        *mode = port_mode_info.cur_mode;
        *lanes = port_mode_info.lanes;
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_control_set
 * Purpose:
 *      Set PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control set to be applied 
 *             on system side(1)or line side(0).
 *      phy_ctrl - PHY control type to change
 *      value    - New setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_control_set(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 value)
{
#ifdef PORTMOD_SUPPORT
    phymod_phy_access_t pm_acc;
    phymod_phy_access_t pm_acc12[3];
    int num_pm_acc;
    portmod_access_get_params_t params;
    int per_lane_ctrl = 1;
    int ref_clk;

    portmod_access_get_params_t_init(unit, &params);

    params.phyn = phyn;
    params.lane = phy_lane;
    params.sys_side = (sys_side == 1) ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    if (IS_C_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                              3, &pm_acc12[0], &num_pm_acc, NULL));
    } else {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                              1, &pm_acc, &num_pm_acc, NULL));
    }

    SOC_IF_ERROR_RETURN
        (portmod_port_ref_clk_get(unit, port, &ref_clk));

    switch (phy_ctrl) {
        /* Not per lane control */
        case SOC_PHY_CONTROL_LANE_SWAP:
            per_lane_ctrl = 0;
            break;
        default:
            break;
    }

    if (IS_C_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(
            soc_port_control_set_wrapper(unit, ref_clk, per_lane_ctrl,
                                         &pm_acc12[0], 3, phy_ctrl, value));
    } else {
        SOC_IF_ERROR_RETURN(
            soc_port_control_set_wrapper(unit, ref_clk, per_lane_ctrl,
                                         &pm_acc, 1, phy_ctrl, value));
    }
    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_portctrl_phy_control_get
 * Purpose:
 *      Get PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control get to be applied
 *             on system side(1)or line side(0).
 *      phy_ctrl - PHY control type to read
r
 *      value    - Current setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_portctrl_phy_control_get(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 *value)
{
#ifdef PORTMOD_SUPPORT
    phymod_phy_access_t pm_acc, pm12_acc[3];
    int num_pm_acc;
    portmod_access_get_params_t params;
    int per_lane_ctrl = 1;
    int ref_clk;

    portmod_access_get_params_t_init(unit, &params);

    params.phyn = phyn;
    params.lane = phy_lane;
    params.sys_side = (sys_side == 1) ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    if (IS_C_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                          3, &pm12_acc[0], &num_pm_acc, NULL));
    } else {
        SOC_IF_ERROR_RETURN
            (portmod_port_phy_lane_access_get(unit, port, &params,
                                          1, &pm_acc, &num_pm_acc, NULL));
    }

    SOC_IF_ERROR_RETURN
        (portmod_port_ref_clk_get(unit, port, &ref_clk));

    switch (phy_ctrl) {
        /* Not per lane control */
        case SOC_PHY_CONTROL_LANE_SWAP:
            per_lane_ctrl = 0;
            break;
        default:
            break;
    }

    if (IS_C_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(
            soc_port_control_get_wrapper(unit, ref_clk, per_lane_ctrl,
                                    &pm12_acc[0], 3, phy_ctrl, value));
    } else {
        SOC_IF_ERROR_RETURN(
            soc_port_control_get_wrapper(unit, ref_clk, per_lane_ctrl,
                                     &pm_acc, 1, phy_ctrl, value));
    }

    return SOC_E_NONE;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_encap_get
 * Purpose:
 *      Get the port encapsulation mode.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 *      mode   - (OUT) One of _SHR_PORT_ENCAP_xxx.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_encap_get(int unit, soc_port_t port, int *mode)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    int flags = 0;
    portctrl_pport_t pport;
    portmod_encap_t encap;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_CONTROL_LOCK(unit);
    rv = portmod_port_encap_get(unit, pport, &flags, &encap);
    SOC_CONTROL_UNLOCK(unit);

    if (PORTMOD_SUCCESS(rv)) {
        *mode = encap;
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_port_to_phyaddr
 * Purpose:
 *      This function gets phy address associated with the port
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 * Returns:
 *      Phy address value
 */
int
soc_esw_portctrl_port_to_phyaddr(int unit, soc_port_t port, uint8* phy_addr)
{
#ifdef PORTMOD_SUPPORT
    int addr;
    portctrl_pport_t pport;
    int rv = SOC_E_NONE;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    SOC_CONTROL_LOCK(unit);
    addr = portmod_port_to_phyaddr(unit, pport);
    SOC_CONTROL_UNLOCK(unit);

    /* Portmod API returns phy address on successful execution, and
     * negative value in case of error.
     * So we program phy_addr, rv accordingly
     */
    /* coverity[negative_returns] */
    if (addr >= 0) {
        *phy_addr = addr; 
    } else {
        rv = addr;
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      soc_esw_portctrl_speed_get
 * Purpose:
 *      Get the port speed.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 *      speed  - (OUT) Port speed.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_esw_portctrl_speed_get(int unit, soc_port_t port, int *speed)
{
#ifdef PORTMOD_SUPPORT
    int rv = SOC_E_NONE;
    portctrl_pport_t pport;
    portmod_port_interface_config_t portmod_if_config;

    PORTCTRL_PORT_TO_PPORT(port, pport);

    /*
     * Temporary solution until PortMod null driver is in place
     */
    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) {
        *speed = SOC_INFO(unit).port_speed_max[port];
        return SOC_E_NONE;
    }

    SOC_CONTROL_LOCK(unit);
    rv = portmod_port_interface_config_get(unit, pport, &portmod_if_config);
    SOC_CONTROL_UNLOCK(unit);

    if (PORTMOD_SUCCESS(rv)) {
        *speed = portmod_if_config.speed;

        if (IS_HG_PORT(unit, port) && *speed < 5000) {
            *speed = 0;
        }
    }

    return rv;

#else  /* PORTMOD_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_SUPPORT */
}
