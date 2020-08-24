/**
 * \file init_mem.c
 * DNX init memories.
 * - Set memories to default values
 * - Get the default value defined per each memory
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MEM
/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/gport.h>
#include <bcm_int/control.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>

#include <soc/mcm/memregs.h>

#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/dnxc_mem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/sand/sand_mem.h>

#include <soc/memory.h>
#include <soc/cmic.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/mem.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <sal/core/boot.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#define DNX_MEM_BRINGUP_DONE
/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc property functions */

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
const soc_reg_t jr2_dynamic_mem_enable_regs[] = {
    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CDUM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CLU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DCC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DHC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
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
    FASIC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FEU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FLEXEWP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSCL_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * HBC_ENABLE_DYNAMIC_MEMORY_ACCESSr, to be enabled for silicon
     */
    HBMC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
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
const soc_reg_t jr2_dynamic_mem_disable_regs[] = {

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
    /*
     * SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, needed for SCH_TOKEN_MEMORY_CONTROLLER_TMC
     */
    /*
     * SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr, because of SPB_CONTEXT_MRU
     */
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,

    INVALIDr
};

/* registers to enable Q2A dynamic memory writes */
const soc_reg_t q2a_dynamic_mem_enable_regs[] = {
    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CLU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#ifdef BLOCK_NOT_INITIALIZED    /* DCC cannot be initialized at this stage */
    DCC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#endif
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ERPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ESB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EVNT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#ifdef BLOCK_NOT_INITIALIZED    /* FlexE blocks cannot be initialized at this stage */
    FASIC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FLEXEWP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSCL_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#endif
    FEU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
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
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TCAM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/* registers to disable Q2A dynamic memory writes */
const soc_reg_t q2a_dynamic_mem_disable_regs[] = {
    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CLU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#ifdef BLOCK_NOT_INITIALIZED    /* DCC cannot be initialized at this stage */
    DCC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#endif
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ERPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ESB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#ifdef BLOCK_NOT_INITIALIZED    /* FlexE blocks cannot be initialized at this stage */
    FASIC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FLEXEWP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSAR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSCL_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#endif
    FEU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
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
    OCB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PEM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,  needed for SCH_TOKEN_MEMORY_CONTROLLER_TMC
     */
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TCAM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/* registers to enable J2C dynamic memory writes */
const soc_reg_t j2c_dynamic_mem_enable_regs[] = {
    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CLU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ERPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ESB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EVNT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#ifdef HBC_POWERED 
    HBC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    HBMC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
#endif
    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
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

/* registers to disable J2C dynamic memory writes */
const soc_reg_t j2c_dynamic_mem_disable_regs[] = {
    CDU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CLU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DDHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    DQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECGM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ERPP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ESB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ETPPC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FQP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    /*
     * for HBC and HBMC blocks, dyamic memory access shouldn't be disabled, it is needed for phy access
     * HBC_ENABLE_DYNAMIC_MEMORY_ACCESSr, HBMC_ENABLE_DYNAMIC_MEMORY_ACCESSr, 
     */
    ILE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
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
    /*
     * SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,  needed for SCH_TOKEN_MEMORY_CONTROLLER_TMC
     */
    SPB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    TCAM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};

/**
 * \brief - Maximum number of indirect operations required to initialized the memory of a block.
 */
#define DNX_INIT_MEM_INDIRECT_CMDS_PER_BLOCK_MAX 200

/**
 * \brief - structure that holds the required info to perform indirect memory initialization.
 */
typedef struct
{
    int blk_id;

    soc_mem_t mem;
    int array_index;
    int entry_index;

    int nof_entries;
} dnx_init_mem_indirect_cmd_s;

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

    /** get mem info about table */
    blk_index_min = dnx_drv_soc_mem_block_min(unit, mem);
    blk_index_max = dnx_drv_soc_mem_block_max(unit, mem);
    index_min = dnx_drv_soc_mem_index_min(unit, mem);
    index_max = dnx_drv_soc_mem_index_max(unit, mem);

    if (dnx_drv_soc_mem_is_array(unit, mem))
    {
        array_index_max = (dnx_drv_soc_mem_numels(unit, mem) - 1) + dnx_drv_soc_mem_first_array_index(unit, mem);
        array_index_min = dnx_drv_soc_mem_first_array_index(unit, mem);
    }

    for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
    {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            for (index = index_min; index <= index_max; index++)
            {
                mem_addr = soc_mem_addr_get(unit, mem, array_index, blk_index, index, &at);

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMANDr(unit, _SHR_CORE_ALL, 0xE00003));

                reg_val = 0;
                soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
                soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_ADDRESSr(unit, _SHR_CORE_ALL, reg_val));

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_WR_MASKr(unit, _SHR_CORE_ALL, 0xFF));

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_WR_DATAr(unit, _SHR_CORE_ALL, reg_val_zero));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_init_mem_reset_exclude_list(
    int unit,
    soc_mem_t mem)
{
    soc_block_type_t block;
    SHR_FUNC_INIT_VARS(unit);
    block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));
    if (block == SOC_BLK_FSCL)
    {
        return 0;
    }
    switch (mem)
    {   /* skip failing memories */
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
        case DDHB_MACRO_2_ABK_BANK_Am:
        case DDHB_MACRO_2_ABK_BANK_Bm:
        case DDHB_MACRO_2_ENTRY_BANKm:
        case DDHB_MACRO_3_ABK_BANK_Am:
        case DDHB_MACRO_3_ABK_BANK_Bm:
        case DDHB_MACRO_3_ENTRY_BANKm:
        case DDHA_MACRO_2_ABK_BANK_Am:
        case DDHA_MACRO_2_ABK_BANK_Bm:
        case DDHA_MACRO_2_ENTRY_BANKm:
        case DDHA_MACRO_3_ABK_BANK_Am:
        case DDHA_MACRO_3_ABK_BANK_Bm:
        case DDHA_MACRO_3_ENTRY_BANKm:
        case DHC_MACRO_ABK_BANK_Am:
        case DHC_MACRO_ABK_BANK_Bm:
        case DHC_MACRO_ENTRY_BANKm:
            /*
             * Should not be initilized 
             */
        case DCC_DRAM_CPU_ACCESSm:
            /*
             * Should not be initilized - Ofer Landau 
             */
        case OAMP_RXP_R_MEP_INDEX_FIFOm:
            /*
             * FLEXE memories are being initialized in a later stage 
             */
        case FASIC_BRCM_RAM_S_W_R_60_80_R_2_ECC_RBUSm:
        case FASIC_BRCM_RAM_S_W_R_70_6400_R_2_ECC_RBUSm:
        case FLEXEWP_TINY_MAC_MEMORIESm:
        case FLEXEWP_TINY_MAC_REGISTERSm:
        case FSAR_BRCM_RAM_S_W_R_60_80_R_2_ECC_RBUSm:
        case FSCL_FLEXE_CORE_REGISTERSm:
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
            return 0;
        case MDB_EEDB_ENTRY_BANKm:
            if (dnx_data_mdb.hw_bug.feature_get(unit, dnx_data_mdb_hw_bug_eedb_bank_traffic_lock))
            {
                return 0;
            }
            else
            {
                return 1;
            }
        default:
            return 1;
    }
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Reset all valid writeable memories which are not included in mem_exclude_bitmap using SLAM DMA.
 *
 * \param [in] unit - unit #
 * \param [in] mem_exclude_bitmap - Each bit represent the memories that should be excluded (will be skipped)
 * \param [in] reset_value - Init value (only LSB) - used for testing - should be zero for standart initialziation.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_init_mem_dma_reset(
    int unit,
    uint32 *mem_exclude_bitmap,
    uint32 reset_value)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** set reset value */
    entry[0] = reset_value;

    /*
     * Set the rest of the tables to zero
     */
    /** iterate over all memories */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        /** filter invalid memories*/
        if (!dnx_drv_soc_mem_is_valid(unit, mem))
        {
            continue;
        }

         /** filter read only memories */
        if ((dnx_drv_soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
        {
            continue;
        }

         /** filter aliased memories - to avoid duplication */
        if (SOC_MEM_IS_ALIAS(unit, mem))
        {
            continue;
        }

         /** filter mems included in specified defaults */
        if (SHR_BITGET(mem_exclude_bitmap, mem))
        {
            continue;
        }
        if (dnx_init_mem_reset_exclude_list(unit, mem) == 0)
        {
            continue;
        }
        /*
         * The ECGM and CFC memories are filled separately
         */

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Zeros memory # %d - %s %s%s\n", mem, SOC_MEM_NAME(unit, mem), EMPTY, EMPTY);
        DNX_INIT_TIME_ANALYZER_ACCESS_START(unit, DNX_INIT_TIME_ANALYZER_MEM_ZEROS);
#ifndef DNX_MEM_BRINGUP_DONE
        SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem, dnx_drv_mem_block_all(), entry));
#else
        SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem, dnx_drv_mem_block_all(), entry));
