/*
 *         
 * $Id:$
 * 
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
 */

#include <soc/portmod/xlmac.h>

#if defined(PORTMOD_PM4X10_SUPPORT)

#include <soc/portmod/portmod.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define JUMBO_MAXSZ  0x3fe8 /* Max legal value (per regsfile) */

#define XLMAC_AVERAGE_IPG_DEFAULT         12
#define XLMAC_AVERAGE_IPG_HIGIG           8

/*
 * Functions to Review for Coverage:
 * xlmac_loopback_set
 * xlmac_encap_set
 *
 * Fucntions to be added (may be)
 * mac_xl_egress_queue_drain
 * mac_xl_control_set
 * mac_xl_control_get
 * mac_xl_ability_local_get
 */

struct {
    int speed;
    uint32 clock_rate;
}_xlmac_clock_rate[] = {
    { 40000, 312 },
    { 20000, 156 },
    { 10000, 78  },
    { 5000,  78  },
    { 2500,  312 },
    { 1000,  125 },
    { 0,     25  },
};

void
_xlmac_speed_to_clock_rate(int unit, soc_port_t port, int speed,
                            uint32 *clock_rate)
{
    int idx;


    for (idx = 0;
         idx < sizeof(_xlmac_clock_rate) / sizeof(_xlmac_clock_rate[0]);
         idx++) {
        if (speed >=_xlmac_clock_rate[idx].speed) {
            *clock_rate = _xlmac_clock_rate[idx].clock_rate;
            return;
        }
    }
    *clock_rate = 0;
}


STATIC int
_xlmac_drain_cells(int unit, soc_port_t port, int notify_phy)
{
#if 0
    int            pause_tx, pause_rx, pfc_rx, llfc_rx;
    uint32         values[2], fval;
    uint64         mac_ctrl, rval64;
    soc_field_t    fields[2];
    soc_timeout_t  to;

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    /* Disable pause/pfc function */
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_set(unit, port, pause_tx, 0));

    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_get(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   &pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   0));

    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_get(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   &llfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   0));

    /* Assert SOFT_RESET before DISCARD just in case there is no credit left */
    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 1);

    /* Drain data in TX FIFO without egressing at packet boundary */
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));
    fields[0] = DISCARDf;
    values[0] = 1;
    fields[1] = EP_DISCARDf;
    values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, XLMAC_TX_CTRLr, port, 2,
                                                fields, values));

    /* De-assert SOFT_RESET to let the drain start */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

    /* Wait until TX fifo cell count is 0 */
    soc_timeout_init(&to, 250000, 0);
    for (;;) {
        SOC_IF_ERROR_RETURN(READ_XLMAC_TXFIFO_CELL_CNTr(unit, port, &rval64));
        fval = soc_reg64_field32_get(unit, XLMAC_TXFIFO_CELL_CNTr, rval64,
                                     CELL_CNTf);
        if (fval == 0) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_XLMAC,
                      (BSL_META_U(unit,
                                  "ERROR: port %d:%s: "
                                  "timeout draining TX FIFO (%d cells remain)\n"),
                       unit, SOC_PORT_NAME(unit, port), fval));
            return SOC_E_INTERNAL;
        }
    }

    /* Stop TX FIFO drainging */
    values[0] = 0;
    values[1] = 0;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, XLMAC_TX_CTRLr, port, 2,
                                                fields, values));

    /* Restore original pause/pfc/llfc configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_set(unit, port, pause_tx, pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   llfc_rx));

#endif
    return SOC_E_NONE;
}


int xlmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint64 reg_val, orig_reg_val;
    uint32 soft_reset;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    orig_reg_val = reg_val;

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, TX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, RX_ENf, enable?1:0);

    /* write only if value changed */
    if(COMPILER_64_NE(reg_val, orig_reg_val)) { 
        soft_reset = 
           soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, SOFT_RESETf);

        if (enable || (!enable && soft_reset)) {
            return (SOC_E_NONE);
        }
    }

    if (!enable) {
        SOC_IF_ERROR_RETURN(_xlmac_drain_cells(unit, port, 1));
    }

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                           SOFT_RESETf, enable ? 0 : 1);
    return(WRITE_XLMAC_CTRLr(unit, port, reg_val));
}

int xlmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, RX_ENf);

    return (SOC_E_NONE);
}


int xlmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                           SOFT_RESETf, enable? 1 : 0);
    return (WRITE_XLMAC_CTRLr(unit, port, reg_val));
}


int xlmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, SOFT_RESETf)?1:0;
    return (SOC_E_NONE);
}


/*
 * Function:
 *      _xlmac_timestamp_delay_set
 * Purpose:
 *      Set Timestamp delay for one-step to account for lane and pipeline delay.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      speed - Speed
 *      phy_mode - single/dual/quad phy mode
 * Returns:
 *      SOC_E_XXX
 */
#if 0
STATIC int
_xlmac_timestamp_delay_set(int unit, soc_port_t port, int speed)
{
    uint64 ctrl;
    uint32 clk_rate;
#ifdef BCM_TIMESYNC_SUPPORT
    uint32 tx_clk_ns;
    int    divisor;
#endif /* BCM_TIMESYNC_SUPPORT */

    SOC_IF_ERROR_RETURN(READ_XLMAC_TIMESTAMP_ADJUSTr(unit, port, &ctrl));

    _xlmac_speed_to_clock_rate(unit, port, speed, &clk_rate);

#ifdef BCM_TIMESYNC_SUPPORT
    /* Tx clock rate for single/dual/quad phy mode. Rate in nano seconds */
    if ((speed >= 5000) && (speed <= 40000)) {
        divisor   = speed > 20000 ? 1 : speed > 10000 ? 2 : 4;
        tx_clk_ns = ((1000 / clk_rate) / divisor); /* tx clock rate in ns */
    } else {
        tx_clk_ns = 1000 / clk_rate;   
    }
#endif /* BCM_TIMESYNC_SUPPORT */
    
    /* 
     * MAC pipeline delay for XGMII/XGMII mode is:
     *          = (5.5 * TX line clock period) + (Timestamp clock period)
     */
    /* signed value of pipeline delay in ns */
    soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                   TS_OSTS_ADJUSTf, 
                   (SOC_TIMESYNC_PLL_CLOCK_NS(unit) - ((11 * tx_clk_ns ) / 2)));

    return(WRITE_XLMAC_TIMESTAMP_ADJUSTr(unit, port, ctrl));
}
#endif

int xlmac_speed_set(int unit, soc_port_t port, int speed)
{
    uint32 spd_fld;
    int    enable;
    uint64 rval;

    if (speed >= 10000)       /* 10G Speed */
        spd_fld = 4; 
    else if (speed >= 2500)   /* 2.5G Speed */
        spd_fld = 3; 
    else if (speed >= 1000)   /* 1G Speed */
        spd_fld = 2; 
    else if (speed >= 100)    /* 100M Speed */
        spd_fld = 1; 
    else
        spd_fld = 0;          /* 10M Speed */ 

    SOC_IF_ERROR_RETURN(xlmac_enable_get(unit, port, 0, &enable));

    /* disable before updating the speed */ 
    if (enable) {
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, 0, 0));
    }

    /* Update the speed */
    SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit ,port, &rval));
    soc_reg64_field32_set(unit, XLMAC_MODEr, &rval, SPEED_MODEf, spd_fld);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_MODEr(unit ,port, rval));

#if 0
    soc_reg_field32_modify (unit, XLMAC_RX_CTRLr, port, STRICT_PREAMBLEf, 
                            (speed > 10000)&& IS_XE_PORT(unit, port)? 1 : 0); 
#endif

#ifdef WHY_IT_IS_REALTED
    uint32 values[2]
    static soc_field_t fields[2] = { LOCAL_FAULT_DISABLEf, 
                             REMOTE_FAULT_DISABLEf };
    values[0] = values[1] = speed < 5000 ? 1 : 0;
    soc_reg_fields32_modify(unit, XLMAC_RX_LSS_CTRLr, port, 2, fields, values);
