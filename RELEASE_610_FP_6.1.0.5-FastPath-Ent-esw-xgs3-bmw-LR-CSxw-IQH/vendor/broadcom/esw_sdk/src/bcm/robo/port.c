/*
 * $Id: port.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:    port.c
 * Purpose:     Tracks and manages ports.
 *      MAC/PHY interfaces are managed through respective drivers.
 *
 */
#include <soc/ll.h>
#include <soc/mcm/robo/driver.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/phy.h>
#include <soc/ptable.h>
#include <soc/phyctrl.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/filter.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm/stg.h>
#include <bcm/rate.h>
#include <bcm/mirror.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>

#include <bcm_int/robo/port.h>
#include <bcm_int/robo/link.h>
#include <bcm_int/robo/stg.h>
#include <bcm_int/robo/rate.h>
#include <bcm_int/robo/vlan.h>

#include <bcm_int/robo_dispatch.h>

#include <sal/types.h>
#include <sal/appl/io.h>

/* for marking the API which is still not dispatchable */
#define BCM_ROBO_API_IS_NOT_DISPATCHABLE 0  
static int              robo_p_ut_prio; /* Untagged priority*/
static bcm_port_cfg_t   *robo_port_config[BCM_LOCAL_UNITS_MAX];

/*
 * Define:
 *  PORT_INIT
 * Purpose:
 *  Causes a routine to return BCM_E_INIT if port is not yet initialized.
 */

#define PORT_INIT(unit) \
    if (_bcm_robo_port_init(unit) == FALSE) { return BCM_E_INIT; }
    
/*
 * Define:
 *  PORT_PARAM_CHECK
 * Purpose:
 *  Check unit and port parameters for most bcm_port api calls
 */
#define PORT_PARAM_CHECK(unit, port) do { \
    if (!SOC_UNIT_VALID(unit)) {return BCM_E_UNIT;}\
    if (!SOC_PORT_VALID(unit, port)) { return BCM_E_PORT; } \
    PORT_INIT(unit);} while (0);


int
_bcm_robo_port_init(int unit)
{
    uint32      val = FALSE;
    int         rv = BCM_E_NONE;

    rv = ((DRV_SERVICES(unit)->port_status_get)
                (unit, 0, DRV_PORT_STATUS_INIT, &val));
    if (rv < 0) {
        return FALSE;
    }

    if (val) {
        return TRUE;
    }
    return FALSE;
    
}

/*
 * Function    : _bcm_robo_modid_is_local
 * Description : Identifies if given modid is local on a given unit
 *
 * Parameters  : (IN)   unit      - BCM device number
 *               (IN)   modnd     - Module ID 
 *               (OUT)  result    - TRUE if modid is local, FALSE otherwise
 * Returns     : BCM_E_XXX
 */
int 
_bcm_robo_modid_is_local(int unit, bcm_module_t modid, int *result)
{
    bcm_module_t    mymodid;    
    int             rv;

    /* Input parameters check. */
    if (NULL == result) {
        return (BCM_E_PARAM);
    }

    /* Get local module id. */
    rv = bcm_robo_stk_my_modid_get(unit, &mymodid);
    if (BCM_E_UNAVAIL == rv) {
        if (0 == modid) {
            *result = TRUE;
        } else {
            *result = FALSE;
        }
        return (BCM_E_NONE);
    }

    if (mymodid == modid) {
        *result = TRUE;
    } else if (NUM_MODID(unit) == 2) {
        if (modid == (mymodid +1)) {
            *result = TRUE;
        } else {
            *result = FALSE;
        }
    } else {
        *result = FALSE;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_robo_port_gport_validate
 * Description:
 *      Helper funtion to validate port/gport parameter 
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port_in  - (IN) Port / Gport to validate
 *      port_out - (OUT) Port number if valid. 
 * Return Value:
 *      BCM_E_NONE - Port OK 
 *      BCM_E_INIT - Not initialized
 *      BCM_E_PORT - Port Invalid
 */
int
_bcm_robo_port_gport_validate(int unit, 
                bcm_port_t port_in, bcm_port_t *port_out)
{
    bcm_module_t my_mod, mod_temp;
    bcm_port_t port_temp;

    PORT_INIT(unit); 

    if (BCM_GPORT_IS_SET(port_in)) {
#if BCM_ROBO_API_IS_NOT_DISPATCHABLE
        BCM_IF_ERROR_RETURN(
            bcm_robo_port_local_get(unit, port_in, port_out));
#else   /* BCM_ROBO_API_IS_NOT_DISPATCHABLE */
        if (SOC_GPORT_IS_LOCAL(port_in)) {
            *port_out = SOC_GPORT_LOCAL_GET(port_in);
        } else if (SOC_GPORT_IS_DEVPORT(port_in)) {
            *port_out = SOC_GPORT_DEVPORT_PORT_GET(port_in);
            if (unit != SOC_GPORT_DEVPORT_DEVID_GET(port_in)) {
                return BCM_E_PORT;
            }
        } else if (SOC_GPORT_IS_MODPORT(port_in)) {
            BCM_IF_ERROR_RETURN(
                bcm_robo_stk_my_modid_get(unit, &my_mod));
            mod_temp = SOC_GPORT_MODPORT_MODID_GET(port_in);
            port_temp = SOC_GPORT_MODPORT_PORT_GET(port_in);
            if (mod_temp == my_mod){ 
                *port_out = port_temp;
            } else {
                return BCM_E_PORT;
            }
            if (!SOC_PORT_VALID(unit, *port_out)) {
                return BCM_E_PORT;
            }
        } else {
            return BCM_E_PORT;
        }
#endif  /* BCM_ROBO_API_IS_NOT_DISPATCHABLE */
    } else if (SOC_PORT_VALID(unit, port_in)) { 
        *port_out = port_in;
    } else {
        return BCM_E_PORT; 
    }

    return BCM_E_NONE;
}

STATIC void
_bcm_robo_port_cfg_init(int unit)
{
    int         len;
    int         port;
    int         rv;
    bcm_port_cfg_t  * pcfg;

    /* set default PVID on each port*/
    BCM_PBMP_ITER(PBMP_ALL(unit), port){
        rv = ((DRV_SERVICES(unit)->port_vlan_pvid_set)
                        (unit, port, BCM_VLAN_DEFAULT, 0));
    }
    
    if (robo_port_config[unit] == NULL){
        /* Allocate module database array */
        len = SOC_MAX_NUM_PORTS * sizeof(bcm_port_cfg_t);
        robo_port_config[unit] = sal_alloc(len, "ROBO Port Database");
        sal_memset(robo_port_config[unit], 0, len);
    }
    
    /* keep SW info for the default PVID on each port */
    BCM_PBMP_ITER(PBMP_ALL(unit), port){
        pcfg = &robo_port_config[unit][port];
        pcfg->pc_vlan = BCM_VLAN_DEFAULT;
    }
    
}

/*
 * Function:
 *  _bcm_robo_port_link_get
 * Purpose:
 *  Return current PHY up/down status
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  hw - If TRUE, assume hardware linkscan is active and use it
 *      to reduce PHY reads.
 *         If FALSE, do not use information from hardware linkscan.
 *  up - (OUT) TRUE for link up, FALSE for link down.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
_bcm_robo_port_link_get(int unit, bcm_port_t port, int hw, int *up)
{
    int     rv;
    uint32  drv_value;

    PORT_PARAM_CHECK(unit, port); 
    /*
     * This routines in original code is for an potential issue on 
     *   HW the link status might not sync. with the real PHY status
     *   when the HW link up represented.
     * Now, for Robo chip in this source we force all register R/W 
     *   been hided in the Drv Service so we do same thing to get 
     *   link status through "port_status_get()" within 
     *   "DRV_PORT_STATUS_LINK_UP" flag no matter hw=1 or hw=0.
     *
     */
    if (hw) {
        rv = ((DRV_SERVICES(unit)->port_status_get)
                (unit, port, 
                DRV_PORT_STATUS_LINK_UP, &drv_value));
        *up = (drv_value) ? TRUE : FALSE;

    } else {
        rv = ((DRV_SERVICES(unit)->port_status_get)
                (unit, port, 
                DRV_PORT_STATUS_LINK_UP, &drv_value));
        *up = (drv_value) ? TRUE : FALSE;
    }

    if (BCM_SUCCESS(rv)) {
        if (PHY_FLAGS_TST(unit, port, PHY_FLAGS_MEDIUM_CHANGE)) {
            soc_port_medium_t  medium;
            soc_phyctrl_medium_get(unit, port, &medium);
            soc_phy_medium_status_notify(unit, port, medium);
        }
    }

    SOC_DEBUG_PRINT((DK_PORT | DK_VERBOSE | DK_LINK,
             "_bcm_port_link_get: u=%d p=%d hw=%d up=%d rv=%d\n",
             unit, port, hw, *up, rv));

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_probe
 * Purpose:
 *  Probe the PHY and set up the PHY and MAC for the specified ports.
 *      This is purely a discovery routine and does no configuration.
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  pbmp - Bitmap of ports to probe.
 *      okay_pbmp (OUT) - Ports which were successfully probed.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_INTERNAL - internal error.
 * Notes:
 *      If error is returned, the port should not be enabled.
 *      Assumes port_init done.
 *      Note that if a PHY is not present, the port will still probe
 *      successfully.  The default driver will be installed.
 */

int
bcm_robo_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp)
{
    int rv = BCM_E_NONE;
    bcm_port_t port;
    uint32 okay;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_probe()..\n");
    BCM_PBMP_CLEAR(*okay_pbmp);

    PBMP_ITER(pbmp, port) 
    {
        /* do port_probe process */
        rv = ((DRV_SERVICES(unit)->port_status_get)
                    (unit, port, DRV_PORT_STATUS_PROBE, &okay));
        if (okay) 
        {
            BCM_PBMP_PORT_ADD(*okay_pbmp, port);
        }
        if (rv < 0) 
        {
            break;
        }
    }

    return rv;
}


/*
 * Function:
 *  _bcm_robo_port_mode_setup
 * Purpose:
 *  Set initial operating mode for a port.
 * Parameters:
 *  unit - RoboSwitch unit #.
 *  port - RoboSwitch port #.
 *  enable - Whether to enable or disable
 * Returns:
 *  BCM_E_XXXX
 */

STATIC int
_bcm_robo_port_mode_setup(int unit, bcm_port_t port, int enable)
{
    soc_port_if_t       pif;
    bcm_port_ability_t  local_pa;

    BCM_IF_ERROR_RETURN(bcm_port_ability_local_get(unit, port, &local_pa));

    /* If MII supported, enable it, otherwise use TBI */

    if (local_pa.interface & (SOC_PA_INTF_MII | SOC_PA_INTF_GMII | 
                                                SOC_PA_INTF_SGMII) ){
        if (IS_GE_PORT(unit, port)) 
        {
            pif = SOC_PORT_IF_GMII;
        }
        else 
        {
            pif = SOC_PORT_IF_MII;
        }
    } else {
        pif = SOC_PORT_IF_TBI;
    }

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_interface_set(unit, port, pif));
    return BCM_E_NONE;
    
}

/*
 * Function:
 *  _bcm_robo_port_detach
 * Purpose:
 *  Main part of bcm_port_detach
 */

int
_bcm_robo_port_detach(int unit, pbmp_t pbmp, pbmp_t *detached)
{
    bcm_port_t      port;

    BCM_PBMP_CLEAR(*detached);

    PBMP_ITER(pbmp, port) 
    {
        BCM_IF_ERROR_RETURN(bcm_port_stp_set(unit, port, BCM_STG_STP_DISABLE));
        
        BCM_IF_ERROR_RETURN(_bcm_robo_port_mode_setup(unit, port, FALSE));
        SOC_PBMP_PORT_ADD(*detached, port);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_detach
 * Purpose:
 *  Detach a port.  Set phy driver to no connection.
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  pbmp - Bitmap of ports to detach.
 *      detached (OUT) - Bitmap of ports successfully detached.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_INTERNAL - internal error.
 * Notes:
 *      If a port to be detached does not appear in detached, its
 *      state is not defined.
 */

int
bcm_robo_port_detach(int unit, pbmp_t pbmp, pbmp_t *detached)
{
    int     rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_detach()..\n");
    PORT_INIT(unit);
    rv = _bcm_robo_port_detach(unit, pbmp, detached);

    return rv;
}

/* General routines on which most port routines are built */

/*
 * Function:
 *  bcm_robo_port_config_get
 * Purpose:
 *  Get port configuration of a device
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  config - (OUT) Structure returning configuration
 * Returns:
 *  BCM_E_XXX
 */

int
bcm_robo_port_config_get(int unit, bcm_port_config_t *config)
{
    bcm_pbmp_t pbm;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_config_get()..\n");
    PORT_INIT(unit);
    SOC_PBMP_CLEAR(pbm);

    config->fe         = PBMP_FE_ALL(unit);
    config->ge         = PBMP_GE_ALL(unit);
    config->xe         = PBMP_XE_ALL(unit);
    config->e         = PBMP_E_ALL(unit);
    config->hg         = PBMP_HG_ALL(unit);
    config->port     = PBMP_PORT_ALL(unit);
    config->cpu     = PBMP_CMIC(unit);
    config->all     = PBMP_ALL(unit);
    config->stack_int     = pbm;
    config->stack_ext     = pbm;
    config->sci           = pbm;
    config->sfi           = pbm;
    config->spi           = pbm;
    config->spi_subport   = pbm;

    return BCM_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_enable_set
 * Purpose:
 *  Physically enable/disable the MAC/PHY on this port.
 * Parameters:
 *  unit - RoboSwitch unit #.
 *  port - RoboSwitch port #.
 *  enable - TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *  BCM_E_XXXX
 * Notes:
 *  If linkscan is running, it also controls the MAC enable state.
 */

int
bcm_robo_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int     rv = BCM_E_NONE;
    pbmp_t  t_pbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_enable_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    /* process the MAC ENABLE & PHY ENABLE */
    rv = ((DRV_SERVICES(unit)->port_set)
                (unit, t_pbm, 
                DRV_PORT_PROP_ENABLE , enable));
    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_enable_set: u=%d p=%d enable=%d rv=%d\n",
             unit, port, enable, rv));

    return rv;
}



/*
 * Function:
 *  bcm_robo_port_enable_get
 * Purpose:
 *  Gets the enable state as defined by bcm_port_enable_set()
 * Parameters:
 *  unit - RoboSwitch unit #.
 *  port - RoboSwitch port #.
 *  enable - (OUT) TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *  BCM_E_XXXX
 * Notes:
 *  The PHY enable holds the port enable state set by the user.
 *  The MAC enable transitions up and down automatically via linkscan
 *  even if user port enable is always up.
 */

int
bcm_robo_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    int         rv = BCM_E_NONE ;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_enable_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = ((DRV_SERVICES(unit)->port_get)
                (unit, port, DRV_PORT_PROP_ENABLE, (uint32 *) enable));

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_enable_get: u=%d p=%d rv=%d enable=%d\n",
             unit, port, rv, *enable));

    return rv;
}

/*
 * Function:
 *      bcm_robo_port_error_symbol_count
 * Description:
 *      Get the number of |E| symbol in XAUI lanes since last read.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      count - (OUT) Number of |E| error since last read.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 *      BCM_E_INIT    - Error symbol detect feature is not enabled
 */
int 
bcm_robo_port_error_symbol_count(int unit, bcm_port_t port, int *count) 
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_port_settings_init
 * Purpose:
 *      Initialize port settings if they are to be different from the
 *      default ones
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      port - port number 
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 * Notes:
 *      This function initializes port settings based on the folowing config
 *      variables:
 *           port_init_speed
 *           port_init_duplex
 *           port_init_adv
 *           port_init_autoneg
 *      If a variable is not set, then no additional initialization of the 
 *      corresponding parameter is done (and the defaults will normally be
 *      advertize everything you can do and use autonegotiation).
 *
 *      A typical use would be to set:
 *          port_init_adv=0
 *          port_init_autoneg=1
 *      to force link down in the beginning.
 *
 *      Another setup that makes sense is something like:
 *          port_init_speed=10
 *          port_init_duplex=0
 *          port_init_autoneg=0
 *      in order to force link into a certain mode. (It is very important to
 *      disable autonegotiation in this case).
 *
 *      PLEASE NOTE: 
 *          The standard rc.soc forces autoneg=on on all the ethernet ports
 *          (FE and GE). Thus, to use the second example one has to edit rc.soc
 *          as well.
 * 
 *     This function has been declared as global, but not exported. This will
 *     make port initialization easier when using VxWorks shell. 
 */
int
bcm_robo_port_settings_init(int unit, bcm_port_t port)
{
    int             val;
    bcm_port_info_t info;
 
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_settings_init()..\n");
    info.action_mask = 0;

    val = soc_property_port_get(unit, port, spn_PORT_INIT_SPEED, -1);
    if (val != -1) {
        info.speed = val;
        info.action_mask |= BCM_PORT_ATTR_SPEED_MASK;
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_DUPLEX, -1);
    if (val != -1) {
        info.duplex = val;
        info.action_mask |= BCM_PORT_ATTR_DUPLEX_MASK;
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_ADV, -1);
    if (val != -1) {
        info.local_advert = val;
        info.action_mask |= BCM_PORT_ATTR_LOCAL_ADVERT_MASK;
    }

    val = soc_property_port_get(unit, port, spn_PORT_INIT_AUTONEG, -1);
    if (val != -1) {
        info.autoneg = val;
        info.action_mask |= BCM_PORT_ATTR_AUTONEG_MASK;
    }
    

    return bcm_port_selective_set(unit, port, &info);
}

/*
 * Function:
 *      bcm_port_clear
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device
 *      without resetting stacking ports.
 * Parameters:
 *      unit - RoboSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      All ports set in disabled state.
 *      PTABLE initialized.
 */