#endif
        DNX_INIT_TIME_ANALYZER_ACCESS_STOP(unit, DNX_INIT_TIME_ANALYZER_MEM_ZEROS);

    }

    SHR_IF_ERR_EXIT(dnx_init_mem_tcam_hit_indication_reset(unit, TCAM_TCAM_ACTION_HIT_INDICATIONm));
    SHR_IF_ERR_EXIT(dnx_init_mem_tcam_hit_indication_reset(unit, TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - Reset all valid writeable memories which are not included in mem_exclude_bitmap using indirect commands.
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
dnx_init_mem_indirect_reset(
    int unit,
    uint32 *mem_exclude_bitmap)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    dnx_init_mem_indirect_cmd_s *cmds[SOC_MAX_NUM_BLKS];
    int nof_cmds[SOC_MAX_NUM_BLKS];
    int blk_index_min, blk_index_max;
    int index_min, index_max;
    int array_index_min, array_index_max;
    int blk_index, array_index;
    int cmd_id;
    int index;
    SHR_FUNC_INIT_VARS(unit);

    /** init commands data structure */
    sal_memset(cmds, 0, sizeof(cmds));
    sal_memset(nof_cmds, 0, sizeof(nof_cmds));
    for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++)
    {
        SHR_ALLOC_SET_ZERO(cmds[blk_index],
                           DNX_INIT_MEM_INDIRECT_CMDS_PER_BLOCK_MAX * sizeof(dnx_init_mem_indirect_cmd_s), "cmds",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }

    /*
     * Set the rest of the tables to zero
     */
    /** iterate over all memories */
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        /** filter invalid memories*/
        if (!dnx_drv_soc_mem_is_valid(unit, mem))
        {
            continue;
        }

         /** filter read only memories */
        if ((dnx_drv_soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
        {
            continue;
        }

         /** filter aliased memories - to avoid duplication */
        if (SOC_MEM_IS_ALIAS(unit, mem))
        {
            continue;
        }

         /** filter mems included in specified defaults */
        if (SHR_BITGET(mem_exclude_bitmap, mem))
        {
            continue;
        }

        /** filter special memories - should be moved to another function */
        if (dnx_init_mem_reset_exclude_list(unit, mem) == 0)
        {
            continue;
        }

        /** get info about table */
        blk_index_min = dnx_drv_soc_mem_block_min(unit, mem);
        blk_index_max = dnx_drv_soc_mem_block_max(unit, mem);
        index_min = dnx_drv_soc_mem_index_min(unit, mem);
        index_max = dnx_drv_soc_mem_index_max(unit, mem);
        if (dnx_drv_soc_mem_is_array(unit, mem))
        {
            array_index_max = (dnx_drv_soc_mem_numels(unit, mem) - 1) + dnx_drv_soc_mem_first_array_index(unit, mem);
            array_index_min = dnx_drv_soc_mem_first_array_index(unit, mem);
        }
        else
        {
            array_index_min = 0;
            array_index_max = 0;
        }
        /*
         * Just making sure...
         */
        if (blk_index_min >= (sizeof(cmds) / sizeof(cmds[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "blk_index_min (%d) is larger than number of blocks (%d). Quit. \n",
                         blk_index_min, (int) (sizeof(cmds) / sizeof(cmds[0])));
        }
        if (blk_index_max >= (sizeof(cmds) / sizeof(cmds[0])))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "blk_index_max (%d) is larger than number of blocks (%d). Quit. \n",
                         blk_index_max, (int) (sizeof(cmds) / sizeof(cmds[0])));
        }

       /** add memory commands to database */
        for (blk_index = blk_index_min; blk_index <= blk_index_max; blk_index++)
        {
            for (array_index = array_index_min; array_index <= array_index_max; array_index++)
            {
                for (index = index_min; index <= index_max; index += 16 * 1024 - 1)
                {
                    assert(blk_index < SOC_MAX_NUM_BLKS);

                    cmd_id = nof_cmds[blk_index];
                    assert(cmd_id < DNX_INIT_MEM_INDIRECT_CMDS_PER_BLOCK_MAX);

                    cmds[blk_index][cmd_id].mem = mem;
                    cmds[blk_index][cmd_id].blk_id = blk_index;
                    cmds[blk_index][cmd_id].array_index = array_index;
                    cmds[blk_index][cmd_id].entry_index = index;
                    cmds[blk_index][cmd_id].nof_entries = UTILEX_MIN(index_max - index + 1, 16 * 1024 - 1);

                    nof_cmds[blk_index]++;
                }
            }
        }
    }

    /** perform all commands  */
    for (cmd_id = 0; cmd_id < DNX_INIT_MEM_INDIRECT_CMDS_PER_BLOCK_MAX; cmd_id++)
    {
        for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++)
        {
            if (nof_cmds[blk_index] > cmd_id)
            {
                SHR_IF_ERR_EXIT(sand_mem_indirect_reset_write
                                (unit, cmds[blk_index][cmd_id].mem, cmds[blk_index][cmd_id].blk_id,
                                 cmds[blk_index][cmd_id].array_index, cmds[blk_index][cmd_id].entry_index,
                                 cmds[blk_index][cmd_id].nof_entries, entry));
            }
        }
    }

    /** verify all operations done */
    for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++)
    {
        if (nof_cmds[blk_index] > 0)
        {
            SHR_IF_ERR_EXIT(sand_mem_indirect_poll_done(unit, cmds[blk_index][0].blk_id, 10000));
        }
    }

exit:
    for (blk_index = 0; blk_index < SOC_MAX_NUM_BLKS; blk_index++)
    {
        SHR_FREE(cmds[blk_index]);
    }
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
    int blk_index, array_index, index, found_error = 0, found_error_in_table = 0;
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
        if (!dnx_drv_soc_mem_is_valid(unit, mem))
        {
            continue;
        }

         /** filter read only memories */
        if ((dnx_drv_soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
        {
            continue;
        }

        /** filter write only memories */
        if ((dnx_drv_soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0)
        {
            continue;
        }

       /** filter signal memories */
        if ((dnx_drv_soc_mem_flags(unit, mem) & SOC_MEM_FLAG_SIGNAL) != 0)
        {
            continue;
        }

         /** filter aliased memories - to avoid duplication */
        if (SOC_MEM_IS_ALIAS(unit, mem))
        {
            continue;
        }
        if (dnx_init_mem_reset_exclude_list(unit, mem) == 0)
        {
            continue;
        }

        /** get info about table */
        blk_index_min = dnx_drv_soc_mem_block_min(unit, mem);
        blk_index_max = dnx_drv_soc_mem_block_max(unit, mem);

        index_min = dnx_drv_soc_mem_index_min(unit, mem);
        index_max = dnx_drv_soc_mem_index_max(unit, mem);

        if (dnx_drv_soc_mem_is_array(unit, mem))
        {
            array_index_max = (dnx_drv_soc_mem_numels(unit, mem) - 1) + dnx_drv_soc_mem_first_array_index(unit, mem);
            array_index_min = dnx_drv_soc_mem_first_array_index(unit, mem);
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

                    SHR_IF_ERR_EXIT(dnx_init_mem_default_get
                                    (unit, mem, array_index, blk_index, index, entry_def, &mode));
                    if (mode == dnx_init_mem_default_mode_none)
                    {
                        break;
                    }
                    /*
                     * soc_mem_parity_field_clear(unit,mem, entry_def, entry_def);
                     */
                    SHR_IF_ERR_EXIT(utilex_bitstream_and(entry_def, entry_mask, dnx_drv_soc_mem_words(unit, mem)));

                    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, mem, array_index, blk_index, index, entry_read));
                    /*
                     * soc_mem_parity_field_clear(unit,mem, entry_read, entry_read);
                     */
                    SHR_IF_ERR_EXIT(utilex_bitstream_and(entry_read, entry_mask, dnx_drv_soc_mem_words(unit, mem)));

                    if (sal_memcmp(entry_def, entry_read, dnx_drv_soc_mem_bytes(unit, mem)) != 0)
                    {
                        LOG_ERROR(BSL_LOG_MODULE,
                                  ("Error: Memory default verify failed in memory %s (blk_index=%d, array_index=%d, index=%d)\n",
                                   SOC_MEM_NAME(unit, mem), blk_index, array_index, index));
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
                    (unit, 0, (void **) &entries, dnx_drv_soc_mem_table_bytes(unit, mem),
                     "dnxc_fill_table_with_index_val"));

    /** get info about table */
    blk_index_min = dnx_drv_soc_mem_block_min(unit, mem);
    blk_index_max = dnx_drv_soc_mem_block_max(unit, mem);

    index_min = dnx_drv_soc_mem_index_min(unit, mem);
    index_max = dnx_drv_soc_mem_index_max(unit, mem);

    entry_words = dnx_drv_soc_mem_entry_words(unit, mem);

    if (dnx_drv_soc_mem_is_array(unit, mem))
    {
        array_index_max = (dnx_drv_soc_mem_numels(unit, mem) - 1) + dnx_drv_soc_mem_first_array_index(unit, mem);
        array_index_min = dnx_drv_soc_mem_first_array_index(unit, mem);
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
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LOAD_BALANCING_MPLS_1ST_STACK_FORCE_LABELS_HL_TCAM));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LOAD_BALANCING_MPLS_2ND_STACK_FORCE_LABELS_HL_TCAM));