#endif

    if (enable) {
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, 0, 1));
    }

#if 0
    return (_xlmac_timestamp_delay_set(unit, port, speed));
#endif

    return SOC_E_NONE;
}


int xlmac_loopback_set (int unit, soc_port_t port, 
                        portmod_loopback_mode_t lb, int enable)
{
    uint64 reg_val;

    switch(lb){

        case portmodLoopbackMacCore:
             SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr (unit, port, &reg_val));

             soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                                    LOCAL_LPBKf, enable ? 1: 0);
             SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, reg_val));
             break;

        default:
             break;
    }

    return(SOC_E_NONE);
}

int xlmac_loopback_get (int unit, soc_port_t port, 
                        portmod_loopback_mode_t lb, int *enable)
{
    uint64 reg_val;

    switch(lb){

        case portmodLoopbackMacCore:
        case portmodLoopbackMacOuter:
        case portmodLoopbackMacPCS:
        case portmodLoopbackMacAsyncFifo:
             SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

             *enable = soc_reg64_field32_get (unit, XLMAC_CTRLr, 
                                              reg_val, LOCAL_LPBKf);
             break;

        /* For these the code should not come here.., */
        case portmodLoopbackPhyGloopPCS:
        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPCS:
        case portmodLoopbackPhyRloopPMD:
             *enable = 0;
             break;

        default:
             break;
    }

    return(SOC_E_NONE);
}

/*
 * Function:
 *      _mac_xl_port_mode_update
 * Purpose:
 *      Set the XLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      hg_mode - SOC HG encap modes. value={HG/HG2/HG2-LITE}
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_xlmac_port_mode_update(int unit, soc_port_t port, int hg_mode)
{
    /* CHECK CHECK FIX FIX -- NEED TO UPDATE THIS */
    return (0);
}


int xlmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap)
{
    uint64 reg_val;
    uint32 encap_val= 0, no_sop_for_crc_hg = 0, ext_hi2=0;
    int    enable;

    
    switch(encap){
        case SOC_ENCAP_IEEE:
             encap_val = 0;
             break;

        case SOC_ENCAP_HIGIG:
             encap_val = 1;
             no_sop_for_crc_hg = 
                (flags & XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG)?1:0;
             break;

        case SOC_ENCAP_HIGIG2:
        case SOC_ENCAP_HIGIG2_LITE:
             encap_val = 2;
             no_sop_for_crc_hg = 
                (flags & XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG)?1:0;
             ext_hi2 = (flags & XLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN) ? 1 : 0;
             break;

        case SOC_ENCAP_SOP_ONLY:
             encap_val = 5;
             break;

        default:
             LOG_ERROR(BSL_LS_SOC_XLMAC,
                      (BSL_META_U(unit, "illegal encap mode %d"), encap));
             return (SOC_E_PARAM); 
             break;
    }

    SOC_IF_ERROR_RETURN(xlmac_enable_get(unit, port, 0, &enable));
    
    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, 0, 0));
    }

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        /* Greyhound need mode update for all encap mode change! */
        SOC_IF_ERROR_RETURN(_xlmac_port_mode_update(unit, port, encap));

        /* GH specific : for HG2-LITE is sepecial case in PortMacro(PM).
         * - encap mode in PM must be IEEE (out of PM must be HG2-LITE)
         */
        encap_val = (encap == SOC_ENCAP_HIGIG2_LITE) ? 0 : encap_val;
    } else
