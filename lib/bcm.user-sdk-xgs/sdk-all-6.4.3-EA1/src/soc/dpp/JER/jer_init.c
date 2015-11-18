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
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_trunk.h>
#include <soc/dpp/JER/JER_PP/jer_pp_ing_protection.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_protection.h>
#include <soc/dpp/JER/jer_regs.h>
#include <soc/dpp/JER/jer_tbls.h>

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
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#endif

/* 
 * Access init functions
 */

/* 
 * Initialize the broadcast IDs of the blocks
 */
int soc_jer_init_brdc_blk_id_set(int unit)
{
    int index;

    SOCDNX_INIT_FUNC_DEFS;

    /* fix soc_dpp_defines_t in case of QMX */ 
    if (SOC_IS_QMX(unit)) {
        SOC_DPP_DEFS_SET(unit, nof_fabric_links, 16); 
        SOC_DPP_DEFS_SET(unit, nof_fabric_macs, 4);
        SOC_DPP_DEFS_SET(unit, nof_instances_fmac, 4);
    }

    /* Init broadcast in FMAC */
    for (index=0 ; index < SOC_DPP_DEFS_GET(unit, nof_instances_fmac) - 1 ; index++) {
      SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, index, FMAC_BRDC_ID));
      SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_LAST_IN_CHAINr(unit, index, 0));
    }
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, index, FMAC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_LAST_IN_CHAINr(unit, index, 1));

    /* Init broadcast in FSRD */
    for (index=0 ; index < SOC_DPP_DEFS_GET(unit, nof_instances_fsrd) - 1 ; index++) {
      SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, index, FSRD_BRDC_ID));
      SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_LAST_IN_CHAINr(unit, index, 0));
    }
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, index, FSRD_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_LAST_IN_CHAINr(unit, index, 1));

    /* Init broadcast in CGM */
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SBUS_BROADCAST_IDr(unit, 0, CGM_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SBUS_BROADCAST_IDr(unit, 1, CGM_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SBUS_LAST_IN_CHAINr(unit, 0, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SBUS_LAST_IN_CHAINr(unit, 1, 1));

    /* Init broadcast in EGQ */
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_SBUS_BROADCAST_IDr(unit, 0, EGQ_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_SBUS_BROADCAST_IDr(unit, 1, EGQ_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_SBUS_LAST_IN_CHAINr(unit, 0, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_SBUS_LAST_IN_CHAINr(unit, 1, 1));

    /* Init broadcast in EPNI */
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_SBUS_BROADCAST_IDr(unit, 0, EPNI_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_SBUS_BROADCAST_IDr(unit, 1, EPNI_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_SBUS_LAST_IN_CHAINr(unit, 0, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_SBUS_LAST_IN_CHAINr(unit, 1, 1));

    /* Init broadcast in IHB */
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_SBUS_BROADCAST_IDr(unit, 0, IHB_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_SBUS_BROADCAST_IDr(unit, 1, IHB_BRDC_ID));
    /* last in chain reversed in IHB */
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_SBUS_LAST_IN_CHAINr(unit, 0, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_SBUS_LAST_IN_CHAINr(unit, 1, 0));

    /* Init broadcast in IHP */
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_SBUS_BROADCAST_IDr(unit, 0, IHP_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_SBUS_BROADCAST_IDr(unit, 1, IHP_BRDC_ID));
    /* last in chain reversed in IHP */
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_SBUS_LAST_IN_CHAINr(unit, 0, 1));
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_SBUS_LAST_IN_CHAINr(unit, 1, 0));

    /* Init broadcast in IPS */
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_SBUS_BROADCAST_IDr(unit, 0, IPS_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_SBUS_BROADCAST_IDr(unit, 1, IPS_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_SBUS_LAST_IN_CHAINr(unit, 0, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_SBUS_LAST_IN_CHAINr(unit, 1, 1));

    /* Init broadcast in IQM */
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_SBUS_BROADCAST_IDr(unit, 0, IQM_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_SBUS_BROADCAST_IDr(unit, 1, IQM_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_SBUS_LAST_IN_CHAINr(unit, 0, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_SBUS_LAST_IN_CHAINr(unit, 1, 1));

    /* Init broadcast in SCH */
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SBUS_BROADCAST_IDr(unit, 0, SCH_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SBUS_BROADCAST_IDr(unit, 1, SCH_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SBUS_LAST_IN_CHAINr(unit, 0, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SBUS_LAST_IN_CHAINr(unit, 1, 1));

    /* Init broadcast in DRC */
    SOCDNX_IF_ERR_EXIT(WRITE_DRCA_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCB_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCC_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCD_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCE_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCF_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCG_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCH_SBUS_BROADCAST_IDr(unit, DRC_BRDC_ID));

	/* Last in chain - DRC */
    SOCDNX_IF_ERR_EXIT(WRITE_DRCA_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCB_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCC_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCD_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCE_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCF_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCG_REG_0087r(unit, 0));
    SOCDNX_IF_ERR_EXIT(WRITE_DRCH_REG_0087r(unit, 1));

exit:
    SOCDNX_FUNC_RETURN;
}

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





int soc_jer_init_before_blocks_oor(int unit)
{
  
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN;
}




/* excludes irrelevant blocks which are missing due to partial emul compilation */
STATIC int soc_jer_init_exclude_blocks(int unit)
{
    int block = -1;
    int32 instance = 0;
    int block_iter = 0;

    typedef struct
    {
        soc_block_type_t type;
        int32 instance;
    } jer_init_block_type_instance_pair;

    /* list of excluded blocks - type and instance, should end with the Last Block In List */
    jer_init_block_type_instance_pair excluded_blocks[] = {
        {SOC_BLK_MMU, 0},
        {SOC_BLK_DRCA, 0},
        {SOC_BLK_DRCB, 0},
        {SOC_BLK_DRCC, 0},
        {SOC_BLK_DRCD, 0},
        {SOC_BLK_DRCE, 0},
        {SOC_BLK_DRCF, 0},
        {SOC_BLK_DRCG, 0},
        {SOC_BLK_DRCH, 0},
        {SOC_BLK_DRCBROADCAST, 0},
        {SOC_BLK_NBIL, 0},
        {SOC_BLK_NBIL, 1},
        {SOC_BLK_CLP, 3},
        {SOC_BLK_CLP, 4},
        {SOC_BLK_CLP, 5},

        /* Last Block In List */
        {SOC_BLK_NONE, 0},
    };

    SOCDNX_INIT_FUNC_DEFS;

    /* parse over block list */
    while (excluded_blocks[block_iter].type != SOC_BLK_NONE) {
        instance = excluded_blocks[block_iter].instance;
        /* get block number using specific macro for each block type */
        switch (excluded_blocks[block_iter].type) {
            case SOC_BLK_CLP:
                block = CLP_BLOCK(unit, instance);
                break;
            case SOC_BLK_NBIL:
                block = NBIL_BLOCK(unit, instance);
                break;
            case SOC_BLK_MMU:
                block = MMU_BLOCK(unit);
                break;
            case SOC_BLK_DRCA:
                block = DRCA_BLOCK(unit);
                break;
            case SOC_BLK_DRCB:
                block = DRCB_BLOCK(unit);
                break;
            case SOC_BLK_DRCC:
                block = DRCC_BLOCK(unit);
                break;
            case SOC_BLK_DRCD:
                block = DRCD_BLOCK(unit);
                break;
            case SOC_BLK_DRCE:
                block = DRCE_BLOCK(unit);
                break;
            case SOC_BLK_DRCF:
                block = DRCF_BLOCK(unit);
                break;
            case SOC_BLK_DRCG:
                block = DRCG_BLOCK(unit);
                break;
            case SOC_BLK_DRCH:
                block = DRCH_BLOCK(unit);
                break;
            case SOC_BLK_DRCBROADCAST:
                block = DRCBROADCAST_BLOCK(unit);
                break;
            default:
                block = -1;
                break;
        }

        /* if successful exclude the block, if failed exit with error */
        if (block == -1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_jer_init_exclude_blocks: invalid block type or block instance")));
        } else {
            SOC_INFO(unit).block_valid[block] = FALSE;
        }
                
        ++block_iter; 
    }
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_init_blocks_init(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    /* exclude irrelevant blocks which are missing due to partial emul compilation - controlled via custom soc property */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "exclude_blocks_missing_in_partial_emul_compilation", 0)) {
        SOCDNX_IF_ERR_EXIT(soc_jer_init_exclude_blocks(unit)); 
    }

    /* access check to blocks after reset*/
    SOCDNX_IF_ERR_EXIT(soc_jer_regs_blocks_access_check(unit));

    /* Init blocks' broadcast IDs */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_brdc_blk_id_set(unit));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_after_blocks_oor(int unit)
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
    soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_1r, &reg32, MESH_MODEf, 0);
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

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_init_hw_interfaces_set(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_dram_init(unit, &SOC_DPP_CONFIG(unit)->arad->init.drc_info));

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


/* Modules initialization */
STATIC int soc_jer_init_functional_init(int unit)
{
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 soc_sand_rv = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_init_mesh_topology(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_jer_fabric_init(unit);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

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

        reg32 = 0;
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_TCAM_BUBBLESf, 1);
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, ENABLE_MACT_BUBBLESf, 1);
        soc_reg_field_set(unit, IHP_IHP_ENABLERSr, &reg32, NON_FIRST_FRAGMENTS_ENABLEf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_IHP_ENABLERSr(unit, SOC_CORE_ALL, reg32));

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
        soc_reg_field_set(unit, IQM_IQM_INITr, &reg32, IQC_INITf, 1);
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
        soc_mem_field32_set(unit, EGQ_PER_PORT_DESCRIPTORS_THRESHOLDS_MEMORYm, m, PORT_UC_PD_DIS_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_DESCRIPTORS_THRESHOLDS_MEMORYm, m, PORT_MC_PD_SHARED_MAX_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_DESCRIPTORS_THRESHOLDS_MEMORYm, m, PORT_MC_PD_SHARED_MIN_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_DESCRIPTORS_THRESHOLDS_MEMORYm, m, PORT_UC_PD_MAX_FC_THf, 0x7fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_DESCRIPTORS_THRESHOLDS_MEMORYm, m, PORT_UC_PD_MIN_FC_THf, 0x7fff); 
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PER_PORT_DESCRIPTORS_THRESHOLDS_MEMORYm, MEM_BLOCK_ALL, m));

        m[2] = m[1] = m[0] = 0;
        soc_mem_field32_set(unit, EGQ_PER_PORT_BUFFERS_THRESHOLDS_MEMORYm, m, PORT_UC_DB_DIS_THf, 0x3fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_BUFFERS_THRESHOLDS_MEMORYm, m, PORT_MC_DB_SHARED_THf, 0x3fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_BUFFERS_THRESHOLDS_MEMORYm, m, PORT_UC_DB_MAX_FC_THf, 0x3fff); 
        soc_mem_field32_set(unit, EGQ_PER_PORT_BUFFERS_THRESHOLDS_MEMORYm, m, PORT_UC_DB_MIN_FC_THf, 0x3fff); 
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PER_PORT_BUFFERS_THRESHOLDS_MEMORYm, MEM_BLOCK_ALL, m));

        m[3] = m[2] = m[1] = m[0] = 0;
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_UC_PD_MAX_FC_THf, 100);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_UC_PD_MIN_FC_THf, 100);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MAX_TH_DP_3f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MAX_TH_DP_2f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MAX_TH_DP_1f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MAX_TH_DP_0f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MIN_TH_DP_2f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MIN_TH_DP_1f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_MIN_TH_DP_0f, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_MC_PD_RSVD_THf, 0xfff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, m, QUEUE_UC_PD_DIS_THf, 0xfff);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PER_QUEUE_DESCRIPTORS_THRESHOLDS_MEMORYm, MEM_BLOCK_ALL, m));

        m[6] = m[5] = m[4] = m[3] = m[2] = m[1] = m[0] = 0;
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_UC_DB_MAX_FC_THf, 160);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_UC_DB_MIN_FC_THf, 160);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_MC_DB_DP_3_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_MC_DB_DP_2_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_MC_DB_DP_1_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_MC_DB_DP_0_THf, 0x3ffff);
        soc_mem_field32_set(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, m, QUEUE_UC_DB_DIS_THf, 0x3fff);
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_PER_QUEUE_BUFFERS_THRESHOLDS_MEMORYm, MEM_BLOCK_ALL, m));


        m[0] = 0; /* EGRESS_TCf=0 */
        SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EGQ_TC_DP_MAPm, MEM_BLOCK_ALL, m));

        /*  NBIH_EGQ_CORE_FOR_NIF_QMLF set bit for core number (quartet) */
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_TX_PORTS_SRSTNr(unit, 0xfffff));
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_RX_PORTS_SRSTNr(unit, 0xfffff));

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, NBIH_TX_EGRESS_CREDITS_DEBUG_PMr, REG_PORT_ANY, 0, TX_FLUSH_EGRESS_PORT_0_MLF_0_QMLF_Nf, 1));


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