exit:
    SHR_FUNC_EXIT;
}

#ifndef DNX_MEM_BRINGUP_DONE
/*
 * initialize memory zeroing exclusion to the given array.
 */
void
soc_sand_tbls_zero_init(
    int unit,
    soc_mem_t * mem_exclude_list,
    SHR_BITDCL * mems_bitmap)
{
    soc_mem_t *excluded_list = mem_exclude_list;        /* excluded memory list iterator */

    /*
     * Set exclusion bitmap to zero. Bits of manually filled memories will be later set to 1. 
     */
    sal_memset(mems_bitmap, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));

    /*
     * Add excluded memories to the exclusion bitmap 
     */
    for (; *excluded_list != INVALIDm; ++excluded_list)
    {   /* iterate on the excluded memories */
        SHR_BITSET(mems_bitmap, *excluded_list);        /* set the bits of excluded memories in the bitmap */
    }
}

#define SAND_IN_BRINGUP
/*
 * initialize all tables to zero, except for exception array, and marked not to be zeroed.
 */
shr_error_e
soc_sand_tbls_zero(
    int unit,
    SHR_BITDCL * mems_bitmap)
{
    int mem_iter = 0;
    uint32 entry0[128] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Zero tables if not running in emulation/simulation 
     */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_tbls_to_zero", !(
#ifdef PLISIM
                                                                                            SAL_BOOT_PLISIM ||
                          /** not pcid and not emulation */
#endif
                                                                                            soc_sand_is_emulation_system
                                                                                            (unit))))
    {

        /*
         * Zero all the memories that need it. Memories should not read only, not aliases, not filled earlier... 
         */
        for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++)
        {       /* iterate over all memories */
            if (dnx_drv_soc_mem_is_valid(unit, mem_iter) &&     /* Memory must be valid for the device */
                (dnx_drv_soc_mem_flags(unit, mem_iter) & SOC_MEM_FLAG_READONLY) == 0 && /* should not be read-only
                                                                                         * (dynamic * memories are
                                                                                         * zeroed) */
                /*
                 * memory must not be an alias, to avoid multiple resets of the same memory 
                 */
                !(SOC_MEM_IS_ALIAS(unit, mem_iter)) && !SHR_BITGET(mems_bitmap, mem_iter))
            {   /* if the mem is not in excluded bitmap */

                /*
                 * reset memory - set all values to 0 
                 */
                LOG_VERBOSE(BSL_LS_SOC_INIT,
                            (BSL_META_U(unit, "Reseting memory # %d - %s\n"), mem_iter, SOC_MEM_NAME(unit, mem_iter)));

#ifndef SAND_IN_BRINGUP
                SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem_iter, dnx_drv_mem_block_all(), entry0));
