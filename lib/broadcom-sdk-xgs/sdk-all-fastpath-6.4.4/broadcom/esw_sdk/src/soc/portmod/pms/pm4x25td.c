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
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X25TD_SUPPORT

#include <soc/portmod/clmac.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>

int pm4x25td_port_tsc_reset_set(int unit, int port, int in_reset)
{
    static int rst_flag = 0;
    SOC_INIT_FUNC_DEFS;

    if (!in_reset && !rst_flag) {
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 0));
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 1));
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 2));
        _SOC_IF_ERR_EXIT(soc_tsc_xgxs_reset(unit, port, 3));
        rst_flag = 1;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CPORT_LAG_FAILOVER_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_LAG_FAILOVER_CONFIGr(unit, port, rval));
    soc_reg_field_set(unit, CPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_LAG_FAILOVER_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}


int pm4x25td_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    int rv;
    rv = READ_PGW_CNTMAXSIZEr(unit, port, (uint32_t *)val);
    return rv;
}

int pm4x25td_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    int rv;
    rv = WRITE_PGW_CNTMAXSIZEr(unit, port, (uint32_t)val);
    return rv;
}

int pm4x25td_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    uint32_t rval, modid;
    int      flen;
    SOC_INIT_FUNC_DEFS;

    flen = soc_reg_field_length(unit, CPORT_CONFIGr, MY_MODIDf)? value : 0;
    modid =  (value < (1 <<  flen))? value : 0;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, MY_MODIDf, modid);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig2_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, HIGIG2_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig2_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    *(mode) = soc_reg_field_get(unit, CPORT_CONFIGr, rval, HIGIG2_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, HIGIG_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_higig_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    *(mode) = soc_reg_field_get(unit, CPORT_CONFIGr, rval, HIGIG_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_config_port_type_set (int unit, int port, pm_info_t pm_info, int type)
{

    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, CPORT_CONFIGr, &rval, PORT_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_CPORT_CONFIGr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_config_port_type_get (int unit, int port, pm_info_t pm_info, int *type)
{

    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPORT_CONFIGr(unit, port, &rval));
    *(type) = soc_reg_field_get(unit, CPORT_CONFIGr, rval, PORT_TYPEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25td_port_led_chain_config (int unit, int port, pm_info_t *pm, 
                          int value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(WRITE_CPORT_LED_CHAIN_CONFIGr (unit, port, value));

exit:
    SOC_FUNC_RETURN;
}


int _pm4x25td_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_add_info_t* add_info, int enable)
{
    uint32 reg_val;
    uint32 rsv_mask;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    int i, lane, rv;
    phymod_core_access_t* core_access;
    SOC_INIT_FUNC_DEFS;

    if(enable) {
        /* Power Save */
        _SOC_IF_ERR_EXIT(READ_CPORT_POWER_SAVEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_POWER_SAVEr, &reg_val, CPORT_COREf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_POWER_SAVEr(unit, port, reg_val));


        /* Bring MAC OOR */
        _SOC_IF_ERR_EXIT(READ_CPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_MAC_CONTROLr, &reg_val, CMAC_RESETf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_MAC_CONTROLr(unit, port, reg_val));

        if (add_info->interface_config.interface == SOC_PORT_IF_CAUI) {
            rv = WRITE_CPORT_MODE_REGr(unit, port, 1); 
            _SOC_IF_ERR_EXIT(rv);
        } else {
            rv = WRITE_CPORT_MODE_REGr(unit, port, 0); 
            _SOC_IF_ERR_EXIT(rv);
        }
    
        /* RSV Mask */
        rsv_mask = 0;
        SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
        SHR_BITSET(&rsv_mask, 4); /* CRC error */
        SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
        SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
        _SOC_IF_ERR_EXIT(WRITE_PGW_MAC_RSV_MASKr(unit, port, rsv_mask));

        if (PM_4x25_INFO(pm_info)->nof_phys) {
            /* Get Serdes OOR */
            _SOC_IF_ERR_EXIT(pm4x25td_port_tsc_reset_set(unit, port, 1));
            _SOC_IF_ERR_EXIT(pm4x25td_port_tsc_reset_set(unit, port, 0));
        }

        /* Initialize Phys */
        for(i=0 ; i<PM_4x25_INFO(pm_info)->nof_phys ; i++) {
            core_access = &(PM_4x25_INFO(pm_info)->core_access[i]);
            _SOC_IF_ERR_EXIT(phymod_core_probe(&(core_access->access), &(core_access->type)));

            _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

            if(i==0 /*internal phy */) {
                core_conf.firmware_load_method = PM_4x25_INFO(pm_info)->fw_load_method;
                core_conf.firmware_loader = PM_4x25_INFO(pm_info)->external_fw_loader;
                core_conf.lane_map = PM_4x25_INFO(pm_info)->lane_map;
            } else {
                core_conf.firmware_load_method = 
                    (PM_4x25_INFO(pm_info)->fw_load_method == phymodFirmwareLoadMethodNone) ? phymodFirmwareLoadMethodNone : phymodFirmwareLoadMethodInternal;
                core_conf.firmware_loader = NULL;

                /* configure "no swap" for external phys. can be set later using APIs*/
                _SOC_IF_ERR_EXIT(phymod_lane_map_t_init(&core_conf.lane_map));
                core_conf.lane_map.num_of_lanes = PM4X25_LANES_PER_CORE;
                for(lane=0 ; lane<PM4X25_LANES_PER_CORE ; lane++) {
                    core_conf.lane_map.lane_map_rx[lane] = lane;
                    core_conf.lane_map.lane_map_tx[lane] = lane;
                }
            }

            _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
            _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, add_info->interface_config.interface, &core_conf.interface.interface_type));
            core_conf.interface.data_rate = add_info->interface_config.speed;
            core_conf.interface.interface_modes = add_info->interface_config.interface_modes;
            core_conf.interface.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

            _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
            core_status.pmd_active = 0;

            if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
                PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
            } else {
                PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_CLR(&core_conf);
            }

            _SOC_IF_ERR_EXIT(phymod_core_init(core_access, &core_conf, &core_status));
        }
        

    } else { /* disable */

        rv = WRITE_CPORT_MODE_REGr(unit, port, 0); 
        _SOC_IF_ERR_EXIT(rv);

        /* Put Serdes in reset*/
        _SOC_IF_ERR_EXIT(pm4x25td_port_tsc_reset_set(unit, port, 1));

        /* put MAC in reset */
        _SOC_IF_ERR_EXIT(READ_CPORT_MAC_CONTROLr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_MAC_CONTROLr, &reg_val, CMAC_RESETf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_MAC_CONTROLr(unit, port, reg_val));

        /* Turn on Power Save */
        _SOC_IF_ERR_EXIT(READ_CPORT_POWER_SAVEr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_POWER_SAVEr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_POWER_SAVEr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}

int _pm4x25td_pm_port_init(int unit, int port, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable)
{
    int rv;
    uint32 reg_val, flags;
    SOC_INIT_FUNC_DEFS;

    if(enable) {

        /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));

        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));

        /* Port enable */
        _SOC_IF_ERR_EXIT(READ_CPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_ENABLE_REGr, &reg_val, PORT0f, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_ENABLE_REGr(unit, port, reg_val));
      
        /* Init MAC */
        flags = 0;
        if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_RX_STRIP_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_TX_APPEND_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_TX_REPLACE_CRC;
        }

        if(PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
            flags |= CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
        }

        flags |= CLMAC_INIT_F_IPG_CHECK_DISABLE;

        if(PHYMOD_INTF_MODES_HIGIG_GET(&(add_info->interface_config))) {
            flags |= CLMAC_INIT_F_IS_HIGIG;
        }

        rv = clmac_init(unit, port, flags);
        _SOC_IF_ERR_EXIT(rv);

        /* LSS */
        _SOC_IF_ERR_EXIT(READ_CPORT_FAULT_LINK_STATUSr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_FAULT_LINK_STATUSr, &reg_val,
                          REMOTE_FAULTf, 1);
        soc_reg_field_set(unit, CPORT_FAULT_LINK_STATUSr, &reg_val,
                          LOCAL_FAULTf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_FAULT_LINK_STATUSr(unit, port, reg_val)); 


        /* Counter MAX size */ 
        _SOC_IF_ERR_EXIT(READ_PGW_CNTMAXSIZEr(unit, port, &reg_val));
        soc_reg_field_set(unit, PGW_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, 1518);
        _SOC_IF_ERR_EXIT(WRITE_PGW_CNTMAXSIZEr(unit, port, reg_val));
 
        /* Reset MIB counters */ 
        _SOC_IF_ERR_EXIT(READ_PGW_MIB_RESETr(unit, &reg_val));
        /* coverity[ptr_arith:FALSE] */
        SHR_BITSET(&reg_val, internal_port);
        _SOC_IF_ERR_EXIT(WRITE_PGW_MIB_RESETr(unit, reg_val));
        SHR_BITCLR(&reg_val, internal_port);
        _SOC_IF_ERR_EXIT(WRITE_PGW_MIB_RESETr(unit, reg_val));

    } else {
        /* Port disable */
        _SOC_IF_ERR_EXIT(READ_CPORT_ENABLE_REGr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_ENABLE_REGr, &reg_val ,PORT0f, 0);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_ENABLE_REGr(unit, port, reg_val));

         /* Soft reset */
        _SOC_IF_ERR_EXIT(READ_CPORT_SOFT_RESETr(unit, port, &reg_val));
        soc_reg_field_set(unit, CPORT_SOFT_RESETr, &reg_val, CPORT_COREf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPORT_SOFT_RESETr(unit, port, reg_val));
    }

exit:
    SOC_FUNC_RETURN;
}


#endif /* PORTMOD_PM4X25TD_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
