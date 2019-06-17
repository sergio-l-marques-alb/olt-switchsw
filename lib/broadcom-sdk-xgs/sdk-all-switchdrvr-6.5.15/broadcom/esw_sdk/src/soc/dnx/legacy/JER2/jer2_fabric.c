/** \file jer2_fabric.c
 *
 * Functions for handling Fabric.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <sal/compiler.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/legacy/fabric.h>
#include <soc/dnx/legacy/JER2/jer2_fabric.h>
#include <soc/dnx/legacy/JER2/jer2_regs.h>
#include <soc/dnx/legacy/SAND/Utils/sand_u64.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <bcm/fabric.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Group context length in bits
 */
#define SOC_JER2_FABRIC_GROUP_CTX_LENGTH         (4)

/*
 * Leaky Bucket configurations
 */
#define SOC_JER2_FABRIC_LINK_FAP_BKT_FILL_RATE  (6)
#define SOC_JER2_FABRIC_LINK_FAP_BKT_UP_LINK_TH (32)
#define SOC_JER2_FABRIC_LINK_FAP_BKT_DN_LINK_TH (16)

/*
 * ALDWP offset as a result of FEs delays along the way.
 * Fap->FE1->FE2->FE3->Fap - each FE has delay of 1.
 */
#define SOC_JER2_FABRIC_ALDWP_FE_DELAY_OFFSET   (3)
/*
 * Min value the ALDWP can have (value inherited from Arad)
 */
#define SOC_JER2_FABRIC_ALDWP_MIN               (0x2)
/*
 * Max value the ALDWP can have (register's field restriction)
 */
#define SOC_JER2_FABRIC_ALDWP_MAX               (0x3f)
/*
 * The ALDWP is written to HW in units of 64 tiks
 */
#define SOC_JER2_FABRIC_ALDWP_RESOLUTION        (64)

/*
 * Number of bits needed for setting minimum number of links per FAP.
 * 112 links requires 7 bits.
 */
#define SOC_JER2_FABRIC_MIN_NOF_LINKS_BITS_NOF  (7)

/*
 * Maximum delay between single cells when "JR1 in system".
 * Value taken from ARCH.
 */
#define SOC_JER2_FABRIC_SINGLE_CELL_IN_FABRIC_RX_MAX_DELAY  (16)

#define SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP (16)

/*
 * Comma Burst configurations
 */
#define SOC_JER2_FABRIC_COMMA_BURST_SIZE         (1)
#define SOC_JER2_FABRIC_COMMA_BURST_PERIOD       (11)
#define SOC_JER2_FABRIC_COMMA_BURST_LLFC_ENABLER (0x7)
#define SOC_JER2_FABRIC_COMMA_BURST_BYTE_MODE    (0x1)

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * See .h file
 */