/*
 * Function:
 *      soc_jer_init_sequence_phase1
 * Purpose:
 *     Initialize the device, including:
 *     - Prepare internal data
 *     - Initialize basic configuration (must be before per-block Out-Of-Reset)
 *     - Out-of-reset Device internal blocks
 *     - Set Core clock frequency
 *     - Stop all traffic and Control Cells
 *     - Initialize basic configuration (based on pre-OOR)
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
      chip_sim_em_init(SOC_SAND_DEV_ARAD);
    }
#endif

    /* Prepare internal data */
    SOCDNX_IF_ERR_EXIT(soc_jer_init_prepare_internal_data(unit));

    /* Initialize basic configuration (must be before per-block Out-Of-Reset) */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Before Blocks OOR configuration\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_before_blocks_oor(unit));

    /* Out-of-reset Device internal blocks */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: OOR Device internal blocks\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_blocks_init(unit));

    /* Initialize all tables */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Configure tables defaults\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_tbls_init(unit));


    /* Set Core clock frequency */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Set core clock frequency\n"),unit));
    SOCDNX_IF_ERR_EXIT(arad_mgmt_init_set_core_clock_frequency(unit, init));

    /* Stop all traffic and Control Cells */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Stop All traffic\n"),unit));

    /* Initialize basic configuration (based on pre-OOR) */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: After Blocks OOR configuration\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_after_blocks_oor(unit));

    /* Set board-related configuration (hardware adjustments) */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Set hardware adjustments\n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_hw_interfaces_set(unit));

    /*
     * Before the port configuration: OTMH extensions configuration
     * must know the egress editor program attributes
     */
    SOCDNX_IF_ERR_EXIT(arad_egr_prog_editor_config_dut_by_queue_database(unit));

    /* Set port init */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Set port configurations \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_port(unit));

    /* Set module init */
    LOG_INFO(BSL_LS_SOC_INIT,
             (BSL_META_U(unit,
                         "    + %d: Set default module configurations \n"),unit));
    SOCDNX_IF_ERR_EXIT(soc_jer_init_functional_init(unit));
    
exit:
    SOCDNX_FUNC_RETURN;
}

