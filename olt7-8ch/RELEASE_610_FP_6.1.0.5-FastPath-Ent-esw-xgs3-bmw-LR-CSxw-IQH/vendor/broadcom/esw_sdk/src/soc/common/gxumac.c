/*
 * $Id: gxumac.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 * XGS 1/2.5/10/12/13/16G Media Access Controller Driver (BigMAC+UniMAC combo)
 *
 * This module is used for:
 *
 *   - GXPORTs on Scorpion family
 *   - GXPORTs and XGPORTs(some mode) on Triumph family
 *
 * The general idea is to have this "pseudo" driver act like a wrapper to
 * BigMAC driver and UniMAC driver. Depends on the nature of individual driver
 * funciton, some functions will call corresponding function in both BigMAC and
 * UniMAC driver, some just call the the function in the active MAC driver.
 *
 * Use _xgmac_mode_get to determine which driver is currently active.
 * Should not make decision upon the link speed from XGXS, it is only accurate
 * when link is up, however some driver function may be called regardless of
 * the link status.
 */

#include <soc/drv.h>
#include <soc/debug.h>
#ifdef BCM_TRIUMPH_SUPPORT
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
#include <soc/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_BIGMAC_SUPPORT
#ifdef BCM_UNIMAC_SUPPORT

/*
 * Forward Declarations
 */
mac_driver_t soc_mac_big_uni;

extern mac_driver_t soc_mac_uni;  /* UniMAC */
extern mac_driver_t soc_mac_big;  /* BigMAC */

/*
 * Function:
 *     _xgmac_select_mode
 * Purpose:
 *     Select current MAC
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 *     mode - new MAC mode (UniMAC or BigMAC).
 * Returns:
 *     SOC_E_NONE
 */
STATIC int
_xgmac_select_mac(int unit, soc_port_t port, soc_mac_mode_t mode)
{
    uint32 rval;

    if (SOC_DEBUG_CHECK(SOC_DBG_10G | SOC_DBG_VERBOSE) ||
        SOC_DEBUG_CHECK(SOC_DBG_GE | SOC_DBG_VERBOSE)) {
        soc_cm_print("_xgmac_select_mac: unit %d port %s mode %s\n",
                     unit, SOC_PORT_NAME(unit, port),
                     mode == SOC_MAC_MODE_10000 ? "10G" : "GE");
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (IS_XG_PORT(unit, port)) {
        return soc_triumph_xgport_mode_change(unit, port, mode);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf,
                      mode == SOC_MAC_MODE_10000 ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));

#ifdef BCM_TRIUMPH2_SUPPORT
    if (IS_XQ_PORT(unit, port)) {
#ifdef BCM_ENDURO_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            return soc_en_xqport_mode_change(unit, port, mode);
        } else 
#endif
        {
            return soc_tr2_xqport_mode_change(unit, port, mode);
        }
    } else 
#endif
    {
        SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, XPORT_MODE_BITSf,
                          mode == SOC_MAC_MODE_10000 ? 1 : 2);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
    }
    return SOC_E_NONE;
}

