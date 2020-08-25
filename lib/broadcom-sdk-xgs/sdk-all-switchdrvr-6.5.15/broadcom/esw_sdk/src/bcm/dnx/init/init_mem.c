/** \file init_mem.c
 * DNX init memories.
 * - Set memories to default values
 * - Get the default value defined per each memory
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MEM

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/control.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>

#include <soc/mcm/memregs.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/legacy/dnxc_mem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

#include <soc/drv.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#ifdef DNX_EMULATION_1_CORE
#define PRINT_MEM_INIT_TIME
#ifdef PRINT_MEM_INIT_TIME
#include <sal/core/time.h>
#endif
#endif /* DNX_EMULATION_1_CORE */
/*
 * }
 */
/*
 * Typedefs.
 * {
 */
/*
 *
 */

/* registers to enable JR2 dynamic memory writes */
static const soc_reg_t jr2_dynamic_mem_enable_regs[] = {
    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CDUM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DHC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ERPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EVNT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * HBC_ENABLE_DYNAMIC_MEMORY_ACCESSr, to be enabled for silicon
     */
    HBMC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPF_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITPPD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    KAPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MACT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MCP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MTM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OCB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TCAM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/* registers to enable JR2 dynamic memory writes */
static const soc_reg_t jr2_dynamic_mem_disable_regs[] = {

    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CDUM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     *CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
     Because of :
     CRPS_CRPS_TYPE_MAP_MEM
     CRPS_CRPS_EXPANSION_MAP_A_MEM
     CRPS_CRPS_EXPANSION_MAP_B_MEM
     CRPS_CRPS_EXPANSION_MAP_C_MEM
     */

    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,

    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * FDR_ENABLE_DYNAMIC_MEMORY_ACCESSr, because of FDR_FDR_MC
     */
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * HBC_ENABLE_DYNAMIC_MEMORY_ACCESSr, not to be enabled now (by Nir)
     * HBMC_ENABLE_DYNAMIC_MEMORY_ACCESSr, not to be enabled now (by Nir)
     */

    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#if 0   
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DHC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TCAM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    KAPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ERPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EVNT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MACT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPPF_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,

#endif

    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * IPT_ENABLE_DYNAMIC_MEMORY_ACCESSr, needed for IPT_PCP_CFGm
     */
    IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr, needed for IRE_CTXT_MAPm
     */
    ITPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ITPPD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * MCP_ENABLE_DYNAMIC_MEMORY_ACCESSr, needed for MCP_ENG_DB_C_EXT_MEM, MCP_ENG_DB_A_EXT_MEM
     */
    MRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MTM_ENABLE_DYNAMIC_MEMORY_ACCESSr,

    OCB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr, because of SPB_CONTEXT_MRU
     */
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,

    INVALIDr
};

/*
 * Functions.
 * {
 */

/**
 * \brief - reset the tcam hit indication memories.
 */
static shr_error_e
dnx_init_mem_tcam_hit_indication_reset(
    int unit,
    soc_mem_t mem)
{
    uint32 mem_addr;
    uint8 at;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_val_zero;
    int blk_index, blk_index_min, blk_index_max;
    int index, index_min, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;

    SHR_FUNC_INIT_VARS(unit);

    reg_val = 0;
    SOC_REG_ABOVE_64_CLEAR(reg_val_zero);

    /** get info about table */
    blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
    blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);
    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        array_index_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
    }

    for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
    {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            for (index = index_min; index <= index_max; index++)
            {
                mem_addr = soc_mem_addr_get(unit, mem, array_index, blk_index, index, &at);

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMANDr(unit, SOC_CORE_ALL, 0xe00003));

                reg_val = 0;
                soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
                soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_ADDRESSr(unit, SOC_CORE_ALL, reg_val));

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_WR_MASKr(unit, SOC_CORE_ALL, 0xFF));

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_WR_DATAr(unit, SOC_CORE_ALL, reg_val_zero));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Reset all valid writeable memories which are not included in mem_exclude_bitmap
 *
 * \param [in] unit - unit #
 * \param [in] mem_exclude_bitmap - Each bit represent the memories that should be excluded (will be skipped)
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_init_mem_reset(
    int unit,
    uint32 *mem_exclude_bitmap)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    unsigned max_array_index = 0;
    unsigned min_array_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Do not reset mem in pcid
     */
    if ((SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)) && !dnx_data_dev_init.mem.force_zeros_get(unit))
    {
        SHR_EXIT();
    }

    /*
     * Set the rest of the tables to zero
     */
    /** iterate over all memories */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        /** filter invalid memories*/
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }

         /** filter read only memories */
        if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
        {
            continue;
        }

         /** filter aliased memories - to avoid duplication */
        if (mem != SOC_MEM_ALIAS_MAP(unit, mem) && SOC_MEM_IS_VALID(unit, SOC_MEM_ALIAS_MAP(unit, mem)))
        {
            continue;
        }

         /** filter mems included in specified defaults */
        if (SHR_BITGET(mem_exclude_bitmap, mem))
        {
            continue;
        }

        switch (mem)
        {       /* skip failing memories */
                /*
                 * The HBM memories are not real memories
                 */
            case BRDC_HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case BRDC_HBC_HBM_DRAM_CPU_ACCESSm:
            case BRDC_FSRD_FSRD_PROM_MEMm:
            case HBC_HBM_DRAM_CPU_ACCESSm:
            case HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case HBMC_HBM_PHY_CHM_REGISTER_ACCESSm:

                /*
                 * The CDU instrumentation and rx/tx memories do not have to be initialized and can not be written by
                 * DMA
                 */
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm:
            case CDU_RX_MEMA_CTRLm:
            case CDU_RX_MEMB_CTRLm:
            case CDU_RX_MEMC_CTRLm:
            case CDU_RX_MEMD_CTRLm:
            case CDU_RX_MEME_CTRLm:
            case CDU_RX_MEMF_CTRLm:
            case CDU_RX_MEMG_CTRLm:
            case CDU_RX_MEMH_CTRLm:
            case CDU_RX_MEMI_CTRLm:
            case CDU_RX_MEMJ_CTRLm:
            case CDU_RX_MEMK_CTRLm:
            case CDU_RX_MEML_CTRLm:
            case CDU_RX_MEMM_CTRLm:
            case CDU_RX_MEMN_CTRLm:
            case CDU_RX_MEMO_CTRLm:
            case CDU_RX_MEMP_CTRLm:
            case CDU_TX_MEMA_CTRLm:
            case CDU_TX_MEMB_CTRLm:
            case CDU_TX_MEMC_CTRLm:
            case CDU_TX_MEMD_CTRLm:
            case CDU_TX_MEME_CTRLm:
            case CDU_TX_MEMF_CTRLm:
            case CDU_TX_MEMG_CTRLm:
            case CDU_TX_MEMH_CTRLm:
            case CDU_TX_MEMI_CTRLm:
            case CDU_TX_MEMJ_CTRLm:

                /*
                 * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization
                 */
            case DDHB_MACRO_0_ABK_BANK_Am:
            case DDHB_MACRO_0_ABK_BANK_Bm:
            case DDHB_MACRO_0_ENTRY_BANKm:
            case DDHB_MACRO_1_ABK_BANK_Am:
            case DDHB_MACRO_1_ABK_BANK_Bm:
            case DDHB_MACRO_1_ENTRY_BANKm:
            case DDHA_MACRO_0_ABK_BANK_Am:
            case DDHA_MACRO_0_ABK_BANK_Bm:
            case DDHA_MACRO_0_ENTRY_BANKm:
            case DDHA_MACRO_1_ABK_BANK_Am:
            case DDHA_MACRO_1_ABK_BANK_Bm:
            case DDHA_MACRO_1_ENTRY_BANKm:
            case DHC_MACRO_ABK_BANK_Am:
            case DHC_MACRO_ABK_BANK_Bm:
            case DHC_MACRO_ENTRY_BANKm:

                /*
                 * These are address spaces saved for ilkn memory mapped registers.
                 */
            case ILE_PORT_0_CPU_ACCESSm:
            case ILE_PORT_1_CPU_ACCESSm:

                /*
                 * All below are either not real memories or don't have protection. - Mosi Ravia
                 */
            case KAPS_RPB_TCAM_CPU_COMMANDm:
            case KAPS_TCAM_ECC_MEMORYm:
            case MDB_ARM_KAPS_TCMm:
            case MDB_ARM_MEM_0m:
            case MDB_ARM_MEM_10000m:
            case MDB_ARM_MEM_30000m:
            case MDB_EEDB_ABK_BANKm:
            case MACT_CPU_REQUESTm:
            case ECI_SAM_CTRLm:

                /*
                 * SCH_SCHEDULER_INIT is not a real memory
                 */
            case SCH_SCHEDULER_INITm:
            case SQM_DEQ_QSTATE_PENDING_FIFOm:
            case SQM_ENQ_COMMAND_RXIm:
            case SQM_PDB_LINK_LISTm:
            case SQM_PDMm:
            case SQM_QUEUE_DATA_MEMORYm:
            case SQM_TX_BUNDLE_MEMORYm:
            case SQM_TX_PACKET_DESCRIPTOR_FIFOS_MEMORYm:

                /*
                 * The *_DEBUG_UNIT and *_HIT_INDICATION memories are not real
                 */
            case ERPP_MEM_F000000m:
            case ETPPA_MEM_F000000m:
            case ETPPB_MEM_F000000m:
            case ETPPC_MEM_F000000m:
            case IPPA_MEM_F000000m:
            case IPPB_MEM_F000000m:
            case IPPC_MEM_F000000m:
            case IPPD_MEM_F000000m:
            case IPPE_MEM_B900000m:
            case IPPF_MEM_F000000m:
            case ERPP_EPMFCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTDCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTECS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPACS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPBCS_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSA_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSB_TCAM_HIT_INDICATIONm:
            case IPPD_PMFBCS_TCAM_HIT_INDICATIONm:
            case IPPE_PRTCAM_TCAM_HIT_INDICATIONm:
            case IPPE_LLRCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTACS_TCAM_HIT_INDICATIONm:
            case IPPF_VTCCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTBCS_TCAM_HIT_INDICATIONm:
            case KAPS_RPB_TCAM_HIT_INDICATIONm:
            case KAPS_TCAM_HIT_INDICATIONm:
                continue;
                /*
                 * The ECGM and CFC memories are filled separately
                 */
            case CFC_GLB_RSC_CGM_MAPm:
            case CFC_NIF_PFC_MAPm:
            case ECGM_FDCMm:
            case ECGM_FDCMAXm:
            case ECGM_FQSMm:
            case ECGM_FQSMAXm:
            case ECGM_PDCMm:
            case ECGM_PDCMAXm:
            case ECGM_PQSMm:
            case ECGM_PQSMAXm:
            case ECGM_QDCMm:
            case ECGM_QDCMAXm:
            case ECGM_QQSMm:
            case ECGM_QQSMAXm:
                if (SOC_MEM_IS_ARRAY(unit, mem))
                {
                    soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
                    if (maip)
                    {
                        max_array_index = (maip->numels - 1) + maip->first_array_index;
                        min_array_index = maip->first_array_index;
                    }
                    else
                    {
                        max_array_index = min_array_index = 0;
                    }
                }
                soc_mem_array_fill_range(unit, 1, mem, min_array_index, max_array_index, MEM_BLOCK_ALL,
                                         soc_mem_index_min(unit, mem), soc_mem_index_max(unit, mem), entry);
                continue;
            default:
                break;
        }
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "Zeros memory # %d - %s\n"), mem, SOC_MEM_NAME(unit, mem)));
        DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_MEM_ZEROS);
        SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem, MEM_BLOCK_ALL, entry));
        DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_MEM_ZEROS);

    }

    SHR_IF_ERR_EXIT(dnx_init_mem_tcam_hit_indication_reset(unit, TCAM_TCAM_ACTION_HIT_INDICATIONm));
    SHR_IF_ERR_EXIT(dnx_init_mem_tcam_hit_indication_reset(unit, TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm));