#else
                /*
                 * on startup report memory filling errors instead of stopping 
                 */
                if (sand_fill_table_with_entry(unit, mem_iter, dnx_drv_mem_block_all(), entry0) != _SHR_E_NONE)
                {
                    LOG_ERROR(BSL_LS_SOC_INIT,
                              (BSL_META_U(unit, "Failed filling memory # %d - %s\n"), mem_iter,
                               SOC_MEM_NAME(unit, mem_iter)));
                }
#endif
            }
        }
    }

#ifndef SAND_IN_BRINGUP
exit:
#endif
    SHR_FUNC_EXIT;
}

#if 0
/*
 * Marks the table not to later be filled by soc_jer_tbls_zero.
 */
void
soc_sand_mark_memory(
    const int unit,
    const soc_mem_t mem,        /* memory/table to mark */
    SHR_BITDCL * mems_bitmap)
{
    /*
     * mark the memory not to be later zeroed 
     */
    soc_mem_t mem_alias = mem;
    SOC_MEM_ALIAS_TO_ORIG(unit, mem_alias);
    SHR_BITSET(mems_bitmap, mem_alias);
}

/*
 * Fill the whole table with the given entry, uses fast DMA filling when run on real hardware.
 * Marks the table not to later be filled by soc_sand_tbls_zero.
 */