STATIC int
_xgmac_mode_get(int unit, soc_port_t port, soc_mac_mode_t *mode)
{
    uint32 rval;
    int bits;

    if (IS_XG_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_XGPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XGPORT_MODE_REGr, rval,
                                 XGPORT_MODE_BITSf);
        if (bits == 2) { /* xport-mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 1) { /* gport-mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled or other modes */
            return SOC_E_FAIL;
        }
    } else if (IS_XQ_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(READ_XQPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XQPORT_MODE_REGr, rval,
                                 XQPORT_MODE_BITSf);
        if (bits == 2) { /* xport-mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 1) { /* gport-mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled or other modes */
            return SOC_E_FAIL;
        }
    } else {
        SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
        bits = soc_reg_field_get(unit, XPORT_MODE_REGr, rval,
                                 XPORT_MODE_BITSf);
        if (bits == 1) { /* xport-mode */
            *mode = SOC_MAC_MODE_10000;
        } else if (bits == 2) { /* gport-mode */
            *mode = SOC_MAC_MODE_1000_T;
        } else { /* disabled */
            return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_init
 * Purpose:
 *      Initialize BigMAC and UniMAC into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *
 */
STATIC int
xgmac_init(int unit, soc_port_t port)
{
    soc_mac_mode_t mode;

    /*
     * Initialize UniMAC
     */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_init(unit, port));

    /*
     * Initialize BigMAC
     */
    SOC_IF_ERROR_RETURN(soc_mac_big.md_init(unit, port));

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        _xgmac_select_mac(unit, port, SOC_MAC_MODE_10000);
    } else {
        _xgmac_select_mac(unit, port, SOC_MAC_MODE_1000_T);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_enable_set(int unit, soc_port_t port, int enable)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_enable_set(unit, port, enable);
    } else {
        return soc_mac_uni.md_enable_set(unit, port, enable);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_enable_get(int unit, soc_port_t port, int *enable)
{
    int umac_enable, bmac_enable;

    /* Instead of checking the current mode we read from all MACs */

    /* UniMAC */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_enable_get(unit, port, &umac_enable));

    /* BigMAC */
    SOC_IF_ERROR_RETURN(soc_mac_big.md_enable_get(unit, port, &bmac_enable));

    *enable = umac_enable | bmac_enable;

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_duplex_set
 * Purpose:
 *      Set  xgmac in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
xgmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    if (IS_GE_PORT(unit, port)) {
        return soc_mac_uni.md_duplex_set(unit, port, duplex);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_duplex_get
 * Purpose:
 *      Get xgmac duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_duplex_get(unit, port, duplex);
    } else {
        return soc_mac_uni.md_duplex_get(unit, port, duplex);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_speed_set
 * Purpose:
 *      Set xgmac speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 1000, 2500, ... 16000.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
xgmac_speed_set(int unit, soc_port_t port, int speed)
{
    int enable;
    soc_mac_mode_t mode, new_mode;
    int rv;

    if (IS_HG_PORT(unit, port)) {
        if (speed && speed < 10000) {
            return SOC_E_PARAM;
        }
        if (SOC_INFO(unit).port_speed_max[port] > 0 &&
            speed > SOC_INFO(unit).port_speed_max[port]) {
            return SOC_E_PARAM;
        }
    } else {
        if (speed  && (speed < 0 || speed > 10000)) {
            return SOC_E_PARAM;
        }
    }
    if (speed == 0) {
        return SOC_E_NONE; /* Transient state */
    }

    SOC_IF_ERROR_RETURN(xgmac_enable_get(unit, port, &enable));
    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    new_mode = speed < 10000 ? SOC_MAC_MODE_1000_T : SOC_MAC_MODE_10000;

    /* Disable MACs before doing anything else */
    rv = xgmac_enable_set(unit, port, 0);

    if (SOC_SUCCESS(rv)) {
        if (speed <= 2500) {
            rv = soc_mac_uni.md_speed_set(unit, port, speed);
        } else {
            rv = soc_mac_big.md_speed_set(unit, port, speed);
        }
    }

    if (SOC_SUCCESS(rv)) {
        if (mode != new_mode) {
            rv = _xgmac_select_mac(unit, port, new_mode);
        }
    }

    /* Re-enable MAC */
    if (SOC_SUCCESS(rv)) {
        rv = xgmac_enable_set(unit, port, enable);
    }

    return rv;
}

/*
 * Function:
 *      xgmac_speed_get
 * Purpose:
 *      Get xgmac speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mbps
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_speed_get(unit, port, speed);
    } else {
        return soc_mac_uni.md_speed_get(unit, port, speed);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_pause_set
 * Purpose:
 *      Configure xgmac to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    if (!IS_HG_PORT(unit, port)) { /* Avoid stacking pause conflict */
        /* UniMAC */
        SOC_IF_ERROR_RETURN(soc_mac_uni.md_pause_set(unit, port, pause_tx,
                                                     pause_rx));
    }

    /* BigMAC */
    SOC_IF_ERROR_RETURN(soc_mac_big.md_pause_set(unit, port, pause_tx,
                                                 pause_rx));

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_pause_get
 * Purpose:
 *      Return the pause ability of xgmac
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause
 *      pause_rx - Boolean: receive pause
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    /* use value for BigMAC */
    return soc_mac_big.md_pause_get(unit, port, pause_tx, pause_rx);
}

/*
 * Function:
 *      xgmac_pause_addr_set
 * Purpose:
 *      Configure PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    /* UniMAC */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_pause_addr_set(unit, port, mac));

    /* BigMAC */
    SOC_IF_ERROR_RETURN(soc_mac_big.md_pause_addr_set(unit, port, mac));

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_pause_addr_get
 * Purpose:
 *      Retrieve PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    /* use value for BigMAC */
    return soc_mac_big.md_pause_addr_get(unit, port, mac);
}

