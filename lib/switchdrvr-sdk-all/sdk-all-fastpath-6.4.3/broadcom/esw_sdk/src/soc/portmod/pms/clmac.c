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
 *     
 *
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/clmac.h>

#if defined(PORTMOD_PM4X25_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

/**
 * 
 * @brief init clmac with default configuration
 * @param unit 
 * @param instance 
 * 
 * @return int 
 */

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
    int is_strip_crc, is_append, is_replace, is_higig;
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    is_strip_crc =          (init_flags & CLAMC_INIT_F_RX_STRIP_CRC         ? 1 : 0);
    is_append =             (init_flags & CLAMC_INIT_F_TX_APPEND_CRC        ? 1 : 0);
    is_replace =            (init_flags & CLAMC_INIT_F_TX_REPLACE_CRC       ? 1 : 0);
    is_higig =              (init_flags & CLAMC_INIT_F_IS_HIGIG             ? 1 : 0);
    is_ipg_check_disable =  (init_flags & CLAMC_INIT_F_IPG_CHECK_DISABLE    ? 1 : 0);

    if(is_append && is_replace) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("CLAMC_INIT_F_TX_APPEND_CRC and CLAMC_INIT_F_TX_REPLACE_CRC can't co-exist")));
    }

    /* RX Control */
    _SOC_IF_ERR_EXIT(READ_CLMAC_RX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRIP_CRCf, is_strip_crc);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRICT_PREAMBLEf, is_higig ? 0 : 1);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, 64);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_RX_CTRLr(unit, port, reg_val));

    /* TX Control */
    crc_mode = CLMAC_CRC_MODE_REPLACE; /* Replace CRC is the default */
    if(is_append) {
        crc_mode = CLMAC_CRC_MODE_APPEND;
    }
    _SOC_IF_ERR_EXIT(READ_CLMAC_TX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, CRC_MODEf, crc_mode);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, is_higig ? CLMAC_AVERAGE_IPG_HIGIG : CLMAC_AVERAGE_IPG_DEFAULT);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, TX_PREAMBLE_LENGTHf, CLMAC_TX_PREAMBLE_LENGTH);
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_TX_CTRLr(unit, port, reg_val));

    /*Pause*/
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    pause_control.tx_enable = 0;
    pause_control.rx_enable = 1;
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
        case portmodLoopbackMacCore:
            _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
            *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, LOCAL_LPBKf);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unsupported loopback type %d"), lb));
    }
exit:
    SOC_FUNC_RETURN;
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
        fld_val = soc_reg64_field32_get(unit, CLMAC_MODEr, reg_val, NO_SOP_FOR_CRC_HGf);
        if(fld_val) {
            (*flags) |= CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG;
        }
    }

    if(*encap == SOC_ENCAP_HIGIG2){
        _SOC_IF_ERR_EXIT(READ_CLMAC_CTRLr(unit, port, &reg_val));
        fld_val = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, EXTENDED_HIG2_ENf);
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

int clmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));
    
    _SOC_IF_ERR_EXIT(WRITE_CLMAC_TX_MAC_SAr(unit, port, mac_addr));
   
exit:
    SOC_FUNC_RETURN;
}

/******************************************************************************* 
 Remote/local Fault
********************************************************************************/

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

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, REMOTE_FAULT_STATUSf);
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

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, LOCAL_FAULT_STATUSf);
    *status = field_val ? 1: 0;

exit:
    SOC_FUNC_RETURN;
}

/******************************************************************************* 
 Flow Control
********************************************************************************/


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
        if(control->refresh_timer > 0){
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


int clmac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control)
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
    control->xoff_timer = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PAUSE_XOFF_TIMERf);
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


#undef _ERR_MSG_MODULE_NAME


#endif