exit:
    SHR_FUNC_EXIT;
}


 shr_error_e
dnx_init_mem_reset_verify(
    int unit)
{
    soc_mem_t mem;
    uint32 entry_def[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 entry_read[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 entry_mask[SOC_MAX_MEM_WORDS] = { 0 };
    int blk_index_min, blk_index_max, index_min, index_max, array_index_min, array_index_max;
    uint32 entry_words;
    int blk_index, array_index, index, found_error=0, found_error_in_table=0;
    int mem_count = 0;
    dnx_init_mem_default_mode_e mode;
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LOG_MODULE, ("Memory defaults verify started\n"));

    /*
     * Set the rest of the tables to zero
     */
    /** iterate over all memories */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        /** filter invalid memories*/
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }

         /** filter read only memories */
        if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
        {
            continue;
        }

        /** filter write only memories */
       if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0)
       {
           continue;
       }

       /** filter signal memories */
       if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_SIGNAL) != 0)
       {
           continue;
       }

         /** filter aliased memories - to avoid duplication */
        if (mem != SOC_MEM_ALIAS_MAP(unit, mem) && SOC_MEM_IS_VALID(unit, SOC_MEM_ALIAS_MAP(unit, mem)))
        {
            continue;
        }


        switch (mem)
        {       /* skip failing memories */
                /*
                 * The HBM memories are not real memories
                 */
            case BRDC_HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case BRDC_HBC_HBM_DRAM_CPU_ACCESSm:
            case BRDC_FSRD_FSRD_PROM_MEMm:
            case HBC_HBM_DRAM_CPU_ACCESSm:
            case HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case HBMC_HBM_PHY_CHM_REGISTER_ACCESSm:

                /*
                 * The CDU instrumentation and rx/tx memories do not have to be initialized and can not be written by
                 * DMA
                 */
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm:
            case CDU_RX_MEMA_CTRLm:
            case CDU_RX_MEMB_CTRLm:
            case CDU_RX_MEMC_CTRLm:
            case CDU_RX_MEMD_CTRLm:
            case CDU_RX_MEME_CTRLm:
            case CDU_RX_MEMF_CTRLm:
            case CDU_RX_MEMG_CTRLm:
            case CDU_RX_MEMH_CTRLm:
            case CDU_RX_MEMI_CTRLm:
            case CDU_RX_MEMJ_CTRLm:
            case CDU_RX_MEMK_CTRLm:
            case CDU_RX_MEML_CTRLm:
            case CDU_RX_MEMM_CTRLm:
            case CDU_RX_MEMN_CTRLm:
            case CDU_RX_MEMO_CTRLm:
            case CDU_RX_MEMP_CTRLm:
            case CDU_TX_MEMA_CTRLm:
            case CDU_TX_MEMB_CTRLm:
            case CDU_TX_MEMC_CTRLm:
            case CDU_TX_MEMD_CTRLm:
            case CDU_TX_MEME_CTRLm:
            case CDU_TX_MEMF_CTRLm:
            case CDU_TX_MEMG_CTRLm:
            case CDU_TX_MEMH_CTRLm:
            case CDU_TX_MEMI_CTRLm:
            case CDU_TX_MEMJ_CTRLm:

                /*
                 * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization
                 */
            case DDHB_MACRO_0_ABK_BANK_Am:
            case DDHB_MACRO_0_ABK_BANK_Bm:
            case DDHB_MACRO_0_ENTRY_BANKm:
            case DDHB_MACRO_1_ABK_BANK_Am:
            case DDHB_MACRO_1_ABK_BANK_Bm:
            case DDHB_MACRO_1_ENTRY_BANKm:
            case DDHA_MACRO_0_ABK_BANK_Am:
            case DDHA_MACRO_0_ABK_BANK_Bm:
            case DDHA_MACRO_0_ENTRY_BANKm:
            case DDHA_MACRO_1_ABK_BANK_Am:
            case DDHA_MACRO_1_ABK_BANK_Bm:
            case DDHA_MACRO_1_ENTRY_BANKm:
            case DHC_MACRO_ABK_BANK_Am:
            case DHC_MACRO_ABK_BANK_Bm:
            case DHC_MACRO_ENTRY_BANKm:

                /*
                 * These are address spaces saved for ilkn memory mapped registers.
                 */
            case ILE_PORT_0_CPU_ACCESSm:
            case ILE_PORT_1_CPU_ACCESSm:

                /*
                 * All below are either not real memories or don't have protection. - Mosi Ravia
                 */
            case KAPS_RPB_TCAM_CPU_COMMANDm:
            case KAPS_TCAM_ECC_MEMORYm:
            case MDB_ARM_KAPS_TCMm:
            case MDB_ARM_MEM_0m:
            case MDB_ARM_MEM_10000m:
            case MDB_ARM_MEM_30000m:
            case MDB_EEDB_ABK_BANKm:
            case MACT_CPU_REQUESTm:
            case ECI_SAM_CTRLm:

                /*
                 * SCH_SCHEDULER_INIT is not a real memory
                 */
            case SCH_SCHEDULER_INITm:
            case SQM_DEQ_QSTATE_PENDING_FIFOm:
            case SQM_ENQ_COMMAND_RXIm:
            case SQM_PDB_LINK_LISTm:
            case SQM_PDMm:
            case SQM_QUEUE_DATA_MEMORYm:
            case SQM_TX_BUNDLE_MEMORYm:
            case SQM_TX_PACKET_DESCRIPTOR_FIFOS_MEMORYm:

                /*
                 * The *_DEBUG_UNIT and *_HIT_INDICATION memories are not real
                 */
            case ERPP_MEM_F000000m:
            case ETPPA_MEM_F000000m:
            case ETPPB_MEM_F000000m:
            case ETPPC_MEM_F000000m:
            case IPPA_MEM_F000000m:
            case IPPB_MEM_F000000m:
            case IPPC_MEM_F000000m:
            case IPPD_MEM_F000000m:
            case IPPE_MEM_B900000m:
            case IPPF_MEM_F000000m:
            case ERPP_EPMFCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTDCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTECS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPACS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPBCS_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSA_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSB_TCAM_HIT_INDICATIONm:
            case IPPD_PMFBCS_TCAM_HIT_INDICATIONm:
            case IPPE_PRTCAM_TCAM_HIT_INDICATIONm:
            case IPPE_LLRCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTACS_TCAM_HIT_INDICATIONm:
            case IPPF_VTCCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTBCS_TCAM_HIT_INDICATIONm:
            case KAPS_RPB_TCAM_HIT_INDICATIONm:
            case KAPS_TCAM_HIT_INDICATIONm:
                continue;
            default:
                break;
        }

        /** get info about table */
        blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
        blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);

        index_min = soc_mem_index_min(unit, mem);
        index_max = soc_mem_index_max(unit, mem);

        entry_words = soc_mem_entry_words(unit, mem);

        if (SOC_MEM_IS_ARRAY(unit, mem))
        {
            array_index_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            array_index_max = 0;
            array_index_min = 0;
        }


        sal_memset(entry_read, 0, sizeof(entry_mask));
        soc_mem_datamask_rw_get(unit, mem, entry_mask);

        found_error_in_table = 0;
        for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
        {
            for (array_index = array_index_min; array_index <= array_index_max; array_index++)
            {
                for (index = index_min; index <= index_max; index++)
                {

                    sal_memset(entry_def, 0, sizeof(entry_def));
                    sal_memset(entry_read, 0, sizeof(entry_read));


                    SHR_IF_ERR_EXIT(dnx_init_mem_default_get(unit, mem, array_index, blk_index, index, entry_def, &mode));
                    if (mode == dnx_init_mem_default_mode_none)
                    {
                        break;
                    }
                    /*soc_mem_parity_field_clear(unit,mem, entry_def, entry_def);*/
                    SHR_IF_ERR_EXIT(utilex_bitstream_and(entry_def, entry_mask, SOC_MEM_WORDS(unit, mem)));

                    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, mem, array_index, blk_index, index, entry_read));
                    /*soc_mem_parity_field_clear(unit,mem, entry_read, entry_read);*/
                    SHR_IF_ERR_EXIT(utilex_bitstream_and(entry_read, entry_mask, SOC_MEM_WORDS(unit, mem)));

                    if (sal_memcmp(entry_def, entry_read, SOC_MEM_BYTES(unit, mem)) != 0)
                    {
                        LOG_ERROR(BSL_LOG_MODULE, ("Memory default verify failed in memory %s (blk_index=%d, array_index=%d, index=%d)\n", SOC_MEM_NAME(unit, mem), blk_index, array_index, index));
                        found_error = 1;
                        found_error_in_table = 1;
                        break;
                    }

                }
                if (found_error_in_table)
                {
                    break;
                }
            }
            if (found_error_in_table)
            {
                break;
            }
            mem_count += 1;
        }
    }

    if (found_error)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Memory defaults verify failed \n");
    }
    else
    {
        LOG_INFO(BSL_LOG_MODULE, ("Memory defaults verify done (%d passed) \n", mem_count));

    }
exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief - fill the memory according to custom values got from a callback using DMA. */
static shr_error_e
dnx_init_mem_default_custom_per_entry_set(
    int unit,
    soc_mem_t mem,
    dnx_init_mem_default_get_f default_cb)
{
    uint32 *entries = NULL, *entry = NULL;
    uint32 entry_words;
    int blk_index, blk_index_min, blk_index_max;
    int index, index_min, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate dma buffer */
    SHR_IF_ERR_EXIT(sand_alloc_dma_mem
                    (unit, 0, (void **) &entries, SOC_MEM_TABLE_BYTES(unit, mem), "dnxc_fill_table_with_index_val"));

    /** get info about table */
    blk_index_min = SOC_MEM_BLOCK_MIN(unit, mem);
    blk_index_max = SOC_MEM_BLOCK_MAX(unit, mem);

    index_min = soc_mem_index_min(unit, mem);
    index_max = soc_mem_index_max(unit, mem);

    entry_words = soc_mem_entry_words(unit, mem);

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        array_index_max = (SOC_MEM_NUMELS(unit, mem) - 1) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
    }

    /** fill and set dma memory */
    for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
    {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            for (index = index_min; index <= index_max; index++)
            {
                entry = entries + (entry_words * index);
                SHR_IF_ERR_EXIT(default_cb(unit, array_index, blk_index, index, entry));
            }

            SHR_IF_ERR_EXIT(soc_mem_array_write_range
                            (unit, 0, mem, array_index, blk_index, index_min, index_max, entries));
        }
    }