int
bcm_robo_port_clear(int unit)
{

    bcm_port_config_t port_config;
    bcm_pbmp_t reset_ports;
    bcm_port_t port;
    int rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_clear()..\n");
    PORT_INIT(unit);

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &port_config));

    /* Clear all non-stacking ethernet ports */
    BCM_PBMP_ASSIGN(reset_ports, port_config.e);

    PBMP_ITER(reset_ports, port) {
        soc_cm_debug(DK_PORT | DK_VERBOSE,
                     "bcm_port_clear: unit %d port %s\n",
             unit, SOC_PORT_NAME(unit, port));

        if ((rv = _bcm_robo_port_mode_setup(unit, port, TRUE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set initial mode: %s\n",
                         unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
        }

        if ((rv = bcm_port_enable_set(unit, port, TRUE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to enable port: %s\n",
                         unit, SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
        }
    }

    return BCM_E_NONE;
}

/* Module one-time initialization routine */

/*
 * Function:
 *      bcm_robo_port_init
 * Purpose:
 *      Initialize the PORT interface layer for the specified SOC device.
 * Parameters:
 *      unit - RoboSwitch unit number.
 * Returns:
 *      BCM_E_NONE - success (or already initialized)
 *      BCM_E_MEMORY - failed to allocate required memory.
 * Notes:
 *      All ports set in disabled state.
 *      Default PVID initialized.
 */

int
bcm_robo_port_init(int unit)
{
    int                 rv;
    int                 dt_mode;
    bcm_port_t          p;
    pbmp_t              okay_ports, pbmp, t_pbm;
    bcm_vlan_data_t     vd;
    char                pfmtok[SOC_PBMP_FMT_LEN],
                        pfmtall[SOC_PBMP_FMT_LEN];

    assert(unit < BCM_LOCAL_UNITS_MAX);
    
    /*
     * Write port configuration tables to contain the Initial System
     * Configuration (see init.c).
     */

    vd.vlan_tag = BCM_VLAN_DEFAULT;
    BCM_PBMP_ASSIGN(vd.port_bitmap, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(vd.ut_port_bitmap, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));

    /* for software table and setting default PVID */
    _bcm_robo_port_cfg_init(unit);
    BCM_IF_ERROR_RETURN(soc_phy_common_init(unit));

    /* Probe for ports */
    BCM_PBMP_CLEAR(okay_ports);
    
    if ((rv = bcm_port_probe(unit, PBMP_PORT_ALL(unit), &okay_ports)) 
                    != BCM_E_NONE) 
    {
        soc_cm_debug(DK_ERR, "Error unit %d:  Failed port probe: %s\n",
                     unit, bcm_errmsg(rv));
        return rv;
    }

    soc_cm_debug(DK_VERBOSE, "Probed ports okay: %s of %s\n",
         SOC_PBMP_FMT(okay_ports, pfmtok),
         SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmtall));

    /* force the CPU been ISP port after init */
    p = CMIC_PORT(unit);
    rv = bcm_port_dtag_mode_get(unit, p, &dt_mode);
    if (rv) {
        soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                     "Failed to get dtag_mode : %s\n",
                     unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
    } else {
        if ((rv = bcm_port_dtag_mode_set
                    (unit, p, BCM_PORT_DTAG_MODE_INTERNAL)) < 0) {
            /* if the return code is UNAVAIL, it means this chip have no dtag 
             *  feature, this error code can be ignored here.
             */
            if ((rv != BCM_E_UNAVAIL)){
                soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                             "Failed to set dtag_mode_internal : %s\n",
                             unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
            }
        }
    }
    
    /* Probe and initialize MAC and PHY drivers for ports that were OK */

    PBMP_ITER(okay_ports, p) {
        soc_cm_debug(DK_PORT | DK_VERBOSE, "bcm_port_init: unit %d port %s\n",
             unit, SOC_PORT_NAME(unit, p));

        if ((rv = _bcm_robo_port_mode_setup(unit, p, TRUE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set initial mode: %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        if ((rv == bcm_robo_port_settings_init(unit, p)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to configure initial settings: %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        if ((rv == bcm_port_discard_set
                    (unit, p, BCM_PORT_DISCARD_NONE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set discard_none : %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        rv = bcm_port_dtag_mode_get(unit, p, &dt_mode);
        if (rv) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to get dtag_mode : %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        } else {
            if (dt_mode == BCM_PORT_DTAG_MODE_INTERNAL) {
                /* set port as non-ISP port when init */
                if ((rv == bcm_port_dtag_mode_set
                            (unit, p, BCM_PORT_DTAG_MODE_EXTERNAL)) < 0) {
                    soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                                 "Failed to set dtag_mode_external : %s\n",
                                 unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                }
            }
        }

        if ((rv == bcm_port_tpid_set
                    (unit, p, 0x9100)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set tpid : %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        if ((rv == bcm_port_pfm_set(unit, p, 
            BCM_PORT_MCAST_FLOOD_UNKNOWN)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to set pfm mode : %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }

        if ((rv = bcm_port_enable_set(unit, p, TRUE)) < 0) {
            soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
                         "Failed to enable port: %s\n",
                         unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
        }
    }

    /* disable system based Double tagging mode :
     *  - Robo only! disable DT_Mode at one port means disable system basis.
     */
    p = CMIC_PORT(unit);
    if ((rv == bcm_port_dtag_mode_set
            (unit, p, BCM_PORT_DTAG_MODE_NONE)) < 0) {
        soc_cm_debug(DK_WARN, "Warning: Unit %d Port %s: "
             "Failed to set dtag_mode_none : %s\n",
             unit, SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
    }

    /* Clear port-based vlan mask to default value */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));

    BCM_PBMP_ITER(pbmp, p) {
        BCM_PBMP_CLEAR(t_pbm);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_get)
                            (unit, p, &t_pbm));

        if (!BCM_PBMP_EQ(pbmp, t_pbm)) {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_set)
                        (unit, p, pbmp));
        }
    }

    soc_cm_debug(DK_PORT, "BCM API : bcm_port_init() Done!\n");

    return BCM_E_NONE;    
}


/*
 * Function:
 *  bcm_robo_port_advert_get
 * Purpose:
 *  Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  ability_mask - (OUT) Local advertisement.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_advert_get(int unit, bcm_port_t port, 
            bcm_port_abil_t *ability_mask)
{
    int     rv;
    bcm_port_ability_t  ability;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_advert_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = soc_phyctrl_ability_advert_get(unit, port, &ability);
    if (BCM_SUCCESS(rv)) {
        rv = soc_port_ability_to_mode(&ability, ability_mask);
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_advert_get: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, *ability_mask, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_ability_advert_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch port #.
 *      ability_mask - (OUT) Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_robo_port_ability_advert_get(int unit, bcm_port_t port, 
                                bcm_port_ability_t *ability_mask)
{
    int         rv;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = soc_phyctrl_ability_advert_get(unit, port, ability_mask);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_advert_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_advert_set
 * Purpose:
 *  Set the local port advertisement for autonegotiation.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  ability_mask - Local advertisement.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Does NOT restart autonegotiation.
 *  To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

int
bcm_robo_port_advert_set(int unit, bcm_port_t port, 
                bcm_port_abil_t ability_mask)
{
    int     rv;
    pbmp_t  t_pbm;
    uint32  drv_value = 0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_advert_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    drv_value = ability_mask;
    
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, t_pbm, 
            DRV_PORT_PROP_LOCAL_ADVERTISE, drv_value));
    return rv;
}

/*
 * Function:
 *      bcm_port_ability_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch port #.
 *      ability_mask - Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call MAY NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

int
bcm_robo_port_ability_advert_set(int unit, bcm_port_t port,
                                bcm_port_ability_t *ability_mask)
{
    int             rv;
    bcm_port_ability_t port_ability;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    BCM_IF_ERROR_RETURN
        (bcm_port_ability_local_get(unit, port, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.speed_half_duplex   &= ability_mask->speed_half_duplex;
    port_ability.speed_full_duplex   &= ability_mask->speed_full_duplex;
    port_ability.pause      &= ability_mask->pause;
    port_ability.interface  &= ability_mask->interface;
    port_ability.medium     &= ability_mask->medium;
    port_ability.loopback   &= ability_mask->loopback;
    port_ability.flags      &= ability_mask->flags;

    rv = soc_phyctrl_ability_advert_set(unit, port, &port_ability);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_advert_set: u=%d p=%d rv=%d\n",
                     unit, port, rv));

    return rv;
}

/*
 * Function:
 *      bcm_robo_port_autoneg_advert_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch port #.
 *      ability_mask - (OUT) Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_robo_port_autoneg_advert_get (int unit, bcm_port_t port, 
                bcm_port_ability_t *ability_mask)
{
    int         rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_autoneg_advert_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = soc_phyctrl_ability_advert_get(unit, port, ability_mask);
    
    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_autoneg_advert_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));
    
    return rv;
}

/*
 * Function:
 *      bcm_robo_port_autoneg_advert_set
 * Purpose:
 *      Set the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch port #.
 *      ability_mask - Local advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 *      This call MAY NOT restart autonegotiation (depending on the phy).
 *      To do that, follow this call with bcm_port_autoneg_set(TRUE).
 */

int
bcm_robo_port_autoneg_advert_set (int unit, bcm_port_t port, 
                bcm_port_ability_t *ability_mask)
{
    int             rv;
    bcm_port_ability_t port_ability;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_autoneg_advert_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    BCM_IF_ERROR_RETURN
        (bcm_port_ability_local_get(unit, port, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.speed_half_duplex   &= ability_mask->speed_half_duplex;
    port_ability.speed_full_duplex   &= ability_mask->speed_full_duplex;
    port_ability.pause      &= ability_mask->pause;
    port_ability.interface  &= ability_mask->interface;
    port_ability.medium     &= ability_mask->medium;
    port_ability.loopback   &= ability_mask->loopback;
    port_ability.flags      &= ability_mask->flags;
    
    rv = soc_phyctrl_ability_advert_set(unit, port, &port_ability);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_autoneg_advert_set: u=%d p=%d rv=%d\n",
                     unit, port, rv));

    return rv;
}

/*
 * Function:
 *      _bcm_port_autoneg_advert_remote_get
 * Purpose:
 *      Main part of bcm_port_advert_get_remote
 */

STATIC int
_bcm_robo_port_autoneg_advert_remote_get(int unit, bcm_port_t port,
                            bcm_port_ability_t *ability_mask)
{
    int                 an, an_done;

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_auto_negotiate_get(unit,  port,
                                &an, &an_done));

    if (!an) {
        return BCM_E_DISABLED;
    }

    if (!an_done) {
        return BCM_E_BUSY;
    }

    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_remote_get(unit, port, ability_mask));

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_advert_remote_get
 * Purpose:
 *  Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  ability_mask - (OUT) Remote advertisement.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_advert_remote_get(int unit, bcm_port_t port,
               bcm_port_abil_t *ability_mask)
{
    int     rv;    
    bcm_port_ability_t  port_ability;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_advert_remote_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = _bcm_robo_port_autoneg_advert_remote_get(unit, port, &port_ability);

    if (BCM_SUCCESS(rv)) {
        rv = soc_port_ability_to_mode(&port_ability, ability_mask);
    }


    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_advert_remote_get: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, *ability_mask, rv));
    return rv;
}

/*
 * Function:
 *      bcm_port_ability_remote_get
 * Purpose:
 *      Retrieve the local port advertisement for autonegotiation.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch port #.
 *      ability_mask - (OUT) Remote advertisement.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_robo_port_ability_remote_get(int unit, bcm_port_t port,
                           bcm_port_ability_t *ability_mask)
{
    int         rv;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = _bcm_robo_port_autoneg_advert_remote_get(unit, port, ability_mask);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_remote_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));

    return rv;
}

/*
 * Function:
 *      _bcm_robo_port_ability_local_get
 * Purpose:
 *      Main part of bcm_port_ability_local_get
 * Notes:
 *      Relies on the fact the soc_port_mode_t and bcm_port_abil_t have
 *      the same values.
 */

STATIC int
_bcm_robo_port_ability_local_get(int unit, bcm_port_t port,
                           bcm_port_ability_t *ability_mask)
{
    soc_port_ability_t             mac_ability, phy_ability;
    mac_driver_t    *macd;
    int rv;

    if ((rv = soc_robo_mac_probe(unit, port, &macd)) < 0) {
        soc_cm_debug(DK_WARN | DK_PORT,
             "_bcm_robo_port_ability_local_get : unit %d Port %s: Failed to probe MAC: %s\n",
             unit, SOC_PORT_NAME(unit, port), soc_errmsg(rv));
        return rv;
    }
    
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_ability_local_get(unit, port, &phy_ability));

    SOC_IF_ERROR_RETURN
        (MAC_ABILITY_LOCAL_GET(macd, unit, port, &mac_ability));

    /* Combine MAC and PHY abilities */
    ability_mask->speed_half_duplex  = mac_ability.speed_half_duplex & phy_ability.speed_half_duplex;
    ability_mask->speed_full_duplex  = mac_ability.speed_full_duplex & phy_ability.speed_full_duplex;
    ability_mask->pause     = mac_ability.pause & phy_ability.pause;
    if (phy_ability.interface == 0) {
        ability_mask->interface = mac_ability.interface;
    } else {
        ability_mask->interface = phy_ability.interface;
    }
    ability_mask->medium    = phy_ability.medium;
    ability_mask->loopback  = mac_ability.loopback | phy_ability.loopback |
                               BCM_PORT_ABILITY_LB_NONE;
    ability_mask->flags     = mac_ability.flags | phy_ability.flags;

    return BCM_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_ability_get
 * Purpose:
 *  Retrieve the local port abilities.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  ability_mask - (OUT) Mask of BCM_PORT_ABIL_ values indicating the
 *      ability of the MAC/PHY.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_ability_get(int unit, bcm_port_t port, 
                    bcm_port_abil_t *ability_mask)
{
    int rv;
    bcm_port_ability_t  port_ability;

    soc_cm_debug(DK_PORT, "BCM API : bcm_port_ability_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = _bcm_robo_port_ability_local_get(unit, port, &port_ability);
    if (BCM_SUCCESS(rv)) {
        rv = soc_port_ability_to_mode(&port_ability, ability_mask);
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_get: u=%d p=%d abil=0x%x rv=%d\n",
                     unit, port, *ability_mask, rv));
    return rv;
}


/*
 * Function:
 *      bcm_robo_port_ability_local_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch port #.
 *      ability_mask - (OUT) Mask of BCM_PORT_ABIL_ values indicating the
 *              ability of the MAC/PHY.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_robo_port_ability_local_get(int unit, bcm_port_t port,
                         bcm_port_ability_t *ability_mask)
{
    int         rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_ability_local_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = _bcm_robo_port_ability_local_get(unit, port, ability_mask);

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_ability_local_get: u=%d p=%d rv=%d\n",
                     unit, port, rv));
    SOC_DEBUG_PRINT((DK_PORT | DK_VERBOSE,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x\n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n",
                     ability_mask->speed_half_duplex,
                     ability_mask->speed_full_duplex,
                     ability_mask->pause, ability_mask->interface,
                     ability_mask->medium, ability_mask->loopback,
                     ability_mask->flags));
    return rv;
}

/* PVLAN functions */

/*
 * Function:
 *  bcm_robo_port_untagged_vlan_get
 * Purpose:
 *  Retrieve the default VLAN TAG for the port.
 *  This is the VLAN ID assigned to received untagged packets.
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  port - RoboSwitch port number of port to get info for
 *  vid_ptr - (OUT) Pointer to VLAN ID for return
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_INTERNAL if table read failed.
 */

int
bcm_robo_port_untagged_vlan_get(int unit, 
                bcm_port_t port, 
                bcm_vlan_t *vid_ptr)
{
    bcm_port_cfg_t  * pcfg;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_untagged_vlan_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    pcfg = &robo_port_config[unit][port];

    if (vid_ptr != NULL) {
        /* include the VID + priority */
        *vid_ptr = pcfg->pc_vlan;
    }

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_untagged_vlan_get: u=%d p=%d pv_tag=%d\n",
             unit, port, pcfg->pc_vlan));

    return BCM_E_NONE;
}


/*
 * Function:
 *  _bcm_robo_port_untagged_vlan_set
 * Purpose:
 *      Main part of bcm_port_untagged_vlan_set.
 * Notes:
 *  Port must already be a member of its default VLAN.
 */

STATIC int
_bcm_robo_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    bcm_port_cfg_t     * pcfg;
    bcm_vlan_t      pdvid;
    uint32          pri;

    if (!BCM_VLAN_VALID(vid)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_pvid_get)
            (unit, port, (uint32 *)&pdvid, &pri));
            
    pcfg = &robo_port_config[unit][port];

    if (vid ^ pdvid){
        
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_pvid_set)
                (unit, port, vid, pri));
                
        pcfg->pc_vlan = vid;
    }
    
    return BCM_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_untagged_vlan_set
 * Purpose:
 *  Set the default VLAN ID for the port.
 *  This is the VLAN ID assigned to received untagged packets.
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  port - RoboSwitch port number.
 *  vid - (OUT) Pointer to VLAN ID for return
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_NOT_FOUND if vid not in VTABLE
 *  BCM_E_INTERNAL if table read failed.
 *  BCM_E_CONFIG - port does not belong to the VLAN
 */

int
bcm_robo_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    int rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_untagged_vlan_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    rv = _bcm_robo_port_untagged_vlan_set(unit, port, vid);

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_untagged_vlan_set: u=%d p=%d vid=%d rv=%d\n",
             unit, port, vid, rv));

    return rv;
}
                      

/*
 * Function:
 *  _bcm_robo_port_untagged_priority_set
 * Purpose:
 *  Main part of bcm_port_untagged_priority_set.
 */

STATIC int
_bcm_robo_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    bcm_port_cfg_t  *pcfg;
    uint32          vid, pri;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    if (priority < 0 || priority > 7) 
    {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_pvid_get)
            (unit, port, &vid, &pri));
    
    if (pri ^ priority){
        
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_pvid_set)
                (unit, port, vid, priority));
                
        pcfg = &robo_port_config[unit][port];
        /* pc_vlan = VID + priority */
        pcfg->pc_vlan = vid;
    }
    robo_p_ut_prio = priority;

    return SOC_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_untagged_priority_set
 * Purpose:
 *  Set the 802.1P priority for untagged packets coming in on a
 *  port.  This value will be written into the priority field of the
 *  tag that is added at the ingress.
 * Parameters:
 *  unit      - RoboSwitch Unit #.
 *  port      - RoboSwitch port #.
 *      priority  - Priority to be set in 802.1p priority tag, from 0 to 7
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    int     rv;

    soc_cm_debug(DK_PORT, 
                "BCM API : bcm_robo_port_untagged_priority_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    rv = _bcm_robo_port_untagged_priority_set(unit, port, priority);

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_ut_priority_set: u=%d p=%d pri=%d rv=%d\n",
             unit, port, priority, rv));
    return rv;
}

/*
 * Function:
 *  bcm_robo_port_untagged_priority_get
 * Purpose:
 *      Returns priority being assigned to untagged receive packets
 * Parameters:
 *  unit      - RoboSwitch Unit #.
 *  port      - RoboSwitch port #.
 *      priority  - Pointer to an int in which priority value is returned.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
   
    uint32          vid, pri;
   
    soc_cm_debug(DK_PORT, 
                "BCM API : bcm_robo_port_untagged_priority_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_pvid_get)
        (unit, port, &vid, &pri));

    if (priority != NULL) 
    {
        *priority = pri;

    }

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_ut_priority_get: u=%d p=%d pri=%d\n",
             unit, port, pri));

    return BCM_E_NONE;
}
                      

/* DSCP mapping functions */

/*
 * Function:
 *      bcm_robo_port_dscp_map_mode_get
 * Purpose:
 *      DSCP mapping status for the port.
 * Parameters:
 *      unit - switch device
 *      port - switch port or -1 to get mode from first available port
 *      mode - (OUT)
 *           - BCM_PORT_DSCP_MAP_NONE
 *           - BCM_PORT_DSCP_MAP_ZERO Map if incomming DSCP = 0
 *           - BCM_PORT_DSCP_MAP_ALL DSCP -> DSCP mapping. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_port_dscp_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int rv;
    uint8 tmp = 0;
    
    if (port != -1) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));
    }

    if (port == -1) {
        PBMP_E_ITER(unit, port) {
            break;
        }
    }
    if (!IS_E_PORT(unit, port)) {
        rv = BCM_E_PORT;
    } else {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_get)
                        (unit, port, 
                        DRV_QUEUE_MAP_DFSV, &tmp));
    }
    
    if (tmp) {
        *mode = BCM_PORT_DSCP_MAP_ALL;
    } else {
        *mode = BCM_PORT_DSCP_MAP_NONE;
    }
    rv = BCM_E_NONE;
    return rv;
}

/*
 * Function:
 *      bcm_robo_port_dscp_map_mode_set
 * Purpose:
 *      Set DSCP mapping for the port.
 * Parameters:
 *      unit - switch device
 *      port - switch port      or -1 to apply on all the ports.
 *      mode - BCM_PORT_DSCP_MAP_NONE
 *           - BCM_PORT_DSCP_MAP_ZERO Map if incomming DSCP = 0
 *           - BCM_PORT_DSCP_MAP_ALL DSCP -> DSCP mapping. 
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_port_dscp_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int rv;
    bcm_pbmp_t t_pbm;

    if (port != -1) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));
    }
    rv = BCM_E_PORT;

    switch(mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            if (port == -1) {
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, PBMP_E_ALL(unit), 
                            DRV_QUEUE_MAP_DFSV, FALSE));             
            } else {
                if (IS_E_PORT(unit, port)) {
                    BCM_PBMP_CLEAR(t_pbm); 
                    BCM_PBMP_PORT_ADD(t_pbm, port);
                    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, t_pbm, 
                            DRV_QUEUE_MAP_DFSV, FALSE));
                }
            }
            break;
        case BCM_PORT_DSCP_MAP_ZERO:    
            rv = BCM_E_UNAVAIL;
            break;
        case BCM_PORT_DSCP_MAP_ALL:
            if (port == -1) {
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, PBMP_E_ALL(unit), 
                            DRV_QUEUE_MAP_DFSV, TRUE));             
            } else {
                if (IS_E_PORT(unit, port)) {
                    BCM_PBMP_CLEAR(t_pbm); 
                    BCM_PBMP_PORT_ADD(t_pbm, port);
                    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, t_pbm, 
                            DRV_QUEUE_MAP_DFSV, TRUE));
                }
            }
            break;
        default:
            return BCM_E_PARAM;
    }
    rv = BCM_E_NONE;
    return rv;
}

#ifdef LVL7_FIXUP
int
bcm_robo_port_dot1p_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int rv;
    bcm_pbmp_t t_pbm;

    if (port != -1) {
        PORT_PARAM_CHECK(unit, port);
    }
    rv = BCM_E_PORT;

    switch(mode) {
        case 0 :
            if (port == -1) {
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, PBMP_E_ALL(unit),
                            DRV_QUEUE_MAP_PRIO, FALSE));
            } else {
                if (IS_E_PORT(unit, port)) {
                    BCM_PBMP_CLEAR(t_pbm);
                    BCM_PBMP_PORT_ADD(t_pbm, port);
                    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, t_pbm,
                            DRV_QUEUE_MAP_PRIO, FALSE));
                }
            }
            break;
        case 1:
            if (port == -1) {
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, PBMP_E_ALL(unit),
                            DRV_QUEUE_MAP_PRIO, TRUE));
            } else {
                if (IS_E_PORT(unit, port)) {
                    BCM_PBMP_CLEAR(t_pbm);
                    BCM_PBMP_PORT_ADD(t_pbm, port);
                    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, t_pbm,
                            DRV_QUEUE_MAP_PRIO, TRUE));
                }
            }
            break;

        /* Port based QoS for 53115 */
        case 2:
            if (port == -1) {
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, PBMP_E_ALL(unit),
                            DRV_QUEUE_MAP_PORT, TRUE));
            } else {
                if (IS_E_PORT(unit, port)) {
                    BCM_PBMP_CLEAR(t_pbm);
                    BCM_PBMP_PORT_ADD(t_pbm, port);
                    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, t_pbm,
                            DRV_QUEUE_MAP_PORT, TRUE));
                }
            }
            break;

        default:
            return BCM_E_PARAM;
    }
    rv = BCM_E_NONE;
    return rv;
}
#endif

/*
 * Function:
 *  bcm_robo_port_dscp_map_set
 * Purpose:
 *  Define a mapping from diffserv code points to CoS queue.
 * Parameters:
 *  unit  - RoboSwitch unit #
 *  port  - RoboSwitch port #, ignore in Robo.
 *  srccp - src code point or -1
 *    mapcp - mapped code point or -1
 *    prio - priority value for mapped code point
 *              -1 to use port default untagged priority
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  1. Now Only BCM5338 support this feature.
 *      srccp -1, mapcp -1: disable the DiffServ function.
 *      srccp -1, mapcp 0..3:   map all packets
 *      srccp 0,  mapcp 0..3:   map packets with cp 0
 *  2. the selective on DiffServ/TOS will impact in each other.
 *      If current device is working at TOS mode, called this API may
 *        set this device been working at DiffServ mode.
 */
int
bcm_robo_port_dscp_map_set(int unit, bcm_port_t port, int srccp, int mapcp,
               int prio)
{
#define DSCP_CODE_POINT_CNT 64
#define DSCP_CODE_POINT_MAX (DSCP_CODE_POINT_CNT - 1)

    pbmp_t      t_pbm;
    uint8 queue;

    if (port != -1 && !IS_E_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    
    if (srccp < -1 || srccp > DSCP_CODE_POINT_MAX) {
        return BCM_E_PARAM;
    }

    if (mapcp < -1 || mapcp > DSCP_CODE_POINT_MAX) {
        return BCM_E_PARAM;
    }

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_dscp_map_set()..\n");
   
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    
    if (soc_feature(unit, soc_feature_dscp)) {
        
        if (srccp != mapcp) {        
                return BCM_E_UNAVAIL;
        }
        if (srccp < 0 && mapcp < 0) {
            /* No mapping  disable DFSV*/
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_mapping_type_set)
                            (unit, t_pbm, 
                            DRV_QUEUE_MAP_DFSV, FALSE));
                    
            return BCM_E_NONE;
        }
        
        if (prio < 0) {
            /* prio == -1 get untag priority*/
            bcm_port_untagged_priority_get(unit, port, &prio);
        } 
        /* get the mapping queue with giving prio */
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_prio_get)
                (unit, -1, prio, &queue));
            /* set DSCP mapping to queue */
#ifndef LVL7_FIXUP 
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_dfsv_set)
                    (unit, srccp, queue));
#else
            /* Fastpath Applicantion sends the Queues information
               itself. Identifying the Queue based on priority is
               giving the wrong the values.So sending the prio
               information as the Queue. */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_dfsv_set)
                    (unit, srccp, prio));
#endif

    }
        
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_dscp_map_get
 * Purpose:
 *  Get a mapped CoS queue for a specific diffserv code points.
 * Parameters:
 *  unit  - RoboSwitch unit #
 *  port  - RoboSwitch port #, ignore in Robo.
 *  srccp - src code point or -1
 *    mapcp - (OUT) pointer to returned mapped code point
 *    prio - (OUT) Priority value for mapped code point or -1
 *                      May have BCM_PRIO_DROP_FIRST or'ed into it
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  1. the "prio" parameter is not used in RoboSwitch.
 */
int
bcm_robo_port_dscp_map_get(int unit, bcm_port_t port, int srccp, int *mapcp,
               int *prio)
{
    uint8 drv_value;
    uint8 pri, queue_num;
    int i;
    
    /*
      * Global DSCP table.
      */
    if (port != -1) {
        return BCM_E_PORT;
    }
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_dscp_map_get()..\n");
   
    if ((srccp < -1) || (srccp > DSCP_CODE_POINT_CNT) || 
        (mapcp == NULL) || (prio == NULL)) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_dscp)) {
        
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_dfsv_get)
                            (unit, (uint8)srccp, &drv_value));
        
    } else {
        return BCM_E_UNAVAIL;
    }
    pri = 8;
    for (i=0; i < pri; i++) {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->queue_prio_get)
            (unit, -1, i, &queue_num));
        if (queue_num == drv_value){
            break;
        }
    }

    *prio = pri;
    *mapcp = srccp;
    return BCM_E_NONE;
}
#undef DSCP_CODE_POINT_MAX
#undef DSCP_CODE_POINT_CNT
 
/*
 * General port functions
 */

/*
 * Function:
 *  bcm_robo_port_linkscan_get
 * Purpose:
 *  Get the link scan state of the port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  linkscan - (OUT) Linkscan value (None, S/W, H/W)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_linkscan_get(int unit, bcm_port_t port, int *linkscan)
{
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_linkscan_get()..\n");
    return bcm_linkscan_mode_get(unit, port, linkscan);
}

/*
 * Function:
 *  bcm_robo_port_linkscan_set
 * Purpose:
 *  Set the linkscan state for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  linkscan - Linkscan value (None, S/W, H/W)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_linkscan_set(int unit, bcm_port_t port, int linkscan)
{
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_linkscan_set()..\n");
    return bcm_linkscan_mode_set(unit, port, linkscan);
}
    

/*
 * Function:
 *  bcm_robo_port_autoneg_get
 * Purpose:
 *  Get the autonegotiation state of the port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  autoneg - (OUT) Boolean value
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_autoneg_get(int unit, bcm_port_t port, int *autoneg)
{
    int         rv;
    int         drv_act_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_autoneg_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    /* Note : 
     *   - port_get(DRV_PORT_PROP_AUTONEG) will return AN status.
     *      So we need to trnslate status into boolean.
     */
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_AUTONEG, (uint32 *) &drv_act_value));
    *autoneg = (drv_act_value == DRV_PORT_STATUS_AUTONEG_ENABLE) ?
                        TRUE : FALSE;

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_autoneg_set
 * Purpose:
 *  Set the autonegotiation state for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  autoneg - Boolean value
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_autoneg_set(int unit, bcm_port_t port, int autoneg)
{

    int         rv;
    int         drv_act_value;
    pbmp_t      t_pbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_autoneg_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    drv_act_value = autoneg;
    
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, t_pbm, 
            DRV_PORT_PROP_AUTONEG, drv_act_value));
    return rv;

}
    

