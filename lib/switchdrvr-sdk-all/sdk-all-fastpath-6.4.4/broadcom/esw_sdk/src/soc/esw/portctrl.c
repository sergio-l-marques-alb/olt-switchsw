/* 
 * $Id:$
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/phy/phymod_port_control.h>
#endif /* PORTMOD_SUPPORT */

#ifdef PORTMOD_SUPPORT

/*
 * Define:
 *      PORTCTRL_PORT_RESOLVE
 * Purpose:
 *      Converts a SOC port into port type used in PortMod functions '_pport'
 */
#define PORTCTRL_PORT_TO_PPORT(_port, _pport) do {  \
        _pport = _port;                             \
    } while (0)

#endif /* PORTMOD_SUPPORT */


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