exit:
    if (entries != NULL)
    {
        SHR_IF_ERR_EXIT(sand_free_dma_mem(unit, 0, (void **) &entries));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset TCAM CS tables
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_init_tcam_cs_reset(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Reset ingress CS tcam tables */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LLR_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT1_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT2_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT3_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT4_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT5_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD1_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD2_ACL_CONTEXT_SELECTION));
    /** Reset PMF CS tables   */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_IPMF1));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_IPMF2));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_IPMF3));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_EPMF));
    /** Reset IPPB_MPLS_LABEL_TYPES_CAM_0/1 */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_MPLS_LABEL_TYPES_0));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_MPLS_LABEL_TYPES_1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_init_mem_default_init(
    int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 field[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 *mem_exclude_bitmap = NULL;
    const dnx_data_dev_init_mem_default_t *default_mem;
    const dnxc_data_table_info_t *table_info;
    int mem_index;
#ifdef PRINT_MEM_INIT_TIME
    sal_usecs_t start_usecs = sal_time_usecs(), prev_usecs = start_usecs, usecs, passed_secs, last_time;
#endif
    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_EMULATION_1_CORE
    if ((SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)) && !dnx_data_dev_init.mem.force_zeros_get(unit)
        && SOC_IS_JERICHO_2_ONLY(unit))
    {   /* If we rely on memories to be externally zeroed before the run, check for it */
        uint32 entry = 0xffffffff;
        SHR_IF_ERR_EXIT(READ_CGM_REP_CMD_FIFO_OCCUPANCY_RJCT_THm(unit, MEM_BLOCK_ANY, 0, &entry));
        if (entry != 0)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_MEMORY,
                              "This run expects all memories to be zeroed before the run, and they were not%s%s%s",
                              EMPTY, EMPTY, EMPTY);
        }
    }