#endif
    if ((IS_E_PORT(unit, port) && encap != SOC_ENCAP_IEEE) ||
            (IS_ST_PORT(unit, port) && encap == SOC_ENCAP_IEEE)) {

        SOC_IF_ERROR_RETURN(_xlmac_port_mode_update(unit, port, encap));
    }                       
                 
    SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_MODEr, &reg_val, HDR_MODEf, encap_val);
    soc_reg64_field32_set (unit, XLMAC_MODEr, &reg_val, NO_SOP_FOR_CRC_HGf, 
                           no_sop_for_crc_hg);

    SOC_IF_ERROR_RETURN(WRITE_XLMAC_MODEr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit ,port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &reg_val, STRICT_PREAMBLEf, encap == SOC_ENCAP_IEEE ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_CTRLr(unit ,port, reg_val));

    /* Enable HiGig2 Mode */
    if(encap == SOC_ENCAP_HIGIG2) {
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

        soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                               EXTENDED_HIG2_ENf, ext_hi2); 
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, reg_val));
    }

    if (enable) {
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, 0, 1));
    }

    return (SOC_E_NONE);
}

int xlmac_encap_get(int unit, soc_port_t port, portmod_encap_t *encap)
{
    uint64 reg_val; 
    uint32 fld_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit, port, &reg_val));
    fld_val = soc_reg64_field32_get(unit, XLMAC_MODEr, reg_val, HDR_MODEf);

    switch(fld_val){
        case 0:
             *encap = SOC_ENCAP_IEEE;
             break;

        case 1:
             *encap = SOC_ENCAP_HIGIG;
             break;

        case 2:
             *encap = SOC_ENCAP_HIGIG2;
             break;

        case 5:
             *encap = SOC_ENCAP_SOP_ONLY;
             break;

        default:
             return(SOC_E_PARAM);
             break;
    }

    return (SOC_E_NONE);
}


int xlmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAX_SIZEr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_RX_MAX_SIZEr, &reg_val, 
                           RX_MAX_SIZEf, value);
    return(WRITE_XLMAC_RX_MAX_SIZEr(unit, port, reg_val));
}

int xlmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAX_SIZEr(unit, port, &reg_val));
    *value = (int)soc_reg64_field32_get (unit, XLMAC_RX_MAX_SIZEr, 
                                         reg_val, RX_MAX_SIZEf);
    return (SOC_E_NONE);
}

int xlmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    if (value > 96) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                     "runt size should be small than 96. got %d"), value));
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &reg_val, 
                           RUNT_THRESHOLDf, value);

    return(WRITE_XLMAC_RX_CTRLr(unit, port, reg_val));
}

int xlmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &reg_val));
    *value = (int)soc_reg64_field32_get (unit, XLMAC_RX_CTRLr, 
                                         reg_val, RUNT_THRESHOLDf);
    return (SOC_E_NONE);
}


int xlmac_rx_vlan_tag_set (int unit, soc_port_t port, 
                           int outer_vlan_tag, int inner_vlan_tag)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_VLAN_TAGr(unit, port, &reg_val));

    if(inner_vlan_tag == -1) {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               INNER_VLAN_TAG_ENABLEf, 0);
    } else {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               INNER_VLAN_TAGf, inner_vlan_tag);
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               INNER_VLAN_TAG_ENABLEf, 1);
    }

    if(outer_vlan_tag == -1) {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               OUTER_VLAN_TAG_ENABLEf, 0);
    } else {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               OUTER_VLAN_TAGf, outer_vlan_tag);
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               OUTER_VLAN_TAG_ENABLEf, 1);
    }

    return (WRITE_XLMAC_RX_VLAN_TAGr(unit, port, reg_val));
}


int xlmac_rx_vlan_tag_get (int unit, soc_port_t port, 
                           int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint64 reg_val;
    uint32 enable = 0;

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_VLAN_TAGr(unit, port, &reg_val));

    enable = soc_reg64_field32_get (unit, XLMAC_RX_VLAN_TAGr, reg_val, 
                                    INNER_VLAN_TAG_ENABLEf);

    *inner_vlan_tag = enable ? 
                         soc_reg64_field32_get (unit, XLMAC_RX_VLAN_TAGr, 
                                                reg_val, INNER_VLAN_TAGf) : -1;

    enable = soc_reg64_field32_get (unit, XLMAC_RX_VLAN_TAGr, reg_val, 
                                    OUTER_VLAN_TAG_ENABLEf);

    *outer_vlan_tag = enable ? soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, 
                                    reg_val, OUTER_VLAN_TAGf) : -1;
    return (SOC_E_NONE);
}


int xlmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;

    COMPILER_64_ZERO(mac_addr);

    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));

    return (WRITE_XLMAC_RX_MAC_SAr(unit, port, mac_addr));
}


int xlmac_sw_link_status_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                           LINK_STATUS_SELECTf, enable);

    return(WRITE_XLMAC_CTRLr(unit, port, reg_val));
}

int xlmac_sw_link_status_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    *enable = (int)soc_reg64_field32_get (unit, XLMAC_CTRLr, reg_val, 
                                          LINK_STATUS_SELECTf);
    return (SOC_E_NONE);
}

int xlmac_sw_link_status_set(int unit, soc_port_t port, int link)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, SW_LINK_STATUSf, link);
    return (WRITE_XLMAC_CTRLr(unit, port, reg_val));
}



int xlmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;

    COMPILER_64_ZERO(mac_addr);
    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));

    return (WRITE_XLMAC_TX_MAC_SAr(unit, port, mac_addr));
}


int xlmac_tx_averge_ipg_set(int unit, soc_port_t port, int val)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, val);

    return (WRITE_XLMAC_TX_CTRLr(unit, port, reg_val));
}


int xlmac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint64 reg_val;

    if(length > 8){
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                       "runt size should be small than 8. got %d"), length));
        return (SOC_E_PARAM);
    }

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_TX_CTRLr, &reg_val, 
                           TX_PREAMBLE_LENGTHf, length);
    return (WRITE_XLMAC_TX_CTRLr(unit, port, reg_val));
}

/***************************************************************************** 
 * Remote/local Fault                                                        *
 *****************************************************************************/

int xlmac_remote_fault_control_set (int unit, soc_port_t port, 
                    const portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
 
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
        REMOTE_FAULT_DISABLEf, control->enable ? 0 : 1); 
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
        DROP_TX_DATA_ON_REMOTE_FAULTf, control->drop_tx_on_fault ? 1: 0);

    return(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, reg_val));
}


int xlmac_remote_fault_control_get(int unit, soc_port_t port, 
                     portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    control->enable = soc_reg64_field32_get (unit, XLMAC_RX_LSS_CTRLr, 
                                  reg_val, REMOTE_FAULT_DISABLEf)?1:0;
    control->drop_tx_on_fault = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr,
                                  reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf)?1:0;
    return (SOC_E_NONE);
}


int xlmac_local_fault_control_set (int unit, soc_port_t port, 
                 const portmod_local_fault_control_t *control)
{
    uint64 reg_val;
 
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
            LOCAL_FAULT_DISABLEf, control->enable ? 0 : 1);    

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
            DROP_TX_DATA_ON_LOCAL_FAULTf, control->drop_tx_on_fault ? 1: 0); 

    return (WRITE_XLMAC_RX_LSS_CTRLr(unit, port, reg_val));
}


int xlmac_local_fault_control_get(int unit, soc_port_t port, 
                      portmod_local_fault_control_t *control)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    control->enable = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, 
                                  reg_val, LOCAL_FAULT_DISABLEf)? 1 : 0;
    control->drop_tx_on_fault  = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr,
                                  reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf)?1:0;
    return (SOC_E_NONE);
}


int xlmac_remote_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_STATUSr(unit, port, &reg_val));
    *status = soc_reg64_field32_get (unit, XLMAC_RX_LSS_CTRLr, reg_val, 
                                     REMOTE_FAULT_STATUSf)? 1 : 0;
    return (SOC_E_NONE);
}

int xlmac_local_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_STATUSr(unit, port, &reg_val));

    *status = soc_reg64_field32_get (unit, XLMAC_RX_LSS_CTRLr, reg_val, 
                                     LOCAL_FAULT_STATUSf)?1:0;

    return (SOC_E_NONE);
}

/**************************************************************************** 
 Flow Control
*****************************************************************************/

