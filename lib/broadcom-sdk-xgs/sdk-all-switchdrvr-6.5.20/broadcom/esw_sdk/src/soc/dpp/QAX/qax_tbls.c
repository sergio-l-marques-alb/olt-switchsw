/*
 * $Id: qax_tbls.c Exp $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
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
#include <soc/sand/sand_mem.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPC/ppc_api_lif.h>

/* SOC DPP JER includes */
#include <soc/sand/sand_mem.h>
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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>



/* 
 * Defines
 */
#define QAX_TBLS_UNUSED_DTQ_PDQ_FC_TH     (0x2)
#define QAX_TBLS_MAX_NUM_OF_DTQS          (0x6)

/*
 * Typedefs
 */

/* PDQ (DQCQ) init struct*/
typedef struct pdq_init_vals_s {
    int queue_start;
    int queue_size;
    int pdq_word_fc_threshold;
    int eir_crdt_fc_threshold;
    int pdq_fc_threshold;
} pdq_init_vals_t;

/* memories which should be skipped when zeroing all memories */
static soc_mem_t qax_tbls_excluded_mem_list[] = {
    TAR_MCDBm,
    ILKN_PMH_PORT_0_CPU_ACCESSm, /* These are not real memories */
    ILKN_PMH_PORT_1_CPU_ACCESSm,
    ILKN_PML_PORT_0_CPU_ACCESSm,
    ILKN_PML_PORT_1_CPU_ACCESSm,
    SCH_SCHEDULER_INITm, /* not a real memory, handled separately */
    OAMP_FLEX_VER_MASK_TEMPm, /* This table will be initiated by OAM special function. */
    OAMP_FLOW_STAT_ACCUM_ENTRY_4m,
    SER_ACC_TYPE_MAPm,
    SER_MEMORYm,
    SER_RESULT_DATA_1m,
    SER_RESULT_DATA_0m,
    SER_RESULT_EXPECTED_1m,
    SER_RESULT_EXPECTED_0m,
    SER_RESULT_1m,
    SER_RESULT_0m,

    MMU_DRAM_ADDRESS_SPACEm, /* not a real memory */
    DQM_BDB_LINK_LISTm, /* link list is initialized by hw */
    /* Has to be last memory in array */
    INVALIDm
};

static soc_mem_t qux_tbls_excluded_mem_list[] = {
    TAR_MCDBm,
    ILKN_PMH_PORT_0_CPU_ACCESSm, /* These are not real memories */
    ILKN_PMH_PORT_1_CPU_ACCESSm,
    ILKN_PML_PORT_0_CPU_ACCESSm,
    ILKN_PML_PORT_1_CPU_ACCESSm,
    SCH_SCHEDULER_INITm, /* not a real memory, handled separately */
    OAMP_FLOW_STAT_ACCUM_ENTRY_4m,
    SER_ACC_TYPE_MAPm,
    SER_MEMORYm,
    SER_RESULT_DATA_1m,
    SER_RESULT_DATA_0m,
    SER_RESULT_EXPECTED_1m,
    SER_RESULT_EXPECTED_0m,
    SER_RESULT_1m,
    SER_RESULT_0m,

    /* memories whose zeroing failed on QUX */
    IPSEC_SPU_WRAPPER_TOP_SPU_INPUT_FIFO_MEM_Hm,
    IPSEC_SPU_WRAPPER_TOP_SPU_OUTPUT_FIFO_MEM_Hm,

    MMU_DRAM_ADDRESS_SPACEm, /* not a real memory */
    DQM_BDB_LINK_LISTm, /* link list is initialized by hw */
    /* Has to be last memory in array */
    INVALIDm
};