/*
 * Function:
 *  bcm_robo_port_speed_get
 * Purpose:
 *  Getting the speed of the port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  If port is in MAC loopback, the speed of the loopback is returned.
 *  On chips with a PHYMOD field:
 *      If PHYMOD=0, the speed is hardwired to 100Mb/s.
 *      If PHYMOD=1, the speed is obtained directly from the PHY.
 *  In either case, FE_SUPP.SPEED is completely ignored.
 */

int
bcm_robo_port_speed_get(int unit, bcm_port_t port, int *speed)
{
    int     rv = SOC_E_NONE;
    uint32  drv_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_speed_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_SPEED, &drv_value));

    if (drv_value != DRV_PORT_STATUS_SPEED_10G) {
        *speed = (drv_value == DRV_PORT_STATUS_SPEED_10M) ? 10 : 
                    (drv_value == DRV_PORT_STATUS_SPEED_100M) ? 100 : 
                    (drv_value == DRV_PORT_STATUS_SPEED_1G) ? 1000 :
                    0;
    }

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_speed_max
 * Purpose:
 *  Getting the maximum speed of the port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  speed - (OUT) Value in megabits/sec (10, 100, etc)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_speed_max(int unit, bcm_port_t port, int *speed)
{
    bcm_port_abil_t pm;
    int         rv;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_speed_max()..\n");
    rv = bcm_port_ability_get(unit, port, &pm);

    if (BCM_SUCCESS(rv)) {
        if (pm & (BCM_PORT_ABIL_10GB_FD | BCM_PORT_ABIL_10GB_HD)) {
            *speed = 10000;
        } else if (pm & (BCM_PORT_ABIL_2500MB_FD | BCM_PORT_ABIL_2500MB_HD)) {
            *speed = 2500;
        } else if (pm & (BCM_PORT_ABIL_1000MB_FD | BCM_PORT_ABIL_1000MB_HD)) {
            *speed = 1000;
        } else if (pm & (BCM_PORT_ABIL_100MB_FD | BCM_PORT_ABIL_100MB_HD)) {
            *speed = 100;
        } else if (pm & (BCM_PORT_ABIL_10MB_FD | BCM_PORT_ABIL_10MB_HD)) {
            *speed = 10;
        }
    }

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_speed_set
 * Purpose:
 *  Setting the speed for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  speed - Value in megabits/sec (10, 100, etc)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Turns off autonegotiation.  Caller must make sure other forced
 *  parameters (such as duplex) are set.
 */

int
bcm_robo_port_speed_set(int unit, bcm_port_t port, int speed)
{
    int     rv;
    pbmp_t  pbm;
    uint32  drv_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_speed_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    drv_value = (speed == 10) ? DRV_PORT_STATUS_SPEED_10M : 
            (speed == 100) ? DRV_PORT_STATUS_SPEED_100M :
            DRV_PORT_STATUS_SPEED_1G;
            
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, pbm, 
            DRV_PORT_PROP_SPEED, drv_value));
    if (BCM_SUCCESS(rv) && !SAL_BOOT_SIMULATION) {
        (void)bcm_link_change(unit, pbm);
    }

    return rv;
}
    

/*
 * Function:
 *  bcm_robo_port_master_get
 * Purpose:
 *  Getting the master status of the port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  ms - (OUT) BCM_PORT_MS_*
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *      WARNING: assumes BCM_PORT_MS_* matches SOC_PORT_MS_*
 */

int
bcm_robo_port_master_get(int unit, bcm_port_t port, int *ms)
{
    int     rv;
    uint32  drv_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_master_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    /* Get the master type */
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_MS, &drv_value));
            
    *ms = (drv_value == SOC_PORT_MS_SLAVE) ? BCM_PORT_MS_SLAVE : 
            (drv_value == SOC_PORT_MS_MASTER) ? BCM_PORT_MS_MASTER : 
            (drv_value == SOC_PORT_MS_AUTO) ? BCM_PORT_MS_AUTO : 
                    BCM_PORT_MS_NONE;

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_master_set
 * Purpose:
 *  Setting the master status for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  ms - BCM_PORT_MS_*
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Ignored if not supported on port.
 *      WARNING: assumes BCM_PORT_MS_* matches SOC_PORT_MS_*
 */

int
bcm_robo_port_master_set(int unit, bcm_port_t port, int ms)
{
    int     rv;
    pbmp_t  pbm;
    uint32  drv_value = 0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_master_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    drv_value = (ms == BCM_PORT_MS_SLAVE) ? SOC_PORT_MS_SLAVE : 
                (ms == BCM_PORT_MS_MASTER) ? SOC_PORT_MS_MASTER : 
                (ms == BCM_PORT_MS_AUTO) ? SOC_PORT_MS_AUTO : 
                        SOC_PORT_MS_NONE;
    
    BCM_PBMP_CLEAR(pbm); 
    BCM_PBMP_PORT_ADD(pbm, port);
    /* set the master type */
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, pbm, 
            DRV_PORT_PROP_MS, drv_value));

    SOC_IF_ERROR_DEBUG_PRINT
        (rv, (DK_VERBOSE, "PHY_MASTER_SET failed: %s\n", bcm_errmsg(rv)));

    if (BCM_SUCCESS(rv)) {
        (void)bcm_link_change(unit, pbm);
    }

    return rv;
}
    

/*
 * Function:
 *  bcm_robo_port_interface_get
 * Purpose:
 *  Getting the interface type of a port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  intf - (OUT) BCM_PORT_IF_*
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *      WARNING: assumes BCM_PORT_IF_* matches SOC_PORT_IF_*
 */

int
bcm_robo_port_interface_get(int unit, bcm_port_t port, bcm_port_if_t *intf)
{
    int     rv;
    uint32  drv_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_interface_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, DRV_PORT_PROP_INTERFACE, &drv_value));
            
    SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_PORT|DK_VERBOSE, "DRV_PORT_PROP_INTERFACE failed: %s\n", 
            bcm_errmsg(rv)));

    *intf = (drv_value == SOC_PORT_IF_MII) ? BCM_PORT_IF_MII : 
            (drv_value == SOC_PORT_IF_GMII) ? BCM_PORT_IF_GMII :
            (drv_value == SOC_PORT_IF_RGMII) ? BCM_PORT_IF_RGMII :
            (drv_value == SOC_PORT_IF_SGMII) ? BCM_PORT_IF_SGMII :
            (drv_value == SOC_PORT_IF_XGMII) ? BCM_PORT_IF_XGMII :
                    BCM_PORT_IF_TBI;

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_interface_set
 * Purpose:
 *  Setting the interface type for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  if - BCM_PORT_IF_*
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *      WARNING: assumes BCM_PORT_IF_* matches SOC_PORT_IF_*
 */

int
bcm_robo_port_interface_set(int unit, bcm_port_t port, bcm_port_if_t intf)
{
    int     rv;
    pbmp_t  pbm;
    uint32  drv_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_interface_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    switch(intf) {
        case BCM_PORT_IF_MII:
            drv_value = SOC_PORT_IF_MII;
            break;
        case BCM_PORT_IF_GMII:
            drv_value = SOC_PORT_IF_GMII;
            break;
        case BCM_PORT_IF_RGMII:
            drv_value = SOC_PORT_IF_RGMII;
            break;
        case BCM_PORT_IF_SGMII:
                drv_value = SOC_PORT_IF_SGMII;
                break;         
        default:
            drv_value = SOC_PORT_IF_TBI;
            break;
    }

    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, pbm,
            DRV_PORT_PROP_INTERFACE, drv_value));
    SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_PORT|DK_VERBOSE, "DRV_PORT_PROP_INTERFACE failed: %s\n", 
            bcm_errmsg(rv)));

    if (BCM_SUCCESS(rv)) {
        (void)bcm_link_change(unit, pbm);
    }

    return rv;
}

 
/*
 * Function:
 *  bcm_robo_port_duplex_get
 * Purpose:
 *  Get the port duplex settings
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      duplex - (OUT) Duplex setting, one of BCM_PORT_DUPLEX_xxx
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_duplex_get(int unit, bcm_port_t port, int *duplex)
{
    int         rv;
    int         drv_act_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_duplex_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
        
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_DUPLEX, (uint32 *) &drv_act_value));
    
    *duplex = (drv_act_value== DRV_PORT_STATUS_DUPLEX_FULL) ?
                BCM_PORT_DUPLEX_FULL : BCM_PORT_DUPLEX_HALF;

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_duplex_set
 * Purpose:
 *  Set the port duplex settings.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      duplex - Duplex setting, one of SOC_PORT_DUPLEX_xxx
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Turns off autonegotiation.  Caller must make sure other forced
 *  parameters (such as speed) are set.
 */

int
bcm_robo_port_duplex_set(int unit, bcm_port_t port, int duplex)
{
    int         rv;
    int         drv_act_value;
    pbmp_t      t_pbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_duplex_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    if (duplex == BCM_PORT_DUPLEX_HALF){
        drv_act_value = DRV_PORT_STATUS_DUPLEX_HALF;
    }else {
        drv_act_value = DRV_PORT_STATUS_DUPLEX_FULL;      
    }
    
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, t_pbm,
            DRV_PORT_PROP_DUPLEX, drv_act_value));
    return rv;
}
    

/*
 * Function:
 *  bcm_robo_port_pause_get
 * Purpose:
 *  Get the pause state for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  pause_tx - (OUT) Boolean value
 *  pause_rx - (OUT) Boolean value
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */





int
bcm_robo_port_pause_get(int unit, bcm_port_t port, 
                int *pause_tx, int *pause_rx)
{
    uint32  drv_value=0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_pause_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
                (unit, port, 
                DRV_PORT_PROP_TX_PAUSE, &drv_value));
    *pause_tx = (drv_value) ? TRUE : FALSE;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
                (unit, port, 
                DRV_PORT_PROP_RX_PAUSE, &drv_value));
    *pause_rx = (drv_value) ? TRUE : FALSE;
    soc_cm_debug(DK_PORT, "\t Port_Pause, tpau=%d, rpau=%d\n",
                *pause_tx, *pause_rx);

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_pause_set
 * Purpose:
 *  Set the pause state for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  pause_tx - Boolean value
 *  pause_rx - Boolean value
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Symmetric pause requires the two "pause" values to be the same.
 */

int
bcm_robo_port_pause_set(int unit, bcm_port_t port, 
                int pause_tx, int pause_rx)
{
    pbmp_t  t_pbm;
    uint32  drv_value = 0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_pause_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    if (pause_tx >= 0){
        drv_value = pause_tx;
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                (unit, t_pbm,
                DRV_PORT_PROP_TX_PAUSE, drv_value));
   }
    
    if (pause_rx >= 0){
        drv_value = pause_rx;

        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                (unit, t_pbm,
                DRV_PORT_PROP_RX_PAUSE, drv_value));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_pause_addr_get
 * Purpose:
 *  Get the source address for transmitted PAUSE frames.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  pause_tx - (OUT) Boolean value
 *  pause_rx - (OUT) Boolean value
 *  mac - (OUT) MAC address sent with pause frames.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */
int
bcm_robo_port_pause_addr_get(int unit, bcm_port_t port, mac_addr_t mac)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *  bcm_robo_port_pause_addr_set
 * Purpose:
 *  Set the source address for transmitted PAUSE frames.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  pause_tx - Boolean value
 *  pause_rx - Boolean value
 *  mac - station address used for pause frames.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Symmetric pause requires the two "pause" values to be the same.
 */
int
bcm_robo_port_pause_addr_set(int unit, bcm_port_t port, mac_addr_t mac)
{
    return (BCM_E_UNAVAIL);
}
    

/*
 * Function:
 *  bcm_robo_port_pause_sym_get
 * Purpose:
 *  Get the current pause setting for pause
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  pause - (OUT) returns a bcm_port_pause_e enum value
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_pause_sym_get(int unit, bcm_port_t port, int *pause)
{
    int     rv;
    int     pause_rx, pause_tx;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_pause_sym_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = bcm_port_pause_get(unit, port, &pause_tx, &pause_rx);

    BCM_IF_ERROR_RETURN(rv);
    if (pause_tx){
        if (pause_rx){
            *pause = BCM_PORT_PAUSE_SYM;
        } else {
            *pause = BCM_PORT_PAUSE_ASYM_TX;
        }
    } else if (pause_rx){
        *pause = BCM_PORT_PAUSE_ASYM_RX;
    } else {
        *pause = BCM_PORT_PAUSE_NONE;
    }

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_pause_sym_set
 * Purpose:
 *  Set the pause values for the port using single integer
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  pause - a bcm_port_pause_e enum value
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_pause_sym_set(int unit, bcm_port_t port, int pause)
{
    int     pause_rx, pause_tx;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_pause_sym_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    pause_tx = pause_rx = 0;

    switch (pause) {
    case BCM_PORT_PAUSE_SYM:
        pause_tx = pause_rx = 1;
        break;
    case BCM_PORT_PAUSE_ASYM_RX:
        pause_rx = 1;
        break;
    case BCM_PORT_PAUSE_ASYM_TX:
        pause_tx = 1;
        break;
    case BCM_PORT_PAUSE_NONE:
        break;
    default:
        return BCM_E_PARAM;
    }

    return bcm_port_pause_set(unit, port, pause_tx, pause_rx);
}

/*
 * Function:
 *  _bcm_robo_port_update
 * Purpose:
 *  Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      link -  True if link is active, false if link is inactive.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
_bcm_robo_port_update(int unit, bcm_port_t port, int link)
{

    int                     an;
    int                     rv;
    pbmp_t pbmp;

    /* get AN status */
    if ((rv = (DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_AUTONEG, (uint32 *) &an)) < 0) {
        return rv;
    }

    if (!link) {
        /* Disable MAC */

        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_PORT_ADD(pbmp, port);
        if ((rv = (DRV_SERVICES(unit)->port_set)
                (unit, pbmp, 
                DRV_PORT_PROP_MAC_ENABLE, FALSE)) < 0) {
            return rv;
        }
        
        /* phy link-down event */
        if ((rv = (DRV_SERVICES(unit)->port_set)
                (unit, pbmp, 
                DRV_PORT_PROP_PHY_LINKDN_EVT, 0)) < 0) {
            return rv;
        }
        return (BCM_E_NONE);
    }

    /* PHY link up event */

    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_PORT_ADD(pbmp, port);
    if ((rv = (DRV_SERVICES(unit)->port_set)
            (unit, pbmp, 
            DRV_PORT_PROP_PHY_LINKUP_EVT, 0)) < 0) {
        return rv;
    }
    
    /* in new ROBO linkscan each ROBO chip will use the same linkscan process
     * and all ports' MAC on the chip will be updated by this new linkscan 
     * through SW override.
     */

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    if ((rv = (DRV_SERVICES(unit)->port_sw_mac_update)
                    (unit, pbmp)) < 0) {
        return rv;
    }

    /*
     * If autonegotiating, check the negotiated PAUSE values, and program
     * MACs accordingly.
     */

    if (an == DRV_PORT_STATUS_AUTONEG_ENABLE) {
        bcm_port_abil_t     r_advert, l_advert;
        int         tx_pause, rx_pause;

        /* get PHY local/remot advertise */
        r_advert = 0;
        l_advert = 0;
        if ((rv = (DRV_SERVICES(unit)->port_get)
                (unit, port, 
                DRV_PORT_PROP_LOCAL_ADVERTISE, (uint32 *)&l_advert)) < 0) {
            return rv;
        }
        if ((rv = (DRV_SERVICES(unit)->port_get)
                (unit, port, 
                DRV_PORT_PROP_REMOTE_ADVERTISE, (uint32 *)&r_advert)) < 0) {
            return rv;
        }
        
        /*
         * IEEE 802.3 Flow Control Resolution.
         * Please see $SDK/doc/pause-resolution.txt for more information.
         */
        
        tx_pause =
            ((r_advert & SOC_PM_PAUSE_RX) &&
             (l_advert & SOC_PM_PAUSE_RX)) ||
            ((r_advert & SOC_PM_PAUSE_RX) &&
             !(r_advert & SOC_PM_PAUSE_TX) &&
             (l_advert & SOC_PM_PAUSE_TX));
        
        rx_pause =
            ((r_advert & SOC_PM_PAUSE_RX) &&
             (l_advert & SOC_PM_PAUSE_RX)) ||
            ((l_advert & SOC_PM_PAUSE_RX) &&
             (r_advert & SOC_PM_PAUSE_TX) &&
             !(l_advert & SOC_PM_PAUSE_TX));

        soc_cm_debug(DK_PORT,
                 "_port_update: Pause check @ AN, tpau=%d,rpau=%d\n",
                 tx_pause, rx_pause);
       /* set proper MAC pause */

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port);
        if ((rv = (DRV_SERVICES(unit)->port_set)
                (unit, pbmp, 
                DRV_PORT_PROP_TX_PAUSE, tx_pause)) < 0) {
            return rv;
        }
        
        if ((rv = (DRV_SERVICES(unit)->port_set)
                (unit, pbmp, 
                DRV_PORT_PROP_RX_PAUSE, rx_pause)) < 0) {
            return rv;
        }
    }

    /* Enable the MAC. */
    if ((rv = (DRV_SERVICES(unit)->port_set)
            (unit, pbmp, 
            DRV_PORT_PROP_ENABLE, TRUE)) < 0) {
        return rv;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_update
 * Purpose:
 *  Get port characteristics from PHY and program MAC to match.
 * Parameters:
 *  unit -  RoboSwitch Unit #.
 *  port -  RoboSwitch port #.
 *  link -  TRUE - process as link up.
 *          FALSE - process as link down.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_update(int unit, bcm_port_t port, int link)
{
    int     rv = BCM_E_NONE;

    soc_cm_debug(DK_PORT, 
                "BCM API : bcm_robo_port_update()..port=%d, link=%d\n",
                port, link);
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = _bcm_robo_port_update(unit, port, link);

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_update: u=%d p=%d link=%d rv=%d\n",
             unit, port, link, rv));

    return(rv);

}
    
/*
 * Function:
 *  bcm_port_frame_max_get
 * Description:
 *  Set the maximum receive frame size for the port
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  size - Maximum frame size in bytes
 * Return Value:
 *  BCM_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size 
 *      might be slightly higher.
 *
 *      For GE ports that use 2 separate MACs (one for GE and another one for
 *      10/100 modes) the function returns the maximum rx frame size set for
 *      the current mode.
 */

int
bcm_robo_port_frame_max_get(int unit, bcm_port_t port, int * size)
{
    int     rv;
    uint32  temp;

    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_frame_max_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, DRV_PORT_PROP_MAX_FRAME_SZ, &temp));
            
    SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_PORT|DK_VERBOSE, "DRV_PORT_PROP_INTERFACE failed: %s\n", 
            bcm_errmsg(rv)));

    *size = temp;

    return rv;
}

/*
 * Function:
 *  bcm_port_frame_max_set
 * Description:
 *  Set the maximum receive frame size for the port
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  size - Maximum frame size in bytes
 * Return Value:
 *  BCM_E_XXX
 * Notes:
 *      Depending on chip or port type the actual maximum receive frame size 
 *      might be slightly higher.
 *
 *      It looks like this operation is performed the same way on all the chips
 *      and the only depends on the port type.
 */
int
bcm_robo_port_frame_max_set(int unit,bcm_port_t port, int size)
{
    int     rv;
    pbmp_t  pbm;
    uint32      temp;
    
    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_frame_max_set()..\n");
    
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);

    temp = size;
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, pbm,
            DRV_PORT_PROP_MAX_FRAME_SZ, temp));
    SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_PORT|DK_VERBOSE, "DRV_PORT_PROP_MAX_FRAME_SZ failed: %s\n", 
            bcm_errmsg(rv)));

    return rv;
}
    

/*
 * Function:
 *  bcm_port_jam_get
 * Description:
 *  Return half duplex jamming state
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  enable - (OUT) non-zero if jamming enabled
 * Return Value:
 *  BCM_E_XXX
 */