int xlmac_pause_control_set (int unit, soc_port_t port, 
                             const portmod_pause_control_t *control)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_PAUSE_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_REFRESH_TIMERf,control->refresh_timer);
            soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_REFRESH_ENf, 1);
        } else {
            soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_XOFF_TIMERf, control->xoff_timer);
    }

    soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   TX_PAUSE_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   RX_PAUSE_ENf, control->rx_enable);

    return(WRITE_XLMAC_PAUSE_CTRLr(unit, port , reg_val));
}


int xlmac_pfc_control_set (int unit, soc_port_t port, 
                           const portmod_pfc_control_t *control)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                                   PFC_REFRESH_TIMERf, control->refresh_timer);
            soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                                   PFC_REFRESH_ENf, 1);
        } else {
            soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                                   PFC_REFRESH_ENf, 0);
        }

        soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                               PFC_STATS_ENf, control->stats_en);
        soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                               PAUSE_XOFF_TIMERf, control->xoff_timer);
        soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                               FORCE_PFC_XONf, control->force_xon);
    }

    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, 
                                TX_PFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, 
                                RX_PFC_ENf, control->rx_enable);

    return(WRITE_XLMAC_PFC_CTRLr(unit, port , reg_val));
}


int xlmac_llfc_control_set (int unit, soc_port_t port, 
                            const portmod_llfc_control_t *control)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_LLFC_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                               LLFC_IN_IPG_ONLYf, control->in_ipg_only);
        soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                               LLFC_CRC_IGNOREf, control->crc_ignore);
    }

    soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                           TX_LLFC_ENf, control->tx_enable);
    soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                           RX_LLFC_ENf, control->rx_enable);

    return(WRITE_XLMAC_LLFC_CTRLr(unit, port , reg_val));
}


int xlmac_pause_control_get (int unit, soc_port_t port, 
                             portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    COMPILER_64_ZERO(reg_val);
    SOC_IF_ERROR_RETURN(READ_XLMAC_PAUSE_CTRLr(unit, port , &reg_val));

    refresh_enable = soc_reg64_field32_get  (unit, XLMAC_PAUSE_CTRLr, 
                                             reg_val, PAUSE_REFRESH_ENf);
    refresh_timer  =  soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                             reg_val, PAUSE_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable) ? refresh_timer : -1;

    control->xoff_timer = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                 reg_val, PAUSE_XOFF_TIMERf);
    control->rx_enable  = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                 reg_val, RX_PAUSE_ENf);
    control->tx_enable  = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                 reg_val, TX_PAUSE_ENf);
    return (SOC_E_NONE);
}


int xlmac_pfc_control_get (int unit, soc_port_t port, 
                           portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port , &reg_val));

    refresh_timer  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                            reg_val, PFC_REFRESH_TIMERf);
    refresh_enable = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                            reg_val, PFC_REFRESH_ENf);

    control->refresh_timer = (refresh_enable) ? refresh_timer : -1;

    control->stats_en   = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, PFC_STATS_ENf);
    control->xoff_timer = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, PAUSE_XOFF_TIMERf);
    control->force_xon  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, FORCE_PFC_XONf);
    control->rx_enable  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, TX_PFC_ENf);
    control->tx_enable  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, RX_PFC_ENf);
    return (SOC_E_NONE);
}


int xlmac_llfc_control_get (int unit, soc_port_t port, 
                            portmod_llfc_control_t *control)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_LLFC_CTRLr(unit, port , &reg_val));

    control->in_ipg_only = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, LLFC_IN_IPG_ONLYf);
    control->crc_ignore  = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, LLFC_CRC_IGNOREf);
    control->rx_enable   = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, TX_LLFC_ENf);
    control->tx_enable   = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, RX_LLFC_ENf);
    return (SOC_E_NONE);
}


/*
 * Function:
 *      xlmac_duplex_set
 * Purpose:
 *      Set XLMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
xlmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}


/*
 * Function:
 *      xlmac_duplex_get
 * Purpose:
 *      Get XLMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
int
xlmac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/**
 * 
 * @brief init xlmac with default configuration
 * @param unit 
 * @param instance 
 * 
 * @return int 
 */