/* registers to enable QAX dynamic memory writes */
static soc_reg_t qax_dynamic_mem_enable_regs[] = {
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IEP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PMH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PML_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_SPU_WRAPPER_TOP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    KAPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    NBIH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    NBIL_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PTS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TXQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/* Registers to disable QAX dynamic memory writes (for blocks we don't wish to allow writes to dynamic memories). */
static soc_reg_t qax_dynamic_mem_disable_regs[] = {
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for EDB */
    EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for EPNI */
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IEP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for IHB */
    IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PMH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PML_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_SPU_WRAPPER_TOP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for KAPS */
    MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    NBIH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    NBIL_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for PPDB_B */
    PTS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TXQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

static soc_reg_t qux_dynamic_mem_enable_regs[] = {
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IEP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_SPU_WRAPPER_TOP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    KAPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    NIF_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PTS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TXQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/* Registers to disable QUX dynamic memory writes (for blocks we don't wish to allow writes to dynamic memories). */
static soc_reg_t qux_dynamic_mem_disable_regs[] = {
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for EDB */
    EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for EPNI */
    IEP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for IHB */
    IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPSEC_SPU_WRAPPER_TOP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for KAPS */
    MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    NIF_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for OAMP */
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /* Access to dynamic memories needed for PPDB_B */
    PTS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TXQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/*
 * Functions
 */


/*
 * Function:
 *      soc_qax_tbls_init
 * Purpose:
 *      initialize all tables relevant for QAX.
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
int soc_qax_tbls_init(int unit)
{
    /* define bitmap for memory exclusions */
    uint32 sch_init_val = 1;
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize memory zeroing exclusion to the given array. */
    soc_jer_tbls_zero_init(unit, SOC_IS_QUX(unit) ? qux_tbls_excluded_mem_list : qax_tbls_excluded_mem_list);
        
    SOCDNX_IF_ERR_EXIT(arad_tbl_access_init_unsafe(unit)); /* init arad_fill_table_with_variable_values_by_caching() */
    SOCDNX_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, SOC_IS_QUX(unit) ? qux_dynamic_mem_enable_regs : qax_dynamic_mem_enable_regs, 1)); /* enable dynamic memory writes */

    /* Init tables having none zero values */

    SOCDNX_IF_ERR_EXIT(soc_qax_sch_tbls_init(unit)); /* SCH block */
    SOCDNX_IF_ERR_EXIT(soc_qax_tar_tbls_init(unit)); /* TAR block */

    /* IRE block */

    SOCDNX_IF_ERR_EXIT(soc_qax_ihp_tbls_init(unit)); /* IHP block */

    SOCDNX_IF_ERR_EXIT(soc_qax_cgm_tbls_init(unit)); /* CGM block */

    SOCDNX_IF_ERR_EXIT(soc_qax_ips_tbls_init(unit)); /* IPS block */

    SOCDNX_IF_ERR_EXIT(soc_qax_pts_tbls_init(unit)); /*PTS block*/

    if (!SOC_IS_QUX(unit)) {
    SOCDNX_IF_ERR_EXIT(soc_qax_fcr_tbls_init(unit)); /* FCR block */
    }

    SOCDNX_IF_ERR_EXIT(soc_qax_txq_tbls_init(unit)); /* TXQ block */
    /* ITE block */

    SOCDNX_IF_ERR_EXIT(soc_jerplus_fdt_tbls_init(unit)); /* FDT block */

    SOCDNX_IF_ERR_EXIT(soc_qax_egq_tbls_init(unit)); /* EGQ block */
    SOCDNX_IF_ERR_EXIT(soc_qax_epni_tbls_init(unit)); /* EPNI block */

    SOCDNX_IF_ERR_EXIT(soc_qax_kaps_tbls_init(unit)); /* KAPS block */

    /* OAMP block */

    /* MRPS+MRPS_EM block (IMP+IEP) */
    SOCDNX_IF_ERR_EXIT(soc_qax_imp_tbls_init(unit));
	SOCDNX_IF_ERR_EXIT(soc_qax_iep_tbls_init(unit));

    /* PPDB block */
    SOCDNX_IF_ERR_EXIT(soc_qax_ppdb_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(qax_mult_rplct_tbl_entry_unoccupied_set_all(unit));
    SOCDNX_IF_ERR_EXIT(qax_mcds_multicast_init2(unit));

    /* initialize all tables to zero, except for exception array, and marked not to be zeroed.  */
    SOCDNX_IF_ERR_EXIT(soc_jer_tbls_zero(unit));

    SOCDNX_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, SOC_IS_QUX(unit) ? qux_dynamic_mem_disable_regs : qax_dynamic_mem_disable_regs, 0)); /* disable dynamic memory writes */
    /* trigger SCH_SCHEDULER_INIT to reset dynamic SCH tables, needs to be after the dynamic table zeroing */
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), 0, &sch_init_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_ppdb_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    /* tcam memory init, special handle */
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_VALIDf, 0x0);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_BANK_COMMANDm, MEM_BLOCK_ANY, table_entry));

    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_VALIDf, 0x0);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_BANK_COMMANDm, MEM_BLOCK_ANY, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_ihp_tbls_init(int unit)
{    
    uint32 entry[20] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    /* default FID = VSI */
    soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, entry, FID_CLASSf, 7);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_VSI_LOW_CFG_2m, entry));    
	
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_ips_tbls_init(int unit)
{
    uint32 entry[20] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    /* Init common Jericho tables */
    SOCDNX_IF_ERR_EXIT(soc_jer_ips_tbls_init(unit));

    /* set to max (127) CAM full thresholds */
    soc_mem_field32_set(unit, IPS_CFMEMm, entry, DRAM_CAM_FULL_THRESHOLDf, 127);
    soc_mem_field32_set(unit, IPS_CFMEMm, entry, FABRIC_CAM_FULL_THRESHOLDf, 127);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IPS_CFMEMm, entry));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_pts_tbls_init(int unit)
{
    uint32 txq_per_wfq_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pts_per_wfq_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pts_per_wfq_mesh_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pts_per_shaper_cfg_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pts_shaper_fmc_cfg_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pts_per_pdq_default[SOC_MAX_MEM_WORDS] = {0};
    int i, num_of_queues_to_init;

    /* In DRAM/SRAM: Queue[n+1].start = Queue[n].start + Queue[n].size + 1 */

    pdq_init_vals_t pdq_init_vals[] = {
            /* queue_start, queue_size, dqcq_word_fc_threshold, eir_crdt_fc_threshold, dqcq_fc_threshold*/
            {0x0,   0x1ff,  0x17f4, 0xaa, 0xa9},    /*PDQ_SRAM_LOC_HP 0*/
            {0x0,   0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_LOC_HP 1*/
            {0x200, 0x1ff,  0x17f4, 0xaa, 0xa9},    /*PDQ_SRAM_LOC_LP 2*/
            {0x92,  0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_LOC_LP 3*/
            {0x400, 0x1ff,  0x17f4, 0xaa, 0xa9},    /*PDQ_SRAM_FUC_HP 4*/
            {0x124, 0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_FUC_HP 5*/
            {0x600, 0x1ff,  0x17f4, 0xaa, 0xa9},    /*PDQ_SRAM_FUC_LP 6*/
            {0x1b6, 0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_FUC_LP 7*/
            {0x800, 0x1ff,  0x17f4, 0xaa, 0xa9},    /*PDQ_SRAM_FMC_HP 8*/
            {0x248, 0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_FMC_HP 9*/
            {0xa00, 0x1ff,  0x17f4, 0xaa, 0xa9},    /*PDQ_SRAM_FMC_LP 10*/
            {0x2da, 0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_FMC_LP 11*/
            {0xc00, 0x54,   0x3f0,  0x1c, 0x3a},    /*PDQ_SRAM_DEL 12*/
            {0x36c, 0x91,   0x6cc,  0x48, 0x48},    /*PDQ_DRAM_DEL 13*/
            {0xc55, 0x154,  0xff0,  0x71, 0x70}     /*PDQ_SRAM_TO_DRAM_HP 14*/
    };

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    soc_mem_field32_set(unit, TXQ_PER_WFQ_CFGm, txq_per_wfq_default, WFQ_WEIGHT_0f, 0x1);
    soc_mem_field32_set(unit, TXQ_PER_WFQ_CFGm, txq_per_wfq_default, WFQ_WEIGHT_1f, 0x1);
    soc_mem_field32_set(unit, TXQ_PER_WFQ_CFGm, txq_per_wfq_default, WFQ_MASK_RD_SIZEf, 0x0);
    soc_mem_field32_set(unit, TXQ_PER_WFQ_CFGm, txq_per_wfq_default, DEFAULT_PKT_SIZEf, 0x40);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, TXQ_PER_WFQ_CFGm, txq_per_wfq_default));

    soc_mem_field32_set(unit, PTS_PER_WFQ_CFGm, pts_per_wfq_default, WFQ_WEIGHT_0f, 0x1);
    soc_mem_field32_set(unit, PTS_PER_WFQ_CFGm, pts_per_wfq_default, WFQ_WEIGHT_1f, 0x1);
    soc_mem_field32_set(unit, PTS_PER_WFQ_CFGm, pts_per_wfq_default, WFQ_MASK_RD_SIZEf, 0x0);
    soc_mem_field32_set(unit, PTS_PER_WFQ_CFGm, pts_per_wfq_default, DEFAULT_PKT_SIZEf, 0x40);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, PTS_PER_WFQ_CFGm, pts_per_wfq_default));

    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, pts_per_wfq_mesh_default, WFQ_WEIGHT_0f, 0x1);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, pts_per_wfq_mesh_default, WFQ_WEIGHT_1f, 0x1);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, pts_per_wfq_mesh_default, WFQ_WEIGHT_2f, 0x1);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, pts_per_wfq_mesh_default, WFQ_MASK_RD_SIZEf, 0x0);
    soc_mem_field32_set(unit, PTS_PER_WFQ_MESH_CFGm, pts_per_wfq_mesh_default, DEFAULT_PKT_SIZEf, 0x40);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, PTS_PER_WFQ_MESH_CFGm, pts_per_wfq_mesh_default));

    /* ingress shapers configuration- same as HW default */
    soc_mem_field32_set(unit, PTS_PER_SHAPER_CFGm, pts_per_shaper_cfg_default, SHAPER_DELAYf, 0x1);
    soc_mem_field32_set(unit, PTS_PER_SHAPER_CFGm, pts_per_shaper_cfg_default, SHAPER_CALf, 0xffff);
    soc_mem_field32_set(unit, PTS_PER_SHAPER_CFGm, pts_per_shaper_cfg_default, SHAPER_MAX_CREDITf, 0xffff);
    soc_mem_field32_set(unit, PTS_PER_SHAPER_CFGm, pts_per_shaper_cfg_default, DCR_FACTORf, 0x3); /*different from HW default, same as in Jer*/
    soc_mem_field32_set(unit, PTS_PER_SHAPER_CFGm, pts_per_shaper_cfg_default, SHAPER_ENf, 0x1);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, PTS_PER_SHAPER_CFGm, pts_per_shaper_cfg_default));

    /* FMQ slow start default shapers' configuration (slow start is disabled)- same as HW default */
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_ENf, 0x0);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_TIMER_PERIOD_0f, 0x11); /*different from HW default, same as in Jer*/
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_TIMER_PERIOD_1f, 0x11); /*different from HW default, same as in Jer*/
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_DELAY_0f, 0x1);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_DELAY_1f, 0x1);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_CAL_0f, 0xffff);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, SLOW_START_CAL_1f, 0xffff);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, JITTER_ENf, 0x0);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, JITTER_MASKf, 0xfff);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, DEL_JITTER_ENf, 0x0);
    soc_mem_field32_set(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default, DEL_JITTER_MASKf, 0xfff);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, PTS_SHAPER_FMC_CFGm, pts_shaper_fmc_cfg_default));

    /*
     * PDQ (DQCQ) Configuration -
     * 16 contexts:
     * - Mesh: (Local0, Dest0, Dest1/MC) x SRAM/DRAM x (HP, LP)
     * - Fabric: (Local0, UC, MC) x  SRAM/DRAM x (HP, LP)
     * - For Mesh and Fabric: Delete x SRAM/DRAM + (SRAM to DRAM) x (HP, LP)
     */
    num_of_queues_to_init = sizeof(pdq_init_vals) / sizeof(*pdq_init_vals);
    for (i = 0; i < num_of_queues_to_init; ++i) {
        sal_memset(pts_per_pdq_default, 0, sizeof(pts_per_pdq_default));
        soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, QUEUE_STARTf, pdq_init_vals[i].queue_start);
        soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, QUEUE_SIZEf, pdq_init_vals[i].queue_size);
        soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, DQCQ_WORD_FC_THf, pdq_init_vals[i].pdq_word_fc_threshold);
        soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, EIR_CRDT_FC_THf, pdq_init_vals[i].eir_crdt_fc_threshold);
        soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, DQCQ_FC_THf, pdq_init_vals[i].pdq_fc_threshold);
        SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, i, pts_per_pdq_default));
    }

    /*PDQ_DRAM_UNUSED 15*/

    /*PDQ_SRAM_TO_DRAM_LP 16*/
    sal_memset(pts_per_pdq_default, 0, sizeof(pts_per_pdq_default));
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, QUEUE_STARTf, 0xdaa);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, QUEUE_SIZEf, 0x254);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, DQCQ_WORD_FC_THf, 0x1bf0);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, EIR_CRDT_FC_THf, 0xc6);
    soc_mem_field32_set(unit, PTS_PER_PDQ_CFGm, pts_per_pdq_default, DQCQ_FC_THf, 0xc5);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, PTS_PER_PDQ_CFGm, 0, MEM_BLOCK_ALL, 16, pts_per_pdq_default));

    jer_mark_memory(unit, PTS_PER_PDQ_CFGm);

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_qax_fcr_tbls_init(int unit)
{
    uint32 table_entry[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    /*
     *  all devices is set to EFMS bypass enabled, EFMS is in bypass mode by default
     */
    soc_mem_field32_set(unit, FCR_EFMS_SOURCE_PIPEm, table_entry, DATAf, 0xff); /*1 bit for each device, 8 devices in row */
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, FCR_EFMS_SOURCE_PIPEm, table_entry));
    soc_mem_field32_set(unit, FCR_EFMS_SOURCE_PIPEm, table_entry, DATAf, 0);

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_qax_cgm_tbls_init(int unit)
{
    uint32 entry[20] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    /* set SRAM only mode */
    soc_mem_field32_set(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, entry, SRAM_DRAM_ONLY_MODEf, SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams ? 0 : 1);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, entry));
    sal_memset(entry, 0, sizeof(entry));

    /* Init the Eth mtr table*/
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, CGM_ETH_MTR_PTR_MAPm, MEM_BLOCK_ANY, entry));

    /* disable CNI by default */
    soc_mem_field32_set(unit, CGM_CNI_PRMSm, entry, MAX_SIZE_THf, 0xfff);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, CGM_CNI_PRMSm, entry));
    sal_memset(entry, 0, sizeof(entry));

    /* 
       initialize shared occupied reject thresholds not to discard by default
       reset value of these fields is 0 -- would discard everything 
    */
    soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry, WORDS_SET_THf, 0x1fff);
    soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry, WORDS_CLR_THf, 0x1fff);
    soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry, SRAM_WORDS_SET_THf, 0xfff);
    soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry, SRAM_WORDS_CLR_THf, 0xfff);
    soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry, SRAM_PDS_SET_THf, 0xfff);
    soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry, SRAM_PDS_CLR_THf, 0xfff);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, CGM_VOQ_SHRD_OC_RJCT_THm, entry));
    sal_memset(entry, 0, sizeof(entry));
	
	/* Set VSQ shared pools to max */
    soc_mem_field32_set(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry, WORDS_SET_THf, 0x1fff);
    soc_mem_field32_set(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry, WORDS_CLR_THf, 0x1fff);
    soc_mem_field32_set(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry, SRAM_BUFFERS_SET_THf, 0xfff);
    soc_mem_field32_set(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry, SRAM_BUFFERS_CLR_THf, 0xfff);
    soc_mem_field32_set(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry, SRAM_PDS_SET_THf, 0xfff);
    soc_mem_field32_set(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry, SRAM_PDS_CLR_THf, 0xfff);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, CGM_VSQ_SHRD_OC_RJCT_THm, entry));
    sal_memset(entry, 0, sizeof(entry));

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * Tables Init Functions
 */ 
