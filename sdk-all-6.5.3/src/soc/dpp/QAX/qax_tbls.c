/*
 * $Id: qax_tbls.c Exp $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
#include <shared/bitop.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>


/* SOC DPP JER includes */
#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/QAX/qax_tbls.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/QAX/qax_fabric.h>

/* SOC DPP Arad includes */ 
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>


/* 
 * Defines
 */


/* Static memoried which should be skipped when zeroing all static memories */
static soc_mem_t qax_tbls_excluded_mem_list[] = {
    TAR_MCDBm,
    ILKN_PMH_PORT_0_CPU_ACCESSm, /* These are not real memories */
    ILKN_PMH_PORT_1_CPU_ACCESSm,
    ILKN_PML_PORT_0_CPU_ACCESSm,
    ILKN_PML_PORT_1_CPU_ACCESSm,
    SCH_SCHEDULER_INITm,
    OAMP_FLEX_VER_MASK_TEMPm, 
    OAMP_FLOW_STAT_ACCUM_ENTRY_4m,
    SER_ACC_TYPE_MAPm,
    SER_MEMORYm,
    SER_RESULT_DATA_1m,
    SER_RESULT_DATA_0m,
    SER_RESULT_EXPECTED_1m,
    SER_RESULT_EXPECTED_0m,
    SER_RESULT_1m,
    SER_RESULT_0m,

    /* Has to be last memory in array */
    INVALIDm
};

/*
 * Functions
 */


/* Clear a maximum sized memory entry */
static void clear_full_mem_entry(uint32 *entry) {
    sal_memset(entry, 0, SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY*4);
}

/*
 * Fill the whole table with the given entry, uses fast DMA filling when run on real hardware.
 * Marks the table not to later be filled by soc_qax_tbls_init.
 */

