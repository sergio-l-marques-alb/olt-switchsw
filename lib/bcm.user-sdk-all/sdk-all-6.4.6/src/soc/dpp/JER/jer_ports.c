/*
 * $Id: $
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
 * File: jer_ports.c
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
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/TMC/tmc_api_ports.h>


/* EGQ FQ Ready thresholds */
#define JER_EGQ_FQ_RDY_TH_ID_1GB     5
#define JER_EGQ_FQ_RDY_TH_ID_10GB    5
#define JER_EGQ_FQ_RDY_TH_ID_25GB    10 
#define JER_EGQ_FQ_RDY_TH_ID_40GB    19
#define JER_EGQ_FQ_RDY_TH_ID_100GB   47
#define JER_EGQ_FQ_RDY_TH_ID_200GB   64
#define JER_EGQ_FQ_RDY_TH_ID_300GB   96
#define JER_EGQ_FQ_RDY_TH_ID_400GB   128
#define JER_EGQ_FQ_RDY_TH_ID_CPU_OAM 19
#define JER_EGQ_FQ_RDY_TH_ID_OLP     19
#define JER_EGQ_FQ_RDY_TH_ID_RCY     64
#define JER_EGQ_FQ_RDY_TH_ID_OFF     0


 int
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
    uint32 is_master_port, reassebly_ctxt = 0xFFFFFFFF, master_channel, base_ctxt_id;
    uint8 found;
    soc_reg_above_64_val_t data_above_64;
    soc_port_t master_port;
    SOC_SAND_OCC_BM_PTR reassembly_ctxt_occ;
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
        reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
        }

        data = 0;
        soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_N_REASSEMBLY_CONTEXTf, reassebly_ctxt);
        soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_N_PORT_TERMINATION_CONTEXTf, tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    } 
    else if(interface == SOC_PORT_IF_OAMP) {
        reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
        }

        data = 0;
        soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_N_REASSEMBLY_CONTEXTf, reassebly_ctxt);
        soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_N_PORT_TERMINATION_CONTEXTf, tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    }
    else if ((interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (interface == SOC_PORT_IF_CPU) || (is_master_port && is_nif)) 
    {   
        ctxt_channelized = 0;
        ctxt_addr = SOC_JER_INVALID_CTXT_ADDRESS;

        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset /* ilkn id */));
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
            ctxt_id = base_ctxt_id + channel;
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

        if((interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved) || is_master_port || (interface == SOC_PORT_IF_RCY)) 
        {
            /* Allocate new context */
            reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt, &found));
            if (!found) {
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
            }

        } else {
            /* Use same context */
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, master_port, &master_channel));
            master_ctxt_id = base_ctxt_id + master_channel;

            SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, master_ctxt_id, &data));
            soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
        }

        data = 0;
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));

        if ((interface == SOC_PORT_IF_ILKN) || is_nif) {
            /* TDM context map is writen staticly to port# (512 context, each port has 1 context) regardless of the port status (TDM/non TDM) */
            data = 0;
            soc_mem_field_set(unit, IRE_NIF_TDM_CTXT_MAPm, &data, TDM_CONTEXTf, (uint32*)&port);
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IRE_NIF_TDM_CTXT_MAPm, MEM_BLOCK_ANY, ctxt_id, &data));
        }

         if(interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved) {
            SOC_REG_ABOVE_64_CLEAR(data_above_64);
            SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
            SHR_BITSET(data_above_64, reassebly_ctxt);
            SOCDNX_IF_ERR_EXIT(WRITE_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
        }

        /* map CPU channel to core */
        if (interface == SOC_PORT_IF_CPU) {
            SOC_REG_ABOVE_64_CLEAR(data_above_64);
            SOCDNX_IF_ERR_EXIT(READ_IRE_CPU_CHANNEL_PIPE_MAPr(unit, data_above_64));
            if(core == 1) {
                SHR_BITSET(data_above_64, channel);
            } else { /*core==0*/
                SHR_BITCLR(data_above_64, channel);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_IRE_CPU_CHANNEL_PIPE_MAPr(unit, data_above_64));            
        }
    }

    /* map reassembly context to core */
    if (interface != SOC_PORT_IF_ERP && reassebly_ctxt != 0xFFFFFFFF) {
        SOC_REG_ABOVE_64_CLEAR(data_above_64);
        SOCDNX_IF_ERR_EXIT(READ_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, data_above_64));
        if(core == 1) {
            SHR_BITSET(data_above_64, reassebly_ctxt);
        } else { /*core==0*/
            SHR_BITCLR(data_above_64, reassebly_ctxt);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, data_above_64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_port_ingr_reassembly_context_get(
      SOC_SAND_IN int         unit,
      SOC_SAND_IN soc_port_t  port,
      SOC_SAND_OUT uint32     *port_termination_context,
      SOC_SAND_OUT uint32     *reassembly_context
  )
{
    soc_port_if_t interface;
    uint32 data, channel, ctxt_id, tm_port, phy_port;
    int core;
    uint32 protocol_offset, flags, is_nif;
    uint32 is_master_port, base_ctxt_id;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));
    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

    if(interface == SOC_PORT_IF_ERP) {
        /* Do nothing */
        *port_termination_context = 0xffffffff;
        *reassembly_context = 0xffffffff;
    }
    else if(interface == SOC_PORT_IF_OLP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        *reassembly_context = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OLP_N_REASSEMBLY_CONTEXTf);
        *port_termination_context = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OLP_N_PORT_TERMINATION_CONTEXTf);
    }
    else if(interface == SOC_PORT_IF_OAMP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        *reassembly_context = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OAMP_N_REASSEMBLY_CONTEXTf);
        *port_termination_context = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OAMP_N_PORT_TERMINATION_CONTEXTf);
    }
    else if ((interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (interface == SOC_PORT_IF_CPU) || (is_master_port && is_nif))
    {
        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset /* ilkn id */));

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
            phy_port--; /* 0 based */

            base_ctxt_id = SOC_JER_ILKN_CTXT_MAP_START + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
            ctxt_id = base_ctxt_id + channel;
        }
        else if (interface == SOC_PORT_IF_RCY){
            base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(core);
            ctxt_id = base_ctxt_id + channel;
        } else if (interface == SOC_PORT_IF_CPU){
            base_ctxt_id = SOC_JER_CPU_CTXT_MAP_START;
            ctxt_id = base_ctxt_id + channel;
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
            phy_port--; /* 0 based */

            base_ctxt_id = SOC_JER_NIF_CTXT_MAP_START + phy_port;
            ctxt_id = base_ctxt_id;
        }

        SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
        soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, reassembly_context);
        soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, port_termination_context);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
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
    SOC_SAND_OCC_BM_PTR reassembly_ctxt_occ;
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
        /* deallocate rassembly context */
        reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
    }
    else if(interface == SOC_PORT_IF_OAMP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        reassebly_ctxt = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OAMP_N_REASSEMBLY_CONTEXTf);
        /* deallocate rassembly context */
        reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
    } 
    else if((interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (is_last_port && (interface == SOC_PORT_IF_CPU || is_nif))) {

        if(interface == SOC_PORT_IF_ILKN) { 

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset /* ilkn id */));
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

            if(ilkn_config->interleaved || is_last_port) { 
                base_ctxt_id = SOC_JER_ILKN_CTXT_MAP_START + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
                ctxt_id = base_ctxt_id + channel;
                SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
                soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);

                /* deallocate rassembly context */
                reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
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
                    ctxt_id = SOC_JER_CPU_CTXT_MAP_START + channel;
                    break;
                default:
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
                    phy_port--; /* 0 based */
                    ctxt_id = SOC_JER_NIF_CTXT_MAP_START + phy_port;
                    break;
            }

            SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
            soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);

            /* deallocate rassembly context */
            reassembly_ctxt_occ = arad_sw_db_reassembly_context_occ_get(unit);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
        }
    }

    /* NIF port which isn't last and not ILKN - we might need to switch the tm_port*/
    if (is_nif && is_master_port && !is_last_port && (interface != SOC_PORT_IF_ILKN)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_next_master_get(unit, port, &next_master));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, next_master, &tm_port, &core));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port /*one based*/));
        phy_port--; /* 0 based */
        ctxt_id = SOC_JER_NIF_CTXT_MAP_START + phy_port;

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
    uint32 offset, channel, if_rate_mbps, otm_port_number, ifc_delay;
    uint32 fq_rdy_sel_val, base_q_pair, phy_port;
    int rv, core; 
    uint32 cancel_en1, cancel_en2, cancel_en3, cancel_en4, quad;
    uint32 protocol_offset, is_nif, flags;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface != SOC_PORT_IF_ERP) {
        ifc_delay = 1;
        cancel_en1 = 0;
        cancel_en2 = 0;
        cancel_en3 = 0;
        cancel_en4 = 0;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OAMP) {
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_CPU_OAM;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (interface == SOC_PORT_IF_OLP) {
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_OLP;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (interface == SOC_PORT_IF_RCY) {
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_RCY;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps == 0) { /* OFF */
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_OFF;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 2500) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_1GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 12500) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_10GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 30000) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_25GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps <= 50000) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_40GB;
            cancel_en1 = 1;
            cancel_en2 = 1;
        } else if (if_rate_mbps <= 100000) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_100GB;
        } else if (if_rate_mbps <= 150000) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_200GB;
         } else if (if_rate_mbps <= 200000) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_200GB;
        } else if (if_rate_mbps <= 300000) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_300GB;
        } else { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_400GB;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));

        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, FQ_RDY_THf, &fq_rdy_sel_val);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_1f, &cancel_en1);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_2f, &cancel_en2);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_3f, &cancel_en3);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_4f, &cancel_en4);
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, IFC_DELAYf, &ifc_delay);
        otm_port_number = is_channelized ? 0xFF : base_q_pair;
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, &otm_port_number);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));

         /* Run over all q_pairs that belong to that port */
        for (q_pair_num = 0; q_pair_num < nof_q_pairs; q_pair_num++)
        {    
            offset = base_q_pair+q_pair_num;
            SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), offset, data));
            soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &channel);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), offset, data));
        }

        /* enable port queues */
        SOCDNX_IF_ERR_EXIT(soc_jer_port_control_tx_nif_enable_set(unit, port, TRUE));

        if (SOC_DPP_CONFIG((unit))->tm.queue_level_interface_enable) 
        {
            if(interface == SOC_PORT_IF_ILKN) {
                int curr_mc_q_pair, curr_uc_q_pair;

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset /* ilkn id */));

                for (q_pair_num = 0; q_pair_num < nof_q_pairs; q_pair_num++)
                { 
                    curr_uc_q_pair = base_q_pair+q_pair_num;
                    curr_mc_q_pair = ARAD_EGR_QUEUING_MC_QUEUE_OFFSET(curr_uc_q_pair); 

                    SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_uc_q_pair, data));
                    channel = SOC_DPP_CONFIG((unit))->tm.uc_q_pair2channel_id[protocol_offset][q_pair_num];
                    soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &channel);
                    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_uc_q_pair, data));

                    SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_mc_q_pair, data));
                    channel = SOC_DPP_CONFIG((unit))->tm.mc_q_pair2channel_id[protocol_offset][q_pair_num];
                    soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &channel);
                    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_mc_q_pair, data));
                }
            }
        }

        rv = arad_egq_ppct_tbl_get_unsafe(unit, core, base_q_pair, &(ppct_data));
        SOCDNX_IF_ERR_EXIT(rv);

        ppct_data.cgm_interface = egr_if;

        rv = arad_egq_ppct_tbl_set_unsafe(unit, core, base_q_pair, &(ppct_data));
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
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset /* ilkn id */));

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
    uint32 fq_rdy_sel_val, invalid_otm;
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

        fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_OFF;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &num_of_channels));
        is_last_port = (num_of_channels==1 ? 1 : 0);
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        invalid_otm = 0xFF;
        higher_req_en = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS ? 1 : 0);

        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, FQ_RDY_THf, &fq_rdy_sel_val);
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
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset /* ilkn id */));

                    /* protocol offset */
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_HRFr(unit, protocol_offset, JER_PORTS_INVALID_INTF));
                }
            }
        }
    }



exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_port_synce_clk_sel_get(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  *port)
{
    uint32
        reg_val,
        phy_line,
        phy_port = 0x0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_NBIH_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    phy_line = soc_reg_field_get(unit, NBIH_SYNC_ETH_CFGr, reg_val, SYNC_ETH_CLOCK_SEL_Nf) + 1;
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_add(unit, phy_line, &phy_port));
    *port = SOC_INFO(unit).port_p2l_mapping[phy_port];

exit:
    SOCDNX_FUNC_RETURN; 
}

uint32 soc_jer_port_synce_clk_sel_set(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  port)
{
    uint32
        reg_val,
        phy_line,
        phy_port = 0x0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_NBIH_SYNC_ETH_CFGr(unit, synce_cfg_num, &reg_val));

    /* Configure first port serdes */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit,  port, &phy_port));
    /* The function first_phy_port_get return shifted value due to CPU port 0 */
    phy_port -= 0x1; 
    SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, phy_port, &phy_line));
    soc_reg_field_set(unit, NBIH_SYNC_ETH_CFGr, &reg_val, SYNC_ETH_CLOCK_SEL_Nf,  phy_line);

    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_SYNC_ETH_CFGr(unit, synce_cfg_num, reg_val));

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


uint32 
  soc_jer_port_rate_egress_pps_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
    )
{
    uint32 limit_max, core_clock_speed, nof_tiks = 0/*packets per tiks*/;
    uint64 nof_tiks64, pps64, reg64_val;
    int max_length, fmac_index;
    SOCDNX_INIT_FUNC_DEFS;

    /*calculate packets per tiks*/
    core_clock_speed = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 1000 /*Hz*/;
    /*nof tiks = burst * core_clock / pps */
    COMPILER_64_SET(nof_tiks64, 0, burst);
    COMPILER_64_UMUL_32(nof_tiks64, core_clock_speed); /*packets per tiks to packets per time*/

    /*pps == 0 --> disables rate limiting*/
    if (pps != 0) {
        COMPILER_64_SET(pps64, 0, pps);
        COMPILER_64_UDIV_64(nof_tiks64, pps64);
        nof_tiks = COMPILER_64_LO(nof_tiks64);
    }

    /*validate input*/
    max_length = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_COUNTf);
    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max <  burst) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Burst %u is too big - should be < %u"), burst, limit_max)); 
    }

    max_length = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_PERIODf);
    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max <  nof_tiks) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Burst %u is too big relative to pps %u"), burst, nof_tiks)); 
    }

    if (nof_tiks == 0 && burst != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("num of ticks %u is too big relative to burst %u"), nof_tiks, burst)); 
    }

    /*disable shaper handle*/
    if (burst == 0) {
        fmac_index = port / SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
        COMPILER_64_SET(reg64_val, 0, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_index, reg64_val));
        SOC_EXIT;
    } else if (burst < 3){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Burst %u is too small - should be > 3"), burst));  
    }

    fmac_index = port / SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);
    COMPILER_64_SET(reg64_val, 0, 0);
    soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_COUNTf, burst - 2 /*-2 for shaper accuracy*/);
    soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_PERIODf, nof_tiks);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_index, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
  soc_jer_port_rate_egress_pps_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
  )
{
    uint32 nof_tiks, core_clock_speed;
    uint64 reg64_val;
    uint64 pps64, nof_tiks64;
    int fmac_index; 
    SOCDNX_INIT_FUNC_DEFS;
    
    fmac_index = port / SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac);

    SOCDNX_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_index, &reg64_val));
    *burst =  soc_reg64_field32_get(unit, FMAC_TX_CELL_LIMITr, reg64_val, CELL_LIMIT_COUNTf) + 2 /*+2 for shaper accuracy*/;
    nof_tiks = soc_reg64_field32_get(unit, FMAC_TX_CELL_LIMITr, reg64_val, CELL_LIMIT_PERIODf);

    if (*burst == 0 || nof_tiks == 0) {
        *pps = 0;
        *burst = 0;
        SOC_EXIT;
    }
    /*calculate packets per sec*/
    /*pps = burst * core_clock / nof_tiks */
    core_clock_speed = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency;
    COMPILER_64_SET(pps64, 0, *burst);
    COMPILER_64_UMUL_32(pps64, core_clock_speed); /*packets per tiks to packets per time*/
    COMPILER_64_UMUL_32(pps64, 1000); /*KHz to Hz*/

    COMPILER_64_SET(nof_tiks64, 0, nof_tiks);
    COMPILER_64_UDIV_64(pps64, nof_tiks64);

    *pps = COMPILER_64_LO(pps64); 