#endif /* DNX_EMULATION_1_CORE */

    /** Allocate exclude bitmap */
    mem_exclude_bitmap = sal_alloc(SHR_BITALLOCSIZE(NUM_SOC_MEM), "reset memories exclude bitmap");
    sal_memset(mem_exclude_bitmap, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));

    /** Get size of table default size */
    table_info = dnx_data_dev_init.mem.default_info_get(unit);

    /** enable dynamic memory access */
    SHR_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, jr2_dynamic_mem_enable_regs, 1));

    /*
     * Iterate over the table and set value for each mem
     * according to default value mode.
     */
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYZER_MEM_INIT);
    for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
    {
        /** Init vars */
        sal_memset(entry, 0x0, sizeof(entry));
        sal_memset(field, 0x0, sizeof(field));
        default_mem = dnx_data_dev_init.mem.default_get(unit, mem_index);

        if (SHR_BITGET(mem_exclude_bitmap, default_mem->mem))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Memory %s initialized twice in dnx_data_dev_init.mem.default .\n",
                         SOC_MEM_NAME(unit, default_mem->mem));
        }

        /** per mode action */
        switch (default_mem->mode)
        {
            /** Set default value to zero */
            case dnx_init_mem_default_mode_zero:
            {
                /** Reset to zero */
                if (!SAL_BOOT_PLISIM)
                {
                    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, default_mem->mem, MEM_BLOCK_ALL, entry));
                    SHR_BITSET(mem_exclude_bitmap, default_mem->mem);
                }
                break;
            }
            /** Do not set default value */
            case dnx_init_mem_default_mode_none:
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Skipping memory # %d - %s\n"), default_mem->mem,
                             SOC_MEM_NAME(unit, default_mem->mem)));
                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, default_mem->mem);
                break;
            }
            /** Set default value to all ones (0xFFFF...) */
            case dnx_init_mem_default_mode_all_ones:
            {
                if (default_mem->field == INVALIDf)
                {
                    int mem_bits;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Allones memory # %d - %s\n"), default_mem->mem,
                                 SOC_MEM_NAME(unit, default_mem->mem)));
                    mem_bits = soc_mem_entry_bits(unit, default_mem->mem);
                    SHR_BITSET_RANGE(entry, 0, mem_bits);
                    
                    soc_mem_parity_field_clear(unit, default_mem->mem, entry, entry);
                }
                else
                {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Allones memory # %d - %s, field # %d - %s\n"), default_mem->mem,
                                 SOC_MEM_NAME(unit, default_mem->mem), default_mem->field, SOC_FIELD_NAME(unit,
                                                                                                          default_mem->field)));
                    sal_memset(entry, 0x0, sizeof(entry));
                    sal_memset(field, 0xFF, sizeof(field));
                    soc_mem_field_width_fit_set(unit, default_mem->mem, entry, default_mem->field, field);
                }