shr_error_e
soc_sand_fill_and_mark_memory(
    const int unit,
    const soc_mem_t mem,        /* memory/table to fill */
    const void *entry_data,     /* The contents of the entry to fill the table with. Does not have to be DMA memory */
    SHR_BITDCL * mems_bitmap)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem, dnx_drv_mem_block_all(), entry_data));
    soc_sand_mark_memory(unit, mem, mems_bitmap);

exit:
    SHR_FUNC_EXIT;
}
#endif /* 0 */

/*
 * Function:
 *      soc_jr2_tbls_init
 * Purpose:
 *      initialize all tables relevant for JR2.
 * Parameters:
 *      unit -  unit number
 *  
 * Returns:
 *      SOC_E_XXX
 */
shr_error_e
soc_jr2_tbls_init(
    int unit)
{
    /*
     * define bitmap for memory exclusions 
     */
    SHR_BITDCL mems_bitmap[_SHR_BITDCLSIZE(NUM_SOC_MEM)];
    soc_mem_t jr2_tbls_excluded_mem_list[] = {  /* memories which should be skipped when zeroing all memories */
        SCH_SCHEDULER_INITm,    /* not a real memory, handled separately */
        /*
         * The HBM memories are not real memories
         */
        BRDC_HBC_HBM_PHY_CH_REGISTER_ACCESSm,
        BRDC_HBC_HBM_DRAM_CPU_ACCESSm,
        BRDC_FSRD_FSRD_PROM_MEMm,
        HBC_HBM_DRAM_CPU_ACCESSm,
        HBC_HBM_PHY_CH_REGISTER_ACCESSm,
        HBMC_HBM_PHY_CHM_REGISTER_ACCESSm,

        /*
         * The GDDR6 PHY register access memory is not a real memory, this is added here to a jr2 function due to the insistence that
         * this old out-dated mechanism remains for future BUs, although a perfectly better mechanism exists via the DNXDATA, in that
         * mechanism, this memory is already excluded, however it has to be excluded here as well so the init sequence doesn't clear it.
         * in essence the PHY registers - even though represented by a memory - should retain there default values after reset and not
         * overridden by zeroes like other real memories.
         */
        DPC_DRAM_PHY_REGISTER_ACCESSm,

        /*
         * The CDU instrumentation and rx/tx memories do not have to be initialized and can not be written by
         * DMA
         */
        CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm,
        CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm,
        CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm,
        CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm,
        CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm,
        CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm,
        CDU_RX_MEMA_CTRLm,
        CDU_RX_MEMB_CTRLm,
        CDU_RX_MEMC_CTRLm,
        CDU_RX_MEMD_CTRLm,
        CDU_RX_MEME_CTRLm,
        CDU_RX_MEMF_CTRLm,
        CDU_RX_MEMG_CTRLm,
        CDU_RX_MEMH_CTRLm,
        CDU_RX_MEMI_CTRLm,
        CDU_RX_MEMJ_CTRLm,
        CDU_RX_MEMK_CTRLm,
        CDU_RX_MEML_CTRLm,
        CDU_RX_MEMM_CTRLm,
        CDU_RX_MEMN_CTRLm,
        CDU_RX_MEMO_CTRLm,
        CDU_RX_MEMP_CTRLm,
        CDU_TX_MEMA_CTRLm,
        CDU_TX_MEMB_CTRLm,
        CDU_TX_MEMC_CTRLm,
        CDU_TX_MEMD_CTRLm,
        CDU_TX_MEME_CTRLm,
        CDU_TX_MEMF_CTRLm,
        CDU_TX_MEMG_CTRLm,
        CDU_TX_MEMH_CTRLm,
        CDU_TX_MEMI_CTRLm,
        CDU_TX_MEMJ_CTRLm,

        /*
         * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization
         */
        DDHB_MACRO_0_ABK_BANK_Am,
        DDHB_MACRO_0_ABK_BANK_Bm,
        DDHB_MACRO_0_ENTRY_BANKm,
        DDHB_MACRO_1_ABK_BANK_Am,
        DDHB_MACRO_1_ABK_BANK_Bm,
        DDHB_MACRO_1_ENTRY_BANKm,
        DDHA_MACRO_0_ABK_BANK_Am,
        DDHA_MACRO_0_ABK_BANK_Bm,
        DDHA_MACRO_0_ENTRY_BANKm,
        DDHA_MACRO_1_ABK_BANK_Am,
        DDHA_MACRO_1_ABK_BANK_Bm,
        DDHA_MACRO_1_ENTRY_BANKm,
        DDHB_MACRO_2_ABK_BANK_Am,
        DDHB_MACRO_2_ABK_BANK_Bm,
        DDHB_MACRO_2_ENTRY_BANKm,
        DDHB_MACRO_3_ABK_BANK_Am,
        DDHB_MACRO_3_ABK_BANK_Bm,
        DDHB_MACRO_3_ENTRY_BANKm,
        DDHA_MACRO_2_ABK_BANK_Am,
        DDHA_MACRO_2_ABK_BANK_Bm,
        DDHA_MACRO_2_ENTRY_BANKm,
        DDHA_MACRO_3_ABK_BANK_Am,
        DDHA_MACRO_3_ABK_BANK_Bm,
        DDHA_MACRO_3_ENTRY_BANKm,
        DHC_MACRO_ABK_BANK_Am,
        DHC_MACRO_ABK_BANK_Bm,
        DHC_MACRO_ENTRY_BANKm,
        /*
         * Should not be initilized 
         */
        DCC_DRAM_CPU_ACCESSm,
        /*
         * Should not be initilized - Ofer Landau 
         */
        OAMP_RXP_R_MEP_INDEX_FIFOm,
        /*
         * FLEXE memories are being initialized in a later stage 
         */
        FASIC_BRCM_RAM_S_W_R_60_80_R_2_ECC_RBUSm,
        FASIC_BRCM_RAM_S_W_R_70_6400_R_2_ECC_RBUSm,
        FLEXEWP_TINY_MAC_MEMORIESm,
        FLEXEWP_TINY_MAC_REGISTERSm,
        FSAR_BRCM_RAM_S_W_R_60_80_R_2_ECC_RBUSm,
        FSCL_FLEXE_CORE_REGISTERSm,

        /*
         * These are address spaces saved for ilkn memory mapped registers.
         */
        ILE_PORT_0_CPU_ACCESSm,
        ILE_PORT_1_CPU_ACCESSm,

        /*
         * All below are either not real memories or don't have protection. - Mosi Ravia
         */
        KAPS_RPB_TCAM_CPU_COMMANDm,
        KAPS_TCAM_ECC_MEMORYm,
        MDB_ARM_KAPS_TCMm,
        MDB_ARM_MEM_0m,
        MDB_ARM_MEM_10000m,
        MDB_ARM_MEM_30000m,
        MACT_CPU_REQUESTm,
        ECI_SAM_CTRLm,

        SQM_DEQ_QSTATE_PENDING_FIFOm,
        SQM_ENQ_COMMAND_RXIm,
        SQM_PDB_LINK_LISTm,
        SQM_PDMm,
        SQM_QUEUE_DATA_MEMORYm,
        SQM_TX_BUNDLE_MEMORYm,
        SQM_TX_PACKET_DESCRIPTOR_FIFOS_MEMORYm,

        /*
         * These memories are not real
         */
        ERPP_MEM_F000000m,
        ETPPA_MEM_F000000m,
        ETPPB_MEM_F000000m,
        ETPPC_MEM_F000000m,
        IPPA_MEM_F000000m,
        IPPB_MEM_F000000m,
        IPPC_MEM_F000000m,
        IPPD_MEM_F000000m,
        IPPE_MEM_B900000m,
        IPPF_MEM_F000000m,
        ERPP_EPMFCS_TCAM_HIT_INDICATIONm,
        IPPA_VTDCS_TCAM_HIT_INDICATIONm,
        IPPA_VTECS_TCAM_HIT_INDICATIONm,
        IPPB_FLPACS_TCAM_HIT_INDICATIONm,
        IPPB_FLPBCS_TCAM_HIT_INDICATIONm,
        IPPC_PMFACSA_TCAM_HIT_INDICATIONm,
        IPPC_PMFACSB_TCAM_HIT_INDICATIONm,
        IPPD_PMFBCS_TCAM_HIT_INDICATIONm,
        IPPE_PRTCAM_TCAM_HIT_INDICATIONm,
        IPPE_LLRCS_TCAM_HIT_INDICATIONm,
        IPPF_VTACS_TCAM_HIT_INDICATIONm,
        IPPF_VTCCS_TCAM_HIT_INDICATIONm,
        IPPF_VTBCS_TCAM_HIT_INDICATIONm,
        KAPS_RPB_TCAM_HIT_INDICATIONm,
        KAPS_TCAM_HIT_INDICATIONm,

        /*
         * The ECGM memories are filled separately?
         */
        ECGM_FDCMm,
        ECGM_FDCMAXm,
        ECGM_FQSMm,
        ECGM_FQSMAXm,
        ECGM_PDCMm,
        ECGM_PDCMAXm,
        ECGM_PQSMm,
        ECGM_PQSMAXm,
        ECGM_QDCMm,
        ECGM_QDCMAXm,
        ECGM_QQSMm,
        ECGM_QQSMAXm,

        INVALIDm        /* Has to be last memory in array */
    };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * initialize memory zeroing exclusion to the given array. 
     */
    soc_sand_tbls_zero_init(unit, jr2_tbls_excluded_mem_list, mems_bitmap);

    SHR_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, SOC_IS_Q2A(unit) ? q2a_dynamic_mem_enable_regs : (SOC_IS_J2C(unit) ? j2c_dynamic_mem_enable_regs : jr2_dynamic_mem_enable_regs), 1));     /* enable 
                                                                                                                                                                                                         * dynamic
                                                                                                                                                                                                         * * memory 
                                                                                                                                                                                                         * writes */

