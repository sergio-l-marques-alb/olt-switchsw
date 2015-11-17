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

#include <soc/portmod/portmod.h>
#include <soc/portmod/clmac.h>

#if defined(PORTMOD_PM4X25_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT


#define CLMAC_SPEED_100000 0x0
#define CLMAC_CRC_MODE_APPEND   0
#define CLMAC_CRC_MODE_REPLACE  2

#define CLMAC_AVERAGE_IPG_DEFAULT         12
#define CLMAC_AVERAGE_IPG_HIGIG           8

#define CLMAC_TX_PREAMBLE_LENGTH  8

int clmac_init(int unit, soc_port_t port, uint32 init_flags)
{
    portmod_pause_control_t pause_control;
    portmod_pfc_control_t pfc_control;
    portmod_llfc_control_t llfc_control;
    portmod_remote_fault_control_t remote_fault_control;
    portmod_local_fault_control_t local_fault_control;
    int crc_mode, is_ipg_check_disable;
    int is_strip_crc, is_append_crc, is_replace_crc, is_higig, is_pass_through_crc;
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    is_strip_crc =          (init_flags & CLMAC_INIT_F_RX_STRIP_CRC                 ? 1 : 0);
    is_append_crc =         (init_flags & CLMAC_INIT_F_TX_APPEND_CRC                ? 1 : 0);
    is_replace_crc =        (init_flags & CLMAC_INIT_F_TX_REPLACE_CRC               ? 1 : 0);
    is_pass_through_crc =   (init_flags & CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE     ? 1 : 0);
    is_higig =              (init_flags & CLMAC_INIT_F_IS_HIGIG                     ? 1 : 0);
    is_ipg_check_disable =  (init_flags & CLMAC_INIT_F_IPG_CHECK_DISABLE            ? 1 : 0);

    if(is_append_crc + is_replace_crc + is_pass_through_crc > 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("CLAMC_INIT_F_TX_APPEND_CRC, CLAMC_INIT_F_TX_REPLACE_CRC, and CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE can't co-exist")));
    }

    /* RX Control */
    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRIP_CRCf, is_strip_crc);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRICT_PREAMBLEf, is_higig ? 0 : 1);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, 64);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_CTRLr(unit, port, reg_val));

    /* TX Control */
    if(is_append_crc) {
        crc_mode = 0;
    } else if(is_append_crc) {
        crc_mode = 2;
    } else if(is_pass_through_crc) {
        crc_mode = 1;
    } else { /* CRC AUTO Mode */
        crc_mode = 3;
    }
    _SOC_IF_ERR_EXIT(READ_CLMAC_TX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, CRC_MODEf, crc_mode);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, is_higig ? CLMAC_AVERAGE_IPG_HIGIG : CLMAC_AVERAGE_IPG_DEFAULT);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, TX_PREAMBLE_LENGTHf, CLMAC_TX_PREAMBLE_LENGTH);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_TX_CTRLr(unit, port, reg_val));

    /*Pause*/
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    /* PAUSE */
    pause_control.tx_enable = TRUE;
    pause_control.rx_enable = TRUE;
    _SOC_IF_ERR_EXIT(clmac_pause_control_set(unit, port, &pause_control));
    
    /*PFC*/
    _SOC_IF_ERR_EXIT(portmod_pfc_control_t_init(unit, &pfc_control));
    pfc_control.rx_enable = 1;
    pfc_control.tx_enable = 0;
    pfc_control.stats_en = 1;
    _SOC_IF_ERR_EXIT(clmac_pfc_control_set(unit, port, &pfc_control));

    /*LLFC*/
    _SOC_IF_ERR_EXIT(portmod_llfc_control_t_init(unit, &llfc_control));
    llfc_control.rx_enable = 0;
    llfc_control.tx_enable = 0;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_set(unit, port, &llfc_control));

    /*LSS*/
    _SOC_IF_ERR_EXIT(portmod_remote_fault_control_t_init(unit, &remote_fault_control));
    remote_fault_control.enable = 1;
    remote_fault_control.drop_tx_on_fault = 1;
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_set(unit, port, &remote_fault_control));

    _SOC_IF_ERR_EXIT(portmod_local_fault_control_t_init(unit, &local_fault_control));
    local_fault_control.enable = 1;
    local_fault_control.drop_tx_on_fault = 1;
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_set(unit, port, &local_fault_control));

    /*MAC control*/
    _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, RX_ENf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, SOFT_RESETf, 1); /* exit init in disable state*/
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, XGMII_IPG_CHECK_DISABLEf, is_ipg_check_disable);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint64 reg_val, orig_reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
    orig_reg_val = reg_val;

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, RX_ENf, enable ? 1 :0 );

    if(COMPILER_64_NE(reg_val, orig_reg_val)) { /* write only if value changed */
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CTRLr(unit, port, reg_val));
    }

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, SOFT_RESETf, enable ? 0 : 1); 
    if(COMPILER_64_NE(reg_val, orig_reg_val)) { /* write only if value changed */
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CTRLr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, RX_ENf);