int
bcm_robo_port_jam_get(int unit, bcm_port_t port, int * enable)
{
    uint32    drv_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_jam_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    if (!IS_E_PORT(unit, port)) {    /* CPU ports */
        *enable = 0;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
            (unit, port, DRV_PORT_PROP_JAM, &drv_value));
    *enable = drv_value ? TRUE : FALSE;
    
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_port_jam_set
 * Description:
 *  Enable or disable half duplex jamming on a port
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  enable - non-zero to enable jamming
 * Return Value:
 *  BCM_E_XXX
 * Note :
 *  1. RoboSwitch allowed per port flow control setting but such 
 *     setting effect on Pause(FD) and Jamming(HD) at the same time.
 *     The Pause and Jamming can't be set independent.
 *  2. This API for setting Jamming flow control in Robo chip might 
 *     effects pause flow control setting for above reason.
 */
int
bcm_robo_port_jam_set(int unit, bcm_port_t port, int enable)
{
    pbmp_t pbmp;
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_jam_set()..\n");

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    if (!IS_E_PORT(unit, port)) {    /* CPU ports */
        return BCM_E_NONE;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
            (unit, pbmp, DRV_PORT_PROP_JAM, enable));
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_port_ifg_get
 * Description:
 *  Gets the new ifg (Inter-frame gap) value
 * Parameters:
 *  unit   - Device number
 *  port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *  ifg    - Inter-frame gap in bit-times
 * Return Value:
 *  BCM_E_XXX
 * Notes:
 */
int
bcm_robo_port_ifg_get( int unit, bcm_port_t port, int speed,
                bcm_port_duplex_t duplex, int * bit_times)
{
    int rv;
    
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_ifg_get()..\n");
            
    if (speed == 1000){
        /* get GE IPG */
        rv = ((DRV_SERVICES(unit)->port_get)
                (unit, port, DRV_PORT_PROP_IPG_GE, (uint32 *) bit_times));
    } else {
        /* get FE IPG */
        rv = ((DRV_SERVICES(unit)->port_get)
                (unit, port, DRV_PORT_PROP_IPG_FE, (uint32 *) bit_times));
    }

    return (rv);
}

/*
 * Function:
 *  bcm_port_ifg_set
 * Description:
 *  Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *  unit   - Device number
 *  port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *  ifg    - Inter-frame gap in bit-times
 * Return Value:
 *  BCM_E_XXX
 * Notes:
 *      1. The function makes sure the IFG value makes sense and updates the
 *         IPG register in case the speed/duplex match the current settings
 *      2. Robo5324 can only set the IPG(IFG) per device but per port basis.
 *         That's why this API is unavailable.
 */
int
bcm_robo_port_ifg_set(int unit, bcm_port_t port, int speed,
                bcm_port_duplex_t duplex, int bit_times)
{
    return BCM_E_UNAVAIL;
}
                            

/*
 * Additional PHY-related APIs
 */

/*
 * Function:
 *  bcm_port_phy_drv_name_get
 * Purpose:
 *  Return the name of the PHY driver being used on a port.
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 * Returns:
 *  Pointer to static string
 * Note :
 *  This API is un-dispatchable, if any upper layer application
 *  trying to call this API should give the full name(with "robo_"
 *  string appended to this API's name after "bcm_xxx") to point 
 *  the process to this API.
 */

int
bcm_robo_port_phy_drv_name_get(int unit, bcm_port_t port, char *name, int len)
{
    uint32 val = FALSE;
    int rv;
    int str_len;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_phy_drv_name_get()..\n");

    rv = ((DRV_SERVICES(unit)->port_status_get)
                (unit, 0, DRV_PORT_STATUS_INIT, &val));
    
    if (!val) {
        str_len = sal_strlen("driver not initialized");
        if (str_len <= len) {
            sal_strcpy(name, "driver not initialized");
        }
        return BCM_E_INIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        str_len = sal_strlen("invalid port");
        if (str_len <= len) {
            sal_strcpy(name, "invalid port");
        }
        return BCM_E_PORT;
    }
    return (soc_phyctrl_drv_name_get(unit, port, name, len)); 
}

/*
 * Direct PHY register access
 */

/*
 * Function:
 *  int bcm_port_phy_get
 * Description:
 *  General PHY register read
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  flags - Logical OR of one or more of the following flags:
 *      BCM_PORT_PHY_INTERNAL
 *          Address internal SERDES PHY for port
 *      BCM_PORT_PHY_NOMAP
 *          Instead of mapping port to PHY MDIO address,
 *          treat port parameter as actual PHY MDIO address.
 *      BCM_PORT_PHY_CLAUSE45
 *          Assume Clause 45 device instead of Clause 22
 *  phy_addr - PHY internal register address
 *  phy_data - (OUT) Data that was read
 * Returns:
 *  BCM_E_XXX
 */

int
bcm_robo_port_phy_get(int unit, bcm_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 * phy_data)
{
    uint8  phy_id;
    uint16 phy_reg;
    uint16 phy_rd_data;
    int    rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_phy_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    if (flags & BCM_PORT_PHY_NOMAP) {
        phy_id = port;
    } else {
        if (flags & BCM_PORT_PHY_INTERNAL) {
            phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
        } else {
            phy_id = PORT_TO_PHY_ADDR(unit, port);
        }
    }
    
    if (flags & BCM_PORT_PHY_CLAUSE45) {
        return BCM_E_UNAVAIL;
    } else {
        phy_reg = phy_reg_addr;
        rv = ((DRV_SERVICES(unit)->miim_read)
                    (unit, phy_id, phy_reg, &phy_rd_data));
    }

    if (rv == SOC_E_NONE) {
        *phy_data = phy_rd_data;
    }

    return rv;
}

/*
 * Function:
 *  int bcm_port_phy_set
 * Description:
 *  General PHY register write
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  flags - Logical OR of one or more of the following flags:
 *      BCM_PORT_PHY_INTERNAL
 *          Address internal SERDES PHY for port
 *      BCM_PORT_PHY_NOMAP
 *          Instead of mapping port to PHY MDIO address,
 *          treat port parameter as actual PHY MDIO address.
 *      BCM_PORT_PHY_CLAUSE45
 *          Assume Clause 45 device instead of Clause 22
 *  phy_addr - PHY internal register address
 *  phy_data - Data to write
 * Returns:
 *  BCM_E_XXX
 */

int
bcm_robo_port_phy_set(int unit, bcm_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data)
{
    uint8  phy_id;
    uint16 phy_reg;
    uint16 phy_wr_data;
    int    rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_phy_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    if (flags & BCM_PORT_PHY_NOMAP) {
        phy_id = port;
    } else {
        if (flags & BCM_PORT_PHY_INTERNAL) {
            phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
        } else {
            phy_id = PORT_TO_PHY_ADDR(unit, port);
        }
    }
    
    if (flags & BCM_PORT_PHY_CLAUSE45) {
        return BCM_E_UNAVAIL;
    } else {
        phy_reg = phy_reg_addr;
        phy_wr_data = phy_data;
        rv = ((DRV_SERVICES(unit)->miim_write)
                    (unit, phy_id, phy_reg, phy_wr_data));
    }

    return rv;
}

/*
 * Function:
 *      bcm_port_phy_reset
 * Description:
 *      This function performs the low-level PHY reset and is intended to be
 *      called ONLY from callback function registered with 
 *      bcm_port_phy_reset_register. Attempting to call it from any other 
 *      place will break lots of things.
 * Parameters:
 *      unit    - Device number
 *      port    - Port number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_port_phy_reset(int unit, bcm_port_t port)
{
    int rv;
    pbmp_t pbmp;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_phy_reset()..\n");

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    if ((rv = (DRV_SERVICES(unit)->port_set)
            (unit, pbmp, 
            DRV_PORT_PROP_PHY_RESET, TRUE)) < 0) {
        return rv;
    }
    return rv;
}

/* 
 * Function:
 *      bcm_port_phy_reset_register
 * Description:
 *      Register a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset 
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data} 
 *                           combination have not been registered before
 */
int
bcm_robo_port_phy_reset_register(int unit, bcm_port_t port, 
                        bcm_port_phy_reset_cb_t callback, void * user_data)
{
    return soc_phy_reset_register(unit, port, callback, user_data, FALSE);
}

/* 
 * Function:
 *      bcm_port_phy_reset_unregister
 * Description:
 *      Unregister a callback function to be called whenever a PHY driver
 *      needs to perform a PHY reset 
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data} 
 *                           combination have not been registered before
 */
int
bcm_robo_port_phy_reset_unregister(int unit,bcm_port_t port,
                        bcm_port_phy_reset_cb_t callback,void * user_data)
{
    return soc_phy_reset_unregister(unit, port, callback, user_data);
}


/*
 * MDI crossover control and status
 */
 

/*
 * Function:
 *  bcm_port_mdix_get
 * Description:
 *  Get the Auto-MDIX mode of a port/PHY
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  mode - (Out) One of:
 *            BCM_PORT_MDIX_AUTO
 *          Enable auto-MDIX when autonegotiation is enabled
 *            BCM_PORT_MDIX_FORCE_AUTO
 *          Enable auto-MDIX always
 *      BCM_PORT_MDIX_NORMAL
 *          Disable auto-MDIX
 *      BCM_PORT_MDIX_XOVER
 *          Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *  BCM_E_UNAVAIL - feature unsupported by hardware
 *  BCM_E_XXX - other error
 */
int
bcm_robo_port_mdix_get(int unit, bcm_port_t port, bcm_port_mdix_t * mode)
{
    int     rv = BCM_E_NONE;
    uint32  drv_value;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_mdix_get()..\n");

    /* get port mdix mode */
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_PHY_MDIX, &drv_value));
    *mode = (drv_value == SOC_PORT_MDIX_AUTO) ? BCM_PORT_MDIX_AUTO : 
            (drv_value == SOC_PORT_MDIX_FORCE_AUTO) ? BCM_PORT_MDIX_FORCE_AUTO :
            (drv_value == SOC_PORT_MDIX_NORMAL) ? BCM_PORT_MDIX_NORMAL :
                            BCM_PORT_MDIX_XOVER;
    

    return (rv);
}

/*
 * Function:
 *  bcm_port_mdix_set
 * Description:
 *  Set the Auto-MDIX mode of a port/PHY
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  mode - One of:
 *            BCM_PORT_MDIX_AUTO
 *          Enable auto-MDIX when autonegotiation is enabled
 *            BCM_PORT_MDIX_FORCE_AUTO
 *          Enable auto-MDIX always
 *      BCM_PORT_MDIX_NORMAL
 *          Disable auto-MDIX
 *      BCM_PORT_MDIX_XOVER
 *          Disable auto-MDIX, and swap cable pairs
 * Return Value:
 *  BCM_E_UNAVAIL - feature unsupported by hardware
 *  BCM_E_XXX - other error
 */
int
bcm_robo_port_mdix_set(
    int unit,
    bcm_port_t  port,
    bcm_port_mdix_t mode)
{
    int rv = BCM_E_NONE;
    pbmp_t      t_pbm;
    uint32      drv_value;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_mdix_set()..\n");
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    /* set port mdix mode */
    drv_value = (mode == BCM_PORT_MDIX_AUTO) ? SOC_PORT_MDIX_AUTO : 
            (mode == BCM_PORT_MDIX_FORCE_AUTO) ? SOC_PORT_MDIX_FORCE_AUTO :
            (mode == BCM_PORT_MDIX_NORMAL) ? SOC_PORT_MDIX_NORMAL : 
                            SOC_PORT_MDIX_XOVER;
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, t_pbm,
            DRV_PORT_PROP_PHY_MDIX, drv_value));
    soc_cm_debug(DK_PORT, 
            "bcm_robo_port_mdix_set()=%d: \n\t port_set()>>drv_value=%x\n", 
            rv, drv_value);
    return (rv);
}

/*
 * Function:
 *  bcm_port_mdix_status_get
 * Description:
 *  Get the current MDIX status on a port/PHY
 * Parameters:
 *  unit    - Device number
 *  port    - Port number
 *  status  - (OUT) One of:
 *            BCM_PORT_MDIX_STATUS_NORMAL
 *          Straight connection
 *            BCM_PORT_MDIX_STATUS_XOVER
 *          Crossover has been performed
 * Return Value:
 *  BCM_E_UNAVAIL - feature unsupported by hardware
 *  BCM_E_XXX - other error
 */
int
bcm_robo_port_mdix_status_get(
    int unit,
    bcm_port_t  port,
    bcm_port_mdix_status_t *    status)
{
    int     rv = BCM_E_NONE;
    uint32  drv_value;
    
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_mdix_status_get()..\n");
    /* get port mdix status */
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_PHY_MDIX_STATUS, &drv_value));
    *status = (drv_value == SOC_PORT_MDIX_STATUS_NORMAL) ? 
                                BCM_PORT_MDIX_STATUS_NORMAL : 
                                BCM_PORT_MDIX_STATUS_XOVER;
    
    return (rv);
}
                                   

/*
 * Combo port control/status 
 */

/*
 * Function:
 *      bcm_port_medium_config_get
 * Description:
 *      Get the medium-specific configuration for a combo port
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 to get the config for
 *      config   - per-medium configuration
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_robo_port_medium_config_get(
    int unit,
    bcm_port_t  port,
    bcm_port_medium_t   medium,
    bcm_phy_config_t *  config)
{
    int     rv = BCM_E_NONE;
    uint32  drv_type;
    
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_medium_config_get()..\n");
    drv_type = (medium == BCM_PORT_MEDIUM_FIBER) ?
            DRV_PORT_PROP_PHY_MEDIUM_CONFIG_FIBER : 
            DRV_PORT_PROP_PHY_MEDIUM_CONFIG_COPPER ;
    /* get port medium config */
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, drv_type,(uint32 *)config));

    return (rv);
}

/*
 * Function:
 *      bcm_port_medium_config_set
 * Description:
 *      Set the medium-specific configuration for a combo port
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 to apply the configuration to
 *      config   - per-medium configuration
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_robo_port_medium_config_set(
    int unit,
    bcm_port_t  port,
    bcm_port_medium_t   medium,
    bcm_phy_config_t *  config)
{
    int rv;
    pbmp_t      t_pbm;
    uint32      drv_type;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_medium_config_set()..\n");
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    
    drv_type = (medium == BCM_PORT_MEDIUM_FIBER) ?
                DRV_PORT_PROP_PHY_MEDIUM_CONFIG_FIBER : 
                DRV_PORT_PROP_PHY_MEDIUM_CONFIG_COPPER ;
                
    /* set port medium config */
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, t_pbm,
            drv_type, (uint32)config));
    return (rv);
}
                                      
/*
 * Function:
 *      bcm_port_medium_config_get
 * Description:
 *      Get the medium-specific configuration for a combo port
 * Parameters:
 *      unit     - Device number
 *      port     - Port number
 *      medium   - The medium (BCM_PORT_MEDIUM_COPPER or BCM_PORT_MEDIUM_FIBER)
 *                 to get the config for
 *      config   - per-medium configuration
 * Return Value:
 *      BCM_E_XXX
 */

int
bcm_robo_port_medium_get(
    int unit,
    bcm_port_t  port,
    bcm_port_medium_t * medium)
{
    int     rv = BCM_E_NONE;
    uint32  drv_value = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_medium_get()..\n");
    /* get port medium type */
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_PHY_MEDIUM, &drv_value));
    *medium = drv_value;
    return (rv);
}

/* 
 * Function:
 *      bcm_robo_port_medium_status_register
 * Description:
 *      Register a callback function to be called on medium change event
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM  -- NULL function pointer or bad {unit, port} combination
 *      BCM_E_FULL   -- Cannot register more than 1 callback per {unit, port}
 */
int
bcm_robo_port_medium_status_register(
    int unit,
    bcm_port_t  port,
    bcm_port_medium_status_cb_t callback,
    void *  user_data)
{
    return (soc_phy_medium_status_register(
                    unit, port, callback, user_data));
}

/* 
 * Function:
 *      bcm_robo_port_medium_status_unregister
 * Description:
 *      Unegister a callback function to be called on medium change event
 * Parameters:
 *      unit      - Device number
 *      port      - port number
 *      callback  - The callback function to call
 *      user_data - An opaque cookie to pass to callback function 
 *                  whenever it is called
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM       -- Bad {unit, port} combination
 *      BCM_E_NOT_FOUND   -- The specified {unit, port, callback, user_data} 
 *                           combination have not been registered before
 */
int
bcm_robo_port_medium_status_unregister(
    int unit,
    bcm_port_t  port,
    bcm_port_medium_status_cb_t callback,
    void *  user_data)
{
    return (soc_phy_medium_status_unregister(
                    unit, port, callback, user_data));
}

/*
 * Flags used for loopback modes
 */

 
/*
 * Function:
 *  bcm_robo_port_loopback_set
 * Purpose:
 *  Setting the speed for a given port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  loopback - one of:
 *      BCM_PORT_LOOPBACK_NONE
 *      BCM_PORT_LOOPBACK_MAC
 *      BCM_PORT_LOOPBACK_PHY
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_loopback_set(int unit, bcm_port_t port, int loopback)
{
    int         rv = BCM_E_NONE;
    pbmp_t      t_pbm;
    int     link, i=0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_loopback_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    /* 
     * Always force link before changing hardware to avoid 
     * race with the linkscan thread.
     */
    if (!(loopback == BCM_PORT_LOOPBACK_NONE)) {
        rv = _bcm_robo_link_force(unit, port, TRUE, FALSE);
    }

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);

    if (BCM_SUCCESS(rv)) {
        /* Set MAC loopback status */
        rv = ((DRV_SERVICES(unit)->port_set)
                (unit, t_pbm,
                DRV_PORT_PROP_MAC_LOOPBACK, 
                (loopback == BCM_PORT_LOOPBACK_MAC)));
        soc_cm_debug(DK_PORT, 
                "bcm_robo_port_loopback_set(%d):set MAC loopback, rv=%d\n", 
                (loopback == BCM_PORT_LOOPBACK_MAC),
                rv);
    }
    if (BCM_SUCCESS(rv)) {
        /* Set PHY loopback status */
        if (loopback == BCM_PORT_LOOPBACK_NONE){
            if ((rv = (DRV_SERVICES(unit)->port_set)
                (unit, t_pbm, 
                DRV_PORT_PROP_MAC_ENABLE, FALSE)) < 0) {
                return rv;
            }
        }
        rv = ((DRV_SERVICES(unit)->port_set)
                (unit, t_pbm,
                DRV_PORT_PROP_PHY_LOOPBACK, 
                (loopback == BCM_PORT_LOOPBACK_PHY)));
        soc_cm_debug(DK_PORT, 
                "bcm_robo_port_loopback_set(%d):set PHY loopback, rv=%d\n", 
                (loopback == BCM_PORT_LOOPBACK_PHY),
                rv);
        if (loopback == BCM_PORT_LOOPBACK_NONE){            
            if ((rv = (DRV_SERVICES(unit)->port_set)                
                (unit, t_pbm,                 
                DRV_PORT_PROP_MAC_ENABLE, TRUE)) < 0) {
                return rv;            
            }        
        }
    }

    if ((loopback == BCM_PORT_LOOPBACK_NONE) || !BCM_SUCCESS(rv)) {
        _bcm_robo_link_force(unit, port, FALSE, DONT_CARE);
    }else {                                              
        while (1){
            soc_phyctrl_link_get(unit, port, &link);
            if (link){
                break;
            }        
            i++;
            if (i == 100000000) {
                SOC_DEBUG_PRINT((DK_WARN,"Phy loopback fail! \n"));
                break;
            }
        }
        _bcm_robo_link_force(unit, port, TRUE, TRUE);        
    }

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_loopback_set: u=%d p=%d lb=%d rv=%d\n",
             unit, port, loopback, rv));

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_loopback_get
 * Purpose:
 *  Recover the current loopback operation for the specified port.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  loopback - (OUT) one of:
 *      BCM_PORT_LOOPBACK_NONE
 *      BCM_PORT_LOOPBACK_MAC
 *      BCM_PORT_LOOPBACK_PHY
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_loopback_get(int unit, bcm_port_t port, int *loopback)
{
    int     rv;
    int     phy_lb, mac_lb;
    uint32  drv_act_value;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_loopback_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    /* Get PHY loopback status */
    drv_act_value = 0;
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_PHY_LOOPBACK, &drv_act_value));
    phy_lb = drv_act_value;

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_loopback_get: phy_lb=%x rv=%d\n",
             drv_act_value, rv));
    if (rv >= 0) {
        /* Get MAC loopback status */
        drv_act_value = 0;
        rv = ((DRV_SERVICES(unit)->port_get)
                (unit, port, 
                DRV_PORT_PROP_MAC_LOOPBACK, &drv_act_value));
        mac_lb = drv_act_value;
        SOC_DEBUG_PRINT((DK_PORT,
                 "bcm_port_loopback_get: mac_lb=%x rv=%d\n",
                 drv_act_value, rv));
    }

    if (rv >= 0) {
        if (mac_lb) {
            *loopback = BCM_PORT_LOOPBACK_MAC;
        } else if (phy_lb) {
            *loopback = BCM_PORT_LOOPBACK_PHY;
        } else {
            *loopback = BCM_PORT_LOOPBACK_NONE;
        }
    }

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_loopback_get: u=%d p=%d lb=%d rv=%d\n",
             unit, port, *loopback, rv));

    return rv;
}
    

/*
 * Function:
 *      bcm_robo_port_stp_set
 * Purpose:
 *      Set the spanning tree state for a port.
 *  All STGs containing all VLANs containing the port are updated.
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      port - RoboSwitch port number.
 *      stp_state - State to place port in, one of BCM_STG_STP_xxx.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 * Notes:
 *  BCM_LOCK is taken so that the current list of VLANs
 *  can't change during the operation.
 */

int
bcm_robo_port_stp_set(int unit, bcm_port_t port, int stp_state)
{
    bcm_vlan_data_t *list;
    int         count, i;
    int         rv;
    pbmp_t      pbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_stp_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    
    rv = bcm_vlan_list_by_pbmp(unit, pbm, &list, &count);

    if (BCM_SUCCESS(rv)) 
    {
        for (i = 0; i < count; i++) 
        {
            if ((rv = bcm_vlan_stp_set(
                            unit,list[i].vlan_tag,port, stp_state)) < 0) 
            {
                break;
            }
        }
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_list_destroy
                    (unit, list, count));


    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_stp_set: u=%d p=%d state=%d rv=%d\n",
             unit, port, stp_state, rv));

    return rv;
}



/*
 * Function:
 *  bcm_robo_port_stp_get
 * Purpose:
 *  Get the spanning tree state for a port in the default STG.
 * Parameters:
 *  unit - RoboSwitch unit number.
 *  port - RoboSwitch port number.
 *  stp_state - Pointer where state stored.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_INTERNAL
 */

int
bcm_robo_port_stp_get(int unit, bcm_port_t port, int *stp_state)
{
    int stg_defl, rv;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_stp_get()..\n");
    rv = bcm_stg_default_get(unit, &stg_defl);
    if (BCM_SUCCESS(rv))  {
        rv = bcm_stg_stp_get(unit, stg_defl, port, stp_state);
    }
    
    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_stp_get: u=%d p=%d state=%d rv=%d\n",
             unit, port, *stp_state, rv));    
    return rv;         
}

/*
 * Modes for discard
 */


/*
 * Function:
 *  bcm_robo_port_discard_get
 * Purpose:
 *  Get port discard attributes for the specified port
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      mode - (OUT) Port discard mode, one of BCM_PORT_DISCARD_xxx
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_discard_get(int unit, bcm_port_t port, int *mode)
{
    bcm_port_cfg_t * pcfg;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_discard_get()..\n");
    pcfg = &robo_port_config[unit][port];
    *mode = pcfg->pc_disc;
        
    return BCM_E_NONE;   
}

/*
 * Function:
 *  bcm_robo_port_discard_set
 * Purpose:
 *  Set port discard attributes for the specified port.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  mode - Port discard mode, one of BCM_PORT_DISCARD_xxx
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_discard_set(int unit, bcm_port_t port, int mode)
{
    bcm_port_cfg_t  *pcfg;
    int             ori_mode;
    pbmp_t          t_pbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_discard_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    bcm_robo_port_discard_get(unit, port, &ori_mode);
    if (!(ori_mode ^ mode)){
        /* 
         * Since 5395/5397/5398/53115/53118 support phy power mode,
         * the cable checker will perform mac_init.
         * It need to re-set the discard value again.
         */
        if (!((SOC_IS_ROBO5398(unit)) ||(SOC_IS_ROBO5397(unit)) ||
            (SOC_IS_ROBO5395(unit)) || (SOC_IS_ROBO53115(unit)) ||
            (SOC_IS_ROBO53118(unit))))
        return  BCM_E_NONE;
    }
    
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    switch (mode){
        case BCM_PORT_DISCARD_NONE:
            /* enable Rx */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_ENABLE_RX, TRUE));
            /* enable Untag */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_ENABLE_DROP_NON1Q, FALSE));
            break;
        
        case BCM_PORT_DISCARD_ALL:
            /* disable Rx */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_ENABLE_RX, FALSE));
            /* disable Untag */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_ENABLE_DROP_NON1Q, TRUE));
            break;
        
        case BCM_PORT_DISCARD_UNTAG:
            /* enable Rx */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_ENABLE_RX, TRUE));
            /* disable Untag */
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_ENABLE_DROP_NON1Q, TRUE));
            break;
        case BCM_PORT_DISCARD_TAG:
            return BCM_E_UNAVAIL;
            break;
        
        default:
            return BCM_E_PARAM;
    }
                           
    pcfg = &robo_port_config[unit][port];
    pcfg->pc_disc = mode;
        
    return BCM_E_NONE;   
}

/*
 * Flags for learn mode
 *
 * This call takes flags to turn on and off mutually-independent actions
 * that should be taken when a packet is received with an unknown source
 * address, or source lookup failure (SLF).
 *
 * The set call returns BCM_E_UNAVAIL for flag combinations that are
 * not supported by the hardware.
 */

/*
 * Function:
 *      _bcm_port_learn_modify
 * Purpose:
 *      Main part of bcm_port_learn_modify.
 */