#if 0
    /*
     * Init tables having none zero values 
     */

    table_entry[0] = 0; /* we want the entry to be disabled (all 1s queue) and have a traffic class profile of 0 */
    soc_mem_field32_set(unit, TAR_DESTINATION_TABLEm, table_entry, QUEUE_NUMBERf, ARAD_IPQ_DESTINATION_ID_INVALID_QUEUE(unit)); /* mark 
                                                                                                                                 * as 
                                                                                                                                 * disabled 
                                                                                                                                 * entry 
                                                                                                                                 */
    SHR_IF_ERR_EXIT(soc_sand_fill_and_mark_memory(unit, TAR_DESTINATION_TABLEm, table_entry, mems_bitmap));     /* fill 
                                                                                                                 * table 
                                                                                                                 * with 
                                                                                                                 * the
                                                                                                                 * entry 
                                                                                                                 */
#endif /* 0 */

    /*
     * initialize all tables to zero, except for exception array, and marked not to be zeroed.  
     */
    SHR_IF_ERR_EXIT(soc_sand_tbls_zero(unit, mems_bitmap));

    SHR_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes
                    (unit,
                     SOC_IS_Q2A(unit) ? q2a_dynamic_mem_disable_regs : (SOC_IS_J2C(unit) ? j2c_dynamic_mem_disable_regs
                                                                        : jr2_dynamic_mem_disable_regs), 0));
                                                                        /** disable dynamic memory writes */