exit:
    SOC_FUNC_RETURN;
}

int clmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val, 
                           SOFT_RESETf, enable? 1 : 0);
    return (WRITE_CLMAC_CTRLr(unit, port, reg_val));
}


int clmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, SOFT_RESETf)?1:0;
    return (SOC_E_NONE);
}

int clmac_speed_set(int unit, soc_port_t port, int speed)
{
    uint64 reg_val, orig_reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(orig_reg_val);

    _SOC_IF_ERR_EXIT(READ_CLMAC_MODEr(unit, port, &reg_val));
    COMPILER_64_ADD_64(orig_reg_val, reg_val);
    soc_reg64_field32_set(unit, CLMAC_MODEr, &reg_val, SPEED_MODEf, speed>= 10000 ? 4 : 1);

    if(COMPILER_64_NE(orig_reg_val, reg_val)) {
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_MODEr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb, int enable)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    switch(lb){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, LOCAL_LPBKf, enable ? 1: 0);
            _SOC_IF_ERR_EXIT(WRITE_CLMAC_CTRLr(unit, port, reg_val));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unsupported loopback type %d"), lb));
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb, int *enable)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    switch(lb){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
            *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, LOCAL_LPBKf);
            break;
        default:
            /* set to 0 - this loopback is not in use for this MAC */
            *enable = 0;
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_rx_enable_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr (unit, port, &reg_val));

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val,
                           RX_ENf, enable ? 1: 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, reg_val));

    return(SOC_E_NONE);
}

int clmac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get (unit, CLMAC_CTRLr, reg_val,
                                     RX_ENf);

    return(SOC_E_NONE);
}

int clmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap)
{
    uint32 val = 0;
    uint64 reg_val;
    uint32 no_sop_for_crc_hg = 0;
    SOC_INIT_FUNC_DEFS;

    switch(encap){
        case SOC_ENCAP_IEEE:
            val = 0;
            break;
        case SOC_ENCAP_HIGIG:
            val = 1;
            break;
        case SOC_ENCAP_HIGIG2:
            val = 2;
            break;
        case SOC_ENCAP_SOP_ONLY:
            val = 5;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal encap mode %d"), encap));
    }

    if((encap == SOC_ENCAP_HIGIG) || (encap == SOC_ENCAP_HIGIG2)){
        if(flags & CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG){
            no_sop_for_crc_hg = 1;
        }
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_MODEr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_MODEr, &reg_val, HDR_MODEf, val);
    soc_reg64_field32_set(unit, CLMAC_MODEr, &reg_val, NO_SOP_FOR_CRC_HGf, no_sop_for_crc_hg);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_MODEr(unit, port, reg_val));

    if(encap == SOC_ENCAP_HIGIG2){
        _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, EXTENDED_HIG2_ENf, (flags & CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN) ? 1 : 0 );
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CTRLr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_encap_get(int unit, soc_port_t port, int *flags, portmod_encap_t *encap){
    uint64 reg_val;
    uint32 fld_val;
    SOC_INIT_FUNC_DEFS;

    (*flags) = 0;

    _SOC_IF_ERR_EXIT(READ_CLMAC_MODEr(unit, port, &reg_val));
    fld_val = soc_reg64_field32_get(unit, CLMAC_MODEr, reg_val, HDR_MODEf);

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
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unknown encap mode %d"), fld_val));
    }

    if((*encap == SOC_ENCAP_HIGIG) || (*encap == SOC_ENCAP_HIGIG2)){
        fld_val = soc_reg64_field32_get(unit, CLMAC_MODEr, reg_val,
                                        NO_SOP_FOR_CRC_HGf);
        if(fld_val) {
            (*flags) |= CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG;
        }
    }

    if(*encap == SOC_ENCAP_HIGIG2){
        _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
        fld_val = soc_reg64_field32_get(unit, CLMAC_CTRLr,
                                        reg_val, EXTENDED_HIG2_ENf);
        if(fld_val) {
            (*flags) |= CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_MAX_SIZEr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_RX_MAX_SIZEr, &reg_val, RX_MAX_SIZEf, value);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_MAX_SIZEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_MAX_SIZEr(unit, port, &reg_val));
    *value = soc_reg64_field32_get(unit, CLMAC_RX_MAX_SIZEr, reg_val, RX_MAX_SIZEf);