int soc_qax_sch_tbls_init(int unit)
{
    uint32 table_entry[4] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    if (!SOC_IS_QUX(unit)) {
    /* all relevant bits initialized to SOC_TMC_MAX_FAP_ID */
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, SOC_TMC_MAX_FAP_ID);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry));
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, 0);
    }
    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 128);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_PS_8P_RATES_PSRm, table_entry));

    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_0_WEIGHTf, 0x1);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_1_WEIGHTf, 0x2);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_2_WEIGHTf, 0x4);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_3_WEIGHTf, 0x8);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_4_WEIGHTf, 0x10);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_5_WEIGHTf, 0x20);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_6_WEIGHTf, 0x40);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_7_WEIGHTf, 0x80);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_egq_tbls_init(int unit)
{
    soc_reg_above_64_val_t data;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_PCTm, data));

    /* Set the VSI-Membership table to all 1's */
    SOC_REG_ABOVE_64_ALLONES(data);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_VSI_MEMBERSHIPm, data));


    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_DSP_PTR_MAPm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_PER_IFC_CFGm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PP_PPCTm, data, MTUf, 0x3fff);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_PP_PPCTm, data));


    if (SOC_DPP_CONFIG(unit)->emulation_system) {
        /*
         * Tables reset script used for emulation is not effective on this
         * table for some reason. This is a small table so this is not too
         * heavy an operation for emulation.
         */
        SOC_REG_ABOVE_64_CLEAR(data);
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_IVEC_TABLEm, data));
    }

    SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, EGQ_MC_BITMAP_MAPPINGm);
    for (i=0; i < SOC_TMC_NOF_FAP_PORTS_PER_CORE ; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, EGQ_MC_BITMAP_MAPPINGm, MEM_BLOCK_ALL, i, data));
        soc_mem_field32_set(unit, EGQ_MC_BITMAP_MAPPINGm, data, DSP_PTRf, i);
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, EGQ_MC_BITMAP_MAPPINGm, MEM_BLOCK_ALL, i, data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_epni_tbls_init(int unit)
{
    uint32 table_entry[SAND_MAX_U32S_IN_MEM_ENTRY] = {0};
    uint32 tx_tag_table_entry[8] = {0};
    int i;
    int dpp, dp, entry_index, pcp_dei;
    uint32 value;
    /* this outlif points to a null entry, allocated at init */
    uint32 out_lif_null_entry = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID); 

    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_SAND_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_RECYCLE_COMMANDm, table_entry));

    /* init the first half of IHP_RECYCLE_COMMANDm to a static 1-1 mapping from recycle commands to inbound mirror action profiles */
    for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
        value = 0; /* other recycling command actions are disabled */
        soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, MIRROR_PROFILEf, i);
        if (i > 0) { /* for all valid outbound mirror profile , set highest strength for forward action packet to drop */      
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, FORWARD_STRENGTHf, 7); /* highest strength */
            /* setting recycle snoop strength according soc property. default is zero */
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, SNOOP_STRENGTHf,
                                      soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "recycle_snoop_strength", 0));
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, &value, CPU_TRAP_CODEf, SOC_PPC_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0); /* trap code SOC_PPC_TRAP_CODE_USER_DEFINED_27 dest=queue 128K-1 */  
            SOCDNX_IF_ERR_EXIT(WRITE_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ALL, i, &value));
        }
    }

    SOCDNX_SAND_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_PCP_DEI_DP_MAPPING_TABLEm, table_entry));
    /* default EPNI_PCP_DEI_DP_MAPPING_TABLE to  mapping from select PCP-DEI to new out PCP-DEI */
    /* table index : dp_profile(2) <<6 + dp(2) <<4 + selected-pcp-dei */
    for (dpp = 0; dpp <=ARAD_PP_DP_PROFILE_NDX_MAX; ++dpp) {
        for (dp = 0; dp <= ARAD_PP_DP_MAX_VAL; ++dp) {
            for (pcp_dei =0; pcp_dei <= ARAD_PP_PCP_DEI_MAX_VAL; ++pcp_dei){
                value =0;
                entry_index = (dpp << 6) + (dp << 4) + pcp_dei; 
                soc_mem_field32_set(unit, EPNI_PCP_DEI_DP_MAPPING_TABLEm, &value, PUSH_PCP_DEIf, pcp_dei);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_PCP_DEI_DP_MAPPING_TABLEm(unit, MEM_BLOCK_ALL, entry_index, &value));
            } 
        }
    }
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_bitstream_fill(tx_tag_table_entry, 8));

    SOCDNX_SAND_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_TX_TAG_TABLEm, &tx_tag_table_entry));

    soc_mem_field32_set(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry, MTUf, 0x3fff);
    /* we init default outlif with an invalid AC outlif value */
    soc_mem_field32_set(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry, DEFAULT_SEM_RESULTf, out_lif_null_entry); 

    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry));

    if (SOC_DPP_CONFIG(unit)->emulation_system) {
        /*
         * Tables reset script used for emulation is not effective on this
         * table for some reason. This is a small table so this is not too
         * heavy an operation for emulation.*
         */
        sal_memset(table_entry, 0, sizeof(table_entry));
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_IVEC_TABLEm, table_entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_kaps_tbls_init(int unit)
{
    uint32 table_default_path_selection[1] = {0x0};
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;
    SOC_REG_ABOVE_64_ALLONES(data);
    SOCDNX_IF_ERR_EXIT(WRITE_KAPS_INDIRECT_WR_MASKr(unit, data));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, KAPS_RPB_TCAM_HIT_INDICATIONm, KAPS_BLOCK(unit, 0), table_default_path_selection));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_imp_tbls_init(int unit)
{
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDA_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDB_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDA_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDB_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDA_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDB_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDA_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDB_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IMP_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_iep_tbls_init(int unit)
{
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDA_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDB_PRFCFG_0m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDA_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDB_PRFCFG_1m, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDA_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDB_PRFSELm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDA_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDB_IN_PP_PORT_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));
	SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IEP_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MEM_BLOCK_ANY, table_entry));

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
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, TAR_DESTINATION_TABLEm, table_entry)); /* fill table with the entry */
    sal_memset(table_entry, 0, sizeof(table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_qax_dtq_tbls_single_context_init(int unit, int queue_start, int queue_size, int fc_2_threshold, int fc_1_threshold, int spr_fc_threshold, int queue_index)
{
    uint32 entry_buf[20];
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(entry_buf, 0, sizeof(entry_buf));
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry_buf, QUEUE_STARTf, queue_start);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry_buf, QUEUE_SIZEf, queue_size);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry_buf, DTQ_PDQ_FC_2_THf, fc_2_threshold);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry_buf, DTQ_PDQ_FC_1_THf, fc_1_threshold);
    soc_mem_field32_set(unit, TXQ_PER_DTQ_CFGm, entry_buf, DTQ_SPR_FC_THf, spr_fc_threshold);
    SOCDNX_IF_ERR_EXIT(soc_mem_array_write(unit, TXQ_PER_DTQ_CFGm, 0, MEM_BLOCK_ALL, queue_index, entry_buf));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Contexts:
 *  UC x (SRAM/DRAM) - queues 0, 1
 *  MC x (SRAM/DRAM) - queues 4, 5
 */
