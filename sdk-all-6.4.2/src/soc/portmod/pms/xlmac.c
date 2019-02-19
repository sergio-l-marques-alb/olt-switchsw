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

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

int xlmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint64 reg_val, orig_reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
    orig_reg_val = reg_val;

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, TX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, RX_ENf, enable?1:0);

    if(COMPILER_64_NE(reg_val, orig_reg_val)) { /* write only if value changed */
        _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));
    }

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, SOFT_RESETf, enable ? 0 : 1);
    if(COMPILER_64_NE(reg_val, orig_reg_val)) { /* write only if value changed */
        _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int xlmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, RX_ENf);

exit:
    SOC_FUNC_RETURN;
}


int xlmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, SOFT_RESETf, enable? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, SOFT_RESETf)?1:0;

exit:
    SOC_FUNC_RETURN;
}


int xlmac_speed_set(int unit, soc_port_t port, int speed)
{
    uint64 reg_val; 
    uint32 spd_fld;
    SOC_INIT_FUNC_DEFS;

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

    _SOC_IF_ERR_EXIT(READ_XLMAC_MODEr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_MODEr, &reg_val, SPEED_MODEf, spd_fld); 
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_MODEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/* CHECK CHECK CHECK : 
   esw/xlmac.c does this code differently. */ 
int xlmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb, int enable)
{

    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    switch(lb){

        case portmodLoopbackMacCore:
             _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
             soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, LOCAL_LPBKf, enable ? 1: 0);
             _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));
             break;

        default:
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unsupported loopback type %d"), lb));
             break;
    }


exit:
    SOC_FUNC_RETURN;
}

int xlmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb, int *enable)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    switch(lb){

        case portmodLoopbackMacCore:
             _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
             *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, LOCAL_LPBKf);
             break;

        default:
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unsupported loopback type %d"), lb));
             break;
    }


exit:
    SOC_FUNC_RETURN;
}

int xlmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap)
{
    uint64 reg_val;
    uint32 val = 0, no_sop_for_crc_hg = 0, ext_hi2=0;

    SOC_INIT_FUNC_DEFS;
    
    switch(encap){
        case SOC_ENCAP_IEEE:
             val = 0;
             break;

        case SOC_ENCAP_HIGIG:
             val = 1;
             no_sop_for_crc_hg = 
                (flags & XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG)?1:0;
             break;

        case SOC_ENCAP_HIGIG2:
             val = 2;
             no_sop_for_crc_hg = 
                (flags & XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG)?1:0;
             ext_hi2 = (flags & XLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN) ? 1 : 0;
             break;

        case SOC_ENCAP_SOP_ONLY:
             val = 5;
             break;

        default:
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal encap mode %d"), encap));
             break;
    }


    _SOC_IF_ERR_EXIT(READ_XLMAC_MODEr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_MODEr, &reg_val, HDR_MODEf, val);
    soc_reg64_field32_set(unit, XLMAC_MODEr, &reg_val, NO_SOP_FOR_CRC_HGf, no_sop_for_crc_hg);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_MODEr(unit, port, reg_val));

    /* Enable HiGig2 Mode */
    if(encap == SOC_ENCAP_HIGIG2){
        _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, EXTENDED_HIG2_ENf, ext_hi2); 
        _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;

}

int xlmac_encap_get(int unit, soc_port_t port, portmod_encap_t *encap)
{
    uint64 reg_val; 
    uint32 fld_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_MODEr(unit, port, &reg_val));
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
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unknown encap mode %d"), fld_val));
             break;
    }

exit:
    SOC_FUNC_RETURN;

}


int xlmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_MAX_SIZEr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_RX_MAX_SIZEr, &reg_val, RX_MAX_SIZEf, value);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_MAX_SIZEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int xlmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_MAX_SIZEr(unit, port, &reg_val));
    *value = (int)soc_reg64_field32_get(unit, XLMAC_RX_MAX_SIZEr, reg_val, RX_MAX_SIZEf);