exit:
    SOC_FUNC_RETURN;
}

int clmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    if(value > 96){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("runt size should be smaller than 96. got %d"), value));
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, value);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_CTRLr(unit, port, &reg_val));
    *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr, reg_val, RUNT_THRESHOLDf);

exit:
    SOC_FUNC_RETURN;
}

int clmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_VLAN_TAGr(unit, port, &reg_val));

    if(inner_vlan_tag == -1){
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAG_ENABLEf, 0);
    } else{
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAGf, inner_vlan_tag);
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAG_ENABLEf, 1);

    }

    if(outer_vlan_tag == -1){
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAG_ENABLEf, 0);
    } else {
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAGf, outer_vlan_tag);
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAG_ENABLEf, 1);
    }

    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_VLAN_TAGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int clmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_VLAN_TAGr(unit, port, &reg_val));
    field_val = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, INNER_VLAN_TAG_ENABLEf);

    if(field_val == 0){
        *inner_vlan_tag = -1;
    } else {
        *inner_vlan_tag = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, INNER_VLAN_TAGf);
    }

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, OUTER_VLAN_TAG_ENABLEf);
    if(field_val == 0){
        *outer_vlan_tag = -1;
    } else {
        *outer_vlan_tag = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, OUTER_VLAN_TAGf);
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));
    
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_MAC_SAr(unit, port, mac_addr));

exit:
    SOC_FUNC_RETURN;
}


int clmac_sw_link_status_select_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val, 
                           LINK_STATUS_SELECTf, enable);

    return(WRITE_CLMAC_CTRLr(unit, port, reg_val));
}

int clmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    *enable = (int)soc_reg64_field32_get (unit, CLMAC_CTRLr, reg_val, 
                                          LINK_STATUS_SELECTf);
    return (SOC_E_NONE);
}

int clmac_sw_link_status_set(int unit, soc_port_t port, int link)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, SW_LINK_STATUSf, link);
    return (WRITE_CLMAC_CTRLr(unit, port, reg_val));
}
int clmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 values[2];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_MAC_SAr(unit, port, &rval64));
    field64 = soc_reg64_field_get(unit, CLMAC_RX_MAC_SAr, rval64, RX_SAf);

    values[0] = COMPILER_64_HI(field64);
    values[1] = COMPILER_64_LO(field64);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

exit:
    SOC_FUNC_RETURN;
}


int clmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));
    
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_TX_MAC_SAr(unit, port, mac_addr));
   
exit:
    SOC_FUNC_RETURN;
}

int clmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_TX_MAC_SAr(unit, port, &rval64));
    values[0] = soc_reg64_field32_get(unit, CLMAC_TX_MAC_SAr, rval64, SA_HIf);
    values[1] = soc_reg64_field32_get(unit, CLMAC_TX_MAC_SAr, rval64, SA_LOf);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

exit:
    SOC_FUNC_RETURN;
}

int clmac_tx_average_ipg_set(int unit, soc_port_t port, int val)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    _SOC_IF_ERR_EXIT(READ_CLMAC_TX_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, val);

    _SOC_IF_ERR_EXIT(WRITE_CLMAC_TX_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN
}

int clmac_tx_average_ipg_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    _SOC_IF_ERR_EXIT(READ_CLMAC_TX_CTRLr(unit, port, &reg_val));

    *val = soc_reg64_field32_get(unit, CLMAC_TX_CTRLr, reg_val, AVERAGE_IPGf);

exit:
    SOC_FUNC_RETURN
}

int clmac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint64 reg_val;

    if(length > 8){
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                       "runt size should be small than 8. got %d"), length));
        return (SOC_E_PARAM);
    }

    COMPILER_64_ZERO(reg_val);

    SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set (unit, CLMAC_TX_CTRLr, &reg_val, 
                           TX_PREAMBLE_LENGTHf, length);
    return (WRITE_CLMAC_TX_CTRLr(unit, port, reg_val));
}