STATIC int
soc_qax_dtq_tbls_contexts_init_mesh(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*TXQ DTQ SRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 128, 90, 65, 90, 0));
    /*TXQ DTQ DRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 767, 356, 256, 356, 1));

    /*TXQ DTQ SRAM 2*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 129, 128, 89, 64, 89, 4));
    /*TXQ DTQ DRAM 2*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 768, 255, 179, 128, 179, 5));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Contexts:
 *  All traffic x (SRAM/DRAM) - queues 0, 1
 */
STATIC int
soc_qax_dtq_tbls_contexts_init_single_queue_clos(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*TXQ DTQ SRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 255, 205, 154, 205, 0));

    /*TXQ DTQ DRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 1023, 820, 615, 820, 1));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Contexts:
 *  UC x (SRAM/DRAM) - queues 0, 1
 *  MC x (SRAM/DRAM) - queues 2, 3
 */
STATIC int
soc_qax_dtq_tbls_contexts_init_dual_queue_clos(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*TXQ DTQ SRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 127, 89, 64, 89, 0));
    /*TXQ DTQ DRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 511, 409, 307, 409, 1));

    /*TXQ DTQ SRAM 1*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 128, 127, 89, 64, 89, 2));
    /*TXQ DTQ DRAM 1*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 512, 511, 409, 307, 409, 3));

exit:
    SOCDNX_FUNC_RETURN;
}

/* Contexts:
 *  UC x (SRAM/DRAM) - queues 0, 1
 *  MC_HP x (SRAM/DRAM) - queues 2, 3
 *  MC_LP x (SRAM/DRAM) - queues 4, 5
 */
