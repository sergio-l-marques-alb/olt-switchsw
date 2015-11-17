/*
 * $Id: $
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
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#define SOC_JER_INVALID_CTXT_ADDRESS 0x7

#define SOC_JER_ILKN_CTXT_MAP_START         (0x0)
#define SOC_JER_NIF_CTXT_MAP_START          (SOC_JER_ILKN_CTXT_MAP_START        + 0x600)
#define SOC_JER_RCY_CTXT_MAP_START_CORE_0   (SOC_JER_NIF_CTXT_MAP_START         + 144)
#define SOC_JER_RCY_CTXT_MAP_START_CORE_1   (SOC_JER_RCY_CTXT_MAP_START_CORE_0  + 0x100)
#define SOC_JER_CPU_CTXT_MAP_START          (SOC_JER_RCY_CTXT_MAP_START_CORE_1  + 0x100)

#define SOC_JER_RCY_CTXT_MAP_START(core_id) (core_id == 0 ? SOC_JER_RCY_CTXT_MAP_START_CORE_0 : SOC_JER_RCY_CTXT_MAP_START_CORE_1)


#define SOC_JER_RCY_0_INTERFACE 145
#define SOC_JER_RCY_1_INTERFACE 146
#define SOC_JER_RCY_INTERFACE(core_id) (core_id == 0 ? SOC_JER_RCY_0_INTERFACE : SOC_JER_RCY_1_INTERFACE)
#define SOC_JER_CPU_INTERFACE   147



#include <soc/types.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <bcm_int/dpp/alloc_mngr.h>

/*
 *  EGQ TXi thresholds
 */
/* 10Gbps */
#define JER_EGQ_TXI_TH_ID_10GB       0

/* 20Gbps */
#define JER_EGQ_TXI_TH_ID_20GB       1

/* 40Gbps */
#define JER_EGQ_TXI_TH_ID_40GB       2

/* 100Gbps */
#define JER_EGQ_TXI_TH_ID_100GB      3

/* 200Gbps */
#define JER_EGQ_TXI_TH_ID_200GB      4

/* General */
#define JER_EGQ_TXI_TH_ID_OFF        1

/*
 *  EGQ Frequency Ready thresholds
 */
/* 10Gbps */
#define JER_EGQ_FREQ_RDY_TH_ID_10GB  5

/* 20Gbps */
#define JER_EGQ_FREQ_RDY_TH_ID_20GB  10 

/* 40Gbps */
#define JER_EGQ_FREQ_RDY_TH_ID_40GB  19

/* 100Gbps */
#define JER_EGQ_FREQ_RDY_TH_ID_100GB 47

/* 200Gbps */
#define JER_EGQ_FREQ_RDY_TH_ID_200GB 64

/* General */
#define JER_EGQ_FREQ_RDY_TH_ID_OFF   0x20