static int qax_fill_and_mark_memory(
    const int       unit,
    const soc_mem_t mem,        /* memory/table to fill */
    const void      *entry_data /* The contents of the entry to fill the table with. Does not have to be DMA memory */
  )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, mem, MEM_BLOCK_ALL, entry_data));
    /* mark the memory not to be later zeroed */
    SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, mem);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_qax_tbls_init
 * Purpose:
 *      initialize all tables relevant for Jericho.
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
int soc_qax_tbls_init(int unit)
{
    /* define bitmap for memory exclusions */
    SHR_BITDCL *qax_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;
    int mem_iter = 0;
    soc_mem_t* excluded_list = qax_tbls_excluded_mem_list; /* excluded memory list iterator */
    uint32 entry[SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    /* Set exclusion bitmap to zero. Bits of manually filled memories will be later set to 1. */
    sal_memset(qax_excluded_mems_bmap_p, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));
        
    SOCDNX_IF_ERR_EXIT(arad_tbl_access_init_unsafe(unit)); /* init arad_fill_table_with_variable_values_by_caching() */

    /* Init tables having none zero values */

    /* SCH block */
    SOCDNX_IF_ERR_EXIT(soc_qax_sch_tbls_init(unit));
    /* TAR block */
    SOCDNX_IF_ERR_EXIT(soc_qax_tar_tbls_init(unit));

    /* IRE block */

    /* IHP block */
    SOCDNX_IF_ERR_EXIT(soc_qax_ihp_tbls_init(unit));

    /* CGM block */
    SOCDNX_IF_ERR_EXIT(soc_qax_cgm_tbls_init(unit));

    /* IPS block */
    SOCDNX_IF_ERR_EXIT(soc_qax_ips_tbls_init(unit));

    /* TXQ block */
    SOCDNX_IF_ERR_EXIT(soc_qax_txq_tbls_init(unit));
	
    /* ITE block */

    /* FDT block */

    /* EGQ block */
    SOCDNX_IF_ERR_EXIT(soc_qax_egq_tbls_init(unit));

    /* EPNI block */
    SOCDNX_IF_ERR_EXIT(soc_qax_epni_tbls_init(unit));

    /* OAMP block */

    /* MRPS+MRPS_EM block (IMP+IEP) */
    SOCDNX_IF_ERR_EXIT(soc_qax_imp_tbls_init(unit));
	SOCDNX_IF_ERR_EXIT(soc_qax_iep_tbls_init(unit));

    /* PPDB block */
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , 1)); /* Init dynamic table */
    SOCDNX_SAND_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, PPDB_B_LARGE_EM_KEYT_PLDT_Hm, entry));
    SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , 0));

    SOCDNX_IF_ERR_EXIT(qax_mult_rplct_tbl_entry_unoccupied_set_all(unit));
    SOCDNX_IF_ERR_EXIT(qax_mcds_multicast_init2(unit));

    /* Zero tables if not running in emulation/simulation */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "static_tbl_full_init", !(
#ifdef PLISIM
        SAL_BOOT_PLISIM || /*not pcid and not emulation*/
#endif
        SOC_DPP_CONFIG(unit)->emulation_system))) {

        clear_full_mem_entry(entry);
        /* Add excluded memories to the exclusion bitmap */
        for (; *excluded_list != INVALIDm; ++excluded_list)  { /* iterate on the excluded memories */
            SHR_BITSET(qax_excluded_mems_bmap_p, *excluded_list); /* set the bits of excluded memories in the bitmap */
        }

        /* Initialize all the QAX memorires that need it to zero. Memories should be static, not read only, not aliases, not filled earlier... */
        for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++) { /* iterate over all memories */
            if (SOC_MEM_IS_VALID(unit, mem_iter) && /* Memory must be valid for the device */
                /* memory must be static (not dynamic) and not read-only */
                (soc_mem_flags(unit, mem_iter) & (SOC_MEM_FLAG_SIGNAL | SOC_MEM_FLAG_READONLY)) == 0 &&
    
                /* memory must not be an alias, to avoid multiple resets of the same memory */
                (mem_iter == SOC_MEM_ALIAS_MAP(unit, mem_iter) || !SOC_MEM_IS_VALID(unit, SOC_MEM_ALIAS_MAP(unit, mem_iter))) &&
    
                /* if entry is bigger than max entry size and this is a wide mem that should be ignored */
                soc_mem_entry_words(unit, mem_iter) <= SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY &&
                !SHR_BITGET(qax_excluded_mems_bmap_p, mem_iter)) { /* if the mem is not in excluded bitmap */
    
                /* reset memory - set all values to 0 */
                LOG_VERBOSE(BSL_LS_SOC_INIT,(BSL_META_U(unit,"Reseting static memory # %d - %s\n"),mem_iter, SOC_MEM_NAME(unit, mem_iter)));
    
                SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, mem_iter, MEM_BLOCK_ALL, entry));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_ihp_tbls_init(int unit)
{    
    uint32 entry[20] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    /* default FID = VSI */
    soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, entry, FID_CLASSf, 7);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, IHP_VSI_LOW_CFG_2m, entry));    
	
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_ips_tbls_init(int unit)
{
    uint32 entry[20] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    /*
     * Mark whole table as 'invalid'. See arad_interrupt_handles_corrective_action_ips_qdesc().
     */
    entry[1] = ARAD_IPQ_INVALID_FLOW_QUARTET;
    SOCDNX_SAND_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, IPS_FLWIDm, entry));
    entry[1] = 0;

    /* set to max (127) CAM full thresholds */
    soc_mem_field32_set(unit, IPS_CFMEMm, entry, DRAM_CAM_FULL_THRESHOLDf, 127);
    soc_mem_field32_set(unit, IPS_CFMEMm, entry, FABRIC_CAM_FULL_THRESHOLDf, 127);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, IPS_CFMEMm, entry));
    sal_memset(entry, 0, sizeof(entry));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_cgm_tbls_init(int unit)
{
    uint32 entry[20] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    /* set SRAM only mode */
    soc_mem_field32_set(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, entry, SRAM_DRAM_ONLY_MODEf, SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, entry));
    sal_memset(entry, 0, sizeof(entry));

	/* Init the Eth mtr table*/
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, CGM_ETH_MTR_PTR_MAPm, MEM_BLOCK_ANY, entry));

    /* disable CNI by default */
    soc_mem_field32_set(unit, CGM_CNI_PRMSm, entry, MAX_SIZE_THf, 0xfff);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, CGM_CNI_PRMSm, entry));
    sal_memset(entry, 0, sizeof(entry));

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * Tables Init Functions
 */ 