STATIC int
_bcm_robo_port_learn_modify(int unit, bcm_port_t port, 
                           uint32 add, uint32 remove)
{
    uint32      flags;

    SOC_IF_ERROR_RETURN(bcm_port_learn_get(unit, port, &flags));

    flags |= add;
    flags &= ~remove;

    SOC_IF_ERROR_RETURN(bcm_port_learn_set(unit, port, flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_learn_modify
 * Purpose:
 *  Modify the port learn flags, adding add and removing remove flags.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  add  - Flags to set.
 *      remove - Flags to clear.
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */
int
bcm_robo_port_learn_modify(int unit, 
                bcm_port_t port, 
                uint32 add, 
                uint32 remove)
{
    int         rv;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = _bcm_robo_port_learn_modify(unit, port, add, remove);

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_learn_get
 * Purpose:
 *  Get the ARL hardware learning options for this port.
 *  This defines what the hardware will do when a packet
 *  is seen with an unknown address.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  flags - (OUT) Logical OR of BCM_PORT_LEARN_xxx flags
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */
int
bcm_robo_port_learn_get(int unit, bcm_port_t port, uint32 *flags)
{
    bcm_port_cfg_t      *pcfg;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    pcfg = &robo_port_config[unit][port];

    switch (pcfg->pc_cml) {
    case PVP_CML_SWITCH:
        *flags = (BCM_PORT_LEARN_ARL |
                  BCM_PORT_LEARN_FWD |
                  (pcfg->pc_cpu ? BCM_PORT_LEARN_CPU : 0));
        break;
    case PVP_CML_CPU:
        *flags = BCM_PORT_LEARN_CPU;
        break;
    case PVP_CML_FORWARD:
        *flags = BCM_PORT_LEARN_FWD;
        break;
    case PVP_CML_DROP:
        *flags = 0;
        break;
    case PVP_CML_CPU_SWITCH:            /* Possible on Draco only */
        *flags = (BCM_PORT_LEARN_ARL |
                  BCM_PORT_LEARN_CPU |
                  BCM_PORT_LEARN_FWD);
        break;
    case PVP_CML_CPU_FORWARD:           /* Possible on Draco only */
        *flags = BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_learn_set
 * Purpose:
 *  Set the ARL hardware learning options for this port.
 *  This defines what the hardware will do when a packet
 *  is seen with an unknown address.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  flags - Logical OR of BCM_PORT_LEARN_xxx flags
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */
#define Arl     BCM_PORT_LEARN_ARL
#define Cpu     BCM_PORT_LEARN_CPU
#define Fwd     BCM_PORT_LEARN_FWD

int
bcm_robo_port_learn_set(int unit, bcm_port_t port, uint32 flags)
{
    bcm_port_cfg_t  *pcfg;
    int             rv = BCM_E_NONE;
    bcm_pbmp_t      pbmp;
    int             cml, drv_cml = -1;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    pcfg = &robo_port_config[unit][port];

    pcfg->pc_cpu = ((flags & BCM_PORT_LEARN_CPU) != 0);

    /* Use shortened names to handle each flag combination individually */

    switch (flags) {
    case ((!Arl) | (!Cpu) | (!Fwd)):
        cml = PVP_CML_DROP;
        drv_cml = DRV_PORT_DROP;
        break;
    case ((!Arl) | (!Cpu) | ( Fwd)):
        cml = PVP_CML_FORWARD;
        drv_cml = DRV_PORT_DISABLE_LEARN;
        break;
    case ((!Arl) | ( Cpu) | (!Fwd)):
        cml = PVP_CML_CPU;
        drv_cml = DRV_PORT_SWLRN_DROP;
        break;
    case ((!Arl) | ( Cpu) | ( Fwd)):
        cml = PVP_CML_CPU_FORWARD;
        drv_cml = DRV_PORT_SW_LEARN;
        break;
    case (( Arl) | (!Cpu) | (!Fwd)):
        return BCM_E_UNAVAIL;
        break;
    case (( Arl) | (!Cpu) | ( Fwd)):
        cml = PVP_CML_SWITCH;
        drv_cml = DRV_PORT_HW_LEARN;
        break;
    case (( Arl) | ( Cpu) | (!Fwd)):
        return BCM_E_UNAVAIL;
        break;
    case (( Arl) | ( Cpu) | ( Fwd)):
        cml = PVP_CML_CPU_SWITCH;
        drv_cml = DRV_PORT_HW_SW_LEARN;
        break;
    default :
        return BCM_E_UNAVAIL;
    }

    BCM_PBMP_CLEAR(pbmp); 
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = (DRV_SERVICES(unit)->arl_learn_enable_set)(unit, pbmp, drv_cml);
    
    if (BCM_SUCCESS(rv)){
        pcfg->pc_cml = cml;
    }

    return rv;
}

#undef Arl
#undef Cpu
#undef Fwd
                 

/*
 * Function:
 *  bcm_robo_port_link_status_get
 * Purpose:
 *  Return current Link up/down status, queries linkscan, if unable to
 *  retrieve status queries the PHY.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *  up - (OUT) Boolean value, FALSE for link down and TRUE for link up
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 */

int
bcm_robo_port_link_status_get(int unit, bcm_port_t port, int *up)
{
    int     rv;
    uint32  drv_value = 0;
    int loopback= 0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_link_status_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(bcm_port_loopback_get(unit, port, &loopback));

    if (loopback == BCM_PORT_LOOPBACK_PHY) {
        rv = ((DRV_SERVICES(unit)->port_status_get)
                (unit, port, 
                DRV_PORT_STATUS_LINK_UP, &drv_value));
        *up = (drv_value) ? TRUE : FALSE;
    } else {
        rv = _bcm_robo_link_get(unit, port, up);
        if (rv == BCM_E_DISABLED) {
            rv = _bcm_robo_port_link_get(unit, port, 0, up);
        }
    }

    SOC_DEBUG_PRINT((DK_PORT,
                     "bcm_port_linkstatus_get: u=%d p=%d up=%d rv=%d\n",
                     unit, port, *up, rv));

    return rv;
}
                
    

/*
 * Modes for VLAN input filtering
 * (see also VLAN-based ifilter setting in vlan.h)
 */
/*
 * Function:
 *  bcm_robo_port_ifilter_get
 * Description:
 *  Return input filter mode for a port.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *  port - Port number to operate on
 *  mode - (OUT) Filter mode, one of BCM_PORT_IFILTER_xxx
 * Returns:
 *  BCM_E_NONE      Success.
 *  BCM_E_INTERNAL      Chip access failure.
 */
int
bcm_robo_port_ifilter_get(int unit, bcm_port_t port, int *mode)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *  bcm_robo_port_ifilter_set
 * Description:
 *  Set input filter mode for a port.
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *  port - Port number to operate on
 *  mode - BCM_PORT_IFILTER_ON or BCM_PORT_IFILTER_OFF.
 * Returns:
 *  BCM_E_NONE      Success.
 *  BCM_E_INTERNAL      Chip access failure.
 * Notes:
 *  When input filtering is turned on for a port, packets received
 *  on the port that do not match the port's VLAN classifications
 *  are discarded.
 */
int
bcm_robo_port_ifilter_set(int unit, bcm_port_t port, int mode)
{
    return (BCM_E_UNAVAIL);
}


/*
 * Function:
 *  bcm_robo_port_bpdu_enable_set
 * Purpose:
 *  Enable/Disable BPDU reception on the specified port.
 * Parameters:
 *  unit - SOC unit #
 *  port - Port number (0 based)
 *  enable - TRUE to enable, FALSE to disable (reject bpdu).
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_INTERNAL
 */

int
bcm_robo_port_bpdu_enable_set(int unit, bcm_port_t port, int enable)
{
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_bpdu_enable_set()..\n");
    /* Robo Chips could not per-port set */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *  bcm_robo_port_bpdu_enable_get
 * Purpose:
 *  Return whether BPDU reception is enabled on the specified port.
 * Parameters:
 *  unit - SOC unit #
 *  port - Port number (0 based)
 *  enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_INTERNAL
 */

int
bcm_robo_port_bpdu_enable_get(int unit, bcm_port_t port, int *enable)
{
    int         rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_bpdu_enable_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_BPDU_RX, (uint32 *) enable));

    return rv;
}
    
/*
 * Function:
 *  bcm_port_l3_enable_get
 * Purpose:
 *  Return whether L3 switching is enabled on the specified port.
 * Parameters:
 *  unit -      device number
 *  port -      port number
 *  enable -    (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_port_l3_enable_get(
    int unit,
    bcm_port_t  port,
    int *   enable)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *  bcm_port_l3_enable_set
 * Purpose:
 *  Enable/Disable L3 switching on the specified port.
 * Parameters:
 *  unit -      device number
 *  port -      port number
 *  enable -    TRUE to enable, FALSE to disable.
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_port_l3_enable_set(
    int unit,
    bcm_port_t  port,
    int enable)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_port_tgid_get
 * Purpose:
 *       Get the trunk group for a given port.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number (0 based)
 *      tid - trunk ID
 *      psc - trunk selection criterion
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */
int
bcm_robo_port_tgid_get(
    int unit,
    bcm_port_t  port,
    int *   tgid,
    int *   psc)
{
    int                 rv;
    pbmp_t              t_pbmp, trunk_pbmp;
    int                 i;
    uint32              hash_op;
    bcm_trunk_chip_info_t   ti;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    soc_cm_debug(DK_PORT, 
                "BCM API : bcm_robo_port_tgid_get()..\n");
    BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(unit, &ti));
    for (i = ti.trunk_id_min ; i <= ti.trunk_id_max ; i++){
        rv = ((DRV_SERVICES(unit)->trunk_get)
                (unit, i, &trunk_pbmp, DRV_TRUNK_FLAG_BITMAP, &hash_op));
        
        soc_cm_debug(DK_PORT, 
                "\nbcm_robo_port_tgid_get():tgid=%d,pbmp=%x\n", \
                i,SOC_PBMP_WORD_GET(trunk_pbmp, 0));
        BCM_PBMP_CLEAR(t_pbmp);
        SOC_PBMP_ASSIGN(t_pbmp, trunk_pbmp);
        if (PBMP_MEMBER(t_pbmp, port)) {
            soc_cm_debug(DK_PORT, 
                "bcm_robo_port_tgid_get():port in tgid=%d\n",i);
            break;
        }
    }

    if (i <= ti.trunk_id_max){
        
        *tgid = i;
        BCM_IF_ERROR_RETURN(bcm_trunk_psc_get(unit, i, psc));
    } else {
        *tgid = BCM_TRUNK_INVALID;
        *psc = 0;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_tgid_set
 * Purpose:
 *       Set the trunk group for a given port.
 * Parameters:
 *      unit - SOC unit #
 *      port - Port number (0 based)
 *      tid - trunk ID
 *      psc - trunk selection criterion
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */
int
bcm_robo_port_tgid_set(
    int unit,
    bcm_port_t  port,
    int tgid,
    int psc)
{
    return BCM_E_UNAVAIL;

}
    
/*
 * Port Filter Modes
 */
#define MLF_FORWARD_FLOOD 0x0
#define MLF_FORWARD_BY_MLF_MAP 0x1


/*
 * Function:
 *      _bcm_robo_port_pfm_set
 * Purpose:
 *      Main part of function bcm_port_pfm_set
 */

STATIC int
_bcm_robo_port_pfm_set(int unit, bcm_port_t port, int mode)
{
    int reg_len, rv = BCM_E_NONE;
    int cur_mode = 0;
    uint32 reg_addr, reg_value = 0, fld_value = 0;
    uint64 reg_val64;
    int reg_id, field_id;

    rv = bcm_port_pfm_get(unit, port, &cur_mode);
    if (rv < 0) {
        return rv;
    }

    if (cur_mode == mode) {
        return BCM_E_NONE;
    }

    if (port != -1) {
        return BCM_E_UNAVAIL;
    }
    
    switch (mode) {
        case BCM_PORT_MCAST_FLOOD_NONE:
            /* Clear MLF packet forward map */
            reg_len = (DRV_SERVICES(unit)->reg_length_get)
                ( unit, MLF_DROP_MAPr );
            reg_addr = (DRV_SERVICES(unit)->reg_addr)
                ( unit, MLF_DROP_MAPr, 0, 0 );
            if (NUM_ALL_PORT(unit) > 32) {
                COMPILER_64_ZERO(reg_val64);
                if (( rv = (DRV_SERVICES(unit)->reg_write)
                    ( unit, reg_addr, &reg_val64, reg_len ) ) < 0)  {
                    return rv;
                }
            } else {
                reg_value = 0;
                if (( rv = (DRV_SERVICES(unit)->reg_write)
                    ( unit, reg_addr, &reg_value, reg_len ) ) < 0)  {
                    return rv;
                }
            }

            if (SOC_IS_ROBO_GE_SWITCH(unit)) {
                if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){
                    reg_id = NEW_CTRLr;
                    field_id = MC_FWD_ENf;
                } else {
                    reg_id = NEW_CONTROLr;
                    field_id = MC_DLF_FWDf;
                }
            } else {
                reg_id = NEW_CONTROLr;
                field_id = MLF_FM_ENf;
            }
            /* MLF Forward Configure register */
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, reg_id);
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, reg_id, 0, 0);
            
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                            (unit, reg_addr, &reg_value, reg_len)) < 0)  {
                return rv;
            }
            fld_value = MLF_FORWARD_BY_MLF_MAP;
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, reg_id, &reg_value, field_id, &fld_value));

            if ((rv = (DRV_SERVICES(unit)->reg_write)
                            (unit, reg_addr, &reg_value, reg_len)) < 0)  {
                return rv;
            }
            break;
        case BCM_PORT_MCAST_FLOOD_UNKNOWN:
            
            if (SOC_IS_ROBO_GE_SWITCH(unit)) {
                if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){
                    reg_id = NEW_CTRLr;
                    field_id = MC_FWD_ENf;
                } else {
                    reg_id = NEW_CONTROLr;
                    field_id = MC_DLF_FWDf;
                }
            } else {
                reg_id = NEW_CONTROLr;
                field_id = MLF_FM_ENf;
            }
            /* MLF Forward Configure register */
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, reg_id);
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, reg_id, 0, 0);
            
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                            (unit, reg_addr, &reg_value, reg_len)) < 0)  {
                return rv;
            }
            fld_value = MLF_FORWARD_FLOOD;
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, reg_id, &reg_value, field_id, &fld_value));

            if ((rv = (DRV_SERVICES(unit)->reg_write)
                            (unit, reg_addr, &reg_value, reg_len)) < 0)  {
                return rv;
            }
            break;
        default : 
            rv = BCM_E_UNAVAIL;
    }
    return rv;
}

/*
 * Function:
 *  bcm_robo_port_pfm_set
 * Purpose:
 *  Set current port filtering mode (see port.h)
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #, -1 apply to all ports.
 *      mode - mode for PFM bits (see port.h)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  The is system-base configure for Robo chips.
 *    Need to asssign port number as -1.
 */
int
bcm_robo_port_pfm_set(int unit, bcm_port_t port, int mode)
{
    int                 rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_pfm_set()..\n");

    if (port != -1) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));
    }

    rv = _bcm_robo_port_pfm_set(unit, port, mode);

    return rv;
}

/*
 * Function:
 *  bcm_robo_port_pfm_get
 * Purpose:
 *  Return current port filtering mode (see port.h)
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      mode - (OUT) mode read from RoboSwitch for PFM bits (see port.h)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  The is system-base configure for Robo chips.
 */
int
bcm_robo_port_pfm_get(int unit, bcm_port_t port, int *mode)
{
    int reg_len, rv = BCM_E_NONE;
    uint32 reg_addr, reg_value, temp;
    uint64   reg_val64, fld_val64;
    int reg_id, field_id;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_pfm_get()..\n");

    if (port != -1) {
        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));
    }
    
    if (SOC_IS_ROBO_GE_SWITCH(unit)) {
        if (SOC_IS_ROBO53115(unit) || SOC_IS_ROBO53118(unit)){
            reg_id = NEW_CTRLr;
            field_id = MC_FWD_ENf;
        } else {
            reg_id = NEW_CONTROLr;
            field_id = MC_DLF_FWDf;
        }
    } else {
        reg_id = NEW_CONTROLr;
        field_id = MLF_FM_ENf;
    }
    
    /* Get MLF configure value */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, reg_id);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, reg_id, 0, 0);
    
    if ((rv = (DRV_SERVICES(unit)->reg_read)
                    (unit, reg_addr, &reg_value, reg_len)) < 0)  {
        return rv;
    }
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
    (unit, reg_id, &reg_value, field_id, &temp));
    
    if (temp == MLF_FORWARD_FLOOD) {
        *mode = BCM_PORT_MCAST_FLOOD_UNKNOWN;
    } else if (temp == MLF_FORWARD_BY_MLF_MAP) {
    
        /* Check Mcast Forwarding Map */
        reg_len = (DRV_SERVICES(unit)->reg_length_get)
                ( unit, MLF_DROP_MAPr );
        reg_addr = (DRV_SERVICES(unit)->reg_addr)
            ( unit, MLF_DROP_MAPr, 0, 0 );
        
        if (NUM_ALL_PORT(unit) > 32) {
            
            COMPILER_64_ZERO(reg_val64);
            if (( rv = (DRV_SERVICES(unit)->reg_read)
                ( unit, reg_addr, &reg_val64, reg_len ) ) < 0)  {
                return rv;
            }
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                (unit, MLF_DROP_MAPr, (uint32 *)&reg_val64, 
                MLF_FWD_MAPf, (uint32 *)&fld_val64));
            if (COMPILER_64_IS_ZERO(fld_val64)) {
                *mode = BCM_PORT_MCAST_FLOOD_NONE;
            } else {
                *mode = BCM_PORT_MCAST_FLOOD_UNKNOWN;
            }
        } else {
        
            reg_value = 0;
            if (( rv = (DRV_SERVICES(unit)->reg_read)
                ( unit, reg_addr, &reg_value, reg_len ) ) < 0)  {
                return rv;
            }
            if (SOC_IS_ROBO_GE_SWITCH(unit)) {
                BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, MLF_DROP_MAPr, &reg_value, 
                    MUL_LOOKUP_FAIL_FRW_MAPf, &temp));
            } else {
                BCM_IF_ERROR_RETURN(
                    (DRV_SERVICES(unit)->reg_field_get)
                    (unit, MLF_DROP_MAPr, &reg_value, 
                    MLF_FWD_MAPf, &temp));
            }
            if (temp == 0) {
                *mode = BCM_PORT_MCAST_FLOOD_NONE;
            } else {
                *mode = BCM_PORT_MCAST_FLOOD_UNKNOWN;
            }
        }
        *mode = BCM_PORT_MCAST_FLOOD_NONE;
    } else {
        rv = BCM_E_INTERNAL;
    }
    return rv;
}
    

/*
 * Port Encapsulation modes (get/set)
 * Members of the XGS family of switches support multiple frame (link-level)
 * encapsulation modes (HIGIG, BCM5632, Ethernet).
 */

/*
 * Function:
 *  bcm_robo_port_encap_set
 * Purpose:
 *  Set the port encapsulation mode
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *      mode - One of BCM_PORT_ENCAP_xxx (see port.h)
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_port_encap_set(int unit, bcm_port_t port, int mode)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *  bcm_robo_port_encap_get
 * Purpose:
 *  Get the port encapsulation mode
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *      mode (OUT) - One of BCM_PORT_ENCAP_xxx (see port.h)
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_port_encap_get(int unit, bcm_port_t port, int *mode)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *  bcm_robo_port_queued_count_get
 * Purpose:
 *  Returns the count of packets (or cells) currently buffered
 *  for a port.  Useful to know when a port has drained all
 *  data and can then be re-configured.
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *  count (OUT) - count of packets currently buffered
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *    "packets" may actually be cells on most chips,
 *  If no packets are buffered, the cell count is 0,
 *  If some packets are buffered the cell count will be
 *  greater than or equal to the packet count.
 */
int
bcm_robo_port_queued_count_get(int unit, bcm_port_t port, uint32 *count)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *  _bcm_robo_vlan_port_protocol_action_add
 * Description:
 *  Add the action on an created protocol valn.
 * Parameters:
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (IN) Action for outer tag and inner tag
 * Returns:
 *      BCM_E_xxxx
 * Note:
 */
int 
_bcm_robo_vlan_port_protocol_action_add(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    protocol2vlan_entry_t  protcolment;
    uint32  temp;
    bcm_vlan_t  vid;
    bcm_cos_t   priority;
    
    /* only bcm53242/53262 provide HW protocol valn supported only */
    if (!(SOC_IS_ROBO53242(unit) || SOC_IS_ROBO53262(unit))){
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    /* set valid field */
    temp = 1;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_PROTOCOLVLAN, DRV_MEM_FIELD_VALID,
                (uint32 *)&protcolment, (uint32 *)&temp));

    /* set ether_type field */
    temp = ether;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_PROTOCOLVLAN, DRV_MEM_FIELD_ETHER_TYPE,
                (uint32 *)&protcolment, (uint32 *)&temp));

    /* retreive the vid and pri from action 
     *  1. vid : get new_outer_vlan from action.
     *      - ignore inner vid setting in action 
     *  2. pri : get priority from action.
     */
    vid = action->new_outer_vlan;
    priority = action->priority;
    
    /* set vid field */
    temp = vid;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_PROTOCOLVLAN, DRV_MEM_FIELD_VLANID,
                (uint32 *)&protcolment, (uint32 *)&temp));
    
    /* set pri field */
    temp = priority;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
                (unit, DRV_MEM_PROTOCOLVLAN, DRV_MEM_FIELD_PRIORITY,
                (uint32 *)&protcolment, (uint32 *)&temp));
                
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_insert)
                (unit, DRV_MEM_PROTOCOLVLAN, (uint32 *)&protcolment, 
                DRV_MEM_OP_REPLACE));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_protocol_vlan_add
 * Purpose:
 *  Adds a protocol based vlan to a port.  The protocol
 *  is matched by frame type and ether type.  Returns an
 *  error if hardware does not support protocol vlans
 *    (Strata and Hercules).
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *  frame - one of BCM_PORT_FRAMETYPE_{ETHER2,8023,LLC}
 *  ether - 16 bit Ethernet type field
 *  vid - vlan id
 * Returns:
 *  BCM_E_XXX
 * Note:
 *    This API in robobcm53242 won't process the port and frame 
 *      parameters.
 */
int
bcm_robo_port_protocol_vlan_add(int unit,
               bcm_port_t port,
               bcm_port_frametype_t frame,
               bcm_port_ethertype_t ether,
               bcm_vlan_t vid)
{
    bcm_vlan_action_set_t action;
    
    bcm_vlan_action_set_t_init(&action);
    
    /* action value assignment : */
    action.new_outer_vlan = vid;
    action.new_inner_vlan = vid;
    action.priority = 0;    /* default priority */
    
    /* other action items are not supported in ROBO device yet */
    
    return _bcm_robo_vlan_port_protocol_action_add(unit, port, frame,
                                        ether, &action);
    
}

/*
 * Function:
 *  bcm_robo_port_protocol_vlan_delete
 * Purpose:
 *  Remove an already created proto protocol based vlan
 *  on a port.
 * Parameters:
 *      unit      (IN) BCM unit number
 *      port      (IN) Port number
 *      frame     (IN) Frame type
 *      ether     (IN) 16 bit ether type
 *      action    (IN) Action for outer tag and inner tag
 * Returns:
 *  BCM_E_XXX
 * Note:
 *    This API in robobcm53242 won't process the port, frame 
 *      parameters.
 */
int
bcm_robo_port_protocol_vlan_delete(int unit,
                  bcm_port_t port,
                  bcm_port_frametype_t frame,
                  bcm_port_ethertype_t ether)
{
    int rv = BCM_E_NONE;
    protocol2vlan_entry_t  protcolment;
    uint32  temp;

    soc_cm_debug(DK_VLAN,
            "BCM API : %s()\n",__func__);

    PORT_PARAM_CHECK(unit, port);

    sal_memset(&protcolment, 0, sizeof(protcolment));

    /* set ether_type field */
    temp = ether;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->mem_field_set)
        (unit, DRV_MEM_PROTOCOLVLAN, DRV_MEM_FIELD_ETHER_TYPE,
        (uint32 *)&protcolment, (uint32 *)&temp));

    rv = (DRV_SERVICES(unit)->mem_delete)
        (unit, DRV_MEM_PROTOCOLVLAN, (uint32 *)&protcolment, 0);
    return rv;
}

/*
 * Function:
 *  bcm_robo_port_protocol_vlan_delete_all
 * Purpose:
 *  Remove all protocol based vlans on a port.
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_robo_port_protocol_vlan_delete_all(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;

    soc_cm_debug(DK_VLAN,
            "BCM API : %s()\n",__func__);

    PORT_PARAM_CHECK(unit, port);
    
    rv = (DRV_SERVICES(unit)->mem_clear)
        (unit, DRV_MEM_PROTOCOLVLAN);

    return rv;
}

/*
 * Function:
 *  bcm_port_egress_set
 * Description:
 *  Set switching only to indicated ports from given (modid, port).
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *  pbmp - bitmap of ports to allow egress.
 * Returns:
 *      BCM_E_xxxx
 * Note:
 *      if port < 0, means all/any port
 *      if modid < 0, means all/any modid
 */
int bcm_robo_port_egress_set(int unit, bcm_port_t port, int modid,
                        bcm_pbmp_t pbmp)
{
    bcm_pbmp_t t_pbm, i_pbm;
    bcm_port_t p;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_egress_set()..\n");

    if ((modid != -1) && (modid != 0)) {
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(i_pbm);
    /* if port = -1, the configuration is applied to all source ports on the specified module */
    if (port == -1) {
        BCM_PBMP_ASSIGN(i_pbm, PBMP_ALL(unit));
    } else {
        PORT_PARAM_CHECK(unit, port);
        BCM_PBMP_PORT_SET(i_pbm, port);
    }

    BCM_PBMP_ITER(i_pbm, p) {
        BCM_PBMP_CLEAR(t_pbm);
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_get)
                            (unit, p, &t_pbm));

        BCM_PBMP_AND(pbmp, PBMP_ALL(unit));
        if (!BCM_PBMP_EQ(pbmp, t_pbm)) {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_set)
                        (unit, p, pbmp));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_port_egress_get
 * Description:
 *  Retrieve bitmap of ports for which switching is enabled
 *      for (modid, port).
 * Parameters:
 *  unit - RoboSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *  pbmp - (OUT) bitmap of ports where egress allowed.
 * Returns:
 *      BCM_E_xxxx
 */