STATIC int
  soc_jer_port_to_interface_ingress_map_set(
      int                   unit,
      soc_port_t            port
   )
{
    soc_port_if_t interface;
    ARAD_PORTS_ILKN_CONFIG* ilkn_config = NULL;
    uint32 data, channel, ctxt_id, index, tm_port, phy_port;
    int core;
    uint32 protocol_offset, flags, is_nif;
    uint32 ctxt_channelized, ctxt_addr, master_ctxt_id;
    uint32 is_master_port, reassebly_ctxt, master_channel, base_ctxt_id;
    soc_reg_above_64_val_t data_above_64;
    soc_port_t master_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));
    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

    if(interface == SOC_PORT_IF_ERP) {
        /* Do nothing */
    } 
    else if(interface == SOC_PORT_IF_OLP) {
        SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_alloc(unit, &reassebly_ctxt));
        data = 0;
        soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_N_REASSEMBLY_CONTEXTf, reassebly_ctxt);
        soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_N_PORT_TERMINATION_CONTEXTf, tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    } 
    else if(interface == SOC_PORT_IF_OAMP) {
        SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_alloc(unit, &reassebly_ctxt));
        data = 0;
        soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_N_REASSEMBLY_CONTEXTf, reassebly_ctxt);
        soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_N_PORT_TERMINATION_CONTEXTf, tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    }
    else if ((interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (is_master_port && (interface == SOC_PORT_IF_RCY || is_nif))) 
    {   
        ctxt_channelized = 0;
        ctxt_addr = SOC_JER_INVALID_CTXT_ADDRESS;

        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, &protocol_offset /* ilkn id */));
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
            phy_port--; /* 0 based */

            base_ctxt_id = SOC_JER_ILKN_CTXT_MAP_START + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
            ctxt_id = base_ctxt_id + channel;
            index = phy_port;
            ctxt_channelized = ilkn_config->interleaved ? 1 : 0;  
            ctxt_addr = ilkn_config->interleaved ? protocol_offset : SOC_JER_INVALID_CTXT_ADDRESS;      
        } 
        else if (interface == SOC_PORT_IF_RCY){
            base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(core);
            ctxt_id = base_ctxt_id + channel;
            index = SOC_JER_RCY_INTERFACE(core);
        } else if (interface == SOC_PORT_IF_CPU){
            base_ctxt_id = SOC_JER_CPU_CTXT_MAP_START;
            ctxt_id = base_ctxt_id;
            index = SOC_JER_CPU_INTERFACE;
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
            phy_port--; /* 0 based */

            base_ctxt_id = SOC_JER_NIF_CTXT_MAP_START + phy_port;
            ctxt_id = base_ctxt_id;
            index = phy_port;
        } 

        data = 0;
        soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_IS_CHANNELIZEDf, &ctxt_channelized);
        soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf, &ctxt_addr);  
        soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, CTXT_MAP_BASE_ADDRESSf, &base_ctxt_id);  
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_PORT_TO_BASE_ADDRESS_MAPm(unit, MEM_BLOCK_ANY, index, &data));

        if((interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved) || is_master_port ||
           (interface == SOC_PORT_IF_CPU) || (interface == SOC_PORT_IF_RCY)) {
            /* Allocate new context */
            SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_alloc(unit, &reassebly_ctxt));
        } else {
            /* Use same conext */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &master_channel));
            master_ctxt_id = base_ctxt_id + master_channel;

            SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, master_ctxt_id, &data));
            soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
        }

        data = 0;
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));

         if(interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved) {
            SOC_REG_ABOVE_64_CLEAR(data_above_64);
            SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
            SHR_BITSET(data_above_64, reassebly_ctxt);
            SOCDNX_IF_ERR_EXIT(WRITE_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_jer_port_to_interface_ingress_unmap_set(
      int                   unit,
      soc_port_t            port
   )
{
    soc_port_if_t interface;
    ARAD_PORTS_ILKN_CONFIG* ilkn_config = NULL;
    uint32 data, channel, ctxt_id, tm_port, num_of_channels;
    int core;
    uint32 is_last_port, phy_port, reassebly_ctxt, is_master_port;
    soc_reg_above_64_val_t data_above_64;
    uint32 protocol_offset, base_ctxt_id, flags, is_nif;
    soc_port_t next_master;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &num_of_channels));
    is_last_port = (num_of_channels==1 ? 1 : 0);
    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

    if(interface == SOC_PORT_IF_ERP) {
        /* Do nothing */
    } if(interface == SOC_PORT_IF_OLP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        reassebly_ctxt = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OLP_N_REASSEMBLY_CONTEXTf);
        SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_dealloc(unit, reassebly_ctxt));
    }
    else if(interface == SOC_PORT_IF_OAMP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        reassebly_ctxt = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OAMP_N_REASSEMBLY_CONTEXTf);
        SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_dealloc(unit, reassebly_ctxt));
    } 
    else if((interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (is_last_port && (interface == SOC_PORT_IF_CPU || is_nif))) {

        if(interface == SOC_PORT_IF_ILKN) { 

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, &protocol_offset /* ilkn id */));
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

            if(ilkn_config->interleaved || is_last_port) { 
                base_ctxt_id = SOC_JER_ILKN_CTXT_MAP_START + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
                ctxt_id = base_ctxt_id + channel;
                SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
                soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);

                SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_dealloc(unit, reassebly_ctxt));
            } 

            if(ilkn_config->interleaved) {
                SOC_REG_ABOVE_64_CLEAR(data_above_64);
                SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
                SHR_BITCLR(data_above_64, reassebly_ctxt);
                SOCDNX_IF_ERR_EXIT(WRITE_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
            }
        } else {
            switch(interface) {
                case SOC_PORT_IF_RCY:
                    ctxt_id = SOC_JER_RCY_CTXT_MAP_START(core) + channel;
                    break;
                case SOC_PORT_IF_CPU:
                    ctxt_id = SOC_JER_CPU_CTXT_MAP_START;
                    break;
                default:
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
                    phy_port--; /* 0 based */
                    ctxt_id = SOC_JER_NIF_CTXT_MAP_START + phy_port;
                    break;
            }

            SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
            soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);

            SOCDNX_IF_ERR_EXIT(bcm_dpp_am_reassembly_context_dealloc(unit, reassebly_ctxt));
        }
    }

    /* NIF or CPU port which isn't last and not ILKN - we might need to switch the tm_port*/
    if ((is_nif || (interface == SOC_PORT_IF_CPU)) && is_master_port && !is_last_port && (interface != SOC_PORT_IF_ILKN)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_next_master_get(unit, port, &next_master));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, next_master, &tm_port, &core));

        if (is_nif) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
            phy_port--; /* 0 based */
            ctxt_id = SOC_JER_NIF_CTXT_MAP_START + phy_port;
        } else { /* CPU port */
            ctxt_id = SOC_JER_CPU_CTXT_MAP_START;
        }
        SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_jer_ports_phy_to_quad_get(
    int                   unit,
    soc_port_t            first_phy, /*1 based*/
    uint32*               quad
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (first_phy < 37) {
        (*quad) = (first_phy - 1) / 4;
    } else if (first_phy < 85) {
        (*quad) = ((first_phy - 37) / 16) + 9;
    } else if (first_phy < 97) {
        (*quad) = ((first_phy - 85) / 4) + 12;
    } else {
        (*quad) = ((first_phy - 97) / 16) + 15;
    }

    SOCDNX_FUNC_RETURN;
}