/***************************************************************************** 
 * Remote/local Fault                                                        *
 *****************************************************************************/

int clmac_remote_fault_control_set(int unit, soc_port_t port, const portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_LSS_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, REMOTE_FAULT_DISABLEf, control->enable ? 0 : 1 /*flip*/);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf, control->drop_tx_on_fault ? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_LSS_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_remote_fault_control_get(int unit, soc_port_t port, portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, REMOTE_FAULT_DISABLEf);
    control->enable = (field_val ? 0 : 1);/*flip*/

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf);
    control->drop_tx_on_fault  = (field_val ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}

int clmac_local_fault_control_set(int unit, soc_port_t port, const portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_LSS_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, LOCAL_FAULT_DISABLEf, control->enable ? 0 : 1 /*flip*/);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf, control->drop_tx_on_fault ? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_LSS_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int clmac_local_fault_control_get(int unit, soc_port_t port, portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, LOCAL_FAULT_DISABLEf);
    control->enable = (field_val ? 0 : 1);/*flip*/

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf);
    control->drop_tx_on_fault  = (field_val ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}

int clmac_remote_fault_status_get(int unit, soc_port_t port, int clear_status, int *status)
{
    uint64 reg_val, reg_val_clear;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_clear);
    if(clear_status) {
        soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val_clear, CLEAR_REMOTE_FAULT_STATUSf, 1);
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_LSS_STATUSr(unit, port, &reg_val));

    if(clear_status) {
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, reg_val_clear));
        COMPILER_64_ZERO(reg_val_clear);
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, reg_val_clear));
    }

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_STATUSr, reg_val, REMOTE_FAULT_STATUSf);
    *status = field_val ? 1: 0;

exit:
    SOC_FUNC_RETURN;
}

int clmac_local_fault_status_get(int unit, soc_port_t port, int clear_status, int *status)
{
    uint64 reg_val, reg_val_clear;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_clear);
    if(clear_status) {
        soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val_clear, CLEAR_LOCAL_FAULT_STATUSf, 1);
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_LSS_STATUSr(unit, port, &reg_val));

    if(clear_status) {
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, reg_val_clear));
        COMPILER_64_ZERO(reg_val_clear);
        _SOC_IF_ERR_EXIT(WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, reg_val_clear));
    }

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_STATUSr, reg_val, LOCAL_FAULT_STATUSf);
    *status = field_val ? 1: 0;

exit:
    SOC_FUNC_RETURN;
}

int clmac_clear_rx_lss_status_set(int unit, soc_port_t port, int lcl_fault, int rmt_fault)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val, 
                                     CLEAR_REMOTE_FAULT_STATUSf, rmt_fault);
    soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val, 
                                     CLEAR_LOCAL_FAULT_STATUSf, lcl_fault);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, reg_val));

    return (SOC_E_NONE);
}

int clmac_clear_rx_lss_status_get(int unit, soc_port_t port, int *lcl_fault, int *rmt_fault)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CLEAR_RX_LSS_STATUSr(unit, port, &reg_val));
    *rmt_fault = soc_reg64_field32_get(unit, CLMAC_CLEAR_RX_LSS_STATUSr, reg_val, 
                                     CLEAR_REMOTE_FAULT_STATUSf);
    *lcl_fault = soc_reg64_field32_get(unit, CLMAC_CLEAR_RX_LSS_STATUSr, reg_val, 
                                     CLEAR_LOCAL_FAULT_STATUSf);

    return (SOC_E_NONE);
}

/**************************************************************************** 
 Flow Control
*****************************************************************************/

int clmac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_PAUSE_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_TIMERf, control->refresh_timer);
            soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_ENf, 1);
        }else{
            soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_XOFF_TIMERf, control->xoff_timer);
    }

    soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, TX_PAUSE_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, RX_PAUSE_ENf, control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CLMAC_PAUSE_CTRLr(unit, port , reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_PFC_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer){
            soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_TIMERf, control->refresh_timer);
            soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_ENf, 1);
        }else{
            soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_STATS_ENf, control->stats_en);
        soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_XOFF_TIMERf, control->xoff_timer);
        soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, FORCE_PFC_XONf, control->force_xon);
    }

    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, TX_PFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, RX_PFC_ENf, control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CLMAC_PFC_CTRLr(unit, port , reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_llfc_control_set(int unit, soc_port_t port, const portmod_llfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_LLFC_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, LLFC_IN_IPG_ONLYf, control->in_ipg_only);
        soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, LLFC_CRC_IGNOREf, control->crc_ignore);
    }
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, TX_LLFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, RX_LLFC_ENf, control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CLMAC_LLFC_CTRLr(unit, port , reg_val));