exit:
    SOC_FUNC_RETURN;
}

int xlmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    if(value > 96){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("runt size should be small than 96. got %d"), value));
    }

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, value);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int xlmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    if(*value > 96){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("runt size should be small than 96. got %d"), *value));
    }

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_CTRLr(unit, port, &reg_val));
    *value = (int)soc_reg64_field32_get(unit, XLMAC_RX_CTRLr, reg_val, RUNT_THRESHOLDf);

exit:
    SOC_FUNC_RETURN;
}


int xlmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_VLAN_TAGr(unit, port, &reg_val));

    if(inner_vlan_tag == -1){
        soc_reg64_field32_set(unit, XLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAG_ENABLEf, 0);
    } else{
        soc_reg64_field32_set(unit, XLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAGf, inner_vlan_tag);
        soc_reg64_field32_set(unit, XLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAG_ENABLEf, 1);
    }

    if(outer_vlan_tag == -1){
        soc_reg64_field32_set(unit, XLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAG_ENABLEf, 0);
    }
    else{
        soc_reg64_field32_set(unit, XLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAGf, outer_vlan_tag);
        soc_reg64_field32_set(unit, XLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAG_ENABLEf, 1);
    }

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_VLAN_TAGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}


int xlmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint64 reg_val;
    uint32 enable = 0;

    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);


    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_VLAN_TAGr(unit, port, &reg_val));

    enable = soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, reg_val, INNER_VLAN_TAG_ENABLEf);
    *inner_vlan_tag = enable? soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, reg_val, INNER_VLAN_TAGf) : -1;

    enable = soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, reg_val, OUTER_VLAN_TAG_ENABLEf);
    *outer_vlan_tag = enable? soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, reg_val, OUTER_VLAN_TAGf) : -1;

exit:
    SOC_FUNC_RETURN;

}


int xlmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(mac_addr);
    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_MAC_SAr(unit, port, mac_addr));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_sw_link_status_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, LINK_STATUS_SELECTf, enable);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int xlmac_sw_link_status_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
    *enable = (int)soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, LINK_STATUS_SELECTf);

exit:
    SOC_FUNC_RETURN;
}

int xlmac_sw_link_status_set(int unit, soc_port_t port, int link)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, SW_LINK_STATUSf, link);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}



int xlmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(mac_addr);
    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[4]), _shr_uint32_read(mac));

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_TX_MAC_SAr(unit, port, mac_addr));
exit:
    SOC_FUNC_RETURN;
}


int xlmac_tx_averge_ipg_set(int unit, soc_port_t port, int val)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    _SOC_IF_ERR_EXIT(READ_XLMAC_TX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, val);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_TX_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;


    if(length > 8){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("runt size should be small than 8. got %d"), length));
    }

    COMPILER_64_ZERO(reg_val);

    _SOC_IF_ERR_EXIT(READ_XLMAC_TX_CTRLr(unit, port, &reg_val));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val, TX_PREAMBLE_LENGTHf, length);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_TX_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/******************************************************************************* 
 * Remote/local Fault                                                          *
 *******************************************************************************/


int xlmac_remote_fault_control_set(int unit, soc_port_t port, const portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, REMOTE_FAULT_DISABLEf, control->enable ? 0 : 1 /*flip*/);   
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf, control->drop_tx_on_fault ? 1: 0);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int xlmac_remote_fault_control_get(int unit, soc_port_t port, portmod_remote_fault_control_t *control)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    control->enable = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, reg_val, REMOTE_FAULT_DISABLEf)?1:0;
    control->drop_tx_on_fault = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf)?1:0;

exit:
    SOC_FUNC_RETURN;
}


int xlmac_local_fault_control_set(int unit, soc_port_t port, const portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, LOCAL_FAULT_DISABLEf, control->enable ? 0 : 1 /*flip*/);    
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf, control->drop_tx_on_fault ? 1: 0); 

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_local_fault_control_get(int unit, soc_port_t port, portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_CTRLr(unit, port, &reg_val));

    control->enable = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, reg_val, LOCAL_FAULT_DISABLEf)?1:0;
    control->drop_tx_on_fault  = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf)?1:0;