int soc_qax_sch_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};
    int core;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    /* all relevant bits initialized to SOC_TMC_MAX_FAP_ID */
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, SOC_TMC_MAX_FAP_ID);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry));
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, 0);

    /* trigger SCH_SCHEDULER_INIT to reset dynamic SCH tables */
    soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, table_entry, SCH_INITf, 0x1);
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, SCH_BLOCK(unit, core), 0x0, table_entry));
    }
    soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, table_entry, SCH_INITf, 0x0);

    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 128);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, SCH_PS_8P_RATES_PSRm, table_entry));
    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 0);

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_egq_tbls_init(int unit)
{
    soc_reg_above_64_val_t data;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EGQ_PCTm, data));

    /* Set the VSI-Membership table to all 1's */
    SOC_REG_ABOVE_64_ALLONES(data);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EGQ_VSI_MEMBERSHIPm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EGQ_DSP_PTR_MAPm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EGQ_PER_IFC_CFGm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PP_PPCTm, data, MTUf, 0x3fff);    
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EGQ_PP_PPCTm, data));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_epni_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};
    uint32 tx_tag_table_entry[8] = {0};
    uint32 res = 0;
    int i;
    uint32 value;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 1)); /* Init dynamic table */
    SOCDNX_SAND_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EPNI_IFC_2_NIF_PORT_MAPm, table_entry));
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 0)); /* Init dynamic table */

    SOCDNX_SAND_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, IHP_RECYCLE_COMMANDm, table_entry));

    /* init the first half of IHP_RECYCLE_COMMANDm to a static 1-1 mapping from recycle commands to inbound mirror action profiles */
    for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
        value = 0; /* other recycling command actions are disabled */
        soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, MIRROR_PROFILEf, i);
        if (i > 0) { /* for all valid outbound mirror profile , set highest strength for forward action packet to drop */      
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, FORWARD_STRENGTHf, 7); /* highest strength */
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, CPU_TRAP_CODEf, SOC_PPC_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0); /* trap code SOC_PPC_TRAP_CODE_USER_DEFINED_27 dest=queue 128K-1 */  
            SOCDNX_IF_ERR_EXIT(WRITE_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ALL, i, &value));
        }
    }

    res = soc_sand_bitstream_fill(tx_tag_table_entry, 8);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = qax_fill_and_mark_memory(unit, EPNI_TX_TAG_TABLEm, &tx_tag_table_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    sal_memset(table_entry, 0, sizeof(table_entry));
    soc_mem_field32_set(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry, MTUf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry));

 
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_imp_tbls_init(int unit)
{
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDA_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDB_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDA_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDB_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDA_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDB_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDA_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDB_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IMP_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_iep_tbls_init(int unit)
{
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDA_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDB_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDA_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDB_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDA_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDB_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDA_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDB_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IEP_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_tar_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    /* destination table */
    table_entry[0] = 0; /* we want the entry to be disabled (all 1s queue) and have a traffic class profile of 0 */
    soc_mem_field32_set(unit, TAR_DESTINATION_TABLEm, table_entry, QUEUE_NUMBERf, ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit)); /* mark as disabled entry */
    soc_mem_field32_set(unit, TAR_DESTINATION_TABLEm, table_entry, TC_PROFILEf, 0); /* ARAD_IPQ_TC_PROFILE_DFLT is 0 */
    SOCDNX_IF_ERR_EXIT(qax_fill_and_mark_memory(unit, TAR_DESTINATION_TABLEm, table_entry)); /* fill table with the entry */
    sal_memset(table_entry, 0, sizeof(table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*     soc_qax_txq_tbls_init
* TYPE:
*   PROC
* DATE:
*   Dec 14 2015
* FUNCTION:
*     Init TXQ priority tables TXQ_PRIORITY_BITS_MAPPING_2_FDT according to tc bits
* INPUT:
*  SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   None
*********************************************************************/

soc_error_t
  soc_qax_txq_tbls_init(
      SOC_SAND_IN  int                 unit
    )
{
    int i;

    uint32 fabric_priority, tc, fabric_mc_hp_lp_boundary = -1;
    soc_dcmn_fabric_pipe_map_type_t map_type;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    map_type = SOC_DPP_FABRIC_PIPES_CONFIG(unit).mapping_type;

    /*In case of "uc / hp mc / lp mc" configuration - get the fabric priority boundary for moving from lp to hp*/
    if (map_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc){
        for(i=0;i<SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES-1 ; i++) {
           if (SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[i] != SOC_DPP_FABRIC_PIPES_CONFIG(unit).config_mc[i+1]){
               fabric_mc_hp_lp_boundary = i;
               break;
           }
       }
       if(fabric_mc_hp_lp_boundary == -1){
           SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("mode is fabric_pipe_map_triple_uc_hp_mc_lp_mc but all mc priorities are the same \n")));
       }
    }


    /*0 - 3 - fabric priorities*/
    for(i=0;i<SOC_QAX_FABRIC_PRIORITY_NDX_NOF ; i++) {
        if (map_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc)
        {
            /*fabric_priority must be fully aligned with hp_bit since dtq uses hp_bit to decide queue id while FDR uses fabric_priority to decide pipe id.
             *  Therefore, in "uc / hp mc / lp mc" configuration =>
             *     LP bit ==> priority 0 (= lowest priority of lp mc)
             *     HP bit ==> lowest priority of hp mc (= fabric_mc_hp_lp_boundary+1) */
            fabric_priority = (i & SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_MASK) >> SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_OFFSET ? (fabric_mc_hp_lp_boundary+1) : 0;
        }else{
            /*params according to i*/
            /*tc=0, 1, 2 ==> prio=0*/
            /*tc=3, 4, 5 ==> prio=1*/
            /*tc=6, 7    ==> prio=2*/
            tc = (i & SOC_QAX_FABRIC_PRIORITY_NDX_TC_MASK) >> SOC_QAX_FABRIC_PRIORITY_NDX_TC_OFFSET;
            fabric_priority = tc/3;
        }
        SOCDNX_IF_ERR_EXIT(WRITE_TXQ_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &fabric_priority));
    }

    exit:
      SOCDNX_FUNC_RETURN;
}