exit:
    SOC_FUNC_RETURN;
}

int clmac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_PAUSE_CTRLr(unit, port , &reg_val));

    refresh_enable = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, PAUSE_REFRESH_ENf);
    refresh_timer =  soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, PAUSE_REFRESH_TIMERf);
    control->refresh_timer = (refresh_enable ? refresh_timer : -1);
    control->xoff_timer = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, PAUSE_XOFF_TIMERf);
    control->rx_enable = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, RX_PAUSE_ENf);
    control->tx_enable = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, TX_PAUSE_ENf);

exit:
    SOC_FUNC_RETURN;
}

int clmac_pfc_control_get (int unit, soc_port_t port,
                           portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_PFC_CTRLr(unit, port , &reg_val));

    refresh_timer = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_REFRESH_TIMERf);
    refresh_enable = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_REFRESH_ENf);
    control->refresh_timer = (refresh_enable ? refresh_timer : -1);
    control->stats_en = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_STATS_ENf);
    control->xoff_timer = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_XOFF_TIMERf);
    control->force_xon = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, FORCE_PFC_XONf);
    control->rx_enable = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, TX_PFC_ENf);
    control->tx_enable = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, RX_PFC_ENf);

exit:
    SOC_FUNC_RETURN;
}

int clmac_llfc_control_get(int unit, soc_port_t port, portmod_llfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLMAC_LLFC_CTRLr(unit, port , &reg_val));

    control->in_ipg_only = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, LLFC_IN_IPG_ONLYf);
    control->crc_ignore = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, LLFC_CRC_IGNOREf);
    control->rx_enable = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, TX_LLFC_ENf);
    control->tx_enable = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, RX_LLFC_ENf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      clmac_duplex_set
 * Purpose:
 *      Set CLMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
clmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_cl_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}


/*
 * Function:
 *      clmac_duplex_get
 * Purpose:
 *      Get CLMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
int
clmac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_cl_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_speed_get
 * Purpose:
 *      Get CLMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb
 * Returns:
 *      SOC_E_XXX
 */

int
clmac_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, CLMAC_MODEr, rval, SPEED_MODEf)) {
    case CLMAC_SPEED_100000:
    default:
        *speed = 100000;
        break;
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_xl_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}


/*! 
 * clmac_eee_set
 *
 * @brief EEE Control and Timer set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int clmac_eee_set(int unit, int port, const portmod_eee_t* eee)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_CTRLr(unit ,port, &rval));
    soc_reg64_field32_set(unit, CLMAC_EEE_CTRLr, &rval, EEE_ENf, eee->enable);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_EEE_CTRLr(unit ,port, rval));

    SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_TIMERSr(unit ,port, &rval));
    soc_reg64_field32_set(unit, CLMAC_EEE_TIMERSr, &rval, EEE_DELAY_ENTRY_TIMERf, eee->tx_idle_time);
    soc_reg64_field32_set(unit, CLMAC_EEE_TIMERSr, &rval, EEE_WAKE_TIMERf, eee->tx_wake_time);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_EEE_TIMERSr(unit ,port, rval));

    return SOC_E_NONE;
}


/*! 
 * clmac_eee_get
 *
 * @brief EEE Control and Timer get
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int clmac_eee_get(int unit, int port, portmod_eee_t* eee)
{
    uint64 rval;
    portmod_eee_t* pEEE = (portmod_eee_t*)eee;

    SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_CTRLr(unit, port, &rval));
    pEEE->enable = soc_reg64_field32_get(unit, CLMAC_EEE_CTRLr, rval, EEE_ENf);

    SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_TIMERSr(unit, port, &rval));

    pEEE->tx_idle_time = soc_reg64_field32_get (unit, CLMAC_EEE_TIMERSr, rval, 
                                               EEE_DELAY_ENTRY_TIMERf);
    pEEE->tx_wake_time = soc_reg64_field32_get (unit, CLMAC_EEE_TIMERSr, rval, 
                                               EEE_WAKE_TIMERf);
    return(SOC_E_NONE);
}

int clmac_pass_control_frame_set(int unit, int port, int value)
{
    uint32 rval32 = 0;
    SOC_IF_ERROR_RETURN(READ_PGW_MAC_RSV_MASKr(unit, port, &rval32));
    /* PGW_MAC_RSV_MASK: Bit 11 Control Frame recieved
     * Enable  Control Frame : Set 0. Packet go through
     * Disable Control Frame : Set 1. Packet is purged.
     */
    if(value) {
        rval32 &= ~(1 << 11);
    } else {
        rval32 |= (1 << 11);
    }
    SOC_IF_ERROR_RETURN(WRITE_PGW_MAC_RSV_MASKr(unit, port, rval32));

    return(SOC_E_NONE);
}