STATIC int
soc_qax_dtq_tbls_contexts_init_triple_queue_clos(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*TXQ DTQ SRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 127, 89, 64, 89, 0));
    /*TXQ DTQ DRAM 0*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 511, 409, 307, 409, 1));

    /*TXQ DTQ SRAM 1*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 128, 63, 51, 38, 51, 2));
    /*TXQ DTQ DRAM 1*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 512, 255, 205, 154, 205, 3));

    /*TXQ DTQ SRAM 2*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 192, 63, 51, 38, 51, 4));
    /*TXQ DTQ DRAM 2*/
    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 768, 255, 205, 154, 205, 5));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
int soc_qax_dtq_tbls_init(int unit)
{
    soc_dcmn_fabric_pipe_map_type_t fabric_pipe_map_type;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    fabric_pipe_map_type = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config.mapping_type;

    /* In unused queues all values of TXQ_PER_DTQ_CFGm are HW set to 0 => Setting TH1 and TH2 to be positive to avoid flow control from unused queues.
           Otherwise queue content (0) >= FC threshold (0) and flow control is issued, causing packets to be stuck at PDQ.
           Used queues initialization by soc_qax_dtq_contexts_init_ follows */
    for (i = 0; i < QAX_TBLS_MAX_NUM_OF_DTQS; ++i) {
        SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_single_context_init(unit, 0, 0,  QAX_TBLS_UNUSED_DTQ_PDQ_FC_TH, QAX_TBLS_UNUSED_DTQ_PDQ_FC_TH, 0, i));
    }

    if (SOC_DPP_IS_MESH(unit)){
        SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_contexts_init_mesh(unit));

    } else if (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_FE ||
            soc_feature(unit, soc_feature_packet_tdm_marking)) {

        switch (fabric_pipe_map_type) { /* TDM traffic goes straight to FDT (bypass) without entering DTQs, so DTQ_MODE ignores TDM */
        case soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_contexts_init_triple_queue_clos(unit));
            break;

        case soc_dcmn_fabric_pipe_map_triple_uc_mc_tdm:
        case soc_dcmn_fabric_pipe_map_dual_uc_mc:
            SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_contexts_init_dual_queue_clos(unit));
            break;

        case soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm:
        case soc_dcmn_fabric_pipe_map_single:
            SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_contexts_init_single_queue_clos(unit));
            break;

        case soc_dcmn_fabric_pipe_map_triple_custom:
        case soc_dcmn_fabric_pipe_map_dual_custom:
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOC_MSG("custom mode is not supported")));
            break;
        }
    }

    jer_mark_memory(unit, TXQ_PER_DTQ_CFGm);

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

    /* mark the memory not to be later zeroed */
    SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, TXQ_PRIORITY_BITS_MAPPING_2_FDTm);

    SOCDNX_IF_ERR_EXIT(soc_qax_dtq_tbls_init(unit));

exit:
      SOCDNX_FUNC_RETURN;
}