int bcm_robo_port_egress_get(int unit, bcm_port_t port, int modid,
                        bcm_pbmp_t *pbmp)
{
    bcm_pbmp_t t_pbm;

    PORT_PARAM_CHECK(unit, port); 
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_egress_get()..\n");
        
    if ((modid != -1) && (modid != 0)) {
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(t_pbm);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_get)
                        (unit, port, &t_pbm));

    BCM_PBMP_AND(t_pbm, PBMP_ALL(unit));
    BCM_PBMP_ASSIGN(*pbmp, t_pbm);

    return BCM_E_NONE;
}
                               

/* Source module control */

/*
 * Function:
 *      bcm_port_modid_egress_get
 * Description:
 *      Retrieve port bitmap of egress ports on which the incoming packets
 *      will be forwarded.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *      pbmp - (OUT) bitmap of ports where egress allowed.
 * Returns:
 *      BCM_E_xxxx
 * Note:
 *      This API is designed for Hercules15. Not suitable for Robo.
 */
int 
bcm_robo_port_modid_egress_get(int unit, bcm_port_t port,
                              bcm_module_t modid, bcm_pbmp_t *pbmp)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_modid_egress_set
 * Description:
 *      Set port bitmap of egress ports on which the incoming packets
 *      will be forwarded.
 * Parameters:
 *      unit - RoboSwitch PCI device unit number (driver internal).
 *      port  - ingress port number.
 *      modid - source module ID index (in HiGig mod header).
 *      pbmp - bitmap of ports to allow egress.
 * Returns:
 *      BCM_E_xxxx
 * Note:
 *      if port < 0, means all/any port
 *      if modid < 0, means all/any modid
 * Note:
 *      This API is designed for Hercules15. Not suitable for Robo.
 */
int 
bcm_robo_port_modid_egress_set(int unit, bcm_port_t port,
                              bcm_module_t modid, bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *    bcm_robo_port_modid_enable_set
 * Purpose:
 *    Enable/block packets from a specific module on a port.
 * Parameters:
 *    unit - RoboSwitch PCI device unit number (driver internal).
 *    port - RoboSwitch port number.
 *    modid - Which source module id to enable/disable
 *    enable - TRUE/FALSE Enable/disable forwarding packets from
 *             source module arriving on port.
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_robo_port_modid_enable_set(int unit, 
                bcm_port_t port, 
                int modid, 
                int enable)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *    bcm_robo_port_modid_enable_get
 * Purpose:
 *    Return enable/block state for a specific module on a port.
 * Parameters:
 *    unit - RoboSwitch PCI device unit number (driver internal).
 *    port - RoboSwitch port number.
 *    modid - Which source module id to enable/disable
 *    enable - (OUT) TRUE/FALSE Enable/disable forwarding packets from
 *             source module arriving on port.
 * Returns:
 *    BCM_E_XXX
 */
int
bcm_robo_port_modid_enable_get(int unit, 
                bcm_port_t port, 
                int modid, 
                int *enable)
{
    return (BCM_E_UNAVAIL);
}
                                     
/* Flood blocking */
/*
 * Function:
 *  bcm_robo_port_flood_block_set
 * Purpose:
 *  Set selective per-port blocking of flooded VLAN traffic
 * Parameters:
 *  unit        - unit number
 *  ingress_port    - Port traffic is ingressing
 *  egress_port - Port for which the traffic should be blocked.
 *  flags       - Specifies the type of traffic to block
 * Returns:
 *  BCM_E_UNAVAIL   - Functionality not available
 *  BCM_E_NONE
 */
int
bcm_robo_port_flood_block_set(int unit,
             bcm_port_t ingress_port, 
             bcm_port_t egress_port,
             uint32 flags)
{
    pbmp_t t_pbm, opbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_flood_block_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, ingress_port, &ingress_port));
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, egress_port, &egress_port));

    BCM_PBMP_CLEAR(t_pbm);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_get)
                        (unit, ingress_port, &t_pbm));

    BCM_PBMP_ASSIGN(opbm, t_pbm);
    if (flags) {
        BCM_PBMP_PORT_REMOVE(t_pbm, egress_port);
    } else {
        BCM_PBMP_PORT_ADD(t_pbm, egress_port);
    }

    if ((!flags) || (flags & BCM_PORT_FLOOD_BLOCK_ALL)) {
        if (BCM_PBMP_NEQ(t_pbm, opbm)) {
            BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_set)
                    (unit, ingress_port, t_pbm));
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_flood_block_get
 * Purpose:
 *  Get the current per-port flood block settings
 * Parameters:
 *  unit        - unit number
 *  ingress_port    - Port traffic is ingressing
 *  egress_port - Port for which the traffic would be blocked
 *  flags       - (OUT) Returns the current settings
 * Returns:
 *  BCM_E_UNAVAIL   - Functionality not available
 *  BCM_E_NONE
 */
int
bcm_robo_port_flood_block_get(int unit,
             bcm_port_t ingress_port, bcm_port_t egress_port,
             uint32 *flags)
{
    pbmp_t t_pbm;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_flood_block_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, ingress_port, &ingress_port));
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, egress_port, &egress_port));

    BCM_PBMP_CLEAR(t_pbm);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_vlan_get)
                        (unit, ingress_port, &t_pbm));

    *flags = (!BCM_PBMP_MEMBER(t_pbm, egress_port)) ? BCM_PORT_FLOOD_BLOCK_ALL : 0;

    return BCM_E_NONE;
}

/*
 * Ingress and egress rate limiting
 */

/*
 * Function:
 *  bcm_robo_port_rate_egress_set
 * Purpose:
 *  Set egress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - Rate in kilobits (1000 bits) per second.
 *          Rate of 0 disables rate limiting.
 *  kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  1. Robo Switch support 2 ingress buckets for different packet type.
 *     And the bucket1 contains higher priority if PKT_MSK confilict 
 *       with bucket0's PKT_MSK.
 *  2. Robo Switch allowed system basis rate/packet type assignment for 
 *     Rate Control. The RATE_TYPE and PKT_MSK will be set once in the 
 *       initial routine.
 */
int bcm_robo_port_rate_egress_set(int unit,
                 bcm_port_t port,
                 uint32 kbits_sec,
                 uint32 kbits_burst)
{
    pbmp_t      t_pbm;
    bcm_pbmp_t  valid_bmp;
    uint32 value[SOC_PBMP_WORD_MAX];
    int i;
    int queue = 0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_rate_egress_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);

    if (!IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
                (unit, DRV_DEV_PROP_RATE_CONTROL_PBMP, &value[0]));
        for (i=0; i<SOC_PBMP_WORD_MAX; i++){
            SOC_PBMP_WORD_SET(valid_bmp, i, value[i]);
        }

        BCM_PBMP_AND(t_pbm, valid_bmp);
    }
    /* The 3rd parameter in this BCM API is for setting the rate with 
     *    kbit/sec unit. But for the resolution in Robo register setting 
     *    is not the same with this unit(kbit/sec), we might get different 
     *    setting from register when compare with the original user's 
     *    setting value.
     */   
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_set)
                        (unit, t_pbm, queue,
                        DRV_RATE_CONTROL_DIRECTION_EGRESSS,
                        kbits_sec, kbits_burst));

    return BCM_E_NONE;

}

/*
 * Function:
 *  bcm_robo_port_rate_egress_get
 * Purpose:
 *  Get egress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - (OUT) Rate in kilobits (1000 bits) per second, or
 *              zero if rate limiting is disabled.
 *  kbits_burst - (OUT) Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 */
int bcm_robo_port_rate_egress_get(int unit,
                 bcm_port_t port,
                 uint32 *kbits_sec,
                 uint32 *kbits_burst)
{
    int queue = 0;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_rate_egress_get()..\n");

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    if (!IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_robo_rate_valid_pbmp_check(unit, port));
    }

    /* The 3rd parameter in this BCM API is for setting the rate with 
     *    kbit/sec unit. But for the resolution in Robo register setting 
     *    is not the same with this unit(kbit/sec), we might get different 
     *    setting from register when compare with the original user's 
     *    setting value.
     */   
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_get)
                        (unit, port, queue,
                        DRV_RATE_CONTROL_DIRECTION_EGRESSS,
                        kbits_sec, kbits_burst));

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_rate_ingress_set
 * Purpose:
 *  Set ingress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - Rate in kilobits (1000 bits) per second.
 *            Rate of 0 disables rate limiting.
 *  kbits_burst - Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 * Note :
 *  1. Robo Switch support 2 ingress buckets for different packet type.
 *     And the bucket1 contains higher priority if PKT_MSK confilict 
 *       with bucket0's PKT_MSK.
 *  2. Robo Switch allowed system basis rate/packet type assignment for 
 *     Rate Control. The RATE_TYPE and PKT_MSK will be set once in the 
 *       initial routine.
 */
int bcm_robo_port_rate_ingress_set(int unit,
                  bcm_port_t port,
                  uint32 kbits_sec,
                  uint32 kbits_burst)
{
    pbmp_t      t_pbm;
    bcm_pbmp_t  valid_bmp;
    uint32 value[SOC_PBMP_WORD_MAX];
    int i, queue = 0;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_rate_ingress_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);

    if (!IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->dev_prop_get
               (unit, DRV_DEV_PROP_RATE_CONTROL_PBMP, &value[0]));
        for (i=0; i < SOC_PBMP_WORD_MAX; i++){
            SOC_PBMP_WORD_SET(valid_bmp, i, value[i]);
        }
        BCM_PBMP_AND(t_pbm, valid_bmp);
    }

    /* The 3rd parameter in this BCM API is for setting the rate with 
     *    kbit/sec unit. But for the resolution in Robo register setting 
     *    is not the same with this unit(kbit/sec), we might get different 
     *    setting from register when compare with the original user's 
     *    setting value.
     */   
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_set)
                        (unit, t_pbm, queue,
                        DRV_RATE_CONTROL_DIRECTION_INGRESSS,
                        kbits_sec, kbits_burst));

    return BCM_E_NONE;

}

/*
 * Function:
 *  bcm_robo_port_rate_ingress_get
 * Purpose:
 *  Get ingress rate limiting parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_sec - (OUT) Rate in kilobits (1000 bits) per second, or
 *                  zero if rate limiting is disabled.
 *  kbits_burst - (OUT) Maximum burst size in kilobits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 */
int bcm_robo_port_rate_ingress_get(int unit,
                  bcm_port_t port,
                  uint32 *kbits_sec,
                  uint32 *kbits_burst)
{
    int queue = 0;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_rate_ingress_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    if (!IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_robo_rate_valid_pbmp_check(unit, port));
    }

    /* The 3rd parameter in this BCM API is for setting the rate with 
     *    kbit/sec unit. But for the resolution in Robo register setting 
     *    is not the same with this unit(kbit/sec), we might get different 
     *    setting from register when compare with the original user's 
     *    setting value.
     */   
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->rate_get)
                        (unit, port, queue,
                        DRV_RATE_CONTROL_DIRECTION_INGRESSS,
                        kbits_sec, kbits_burst));

    return BCM_E_NONE;
}
                     

/*
 * Function:
 *  bcm_robo_port_rate_pause_set
 * Purpose:
 *  Set ingress rate limiting pause frame control parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_pause - Pause threshold in kbits (1000 bits).
 *      A value of zero disables the pause/resume mechanism.
 *  kbits_resume - Resume threshold in kbits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This interface applies only when port ingress rate limiting
 *  is active.  Currently, only the 566x supports this feature.
 *
 *  If the maximum of bits that could be received before dropping a
 *  frame falls below the kbits_pause, a pause frame is sent.
 *  A resume frame will be sent once it becomes possible to receive
 *  kbits_resume bits of data without dropping.
 */
int bcm_robo_port_rate_pause_set(int unit,
                bcm_port_t port,
                uint32 kbits_pause,
                uint32 kbits_resume)
{
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *  bcm_robo_port_rate_pause_get
 * Purpose:
 *  Get ingress rate limiting pause frame control parameters
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  kbits_pause - (OUT) Pause threshold in kbits (1000 bits).
 *      Zero indicates the pause/resume mechanism is disabled.
 *  kbits_resume - (OUT) Resume threshold in kbits (1000 bits).
 * Returns:
 *  BCM_E_XXX
 */
int bcm_robo_port_rate_pause_get(int unit,
                bcm_port_t port,
                uint32 *kbits_pause,
                uint32 *kbits_resume)
{
    return (BCM_E_UNAVAIL);
}
                   

/* Double tagging */

/*
 * Function:
 *  bcm_robo_port_dtag_mode_set
 * Description:
 *  Set the double-tagging mode of a port.
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  mode - Double-tagging mode, one of:
 *      BCM_PORT_DTAG_MODE_NONE     No double tagging
 *      BCM_PORT_DTAG_MODE_INTERNAL Service provider port
 *      BCM_PORT_DTAG_MODE_EXTERNAL Customer port
 * Return Value:
 *  BCM_E_XXX
 */
int
bcm_robo_port_dtag_mode_set(int unit, bcm_port_t port, int mode)
{
    pbmp_t  t_pbm, clear_pbm;
    int     pre_mode;

    soc_cm_debug(DK_PORT, "BCM API : %s(port=%d,mode=%d)..\n",
            __func__,port,mode);
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(bcm_port_dtag_mode_get(unit, port, &pre_mode));

    BCM_PBMP_CLEAR(clear_pbm); 
    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    
    switch (mode) {
    case BCM_PORT_DTAG_MODE_NONE:
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, clear_pbm, 
                    DRV_PORT_PROP_DTAG_MODE, FALSE));
        break;
        
    case BCM_PORT_DTAG_MODE_INTERNAL:
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, clear_pbm, 
                    DRV_PORT_PROP_DTAG_MODE, TRUE));
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_DTAG_ISP_PORT, TRUE));
        break;

    case BCM_PORT_DTAG_MODE_EXTERNAL:
        /* CPU must be ISP port always */
        if (IS_CPU_PORT(unit, port)){
            soc_cm_debug(DK_WARN, "CPU is not proper to be None-ISP port.\n");
        }
        
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, clear_pbm, 
                    DRV_PORT_PROP_DTAG_MODE, TRUE));
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
                    (unit, t_pbm,
                    DRV_PORT_PROP_DTAG_ISP_PORT, FALSE));

        break;
       
    default: 
           return SOC_E_PARAM; 
    }
    
#if defined(BCM_53115)  
    if (SOC_IS_ROBO53115(unit)){
#ifndef LVL7_FIXUP        
        if (IS_VT_CFP_INIT){
#endif
            /* if Dtag Mode changed, rebuild the bounded CFP and EVR table */
            if (pre_mode != mode){
                /* rebuild cfp for those created VT entries */
#ifndef LVL7_FIXUP
                BCM_IF_ERROR_RETURN(_bcm_robo_vlan_vtcfp_isp_change(unit));
#endif
                /* rebuild evr(egress vlan remark) */
                BCM_IF_ERROR_RETURN(
                        _bcm_robo_vlan_vtevr_isp_change(unit, t_pbm));
            }
#ifndef LVL7_FIXUP
        }
#endif
    }
#endif
        
#if QVLAN_UTBMP_BACKUP
    if(pre_mode != mode){
        /* if mode setting is changed, we have to reprogram whole valid 1Q VLAN 
         * untag bitmap. 
         */
        BCM_IF_ERROR_RETURN(_bcm_robo_vlan_utbmp_dt_rebuild(unit, mode));
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_dtag_mode_get
 * Description:
 *  Return the current double-tagging mode of a port.
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  mode - (OUT) Double-tagging mode
 * Return Value:
 *  BCM_E_XXX
 */
int
bcm_robo_port_dtag_mode_get(int unit, bcm_port_t port, int *mode)
{

    uint32  drv_value, temp;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_dtag_mode_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
             (unit, port, 
              DRV_PORT_PROP_DTAG_MODE, &drv_value));

    if (drv_value) {
        BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
             (unit, port, DRV_PORT_PROP_DTAG_ISP_PORT, &temp));
        if (temp) {
            *mode = BCM_PORT_DTAG_MODE_INTERNAL;
        } else {
            *mode = BCM_PORT_DTAG_MODE_EXTERNAL;
        }
    } else {
    *mode = BCM_PORT_DTAG_MODE_NONE;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *  bcm_robo_port_tpid_set
 * Description:
 *  Set the default Tag Protocol ID for a port.
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  tpid - Tag Protocol ID
 * Return Value:
 *  BCM_E_XXX
 * Notes:
 *  This API is not specifically double-tagging-related, but
 *  the port TPID becomes the service provider TPID when double-tagging
 *  is enabled on a port.  The default TPID is 0x8100.
 *  On BCM5673, only 0x8100 is allowed for the inner (customer) tag.
 */
int
bcm_robo_port_tpid_set(int unit, bcm_port_t port, uint16 tpid)
{

    pbmp_t      t_pbm;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_tpid_set()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_set)
            (unit, t_pbm,
            DRV_PORT_PROP_DTAG_TPID, tpid));
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_tpid_get
 * Description:
 *  Retrieve the default Tag Protocol ID for a port.
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  tpid - (OUT) Tag Protocol ID
 * Return Value:
 *  BCM_E_XXX
 */
int
bcm_robo_port_tpid_get(int unit, bcm_port_t port, uint16 *tpid)
{
    uint32 drv_val;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_tpid_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
             (unit, port, 
              DRV_PORT_PROP_DTAG_TPID, &drv_val));
    *tpid = (uint16)drv_val;
    return BCM_E_NONE;
}
    
/*
 * Function:
 *      bcm_robo_port_tpid_add
 * Description:
 *      Add allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      tpid - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_robo_port_tpid_add(int unit, bcm_port_t port, 
                uint16 tpid, int color_select)
{
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_robo_port_tpid_delete
 * Description:
 *      Delete allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 *      tpid - (IN) Tag Protocol ID
 * Return Value:
 *      BCM_E_XXX
 */
int
bcm_robo_port_tpid_delete(int unit, bcm_port_t port, uint16 tpid)
{
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_robo_port_tpid_delete_all
 * Description:
 *      Delete all allowed TPID for a port.
 * Parameters:
 *      unit - (IN) Device number
 *      port - (IN) Port number
 * Return Value:
 *      BCM_E_XXX
 */
int 
bcm_robo_port_tpid_delete_all(int unit, bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}
/*
 * Function:
 *  bcm_port_inner_tpid_get
 * Purpose:
 *      Get the expected TPID for the inner tag in double-tagging mode.
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  tpid - (OUT) Tag Protocol ID
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_XXX
 */
int
bcm_robo_port_inner_tpid_get(
    int unit,
    bcm_port_t  port,
    uint16 *    tpid)
{
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_inner_tpid_get()..\n");

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    *tpid = 0x8100;
    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_port_inner_tpid_set
 * Purpose:
 *      Set the expected TPID for the inner tag in double-tagging mode.
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  tpid - Tag Protocol ID
 * Returns:
 *  BCM_E_NONE - Success.
 *  BCM_E_XXX
 */
int
bcm_robo_port_inner_tpid_set(
    int unit,
    bcm_port_t  port,
    uint16  tpid)
{
    return BCM_E_UNAVAIL;
}
    
/*
 * Function:
 *  bcm_port_cable_diag
 * Description:
 *  Run Cable Diagnostics on port
 * Parameters:
 *  unit - Device number
 *  port - Port number
 *  status - (OUT) cable diag status structure
 * Return Value:
 *  BCM_E_XXX
 * Notes:
 *  Cable diagnostics are only supported by some phy types
 *    (currently 5248 10/100 phy and 546x 10/100/1000 phys)
 */
int
bcm_robo_port_cable_diag(
    int unit,
    bcm_port_t  port,
    bcm_port_cable_diag_t * status)
{

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_get)
            (unit, port, 
            DRV_PORT_PROP_PHY_CABLE_DIAG, 
            (uint32 *)status));

    return BCM_E_NONE;
}
                
                   

/*
 * Each field in the bcm_port_info_t structure has a corresponding
 * mask bit to control whether to get or set that value during the
 * execution of the bcm_port_selective_get/_set functions.
 * The OR of all requested masks should be stored in the action_mask
 * field of the bcm_port_info_t before calling the functions.
 */
 

/*
 * Routines to set or get port parameters in one call.
 *
 * The "action mask" indicates which values should be set/gotten.
 */


/*
 * Function:
 *  bcm_robo_port_info_get
 * Purpose:
 *  Get all information on the port
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *  info - Pointer to structure in which to save values
 * Returns:
 *  BCM_E_XXX
 */

int
bcm_robo_port_info_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_info_get()..\n");
    info->action_mask = BCM_PORT_ATTR_ALL_MASK;

    return bcm_port_selective_get(unit, port, info);
}

/*
 * Function:
 *  bcm_robo_port_info_set
 * Purpose:
 *  Set all information on the port
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *  info - Pointer to structure in which to save values
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  Checks if AN is on, and if so, clears the
 *  proper bits in the action mask.
 */

int
bcm_robo_port_info_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_info_set()..\n");
    info->action_mask = BCM_PORT_ATTR_ALL_MASK;

    /* If autoneg is set, remove those attributes controlled by it */
    if (info->autoneg){
        info->action_mask &= ~BCM_PORT_AN_ATTRS;
    }

    return bcm_port_selective_set(unit, port, info);
}

/*
 * Function:
 *  bcm_robo_port_selective_get
 * Purpose:
 *  Get all available port parameters at once.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      info - (OUT) Structure to pass the information (see port.h)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  None.
 */