exit:
    SOC_FUNC_RETURN;
}


int xlmac_remote_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint64 reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_STATUSr(unit, port, &reg_val));
    *status = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, reg_val, REMOTE_FAULT_STATUSf)?1:0;

exit:
    SOC_FUNC_RETURN;
}

int xlmac_local_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_STATUSr(unit, port, &reg_val));
    *status = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, reg_val, LOCAL_FAULT_STATUSf)?1:0;

exit:
    SOC_FUNC_RETURN;
}

/******************************************************************************* 
 Flow Control
********************************************************************************/

int xlmac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    _SOC_IF_ERR_EXIT(READ_XLMAC_PAUSE_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_TIMERf, control->refresh_timer);
            soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_ENf, 1);
        }else{
            soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, PAUSE_XOFF_TIMERf, control->xoff_timer);
    }
    soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, TX_PAUSE_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, RX_PAUSE_ENf, control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_PAUSE_CTRLr(unit, port , reg_val));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    _SOC_IF_ERR_EXIT(READ_XLMAC_PFC_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_TIMERf, control->refresh_timer);
            soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_ENf, 1);
        }else{
            soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, PFC_STATS_ENf, control->stats_en);
        soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, PAUSE_XOFF_TIMERf, control->xoff_timer);
        soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, FORCE_PFC_XONf, control->force_xon);
    }
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, TX_PFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, RX_PFC_ENf, control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_PFC_CTRLr(unit, port , reg_val));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_llfc_control_set(int unit, soc_port_t port, const portmod_llfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_LLFC_CTRLr(unit, port , &reg_val));

    if(control->rx_enable || control->tx_enable){
        soc_reg64_field32_set(unit, XLMAC_LLFC_CTRLr, &reg_val, LLFC_IN_IPG_ONLYf, control->in_ipg_only);
        soc_reg64_field32_set(unit, XLMAC_LLFC_CTRLr, &reg_val, LLFC_CRC_IGNOREf, control->crc_ignore);
    }
    soc_reg64_field32_set(unit, XLMAC_LLFC_CTRLr, &reg_val, TX_LLFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_LLFC_CTRLr, &reg_val, RX_LLFC_ENf, control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_LLFC_CTRLr(unit, port , reg_val));

exit:
    SOC_FUNC_RETURN;
}


int xlmac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    _SOC_IF_ERR_EXIT(READ_XLMAC_PAUSE_CTRLr(unit, port , &reg_val));

    refresh_enable = soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, reg_val, PAUSE_REFRESH_ENf);
    refresh_timer  =  soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, reg_val, PAUSE_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable) ? refresh_timer : -1;

    control->xoff_timer = soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, reg_val, PAUSE_XOFF_TIMERf);
    control->rx_enable  = soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, reg_val, RX_PAUSE_ENf);
    control->tx_enable  = soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, reg_val, TX_PAUSE_ENf);

exit:
    SOC_FUNC_RETURN;
}


int xlmac_pfc_control_get(int unit, soc_port_t port, portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    _SOC_IF_ERR_EXIT(READ_XLMAC_PFC_CTRLr(unit, port , &reg_val));

    refresh_timer  = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, PFC_REFRESH_TIMERf);
    refresh_enable = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, PFC_REFRESH_ENf);

    control->refresh_timer = (refresh_enable) ? refresh_timer : -1;

    control->stats_en   = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, PFC_STATS_ENf);
    control->xoff_timer = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, PAUSE_XOFF_TIMERf);
    control->force_xon  = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, FORCE_PFC_XONf);
    control->rx_enable  = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, TX_PFC_ENf);
    control->tx_enable  = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, reg_val, RX_PFC_ENf);

exit:
    SOC_FUNC_RETURN;
}