/*
 * Function:
 *      xgmac_loopback_set
 * Purpose:
 *      Set a xgmac into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_loopback_set(int unit, soc_port_t port, int lb)
{
    /* UniMAC */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_lb_set(unit, port, lb));

    /* BigMAC */
    SOC_IF_ERROR_RETURN(soc_mac_big.md_lb_set(unit, port, lb));

#if 0 
    /* Power lanes up/down? */
    soc_unicore16g_lane_power_set(unit, port, lb ? 0 : 0xf);
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_loopback_get
 * Purpose:
 *      Get current xgmac loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_loopback_get(int unit, soc_port_t port, int *lb)
{
    /* use value for BigMAC */
    return soc_mac_big.md_lb_get(unit, port, lb);
}

/*
 * Function:
 *      xgmac_interface_set
 * Purpose:
 *      Set a xgmac interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *
 */
STATIC int
xgmac_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    return SOC_E_NONE;
}


/*
 * Function:
 *      xgmac_interface_get
 * Purpose:
 *      Retrieve xgmac interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
xgmac_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_MII;
    return SOC_E_NONE;
}


/*
 * Function:
 *      xgmac_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
xgmac_frame_max_set(int unit, soc_port_t port, int size)
{
    /* UniMAC */
    SOC_IF_ERROR_RETURN(soc_mac_uni.md_frame_max_set(unit, port, size));

    /* BigMAC */
    SOC_IF_ERROR_RETURN(soc_mac_big.md_frame_max_set(unit, port, size));

    return SOC_E_NONE;
}