#ifdef DNX_EMULATION_1_CORE
                if (soc_sand_is_emulation_system(unit))
                {
#ifdef PRINT_MEM_INIT_TIME
                    passed_secs = ((usecs = sal_time_usecs()) - start_usecs) / 1000000;
                    last_time = (usecs - prev_usecs) / 100000;
                    prev_usecs = usecs;
                    LOG_ERROR(BSL_LOG_MODULE, ("%d:%.2d after %d.%ds Allones memory %s\n",
                                               passed_secs / 60, passed_secs % 60, last_time / 10, last_time % 10,
                                               SOC_MEM_NAME(unit, default_mem->mem)));
#endif
                    sand_fill_table_with_entry(unit, default_mem->mem, MEM_BLOCK_ALL, entry);   /* in emulation we want
                                                                                                 * to continue in case
                                                                                                 * of memory init
                                                                                                 * failures, to not
                                                                                                 * loose the long
                                                                                                 * startup time we spent 
                                                                                                 */
                }
                else
#else /* DNX_EMULATION_1_CORE */
                {
                    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, default_mem->mem, MEM_BLOCK_ALL, entry));
                }
#endif /* DNX_EMULATION_1_CORE */
                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, default_mem->mem);

                break;
            }
            /** Each entry value will be the index. */
            case dnx_init_mem_default_mode_index:
            {
#ifdef PRINT_MEM_INIT_TIME
                if (soc_sand_is_emulation_system(unit))
                {
                    passed_secs = ((usecs = sal_time_usecs()) - start_usecs) / 1000000;
                    last_time = (usecs - prev_usecs) / 100000;
                    prev_usecs = usecs;
                    LOG_ERROR(BSL_LOG_MODULE, ("%d:%.2d after %d.%ds Index memory %s\n",
                                               passed_secs / 60, passed_secs % 60, last_time / 10, last_time % 10,
                                               SOC_MEM_NAME(unit, default_mem->mem)));
                }
                else
#endif
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Index memory # %d - %s, field # %d - %s\n"), default_mem->mem,
                                 SOC_MEM_NAME(unit, default_mem->mem), default_mem->field,
                                 default_mem->field < 0 ? "all register" : SOC_FIELD_NAME(unit, default_mem->field)));
                SHR_IF_ERR_EXIT(dnxc_fill_table_with_index_val(unit, default_mem->mem, default_mem->field));
                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, default_mem->mem);

                break;
            }
            /** Custom, expected a callback to provide the default value. */
            case dnx_init_mem_default_mode_custom:
            {
                /*
                 * verify, callback supplied
                 */
                if (default_mem->default_get_cb == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "custom default value requires callback (mem_index %d).\n",
                                 mem_index);
                }
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Custom memory # %d - %s\n"), default_mem->mem,
                             SOC_MEM_NAME(unit, default_mem->mem)));
                SHR_IF_ERR_EXIT(default_mem->default_get_cb(unit, 0, MEM_BLOCK_ALL, 0, entry));