exit:
    SOCDNX_FUNC_RETURN;
}

int
  soc_jer_port_reference_clock_set(
      int                   unit,
      soc_port_t            port
   )
{
    SOC_JER_NIF_PLL_TYPE                pll_type;
    soc_dcmn_init_serdes_ref_clock_t    nif_clk_freq;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_port_pll_type_get(unit, port, &pll_type));

    /* get reference clock according to the matching PLL type */
    switch (pll_type) {
    case SOC_JER_NIF_PLL_TYPE_PMH:
        nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_out;
        break;
    case SOC_JER_NIF_PLL_TYPE_PML0:
        nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[0];
        break;
    case SOC_JER_NIF_PLL_TYPE_PML1:
        nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[1];
        break;
    default:
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
    }

    /* set reference clock */
    if(nif_clk_freq == soc_dcmn_init_serdes_ref_clock_125) {
        SOC_INFO(unit).port_refclk_int[port] = 125;
    } else if (nif_clk_freq == soc_dcmn_init_serdes_ref_clock_156_25){
        SOC_INFO(unit).port_refclk_int[port] = 156;
    } else if (nif_clk_freq == soc_dcmn_init_serdes_ref_clock_25){
        SOC_INFO(unit).port_refclk_int[port] = 25;
    } else {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_port_header_type_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core_id,
    SOC_SAND_IN  uint32                     tm_port,
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_TMC_PORT_HEADER_TYPE   header_type
  )
{
    uint32 channel, offset;
    soc_reg_t reg;
    soc_port_t reg_port;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 reg32_val[1];
    uint32 phy, lane;
    soc_port_if_t interface_type;
    soc_port_t logical_port;
    uint32 data = 0;
    SHR_BITDCLNAME(channel_tdm_bmp, 256);
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core_id, tm_port, &logical_port));
    SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_port_header_type_set(unit, core_id, tm_port, direction_ndx, header_type)));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, logical_port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, logical_port, &channel)); 

    if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {

        if (interface_type == SOC_PORT_IF_CPU) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IRE_CPU_CHANNEL_IS_TDMr, REG_PORT_ANY, 0, reg_above_64_val));
            SHR_BITSET(reg_above_64_val, channel);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IRE_CPU_CHANNEL_IS_TDMr, REG_PORT_ANY, 0, reg_above_64_val));

            SOCDNX_IF_ERR_EXIT(READ_IRE_CPU_TDM_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &data));
            soc_mem_field_set(unit, IRE_CPU_TDM_CTXT_MAPm, &data, TDM_CONTEXTf, (uint32*)&logical_port);
            SOCDNX_IF_ERR_EXIT(WRITE_IRE_CPU_TDM_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &data));

        } else {
            if (interface_type == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, logical_port, 0, &offset));
            reg = (offset < 2) ? NBIH_ILKN_CHANNEL_IS_TDM_PORTr : NBIL_ILKN_CHANNEL_IS_TDM_PORTr; 
            reg_port = (offset < 2) ? REG_PORT_ANY : (offset < 4);
            
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, (offset & 1), reg_above_64_val));
            soc_reg_above_64_field_get(unit, NBIH_ILKN_CHANNEL_IS_TDM_PORTr, reg_above_64_val, ILKN_CHANNEL_IS_TDM_PORTf, channel_tdm_bmp);
            SHR_BITSET(channel_tdm_bmp, channel);
            soc_reg_above_64_field_set(unit, NBIH_ILKN_CHANNEL_IS_TDM_PORTr, reg_above_64_val, ILKN_CHANNEL_IS_TDM_PORTf, channel_tdm_bmp);
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, (offset & 1), reg_above_64_val));
            }

            if (direction_ndx == SOC_TMC_PORT_DIRECTION_INCOMING) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy)); 
                SOCDNX_IF_ERR_EXIT(soc_jer_qsgmii_offsets_remove(unit, phy, &lane));
                lane--; 
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, lane / 4, reg32_val));
                switch (lane % 4) {
                    case 0:
                        soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_0_QMLF_Nf, 1);
                        break;
                    case 1:
                        soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_1_QMLF_Nf, 1);
                        break;
                    case 2:
                        soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_2_QMLF_Nf, 1);
                        break;
                    case 3:
                        soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_3_QMLF_Nf, 1);
                        break;
                }
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, lane / 4, *reg32_val));

                if (core_id) {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_RX_REQ_PIPE_1_TDM_ENr, REG_PORT_ANY, 0, reg32_val));
                    SHR_BITSET(reg32_val, lane / 4);
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_RX_REQ_PIPE_1_TDM_ENr, REG_PORT_ANY, 0, *reg32_val));
                } else {
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, REG_PORT_ANY, 0, reg32_val));
                    SHR_BITSET(reg32_val, lane / 4);
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, REG_PORT_ANY, 0, *reg32_val));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_control_tx_nif_enable_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int        enable)
{
    uint32 tm_port, base_q_pair, curr_q_pair, nof_q_pairs;
    int core;
    soc_reg_above_64_val_t data_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, data_above_64));

    /* disable or enable all queue pairs */
    for (curr_q_pair = base_q_pair; curr_q_pair < (base_q_pair + nof_q_pairs); curr_q_pair++) {
        if (enable) {
            SHR_BITCLR(data_above_64, curr_q_pair); /* unicast queue */
            SHR_BITCLR(data_above_64, ARAD_EGR_NOF_BASE_Q_PAIRS+curr_q_pair); /* multicast queue */
        } else {
            SHR_BITSET(data_above_64, curr_q_pair); /* unicast queue */
            SHR_BITSET(data_above_64, ARAD_EGR_NOF_BASE_Q_PAIRS+curr_q_pair); /* multicast queue */
        }
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, data_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_control_tx_nif_enable_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int        *enable)
{
    uint32 tm_port, base_q_pair;
    int core;
    soc_reg_above_64_val_t data_above_64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_base_q_pair_get(unit, port, &base_q_pair));

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, data_above_64));

    /* read base queue pair bit */
    *enable = !(SHR_BITGET(data_above_64, base_q_pair));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_nrdy_th_profile_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 profile)
{
    uint32 tm_port, egr_if;
    soc_reg_above_64_val_t data;
    uint32 th_profile;
    uint32 flags;
    int core;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags)); 
    if (SOC_PORT_IS_STAT_INTERFACE(flags))
    {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));

    SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
    th_profile = profile;
    soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NRDY_TH_SELf, &th_profile);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_nrdy_th_profile_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *profile)
{
    uint32 tm_port, egr_if;
    soc_reg_above_64_val_t data;
    int core;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));

    SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
    soc_mem_field_get(unit, EGQ_PER_IFC_CFGm, data, NRDY_TH_SELf, profile);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_nrdy_th_optimial_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value)
{
    uint32 num_of_lanes, if_rate_mbps;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OAMP) {
        *value = 27;
    } else if (interface == SOC_PORT_IF_OLP) {
        *value = 14;
    } else if (interface == SOC_PORT_IF_RCY) {
        *value = 230;
    } else if (interface == SOC_PORT_IF_ILKN) {
        if (if_rate_mbps <= 150000) {
            *value = 128;
        } else { /* if_rate_mpbs <= 600000 */
            *value = 256;
        }
    } else if (interface == SOC_PORT_IF_QSGMII) {
        *value = 26;
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

        if (num_of_lanes == 1) {
            if (if_rate_mbps <= 10000) {
                *value = 112;
            } else  { /* if_rate_mpbs <= 25000 */
                *value = 96;
            }

        } else if (num_of_lanes == 2) {
            if (if_rate_mbps <= 10000) {
                *value = 240;
            } else if (if_rate_mbps <= 20000) {
                *value = 224;
            } else if (if_rate_mbps <= 40000) {
                *value = 192;
            } else { /* if_rate_mpbs <= 50000 */
                *value = 180;
            }

        } else if (num_of_lanes == 4) {
            if (if_rate_mbps <= 10000) {
                *value = 496;
            } else if (if_rate_mbps <= 20000) {
                *value = 480;
            } else if (if_rate_mbps <= 40000) {
                *value = 448;
            } else if (if_rate_mbps <= 50000) {
                *value = 436;
            } else { /* if_rate_mpbs <= 100000 */
                *value = 384;
            }
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_mirrored_channel_and_context_map(int unit, int core, uint32 termination_context,
                                             uint32 reassembly_context, uint32 channel)
{
    uint32 ctxt_id, index, base_ctxt_id, data, ctxt_channelized, ctxt_addr;

    SOCDNX_INIT_FUNC_DEFS;
 
    base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(core);
    ctxt_id = base_ctxt_id + channel;
    index = SOC_JER_RCY_INTERFACE(core);

    data = 0;
    ctxt_channelized = 0;
    ctxt_addr = SOC_JER_INVALID_CTXT_ADDRESS;
    soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_IS_CHANNELIZEDf, &ctxt_channelized);
    soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf, &ctxt_addr);  
    soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, CTXT_MAP_BASE_ADDRESSf, &base_ctxt_id);  
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_PORT_TO_BASE_ADDRESS_MAPm(unit, MEM_BLOCK_ANY, index, &data));


    data = 0;
    soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &termination_context);
    soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &termination_context);
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));