int clmac_lag_failover_loopback_set(int unit, int port, int val)
{
    uint64 rval;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CLMAC_LAG_FAILOVER_STATUSr(unit, port, &rval));

    soc_reg64_field32_set (unit, CLMAC_LAG_FAILOVER_STATUSr, &rval, 
                                            LAG_FAILOVER_LOOPBACKf, val);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_LAG_FAILOVER_STATUSr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int clmac_lag_failover_loopback_get(int unit, int port, int *val)
{
    uint64 rval;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CLMAC_LAG_FAILOVER_STATUSr(unit, port, &rval));

    *val = soc_reg64_field32_get (unit, CLMAC_LAG_FAILOVER_STATUSr, rval, 
                                                  LAG_FAILOVER_LOOPBACKf);

exit:
    SOC_FUNC_RETURN;
}

/*!
 * clmac_lag_failover_disable
 *
 * @brief Port Mac Control Lag Failover Disable. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 */
int clmac_lag_failover_disable(int unit, int port)
{
    uint64 mac_ctrl;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LAG_FAILOVER_ENf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LINK_STATUS_SELECTf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, REMOVE_FAILOVER_LPBKf, 0);

    return (WRITE_CLMAC_TX_CTRLr(unit, port, mac_ctrl));
}   

        
/*!
 * clmac_lag_failover_en_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int clmac_lag_failover_en_set(int unit, int port, int val)
{
    uint64 mac_ctrl;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LAG_FAILOVER_ENf, val);

    return (WRITE_CLMAC_TX_CTRLr(unit, port, mac_ctrl));
}   
        
/*!     
 * clmac_lag_failover_en_get
 *      
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  val         -
 */
int clmac_lag_failover_en_get(int unit, int port, int *val)
{                                 
    uint64 mac_ctrl;
 
    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));
 
    *val = soc_reg64_field32_get (unit, CLMAC_CTRLr,
                                      mac_ctrl, LAG_FAILOVER_ENf);
    return (SOC_E_NONE);
}


/*!
 * clmac_lag_remove_failover_lpbk_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int clmac_lag_remove_failover_lpbk_set(int unit, int port, int val)
{
    uint64 mac_ctrl;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, REMOVE_FAILOVER_LPBKf, val);

    return (WRITE_CLMAC_TX_CTRLr(unit, port, mac_ctrl));
}   
        
/*!     
 * clmac_lag_remove_failover_lpbk_get
 *      
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  val         -
 */
int clmac_lag_remove_failover_lpbk_get(int unit, int port, int *val)
{                                 
    uint64 mac_ctrl;
 
    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));
 
    *val = soc_reg64_field32_get (unit, CLMAC_CTRLr,
                                      mac_ctrl, REMOVE_FAILOVER_LPBKf);
    return (SOC_E_NONE);
}

int clmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val)
{
    return (SOC_E_NONE);
}


int clmac_reset_fc_timers_on_link_dn_get(int unit, soc_port_t port, int *val)
{
    return (SOC_E_NONE);
}


int clmac_drain_cell_get (int unit, int port, int *rx_pfc_en, 
                          int *llfc_en, int *rx_pause, int *tx_pause)
{   
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
    *rx_pfc_en = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, 
                                   rval, RX_PFC_ENf);

    SOC_IF_ERROR_RETURN(READ_CLMAC_LLFC_CTRLr(unit, port, &rval));
    *llfc_en = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, 
                                 rval, RX_LLFC_ENf);

    SOC_IF_ERROR_RETURN(READ_CLMAC_PAUSE_CTRLr(unit, port , &rval));
    *rx_pause  = soc_reg64_field32_get (unit, CLMAC_PAUSE_CTRLr,
                                                 rval, RX_PAUSE_ENf);
    *tx_pause  = soc_reg64_field32_get (unit, CLMAC_PAUSE_CTRLr,
                                                 rval, TX_PAUSE_ENf);

    return (0);
}    