int
bcm_robo_port_selective_get(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int         r;
    uint32      mask;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_selective_get()..\n");
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    mask = info->action_mask;

    if (mask & BCM_PORT_ATTR_ENABLE_MASK){

        r = bcm_port_enable_get(unit, port, &info->enable);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LINKSTAT_MASK){
        r = bcm_port_link_status_get(unit, port, &info->linkstatus);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK){
        r = bcm_port_autoneg_get(unit, port, &info->autoneg);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LOCAL_ADVERT_MASK){
        r = bcm_port_advert_get(unit, port, &info->local_advert);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_REMOTE_ADVERT_MASK){
        if ((r = bcm_port_advert_remote_get(unit, port, 
                                &info->remote_advert)) < 0){
            info->remote_advert = 0;
            info->remote_advert_valid = FALSE;
        } else {
            info->remote_advert_valid = TRUE;
        }
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK){
        if ((r = bcm_port_speed_get(unit, port, &info->speed)) < 0){
            if (r != BCM_E_BUSY){
                return(r);
            } else {
                info->speed = 0;
            }
        }
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK){
        if ((r = bcm_port_duplex_get(unit, port, &info->duplex)) < 0){
            if (r != BCM_E_BUSY){
                return(r);
            } else {
                info->duplex = 0;
            }
        }
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MASK){
        r = bcm_port_pause_get(unit, port, &info->pause_tx, &info->pause_rx);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
    /*
        r = bcm_port_pause_addr_get(unit, port, info->pause_mac);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
             "bcm_port_pause_addr_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    */
    }


    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK){
        r = bcm_port_linkscan_get(unit, port, &info->linkscan);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK) {
        r = bcm_port_learn_get(unit, port, &info->learn);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_DISCARD_MASK){
        r = bcm_port_discard_get(unit, port, &info->discard);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK){
        r = bcm_port_vlan_member_get(unit, port, &info->vlanfilter);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK){
        r = bcm_port_untagged_priority_get(unit, port,
                                &info->untagged_priority);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK){
        r = bcm_port_untagged_vlan_get(unit, port, &info->untagged_vlan);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK){
        r = bcm_port_stp_get(unit, port, &info->stp_state);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PFM_MASK) {
        r = bcm_port_pfm_get(unit, port, &info->pfm);
        if (r != BCM_E_UNAVAIL) {
            SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                         "bcm_port_pfm_get failed: %s\n", bcm_errmsg(r)));
        }
        BCM_IF_ERROR_NOT_UNAVAIL_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK){
        r = bcm_port_loopback_get(unit, port, &info->loopback);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PHY_MASTER_MASK){
        r = bcm_port_master_get(unit, port, &info->phy_master);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK){
        r = bcm_port_interface_get(unit, port, &info->interface);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_ENCAP_MASK){
    /*
        r = bcm_port_encap_get(unit, port, &info->encap_mode);
        BCM_IF_ERROR_RETURN(r);
    */
    }

    if (mask & BCM_PORT_ATTR_RATE_MCAST_MASK){
    /* following routine is defined in rate.c file */
    /*
        r = bcm_rate_mcast_get(unit, &info->mcast_limit,
                                &info->mcast_limit_enable, port);
        BCM_IF_ERROR_RETURN(r);
    */
    }

    if (mask & BCM_PORT_ATTR_RATE_BCAST_MASK){
    /* following routine is defined in rate.c file */
    /*
        r = bcm_rate_bcast_get(unit, &info->bcast_limit,
                                &info->bcast_limit_enable, port);
        BCM_IF_ERROR_RETURN(r);
    */
    }

    if (mask & BCM_PORT_ATTR_RATE_DLFBC_MASK){
    /* following routine is defined in rate.c file */
    /*
        r = bcm_rate_dlfbc_get(unit, &info->dlfbc_limit,
                                    &info->dlfbc_limit_enable, port);
        BCM_IF_ERROR_RETURN(r);
    */
    }

    if (mask & BCM_PORT_ATTR_SPEED_MAX){
        r = bcm_port_speed_max(unit, port, &info->speed_max);

        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_ABILITY){
        r = bcm_port_ability_get(unit, port, &info->ability);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        /*
        r = bcm_port_frame_max_get(unit, port, &info->frame_max);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
               "bcm_port_frame_max_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
        */
    }

    if (mask & BCM_PORT_ATTR_MDIX_MASK) {
        r = bcm_port_mdix_get(unit, port, &info->mdix);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
               "bcm_port_mdix_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_MDIX_STATUS_MASK) {
        r = bcm_port_mdix_status_get(unit, port, &info->mdix_status);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
               "bcm_port_mdix_status_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_MEDIUM_MASK) {
        r = bcm_port_medium_get(unit, port, &info->medium);
        SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
               "bcm_port_medium_get failed: %s\n", bcm_errmsg(r)));
        BCM_IF_ERROR_RETURN(r);
    }
    soc_cm_debug(DK_PORT, "\t bcm_robo_port_selective_get()..mask=0x%0x\n",
                       mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_selective_set
 * Purpose:
 *  Set all available port parameters at once.
 * Parameters:
 *  unit - RoboSwitch Unit #.
 *  port - RoboSwitch port #.
 *      info - Structure to pass the information (see port.h)
 * Returns:
 *  BCM_E_NONE
 *  BCM_E_XXX
 * Notes:
 *  Does not set spanning tree state.
 */

int
bcm_robo_port_selective_set(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int         r = BCM_E_NONE;
    uint32      mask;
    int         flags = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    mask = info->action_mask;
    soc_cm_debug(DK_PORT, 
                    "BCM API : bcm_robo_port_selective_set()..mask=0x%0x\n",
                    mask);

    if (mask & BCM_PORT_ATTR_ENABLE_MASK){
        r = bcm_port_enable_set(unit, port, info->enable);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MAC_MASK){
        /*
        r = bcm_port_pause_addr_set(unit, port, info->pause_mac);
        BCM_IF_ERROR_RETURN(r);
        */
    }

    if (mask & BCM_PORT_ATTR_INTERFACE_MASK){
        r = bcm_port_interface_set(unit, port, info->interface);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PHY_MASTER_MASK){
        r = bcm_port_master_set(unit, port, info->phy_master);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LINKSCAN_MASK){
        r = bcm_port_linkscan_set(unit, port, info->linkscan);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LEARN_MASK){
        r = bcm_port_learn_set(unit, port, info->learn);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_DISCARD_MASK){
        r = bcm_port_discard_set(unit, port, info->discard);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_VLANFILTER_MASK){
        r = bcm_port_vlan_member_set(unit, port, info->vlanfilter);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_PRI_MASK){
        r = bcm_port_untagged_priority_set(unit, port, 
                                info->untagged_priority);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_UNTAG_VLAN_MASK){
        r = bcm_port_untagged_vlan_set(unit, port, info->untagged_vlan);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PFM_MASK) {
        r = bcm_port_pfm_set(unit, port, info->pfm);
        if (r != BCM_E_UNAVAIL) {
            SOC_IF_ERROR_DEBUG_PRINT(r, (DK_VERBOSE,
                   "bcm_port_pfm_set failed: %s\n", bcm_errmsg(r)));
        }
        BCM_IF_ERROR_NOT_UNAVAIL_RETURN(r);
    }

    /*
     * Set loopback mode before setting the speed/duplex, since it may
     * affect the allowable values for speed/duplex.
     */
    if (mask & BCM_PORT_ATTR_LOOPBACK_MASK){
        r = bcm_port_loopback_set(unit, port, info->loopback);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_LOCAL_ADVERT_MASK){
        r = bcm_port_advert_set(unit, port, info->local_advert);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_AUTONEG_MASK){
        r = bcm_port_autoneg_set(unit, port, info->autoneg);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_SPEED_MASK){
        r = bcm_port_speed_set(unit, port, info->speed);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_DUPLEX_MASK){
        r = bcm_port_duplex_set(unit, port, info->duplex);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_PAUSE_MASK){
        int tpause, rpause;
        
        tpause = rpause = -1;
        if (mask & BCM_PORT_ATTR_PAUSE_TX_MASK) {
            tpause = info->pause_tx;
        }
        if (mask & BCM_PORT_ATTR_PAUSE_RX_MASK) {
            rpause = info->pause_rx;
        }
        r = bcm_port_pause_set(unit, port, tpause, rpause);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_ENCAP_MASK){
        /*
        r = bcm_port_encap_set(unit, port, info->encap_mode);
        BCM_IF_ERROR_RETURN(r);
        */
    }

    if (mask & BCM_PORT_ATTR_RATE_MCAST_MASK){
        /* following routine is defined in rate.c file */
        flags =  (info->mcast_limit_enable) ? BCM_RATE_MCAST : 0;
        /*
        r = bcm_rate_mcast_set(unit, info->mcast_limit, flags, port);
        if (r == BCM_E_UNAVAIL) 
        {
            r = BCM_E_NONE;
        }
        
        BCM_IF_ERROR_RETURN(r);
        */
    }

    if (mask & BCM_PORT_ATTR_RATE_BCAST_MASK){
        /* following routine is defined in rate.c file */
        flags =  (info->bcast_limit_enable) ? BCM_RATE_BCAST : 0;
        /*
        r = bcm_rate_bcast_set(unit, info->bcast_limit, flags, port);
        if (r == BCM_E_UNAVAIL) 
        {
            r = BCM_E_NONE;
        }
        
        BCM_IF_ERROR_RETURN(r);
        */
    }

    if (mask & BCM_PORT_ATTR_RATE_DLFBC_MASK){
        /* following routine is defined in rate.c file */
        flags =  (info->dlfbc_limit_enable) ? BCM_RATE_DLF : 0;
        /*
        r = bcm_rate_dlfbc_set(unit, info->dlfbc_limit, flags, port);
        if (r == BCM_E_UNAVAIL) 
        {
            r = BCM_E_NONE;
        }
        
        BCM_IF_ERROR_RETURN(r);
        */
    }

    if (mask & BCM_PORT_ATTR_STP_STATE_MASK){
        r = bcm_port_stp_set(unit, port, info->stp_state);
        BCM_IF_ERROR_RETURN(r);
    }

    if (mask & BCM_PORT_ATTR_FRAME_MAX_MASK) {
    /*
        r = bcm_port_frame_max_set(unit, port, info->frame_max);
        BCM_IF_ERROR_RETURN(r);
    */
    }

    if (mask & BCM_PORT_ATTR_MDIX_MASK) {
        r = bcm_port_mdix_set(unit, port, info->mdix);
        BCM_IF_ERROR_RETURN(r);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *  bcm_robo_port_info_save
 * Purpose:
 *  Save the current settings of a port
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *  info - Pointer to structure in which to save values
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  The action_mask will be adjusted so that the
 *  proper values will be set when a restore is made.
 *  This mask should not be altered between these calls.
 */

int
bcm_robo_port_info_save(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    info->action_mask = BCM_PORT_ATTR_ALL_MASK;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_info_save()..\n");
    BCM_IF_ERROR_RETURN(bcm_port_selective_get(unit, port, info));

    /* If autoneg is set, remove those attributes controlled by it */
    if (info->autoneg) {
        info->action_mask &= ~BCM_PORT_AN_ATTRS;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *  bcm_robo_port_info_restore
 * Purpose:
 *  Restore port settings saved by info_save
 * Parameters:
 *  unit - RoboSwitch unit #
 *  port - RoboSwitch port #
 *  info - Pointer to structure with info from port_info_save
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  bcm_port_info_save has done all the work.
 *  We just call port_selective_set.
 */

int
bcm_robo_port_info_restore(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_info_restore()..\n");
    return bcm_port_selective_set(unit, port, info);
}
                              
int 
bcm_robo_port_sample_rate_get(int unit, bcm_port_t port,int *ingress_rate,int *egress_rate)
{
    int rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->port_get)
            (unit, port, DRV_PORT_PROP_SFLOW_INGRESS_RATE,
             (uint32 *) ingress_rate));
    BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->port_get)
            (unit, port, DRV_PORT_PROP_SFLOW_EGRESS_RATE,
             (uint32 *) egress_rate));

    return rv;
}

int 
bcm_robo_port_sample_rate_set(int unit,bcm_port_t port,int ingress_rate, int egress_rate)
{
    int rv = BCM_E_NONE;
    pbmp_t  pbm;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm, port);
    /* 
     * Robo chips that support sFlow feature are BCM5348/5347/53242/53262.
     * According to the above chips' ability of sFlow,
     * the meaning of the parameters of the API are,
     * For ingress_rate,
     * ingress_rate = 0, disable the port's sFlow feature.
     * ingress_rate != 0, change the sample rate of all enabled ports.
     *
     * For egress_rate, only one port can do egress sFlow at a time.
     * egress_rate = 0, disable the port's sFlow feature.
     * egress_rate != 0, change the sample rate of the port.
     * 
     */
    BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->port_set)
            (unit, pbm, DRV_PORT_PROP_SFLOW_INGRESS_RATE, ingress_rate));
    BCM_IF_ERROR_RETURN(
            (DRV_SERVICES(unit)->port_set)
            (unit, pbm, DRV_PORT_PROP_SFLOW_EGRESS_RATE, egress_rate));

    return rv;
}

int
bcm_robo_port_fault_get(int unit, bcm_port_t port, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_port_vlan_member_get(int unit, bcm_port_t port, uint32 *flags)
{
    int     rv;
    uint32      temp;
    
    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_vlan_member_get()..\n");
    
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    /* For Robo chips, this is global setting (not per-port) */ 
    rv = ((DRV_SERVICES(unit)->port_get)
            (unit, port,
            DRV_PORT_PROP_INGRESS_VLAN_CHK, &temp));
    SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_PORT|DK_VERBOSE,
            "DRV_PORT_PROP_INGRESS_VLAN_CHK failed: %s\n", 
            bcm_errmsg(rv)));
    if (temp) {
        *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
    }

    return rv;
}
int
bcm_robo_port_vlan_member_set(int unit, bcm_port_t port, uint32 flags)
{
    int     rv;
    pbmp_t  pbm;
    uint32      temp;
    
    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_vlan_member_set()..\n");
    
    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));
    
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);

    /* For Robo chips, this is global setting (not per-port) */ 

    if (flags & BCM_PORT_VLAN_MEMBER_INGRESS) {
        temp  = 1;
    } else {
        temp = 0;
    }
    rv = ((DRV_SERVICES(unit)->port_set)
            (unit, pbm,
            DRV_PORT_PROP_INGRESS_VLAN_CHK, temp));
    SOC_IF_ERROR_DEBUG_PRINT
            (rv, (DK_PORT|DK_VERBOSE,
            "DRV_PORT_PROP_INGRESS_VLAN_CHK failed: %s\n", 
            bcm_errmsg(rv)));

    return rv;
}

int
bcm_robo_port_trunk_index_get(int unit, bcm_port_t port, int *port_index)
{
    return BCM_E_UNAVAIL;
}

int
bcm_robo_port_trunk_index_set(int unit, bcm_port_t port, int port_index)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_port_cfi_color_get(int unit, bcm_port_t port,
                           int cfi, bcm_color_t *color)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_port_cfi_color_set(int unit, bcm_port_t port,
                           int cfi, bcm_color_t color)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_port_priority_color_get(int unit, bcm_port_t port,
                                int prio, bcm_color_t *color)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_port_priority_color_set(int unit, bcm_port_t port,
                                int prio, bcm_color_t color)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_port_vlan_priority_map_set
 * Description:
 *      Define the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      1. This API programs only the mapping table. 
 *      2. In Robo chip, the color and cfi parameters are not supported here 
 *          so far (for bcm5395/bcm53115/bcm53118)
 *      3. for no color feature in ROBO chips, the feature define is not 
 *          applied here(soc_feature_color_prio_map).
 */

int
bcm_robo_port_vlan_priority_map_set(int unit, bcm_port_t port, int pkt_pri,
                        int cfi, int internal_pri, bcm_color_t color)
{
    
    BCM_IF_ERROR_RETURN(_bcm_robo_port_gport_validate(unit, port, &port));
    
    if ((pkt_pri > 7) || (cfi > 1)) {
        return BCM_E_PARAM;
    }

    /* currently, the color is not used on ROBO chip for PCP2TC */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_pri_mapop_set)
            (unit, port, DRV_PORT_OP_PCP2TC, pkt_pri, internal_pri, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_port_vlan_priority_map_get
 * Description:
 *      Get the mapping of incomming port, packet priority, and cfi bit to
 *      switch internal priority and color.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 *      internal_pri - (OUT) Internal priority
 *      color        - (OUT) color
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      1. This API programs only the mapping table. 
 *      2. In Robo chip, the color parameter here is not supported so far (for 
 *          bcm5395/bcm53115/bcm53118)
 */

int
bcm_robo_port_vlan_priority_map_get(int unit, bcm_port_t port, int pkt_pri,
                       int cfi, int *internal_pri, bcm_color_t *color)
{
    
    BCM_IF_ERROR_RETURN(_bcm_robo_port_gport_validate(unit, port, &port));

    if ((pkt_pri > 7) || (cfi > 1)) {
        return BCM_E_PARAM;
    }

    SOC_DEBUG_PRINT((DK_PORT, "%s, u=%d p=%d pkt_pri=%d\n",
                     __func__, unit, port, pkt_pri));

    /* currently, the color is not used on ROBO chip for PCP2TC */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_pri_mapop_get)
            (unit, port, DRV_PORT_OP_PCP2TC, pkt_pri,
             (uint32 *) internal_pri, NULL));

    *color = _BCM_ROBO_COLOR_NONE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_port_vlan_priority_unmap_set
 * Description:
 *      Define the mapping of outgoing port, internal priority, and color to
 *  outgoing packet priority and cfi bit.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) Color
 *      pkt_pri      - (IN) Packet priority
 *      cfi          - (IN) Packet CFI
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *  1. in ESW device, API return UNAVAIL if no "soc_feature_color_prio_map"
 *      feature designed.
 *  2. In Robo device, internal priority can be mapped to out-going packet
 *      no matter the feature "soc_feature_color_prio_map" is designed or not.
 */
int
bcm_robo_port_vlan_priority_unmap_set(int unit, bcm_port_t port, 
                        int internal_pri, bcm_color_t color, 
                        int pkt_pri, int cfi)
{
    uint32 color_op;
    
    BCM_IF_ERROR_RETURN(_bcm_robo_port_gport_validate(unit, port, &port));
    
    if ((internal_pri > 7) || 
            ((color != bcmColorGreen) && (color != bcmColorYellow) &&
             (color != bcmColorRed))) {
        return BCM_E_PARAM;
    }
    
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        color_op = (_BCM_COLOR_ENCODING(unit, color) ? 
                DRV_PORT_OP_OUTBAND_TC2PCP : DRV_PORT_OP_NORMAL_TC2PCP);
    } else {
        color_op = DRV_PORT_OP_NORMAL_TC2PCP;
    }

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_pri_mapop_set)
            (unit, port, color_op, internal_pri, pkt_pri, cfi));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_robo_port_vlan_priority_unmap_get
 * Description:
 *      Get the mapping of outgoing port, internal priority, and color to
 *  outgoing packet priority and cfi bit.
 * Parameters:
 *      unit         - (IN) Device number
 *      port         - (IN) Port number
 *      internal_pri - (IN) Internal priority
 *      color        - (IN) Color
 *      pkt_pri      - (OUT) Packet priority
 *      cfi          - (OUT) Packet CFI
 * Return Value:
 *      BCM_E_XXX
 * Note:
 *      This API programs only the mapping table. 
 */
int
bcm_robo_port_vlan_priority_unmap_get(int unit, bcm_port_t port, 
                         int internal_pri, bcm_color_t color,
                         int *pkt_pri, int *cfi)
{
    uint32 color_op;
    
    BCM_IF_ERROR_RETURN(_bcm_robo_port_gport_validate(unit, port, &port));

    if ((internal_pri > 7) || 
            ((color != bcmColorGreen) && (color != bcmColorYellow) &&
             (color != bcmColorRed))) {
        return BCM_E_PARAM;
    }

    SOC_DEBUG_PRINT((DK_PORT, "%s, u=%d p=%d internal_pri=%d color=%d\n",
                     __func__, unit, port, internal_pri, color));

    if (soc_feature(unit, soc_feature_color_prio_map)) {
        color_op = (_BCM_COLOR_ENCODING(unit, color) ? 
                DRV_PORT_OP_OUTBAND_TC2PCP : DRV_PORT_OP_NORMAL_TC2PCP);
    } else {
        color_op = DRV_PORT_OP_NORMAL_TC2PCP;
    }

    /* currently, the color is not used on ROBO chip for PCP2TC */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->port_pri_mapop_get)
            (unit, port, color_op, internal_pri, (uint32 *) pkt_pri,
             (uint32 *) cfi));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_port_l3_modify_set
 * Description:
 *      Enable/Disable ingress port based L3 unicast packet operations.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - bitmap of the packet operations to be enabled or disabled.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_robo_port_l3_modify_set(int unit, bcm_port_t port, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_l3_modify_get
 * Description:
 *      Get ingress port based L3 unicast packet operations status.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - (OUT) pointer to uint32 where bitmap of the current L3 packet 
 *              operations is returned.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_robo_port_l3_modify_get(int unit, bcm_port_t port, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_ipmc_modify_set
 * Description:
 *      Enable/Disable ingress port based L3 multicast packet operations.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - bitmap of the packet operations to be enabled or disabled.
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_robo_port_ipmc_modify_set(int unit, bcm_port_t port, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_ipmc_modify_get
 * Description:
 *      Enable/Disable ingress port based L3 multicast packet operations.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - (OUT) pointer to uint32 where bitmap of the current L3 packet
 *              operations is returned.
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 * Notes:
 *      Available on XGS3 only.
 */
int 
bcm_robo_port_ipmc_modify_get(int unit, bcm_port_t port, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_control_set
 * Description:
 *      Enable/Disable various features at the port level.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      type - Identifies the port feature to be controlled
 *      value - Value of the bit field in port table
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_port_control_set(int unit, bcm_port_t port, 
                         bcm_port_control_t type, int value)
{
    pbmp_t  t_pbm;
    int rv = BCM_E_NONE;

    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_control_set()..\n");

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    BCM_PBMP_CLEAR(t_pbm); 
    BCM_PBMP_PORT_ADD(t_pbm, port);
    switch(type) {
        case bcmPortControlPreservePacketPriority:
            rv = ((DRV_SERVICES(unit)->queue_mapping_type_set)
                         (unit, t_pbm, 
                        DRV_QUEUE_MAP_PRIO, (uint8)value));
            break;
            
        case bcmPortControlEgressVlanPriUsesPktPri:
            /* this type value definition is :
             *  1: use incomming packet priority to be outgoing pri.
             *  0: use internal packet priority(TC) to obtain outgoing pri.
             */
            rv = ((DRV_SERVICES(unit)->port_set)
                         (unit, t_pbm, DRV_PORT_PROP_EGRESS_PCP_REMARK, 
                         (value ? 0 : 1)));
            break;
            
        case bcmPortControlEgressVlanCfiUsesPktPri:
            /* this type value definition is :
             *  1: the outgoing CFI is the same as incomming packet CFI.
             *  0: use internal packet priority(TC) to obtain outgoing CFI.
             */
            rv = ((DRV_SERVICES(unit)->port_set)
                         (unit, t_pbm, DRV_PORT_PROP_EGRESS_CFI_REMARK, 
                         (value ? 0 : 1)));
            break;
        case bcmPortControlIngressRateControlDrop:
            if (!(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)||
                SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit))) {
                /* Other chips are per-system configuration*/
                BCM_PBMP_ASSIGN(t_pbm, PBMP_ALL(unit));
            }
            rv = ((DRV_SERVICES(unit)->rate_config_set)
                         (unit, t_pbm, DRV_RATE_CONFIG_DROP_ENABLE, 
                         (value ? 1 : 0)));
            break;
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_control_set: u=%d p=%d type=%d value=%d rv=%d\n",
             unit, port, (int)type, value, rv));

    return rv;
}

/*
 * Function:
 *      bcm_port_control_get
 * Description:
 *      Return the current value of the port feature identified by <type> parameter.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      type - Identifies the port feature to be controlled
 *      value - Value of the bit field in port table
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_robo_port_control_get(int unit, bcm_port_t port, 
                         bcm_port_control_t type, int *value)                         
{
    uint8 state = 0;
    int rv = BCM_E_NONE;

    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_control_get()..\n");

    BCM_IF_ERROR_RETURN(
        _bcm_robo_port_gport_validate(unit, port, &port));

    switch(type) {
        case bcmPortControlPreservePacketPriority:
            rv = ((DRV_SERVICES(unit)->queue_mapping_type_get)
                         (unit, port, 
                        DRV_QUEUE_MAP_PRIO, &state));

            *value = (int)state;
            break;
            
        case bcmPortControlEgressVlanPriUsesPktPri:
            /* this type value definition is :
             *  1: use incomming packet priority to be outgoing pri.
             *  0: use internal packet priority(TC) to obtain outgoing pri.
             */
            rv = ((DRV_SERVICES(unit)->port_get)
                         (unit, port, 
                        DRV_PORT_PROP_EGRESS_PCP_REMARK, (uint32 *)value));
            *value = *value ? 0 : 1;
            break;
            
        case bcmPortControlEgressVlanCfiUsesPktPri:
            /* this type value definition is :
             *  1: the outgoing CFI is the same as incomming packet CFI.
             *  0: use internal packet priority(TC) to obtain outgoing CFI.
             */
            rv = ((DRV_SERVICES(unit)->port_get)
                         (unit, port, 
                        DRV_PORT_PROP_EGRESS_CFI_REMARK, (uint32 *)value));

            *value = *value ? 0 : 1;
            break;
        case bcmPortControlIngressRateControlDrop:
            rv = ((DRV_SERVICES(unit)->rate_config_get)
                         (unit, port, 
                        DRV_RATE_CONFIG_DROP_ENABLE, (uint32 *)value));

            *value = *value ? 1 : 0;
            break;
            
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }

    SOC_DEBUG_PRINT((DK_PORT,
             "bcm_port_control_get: u=%d p=%d type=%d value=%d rv=%d\n",
             unit, port, (int)type, *value, rv));

    return rv;
}