exit:
    SOCDNX_FUNC_RETURN;
}



/*********************************************************************
* NAME:
*     arad_port_swap_global_info_[g|s]et
* TYPE:
*   PROC
* FUNCTION:
*     Set or get the global port swap configuration. 
* INPUT:
*  int                              unit -
*       Identifier of the device to access.
*  soc_ppc_port_swap_global_info    ports_swap_info -
*       Port swap global info to set / get.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

soc_error_t 
soc_jer_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_64);

    /* Fill the buffer */        
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, GLOBAL_TAG_SWAP_N_SIZEf, ports_swap_info->global_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_OFFSET_0f, ports_swap_info->tag_swap_n_offset_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_OFFSET_1f, ports_swap_info->tag_swap_n_offset_1);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_0_TAG_SWAP_N_SIZEf, ports_swap_info->tpid_0_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_1_TAG_SWAP_N_SIZEf, ports_swap_info->tpid_1_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_TPID_0f, ports_swap_info->tag_swap_n_tpid_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_TPID_1f, ports_swap_info->tag_swap_n_tpid_1);

    /* Write to the register. It's an array register, duplicated by number of cores. */
    for (i = 0 ; i < SOC_DPP_DEFS_GET(unit, nof_cores) ; i++) {
        rv = WRITE_IRE_TAG_SWAP_CONFIGURATIONr(unit, i, reg_val_64);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_jer_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;

    SOCDNX_INIT_FUNC_DEFS;

    /* Read from the register. */
    rv = READ_IRE_TAG_SWAP_CONFIGURATIONr(unit, IRE_BLOCK(unit), &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);


    /* Fill the buffer */        
    ports_swap_info->global_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, GLOBAL_TAG_SWAP_N_SIZEf);   
    ports_swap_info->tag_swap_n_offset_0    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_OFFSET_0f);  
    ports_swap_info->tag_swap_n_offset_1    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_OFFSET_1f);
    ports_swap_info->tpid_0_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_0_TAG_SWAP_N_SIZEf);
    ports_swap_info->tpid_1_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_1_TAG_SWAP_N_SIZEf);
    ports_swap_info->tag_swap_n_tpid_0      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_TPID_0f);
    ports_swap_info->tag_swap_n_tpid_1      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_TPID_1f);

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