shr_error_e
soc_jer2_fabric_load_balance_init(int unit)
{
    uint32 reg32_val = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    if (!SOC_DNX_IS_MESH(unit))
    {
        /*
         * Enable switch network, the mechanism that responsible to scramble
         * the output links.
         */
        SHR_IF_ERR_EXIT(READ_FDT_LOAD_BALANCING_CONFIGURATIONr(unit, &reg32_val));
        soc_reg_field_set(unit, FDT_LOAD_BALANCING_CONFIGURATIONr, &reg32_val, ENABLE_SWITCHING_NETWORKf, 1);
        SHR_IF_ERR_EXIT(WRITE_FDT_LOAD_BALANCING_CONFIGURATIONr(unit, reg32_val));

        /*
         * Set LFSR values, needed for switch network.
         */
        SHR_IF_ERR_EXIT(READ_FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr, reg_above_64_val, SWITCHING_NETWORK_SEEDf, sal_time_usecs());
        soc_reg_above_64_field32_set(unit, FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr, reg_above_64_val, SWITCHING_NETWORK_LFSR_CNTf, 1000);
        soc_reg_above_64_field32_set(unit, FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr, reg_above_64_val, SWITCHING_NETWORK_LFSR_CNT_MODEf, 0);
        SHR_IF_ERR_EXIT(WRITE_FDT_LOAD_BALANCING_SWITCH_CONFIGURATIONr(unit, reg_above_64_val));

        /*
         * Set Round-Robin pointer configurations
         */
        SHR_IF_ERR_EXIT(READ_FDT_LB_RR_PTR_CTXr(unit, &reg32_val));

        /** Enable RR pointer for each pipe */
        soc_reg_field_set(unit, FDT_LB_RR_PTR_CTXr, &reg32_val, LB_RR_PTR_CTX_ENf, 1);

        if (dnx_data_fabric.tdm.priority_get(unit) != -1)
        {
            /** Set RR pointer for TDM pipe */
            switch (dnx_data_fabric.pipes.map_get(unit)->type)
            {
                case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
                {
                    soc_reg_field_set(unit, FDT_LB_RR_PTR_CTXr, &reg32_val, LB_RR_PTR_CTX_FIELD_7f, 1);
                    break;
                }
                case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
                {
                    soc_reg_field_set(unit, FDT_LB_RR_PTR_CTXr, &reg32_val, LB_RR_PTR_CTX_FIELD_7f, 2);
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "For TDM, need to configure TDM pipes mapping");
                    break;
                }
            }
        }

        SHR_IF_ERR_EXIT(WRITE_FDT_LB_RR_PTR_CTXr(unit, reg32_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See function declaration
 */
void
soc_jer2_fabric_port_to_fmac(int unit, soc_port_t port, int *fmac_index, int *fmac_inner_link)
{
    *fmac_index = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port) / dnx_data_fabric.blocks.nof_links_in_fmac_get(unit); 
    *fmac_inner_link = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port) % dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
}

/** See .h file */
shr_error_e soc_jer2_fabric_scheduler_adaptation_init(int unit)
{
    int core_i = 0;
    uint32 reg_val32 = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable credit share
     */
    SOC_DNX_CORES_ITER(SOC_CORE_ALL, core_i)
    {
        SHR_IF_ERR_EXIT(READ_SCH_DVS_CREDIT_SHARE_CONFIGURATIONr(unit, core_i, &reg_val32));
        soc_reg_field_set(unit, SCH_DVS_CREDIT_SHARE_CONFIGURATIONr, &reg_val32, ENABLE_DVS_CREDITS_SHAREf, 1);
        SHR_IF_ERR_EXIT(WRITE_SCH_DVS_CREDIT_SHARE_CONFIGURATIONr(unit, core_i, reg_val32));
    }

    /*
     * Configure link state for SCH and FDT.
     * Link state will be a result of link integrity vector masked with
     * accessible links (links that have reachability cells from at least
     * a single device) according to the unicast table.
     */
    SHR_IF_ERR_EXIT(READ_RTP_LINK_STATE_FOR_SC_HAND_FDTr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINK_REDUCTION_MCf, 0);
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_0_PIPEID_Af, 1);
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_0_PIPEID_Bf, 0);
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_1_PIPEID_Af, 0); 
    soc_reg_field_set(unit, RTP_LINK_STATE_FOR_SC_HAND_FDTr, &reg_val32, EN_LOCAL_LINKS_REDUCTION_SCH_1_PIPEID_Bf, 1);
    SHR_IF_ERR_EXIT(WRITE_RTP_LINK_STATE_FOR_SC_HAND_FDTr(unit, reg_val32));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_aldwp_config(int unit)
{
    uint32 max_cell_size = 0;
    uint32 aldwp = 0;
    uint32 highest_aldwp = 0;
    uint32 core_clock_speed_khz = 0;
    soc_port_t port_index = 0;
    uint64 reg64_val = 0;
    int enable = 0, speed, has_speed;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64_val);

    /* 
     * Calc the max number of clock ticks to receive 3 cells:
     *  
     *          3 *  max cell size * core clock
     *  aldwp =   ------------------------------
     *                   link rate
     */

    /*
     * Reterive global configuration
     */
    max_cell_size = dnx_data_fabric.cell.vsc256_max_size_get(unit);
    core_clock_speed_khz = DNXCMN_CORE_CLOCK_KHZ_GET(unit);

    /*
     * Find the highest aldwp
     */
    highest_aldwp = 0;
    for (port_index = SOC_DNX_FABRIC_LINK_TO_PORT(unit, 0); 
            port_index < SOC_DNX_FABRIC_LINK_TO_PORT(unit, dnx_data_fabric.links.nof_links_get(unit)); 
            port_index++)
    {

        /* Skip shut down ports */
        if (!SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port_index))
        {
            continue;
        }

        /* Skip disabled ports */
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, port_index, &enable));
        if (!enable)
        {
            continue;
        }

        /* Skip disabled ports */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port_index, &has_speed));

        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port_index, 0, &speed));
        }
        else
        {
            continue;
        }
        /*
         * Calc the max number of tiks to receive 3 cells.
         * Units calculation: [Bytes] * [KClocks/Sec] / [MBytes/Sec] = [miliClocks]
         */
        aldwp = (3 * max_cell_size * core_clock_speed_khz) / (speed / 8 /*bits to bytes*/); /*mili tiks*/
        /** The devision with 1024 is to convert militiks->tiks units */
        aldwp = aldwp / (1024*SOC_JER2_FABRIC_ALDWP_RESOLUTION) + ((aldwp % (1024*SOC_JER2_FABRIC_ALDWP_RESOLUTION) != 0) ? 1 : 0) /*round up*/; 

        /** Update MAX */
        highest_aldwp = (aldwp > highest_aldwp ? aldwp : highest_aldwp);
    }

    /** +3 to be on the safe side with possible delays from fe2/fe3 */  
    if (highest_aldwp != 0 /** at least one port is enabled */)
    {
        highest_aldwp += SOC_JER2_FABRIC_ALDWP_FE_DELAY_OFFSET;
    }

    /*
     * Check highest_aldwp values
     */
    if (highest_aldwp == 0)
    {
        /** All port are disabled or powered down */
        SHR_EXIT();
    }
    else if (highest_aldwp < SOC_JER2_FABRIC_ALDWP_MIN)
    {
        /** hightest_aldwp may be lower than SOC_JER2_FABRIC_ALDWP_MIN */
        /* coverity[dead_error_line:FALSE] */
        highest_aldwp = SOC_JER2_FABRIC_ALDWP_MIN;
    }
    else if (highest_aldwp > SOC_JER2_FABRIC_ALDWP_MAX)
    {
        highest_aldwp = SOC_JER2_FABRIC_ALDWP_MAX;
    }

    /** Configure relevant register */
    SHR_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r(unit, &reg64_val));
    soc_reg64_field32_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg64_val, AUTO_DOC_NAME_2f, highest_aldwp);
    SHR_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r(unit, reg64_val));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_minimal_links_to_dest_init(int unit)
{
    uint32 enable = 0;
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Minimum number of links feature is not supported in Mesh mode.
     */
    enable = (SOC_DNX_IS_MESH(unit)) ? 0 : 1;

    SHR_IF_ERR_EXIT(READ_FDT_FDT_ENABLER_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit, FDT_FDT_ENABLER_REGISTERr, &reg_val, MIN_NOF_LINKS_ENf, enable);
    SHR_IF_ERR_EXIT(WRITE_FDT_FDT_ENABLER_REGISTERr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_minimal_links_to_dest_set(int unit, soc_module_t module_id, int min_links)
{
    int modid = 0;
    int modid_min = 0;
    int modid_max = 0;
    uint32 core = 0;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    uint32 min_nof_links = min_links;
    SHR_BITDCLNAME(fdt_data, DNX_FABRIC_MESH_MC_TABLE_WIDTH);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set minimum number of links for all modids
     */
    if (module_id == SOC_MODID_ALL)
    {
        modid_min = 0;
        modid_max = dnx_data_device.general.nof_faps_get(unit) - 1;
    }
    /*
     * Set minimum number of links for a specific modid
     */
    else
    {
        modid_min = module_id;
        modid_max = module_id;
    }

    for (core = 0; core < nof_cores; ++core)
    {
        for (modid = modid_min; modid <= modid_max; ++modid)
        {
            SHR_IF_ERR_EXIT(READ_FDT_MESH_MCm(unit, core, MEM_BLOCK_ANY, modid, fdt_data));
            SHR_BITCOPY_RANGE(fdt_data, 0, &min_nof_links, 0, SOC_JER2_FABRIC_MIN_NOF_LINKS_BITS_NOF);
            SHR_IF_ERR_EXIT(WRITE_FDT_MESH_MCm(unit, core, MEM_BLOCK_ALL, modid, fdt_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_minimal_links_to_dest_get(int unit, soc_module_t module_id, int *min_links)
{
    int modid = 0;
    uint32 min_nof_links = 0;
    SHR_BITDCLNAME(fdt_data, DNX_FABRIC_MESH_MC_TABLE_WIDTH);
    SHR_FUNC_INIT_VARS(unit);

    modid = (module_id == SOC_MODID_ALL) ? 0 : module_id;
    SHR_IF_ERR_EXIT(READ_FDT_MESH_MCm(unit, 0, MEM_BLOCK_ANY, modid, fdt_data));
    SHR_BITCOPY_RANGE(&min_nof_links, 0, fdt_data, 0, SOC_JER2_FABRIC_MIN_NOF_LINKS_BITS_NOF);

    *min_links = min_nof_links;

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_minimal_links_all_reachable_set(int unit, int min_links)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, &reg_val, ALRC_MIN_NUM_OF_LINKSf, min_links);
    SHR_IF_ERR_EXIT(WRITE_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
soc_jer2_fabric_minimal_links_all_reachable_get(int unit, int *min_links)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    *min_links = soc_reg_field_get(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, reg_val, ALRC_MIN_NUM_OF_LINKSf);

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_interleaving_init(int unit)
{
    uint64 reg64_val;
    SHR_FUNC_INIT_VARS(unit);

    
    COMPILER_64_ZERO(reg64_val);
    SHR_IF_ERR_EXIT(READ_FDT_FDT_CONTEXTS_INTERLEAVINGr(unit, &reg64_val)); 
    soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_0_ENf, 0x7); 
    soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_1_ENf, 0x7);
    soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IPT_2_ENf, 0x7);
    soc_reg64_field32_set(unit, FDT_FDT_CONTEXTS_INTERLEAVINGr, &reg64_val, INTERLEAVING_IPT_TO_IRE_ENf, 0x7);
    SHR_IF_ERR_EXIT(WRITE_FDT_FDT_CONTEXTS_INTERLEAVINGr(unit, reg64_val));

    /*
     * set RCM (Reassembly Contexts Manager) to allocate contexts dynamically
     */
    COMPILER_64_ZERO(reg64_val);
    SHR_IF_ERR_EXIT(READ_RQP_GENERAL_RQP_CONFIGr(unit, 0, &reg64_val));
    soc_reg64_field32_set(unit, RQP_GENERAL_RQP_CONFIGr, &reg64_val, RCM_MODEf, 1);
    SHR_IF_ERR_EXIT(WRITE_RQP_GENERAL_RQP_CONFIGr(unit, SOC_CORE_ALL, reg64_val));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_delay_single_cell_in_fabric_rx_init(int unit)
{
    uint32 reg32_val = 0;
    uint32 max_delay = 0;
    uint32 pipe = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure maximum delay, between first cell and second cell, for all pipes.
     */
    max_delay = SOC_JER2_FABRIC_SINGLE_CELL_IN_FABRIC_RX_MAX_DELAY;
    for (pipe = SOC_REG_FIRST_ARRAY_INDEX(unit, FDR_FDR_REGISTER_0r);
         pipe < SOC_REG_FIRST_ARRAY_INDEX(unit, FDR_FDR_REGISTER_0r) + SOC_REG_NUMELS(unit, FDR_FDR_REGISTER_0r);
         ++pipe)
    {
        SHR_IF_ERR_EXIT(READ_FDR_FDR_REGISTER_0r(unit, pipe, &reg32_val));
        soc_reg_field_set(unit, FDR_FDR_REGISTER_0r, &reg32_val, FIELD_0_9f, max_delay);
        SHR_IF_ERR_EXIT(WRITE_FDR_FDR_REGISTER_0r(unit, pipe, reg32_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_set(int unit, int enable)
{
    uint32 reg32_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* 
     * FDR_FAP_GLOBAL_GENERAL_CFG_1r JERICHO_1_IN_SYSTEMf
     */
    SHR_IF_ERR_EXIT(READ_FDR_REG_00C3r(unit, &reg32_val));
    soc_reg_field_set(unit, FDR_REG_00C3r, &reg32_val, FIELD_3_3f, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_FDR_REG_00C3r(unit, reg32_val));

    /* 
     * RQP_FAP_GLOBAL_GENERAL_CFG_1r JERICHO_1_IN_SYSTEMf
     */
    SHR_IF_ERR_EXIT(READ_RQP_REG_00C3r(unit, 0, &reg32_val));
    soc_reg_field_set(unit, RQP_REG_00C3r, &reg32_val, FIELD_3_3f, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_RQP_REG_00C3r(unit, SOC_CORE_ALL, reg32_val));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_get(int unit, int *enable)
{
    uint32 reg32_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* 
     * FDR_FAP_GLOBAL_GENERAL_CFG_1r JERICHO_1_IN_SYSTEMf
     */
    SHR_IF_ERR_EXIT(READ_FDR_REG_00C3r(unit, &reg32_val));
    *enable = soc_reg_field_get(unit, FDR_REG_00C3r, reg32_val, FIELD_3_3f);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e soc_jer2_fabric_force_set(
    int                    unit, 
    soc_dnx_fabric_force_t force
)
{
    int modid = -1;
    uint32  sys_fap_id;
    uint32  fap_id_plus_1;
    uint64  reg64;
    uint32  reg32_val;
    uint64  reg64_val;
    int is_traffic_enabled;
    int is_ctrl_cells_enabled;
    SHR_FUNC_INIT_VARS(unit);

    /* check if traffic is enabled on NIF/Fabric sides. Writing to ECI_GLOBAL registers under traffic can cause undefined behavior. */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &is_traffic_enabled));
    SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, &is_ctrl_cells_enabled));

    if (is_traffic_enabled || is_ctrl_cells_enabled) {
        LOG_WARN(BSL_LS_SOC_FABRIC, (BSL_META_U(unit,
                                        " Warning: fabric force should not be done when traffic is enabled.\nTo disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.\n")));
    }

    /* Packets are mapped according to destenation device ID.
     * If Dest_ID | DQCQ_MASK = DQCQ_MAP_LSB | DQCQ_MASK packet is mapped to local core 0
     * If Dest_ID | DQCQ_MASK = DQCQ_MAP_MSB | DQCQ_MASK packet is mapped to local core 1 
     * Else packet goes to fabric*/ 

    if (force == socDnxFabricForceFabric) {
        /*Set DQCQ_MAP so that no dest_id are not mapped to either local core 0 or local core 1*/
        sys_fap_id = 0xFFFFFFFF;
        fap_id_plus_1 = 0xFFFFFFFF;
        COMPILER_64_ZERO(reg64); 
        COMPILER_64_ZERO(reg64_val);
        reg64_val = fap_id_plus_1 | (fap_id_plus_1 << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP);
        soc_reg64_field_set(unit, IPS_DEV_ID_TO_CONTEXT_MAPr, &reg64, DEV_ID_TO_CONTEXT_MAPf,
                            (reg64_val << 32) | (sys_fap_id | (sys_fap_id << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP)));
        SHR_IF_ERR_EXIT(soc_reg64_set(unit,IPS_DEV_ID_TO_CONTEXT_MAPr, SOC_CORE_ALL, 0, reg64));
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_0f, 0);
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_1f, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_DEV_ID_MASKr(unit, SOC_CORE_ALL, reg32_val));

        /*Force fabric for control cells*/
        SHR_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x1);
        SHR_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

        /* Set IPS_FORCE_FABRIC */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_FABRICr, &reg32_val, FORCE_FABRICf, 0x1);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_FABRICr(unit, SOC_CORE_ALL, reg32_val));

        /* Set IPS_FORCE_LOCAL, bit0:Local-0, bit1:Local-1 */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_LOCALr, &reg32_val, FORCE_LOCALf, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_LOCALr(unit, SOC_CORE_ALL, reg32_val));
    } else if (force == socDnxFabricForceLocal0) {
        /* Set DQCQ_MASK so that all dest_ids are mapped to core 0 
         * Set DQCQ_MAP so that no dest_ids are mapped to core 1 */
        sys_fap_id = 0;
        fap_id_plus_1 = 0xFFFFFFFF;
        COMPILER_64_ZERO(reg64);
        COMPILER_64_ZERO(reg64_val);
        reg64_val = fap_id_plus_1 | (fap_id_plus_1 << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP);
        soc_reg64_field_set(unit, IPS_DEV_ID_TO_CONTEXT_MAPr, &reg64, DEV_ID_TO_CONTEXT_MAPf,
                            (reg64_val << 32) | (sys_fap_id | (sys_fap_id << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP)));
        SHR_IF_ERR_EXIT(soc_reg64_set(unit,IPS_DEV_ID_TO_CONTEXT_MAPr, SOC_CORE_ALL, 0, reg64));
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_0f, 0x7FF);
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_1f, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_DEV_ID_MASKr(unit, SOC_CORE_ALL, reg32_val));

        /*Allow local route for control cells*/
        SHR_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0);
        SHR_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

        /* Set IPS_FORCE_FABRIC */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_FABRICr, &reg32_val, FORCE_FABRICf, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_FABRICr(unit, SOC_CORE_ALL, reg32_val));

        /* Set IPS_FORCE_LOCAL, bit0:Local-0, bit1:Local-1 */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_LOCALr, &reg32_val, FORCE_LOCALf, 0x1);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_LOCALr(unit, SOC_CORE_ALL, reg32_val));
    } else if (force == socDnxFabricForceLocal1) {
        /* Set DQCQ_MASK so that all dest_ids are mapped to core 1 
         * Set DQCQ_MAP so that no dest_ids are mapped to core 2 */
        sys_fap_id = 0xFFFFFFFF;
        fap_id_plus_1 = 0;
        COMPILER_64_ZERO(reg64);
        COMPILER_64_ZERO(reg64_val);
        reg64_val = fap_id_plus_1 | (fap_id_plus_1 << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP);
        soc_reg64_field_set(unit, IPS_DEV_ID_TO_CONTEXT_MAPr, &reg64, DEV_ID_TO_CONTEXT_MAPf,
                            (reg64_val << 32) | (sys_fap_id | (sys_fap_id << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP)));
        SHR_IF_ERR_EXIT(soc_reg64_set(unit,IPS_DEV_ID_TO_CONTEXT_MAPr, SOC_CORE_ALL, 0, reg64));
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_0f, 0);
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_1f, 0x7FF);
        SHR_IF_ERR_EXIT(WRITE_IPS_DEV_ID_MASKr(unit, SOC_CORE_ALL, reg32_val));

        /*Allow local route for control cells*/
        SHR_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0);
        SHR_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

        /* Set IPS_FORCE_FABRIC */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_FABRICr, &reg32_val, FORCE_FABRICf, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_FABRICr(unit, SOC_CORE_ALL, reg32_val));

        /* Set IPS_FORCE_LOCAL, bit0:Local-0, bit1:Local-1 */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_LOCALr, &reg32_val, FORCE_LOCALf, 0x2);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_LOCALr(unit, SOC_CORE_ALL, reg32_val));
    } else if (force == socDnxFabricForceRestore) {
        /* Restore default configurations according to device modid */
        SHR_IF_ERR_EXIT(bcm_stk_my_modid_get(unit,&modid));
        sys_fap_id = modid;
        fap_id_plus_1 = sys_fap_id + 1;
        if (dnx_data_device.general.nof_cores_get(unit) == 1)
        {
            fap_id_plus_1 = 0xFFFFFFFF;
        } 
        COMPILER_64_ZERO(reg64);
        COMPILER_64_ZERO(reg64_val);
        reg64_val = fap_id_plus_1 | (fap_id_plus_1 << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP);
        soc_reg64_field_set(unit, IPS_DEV_ID_TO_CONTEXT_MAPr, &reg64, DEV_ID_TO_CONTEXT_MAPf,
                            (reg64_val << 32) | (sys_fap_id | (sys_fap_id << SOC_JER2_FABRIC_FAPID_BIT_OFFSET_IN_DQCQ_MAP)));
        SHR_IF_ERR_EXIT(soc_reg64_set(unit,IPS_DEV_ID_TO_CONTEXT_MAPr, SOC_CORE_ALL, 0, reg64));
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_0f, 0);
        soc_reg_field_set(unit, IPS_DEV_ID_MASKr, &reg32_val, DEV_ID_MASK_1f, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_DEV_ID_MASKr(unit, SOC_CORE_ALL, reg32_val));

        /*Allow local route for control cells*/
        SHR_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0);
        SHR_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

        /* Set IPS_FORCE_FABRIC */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_FABRICr, &reg32_val, FORCE_FABRICf, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_FABRICr(unit, SOC_CORE_ALL, reg32_val));

        /* Set IPS_FORCE_LOCAL, bit0:Local-0, bit1:Local-1 */
        reg32_val = 0;
        soc_reg_field_set(unit, IPS_FORCE_LOCALr, &reg32_val, FORCE_LOCALf, 0);
        SHR_IF_ERR_EXIT(WRITE_IPS_FORCE_LOCALr(unit, SOC_CORE_ALL, reg32_val));
    } else {      /*Error Checking*/
        cli_out("Option not supported\n");
        cli_out("Supported options are fabric, core0, core1, default\n");
        SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

exit:
  SHR_FUNC_EXIT;
}
/* } */