/*
 * Function:
 *      xgmac_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
xgmac_frame_max_get(int unit, soc_port_t port, int *size)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_frame_max_get(unit, port, size);
    } else {
        return soc_mac_uni.md_frame_max_get(unit, port, size);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - the IFG value
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
xgmac_ifg_set(int unit, soc_port_t port, int speed, soc_port_duplex_t duplex,
              int ifg)
{
    if (speed < 10000) {
        /* UniMAC */
        return soc_mac_uni.md_ifg_set(unit, port, speed, duplex, ifg);
    } else {
        /* BigMAC */
        return soc_mac_big.md_ifg_set(unit, port, speed, duplex, ifg);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_ifg_get
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg    - the IFG value
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
xgmac_ifg_get(int unit, soc_port_t port, int speed, soc_port_duplex_t duplex,
              int *ifg)
{
    if (speed < 10000) {
        /* UniMAC */
        return soc_mac_uni.md_ifg_get(unit, port, speed, duplex, ifg);
    } else {
        /* BigMAC */
        return soc_mac_big.md_ifg_get(unit, port, speed, duplex, ifg);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_encap_set
 * Purpose:
 *      Set the xgmac port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - (IN) encap type.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_encap_set(int unit, soc_port_t port, int type)
{
    return soc_mac_big.md_encap_set(unit, port, type);
}

/*
 * Function:
 *      xgmac_encap_get
 * Purpose:
 *      Get the xgmac port encapsulation type.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - encap type.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_encap_get(int unit, soc_port_t port, int *type)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_encap_get(unit, port, type);
    } else {
        return soc_mac_uni.md_encap_get(unit, port, type);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_control_set
 * Purpose:
 *      To configure MAC control properties.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_control_set(unit, port, type, value);
    } else {
        return soc_mac_uni.md_control_set(unit, port, type, value);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_control_get
 * Purpose:
 *      To get current MAC control setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
xgmac_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  int *value)
{
    soc_mac_mode_t mode;

    SOC_IF_ERROR_RETURN(_xgmac_mode_get(unit, port, &mode));
    if (mode == SOC_MAC_MODE_10000) {
        return soc_mac_big.md_control_get(unit, port, type, value);
    } else {
        return soc_mac_uni.md_control_get(unit, port, type, value);
    }

    return SOC_E_INTERNAL;
}

/*
 * Function:
 *      xgmac_ability_local_get
 * Purpose:
 *      Return the abilities of xgmac
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC  int
xgmac_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    soc_port_ability_t umac_ability, bmac_ability;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    sal_memset(&umac_ability, 0, sizeof(umac_ability));
    sal_memset(&bmac_ability, 0, sizeof(bmac_ability));

    if (!IS_HG_PORT(unit, port)) {
        /* UniMAC */
        SOC_IF_ERROR_RETURN
            (soc_mac_uni.md_ability_local_get(unit, port, &umac_ability));
    }

    /* BigMAC */
    SOC_IF_ERROR_RETURN
        (soc_mac_big.md_ability_local_get(unit, port, &bmac_ability));

    ability->speed_half_duplex  = umac_ability.speed_half_duplex | bmac_ability.speed_half_duplex;
    ability->speed_full_duplex  = umac_ability.speed_full_duplex | bmac_ability.speed_full_duplex;
    ability->pause     = umac_ability.pause | bmac_ability.pause;
    ability->interface = umac_ability.interface | bmac_ability.interface;
    ability->medium    = umac_ability.medium | bmac_ability.medium;
    ability->loopback  = umac_ability.loopback | bmac_ability.loopback;
    ability->flags     = umac_ability.flags | bmac_ability.flags;

    return (SOC_E_NONE);
}

/* Exported BigMAC/UniMAC combo MAC driver structure */
mac_driver_t soc_mac_big_uni = {
    "16G/13G/12G/10G/2.5G/1G MAC Driver",  /* drv_name */
    xgmac_init,                    /* md_init  */
    xgmac_enable_set,              /* md_enable_set */
    xgmac_enable_get,              /* md_enable_get */
    xgmac_duplex_set,              /* md_duplex_set */
    xgmac_duplex_get,              /* md_duplex_get */
    xgmac_speed_set,               /* md_speed_set */
    xgmac_speed_get,               /* md_speed_get */
    xgmac_pause_set,               /* md_pause_set */
    xgmac_pause_get,               /* md_pause_get */
    xgmac_pause_addr_set,          /* md_pause_addr_set */
    xgmac_pause_addr_get,          /* md_pause_addr_get */
    xgmac_loopback_set,            /* md_lb_set */
    xgmac_loopback_get,            /* md_lb_get */
    xgmac_interface_set,           /* md_interface_set */
    xgmac_interface_get,           /* md_interface_get */
    NULL,                          /* md_ability_get - Deprecated */
    xgmac_frame_max_set,           /* md_frame_max_set */
    xgmac_frame_max_get,           /* md_frame_max_get */
    xgmac_ifg_set,                 /* md_ifg_set */
    xgmac_ifg_get,                 /* md_ifg_get */
    xgmac_encap_set,               /* md_encap_set */
    xgmac_encap_get,               /* md_encap_get */
    xgmac_control_set,             /* md_control_set */
    xgmac_control_get,             /* md_control_get */
    xgmac_ability_local_get        /* md_ability_local_get */
};

#endif /* BCM_UNIMAC_SUPPORT */
#endif /* BCM_BIGMAC_SUPPORT */
