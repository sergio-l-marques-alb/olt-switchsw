/*
 * $Id: jer_init.c Exp $
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
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/* 
 * Includes
 */ 

#include <shared/bsl.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_mem.h>

/* SOC DPP JER includes */
#include <soc/dpp/JER/jer_drv.h>
#include <soc/dpp/JER/jer_defs.h>
#include <soc/dpp/JER/jer_init.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_reset.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_trunk.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/JER/JER_PP/jer_pp_ing_protection.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_protection.h>

#include <soc/dpp/JER/jer_regs.h>
#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/JER/jer_ofp_rates.h>
#include <soc/dpp/JER/jer_sch.h>

#include <soc/dpp/JER/JER_PP/jer_pp_lbp_init.h>


/* SOC DPP Arad includes */ 
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_pmf_low_level.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/multicast_imp.h>

#include <soc/dpp/port_sw_db.h>

#ifdef PLISIM
    #include <soc/dpp/ARAD/arad_sim_em.h>
#endif

/* alternating <block>_INDIRECT_COMMANDr and <block>_INDIRECT_FORCE_BUBBLEr for bubble configuration */
static soc_reg_t jer_blocks_with_standart_bubble_mechanism[] = {
    IRE_INDIRECT_COMMANDr,        IRE_INDIRECT_FORCE_BUBBLEr,
    IDR_INDIRECT_COMMANDr,        IDR_INDIRECT_FORCE_BUBBLEr,
    MRPS_INDIRECT_COMMANDr,       MRPS_INDIRECT_FORCE_BUBBLEr,
    IRR_INDIRECT_COMMANDr,        IRR_INDIRECT_FORCE_BUBBLEr,
    OCB_INDIRECT_COMMANDr,        OCB_INDIRECT_FORCE_BUBBLEr,
    IQM_INDIRECT_COMMANDr,        IQM_INDIRECT_FORCE_BUBBLEr,
    IQMT_INDIRECT_COMMANDr,       IQMT_INDIRECT_FORCE_BUBBLEr,
    CRPS_INDIRECT_COMMANDr,       CRPS_INDIRECT_FORCE_BUBBLEr,
    IPS_INDIRECT_COMMANDr,        IPS_INDIRECT_FORCE_BUBBLEr,
    IPST_INDIRECT_COMMANDr,       IPST_INDIRECT_FORCE_BUBBLEr,
    IPT_INDIRECT_COMMANDr,        IPT_INDIRECT_FORCE_BUBBLEr,
    RTP_INDIRECT_COMMANDr,        RTP_INDIRECT_FORCE_BUBBLEr,
    EGQ_INDIRECT_COMMANDr,        EGQ_INDIRECT_FORCE_BUBBLEr,
    EPNI_INDIRECT_COMMANDr,       EPNI_INDIRECT_FORCE_BUBBLEr,
    OLP_INDIRECT_COMMANDr,        OLP_INDIRECT_FORCE_BUBBLEr,
    IHP_INDIRECT_COMMANDr,        IHP_INDIRECT_FORCE_BUBBLEr,
    IHB_INDIRECT_COMMANDr,        IHB_INDIRECT_FORCE_BUBBLEr,

    /* last reg in array */
    INVALIDr
};


/* 
 * Init functions
 */

int soc_jer_init_prepare_internal_data(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    /* Calculate boundaries */
    SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_conf_calc(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


/* excludes irrelevant blocks which are missing due to partial emul compilation */
STATIC void soc_jer_init_exclude_blocks(int unit)
{
    SOC_INFO(unit).block_valid[CLP_BLOCK(unit, 3)] = FALSE;
    SOC_INFO(unit).block_valid[CLP_BLOCK(unit, 4)] = FALSE;
    SOC_INFO(unit).block_valid[CLP_BLOCK(unit, 5)] = FALSE;
    SOC_INFO(unit).block_valid[NBIL_BLOCK(unit, 0)] = FALSE;
    SOC_INFO(unit).block_valid[NBIL_BLOCK(unit, 1)] = FALSE;
    SOC_INFO(unit).block_valid[MMU_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCA_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCB_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCC_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCD_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCE_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCF_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCG_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCH_BLOCK(unit)] = FALSE;
    SOC_INFO(unit).block_valid[DRCBROADCAST_BLOCK(unit)] = FALSE;
}


int soc_jer_init_blocks_init(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    /* exclude irrelevant blocks which are missing due to partial emul compilation - controlled via custom soc property */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "exclude_blocks_missing_in_partial_emul_compilation", 0)) {
        soc_jer_init_exclude_blocks(unit);
    }

    /* access check to blocks after reset*/
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check(unit));

    /* Init blocks' broadcast IDs */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_brdc_blk_id_set(unit));

    /* Configure PLL and SyncE */
    SOCDNX_IF_ERR_EXIT(jer_pll_synce_init(unit));

exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_init_bubble_config_standard_blocks
 * Purpose:
 *      makes all the needed configurations for blocks with the common bubble mechanism
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_init_bubble_config_standard_blocks (int unit)
{
    uint32 reg = 0;
    int iter = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_cpu", 1) == 1) {
        /* iterate over bubble registers array and act */
        while (jer_blocks_with_standart_bubble_mechanism[iter] != INVALIDr) {

            /* set block indirect command timeout period */
            SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, jer_blocks_with_standart_bubble_mechanism[iter++], REG_PORT_ANY, INDIRECT_COMMAND_TIMEOUTf, 0x10));

            /* set block bubble mechanism */
            reg = 0;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, jer_blocks_with_standart_bubble_mechanism[iter], REG_PORT_ANY, 0, &reg));
            soc_reg_field_set (unit, jer_blocks_with_standart_bubble_mechanism[iter], &reg, FORCE_BUBBLE_PERIODf, 0x8);
            soc_reg_field_set (unit, jer_blocks_with_standart_bubble_mechanism[iter], &reg, FORCE_BUBBLE_ENf, 0x1);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, jer_blocks_with_standart_bubble_mechanism[iter++], REG_PORT_ANY, 0, reg));

        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_init_bubble_config_specific_egr
 * Purpose:
 *      special configurations needed for egr
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 * NOTES: 
 *      if the need will arise, this is the way a special bubble configuration function should look like
 */