/*
 * Function:
 *      soc_jer2_fabric_reachability_status_get
 * Purpose:
 *      Get reachability status
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      moduleid            - (IN)  Module to check reachbility to
 *      links_max           - (IN)  Max size of links_array
 *      links_array         - (OUT) Links which moduleid is erachable through
 *      links_count         - (OUT) Size of links_array
 * Returns:
 *      _SHR_E_xxx
 */
shr_error_e
soc_jer2_fabric_reachability_status_get(
    int unit,
    int moduleid,
    int links_max,
    uint32 *links_array,
    int *links_count
)
{
    int i, offset, port;
    soc_reg_above_64_val_t rtp_reg_val, link_active_mask;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(rtp_reg_val);
    if(moduleid < 0 || moduleid > 2048) {
      SHR_ERR_EXIT(_SHR_E_PARAM, "module id invalid");
    }
    SHR_IF_ERR_EXIT(READ_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm(unit, MEM_BLOCK_ANY, moduleid/2, rtp_reg_val));
    SHR_IF_ERR_EXIT(READ_RTP_LINK_ACTIVE_MASKr(unit, link_active_mask));

    *links_count = 0;

    /*
     * Each entry in RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS table contains a bitmap of valid links to reach even modid followed by odd modid.
     * Example- entry 0 (read from right to left): <valid links to reach modid 1> <valid links to reach modid 0>.
     */
    offset = (dnx_data_fabric.links.nof_links_get(unit))*(moduleid%2);

    PBMP_SFI_ITER(unit, port)
    {
        i = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port);

        if(SHR_BITGET(rtp_reg_val,i + offset) && !SHR_BITGET(link_active_mask,i))
        {
            if(*links_count >= links_max) {
                SHR_ERR_EXIT(_SHR_E_FULL, "links_array is too small");
            }

            links_array[*links_count] = i;
            (*links_count)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_jer2_fabric_topology_status_connectivity_get(
    int                 unit,
    int                 link_index_min,
    int                 link_index_max,
    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
)
{
    int
        source_lvl;
    bcm_port_t
        link_id, link_index;
    uint32
        reg_val,
        is_active;
    soc_reg_above_64_val_t mask;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mask);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_is_supported))
    {
        SHR_EXIT();
    }

    SHR_NULL_CHECK(connectivity_map, _SHR_E_PARAM, "connectivity_map");

    SHR_RANGE_VERIFY(link_index_min, 0, dnx_data_fabric.links.nof_links_get(unit) - 1, _SHR_E_PARAM, "link_index_min is out of bound.\n");
    SHR_RANGE_VERIFY(link_index_max, 0, dnx_data_fabric.links.nof_links_get(unit) - 1, _SHR_E_PARAM, "link_index_max is out of bound.\n");
    SHR_MAX_VERIFY(link_index_min, link_index_max, _SHR_E_PARAM, "link_index_min must be <= link_index_max\n");

    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(connectivity_map);

    SHR_IF_ERR_EXIT(READ_RTP_LINK_ACTIVE_MASKr(unit, mask));

    SOC_REG_ABOVE_64_NOT(mask);

    for (link_index = link_index_min; link_index <= link_index_max; ++link_index)
    {
        link_id = link_index;
        SHR_BITTEST_RANGE(mask, link_id, 1, is_active);
        if (is_active)
        {
            SHR_IF_ERR_EXIT(READ_FCR_CONNECTIVITY_MAP_REGISTERSr(unit, link_id, &reg_val));
            connectivity_map->link_info[link_index].far_unit = soc_reg_field_get(unit, FCR_CONNECTIVITY_MAP_REGISTERSr, reg_val, SOURCE_DEVICE_ID_Nf);

            source_lvl = soc_reg_field_get(unit, FCR_CONNECTIVITY_MAP_REGISTERSr, reg_val, SOURCE_DEVICE_LEVEL_Nf);
#ifdef PLISIM
            if (SAL_BOOT_PLISIM) {
                source_lvl = 0x3; /* To be FE2 */
            }
#endif
            /*
             *
             * 3'bx0x => FOP
             * 3'b010 => FE3,
             * 3'bX11 => FE2,
             * 3'b110 => FE1
             */

            if ((source_lvl & 0x2) == 0)
            {
                connectivity_map->link_info[link_index].far_dev_type = SOC_DNX_FAR_DEVICE_TYPE_FAP;
            }
            else if (source_lvl == 0x2)
            {
                connectivity_map->link_info[link_index].far_dev_type = SOC_DNX_FAR_DEVICE_TYPE_FE3;
            }
            else if ((source_lvl & 0x3) == 0x3)
            {
                connectivity_map->link_info[link_index].far_dev_type = SOC_DNX_FAR_DEVICE_TYPE_FE2;
            }
            else
            {
                connectivity_map->link_info[link_index].far_dev_type = SOC_DNX_FAR_DEVICE_TYPE_FE1;
            }

            connectivity_map->link_info[link_index].far_link_id = soc_reg_field_get(unit, FCR_CONNECTIVITY_MAP_REGISTERSr, reg_val, SOURCE_DEVICE_LINK_Nf);
            connectivity_map->link_info[link_index].is_logically_connected=1;
        }
        else
        {
            connectivity_map->link_info[link_index].far_link_id = DNXC_FABRIC_LINK_NO_CONNECTIVITY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_jer2_fabric_link_status_clear(int unit, soc_port_t link)
{
    int blk_id, inner_link;
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    blk_id = link / 4;
    inner_link = link % 4;

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, RX_CRC_ERR_N_INTf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, &reg_val, WRONG_SIZE_INTf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,reg_val));

    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,reg_val));
   
    reg_val = 0x0;
    soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, &reg_val, DEC_ERR_INTf, 1 << inner_link);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_jer2_fabric_link_status_get(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  soc_port_t link_id,
    DNX_SAND_OUT uint32 *link_status,
    DNX_SAND_OUT uint32 *errored_token_count)
{
    uint32 reg_val, field_val[1], sig_acc = 0;
    int blk_id, reg_select;
    soc_port_t inner_lnk;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    *link_status = 0;

    blk_id = link_id / 4;
    reg_select = link_id % 4;
   
    /*leaky bucket*/
    SHR_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKETr(unit,blk_id, reg_select, &reg_val));
    *errored_token_count = soc_reg_field_get(unit, FMAC_LEAKY_BUCKETr, reg_val, MACR_N_LKY_BKT_VALUEf);

  /*link status
    BCM_FABRIC_LINK_STATUS_CRC_ERROR Non-zero CRC rate  
    BCM_FABRIC_LINK_STATUS_SIZE_ERROR Non-zero size error-count  
    BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR Non-zero code group error-count  
    BCM_FABRIC_LINK_STATUS_MISALIGN Link down, misalignment error  
    BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK Link down, SerDes signal lock error  
    BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP Link up, but not accepting reachability cells  
    BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS Low value, indicates bad link connectivity or link down, based on reachability cells */

    inner_lnk = link_id % 4;
    SHR_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, RX_CRC_ERR_N_INTf);
    if(SHR_BITGET(field_val, inner_lnk))
        *link_status |= DNXC_FABRIC_LINK_STATUS_CRC_ERROR;

    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_1r, reg_val, WRONG_SIZE_INTf);
    if(SHR_BITGET(field_val, inner_lnk))
        *link_status |= DNXC_FABRIC_LINK_STATUS_SIZE_ERROR;  

    SHR_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
    if(SHR_BITGET(field_val, inner_lnk))
       *link_status |= DNXC_FABRIC_LINK_STATUS_MISALIGN;  

    SHR_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_4r(unit,blk_id,&reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_4r, reg_val, DEC_ERR_INTf);
    if(SHR_BITGET(field_val, inner_lnk))
       *link_status |= DNXC_FABRIC_LINK_STATUS_CODE_GROUP_ERROR;  

    /*BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK - Serdes TBD*/
    rv = soc_dnxc_port_rx_locked_get(unit, SOC_DNX_FABRIC_LINK_TO_PORT(unit, link_id), &sig_acc);
    SHR_IF_ERR_EXIT(rv);

    if(!sig_acc) {
        *link_status |= DNXC_FABRIC_LINK_STATUS_NO_SIG_ACCEP;
    }

    if(*errored_token_count < 63) {
       *link_status |= DNXC_FABRIC_LINK_STATUS_ERRORED_TOKENS;
    }

    /*Clear sticky indication*/
    rv = soc_jer2_fabric_link_status_clear(unit, link_id);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;  
}