int clmac_drain_cell_stop (int unit, int port, int rx_pfc_en, 
                           int llfc_en, int rx_pause, int tx_pause)
{   
    uint64 rval;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, DISCARDf, 0);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, EP_DISCARDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_TX_CTRLr(unit, port, rval));

    /* set pause fields */ 
    SOC_IF_ERROR_RETURN(READ_CLMAC_PAUSE_CTRLr(unit, port , &rval));
    soc_reg64_field32_set (unit, CLMAC_PAUSE_CTRLr,
                           &rval, RX_PAUSE_ENf, rx_pause);
    soc_reg64_field32_set (unit, CLMAC_PAUSE_CTRLr,
                           &rval, TX_PAUSE_ENf, tx_pause);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_PAUSE_CTRLr(unit, port , rval));

    /* set pfc rx_en fields */ 
    SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, 
                          &rval, RX_PFC_ENf, rx_pfc_en);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_PFC_CTRLr(unit, port, rval));

    /* set llfc rx_en fields */ 
    SOC_IF_ERROR_RETURN(READ_CLMAC_LLFC_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, 
                          &rval, RX_LLFC_ENf, llfc_en);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_LLFC_CTRLr(unit, port, rval));

    return (0);
}

int clmac_drain_cell_start(int unit, int port)
{   
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_PAUSE_CTRLr(unit, port , &rval));
    soc_reg64_field32_set (unit, CLMAC_PAUSE_CTRLr, &rval, RX_PAUSE_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_PAUSE_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &rval, RX_PFC_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_PFC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CLMAC_LLFC_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &rval, RX_LLFC_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_LLFC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, rval));
 
    SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, DISCARDf, 1);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, EP_DISCARDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_TX_CTRLr(unit, port, rval));

    return (0);
}

int
clmac_mac_ctrl_set(int unit, int port, uint64 mac_ctrl)
{
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, mac_ctrl));
    return (0);
}


int clmac_txfifo_cell_cnt_get (int unit, int port, uint32* fval)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_TXFIFO_CELL_CNTr(unit, port, &rval));
    *fval = soc_reg64_field32_get(unit, CLMAC_TXFIFO_CELL_CNTr, rval,
                                     CELL_CNTf);
    return (0);
}

int
clmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl, int *rx_en)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    *mac_ctrl = ctrl;

    *rx_en = soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, RX_ENf);

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, 0);

    /* Disable RX */
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));
    return (0);
}

int
clmac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint64 ctrl;
    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, rx_en ? 1 : 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));

    return (0);
}

int
clmac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint64 ctrl, octrl;
    uint32 soft_reset = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, rx_en ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing senario, XLMAC_CTRLr.SOFT_RESETf is 
         *      used to early check to see if this port is at disabled state 
         *      already.
         */
        soft_reset = soc_reg64_field32_get(unit, 
                                           CLMAC_CTRLr, ctrl, SOFT_RESETf);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (rx_en)
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);

    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));

    return (0);
}

/*
 * Function:
 *      
 * Purpose:
 *     This function checks if the MAC control values are already
 *     set.
 *
 *     This function should be called before enabling/disabling the
 *     MAC to prevent the following:
 *     1. Problem occurred for disabling process only.
 *     2. To comply original designing senario, XLMAC_CTRLr.SOFT_RESETf is 
 *        used to early check to see if this port is at disabled state 
 *        already.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Port number.
 *      enable  - (IN) Indicates whether to check for enable or disable.
 *      reset   - (OUT) Indicates MAC control needs to be reset or not.
 * Returns:
 *      SOC_E_XXX
 */
int clmac_reset_check(int unit, int port, int enable, int *reset)
{
    uint64 ctrl, octrl;

    *reset = 1;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        if (enable) {
            *reset = 0;
        } else {
            if (soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, SOFT_RESETf)) {
                *reset = 0;
            }
        }
    }

    return SOC_E_NONE;
}
#undef _ERR_MSG_MODULE_NAME


#endif