int xlmac_init(int unit, soc_port_t port, uint32_t init_flags)
{
    uint64 mac_ctrl, tx_ctrl, rx_ctrl, rval;
    int    is_strip_crc, is_append, is_replace, is_higig;
    portmod_pause_control_t control;

    is_strip_crc         =init_flags & XLMAC_INIT_F_RX_STRIP_CRC     ? 1 : 0;
    is_append            =init_flags & XLMAC_INIT_F_TX_APPEND_CRC    ? 1 : 0;
    is_replace           =init_flags & XLMAC_INIT_F_TX_REPLACE_CRC   ? 1 : 0;
    is_higig             =init_flags & XLMAC_INIT_F_IS_HIGIG         ? 1 : 0;

    if(is_append && is_replace) {
        LOG_VERBOSE(BSL_LS_SOC_10G,
                    (BSL_META_U(unit, "XLAMC_INIT_F_TX_APPEND_CRC & XLAMC_INIT_F_TX_REPLACE_CRC can't co-exist")));
        return (SOC_E_PARAM);
    }

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, 
                                XGMII_IPG_CHECK_DISABLEf,
                                IS_HG_PORT(unit, port) ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

    /* XLMAC_RX_CTRL */
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &rx_ctrl));

    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 
                           is_strip_crc);
    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rx_ctrl, STRICT_PREAMBLEf, 
                           is_higig ? 1 : 0);
    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf, 64);

    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_CTRLr(unit, port, rx_ctrl));

    /* XLMAC_TX_CTRL */
    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &tx_ctrl));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf, 2);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, is_higig ? XLMAC_AVERAGE_IPG_HIGIG : XLMAC_AVERAGE_IPG_DEFAULT);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, tx_ctrl));

    /* PAUSE */
    if (IS_ST_PORT(unit, port)) {
        control.tx_enable = FALSE;
        control.rx_enable = FALSE;
    } else {
        control.tx_enable = TRUE;
        control.rx_enable = TRUE;
    }

    /* Use the existing timer values for now */
    SOC_IF_ERROR_RETURN(READ_XLMAC_PAUSE_CTRLr(unit, port , &rval));

    control.refresh_timer = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                rval, PAUSE_REFRESH_TIMERf);
    control.xoff_timer = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                rval, PAUSE_XOFF_TIMERf);
    SOC_IF_ERROR_RETURN(xlmac_pause_control_set(unit, port, &control));

    /* PFC */
    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit ,port, &rval));
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &rval, PFC_REFRESH_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_CTRLr(unit ,port, rval));

    /* Set jumbo max size (8000 byte payload) */
    SOC_IF_ERROR_RETURN(xlmac_rx_max_size_set(unit, port, JUMBO_MAXSZ));

    /* XLMAC_RX_LSS_CTRL */
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LOCAL_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_REMOTE_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LINK_INTERRUPTf, 1);    
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, rval));

    /* Disable loopback and bring XLMAC out of reset */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 1);

    return(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));
}

/*! 
 * xlmac_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int xlmac_frame_spacing_stretch_set(int unit, int port, int spacing)
{
    uint64 mac_ctrl;

    if (spacing < 0 || spacing > 255) {
        return SOC_E_PARAM;
    } 

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));

    if (spacing >= 8) {
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, spacing);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 1);
    } else {
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 0);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 0);
    }

    return (WRITE_XLMAC_TX_CTRLr(unit, port, mac_ctrl));
}

/*! 
 * xlmac_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  spacing         -
 */ 
int xlmac_frame_spacing_stretch_get(int unit, int port, int *spacing)
{                                               
    uint64 mac_ctrl;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));

    *spacing = soc_reg64_field32_get (unit, XLMAC_TX_CTRLr, 
                                      mac_ctrl, THROT_DENOMf);
    return (SOC_E_NONE);
}