#ifdef DNX_EMULATION_1_CORE
                if (soc_sand_is_emulation_system(unit))
                {
#ifdef PRINT_MEM_INIT_TIME
                    passed_secs = ((usecs = sal_time_usecs()) - start_usecs) / 1000000;
                    last_time = (usecs - prev_usecs) / 100000;
                    prev_usecs = usecs;
                    LOG_ERROR(BSL_LOG_MODULE, ("%d:%.2d after %d.%ds Custom memory %s\n",
                                               passed_secs / 60, passed_secs % 60, last_time / 10, last_time % 10,
                                               SOC_MEM_NAME(unit, default_mem->mem)));
#endif
                    sand_fill_table_with_entry(unit, default_mem->mem, MEM_BLOCK_ALL, entry);   /* in emulation we want
                                                                                                 * to continue in case
                                                                                                 * of memory init
                                                                                                 * failures, to not
                                                                                                 * loose the long
                                                                                                 * startup time we spent 
                                                                                                 */
                }
                else
#endif /* DNX_EMULATION_1_CORE */
                {
                    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, default_mem->mem, MEM_BLOCK_ALL, entry));
                }

                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, default_mem->mem);
                break;
            }

            /** Custom per entry, expected a callback to provide the default value. */
            case dnx_init_mem_default_mode_custom_per_entry:
            {
#ifdef PRINT_MEM_INIT_TIME
                if (soc_sand_is_emulation_system(unit))
                {
                    passed_secs = ((usecs = sal_time_usecs()) - start_usecs) / 1000000;
                    last_time = (usecs - prev_usecs) / 100000;
                    prev_usecs = usecs;
                    LOG_ERROR(BSL_LOG_MODULE, ("%d:%.2d after %d.%ds Custom per entry memory %s\n",
                                               passed_secs / 60, passed_secs % 60, last_time / 10, last_time % 10,
                                               SOC_MEM_NAME(unit, default_mem->mem)));
                }
                else
#endif
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Custom per entry memory # %d - %s\n"), default_mem->mem,
                                 SOC_MEM_NAME(unit, default_mem->mem)));
                /**set data */
                SHR_IF_ERR_EXIT(dnx_init_mem_default_custom_per_entry_set
                                (unit, default_mem->mem, default_mem->default_get_cb));;
                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, default_mem->mem);
                break;
            }

            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", default_mem->mode);
                break;
            }
        }
    }
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYZER_MEM_INIT);

    
    /** Reset the TCAM CS tables */
    SHR_IF_ERR_EXIT(dnx_init_tcam_cs_reset(unit));

    /*
     * Reset the rest of the tables to zero
     */
    SHR_IF_ERR_EXIT(dnx_init_mem_reset(unit, mem_exclude_bitmap));

    /** verify memory defaults */
    if (dnx_data_dev_init.mem.defaults_verify_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_init_mem_reset_verify(unit));
    }

    /** disable dynamic memory access */
    SHR_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, jr2_dynamic_mem_disable_regs, 0));