int soc_jer_init_bubble_config_specific_egr (int unit)
{
    uint32 conf_reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_egr", 1) == 1) {
        /* EGQ_FQP_BUBBLE_CONFIGURATION configuration - used to create bubble in EPNI block */
        SOCDNX_IF_ERR_EXIT(READ_EGQ_FQP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, &conf_reg_val));
        /* Enable constant bubble generation every 32*BubbleDelay clocks.*/
        soc_reg_field_set(unit, EGQ_FQP_BUBBLE_CONFIGURATIONr, &conf_reg_val, FQP_CONST_BUBBLE_ENf, 1);
        /* The minimum delay between the bubble request to the bubble */
        soc_reg_field_set(unit, EGQ_FQP_BUBBLE_CONFIGURATIONr, &conf_reg_val, FQP_BUBBLE_DELAYf, 31);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_FQP_BUBBLE_CONFIGURATIONr(unit, REG_PORT_ANY, conf_reg_val));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_init_bubble_config_specific_ing
 * Purpose:
 *      special configurations needed for ingress
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 * NOTES: 
 *      if the need will arise, this is the way a special bubble configuration function should look like
 */
int soc_jer_init_bubble_config_specific_ing (int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bubble_ing", 1) == 1) {
        /* limit packet rate by bubbles injected every SyncCounter clock cycles. */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IHP_IHP_ENABLERSr, SOC_CORE_ALL, 0, FORCE_BUBBLESf,  1));
        /* every SyncCounter number of clocks a bubble will be inserted to the IHP pipe */
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IHP_SYNC_COUNTERr, SOC_CORE_ALL, 0, SYNC_COUNTERf,  0x3ff));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_jer_init_bubble_configuration
 * Purpose:
 *      configures the bubble configuration for all relevant blocks
 *      bubble - a mechanism to allow cpu to access S-BUS, even though the design has priority over it.
 *               the method of doing it is forcing a "bubble" of inactivity to allow the cpu transaction enter.
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_init_bubble_config (int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Config bubble mechanism for blocks with the common mechanism */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config_standard_blocks(unit));

    /* specific blocks bubble configuration - currently, no such functions */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config_specific_egr(unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config_specific_ing(unit));
    
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_init_blocks_conf(int unit)
{

    uint32 reg32;

    SOCDNX_INIT_FUNC_DEFS;

    /* configure dual core mode, not in clear-channel mode */
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, TURBO_PIPEf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, DUAL_FAP_MODEf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32, FMC_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_1r(unit, &reg32));
      /*
       *  Mesh Mode
       */
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_MESH || 
        /*treating single fap as mesh for tables configuration*/
        SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP )
    {
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32, MESH_MODEf, 1);
    }
    else
    {
        /* Mesh Not enabled, also for BACK2BACK devices */
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32, MESH_MODEf, 0);
    }


    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32, RESERVED_QTSf, 2);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32, TDM_ATTRIBUTEf, 0x180);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_1r(unit, reg32));
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_3r(unit, &reg32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32, PACKET_CRC_ENf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32, OPPORTUNISTIC_CRC_ENf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_3r, &reg32, TOD_MODEf, 3);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_3r(unit, reg32));
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_4r(unit, &reg32));
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_4r, &reg32, IPS_DEQ_CMD_RESf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_4r, &reg32, SINGLE_STAT_PORTf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_4r(unit, reg32));

    /* Configure OCB and Dram Buffers */
    SOCDNX_IF_ERR_EXIT(soc_jer_ocb_dram_buffer_conf_set(unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_ocb_conf_set(unit));

    /* Set bubble configuration */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_bubble_config(unit));

    /* Setting revision fixes Bits */
    SOCDNX_IF_ERR_EXIT(jer_mgmt_revision_fixes(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_hw_interfaces_set(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->arad->init.drc_info.enable) { 
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_dram_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Port initialization */
STATIC int soc_jer_init_port(int unit)
{
    uint32 soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_mgmt_pon_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);    

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int soc_jer_init_mesh_topology(int unit)
{
    uint32 soc_sand_rv = 0, reg_val, field_val = 0;
    ARAD_INIT_FABRIC *fabric;
    SOCDNX_INIT_FUNC_DEFS;

    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric);

    soc_sand_rv = arad_init_mesh_topology(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(fabric->is_128_in_system) {
        field_val = 0x1;
    } else if (fabric->system_contains_multiple_pipe_device) {
        field_val = 0x2;
    } else {
        field_val = 0x0;
    }
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_0117r(unit, &reg_val));
    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_0117r, &reg_val, FIELD_4_8f, field_val);
    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_0117r, &reg_val, FIELD_9_9f, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_0117r(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


/* Modules initialization */
STATIC int soc_jer_init_functional_init(int unit)
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 soc_sand_rv = 0, rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_jer_init_mesh_topology(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    rv = soc_jer_ipt_contexts_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_jer_fabric_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = arad_pmf_low_level_init_unsafe(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);    

    soc_sand_rv = arad_parser_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);    

    soc_sand_rv = arad_ports_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_jer_trunk_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);  

    soc_sand_rv = arad_egr_prog_editor_unsafe(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);  

    soc_sand_rv = soc_jer_lbp_init(unit); 
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv); 

    if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable == FALSE) {
        soc_sand_rv = arad_pp_isem_access_init_unsafe(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);    

        /* FLP - init only if TM */
        arad_pp_flp_prog_sel_cam_key_program_tm(unit);
        arad_pp_flp_process_key_program_tm(unit);

        soc_sand_rv = arad_pp_trap_mgmt_init_unsafe(unit);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);    
    }

    {   

        int i;
        uint32 reg32, m[7] = {0};
        uint64 reg64;
        soc_reg_above_64_val_t reg_above_64 = {0, 0x2008000, 0x10, 0, 0x8000000, 0, 0, 0x4, 0x4002, 0x80000000};
        /* A debug variable for emulation (this will be the maximal configured queue). */
        const int emulation_debug_max_queue_num = 10000;

        /* further IDR configuration */
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, reg_above_64));
        soc_mem_field32_set(unit, IDR_CONTEXT_MRUm, m, MAX_SIZEf, 0x3f80);
        soc_mem_field32_set(unit, IDR_CONTEXT_MRUm, m, MAX_ORG_SIZEf, 0x3f80);
        soc_mem_field32_set(unit, IDR_CONTEXT_MRUm, m, MIN_ORG_SIZEf, 0x20);
        soc_mem_field32_set(unit, IDR_CONTEXT_MRUm, m, MIN_SIZEf, 0x20);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IDR_CONTEXT_MRUm, MEM_BLOCK_ALL, m));

        m[0] = 0;
        for (i = 0; i < 4; ++i) {
            int j;
            soc_mem_field32_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, m, INGRESS_DROP_PRECEDENCEf, i);
            soc_mem_field32_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, m, EGRESS_DROP_PRECEDENCEf, i);
            for (j = i * 16; j < 256; j += 64) {
                SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IDR_DROP_PRECEDENCE_MAPPINGm, 0, 0, MEM_BLOCK_ALL, j, j+15, m));
            }
        }


        /* various enablers, traffic disabled */

        reg32 = 0;
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32, FORCE_ALL_LOCAL_Bf, 1);
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32, FORCE_ALL_LOCALf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32));

        reg32 = 0;
        soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg32, FIELD_24_24f, 1);
        soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg32, FIELD_25_25f, 1);
        soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg32, FIELD_2_2f, 1);
        soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg32, FORCE_ALL_LOCALf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_FDT_ENABLER_REGISTERr(unit, reg32));

        COMPILER_64_ZERO(reg64);
        soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_0_18f, 0x800);
        soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64, FIELD_23_30f, 0xff);
        SOCDNX_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64));

        reg32 = 0;
        soc_reg_field_set(unit, FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr, &reg32, FIELD_15_15f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FCR_FCR_ENABLERS_AND_FILTER_MATCH_INPUT_LINKr(unit, reg32));

        SOCDNX_IF_ERR_EXIT(WRITE_IRE_DYNAMIC_CONFIGURATIONr(unit, 0));

        reg32 = 0;
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32, RCM_MODEf, 2);
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32, FIELD_11_11f, 1);
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32, FIELD_13_17f, 0xe);
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32, FIELD_18_18f, 1);
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32, FIELD_19_19f, 1);
        soc_reg_field_set(unit, EGQ_GENERAL_RQP_CONFIGr, &reg32, FIELD_20_20f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_GENERAL_RQP_CONFIGr(unit, SOC_CORE_ALL, reg32));

        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, FLMC_4K_REP_ENf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, DSCRD_ALL_PKTf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, VSQ_SIZE_MODEf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, DSCRD_DPf, 4);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, IGNORE_DPf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, DEQ_CACHE_ENH_RR_ENf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, CNGQ_ON_BUFF_ENf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, VSQ_ISP_UPD_ENf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, SET_TDM_Q_PER_QSIGNf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, EN_IPT_CD_4_SNOOPf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, VSQ_TH_MODE_SELf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, PDM_INIT_ENf, 1);
        soc_reg_field_set(unit, IQM_IQM_ENABLERSr, &reg32, ISP_CD_SCND_CP_ENf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_ENABLERSr(unit, SOC_CORE_ALL, reg32));

        /* ingress queueing configuration */
        COMPILER_64_ZERO(reg64); 
        soc_reg64_field32_set(unit, IDR_OCB_COMMITTED_MULTICAST_RANGEr, &reg64, OCB_COMMITTED_MULTICAST_RANGE_N_HIGHf, 0x17fff);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_OCB_COMMITTED_MULTICAST_RANGEr(unit, 0, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_OCB_COMMITTED_MULTICAST_RANGEr(unit, 1, reg64));

        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_REJECT_CONFIGURATION_OCBSr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr(unit, SOC_CORE_ALL, reg_above_64));
        reg_above_64[5] = reg_above_64[4] = reg_above_64[3] = 0xffffffff;
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_0_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_1_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_0_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_1_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        COMPILER_64_SET(reg64, 0xffff, 0xffffffff);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_0_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_1_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_0_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_1_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        reg_above_64[2] = reg_above_64[1] = reg_above_64[0] = 0xffffffff;
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_GENERAL_REJECT_CONFIGURATION_B_DSr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_BDB_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_DEQ_INTERNAL_SETTINGr(unit, SOC_CORE_ALL, 0xffff));

        /* configure queues */
        m[0] = 0;
        /* We assume these are always zero at init. */
        /* SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IQM_PQDMSm, 0, 0, IQM_BLOCK(unit, 0), 0, emulation_debug_max_queue_num, m)); */
        /* SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IPS_QUEUE_TYPE_LOOKUP_TABLEm, 0, 0,  MEM_BLOCK_ALL, 0, emulation_debug_max_queue_num, m)); */
        m[1] = m[0] = 0;
        soc_mem_field32_set(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m, COMMITMENTf, 0xffffffff); 
        SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, 0, 0, MEM_BLOCK_ALL, 0, emulation_debug_max_queue_num, m));
        m[0] = 3;
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_QUEUE_PRIORITY_TABLEm(unit, MEM_BLOCK_ALL, 0, m));

        m[3] = m[2] = m[1] = m[0] = 0; /* PQ_WRED_ENf=0 */
        soc_mem_field32_set(unit, IQM_PQREDm, m, PQ_MAX_QUE_SIZEf, 0x1801);
        soc_mem_field32_set(unit, IQM_PQREDm, m, PQ_MAX_QUE_BUFF_SIZEf, 0xf01);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, IQM_PQREDm, 0, 0, MEM_BLOCK_ALL, 0, 3, m));

        /* source based ingress congestion management */
        reg_above_64[5] = reg_above_64[4] = reg_above_64[3] = 0;
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_1_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_1_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_0_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_0_RSRC_RJCT_THr(unit, SOC_CORE_ALL, reg_above_64));
        reg_above_64[2] = 3;
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_GLBL_MIX_MAX_THr(unit, SOC_CORE_ALL, reg_above_64));

        SOCDNX_IF_ERR_EXIT(WRITE_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_1_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_1_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_0_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_0_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));

        m[0] = 0;
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_IPPPMm, MEM_BLOCK_ALL, m));
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_PG_TC_BITMAPm, MEM_BLOCK_ALL, m));
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_VSQF_RCm(unit, MEM_BLOCK_ALL, 0, m)); /* VSQ_RATE_CLASSf=0 */
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_VSQF_FC_PRMm(unit, MEM_BLOCK_ALL, 0, m)); /* SET_THRESHOLD_BDSf=0 CLR_THRESHOLD_BDSf=0 */


        /* Init IQM */
        reg32 = 0;
        soc_reg_field_set(unit, IQM_IQM_INITr, &reg32, PDM_INITf, 0);
        soc_reg_field_set(unit, IQM_IQM_INITr, &reg32, STE_INITf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_INITr(unit, SOC_CORE_ALL, reg32));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IPT_ENABLESr, REG_PORT_ANY, 0, IPT_ENf, 0));