/*! 
 * xlmac_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int xlmac_diag_fifo_status_get (int unit, int port, 
                                const portmod_fifo_status_t* diag_info)
{
    uint64 rval;
    portmod_fifo_status_t* pinfo =(portmod_fifo_status_t*)diag_info;

    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));

    if (soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                  ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));

    pinfo->timestamps_in_fifo = soc_reg64_field32_get(unit, 
                     XLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);

    return (SOC_E_NONE);
}

/*! 
 * xlmac_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int xlmac_pfc_config_set (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 fval; 
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_TYPEr(unit ,port, &rval));
    soc_reg64_field32_set(unit, XLMAC_PFC_TYPEr, &rval, PFC_ETH_TYPEf, pfc_cfg->type);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_TYPEr(unit ,port, rval));

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_OPCODEr(unit ,port, &rval));
    soc_reg64_field32_set(unit, XLMAC_PFC_OPCODEr, &rval, PFC_OPCODEf, pfc_cfg->opcode);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_OPCODEr(unit ,port, rval));

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &rval));

    /* DA OUI */
    fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
    fval |= (((pfc_cfg->da_oui & 0xff) << 24) | (pfc_cfg->da_nonoui & 0xFFFFFF));
    soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

    soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                              pfc_cfg->da_oui >> 8);

    return (WRITE_XLMAC_PFC_DAr(unit, port, rval));
}



/*! 
 * xlmac_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int xlmac_pfc_config_get (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint64 rval;
    uint32 fval;
    portmod_pfc_config_t* pcfg = (portmod_pfc_config_t*) pfc_cfg;

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_TYPEr(unit, port, &rval));

    pcfg->type = soc_reg64_field32_get (unit, XLMAC_PFC_TYPEr, 
                                          rval, PFC_ETH_TYPEf);

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_OPCODEr(unit, port, &rval));
    pcfg->opcode = soc_reg64_field32_get (unit, XLMAC_PFC_OPCODEr, 
                                            rval, PFC_OPCODEf);

    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &rval));

    fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
    pcfg->da_oui    = fval >> 24;
    pcfg->da_nonoui = fval & 0xFFFFFF;

    return (SOC_E_NONE);
}


/*! 
 * xlmac_eee_set
 *
 * @brief EEE Control and Timer set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int xlmac_eee_set(int unit, int port, const portmod_eee_t* eee)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_CTRLr(unit ,port, &rval));
    soc_reg64_field32_set(unit, XLMAC_EEE_CTRLr, &rval, EEE_ENf, eee->enable);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_EEE_CTRLr(unit ,port, rval));

    SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_TIMERSr(unit ,port, &rval));
    soc_reg64_field32_set(unit, XLMAC_EEE_TIMERSr, &rval, EEE_DELAY_ENTRY_TIMERf, eee->tx_idle_time);
    soc_reg64_field32_set(unit, XLMAC_EEE_TIMERSr, &rval, EEE_WAKE_TIMERf, eee->tx_wake_time);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_EEE_TIMERSr(unit ,port, rval));

    return SOC_E_NONE;
}


/*! 
 * xlmac_eee_get
 *
 * @brief EEE Control and Timer get
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int xlmac_eee_get(int unit, int port, const portmod_eee_t* eee)
{
    uint64 rval;
    portmod_eee_t* pEEE = (portmod_eee_t*)eee;

    SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_CTRLr(unit, port, &rval));
    pEEE->enable = soc_reg64_field32_get(unit, XLMAC_EEE_CTRLr, rval, EEE_ENf);

    SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_TIMERSr(unit, port, &rval));

    pEEE->tx_idle_time = soc_reg64_field32_get (unit, XLMAC_EEE_TIMERSr, rval, 
                                               EEE_DELAY_ENTRY_TIMERf);
    pEEE->tx_wake_time = soc_reg64_field32_get (unit, XLMAC_EEE_TIMERSr, rval, 
                                               EEE_WAKE_TIMERf);
    return(SOC_E_NONE);
}

#undef _ERR_MSG_MODULE_NAME


#endif /* PORTMOD_PM4X10_SUPPORT */