exit:
    SHR_FUNC_EXIT;
}
#endif /* DNX_MEM_BRINGUP_DONE */

/*
 * See .h file
 */
shr_error_e
dnx_init_mem_default_init(
    int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 all_ones[SOC_MAX_MEM_WORDS], *entry_to_use;
    uint32 *mem_exclude_bitmap = NULL;
    const dnx_data_dev_init_mem_default_t *default_mem;
    const dnxc_data_table_info_t *table_info;
    dnx_init_mem_reset_mode_e reset_mode;
    int mem_index;
#ifdef PRINT_MEM_INIT_TIME
    sal_usecs_t start_usecs = sal_time_usecs(), prev_usecs = start_usecs, usecs, passed_secs, last_time;
#endif
    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_EMULATION_1_CORE
    if ((SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)) && !dnx_data_dev_init.mem.force_zeros_get(unit)
        && soc_is(unit, JERICHO2_ONLY_DEVICE))
    {   /* If we rely on memories to be externally zeroed before the run, check for it */
        uint32 entry = 0xffffffff;
        SHR_IF_ERR_EXIT(READ_CGM_REP_CMD_FIFO_OCCUPANCY_RJCT_THm(unit, dnx_drv_mem_block_any(), 0, &entry));
        if (entry != 0)
        {
            SHR_EXIT_WITH_LOG(_SHR_E_MEMORY,
                              "This run expects all memories to be zeroed before the run, and they were not%s%s%s",
                              EMPTY, EMPTY, EMPTY);
        }
    }
#endif /* DNX_EMULATION_1_CORE */

#ifndef DNX_MEM_BRINGUP_DONE
    SHR_IF_ERR_EXIT(soc_jr2_tbls_init(unit));
#endif /* DNX_MEM_BRINGUP_DONE */

    /** Allocate exclude bitmap */
    SHR_ALLOC_SET_ZERO(mem_exclude_bitmap, SHR_BITALLOCSIZE(NUM_SOC_MEM), "reset memories exclude bitmap", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);

    /** Get size of table default size */
    table_info = dnx_data_dev_init.mem.default_info_get(unit);

    /** enable dynamic memory access */
    SHR_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes
                    (unit, SOC_IS_Q2A(unit) ? q2a_dynamic_mem_enable_regs : jr2_dynamic_mem_enable_regs, 1));

    /** to verify the initialization - first set all memories using DMA to value nont zero. value */
    if (dnx_data_dev_init.mem.defaults_verify_get(unit) && !SAL_BOOT_PLISIM)
    {
        SHR_IF_ERR_EXIT(dnx_init_mem_dma_reset(unit, mem_exclude_bitmap, 1));
    }

    /*
     * Iterate over the table and set value for each mem
     * according to default value mode.
     */
    DNX_INIT_TIME_ANALYZER_ACCESS_START(unit, DNX_INIT_TIME_ANALYZER_MEM_INIT);
    for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
    {
        /** Init vars */
        sal_memset(entry, 0x0, sizeof(entry));
        sal_memset(all_ones, 0xff, sizeof(all_ones));
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
                    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, default_mem->mem, dnx_drv_mem_block_all(), entry));
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
                    /*
                     * fill with an entry containing all 1s 
                     */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Allones memory # %d - %s\n"), default_mem->mem,
                                 SOC_MEM_NAME(unit, default_mem->mem)));
                    entry_to_use = all_ones;
                }
                else
                {
                    /*
                     * fill with an entry zero, except for one field containing all 1s 
                     */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Allones memory # %d - %s, field # %d - %s\n"), default_mem->mem,
                                 SOC_MEM_NAME(unit, default_mem->mem), default_mem->field, SOC_FIELD_NAME(unit,
                                                                                                          default_mem->field)));
                    sal_memset(entry, 0x0, sizeof(entry));
                    soc_mem_field_width_fit_set(unit, default_mem->mem, entry, default_mem->field, all_ones);
                    entry_to_use = entry;
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
                    /** in emulation we want to continue in case of memory init failures, to not loose the long startup time we spent we spent */
                    sand_fill_table_with_entry(unit, default_mem->mem, dnx_drv_mem_block_all(), entry_to_use);
                }
                else