/* 
 * Function:
 *      bcm_port_vlan_inner_tag_set
 * Purpose:
 *      Set the inner tag to be added to the packet.
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      inner_tag  - (IN) Inner tag. 
 *                    Priority[15:13] CFI[12] VLAN ID [11:0]
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_port_vlan_inner_tag_set(int unit, bcm_port_t port, uint16 inner_tag)
{
    return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_port_vlan_inner_tag_get
 * Purpose:
 *      Get the inner tag to be added to the packet.
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      inner_tag  - (OUT) Inner tag. 
 *                    Priority[15:13] CFI[12] VLAN ID [11:0]
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_port_vlan_inner_tag_get(int unit, bcm_port_t port, uint16 *inner_tag)
{
    return BCM_E_UNAVAIL;
}


int
bcm_robo_port_l3_mtu_get(int unit, bcm_port_t port, int *size)
{
    return BCM_E_UNAVAIL;
}
int
bcm_robo_port_l3_mtu_set(int unit, bcm_port_t port, int size)

{
    return BCM_E_UNAVAIL;
}
int 
bcm_robo_port_force_forward_set(int unit, bcm_port_t port, 
                               bcm_port_t egr_port, int enable)
{
    return BCM_E_UNAVAIL;
}
int 
bcm_robo_port_force_forward_get(int unit, bcm_port_t port, 
                               bcm_port_t *egr_port, int *enabled)
{
    return BCM_E_UNAVAIL;
}
int 
bcm_robo_port_class_get(int unit, bcm_port_t port, 
                bcm_port_class_t pclass, uint32 *pclass_id)
{
    return BCM_E_UNAVAIL;
}
int 
bcm_robo_port_class_set(int unit, bcm_port_t port, 
                bcm_port_class_t pclass, uint32 pclass_id)
{
 return BCM_E_UNAVAIL;
}

int 
bcm_robo_port_force_vlan_get(int unit, bcm_port_t port, 
                                bcm_vlan_t *vlan, int *pkt_prio, uint32 *flags)
{
 return BCM_E_UNAVAIL;
}

int 
bcm_robo_port_force_vlan_set(int unit, bcm_port_t port, 
                                bcm_vlan_t vlan, int pkt_prio, uint32 flags)
{
 return BCM_E_UNAVAIL;
}

int
bcm_robo_port_phy_control_set(int unit, bcm_port_t port, 
                             bcm_port_phy_control_t type, uint32 value)
{
 return BCM_E_UNAVAIL;
}

int
bcm_robo_port_phy_control_get(int unit, bcm_port_t port,
                             bcm_port_phy_control_t type, uint32 *value)
{
 return BCM_E_UNAVAIL;
}

int
bcm_robo_port_phy_firmware_set(int unit, bcm_port_t port, uint32 flags,
                              int offset, uint8 *array, int length)
{
 return BCM_E_UNAVAIL;
}

/*
 * Function    : _bcm_gport_dest_t_init
 * Description : Initialize gport_dest structure
 * Parameters  : (IN/OUT)  gport_dest - Structure to initialize
 * Returns     : None
 */

void
_bcm_robo_gport_dest_t_init(_bcm_gport_dest_t *gport_dest)
{
    sal_memset(gport_dest, 0, sizeof (_bcm_gport_dest_t));
}

/*
 * Function    : _bcm_robo_gport_modport_hw2api_map
 * Description : Remaps module and port from HW space to API space 
 *
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  mod_in    - Module ID to map   
 *               (IN)  port_in   - Physical port to map   
 *               (OUT)  mod_out  - Module ID after mapping
 *               (OUT)  port_out - Port number after mapping 
 * Returns     : BCM_E_XXX
 * Notes       : If mod_out == NULL then port_out will be local physical port.
 */

int 
_bcm_robo_gport_modport_hw2api_map(int unit, bcm_module_t mod_in, 
            bcm_port_t port_in, bcm_module_t *mod_out, bcm_port_t *port_out)
{
    if (port_out == NULL) {
        return (BCM_E_PARAM);
    }

    if (NUM_MODID(unit) == 1) { /* HW device has single modid */
        if (mod_out != NULL) {
            *mod_out = mod_in;
        }
        *port_out = port_in;

        return (BCM_E_NONE);
    } else {
        /* ROBO sdk support no stacking so far */
        return (BCM_E_UNAVAIL);
    }
    
}

/*
 * Function    : _bcm_robo_gport_modport_api2hw_map
 * Description : Remaps module and port gport from API space to HW space.
 *
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  mod_in    - Module ID to map   
 *               (IN)  port_in   - Physical port to map   
 *               (OUT)  mod_out  - Module ID after mapping
 *               (OUT)  port_out - Port number after mapping 
 * Returns     : BCM_E_XXX
 */
int 
_bcm_robo_gport_modport_api2hw_map(int unit, bcm_module_t mod_in, 
            bcm_port_t port_in, bcm_module_t *mod_out, bcm_port_t *port_out)
{
    if (port_out == NULL || mod_out == NULL) {
        return (BCM_E_PARAM);
    }

    *mod_out = mod_in;
    *port_out = port_in;

    return (BCM_E_NONE);
}

/*
 * Function    : bcm_port_gport_get
 * Description : Get the GPORT ID for the specified physical port.
 *
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  port      - Port number
 *               (OUT) gport     - GPORT ID
 * Returns     : BCM_E_XXX
 */
int
bcm_robo_port_gport_get(int unit, bcm_port_t port, bcm_gport_t *gport)
{
    int                 num_modid;
    _bcm_gport_dest_t   dest;

    _bcm_robo_gport_dest_t_init(&dest);

    BCM_IF_ERROR_RETURN(bcm_robo_stk_modid_count(unit, &num_modid));
    if (!num_modid) {
        /* No modid for this device */
        return BCM_E_UNAVAIL;
    }

    PORT_PARAM_CHECK(unit, port);

    dest.port = port;
    /* in latest API document, this API must return the gport at 
     * modport type */
    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;

    BCM_IF_ERROR_RETURN(
        _bcm_robo_gport_modport_api2hw_map(unit, dest.modid, dest.port, 
                                      &(dest.modid), &(dest.port)));
    
    return _bcm_robo_gport_construct(unit, &dest, gport); 
}

#if BCM_ROBO_API_IS_NOT_DISPATCHABLE
/*
 * Function    : bcm_robo_port_local_get
 * Description : Get the local port from the given GPORT ID.
 *
 * Parameters  : (IN)  unit         - BCM device number
 *               (IN)  gport        - global port identifier
 *               (OUT) local_port   - local port encoded in gport
 * Returns     : BCM_E_XXX
 *
 * Note : 
 *  1. This API for ROBO is designed as different processing flow with ESW.
 *      - No Module info.
 */
int
bcm_robo_port_local_get(int unit, bcm_gport_t gport, bcm_port_t *local_port)
{

    if (SOC_GPORT_IS_LOCAL(gport)) {
        *local_port = SOC_GPORT_LOCAL_GET(gport);
    } else if (SOC_GPORT_IS_DEVPORT(gport)) {
        *local_port = SOC_GPORT_DEVPORT_PORT_GET(gport);
        if (unit != SOC_GPORT_DEVPORT_DEVID_GET(gport)) {
            return BCM_E_PORT;
        }
    } else {
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}
#endif  /* BCM_ROBO_API_IS_NOT_DISPATCHABLE */

int
bcm_robo_port_default_vlan_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
 return BCM_E_UNAVAIL;
}

int
bcm_robo_port_default_vlan_action_get(int unit, bcm_port_t port, 
                                     bcm_vlan_action_set_t *action)
{
 return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_port_egress_default_vlan_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_port_egress_default_vlan_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_port_egress_default_vlan_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (OUT) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_robo_port_egress_default_vlan_action_get(int unit, bcm_port_t port, 
                                            bcm_vlan_action_set_t *action)
{
    return BCM_E_UNAVAIL;
}

/* Gigabit PHY Power Modes */
int
bcm_robo_port_phy_power_mode_set(int unit, bcm_port_t port,
    int mode)
{
    uint8   phy_addr = PORT_TO_PHY_ADDR(unit, port);
    uint16 tmp;
    
    soc_cm_debug(DK_PORT, 
        "BCM API : bcm_robo_port_phy_power_mode_set()..port = %d, mode = %d\n",
        port, mode);
    if (SOC_IS_ROBO5398(unit) || SOC_IS_ROBO5397(unit) ||
        SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit) ||
        SOC_IS_ROBO53118(unit)) {

        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));

        /* Early return if the port on the robo device within built-in 
         * SerDes.
         *  (Prevent the improper register setting on the SerDes Mii reg) 
         */
        if (IS_ROBO_SPECIFIC_INT_SERDES(unit, port)){
            return BCM_E_UNAVAIL;
        }

        switch(mode) {
            case PHY_HIGH_PERFORMANCE_MODE:
                /* Set register 18h, shadow 010, bit 6 =0 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x18, 0x2007);
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x18, &tmp);
                tmp &= ~0x40;
                tmp &= ~(0x7 << 13);
                (DRV_SERVICES(unit)->miim_write)(unit, phy_addr, 0x18, tmp);
                
                /* Set expansion register F9h, bit1 = 0 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x17, 0x0ff9);
                (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x15, &tmp);
                if (tmp & 0x2) {
                    tmp &= ~0x2;
                    (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x15, tmp);
                }
                break;
                
            case PHY_LOW_POWER_AB0_MODE:
                /* Set register 18h, shadow 010, bit [15:13] = 000, bit 6 =1 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x18, 0x2007);
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x18, &tmp);
                tmp |= 0x40;
                tmp &= ~(0x7 << 13);
                (DRV_SERVICES(unit)->miim_write)(unit, phy_addr, 0x18, tmp);
                
                /* Set expansion register F9h, bit1 = 0 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x17, 0x0ff9);
                (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x15, &tmp);
                if (tmp & 0x2) {
                    tmp &= ~0x2;
                    (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x15, tmp);
                }
                break;
                
            case PHY_LOW_POWER_AB6_MODE:
                /* Set register 18h, shadow 010, bit [15:13] = 110, bit 6 =1 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x18, 0x2007);
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x18, &tmp);
                tmp |= 0x40;
                tmp &= ~(0x7 << 13);
                tmp |= (0x6 << 13);
                (DRV_SERVICES(unit)->miim_write)(unit, phy_addr, 0x18, tmp);
                
                /* Set expansion register F9h, bit1 = 0 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x17, 0x0ff9);
                (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x15, &tmp);
                if (tmp & 0x2) {
                    tmp &= ~0x2;
                    (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x15, tmp);
                }
                break;
                
            case PHY_GREEN_POWER_MODE:
                /* Set register 18h, shadow 010, bit [15:13] = 000, bit 6 =1 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x18, 0x2007);
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x18, &tmp);
                tmp |= 0x40;
                tmp &= ~(0x7 << 13);
                (DRV_SERVICES(unit)->miim_write)(unit, phy_addr, 0x18, tmp);
                
                /* Set expansion register F9h, bit1 = 0 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x17, 0x0ff9);
                (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x15, &tmp);
                if (!(tmp & 0x2)) {
                    tmp |= 0x2;
                    (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x15, tmp);
                }
                break;
                
            case PHY_STANDBY_POWER_DOWN_MODE:
                /* register 00h, bit 11 = 1*/
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x0, &tmp);
                tmp |= 0x800;
                (DRV_SERVICES(unit)->miim_write)(unit, phy_addr, 0x0, tmp);
                break;
                
            case PHY_NON_STANDBY_POWER_DOWN_MODE:
                /* register 00h, bit 11 = 0*/
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x0, &tmp);
                tmp &= ~0x800;
                (DRV_SERVICES(unit)->miim_write)(unit, phy_addr, 0x0, tmp);
                break;
            case PHY_AUTO_POWER_DOWN_MODE:
                /* Enable Auto Power-Down Mode */
                /* register 1Ch, shadow 01010, bit 5 =1 */
                (DRV_SERVICES(unit)->miim_write)
                        (unit, phy_addr, 0x1c, 0x2800);
                (DRV_SERVICES(unit)->miim_read)
                        (unit, phy_addr, 0x1c, &tmp);
                tmp |= 0x8020; /* auto power-down mode */
                (DRV_SERVICES(unit)->miim_write)
                        (unit, phy_addr, 0x1c, tmp);
                
                /* Enable Auto-MDIX When autoneg disabled */
                /* register 18h, shadow 111, bit 9 = 1 */
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x18, 0x7007);
                (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x18, &tmp);
                tmp |= 0x8200;
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x18, tmp);
                break;
            default:
                return BCM_E_PARAM;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

int
bcm_robo_port_phy_power_mode_get(int unit, bcm_port_t port,
    int *mode)
{
    int rv;
    int link_up;
    uint8   phy_addr = PORT_TO_PHY_ADDR(unit, port);
    uint16 tmp;
    
    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_phy_power_mode_get()..\n");

    if (SOC_IS_ROBO5398(unit) || SOC_IS_ROBO5397(unit) ||
        SOC_IS_ROBO5395(unit) || SOC_IS_ROBO53115(unit) ||
        SOC_IS_ROBO53118(unit)) {

        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));
        
        /* Early return if the port on the robo device within built-in 
         * SerDes.
         *  (Prevent the improper register setting on the SerDes Mii reg) 
         */
        if (IS_ROBO_SPECIFIC_INT_SERDES(unit, port)){
            return BCM_E_UNAVAIL;
        }
         
        rv = bcm_port_link_status_get(unit, port, &link_up);
        if ((rv == BCM_E_NONE) && (link_up)) {
            /* Check Low power bit */
            (DRV_SERVICES(unit)->miim_write)
                (unit, phy_addr, 0x18, 0x2007);
            (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x18, &tmp);
            if (tmp & 0x40) {
                /* Bit 6 = 1 */
                if (!(tmp & 0xe000)) {
                    /* bit [15:13] = 000 */
                    (DRV_SERVICES(unit)->miim_write)
                        (unit, phy_addr, 0x17, 0x0ff9);
                    (DRV_SERVICES(unit)->miim_read)
                        (unit, phy_addr, 0x15, &tmp);
                    if (tmp & 0x2) {
                        /* expansion reg. f9h, bit 1 = 1 */
                        *mode = PHY_GREEN_POWER_MODE;
                    } else {
                        /* expansion reg. f9h, bit 1 = 0 */
                        *mode = PHY_LOW_POWER_AB0_MODE;
                    }
                } else {
                    /* bit [15:13] != 000 */
                    *mode = PHY_LOW_POWER_AB6_MODE;
                    /* 
                     * In our design, if the low power is enabled, 
                     * It should be AB0 or AB6 mode.
                     */
                }
            } else {
                /* Bit 6 = 0 */
                *mode = PHY_HIGH_PERFORMANCE_MODE;
            }
        } else {
            /* Cneck if device in power-down mode */
            (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x0, &tmp);
            if (tmp & 0x800) {
                *mode = PHY_STANDBY_POWER_DOWN_MODE;
            } else {
                (DRV_SERVICES(unit)->miim_write)
                    (unit, phy_addr, 0x1c, 0x2800);
                (DRV_SERVICES(unit)->miim_read)(unit, phy_addr, 0x1c, &tmp);
                if (tmp & 0x20) {
                    *mode = PHY_AUTO_POWER_DOWN_MODE;
                } else {
                    /* Normal mode */
                    *mode = PHY_NON_STANDBY_POWER_DOWN_MODE;
                }
            }
            
            
        }

    } else {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

int
bcm_robo_port_phy_power_mode_select(int unit, bcm_port_t port)
{
    uint8   phy_addr = PORT_TO_PHY_ADDR(unit, port);
    uint16 tmp;
    bcm_port_cable_diag_t cd;
    int link_up = 0, link_speed = 0, discard_mode = 0;
    int rv;

    soc_cm_debug(DK_PORT, "BCM API : bcm_robo_port_phy_power_mode_select()..\n");
    if (SOC_IS_ROBO5398(unit) || SOC_IS_ROBO5397(unit) ||
        SOC_IS_ROBO5395(unit)  || SOC_IS_ROBO53115(unit) ||
        SOC_IS_ROBO53118(unit)) {
            
        BCM_IF_ERROR_RETURN(
            _bcm_robo_port_gport_validate(unit, port, &port));

        /* Early return if the port on the robo device within built-in 
         * SerDes.
         *  (Prevent the improper register setting on the SerDes Mii reg) 
         */
        if (IS_ROBO_SPECIFIC_INT_SERDES(unit, port)){
            return BCM_E_UNAVAIL;
        }

        bcm_port_discard_get(unit, port, &discard_mode);
        bcm_port_speed_get(unit, port, &link_speed);

        if (link_speed != 1000){
            /* For Giga speed only */
            return BCM_E_NONE;
        }

        bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_NONE); 

        /* Cable Checker */
        rv = bcm_port_cable_diag(unit, port, &cd);
        soc_cm_debug(DK_PORT, "CableChecker length = %dM\n ", cd.pair_len[0]);
        if ((rv == BCM_E_NONE) && 
            (cd.pair_state[0] == BCM_PORT_CABLE_STATE_OK) &&
            (cd.pair_len[0] < 45)){

            /* 1. Switch to Green Mode */
            bcm_robo_port_phy_power_mode_set(unit, port,
                PHY_GREEN_POWER_MODE);

            sal_usleep(10);     
            (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x1, &tmp);
            if (tmp & 0x4) {
                link_up = 1;
            } else {
                link_up = 0;
            }
            /* 2. Switch to low power mode */
            if (!link_up) {
                bcm_robo_port_phy_power_mode_set(unit, port,
                    PHY_LOW_POWER_AB0_MODE);

                sal_usleep(10);
                
                (DRV_SERVICES(unit)->miim_read)
                    (unit, phy_addr, 0x1, &tmp);
                if (tmp & 0x4) {
                    link_up = 1;
                } else {
                    link_up = 0;
                }

                /* 3. Switch to High Performance (Class A) Mode */
                if (!link_up) {
                    bcm_robo_port_phy_power_mode_set(unit, port,
                        PHY_HIGH_PERFORMANCE_MODE);
                }
                
            }
            
        } else {
            bcm_robo_port_phy_power_mode_set(unit, port,
                PHY_HIGH_PERFORMANCE_MODE);
        }
        bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW);
        bcm_port_discard_set(unit, port, discard_mode);
    } else {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

#ifdef LVL7_FIXUP
int bcm_robo_port_led_blink_set(int unit, int enable)
{
  uint32 led_map0_regaddr, led_map1_regaddr, led_refr_regaddr;
  uint32 led_map0_reglen, led_map1_reglen, led_refr_reglen;
  uint32 data;
 
  if (!SOC_UNIT_VALID(unit)) 
    return BCM_E_UNIT; 

  led_map0_regaddr = (DRV_SERVICES(unit)->reg_addr)(unit, LED_MODE_MAP_0r, 0, 0);
  led_map1_regaddr = (DRV_SERVICES(unit)->reg_addr)(unit, LED_MODE_MAP_1r, 0, 0);
  led_refr_regaddr = (DRV_SERVICES(unit)->reg_addr)(unit, LED_REFLSH_CTLr, 0, 0);
  
  led_map0_reglen  = (DRV_SERVICES(unit)->reg_length_get)(unit, LED_MODE_MAP_0r);
  led_map1_reglen  = (DRV_SERVICES(unit)->reg_length_get)(unit, LED_MODE_MAP_1r);
  led_refr_reglen  = (DRV_SERVICES(unit)->reg_length_get)(unit, LED_REFLSH_CTLr);
  
  if (enable)
  {
    data = 0x01E0;
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->reg_write(unit,led_map0_regaddr, &data, led_map0_reglen));    
    data = 0x01FF;
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->reg_write(unit,led_map1_regaddr, &data, led_map1_reglen));   
    data = 0x87; /* Blinking rate 6Hz */  
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->reg_write(unit,led_refr_regaddr, &data, led_refr_reglen));   
  }
  else
  {
    data = 0x01FF;
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->reg_write(unit,led_map0_regaddr, &data, led_map0_reglen));    
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->reg_write(unit,led_map1_regaddr, &data, led_map1_reglen));    
    data = 0x83; /* Blinking rate 12Hz */
    BCM_IF_ERROR_RETURN(DRV_SERVICES(unit)->reg_write(unit,led_refr_regaddr, &data, led_refr_reglen));   
  }
 
  return BCM_E_NONE; 
}
#endif
/*
 * Function    : _bcm_robo_gport_resolve
 * Description : Internal function to get modid, port, and trunk_id
 *               from a bcm_gport_t (global port)
 * Parameters  : (IN)  unit      - BCM device number
 *               (IN)  gport     - Global port identifier
 *               (OUT) modid     - Module ID
 *               (OUT) port      - Port number
 *               (OUT) trunk_id  - Trunk ID
 *               (OUT) id        - HW ID
 * Returns     : BCM_E_XXX
 * Notes       : The modid and port are translated from the
 *               application space to local modid/port space
 */
int 
_bcm_robo_gport_resolve(int unit, bcm_gport_t gport,
                       bcm_module_t *modid, bcm_port_t *port, 
                       bcm_trunk_t *trunk_id, int *id)
{
    int             local_id, rv = BCM_E_NONE;
    bcm_module_t    mod_in, local_modid;
    bcm_port_t      port_in, local_port;
    bcm_trunk_t     local_tgid;
    
    local_modid = -1;
    local_port = -1;
    local_id = -1;
    local_tgid = BCM_TRUNK_INVALID;

    if (SOC_GPORT_IS_LOCAL(gport)) {
        BCM_IF_ERROR_RETURN 
            (bcm_stk_my_modid_get(unit, &local_modid));
        local_port = SOC_GPORT_LOCAL_GET(gport);

        if (!SOC_PORT_ADDRESSABLE(unit, local_port)) {
            return BCM_E_PORT;
        }
    } else if (SOC_GPORT_IS_MODPORT(gport)) {
        mod_in = SOC_GPORT_MODPORT_MODID_GET(gport);
        port_in = SOC_GPORT_MODPORT_PORT_GET(gport);
        if ((NUM_MODID(unit) == 2) && (port_in > 31)) {
            return BCM_E_PORT;
        }
        rv = bcm_robo_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                    mod_in, port_in, &local_modid, &local_port);

        if (!SOC_MODID_ADDRESSABLE(unit, local_modid)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, local_port)) {
            return BCM_E_PORT;
        }
    } else if ((gport != BCM_GPORT_INVALID) && 
             (gport != BCM_GPORT_TYPE_BLACK_HOLE)) {
        return BCM_E_BADID;
    }
    *modid = local_modid;
    *port = local_port;
    *trunk_id = local_tgid;
    *id = local_id; 
    return (rv);
}

/*
 * Function    : _bcm_robo_gport_construct
 * Description : Internal function to construct a gport from 
 *                given parameters 
 * Parameters  : (IN)  unit       - BCM device number
 *               (IN)  gport_dest - Structure that contains destination
 *                                   to encode into a gport
 *               (OUT) gport      - Global port identifier
 * Returns     : BCM_E_XXX
 * Notes       : The modid and port are translated from the
 *               local modid/port space to application space
 */
int 
_bcm_robo_gport_construct(int unit, _bcm_gport_dest_t *gport_dest, bcm_gport_t *gport)
{

    bcm_gport_t     l_gport = 0;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    
    if ((NULL == gport_dest) || (NULL == gport) ){
        return BCM_E_PARAM;
    }

    switch (gport_dest->gport_type) {
        case _SHR_GPORT_TYPE_LOCAL:
            SOC_GPORT_LOCAL_SET(l_gport, gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_MODPORT:
            BCM_IF_ERROR_RETURN (
                _bcm_robo_gport_modport_hw2api_map(unit, gport_dest->modid, 
                                              gport_dest->port, &mod_out,
                                              &port_out));
            SOC_GPORT_MODPORT_SET(l_gport, mod_out, port_out);
            break;
        default:    
            return BCM_E_PARAM;
    }

    *gport = l_gport;
    return BCM_E_NONE;
}