int xlmac_llfc_control_get(int unit, soc_port_t port, portmod_llfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_XLMAC_LLFC_CTRLr(unit, port , &reg_val));

    control->in_ipg_only = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, reg_val, LLFC_IN_IPG_ONLYf);
    control->crc_ignore  = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, reg_val, LLFC_CRC_IGNOREf);
    control->rx_enable   = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, reg_val, TX_LLFC_ENf);
    control->tx_enable   = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, reg_val, RX_LLFC_ENf);

exit:
    SOC_FUNC_RETURN;
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
    portmod_pause_control_t control;
    uint64 mac_ctrl, rx_ctrl, tx_ctrl, rval;
    int is_ipg_check_disable;
    int is_strip_crc, is_append, is_replace, is_higig;
    SOC_INIT_FUNC_DEFS;

    is_strip_crc =          (init_flags & XLMAC_INIT_F_RX_STRIP_CRC         ? 1 : 0);
    is_append =             (init_flags & XLMAC_INIT_F_TX_APPEND_CRC        ? 1 : 0);
    is_replace =            (init_flags & XLMAC_INIT_F_TX_REPLACE_CRC       ? 1 : 0);
    is_higig =              (init_flags & XLMAC_INIT_F_IS_HIGIG             ? 1 : 0);
    is_ipg_check_disable =  (init_flags & XLMAC_INIT_F_IPG_CHECK_DISABLE    ? 1 : 0);
    (void)is_ipg_check_disable;

    if(is_append && is_replace) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("XLAMC_INIT_F_TX_APPEND_CRC and XLAMC_INIT_F_TX_REPLACE_CRC can't co-exist")));
    }

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    _SOC_IF_ERR_EXIT(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));

    if (soc_reg64_field32_get(unit, XLMAC_CTRLr, mac_ctrl, SOFT_RESETf))
    {
    /*    _SOC_IF_ERR_EXIT(soc_port_credit_reset(unit, port)); */
    }

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, XGMII_IPG_CHECK_DISABLEf,
                          IS_HG_PORT(unit, port) ? 1 : 0);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

    /* XLMAC_RX_CTRL */
    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_CTRLr(unit, port, &rx_ctrl));

    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, is_strip_crc);
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rx_ctrl, STRICT_PREAMBLEf, is_higig ? 1 : 0);
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf, 64);

    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_CTRLr(unit, port, rx_ctrl));

    /* CHECK CHECK CHECK : How to get correct ipg : forcing to 6 */
    _SOC_IF_ERR_EXIT(READ_XLMAC_TX_CTRLr(unit, port, &tx_ctrl));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf, 2);
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, 6);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_TX_CTRLr(unit, port, tx_ctrl));



    control.tx_enable = FALSE;
    control.rx_enable = FALSE;
    _SOC_IF_ERR_EXIT(xlmac_pause_control_set(unit, port, &control));

#if 0
    /* CHECK CHECK CHECK:  Do we need to program these? */
    _SOC_IF_ERR_EXIT
        (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, PFC_REFRESH_ENf,
                                1));

    if (soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE)) {
        _SOC_IF_ERR_EXIT
            (soc_mac_xl.md_control_set(unit, port,
                                       SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
                                       13));
    }
#endif

    /* Set jumbo max size (8000 byte payload) */
    _SOC_IF_ERR_EXIT(xlmac_rx_max_size_set(unit, port, JUMBO_MAXSZ));

    /* CHECK CHEEK CHECK : How do we get the speed information. Speed hardcoded to 10G */ 
    _SOC_IF_ERR_EXIT(xlmac_speed_set(unit, port, 10000)); 


    /* XLMAC_RX_LSS_CTRL */
    _SOC_IF_ERR_EXIT(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LOCAL_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_REMOTE_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LINK_INTERRUPTf, 1);    
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, rval));

    /* Disable loopback and bring XLMAC out of reset */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 1);
    _SOC_IF_ERR_EXIT(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

exit:
    SOC_FUNC_RETURN;

}


#undef _ERR_MSG_MODULE_NAME


#endif /* PORTMOD_PM4X10_SUPPORT */