#else /* DNX_EMULATION_1_CORE */
                {
#ifdef DNX_MEM_BRINGUP_DONE
                    SHR_IF_ERR_EXIT(sand_fill_table_with_entry
                                    (unit, default_mem->mem, dnx_drv_mem_block_all(), entry_to_use));
#else
                    sand_fill_table_with_entry(unit, default_mem->mem, dnx_drv_mem_block_all(), entry_to_use);
#endif
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
#ifdef DNX_MEM_BRINGUP_DONE
                SHR_IF_ERR_EXIT(dnxc_fill_table_with_index_val(unit, default_mem->mem, default_mem->field));
#else
                dnxc_fill_table_with_index_val(unit, default_mem->mem, default_mem->field);
#endif
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
                SHR_IF_ERR_EXIT(default_mem->default_get_cb(unit, 0, dnx_drv_mem_block_all(), 0, entry));
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
                    sand_fill_table_with_entry(unit, default_mem->mem, dnx_drv_mem_block_all(), entry); /* in emulation 
                                                                                                         * we want * to 
                                                                                                         * continue in
                                                                                                         * case * of
                                                                                                         * memory init
                                                                                                         * * failures,
                                                                                                         * to not *
                                                                                                         * loose the
                                                                                                         * long *
                                                                                                         * startup time 
                                                                                                         * we spent */
                }
                else
#endif /* DNX_EMULATION_1_CORE */
                {
#ifdef DNX_MEM_BRINGUP_DONE
                    SHR_IF_ERR_EXIT(sand_fill_table_with_entry(unit, default_mem->mem, dnx_drv_mem_block_all(), entry));
#else
                    sand_fill_table_with_entry(unit, default_mem->mem, dnx_drv_mem_block_all(), entry);
#endif
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
    DNX_INIT_TIME_ANALYZER_ACCESS_STOP(unit, DNX_INIT_TIME_ANALYZER_MEM_INIT);

    
    /** Reset the TCAM CS tables */
    SHR_IF_ERR_EXIT(dnx_init_tcam_cs_reset(unit));

    /*
     * Reset the rest of the tables to zero
     */
    /** get reset mode from dnx data */
    reset_mode = dnx_data_dev_init.mem.reset_mode_get(unit);

    /*
     * for emulation and simulation - avoid from reset memories - assuming all memories already initilized to zero 
     */
    if ((SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)) && !dnx_data_dev_init.mem.force_zeros_get(unit))
    {
        reset_mode = dnx_init_mem_reset_mode_none;
    }

    switch (reset_mode)
    {
        case dnx_init_mem_reset_mode_indirect:
            SHR_IF_ERR_EXIT(dnx_init_mem_indirect_reset(unit, mem_exclude_bitmap));
            break;
        case dnx_init_mem_reset_mode_dma:
            SHR_IF_ERR_EXIT(dnx_init_mem_dma_reset(unit, mem_exclude_bitmap, 0));
            break;
        case dnx_init_mem_reset_mode_none:
            /*
             * do not perform reset 
             */
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "reset mode not supported %d.\n", reset_mode);
            break;

    }

    /** verify memory defaults */
    if (dnx_data_dev_init.mem.defaults_verify_get(unit))
    {
        SHR_IF_ERR_EXIT(dnx_init_mem_reset_verify(unit));
    }

    /** disable dynamic memory access */
    SHR_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, SOC_IS_Q2A(unit) ?
                                                        q2a_dynamic_mem_disable_regs : (SOC_IS_J2C(unit) ?
                                                                                        j2c_dynamic_mem_disable_regs :
                                                                                        jr2_dynamic_mem_disable_regs),
                                                        0));

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
    dnx_init_mem_default_mode_e * mode)
{
    const dnx_data_dev_init_mem_default_t *default_mem;
    const dnxc_data_table_info_t *table_info;
    int mem_index;
    uint32 field[SOC_MAX_MEM_WORDS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** set entry to zero */
    sal_memset(entry, 0, dnx_drv_soc_mem_words(unit, mem));

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
                        sal_memset(entry, 0xFF, dnx_drv_soc_mem_bytes(unit, default_mem->mem));
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
                    SHR_IF_ERR_EXIT(default_mem->default_get_cb(unit, 0, dnx_drv_mem_block_all(), 0, entry));
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