exit:
    if (mem_exclude_bitmap != NULL)
    {
        sal_free(mem_exclude_bitmap);
    }
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_init_mem_default_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** nothing to do */
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_init_mem_default_mode_get(
    int unit,
    soc_mem_t mem,
    int *is_non_standard)
{
    const dnx_data_dev_init_mem_default_t *default_mem;
    int mem_index;
    const dnxc_data_table_info_t *table_info;

    SHR_FUNC_INIT_VARS(unit);

    (*is_non_standard) = 0;

        /** Get size of table default size */
    table_info = dnx_data_dev_init.mem.default_info_get(unit);

    for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
    {
        default_mem = dnx_data_dev_init.mem.default_get(unit, mem_index);
        if (mem == default_mem->mem)
        {
            /** per mode action */
            switch (default_mem->mode)
            {
                /** Set default value to zero */
                case dnx_init_mem_default_mode_zero:
                case dnx_init_mem_default_mode_none:
                {
                    break;
                }
                /** Set default value to all ones (0xFFFF...) */
                case dnx_init_mem_default_mode_all_ones:
                case dnx_init_mem_default_mode_index:
                case dnx_init_mem_default_mode_custom:
                case dnx_init_mem_default_mode_custom_per_entry:
                {
                    (*is_non_standard) = 1;
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", default_mem->mode);
                    break;
                }
            }

            /** memory found */
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_init_mem_default_get(
    int unit,
    soc_mem_t mem,
    int array_index,
    int copyno,
    int index,
    uint32 *entry,
    dnx_init_mem_default_mode_e *mode)
{
    const dnx_data_dev_init_mem_default_t *default_mem;
    const dnxc_data_table_info_t *table_info;
    int mem_index;
    uint32 field[SOC_MAX_MEM_WORDS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** set entry to zero */
    sal_memset(entry, 0, SOC_MEM_WORDS(unit, mem));

    /** Get size of table default size */
    table_info = dnx_data_dev_init.mem.default_info_get(unit);

    /*
     * Iterate over the table and set value for each mem
     * according to default value mode.
     */
    for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
    {
        default_mem = dnx_data_dev_init.mem.default_get(unit, mem_index);
        if (mem == default_mem->mem)
        {
            if (mode != NULL)
            {
                *mode = default_mem->mode;
            }
            /** per mode action */
            switch (default_mem->mode)
            {
                /** Set default value to zero */
                case dnx_init_mem_default_mode_zero:
                case dnx_init_mem_default_mode_none:
                {
                    /** Do nothing - already zero */
                    break;
                }
                /** Set default value to all ones (0xFFFF...) */
                case dnx_init_mem_default_mode_all_ones:
                {
                    if (default_mem->field == INVALIDf)
                    {
                        sal_memset(entry, 0xFF, SOC_MEM_BYTES(unit, default_mem->mem));
                    }
                    else
                    {
                        sal_memset(field, 0xFF, sizeof(field));
                        soc_mem_field_width_fit_set(unit, default_mem->mem, entry, default_mem->field, field);
                    }
                    break;
                }
                /** Each entry value will be the index. */
                case dnx_init_mem_default_mode_index:
                {
                    if (default_mem->field == INVALIDf)
                    {
                        *entry = index;
                    }
                    else
                    {
                        *field = index;
                        soc_mem_field_width_fit_set(unit, default_mem->mem, entry, default_mem->field, field);
                    }
                    break;
                }
                /** Custom, expected a callback to provide the default value. */
                case dnx_init_mem_default_mode_custom:
                {
                    /*
                     * verify, callback supplied
                     */
                    if (default_mem->default_get_cb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "custom default value requires callback (mem_index %d).\n",
                                     mem_index);
                    }
                    SHR_IF_ERR_EXIT(default_mem->default_get_cb(unit, 0, MEM_BLOCK_ALL, 0, entry));
                    break;
                }

                /** Custom per entry, expected a callback to provide the default value. */
                case dnx_init_mem_default_mode_custom_per_entry:
                {
                    /**set data */
                    SHR_IF_ERR_EXIT(default_mem->default_get_cb(unit, array_index, copyno, index, entry));
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", default_mem->mode);
                    break;
                }
            }

            /** memory found */
            break;
        }
        else
        {
            if (mode != NULL)
            {
                *mode = dnx_init_mem_default_mode_zero;
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}