#define EGQ_CORE_FOR_NIF_QMLF_FIRST_ILKN_BIT 18
#define JER_PORTS_INVALID_INTF 0xFF

STATIC int
  soc_jer_port_to_interface_egress_map_set(
    int                   unit,
    soc_port_t            port
   )
{
    uint32 tm_port, egr_if, reg32[1];
    int ps, is_channelized;
    soc_reg_above_64_val_t data;
    uint32 q_pair_num, nof_q_pairs;
    soc_port_if_t interface;
    ARAD_EGQ_PPCT_TBL_DATA ppct_data;
    uint32 offset, channel, if_rate_mbps, otm_port_number;
    uint32 freq_rdy_sel_val, txi_sel_val, base_q_pair, phy_port;
    int rv, core; 
    uint32 cancel_en1, cancel_en2, cancel_en3, cancel_en4, quad;
    uint32 protocol_offset, is_nif, flags;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface != SOC_PORT_IF_ERP) {
        cancel_en1 = 0;
        cancel_en2 = 0;
        cancel_en3 = 0;
        cancel_en4 = 0;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        if (if_rate_mbps == 0) { /* OFF */
            freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_OFF;
            txi_sel_val = JER_EGQ_TXI_TH_ID_OFF;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 12500) { /* 12.5Gbps or less */
            freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_10GB;
            txi_sel_val = JER_EGQ_TXI_TH_ID_10GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 20000) { /* 12.5Gbps-20Gbps */
            freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_20GB;
            txi_sel_val = JER_EGQ_TXI_TH_ID_20GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 42000) { /* 20Gbps-42Gbps */
            freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_40GB;
            txi_sel_val = JER_EGQ_TXI_TH_ID_40GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
        } else if (if_rate_mbps <= 100000) { /* 40Gbps-100Gbps */
            freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_100GB;
            txi_sel_val = JER_EGQ_TXI_TH_ID_100GB;
        } else { /* > 100Gbps */
            freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_200GB;
            txi_sel_val = JER_EGQ_TXI_TH_ID_200GB;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, tm_port, core, &egr_if));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));

        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NRDY_TH_SELf, &txi_sel_val);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, FQ_RDY_THf, &freq_rdy_sel_val);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_1f, &cancel_en1);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_2f, &cancel_en2);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_3f, &cancel_en3);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_4f, &cancel_en4);
        otm_port_number = is_channelized ? 0xFF : base_q_pair;
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, &otm_port_number);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));

         /* Run over all q_pairs that belong to that port */
        for (q_pair_num = 0; q_pair_num < nof_q_pairs; q_pair_num++)
        {    
            offset = base_q_pair+q_pair_num;
            SOCDNX_IF_ERR_EXIT(READ_EGQ_PORT_CONFIGURATION_TABLE_PCTm(unit, EGQ_BLOCK(unit, core), offset, data));
            soc_mem_field_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PORT_CH_NUMf, &channel);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PORT_CONFIGURATION_TABLE_PCTm(unit, EGQ_BLOCK(unit, core), offset, data));
        }


        rv = arad_egq_ppct_tbl_get_unsafe(unit, base_q_pair, EGQ_BLOCK(unit, core), &(ppct_data));
        SOCDNX_IF_ERR_EXIT(rv);

        ppct_data.cgm_interface = egr_if;

        rv = arad_egq_ppct_tbl_set_unsafe(unit, base_q_pair, EGQ_BLOCK(unit, core), &(ppct_data));
        SOCDNX_IF_ERR_EXIT(rv);

        if(egr_if < SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces)) {
            /*  Map IFC To PS  */
            ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
            SOCDNX_IF_ERR_EXIT(READ_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));    
            SHR_BITSET(data, ps);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));   

            /* Mark channalized interface */
            SOCDNX_IF_ERR_EXIT(READ_EGQ_IFC_IS_CHANNELIZEDr(unit, core, reg32));
            if(is_channelized) {
                SHR_BITSET(reg32, egr_if);
            } else {
                SHR_BITCLR(reg32, egr_if);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_IFC_IS_CHANNELIZEDr(unit, core, *reg32));
        }


        /* Connect to netwrok interface */
        if(is_nif) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
            SOCDNX_IF_ERR_EXIT(soc_jer_ports_phy_to_quad_get(unit, phy_port, &quad));
            phy_port--;
            SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_NIF_PORTr(unit, phy_port, egr_if));

            if(interface == SOC_PORT_IF_ILKN) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, &protocol_offset /* ilkn id */));

                /* protocol offset */
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_HRFr(unit, protocol_offset, egr_if));

                /* ILKN bits are 18 and above */
                SOCDNX_IF_ERR_EXIT(READ_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, reg32));
                if(core == 1) {
                    SHR_BITSET(reg32, EGQ_CORE_FOR_NIF_QMLF_FIRST_ILKN_BIT + protocol_offset);
                } else { /*core==0*/
                    SHR_BITCLR(reg32, EGQ_CORE_FOR_NIF_QMLF_FIRST_ILKN_BIT + protocol_offset);
                }
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, *reg32));
            } else {
                SOCDNX_IF_ERR_EXIT(READ_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, reg32));
                if(core == 1) {
                    SHR_BITSET(reg32, quad);
                } else { /*core==0*/
                    SHR_BITCLR(reg32, quad);
                }
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, *reg32));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_jer_port_to_interface_egress_unmap_set(
      int                   unit,
      soc_port_t            port
   )
{
    uint32 tm_port, egr_if, flags, is_nif;
    int core, ps;
    soc_reg_above_64_val_t data;
    uint32 freq_rdy_sel_val, txi_sel_val, invalid_otm;
    uint32 cancel_en1, cancel_en2, cancel_en3, cancel_en4;
    uint32 base_q_pair, reg32[1], if_rate_mbps, higher_req_en;
    uint32 is_last_port, num_of_channels, phy_port, protocol_offset;
    soc_port_if_t interface;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface != SOC_PORT_IF_ERP) {
        cancel_en1 = 1;
        cancel_en2 = 1;
        cancel_en3 = 1;
        cancel_en4 = 0;

        freq_rdy_sel_val = JER_EGQ_FREQ_RDY_TH_ID_OFF;
        txi_sel_val = JER_EGQ_TXI_TH_ID_OFF;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, tm_port, core, &egr_if));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &num_of_channels));
        is_last_port = (num_of_channels==1 ? 1 : 0);
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        invalid_otm = 0xFF;
        higher_req_en = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS ? 1 : 0);

        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NRDY_TH_SELf, &txi_sel_val);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, FQ_RDY_THf, &freq_rdy_sel_val);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_1f, &cancel_en1);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_2f, &cancel_en2);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_3f, &cancel_en3);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_4f, &cancel_en4);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, &invalid_otm);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, HIGHER_REQ_EN_PER_MALf, &higher_req_en);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

        if(egr_if < SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces)) {
            /*  Map IFC To PS  */
            ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
            SOCDNX_IF_ERR_EXIT(READ_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));    
            SHR_BITCLR(data, ps);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));   

            /* Mark channalized interface */
            SOCDNX_IF_ERR_EXIT(READ_EGQ_IFC_IS_CHANNELIZEDr(unit, core, reg32));
            SHR_BITCLR(reg32, egr_if);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_IFC_IS_CHANNELIZEDr(unit, core, *reg32));
        }

        /* Disconnect to netwrok interface */
        if(is_nif) {
            if(is_last_port) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
                phy_port--;
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_NIF_PORTr(unit, phy_port, JER_PORTS_INVALID_INTF));

                if(interface == SOC_PORT_IF_ILKN) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, &protocol_offset /* ilkn id */));

                    /* protocol offset */
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_HRFr(unit, protocol_offset, JER_PORTS_INVALID_INTF));
                }
            }
        }
    }



exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_port_to_interface_map_set(
      int                   unit,
      soc_port_t            port,
      int                   unmap
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    if(!unmap) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_ingress_map_set(unit, port));
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_egress_map_set(unit, port));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_ingress_unmap_set(unit, port));
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_egress_unmap_set(unit, port));
    }


    exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