#if 0 /* configured elsewhere */
        reg32 = 0;
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_MINI_MULTICAST_0_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_OCB_1_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_FULL_MULTICAST_1_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, MRPS_ETHERNET_METER_DROP_COLORf, 3);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_MINI_MULTICAST_1_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_OCB_0_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_FULL_MULTICAST_0_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_INTERNAL_REUSEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, MAX_DP_THRESHOLDf, 0x27);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_STATIC_CONFIGURATIONr(unit, reg32));
#endif

        /* EGQ PQP thresholds bypass configuration */
        soc_mem_field32_set(unit, EGQ_PDCT_TABLEm, m, PORT_UC_PD_DIS_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PDCT_TABLEm, m, PORT_MC_PD_SHARED_MAX_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PDCT_TABLEm, m, PORT_MC_PD_SHARED_MIN_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PDCT_TABLEm, m, PORT_UC_PD_MAX_FC_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PDCT_TABLEm, m, PORT_UC_PD_MIN_FC_THf, 0x7fff); 
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PDCT_TABLEm, MEM_BLOCK_ALL, m));

        m[2] = m[1] = m[0] = 0;
        soc_mem_field32_set(unit, EGQ_PQST_TABLEm, m, PORT_UC_DB_DIS_THf, 0x3fff); 
        soc_mem_field32_set(unit, EGQ_PQST_TABLEm, m, PORT_MC_DB_SHARED_THf, 0x3fff); 
        soc_mem_field32_set(unit, EGQ_PQST_TABLEm, m, PORT_UC_DB_MAX_FC_THf, 0x3fff); 
        soc_mem_field32_set(unit, EGQ_PQST_TABLEm, m, PORT_UC_DB_MIN_FC_THf, 0x3fff); 
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PQST_TABLEm, MEM_BLOCK_ALL, m));

        m[3] = m[2] = m[1] = m[0] = 0;
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_UC_PD_MAX_FC_THf, 100);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_UC_PD_MIN_FC_THf, 100);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MAX_TH_DP_3f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MAX_TH_DP_2f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MAX_TH_DP_1f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MAX_TH_DP_0f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MIN_TH_DP_2f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MIN_TH_DP_1f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_MIN_TH_DP_0f, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_MC_PD_RSVD_THf, 0xfff);
        soc_mem_field32_set(unit, EGQ_QDCT_TABLEm, m, QUEUE_UC_PD_DIS_THf, 0xfff);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_QDCT_TABLEm, MEM_BLOCK_ALL, m));

        m[6] = m[5] = m[4] = m[3] = m[2] = m[1] = m[0] = 0;
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_UC_DB_MAX_FC_THf, 160);
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_UC_DB_MIN_FC_THf, 160);
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_MC_DB_DP_3_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_MC_DB_DP_2_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_MC_DB_DP_1_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_MC_DB_DP_0_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_QQST_TABLEm, m, QUEUE_UC_DB_DIS_THf, 0x3fff);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_QQST_TABLEm, MEM_BLOCK_ALL, m));


        m[0] = 0; /* EGRESS_TCf=0 */
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_TC_DP_MAPm, MEM_BLOCK_ALL, m));

        /*  NBIH_EGQ_CORE_FOR_NIF_QMLF set bit for core number (quartet) */
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_PORTS_SRSTNr(unit, 0xfffff));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_PORTS_SRSTNr(unit, 0xfffff));

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 0, TX_FLUSH_EGRESS_PORT_0_MLF_0_QMLF_Nf, 1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 1, TX_FLUSH_EGRESS_PORT_0_MLF_0_QMLF_Nf, 1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 2, TX_FLUSH_EGRESS_PORT_0_MLF_0_QMLF_Nf, 1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 2, TX_FLUSH_EGRESS_PORT_0_MLF_1_QMLF_Nf, 1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 2, TX_FLUSH_EGRESS_PORT_0_MLF_2_QMLF_Nf, 1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 2, TX_FLUSH_EGRESS_PORT_0_MLF_3_QMLF_Nf, 1));

        /* PP bypasses */

        /* Egress bypass: define PMF to forward - this is what passes regular packets */
        m[0] = 0;
        soc_mem_field32_set(unit, EPNI_PMF_MIRROR_PROFILE_TABLEm, m, FWD_ENABLEf, 1);
        soc_mem_field32_set(unit, EPNI_PMF_MIRROR_PROFILE_TABLEm, m, FWD_STRENGTHf, 3);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EPNI_PMF_MIRROR_PROFILE_TABLEm, MEM_BLOCK_ALL, m));

        SOCDNX_IF_ERR_EXIT(jer_mgmt_system_fap_id_set(unit, 0x102)); /* set FAP ID and local traffic through local path */
    }

    {   
        uint32 reg32;

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, 0, 0, DBG_FDA_STOPf, 0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_GENERAL_RQP_DEBUG_CONFIGr, 1, 0, DBG_FDA_STOPf, 0));

        reg32 = 0;
        soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg32, UPDATE_MAX_QSZ_FROM_LOCALf, 1);
        soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg32, SEND_FSM_FOR_BIGGER_MAX_QUEUE_SIZEf, 1);
        soc_reg_field_set(unit, IPS_IPS_GENERAL_CONFIGURATIONSr, &reg32, ENABLE_SYSTEM_REDf, 1); /* indirect Q mapping mode */
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_IPS_GENERAL_CONFIGURATIONSr(unit, SOC_CORE_ALL, reg32));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IPT_ENABLESr, REG_PORT_ANY, 0, IPT_ENf, 1));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, 0, 0, DSCRD_ALL_PKTf, 0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, 1, 0, DSCRD_ALL_PKTf, 0));

        
        SOCDNX_IF_ERR_EXIT(READ_IDR_STATIC_CONFIGURATIONr(unit, &reg32));
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_OCB_0_AUTOGEN_ENABLEf, 0);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_OCB_1_AUTOGEN_ENABLEf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_STATIC_CONFIGURATIONr(unit, reg32));
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_OCB_0_AUTOGEN_ENABLEf, 1);
        soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &reg32, FBC_OCB_1_AUTOGEN_ENABLEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IDR_STATIC_CONFIGURATIONr(unit, reg32));

        SOCDNX_IF_ERR_EXIT(WRITE_IRR_DYNAMIC_CONFIGURATIONr(unit, 0x3f));

        reg32 = 0;
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_DATA_PATHf, 1);
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_TCAM_BUBBLESf, 1);
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_MACT_BUBBLESf, 1);
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, NON_FIRST_FRAGMENTS_ENABLEf, 1);
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, MAX_HEADER_STACKf, 5);
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_IHP_ENABLERSr(unit, SOC_CORE_ALL, reg32));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IDR_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, 0, ENABLE_DATA_PATHf, 1));
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_DYNAMIC_CONFIGURATIONr(unit, 1));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));
    SOC_PBMP_ITER(pbmp, port_i) {
        SOCDNX_IF_ERR_EXIT(arad_ports_header_type_update(unit, port_i));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_jer_pp_mgmt_functional_init
 * Purpose:
 *      Perform various module initalization for PP modules that require
 *      Jericho specific configuration
 *      The function is called from arad_pp_mgmt_functional_init_unsafe.
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
soc_error_t soc_jer_pp_mgmt_functional_init(
    int unit)
{
    uint32 soc_sand_rv = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /* Protection initialization */
    soc_sand_rv = soc_jer_pp_ing_protection_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_jer_pp_eg_protection_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;

}



/*
 * Function:
 *      soc_jer_tbls_init
 * Purpose:
 *      initialize all tables relevant for Jericho.
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_tbls_init(
    int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "static_tbl_full_init", 0)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_static_tbls_reset(unit)); 
    }
        
    SOCDNX_IF_ERR_EXIT(arad_tbl_access_init_unsafe(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_sch_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_irr_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_ire_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_ihb_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_iqm_tbls_init(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_jer_ips_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_ipt_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_fdt_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_egq_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_epni_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(dpp_mult_rplct_tbl_entry_unoccupied_set_all(unit));
    SOCDNX_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));

exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_tbls_deinit
 * Purpose:
 *      de-initialize all tables relevant for Jericho.
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_tbls_deinit(
    int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(arad_tbl_access_deinit(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


CONST STATIC int dtq_max_size_contexts[] = {DTQ_0_MAX_SIZE_0f, DTQ_0_MAX_SIZE_1f, DTQ_0_MAX_SIZE_2f, DTQ_0_MAX_SIZE_3f, DTQ_0_MAX_SIZE_4f, DTQ_0_MAX_SIZE_5f,
                                            DTQ_1_MAX_SIZE_0f, DTQ_1_MAX_SIZE_1f, DTQ_1_MAX_SIZE_2f, DTQ_1_MAX_SIZE_3f, DTQ_1_MAX_SIZE_4f, DTQ_1_MAX_SIZE_5f};
CONST STATIC int dtq_start_addr_contexts[] = {DTQ_0_START_0f, DTQ_0_START_1f, DTQ_0_START_2f, DTQ_0_START_3f, DTQ_0_START_4f, DTQ_0_START_5f,
                                              DTQ_1_START_0f, DTQ_1_START_1f, DTQ_1_START_2f, DTQ_1_START_3f, DTQ_1_START_4f, DTQ_1_START_5f};
CONST STATIC int dtq_th_contexts[] = {DTQ_0_TH_0f, DTQ_0_TH_1f, DTQ_0_TH_2f, DTQ_0_TH_3f, DTQ_0_TH_4f, DTQ_0_TH_5f,
                                      DTQ_1_TH_0f, DTQ_1_TH_1f, DTQ_1_TH_2f, DTQ_1_TH_3f, DTQ_1_TH_4f, DTQ_1_TH_5f};
CONST STATIC int dtq_dqcf_th_contexts[] = {DTQ_0_DQCF_TH_0f, DTQ_0_DQCF_TH_1f, DTQ_0_DQCF_TH_2f, DTQ_0_DQCF_TH_3f, DTQ_0_DQCF_TH_4f, DTQ_0_DQCF_TH_5f, 
                                           DTQ_1_DQCF_TH_0f, DTQ_1_DQCF_TH_1f, DTQ_1_DQCF_TH_2f, DTQ_1_DQCF_TH_3f, DTQ_1_DQCF_TH_4f, DTQ_1_DQCF_TH_5f};
CONST STATIC int dqcf_0_max_size_contexts[] = {DQCF_0_MAX_SIZE_0f, DQCF_0_MAX_SIZE_1f, DQCF_0_MAX_SIZE_2f, DQCF_0_MAX_SIZE_3f, DQCF_0_MAX_SIZE_4f, DQCF_0_MAX_SIZE_5f, DQCF_0_MAX_SIZE_6f, 
                                               DQCF_0_MAX_SIZE_7f, DQCF_0_MAX_SIZE_8f, DQCF_0_MAX_SIZE_9f, DQCF_0_MAX_SIZE_10f, DQCF_0_MAX_SIZE_11f, DQCF_0_MAX_SIZE_12f, DQCF_0_MAX_SIZE_13f,
                                               DQCF_0_MAX_SIZE_14f, DQCF_0_MAX_SIZE_15f, DQCF_0_MAX_SIZE_16f, DQCF_0_MAX_SIZE_17f, DQCF_0_MAX_SIZE_18f, DQCF_0_MAX_SIZE_19f};
CONST STATIC int dqcf_1_max_size_contexts[] = {DQCF_1_MAX_SIZE_0f, DQCF_1_MAX_SIZE_1f, DQCF_1_MAX_SIZE_2f, DQCF_1_MAX_SIZE_3f, DQCF_1_MAX_SIZE_4f, DQCF_1_MAX_SIZE_5f, DQCF_1_MAX_SIZE_6f, 
                                               DQCF_1_MAX_SIZE_7f, DQCF_1_MAX_SIZE_8f, DQCF_1_MAX_SIZE_9f, DQCF_1_MAX_SIZE_10f, DQCF_1_MAX_SIZE_11f, DQCF_1_MAX_SIZE_12f, DQCF_1_MAX_SIZE_13f,
                                               DQCF_1_MAX_SIZE_14f, DQCF_1_MAX_SIZE_15f, DQCF_1_MAX_SIZE_16f, DQCF_1_MAX_SIZE_17f, DQCF_1_MAX_SIZE_18f, DQCF_1_MAX_SIZE_19f};
CONST STATIC int dqcf_0_start_addr_contexts[] = {DQCF_0_START_0f, DQCF_0_START_1f, DQCF_0_START_2f, DQCF_0_START_3f, DQCF_0_START_4f, DQCF_0_START_5f, DQCF_0_START_6f, 
                                                 DQCF_0_START_7f, DQCF_0_START_8f, DQCF_0_START_9f, DQCF_0_START_10f, DQCF_0_START_11f, DQCF_0_START_12f, DQCF_0_START_13f, 
                                                 DQCF_0_START_14f, DQCF_0_START_15f, DQCF_0_START_16f, DQCF_0_START_17f, DQCF_0_START_18f, DQCF_0_START_19f};
CONST STATIC int dqcf_1_start_addr_contexts[] = {DQCF_1_START_0f, DQCF_1_START_1f, DQCF_1_START_2f, DQCF_1_START_3f, DQCF_1_START_4f, DQCF_1_START_5f, DQCF_1_START_6f, 
                                                 DQCF_1_START_7f, DQCF_1_START_8f, DQCF_1_START_9f, DQCF_1_START_10f, DQCF_1_START_11f, DQCF_1_START_12f, DQCF_1_START_13f, 
                                                 DQCF_1_START_14f, DQCF_1_START_15f, DQCF_1_START_16f, DQCF_1_START_17f, DQCF_1_START_18f, DQCF_1_START_19f};
CONST STATIC int dqcf_0_th_contexts[] = {DQCF_0_DQCQ_TH_0f, DQCF_0_DQCQ_TH_1f, DQCF_0_DQCQ_TH_2f, DQCF_0_DQCQ_TH_3f, DQCF_0_DQCQ_TH_4f, DQCF_0_DQCQ_TH_5f, DQCF_0_DQCQ_TH_6f, 
                                         DQCF_0_DQCQ_TH_7f, DQCF_0_DQCQ_TH_8f, DQCF_0_DQCQ_TH_9f, DQCF_0_DQCQ_TH_10f, DQCF_0_DQCQ_TH_11f, DQCF_0_DQCQ_TH_12f, DQCF_0_DQCQ_TH_13f, 
                                         DQCF_0_DQCQ_TH_14f, DQCF_0_DQCQ_TH_15f, DQCF_0_DQCQ_TH_16f, DQCF_0_DQCQ_TH_17f, DQCF_0_DQCQ_TH_18f, DQCF_0_DQCQ_TH_19f};
CONST STATIC int dqcf_1_th_contexts[] = {DQCF_1_DQCQ_TH_0f, DQCF_1_DQCQ_TH_1f, DQCF_1_DQCQ_TH_2f, DQCF_1_DQCQ_TH_3f, DQCF_1_DQCQ_TH_4f, DQCF_1_DQCQ_TH_5f, DQCF_1_DQCQ_TH_6f, 
                                         DQCF_1_DQCQ_TH_7f, DQCF_1_DQCQ_TH_8f, DQCF_1_DQCQ_TH_9f, DQCF_1_DQCQ_TH_10f, DQCF_1_DQCQ_TH_11f, DQCF_1_DQCQ_TH_12f, DQCF_1_DQCQ_TH_13f, 
                                         DQCF_1_DQCQ_TH_14f, DQCF_1_DQCQ_TH_15f, DQCF_1_DQCQ_TH_16f, DQCF_1_DQCQ_TH_17f, DQCF_1_DQCQ_TH_18f, DQCF_1_DQCQ_TH_19f};
CONST STATIC int dqcf_0_eir_th_contexts[] = {DQCF_0_EIR_CRDT_TH_0f, DQCF_0_EIR_CRDT_TH_1f, DQCF_0_EIR_CRDT_TH_2f, DQCF_0_EIR_CRDT_TH_3f, DQCF_0_EIR_CRDT_TH_4f, DQCF_0_EIR_CRDT_TH_5f, DQCF_0_EIR_CRDT_TH_6f, 
                                             DQCF_0_EIR_CRDT_TH_7f, DQCF_0_EIR_CRDT_TH_8f, DQCF_0_EIR_CRDT_TH_9f, DQCF_0_EIR_CRDT_TH_10f, DQCF_0_EIR_CRDT_TH_11f, DQCF_0_EIR_CRDT_TH_12f, DQCF_0_EIR_CRDT_TH_13f, 
                                             DQCF_0_EIR_CRDT_TH_14f, DQCF_0_EIR_CRDT_TH_15f, DQCF_0_EIR_CRDT_TH_16f, DQCF_0_EIR_CRDT_TH_17f, DQCF_0_EIR_CRDT_TH_18f, DQCF_0_EIR_CRDT_TH_19f};
CONST STATIC int dqcf_1_eir_th_contexts[] = {DQCF_1_EIR_CRDT_TH_0f, DQCF_1_EIR_CRDT_TH_1f, DQCF_1_EIR_CRDT_TH_2f, DQCF_1_EIR_CRDT_TH_3f, DQCF_1_EIR_CRDT_TH_4f, DQCF_1_EIR_CRDT_TH_5f, DQCF_1_EIR_CRDT_TH_6f, 
                                             DQCF_1_EIR_CRDT_TH_7f, DQCF_1_EIR_CRDT_TH_8f, DQCF_1_EIR_CRDT_TH_9f, DQCF_1_EIR_CRDT_TH_10f, DQCF_1_EIR_CRDT_TH_11f, DQCF_1_EIR_CRDT_TH_12f, DQCF_1_EIR_CRDT_TH_13f, 
                                             DQCF_1_EIR_CRDT_TH_14f, DQCF_1_EIR_CRDT_TH_15f, DQCF_1_EIR_CRDT_TH_16f, DQCF_1_EIR_CRDT_TH_17f, DQCF_1_EIR_CRDT_TH_18f, DQCF_1_EIR_CRDT_TH_19f};
/* 
 * Data Transmit Queue Configuration - 
 * 12 IPT contexts: 3 pipes x OCB/OCB-DRAM-MIX x 2 IPT cores 
 * Buffer size is 2k entries, to be devided between all valid contexts.
 */
STATIC int
soc_jer_ipt_dtq_contexts_init(int unit, int is_dram, int is_dual_fap, int nof_pipes)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 dtq_max_size[12] = {0};
    uint32 *dtq_0_max_size = dtq_max_size, *dtq_1_max_size = dtq_max_size + 6;
    uint32 ctx_max_size_used, ctx_max_size_unused = 0x2;
    uint32 dtq_start_addr;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    /*max size configuration*/
    if (SOC_DPP_IS_MESH(unit)) {
        ctx_max_size_used = (is_dual_fap) ? 0x153 : 0x2a7 ;
    } else {
        switch (nof_pipes) {
        case 1:
            ctx_max_size_used = (is_dual_fap) ? 0x3f6 : 0x7f5 ;
            break;
        case 2:
            ctx_max_size_used = (is_dual_fap) ? 0x1fc : 0x3fb ;
            break;
        case 3:
            ctx_max_size_used = (is_dual_fap) ? 0x153 : 0x2a7 ;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("number of pipes %d in invalid"), nof_pipes));
        }
    }
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE; i += 2) {
        dtq_0_max_size[is_dram + i] = ctx_max_size_used;
        dtq_0_max_size[1 - is_dram + i] = ctx_max_size_unused;
        if (is_dual_fap) {
            dtq_1_max_size[is_dram + i] = ctx_max_size_used;
            dtq_1_max_size[1 - is_dram + i] = ctx_max_size_unused;
        }
    }
    if ((!SOC_DPP_IS_MESH(unit)) && (nof_pipes * 2 < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE)) {
        for (i = nof_pipes * 2; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE; ++i) {
            dtq_0_max_size[i] = ctx_max_size_unused;
            if (is_dual_fap) {
                dtq_1_max_size[i] = ctx_max_size_unused;
            }
        }
    }
    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_MAX_SIZEr(unit, reg_above_64_val)); 
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DTQ_MAX_SIZEr, reg_above_64_val, dtq_max_size_contexts[i], dtq_max_size[i]);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_MAX_SIZEr(unit, reg_above_64_val)); 

    /*queue start address*/
    dtq_start_addr = 0;
    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_STARTr(unit, reg_above_64_val)); 
    soc_reg_above_64_field32_set(unit, IPT_DTQ_STARTr, reg_above_64_val, dtq_start_addr_contexts[0], dtq_start_addr);
    for (i = 1; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        dtq_start_addr += dtq_max_size[i - 1];
        soc_reg_above_64_field32_set(unit, IPT_DTQ_STARTr, reg_above_64_val, dtq_start_addr_contexts[i], dtq_start_addr);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_STARTr(unit, reg_above_64_val)); 

    /*threshold configuration*/
    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_THr(unit, reg_above_64_val)); 
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DTQ_THr, reg_above_64_val, dtq_th_contexts[i], dtq_max_size[i]/2);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_THr(unit, reg_above_64_val)); 

    SOCDNX_IF_ERR_EXIT(READ_IPT_DTQ_DQCF_THr(unit, reg_above_64_val)); 
    for (i = 0; i < SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE * 2; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DTQ_DQCF_THr, reg_above_64_val, dtq_dqcf_th_contexts[i], dtq_max_size[i]/2);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DTQ_DQCF_THr(unit, reg_above_64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * DQCF Configuration - 
 * 20 contexts per IPT core: 
 * - Mesh: (Local0, Local1, Dest0, Dest1, Dest2/MC) x OCB/OCB-DRAM-MIX x (HP, LP) 
 * - Fabric: (Local0, Local1, UC, MC) x  OCB/OCB-DRAM-MIX x (HP, LP) 
 * Buffer size is 8k per core. in single fap mode buffer is 16k. to be devided between all valid contexts. 
 */
STATIC int 
soc_jer_ipt_dqcf_contexts_init(int unit, int is_dram, int is_dual_fap)
{
    soc_reg_above_64_val_t reg_above_64_val_0, reg_above_64_val_1;
    uint64 reg64_val;
    uint32 dqcf_0_max_size[20] = {0}, dqcf_1_max_size[20] = {0};
    uint32 ocb_only_size, dram_ocb_size; 
    uint32 ctx_max_size_used, ctx_max_size_unused = 0x3;
    uint32 dqcf_0_start_addr, dqcf_1_start_addr;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    /*size configuration*/
    if (SOC_DPP_IS_MESH(unit)) {
        ctx_max_size_used = (is_dual_fap) ? 0x330 : 0x663 ;                     
    } else {
        ctx_max_size_used = (is_dual_fap) ? 0x3fd : 0x7fd ;
    }
    ocb_only_size = (is_dram) ? ctx_max_size_unused : ctx_max_size_used;
    dram_ocb_size = (is_dram) ? ctx_max_size_used   : ctx_max_size_unused;
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; i += 4) {
        dqcf_0_max_size[i] = dqcf_0_max_size[1 + i] = ocb_only_size;
        dqcf_0_max_size[2 + i] = dqcf_0_max_size[3 + i] = dram_ocb_size;
        if (is_dual_fap) {
            dqcf_1_max_size[i] = dqcf_1_max_size[1 + i] = ocb_only_size;
            dqcf_1_max_size[2 + i] = dqcf_1_max_size[3 + i] = dram_ocb_size;
        }
    }
    if (!SOC_DPP_IS_MESH(unit)) {
        uint32 mc_ocb_temp_arr[2];
        dqcf_0_max_size[8] = dqcf_0_max_size[9] = dqcf_0_max_size[10] = dqcf_0_max_size[11] = 0; /*contexts 8-11 are not used in CLOS*/
        if (is_dual_fap) {
            dqcf_1_max_size[8] = dqcf_1_max_size[9] = dqcf_1_max_size[10] = dqcf_1_max_size[11] = 0;
        }
        mc_ocb_temp_arr[0] = dqcf_0_max_size[14]; /*in fabric mode the contexts are ordered differently, need to correct*/
        mc_ocb_temp_arr[1] = dqcf_0_max_size[15];
        dqcf_0_max_size[14] = dqcf_0_max_size[16];
        dqcf_0_max_size[15] = dqcf_0_max_size[17];
        dqcf_0_max_size[16] = mc_ocb_temp_arr[0];
        dqcf_0_max_size[17] = mc_ocb_temp_arr[1];
        if (is_dual_fap) {
            mc_ocb_temp_arr[0] = dqcf_1_max_size[14];
            mc_ocb_temp_arr[1] = dqcf_1_max_size[15];
            dqcf_1_max_size[14] = dqcf_1_max_size[16];
            dqcf_1_max_size[15] = dqcf_1_max_size[17];
            dqcf_1_max_size[16] = mc_ocb_temp_arr[0];
            dqcf_1_max_size[17] = mc_ocb_temp_arr[1];
        }
    }
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_MAX_SIZEr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_MAX_SIZEr(unit, reg_above_64_val_1)); 
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DQCF_0_MAX_SIZEr, reg_above_64_val_0, dqcf_0_max_size_contexts[i], dqcf_0_max_size[i]);
        soc_reg_above_64_field32_set(unit, IPT_DQCF_1_MAX_SIZEr, reg_above_64_val_1, dqcf_1_max_size_contexts[i], dqcf_1_max_size[i]);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_MAX_SIZEr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_MAX_SIZEr(unit, reg_above_64_val_1)); 

    /*queue start address*/
    dqcf_0_start_addr = 0;
    dqcf_1_start_addr = 0;
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_STARTr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_STARTr(unit, reg_above_64_val_1));
    soc_reg_above_64_field32_set(unit, IPT_DQCF_0_STARTr, reg_above_64_val_0, dqcf_0_start_addr_contexts[0], 0);
    soc_reg_above_64_field32_set(unit, IPT_DQCF_1_STARTr, reg_above_64_val_1, dqcf_1_start_addr_contexts[0], 0);
    for (i = 1; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        dqcf_0_start_addr += dqcf_0_max_size[i - 1];
        dqcf_1_start_addr += dqcf_1_max_size[i - 1];
        soc_reg_above_64_field32_set(unit, IPT_DQCF_0_STARTr, reg_above_64_val_0, dqcf_0_start_addr_contexts[i], dqcf_0_start_addr);
        soc_reg_above_64_field32_set(unit, IPT_DQCF_1_STARTr, reg_above_64_val_1, dqcf_1_start_addr_contexts[i], dqcf_1_start_addr);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_STARTr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_STARTr(unit, reg_above_64_val_1));

    /*dqcq threshold configuration*/
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_DQCQ_THr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_DQCQ_THr(unit, reg_above_64_val_1)); 
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
        soc_reg_above_64_field32_set(unit, IPT_DQCF_0_DQCQ_THr, reg_above_64_val_0, dqcf_0_th_contexts[i], dqcf_0_max_size[i]/2);
        soc_reg_above_64_field32_set(unit, IPT_DQCF_1_DQCQ_THr, reg_above_64_val_1, dqcf_1_th_contexts[i], dqcf_1_max_size[i]/2);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_DQCQ_THr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_DQCQ_THr(unit, reg_above_64_val_1));

    /*eir threshold configuration*/
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_EIR_CRDT_THr(unit, reg_above_64_val_0)); 
    SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_EIR_CRDT_THr(unit, reg_above_64_val_1)); 
    for (i = 0; i < SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE; ++i) {
         soc_reg_above_64_field32_set(unit, IPT_DQCF_0_EIR_CRDT_THr, reg_above_64_val_0, dqcf_0_eir_th_contexts[i], 2*dqcf_0_max_size[i]/3);
         soc_reg_above_64_field32_set(unit, IPT_DQCF_1_EIR_CRDT_THr, reg_above_64_val_1, dqcf_1_eir_th_contexts[i], 2*dqcf_1_max_size[i]/3);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_DQCQ_THr(unit, reg_above_64_val_0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_DQCQ_THr(unit, reg_above_64_val_1));

    /*multicast threshold configuration -fabric mode only*/
    if (!SOC_DPP_IS_MESH(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_0_MC_THr(unit, &reg64_val)); 
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_GFMC_OCB_THf, is_dram ? 0 : 2*dqcf_0_max_size[14]/3);
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_BFMC_OCB_THf, is_dram ? 0 : 2*dqcf_0_max_size[15]/3);
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_GFMC_MIX_THf, is_dram ? 2*dqcf_0_max_size[18]/3 : 0);
        soc_reg64_field32_set(unit, IPT_DQCF_0_MC_THr, &reg64_val, DQCF_0_MC_BFMC_MIX_THf, is_dram ? 2*dqcf_0_max_size[19]/3 : 0);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_0_MC_THr(unit, reg64_val));

        SOCDNX_IF_ERR_EXIT(READ_IPT_DQCF_1_MC_THr(unit, &reg64_val)); 
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_GFMC_OCB_THf, is_dram ? 0 : 2*dqcf_1_max_size[14]/3);
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_BFMC_OCB_THf, is_dram ? 0 : 2*dqcf_1_max_size[15]/3);
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_GFMC_MIX_THf, is_dram ? 2*dqcf_1_max_size[18]/3 : 0);
        soc_reg64_field32_set(unit, IPT_DQCF_1_MC_THr, &reg64_val, DQCF_1_MC_BFMC_MIX_THf, is_dram ? 2*dqcf_1_max_size[19]/3 : 0);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_DQCF_1_MC_THr(unit, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 *  IPT block default values
 */
uint32
soc_jer_ipt_contexts_init(
  SOC_SAND_IN int     unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 nof_pipes, is_dual_fap, is_dram, reg_val, port_count, is_pcp;
    SOCDNX_INIT_FUNC_DEFS;

    /*Get relevant properites */
    nof_pipes = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.nof_pipes;
    SOC_PBMP_COUNT(SOC_DPP_CONFIG(unit)->arad->init.drc_info.dram_bitmap, port_count);
    is_dram = (port_count) ? 1 : 0;
    is_dual_fap = (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) ? 1 : 0 ;
    is_pcp = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable;

    /*Enable IPT*/
    SOCDNX_IF_ERR_EXIT(READ_IPT_IPT_ENABLESr(unit, &reg_val));
    soc_reg_field_set(unit, IPT_IPT_ENABLESr, &reg_val, IPT_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_IPT_ENABLESr(unit, reg_val));

    /*Disable dropping packets due to large latencys if PCP is enabled*/
    if (is_pcp) {
        SOCDNX_IF_ERR_EXIT(READ_IPT_PACKET_LATENCY_MEASURE_CFGr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, reg_above_64_val, EN_LATENCY_DROPf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_PACKET_LATENCY_MEASURE_CFGr(unit, reg_above_64_val));
    }

    /*configure DTQ contexts*/
    SOCDNX_IF_ERR_EXIT(soc_jer_ipt_dtq_contexts_init(unit, is_dram, is_dual_fap, nof_pipes));

    /*configure DQCF contexts*/
    SOCDNX_IF_ERR_EXIT(soc_jer_ipt_dqcf_contexts_init(unit, is_dram, is_dual_fap));

exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_init_sequence_phase1
 * Purpose:
 *     Initialize the device, including:
 *     - Prepare internal data
 *     - Initialize basic configuration
 *     - Out-of-reset Device internal blocks
 *     - Set Core clock frequency
 *     - Stop all traffic and Control Cells
 *     - Initialize basic configuration
 *     - Set board-related configuration (hardware adjustments)
 * Parameters:
 *      unit -  unit number
 * Returns:
 *      SOC_E_XXX
 *
 */
int soc_jer_init_sequence_phase1(int unit)
{
    ARAD_MGMT_INIT* init;
    SOCDNX_INIT_FUNC_DEFS;

    init = &(SOC_DPP_CONFIG(unit)->arad->init);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
      chip_sim_em_init(unit, SOC_SAND_DEV_ARAD);
    }
#endif

    /* Prepare internal data */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_prepare_internal_data(unit));

    /* Out-of-reset Device internal blocks */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Blocks Initial configuration\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_init(unit));

    /* Initialize all tables */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Configure tables defaults\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_tbls_init(unit));
    /* soft init again just in case we touch dinamic tables */
    SOCDNX_IF_ERR_EXIT(soc_jer_reset_soft_init(unit));

    /* Set Core clock frequency */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Set core clock frequency\n"),unit));
    SOCDNX_IF_ERR_EXIT(arad_mgmt_init_set_core_clock_frequency(unit, init));

    /* Stop all traffic and Control Cells */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Stop All traffic\n"),unit));
    SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_all_ctrl_cells_enable_set(unit, FALSE));

    /* Initialize basic configuration (based on pre-OOR) */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Blocks configuration\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_conf(unit));

    /* Set board-related configuration (hardware adjustments) */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Set hardware adjustments\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_hw_interfaces_set(unit));

    /*
     * Before the port configuration: OTMH extensions configuration
     * must know the egress editor program attributes
     */
    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_config_dut_by_queue_database(unit));

    /* Set port init */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Set port configurations \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_port(unit));

    /* Set module init */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Set default module configurations \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_functional_init(unit));

    /* init egr tm (port mapping) */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: Set egress port mapping \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_tm_init(unit));

    /* init ofp rates */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: init ofp rates \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_ofp_rates_init(unit));

    /* init sch */
    LOG_INFO(BSL_LS_SOC_INIT, (BSL_META_U(unit, "    + %d: init sch \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_sch_init(unit));
    
exit:
    SOCDNX_FUNC_RETURN;
}