/*
 * See .h file.
 */
shr_error_e soc_jer2_fabric_multicast_init(int unit)
{
    uint32 reg_val = 0;
    uint64 reg64_val;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64_val);

    /*
     * Enable Fabric Multicast
     */
    SHR_IF_ERR_EXIT(READ_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr, &reg_val, ENABLE_MCL_UPDATESf, 1);
    SHR_IF_ERR_EXIT(WRITE_RTP_MULTICAST_DISTRIBUTION_CONFIGURATIONr(unit, reg_val));

    /*
     * Set all-reachable vector calculation rate
     */
    SHR_IF_ERR_EXIT(READ_RTP_MC_TRAVERSE_RATEr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_MC_TRAVERSE_RATEr, &reg_val, MC_TRAVERSE_RATEf, 0x1800);
    SHR_IF_ERR_EXIT(WRITE_RTP_MC_TRAVERSE_RATEr(unit, reg_val));

    /*
     * By default open FMC shaper to maximum
     */
    SHR_IF_ERR_EXIT(READ_IPS_FMC_TOP_SHAPER_CONFIGr(unit, 0, &reg64_val));
    soc_reg64_field32_set(unit, IPS_FMC_TOP_SHAPER_CONFIGr, &reg64_val, FMC_TOP_MAX_CRDT_RATEf, 1);
    SHR_IF_ERR_EXIT(WRITE_IPS_FMC_TOP_SHAPER_CONFIGr(unit, SOC_CORE_ALL, reg64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e soc_jer2_fabric_aluwp_init(int unit)
{
    uint32 reg_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** ALUWP - Active-Link-Up-Watchdog-Period */
    SHR_IF_ERR_EXIT(READ_FDR_FDR_ENABLERS_REGISTER_1r_REG32(unit, &reg_val));
    soc_reg_field_set(unit, FDR_FDR_ENABLERS_REGISTER_1r, &reg_val, AUTO_DOC_NAME_3f, 0xfd);
    SHR_IF_ERR_EXIT(WRITE_FDR_FDR_ENABLERS_REGISTER_1r_REG32(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e soc_jer2_fabric_link_force_signal_set(int unit, int link, int force_signal)
{
    soc_reg_above_64_val_t reg_above_64_val, force_signal_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(force_signal_bitmap);

    SHR_IF_ERR_EXIT(READ_FCT_AUTO_DOC_NAME_10r(unit, reg_above_64_val));
    soc_reg_above_64_field_get(unit, FCT_AUTO_DOC_NAME_10r, reg_above_64_val, FORCE_MACT_READY_FIELD_4f, force_signal_bitmap);
    SHR_BITWRITE(force_signal_bitmap, link, force_signal);
    soc_reg_above_64_field_set(unit, FCT_AUTO_DOC_NAME_10r, reg_above_64_val, FORCE_MACT_READY_FIELD_4f, force_signal_bitmap);
    SHR_IF_ERR_EXIT(WRITE_FCT_AUTO_DOC_NAME_10r(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init link on FMAC
 */
shr_error_e soc_jer2_fabric_link_mac_init(int unit, soc_port_t logical_port)
{
    int fmac_index = 0;
    int fmac_inner_link = 0;
    int control_cells_enable = 0;
    uint32 field[1] = {0};
    uint32 nof_pipes = 0;
    uint32 reg_val = 0;
    uint64 reg64_val;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64_val);

    if (soc_sand_is_emulation_system(unit) == 0) /* FMAC Does not work in emulation */
    {
        soc_jer2_fabric_port_to_fmac(unit, logical_port, &fmac_index, &fmac_inner_link);

        /*
         * Configure nof pipes
         */
        nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, &reg_val)); 
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_PARALLEL_DATA_PATHf, nof_pipes - 1);
        SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, reg_val));

        /*
         * Enable PCP
         */
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, &reg_val)); 
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_PCP_ENABLEDf, 1);
        SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, reg_val));

        /*
         * Configure Leaky Bucket
         */
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, fmac_index, fmac_inner_link, &reg_val));
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, BKT_FILL_RATE_Nf, SOC_JER2_FABRIC_LINK_FAP_BKT_FILL_RATE);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, BKT_LINK_UP_TH_Nf, SOC_JER2_FABRIC_LINK_FAP_BKT_UP_LINK_TH);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, BKT_LINK_DN_TH_Nf, SOC_JER2_FABRIC_LINK_FAP_BKT_DN_LINK_TH);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, SIG_DET_BKT_RST_ENA_Nf, 1);
        soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val, ALIGN_LCK_BKT_RST_ENA_Nf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, fmac_index, fmac_inner_link, reg_val));

        /*
         * Configure Comma Burst
         */
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CM_BRST_SIZEf, SOC_JER2_FABRIC_COMMA_BURST_SIZE);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CM_TX_PERIODf, SOC_JER2_FABRIC_COMMA_BURST_PERIOD);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CM_BRST_SIZE_LLFCf, SOC_JER2_FABRIC_COMMA_BURST_SIZE);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CM_TX_PERIOD_LLFCf, SOC_JER2_FABRIC_COMMA_BURST_PERIOD);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CM_BRST_LLFC_ENABLERf, SOC_JER2_FABRIC_COMMA_BURST_LLFC_ENABLER);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CM_TX_BYTE_MODEf, SOC_JER2_FABRIC_COMMA_BURST_BYTE_MODE);
        SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, reg_val));

        /*
         * Set link topology mode
         */
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &control_cells_enable));

        if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_FE)
        {
            SHR_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, &reg_val));
            field[0] = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val, LINK_TOPO_MODE_0f);
            SHR_BITWRITE(field, fmac_inner_link, control_cells_enable);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val, LINK_TOPO_MODE_0f, field[0]);
            SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, reg_val));

            SHR_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, &reg_val));
            field[0] = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val, LINK_TOPO_MODE_1f);
            SHR_BITWRITE(field, fmac_inner_link, (control_cells_enable == FALSE));
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val, LINK_TOPO_MODE_1f, field[0]);
            SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_index, reg_val));
        }

        SHR_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_2r(unit, fmac_index, &reg_val));
        field[0] = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_2r, reg_val, LINK_TOPO_MODE_2f);
        SHR_BITWRITE(field, fmac_inner_link, control_cells_enable);
        soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_2r, &reg_val, LINK_TOPO_MODE_2f, field[0]);
        SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_2r(unit, fmac_index, reg_val));

        /*
         * Enable RX_LOS_SYNC interrupt
         */
        SHR_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, fmac_index, fmac_inner_link, &reg_val));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, fmac_index, fmac_inner_link, reg_val));

        /*
         * Turn on link pump when leaky bucket is down for better link utilization
         */
        COMPILER_64_ZERO(reg64_val);
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val ,FMAL_N_TX_PUMP_WHEN_LB_DNf, 1);
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_index, fmac_inner_link, &reg64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e soc_jer2_fabric_mac_init(int unit, int fmac_index)
{
    int control_cells_enable = 0;
    uint64 reg64_val;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64_val);

    if (soc_sand_is_emulation_system(unit) == 0) /* FMAC Does not work in emulation */
    {
        /*
         * Set FMAC to work on FAP mode
         */
        SHR_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_index, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg64_val ,FAP_MODEf, 1);
        SHR_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_index, reg64_val));

        /*
         * Init global shaper
         */
        /** tx_fap_global_shaper */
        SHR_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_index, &reg64_val));
        /** Shape all except comma injection */
        soc_reg64_field32_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, GENERAL_CONFIGURATION_REGISTER_FIELD_0f, 0x7);
        SHR_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_index, reg64_val));

        /*
         * Set LLFC control cells
         */
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &control_cells_enable));

        SHR_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_index, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, ENABLE_SERIAL_LINKf, (control_cells_enable == TRUE) ? 0x0 : 0x1);
        SHR_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_index, reg64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e soc_jer2_fabric_port_enable_set(int unit, soc_port_t logical_port, int enable)
{
    soc_reg_above_64_val_t reg_above_64_val;
    int link_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));

    /*
     * Set link force signal
     */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_link_force_signal_set(unit, link_id, (enable) ? 0 : 1));

    if (enable)
    {
        int is_link_allowed = 0;

        /*
         * Enable link in FDTL
         */
        SHR_IF_ERR_EXIT(READ_FDTL_LINK_RESETr(unit, reg_above_64_val));
        SHR_BITCLR(reg_above_64_val, link_id);
        SHR_IF_ERR_EXIT(WRITE_FDTL_LINK_RESETr(unit, reg_above_64_val));

        /*
         * Enable port
         */
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, logical_port, 0, enable));

        /*
         * Enable link in RTP according to sw_state
         */
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.get(unit, link_id, &is_link_allowed));
        SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_id, !is_link_allowed));

        /*
         * Enable link in FDT
         */
        SHR_IF_ERR_EXIT(WRITE_FDT_LINK_RESETr(unit, reg_above_64_val));
    }
    else
    {
        /*
         * Disable link in FDT
         */
        SHR_IF_ERR_EXIT(READ_FDT_LINK_RESETr(unit, reg_above_64_val));
        SHR_BITSET(reg_above_64_val, link_id);
        SHR_IF_ERR_EXIT(WRITE_FDT_LINK_RESETr(unit, reg_above_64_val));

        /*
         * Disable link in RTP regardless of sw_state
         */
        SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_id, 1));
        sal_usleep(1000);

        /*
         * Disable port
         */
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, logical_port, 0, enable));

        /*
         * Disable link in FDTL
         */
        SHR_IF_ERR_EXIT(WRITE_FDTL_LINK_RESETr(unit, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
