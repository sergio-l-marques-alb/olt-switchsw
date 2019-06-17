/** \file fabric_transmit.c
 *
 * Functions for handling Fabric transmit adapter.
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
#include <sal/compiler.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/legacy/TMC/tmc_api_fabric.h>

#include <bcm_int/dnx/fabric/fabric_transmit.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit) \
    ((dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_MESH) || \
      (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_SINGLE_FAP))

/*
 * The DTQs have 4 FC thresholds. Crossing the thresholds send the appropriate
 * FC signal.
 * The 'enable' defines are the HW bit masks.
 */
/** Shaper congestion indication */
#define SOC_JER2_FABRIC_DTQ_FC_ENABLE_SHAPER (0x1)
/** Low Priority PDQ congestion indication */
#define SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP     (0x2)
/** High Priority PDQ congestion indication */
#define SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP     (0x4)
/** Read Data Fifo congestion indication */
#define SOC_JER2_FABRIC_DTQ_FC_ENABLE_RDF    (0x8)

/** Total size the configurable DTQs shared */
#define SOC_JER2_FABRIC_PDQS_TOTAL_SIZE (26*1024)
/** Number of PDQs that shared the common PDQs memory */
#define SOC_JER2_FABRIC_PDQS_SHARED_NOF (26)

/** HW index of the WFQ between OCB-Only and HP queues */
#define SOC_JER2_FABRIC_WFQ_PRIO_INDEX      (9)
/** Default packet size for WFQ */
#define SOC_JER2_FABRIC_WFQ_DFLT_PKT_SIZE   (12*1024)

/*
 * Each DTQ shaper has its own FC.
 * local=2 + fabric=3 + total_fabric = 6
 */
#define SOC_JER2_FABRIC_DTQ_SHAPERS_FC_NOF  (6)

/*
 * Sizes of Read Request Fifos.
 * OCB Only + SRAM + MIXS RRFs = 8K
 */
#define SOC_JER2_FABRIC_RRF_OCB_ONLY_DEPTH  (128)
#define SOC_JER2_FABRIC_RRF_SRAM_DEPTH      (4*1024 - SOC_JER2_FABRIC_RRF_OCB_ONLY_DEPTH)
#define SOC_JER2_FABRIC_RRF_MIXS_DEPTH      (4*1024)
/** It takes time for the FC signal to reach from the RRF to the PDQ */
#define SOC_JER2_FABRIC_RRF_IRDY            (13)
/** RRF thresholds */
#define SOC_JER2_FABRIC_RRF_SRAM_HP_FC_HIGH_TH   (SOC_JER2_FABRIC_RRF_SRAM_DEPTH - 46 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_SRAM_HP_FC_LOW_TH    (SOC_JER2_FABRIC_RRF_SRAM_DEPTH - 100 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_SRAM_LP_FC_HIGH_TH   (SOC_JER2_FABRIC_RRF_SRAM_DEPTH - 66 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_SRAM_LP_FC_LOW_TH    (SOC_JER2_FABRIC_RRF_SRAM_DEPTH - 100 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_MIXS_HP_FC_HIGH_TH   (SOC_JER2_FABRIC_RRF_MIXS_DEPTH - 46 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_MIXS_HP_FC_LOW_TH    (SOC_JER2_FABRIC_RRF_MIXS_DEPTH - 100 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_MIXS_LP_FC_HIGH_TH   (SOC_JER2_FABRIC_RRF_MIXS_DEPTH - 66 - SOC_JER2_FABRIC_RRF_IRDY)
#define SOC_JER2_FABRIC_RRF_MIXS_LP_FC_LOW_TH    (SOC_JER2_FABRIC_RRF_MIXS_DEPTH - 100 - SOC_JER2_FABRIC_RRF_IRDY)

/** Number of DTQs */
#define SOC_JER2_FABRIC_DTQ_NOF             (6)
/** Maximum size for all DTQs together */
#define SOC_JER2_FABRIC_DTQ_MAX_SIZE        (2048)
/** Size of OCB-Only DTQs */
#define SOC_JER2_FABRIC_DTQ_OCB_ONLY_SIZE   (128)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 * \brief
 *   PDQ contexts.
 * \remark
 *   The following contexts are relevant only for MESH:
 *     SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_HP
 *     SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_LP
 */
typedef enum
{
    SOC_JER2_FABRIC_PDQ_CONTEXT_NONE = -1,
    /** Local 0 High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_0_HP = 0,
    /** Local 0 Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_0_LP,
    /** Local 1 High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_1_HP,
    /** Local 1 Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_1_LP,
    /** Mesh: Device 1 High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_HP,
    /** Clos: Fabric Unicast High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_FUC_HP = SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_HP,
    /** Mesh: Device 1 Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_LP,
    /** Clos: Fabric Unicast Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_FUC_LP = SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_LP,
    /** Only for Mesh: Device 2 High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_HP,
    /** Only for Mesh: Device 2 Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_LP,
    /** Mesh: Device 3 High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_HP,
    /** Clos: Fabric Multicast High Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_FMC_HP = SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_HP,
    /** Mesh: Device 3 Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_LP,
    /** Clos: Fabric Multicast Low Priority PDQ context */
    SOC_JER2_FABRIC_PDQ_CONTEXT_FMC_LP = SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_LP,
    SOC_JER2_FABRIC_PDQ_CONTEXT_NOF
} soc_jer2_fabric_pdq_context_e;

/*
 * \brief
 *   Holds DTQ-FC enable masks.
 *   The masks are from SOC_JER2_FABRIC_DTQ_FC_ENABLE_* masks,
 *   and tells which flow control from a DTQ affects the PDQs.
 * \see
 *   * soc_jer2_fabric_pdqs_correspondence_to_dtqs_fc_init
 */
typedef struct
{
    /** Enable mask for Local 0 */
    int local_0_mask;
    /** Enable mask for Local 1 */
    int local_1_mask;
    /** Enable mask for Fabric 0 */
    int fabric_0_mask;
    /** Enable mask for Fabric 1 */
    int fabric_1_mask;
    /** Enable mask for Fabric 2 */
    int fabric_2_mask;
} soc_jer2_fabric_pdq_dtq_fc_enable_mask_t;

/*
 * \brief
 *   DTQs Global Shapers.
 *   Values represent actual index in HW table.
 */
typedef enum
{
    /** Global shaper for Fabric MC / Device 3 in Mesh */
    SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_FMC_DEV3 = 0,
    /** Global shaper for Fabric UC / Device 2 in Mesh */
    SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_FUC_DEV2,
    /** Global shaper for Total Fabric / Device 1 in Mesh */
    SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_TOTAL_DEV1,
    /** Global shaper for EGQ 1 */
    SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_EGQ1,
    /** Global shaper for EGQ 0 */
    SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_EGQ0,
    SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_NOF
} soc_jer2_fabric_dtq_global_shaper_e;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * \brief
 *   Set the High Priority Flow Control mask on the appropriate DTQ,
 *   in case of the following contexts:
 *     SOC_JER2_FABRIC_PDQ_CONTEXT_FMC_HP
 *     SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_HP
 *   The decision considers Mesh mode and pipes mapping.
 */
static shr_error_e
soc_jer2_fabric_dtq_fmc_hp_fc_enable_mask_get(int unit, soc_jer2_fabric_pdq_dtq_fc_enable_mask_t *fc_enable_mask)
{
    soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
    {
        /*
         * In case of Mesh mode, the DEV3 PDQ traffic
         * flows towards Fabric2 DTQ
         */
        fc_enable_mask->fabric_2_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
    }
    else
    {
        switch (map_type)
        {
            /*
             * In case of UC/HP-MC/LP-MC pipes map, the FMC HP PDQ traffic
             * flows towards Fabric1 DTQ
             */
            case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            {
                fc_enable_mask->fabric_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                break;
            }
            /*
             * In case of UC/MC pipes map, the FMC PDQ traffic
             * flows towards Fabric1 DTQ
             */
            case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
            case soc_dnxc_fabric_pipe_map_dual_uc_mc:
            {
                fc_enable_mask->fabric_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                break;
            }
            /*
             * In case of single pipes map, the FMC PDQ traffic
             * flows towards Fabric0 DTQ
             */
            case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
            case soc_dnxc_fabric_pipe_map_single:
            {
                fc_enable_mask->fabric_0_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set the Low Priority Flow Control mask on the appropriate DTQ,
 *   in case of the following contexts:
 *     SOC_JER2_FABRIC_PDQ_CONTEXT_FMC_LP
 *     SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_LP
 *   The decision considers Mesh mode and pipes mapping.
 */
static shr_error_e
soc_jer2_fabric_dtq_fmc_lp_fc_enable_mask_get(int unit, soc_jer2_fabric_pdq_dtq_fc_enable_mask_t *fc_enable_mask)
{
    soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
    {
        /*
         * In case of Mesh mode, the DEV3 PDQ traffic
         * flows towards Fabric2 DTQ
         */
        fc_enable_mask->fabric_2_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
    }
    else
    {
        switch (map_type)
        {
            /*
             * In case of UC/HP-MC/LP-MC pipes map, the FMC LP PDQ traffic
             * flows towards Fabric2 DTQ
             */
            case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            {
                fc_enable_mask->fabric_2_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                break;
            }
            /*
             * In case of UC/MC pipes map, the FMC PDQ traffic
             * flows towards Fabric1 DTQ
             */
            case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
            case soc_dnxc_fabric_pipe_map_dual_uc_mc:
            {
                fc_enable_mask->fabric_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                break;
            }
            /*
             * In case of single pipes map, the FMC PDQ traffic
             * flows towards Fabric0 DTQ
             */
            case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
            case soc_dnxc_fabric_pipe_map_single:
            {
                fc_enable_mask->fabric_0_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure which DTQ-FC signal will stop the read from the PDQ.
 *   When DTQ-FC thresholds are crossed, the DTQ send FC signals.
 *   Since there are 4 DTQ-FCs, need to configure for each PDQ which FC
 *   should cause it to stop.
 *   The relevant DTQ-FCs for PDQs are:
 *     SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP
 *     SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP
 */
static shr_error_e
soc_jer2_fabric_pdqs_correspondence_to_dtqs_fc_init(int unit)
{
    uint32 reg32_val = 0;
    soc_jer2_fabric_pdq_context_e context = SOC_JER2_FABRIC_PDQ_CONTEXT_NONE;
    soc_jer2_fabric_pdq_dtq_fc_enable_mask_t fc_enable_mask;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * For each PDQ, configure which DTQ-FC will cause it to stop.
     */
    for (context = 0; context < SOC_JER2_FABRIC_PDQ_CONTEXT_NOF; ++context)
    {
        /*
         * First Set to ignore all Flow Controls, and then enable the
         * appropriate Flow Control.
         */
        sal_memset(&fc_enable_mask, 0, sizeof(fc_enable_mask));

        switch (context)
        {
            case SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_0_HP:
            {
                /** Local0 PDQ traffic flows towards Local0 DTQ */
                fc_enable_mask.local_0_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                break;
            }
            case SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_0_LP:
            {
                /** Local0 PDQ traffic flows towards Local0 DTQ */
                fc_enable_mask.local_0_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                break;
            }
            case SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_1_HP:
            {
                /** Local1 PDQ traffic flows towards Local1 DTQ */
                fc_enable_mask.local_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                break;
            }
            case SOC_JER2_FABRIC_PDQ_CONTEXT_LOC_1_LP:
            {
                /** Local1 PDQ traffic flows towards Local1 DTQ */
                fc_enable_mask.local_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                break;
            }
            /*
             * (the case is commented out because is has the same context
             *  as SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_HP)
             * case SOC_JER2_FABRIC_PDQ_CONTEXT_FUC_HP:
             */
            case SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_HP:
            {
                /** FUC/DEV1 PDQ traffic flows towards Local0 DTQ */
                fc_enable_mask.fabric_0_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                break;
            }
            /*
             * (the case is commented out because is has the same context
             *  as SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_LP)
             * case SOC_JER2_FABRIC_PDQ_CONTEXT_FUC_LP:
             */
            case SOC_JER2_FABRIC_PDQ_CONTEXT_DEV1_LP:
            {
                /** FUC/DEV1 PDQ traffic flows towards Local0 DTQ */
                fc_enable_mask.fabric_0_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                break;
            }
            case SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_HP:
            {
                /** Relevant only in Mesh mode */
                if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
                {
                    /** DEV2 PDQ traffic flows towards Local1 DTQ */
                    fc_enable_mask.fabric_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_HP;
                }
                break;
            }
            case SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_LP:
            {
                /** Relevant only in Mesh mode */
                if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
                {
                    /** DEV2 PDQ traffic flows towards Local1 DTQ */
                    fc_enable_mask.fabric_1_mask = SOC_JER2_FABRIC_DTQ_FC_ENABLE_LP;
                }
                break;
            }
            /*
             * (the case is commented out because is has the same context
             *  as SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_HP)
             * case SOC_JER2_FABRIC_PDQ_CONTEXT_FMC_HP:
             */
            case SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_HP:
            {
                SHR_IF_ERR_EXIT(soc_jer2_fabric_dtq_fmc_hp_fc_enable_mask_get(unit, &fc_enable_mask));
                break;
            }
            /*
             * (the case is commented out because is has the same context
             *  as SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_LP)
             * case SOC_JER2_FABRIC_PDQ_CONTEXT_FMC_LP:
             */
            case SOC_JER2_FABRIC_PDQ_CONTEXT_DEV3_LP:
            {
                SHR_IF_ERR_EXIT(soc_jer2_fabric_dtq_fmc_lp_fc_enable_mask_get(unit, &fc_enable_mask));
                break;
            }
            /*
             * Coverity explanation: we really can't reach 'default', since all
             * cases in the enum are covered, but the instructions are that
             * switch-case statements should always have 'default' case.
             */
            /** coverity[dead_error_begin:FALSE] */
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown context %d", context);
            }
        }

        /*
         * Set which DTQ-FC will cause stop read from both SRAM & MIX PDQs
         */
        SHR_IF_ERR_EXIT(READ_IPT_PDQ_DTQ_FC_ENABLEr(unit, context, &reg32_val));
        soc_reg_field_set(unit, IPT_PDQ_DTQ_FC_ENABLEr, &reg32_val, PDQ_N_LOC_0_DTQ_FC_ENABLEf, fc_enable_mask.local_0_mask);
        soc_reg_field_set(unit, IPT_PDQ_DTQ_FC_ENABLEr, &reg32_val, PDQ_N_LOC_1_DTQ_FC_ENABLEf, fc_enable_mask.local_1_mask);
        soc_reg_field_set(unit, IPT_PDQ_DTQ_FC_ENABLEr, &reg32_val, PDQ_N_FAB_0_DTQ_FC_ENABLEf, fc_enable_mask.fabric_0_mask);
        soc_reg_field_set(unit, IPT_PDQ_DTQ_FC_ENABLEr, &reg32_val, PDQ_N_FAB_1_DTQ_FC_ENABLEf, fc_enable_mask.fabric_1_mask);
        soc_reg_field_set(unit, IPT_PDQ_DTQ_FC_ENABLEr, &reg32_val, PDQ_N_FAB_2_DTQ_FC_ENABLEf, fc_enable_mask.fabric_2_mask);
        SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_DTQ_FC_ENABLEr(unit, context, reg32_val));
    }

    /*
     * Set which DTQ-FC will cause stop read from MIX PDQs.
     * Value of 0 means no Flow Control will affect the PDQ.
     * Keep IPT_PDQ_DTQ_MIX_FC_ENABLEr with default value 0.
     */

    /*
     * Set which DTQ-FC will cause stop read from SRAM PDQs.
     * Value of 0 means no Flow Control will affect the PDQ.
     * Keep IPT_PDQ_DTQ_SRAM_FC_ENABLEr with default value 0.
     */

    /*
     * Set which DTQ-FC will cause stop read from OCB-Only PDQs.
     * Value of 0 means no Flow Control will affect the PDQ.
     * Note that there are only High priority OCB-Only PDQs, hence
     * there are only half OCB-Only PDQs contexts.
     * Keep IPT_PDQ_OCB_DTQ_FC_ENABLEr with default value 0.
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   PDQ configurations.
 *   There are 26K (+2K for S2D) entries in memory that can be shared
 *   amongst the PDQs:
 *     SRAM - 10 PDQ contexts.
 *     Mix - 10 PDQ contexts.
 *     OCB-Only - 5 PDQ contexts.
 *     DRAM-Delete - 1 PDQ context.
 */
static shr_error_e
soc_jer2_fabric_pdqs_contexts_init(int unit)
{
    soc_reg_above_64_val_t reg_above64_val;
    uint64 reg64_val;
    uint32 reg32_val = 0;
    soc_jer2_fabric_pdq_context_e context = SOC_JER2_FABRIC_PDQ_CONTEXT_NONE;
    int pdq_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    COMPILER_64_ZERO(reg64_val);

    /*
     * The PDQs memory is shared between the PDQs equally.
     * On CLOS mode 2 PDQ contexts are not used, so each PDQ will have
     * a bigger size.
     */
    if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
    {
        /** The '-1' is to make sure we don't overflow the memory */
        pdq_size = SOC_JER2_FABRIC_PDQS_TOTAL_SIZE / SOC_JER2_FABRIC_PDQS_SHARED_NOF - 1;
    }
    else
    {
        pdq_size = SOC_JER2_FABRIC_PDQS_TOTAL_SIZE / (SOC_JER2_FABRIC_PDQS_SHARED_NOF - 2);
    }

    /*
     * Configure SRAM + MIX queues.
     */
    for (context = 0; context < SOC_JER2_FABRIC_PDQ_CONTEXT_NOF; ++context)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if (!SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit) &&
           (context == SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_HP || context == SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_LP))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(READ_IPT_PDQ_CFGr(unit, context, reg_above64_val));
        /** Configure PDQ size */
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_SRAM_SIZEf, pdq_size);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIX_SIZEf, pdq_size);
        /** Configure Flow Control thresholds toward the IPS */
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_SRAM_FC_THf, 700);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIXS_FC_THf, 700);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIXD_FC_THf, 700);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIX_MIXS_FC_THf, 600);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIX_MIXD_FC_THf, 600);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_SRAM_EIR_FC_THf, 800);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIXS_EIR_FC_THf, 800);
        soc_reg_above_64_field32_set(unit, IPT_PDQ_CFGr, reg_above64_val, PDQ_N_MIXD_EIR_FC_THf, 800);
        SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_CFGr(unit, context, reg_above64_val));
    }

    /*
     * Configure OCB queues.
     * OCB queues already have priority over regular queues, so there
     * is no need for the LP contexts. As a result, we only have half
     * of the contexts.
     */
    for (context = 0; context < SOC_JER2_FABRIC_PDQ_CONTEXT_NOF; context += 2)
    {
        /** Dev2 contexts are not relevant for CLOS mode */
        if (!SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit) && context == SOC_JER2_FABRIC_PDQ_CONTEXT_DEV2_HP)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(READ_IPT_PDQ_OCB_CFGr(unit, context/2, &reg64_val));
        /** Configure PDQ size */
        soc_reg64_field32_set(unit, IPT_PDQ_OCB_CFGr, &reg64_val, PDQ_OCB_N_SIZEf, pdq_size);
        /** Configure Flow Control thresholds toward the IPS */
        soc_reg64_field32_set(unit, IPT_PDQ_OCB_CFGr, &reg64_val, PDQ_OCB_N_FC_THf, 700);
        soc_reg64_field32_set(unit, IPT_PDQ_OCB_CFGr, &reg64_val, PDQ_OCB_N_EIR_FC_THf, 700);
        SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_OCB_CFGr(unit, context/2, reg64_val));
    }

    /*
     * Configure DRAM delete queue
     */
    SHR_IF_ERR_EXIT(READ_IPT_PDQ_DRAM_DELETE_CFGr(unit, &reg32_val));
    /** Configure PDQ size */
    soc_reg_field_set(unit, IPT_PDQ_DRAM_DELETE_CFGr, &reg32_val, PDQ_DRAM_DELETE_SIZEf, pdq_size);
    /** Configure Flow Control thresholds toward the IPS */
    soc_reg_field_set(unit, IPT_PDQ_DRAM_DELETE_CFGr, &reg32_val, PDQ_DRAM_DELETE_FC_THf, 900);
    SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_DRAM_DELETE_CFGr(unit, reg32_val));

    /*
     * Configure SRAM delete queue
     */
    SHR_IF_ERR_EXIT(READ_IPT_PDQ_SRAM_DELETE_CFGr(unit, &reg32_val));
    /** Configure Flow Control thresholds toward the IPS */
    soc_reg_field_set(unit, IPT_PDQ_SRAM_DELETE_CFGr, &reg32_val, PDQ_SRAM_DELETE_FC_THf, 200);
    SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_SRAM_DELETE_CFGr(unit, reg32_val));

    /*
     * Configure SRAM to DRAM queue.
     * 2 elements - HP & LP.
     * Different memory than the other configurable PDQs.
     */
    for (context = 0; context < SOC_REG_NUMELS(unit, IPT_PDQ_S_2D_CFGr); ++context)
    {
        SHR_IF_ERR_EXIT(READ_IPT_PDQ_S_2D_CFGr(unit, context, &reg32_val));
        /** Configure PDQ size */
        soc_reg_field_set(unit, IPT_PDQ_S_2D_CFGr, &reg32_val, PDQ_S_2D_N_SIZEf, 1023);
        /** Configure Flow Control thresholds toward the IPS */
        soc_reg_field_set(unit, IPT_PDQ_S_2D_CFGr, &reg32_val, PDQ_S_2D_N_FC_THf, 1023-150);
        SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_S_2D_CFGr(unit, context, reg32_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   DTQ-FC thresholds configurations.
 *   There are 2 Local DTQs and 3 Fabric DTQs.
 *   The Tx-DTQ towards the FDT/EGQ has 4 configurable thresholds, each
 *   send a different signal:
 *     Threshold 0 - signals the shaper to reduce the bandwidth.
 *     Threshold 1 - signals to stop LP queue.
 *     Threshold 2 - signals to stop HP queue.
 *     Threshold 3 - signals to stop RDF (Read Data Fifo).
 */
static shr_error_e
soc_jer2_fabric_dtq_thresholds_init(int unit)
{
    uint64 reg64_val;
    uint32 reg32_val = 0;
    int local_index = 0;
    int fabric_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64_val);

    /*
     * Configurations for Local DTQ FC Thresholds
     */
    for (local_index = 0; local_index < SOC_REG_NUMELS(unit, IPT_DTQ_LOC_FC_THr); ++local_index)
    {
        SHR_IF_ERR_EXIT(READ_IPT_DTQ_LOC_FC_THr(unit, local_index, &reg64_val));
        soc_reg64_field32_set(unit, IPT_DTQ_LOC_FC_THr, &reg64_val, DTQ_LOC_N_FC_0_THf, 80);
        soc_reg64_field32_set(unit, IPT_DTQ_LOC_FC_THr, &reg64_val, DTQ_LOC_N_FC_1_THf, 60);
        soc_reg64_field32_set(unit, IPT_DTQ_LOC_FC_THr, &reg64_val, DTQ_LOC_N_FC_2_THf, 40);
        soc_reg64_field32_set(unit, IPT_DTQ_LOC_FC_THr, &reg64_val, DTQ_LOC_N_FC_3_THf, 20);
        SHR_IF_ERR_EXIT(WRITE_IPT_DTQ_LOC_FC_THr(unit, local_index, reg64_val));
    }

    /*
     * Configurations for Fabric DTQ FC Thresholds
     */
    for (fabric_index = 0; fabric_index < SOC_REG_NUMELS(unit, IPT_DTQ_FAB_FC_THr); ++fabric_index)
    {
        SHR_IF_ERR_EXIT(READ_IPT_DTQ_FAB_FC_THr(unit, fabric_index, &reg64_val));
        soc_reg64_field32_set(unit, IPT_DTQ_FAB_FC_THr, &reg64_val, DTQ_FAB_N_FC_0_THf, 100);
        soc_reg64_field32_set(unit, IPT_DTQ_FAB_FC_THr, &reg64_val, DTQ_FAB_N_FC_1_THf, 80);
        soc_reg64_field32_set(unit, IPT_DTQ_FAB_FC_THr, &reg64_val, DTQ_FAB_N_FC_2_THf, 60);
        soc_reg64_field32_set(unit, IPT_DTQ_FAB_FC_THr, &reg64_val, DTQ_FAB_N_FC_3_THf, 40);
        SHR_IF_ERR_EXIT(WRITE_IPT_DTQ_FAB_FC_THr(unit, fabric_index, reg64_val));
    }

    /*
     * Configurations for OCB-Only DTQ FC Threshold.
     * There are only Shaper FC and PDQ FC thresholds, and anyway they
     * are not enabled since OCB-Only DTQ should be mostly empty.
     */
    for (fabric_index = 0; fabric_index < SOC_REG_NUMELS(unit, IPT_DTQ_OCB_FC_THr); ++fabric_index)
    {
        SHR_IF_ERR_EXIT(READ_IPT_DTQ_OCB_FC_THr(unit, fabric_index, &reg32_val));
        soc_reg_field_set(unit, IPT_DTQ_OCB_FC_THr, &reg32_val, DTQ_OCB_N_FC_0_THf, 40);
        soc_reg_field_set(unit, IPT_DTQ_OCB_FC_THr, &reg32_val, DTQ_OCB_N_FC_1_THf, 20);
        SHR_IF_ERR_EXIT(WRITE_IPT_DTQ_OCB_FC_THr(unit, fabric_index, reg32_val));
    }

    /*
     * The following registers are kept with their default values, so
     * no change should be applied to them, but they listed here as a note
     * that these registers configuration wan't skipped by mistake:
     * IPT_MOP_FC_CONFIGr
     * IPT_FROP_MC_LP_CONFIGr
     * IPT_PDQ_MC_THr
     */

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set PDQs WFQs configurations.
 */
static shr_error_e
soc_jer2_fabric_pdq_wfq_init(int unit)
{
    uint32 reg32_val = 0;
    uint64 reg64_val;
    int wfq = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * All the WFQs are set to '1' by default, which means they will
     * apply round-robin between their legs.
     * The WFQ between OCB and HP queues should give Strict Priority to
     * the OCB queue, so it's the only WFQ that is configured different
     * from the default.
     */
    SHR_IF_ERR_EXIT(READ_IPT_WFQ_CFGr(unit, SOC_JER2_FABRIC_WFQ_PRIO_INDEX, &reg32_val));
    soc_reg_field_set(unit, IPT_WFQ_CFGr, &reg32_val, WFQ_N_WEIGHT_0f, 0);
    soc_reg_field_set(unit, IPT_WFQ_CFGr, &reg32_val, WFQ_N_WEIGHT_1f, 1);
    SHR_IF_ERR_EXIT(WRITE_IPT_WFQ_CFGr(unit, SOC_JER2_FABRIC_WFQ_PRIO_INDEX, reg32_val));

    /** Set default packet size for CLOS WFQ */
    for (wfq = 0; wfq < SOC_REG_NUMELS(unit, IPT_WFQ_CFGr); ++wfq)
    {
        SHR_IF_ERR_EXIT(READ_IPT_WFQ_CFGr(unit, wfq, &reg32_val));
        soc_reg_field_set(unit, IPT_WFQ_CFGr, &reg32_val, WFQ_N_DEFAULT_PKT_SIZEf, SOC_JER2_FABRIC_WFQ_DFLT_PKT_SIZE);
        SHR_IF_ERR_EXIT(WRITE_IPT_WFQ_CFGr(unit, wfq, reg32_val));
    }

    /** Set default packet size for Mesh WFQ */
    COMPILER_64_ZERO(reg64_val);
    for (wfq = 0; wfq < SOC_REG_NUMELS(unit, IPT_WFQ_MESH_CFGr); ++wfq)
    {
        SHR_IF_ERR_EXIT(READ_IPT_WFQ_MESH_CFGr(unit, wfq, &reg64_val));
        soc_reg64_field32_set(unit, IPT_WFQ_MESH_CFGr, &reg64_val, WFQ_N_MESH_DEFAULT_PKT_SIZEf, SOC_JER2_FABRIC_WFQ_DFLT_PKT_SIZE);
        SHR_IF_ERR_EXIT(WRITE_IPT_WFQ_MESH_CFGr(unit, wfq, reg64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure which DTQ-FC signal will stop the read from the Read Fifos (RDF).
 *   When DTQ-FC thresholds are crossed, the DTQ send FC signals.
 *   Since there are 4 DTQ-FCs, need to configure for the RDFs which FC
 *   should cause it to stop.
 *   The relevant DTQ-FCs for RDFs are:
 *     SOC_JER2_FABRIC_DTQ_FC_ENABLE_RDF
 */
static shr_error_e
soc_jer2_fabric_rdf_correspondence_to_dtqs_fc_init(int unit)
{
    soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;
    uint32 reg32_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The Read Fifos FC is by default SOC_JER2_FABRIC_DTQ_FC_ENABLE_RDF
     * for all DTQs.
     * In device configurations where there are less DTQs, need to disable the
     * FC in the unused DTQs.
     */
    if (dnx_data_fabric.general.connect_mode_get(unit) == SOC_DNX_FABRIC_CONNECT_MODE_FE)
    {
        switch (map_type)
        {
            case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            {
                /*
                 * All DTQs are active. No need to disable any FC.
                 */
                break;
            }
            case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
            case soc_dnxc_fabric_pipe_map_dual_uc_mc:
            {
                /*
                 * Disable FC from unused Fabric2 DTQ.
                 */
                SHR_IF_ERR_EXIT(READ_IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr(unit, &reg32_val));
                soc_reg_field_set(unit, IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr, &reg32_val, READ_FIFO_MIX_FAB_2_DTQ_FC_ENABLEf, 0);
                SHR_IF_ERR_EXIT(WRITE_IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr(unit, reg32_val));

                SHR_IF_ERR_EXIT(READ_IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr(unit, &reg32_val));
                soc_reg_field_set(unit, IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr, &reg32_val, READ_FIFO_SRAM_FAB_2_DTQ_FC_ENABLEf, 0);
                SHR_IF_ERR_EXIT(WRITE_IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr(unit, reg32_val));

                break;
            }
            case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
            case soc_dnxc_fabric_pipe_map_single:
            {
                /*
                 * Disable FC from unused Fabric1 & Fabric2 DTQs.
                 */
                SHR_IF_ERR_EXIT(READ_IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr(unit, &reg32_val));
                soc_reg_field_set(unit, IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr, &reg32_val, READ_FIFO_MIX_FAB_1_DTQ_FC_ENABLEf, 0);
                soc_reg_field_set(unit, IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr, &reg32_val, READ_FIFO_MIX_FAB_2_DTQ_FC_ENABLEf, 0);
                SHR_IF_ERR_EXIT(WRITE_IPT_READ_FIFO_MIX_DTQ_FC_ENABLEr(unit, reg32_val));

                SHR_IF_ERR_EXIT(READ_IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr(unit, &reg32_val));
                soc_reg_field_set(unit, IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr, &reg32_val, READ_FIFO_SRAM_FAB_1_DTQ_FC_ENABLEf, 0);
                soc_reg_field_set(unit, IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr, &reg32_val, READ_FIFO_SRAM_FAB_2_DTQ_FC_ENABLEf, 0);
                SHR_IF_ERR_EXIT(WRITE_IPT_READ_FIFO_SRAM_DTQ_FC_ENABLEr(unit, reg32_val));

                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure PDQs global shapers.
 */
static shr_error_e
soc_jer2_fabric_pdq_global_shapers_init(int unit)
{
    SHR_BIT_DCL_CLR_NAME(mem_val, 92);
    SHR_BIT_DCL_CLR_NAME(mem_val2, 48);
    uint64 reg64_val;
    uint32 reg32_val = 0;
    uint32 global_shaper_fc_bmp = 0;
    uint32 shaper_cell_header_diff = 0;
    uint32 is_pcp_enabled = 0;
    int index = 0;
    /** Get full rate */
    uint32 shaper_rate_per_link_mbps = (dnx_data_device.general.core_clock_khz_get(unit)/1000) * dnx_data_device.general.bus_size_in_bits_get(unit) * 105 / 100;
    uint32 shaper_delay = 9;
    uint32 shaper_max_burst = 80000;
    uint32 core_clock_mhz = dnx_data_device.general.core_clock_khz_get(unit)/1000;
    uint32 shaper_cal = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The shaper rate is determined by calendar and delay.
     * Since the delay is given, we can calculate the calender by the
     * rate we want to reach.
     *
     * Shaper rate formula from register spec:
     *   Shaper Rate = (8 bit * Shaper Cal B) / ((Shaper Delay+1)* T period)
     * Shaper Delay and period are given, so need to calculate Shaper Cal
     * in order to reach the required Shaper Rate.
     */
    shaper_cal = shaper_rate_per_link_mbps / core_clock_mhz * (shaper_delay+1) / 8;

    for (index = 0; index < SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_NOF; ++index)
    {
        /*
         * Config only local shapers.
         * Fabric shapers should be disabled (default).
         */
        if (index == SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_EGQ0 || index == SOC_JER2_FABRIC_DTQ_GLOBAL_SHAPER_EGQ1)
        {
            SHR_IF_ERR_EXIT(READ_IPT_PER_GIPT_SHAPER_CFGm(unit, MEM_BLOCK_ANY, index, mem_val)); 
            soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_ENf, 1);
            soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_CALf, shaper_cal);
            soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_DELAYf, shaper_delay);
            soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_MAX_CREDITf, shaper_max_burst);
            soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_IS_SEGMENTEDf, 1);
            soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_BUNDLE_ENf, 1);
            /*
             * The following field is disabled, but we put it here in case we want to activate
             * this feature in the future:
             * soc_mem_field32_set(unit, IPT_PER_GIPT_SHAPER_CFGm, mem_val, SHAPER_FC_ENABLEf, 0);
             */
            SHR_IF_ERR_EXIT(WRITE_IPT_PER_GIPT_SHAPER_CFGm(unit, MEM_BLOCK_ANY, index, mem_val)); 
        }
    }

    /*
     * Set FC thresholds for Global Shapers.
     * Currently the FC for the Global Shapers is disabled (IPT_PER_GIPT_SHAPER_CFGm SHAPER_FC_ENABLEf),
     * but we set the thresholds in case we want to enable the FC in the future:
     * - FC_TH_1 is redundant and not in use.
     * - FC_TH_0 - send FC if the free space in the DTQ of a core is below the threshold.
     * - TOTAL_FC_TH_0 - send FC if the free space in the DTQ of BOTH cores together is
     *   below the threshold.
     */
    for (index = 0; index < SOC_JER2_FABRIC_DTQ_SHAPERS_FC_NOF; ++index)
    {
        SHR_IF_ERR_EXIT(READ_IPT_PER_DTQ_GIPT_CFGm(unit, MEM_BLOCK_ANY, index, mem_val2)); 
        soc_mem_field32_set(unit, IPT_PER_DTQ_GIPT_CFGm, mem_val2, DTQ_FC_TH_0f, 50);
        soc_mem_field32_set(unit, IPT_PER_DTQ_GIPT_CFGm, mem_val2, DTQ_FC_TH_1f, 0);
        soc_mem_field32_set(unit, IPT_PER_DTQ_GIPT_CFGm, mem_val2, DTQ_TOTAL_FC_TH_0f, 50);
        soc_mem_field32_set(unit, IPT_PER_DTQ_GIPT_CFGm, mem_val2, DTQ_TOTAL_FC_TH_1f, 0);
        SHR_IF_ERR_EXIT(WRITE_IPT_PER_DTQ_GIPT_CFGm(unit, MEM_BLOCK_ANY, index, mem_val2)); 
    }

    /*
     * Enable PDQs Global Shaper FC.
     * Enable bit per context.
     */
    COMPILER_64_ZERO(reg64_val);
    SHR_IF_ERR_EXIT(READ_IPT_PDQ_GLB_SHAPER_FC_CONFIGr(unit, &reg64_val));
    SHR_BITSET_RANGE(&global_shaper_fc_bmp, 0, SOC_JER2_FABRIC_PDQ_CONTEXT_NOF);
    soc_reg64_field32_set(unit, IPT_PDQ_GLB_SHAPER_FC_CONFIGr, &reg64_val, PDQ_GLB_SHAPER_FC_ENABLEf, global_shaper_fc_bmp);
    SHR_IF_ERR_EXIT(WRITE_IPT_PDQ_GLB_SHAPER_FC_CONFIGr(unit, reg64_val));

    /*
     * Shapers general configurations
     */
    /** Calculate cell header compensation */
    is_pcp_enabled = dnx_data_fabric.cell.pcp_enable_get(unit);
    switch (dnx_data_fabric.pipes.nof_pipes_get(unit))
    {
        case 1:
        {
            shaper_cell_header_diff = is_pcp_enabled ? 11 : 10;
            break;
        }
        case 2:
        {
            shaper_cell_header_diff = is_pcp_enabled ? 13 : 12;
            break;
        }
        case 3:
        {
            shaper_cell_header_diff = is_pcp_enabled ? 16 : 15;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric number of pipes is not supported");
            break;
        }
    }

    SHR_IF_ERR_EXIT(READ_IPT_SHAPER_GENERAL_CONFIGr(unit, &reg32_val));
    soc_reg_field_set(unit, IPT_SHAPER_GENERAL_CONFIGr, &reg32_val, ZERO_ACT_LINKS_RATE_ENf, 1);
    soc_reg_field_set(unit, IPT_SHAPER_GENERAL_CONFIGr, &reg32_val, GIPT_ACT_LINKS_SHP_ENf, 1);
    if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
    {
        soc_reg_field_set(unit, IPT_SHAPER_GENERAL_CONFIGr, &reg32_val, ACT_LINKS_MC_SHP_ENf, 1);
        soc_reg_field_set(unit, IPT_SHAPER_GENERAL_CONFIGr, &reg32_val, MESH_FMC_NUM_LINKS_MAX_ENf, 1);
    }
    soc_reg_field_set(unit, IPT_SHAPER_GENERAL_CONFIGr, &reg32_val, SHAPER_CELL_HDR_DIFFf, shaper_cell_header_diff);
    SHR_IF_ERR_EXIT(WRITE_IPT_SHAPER_GENERAL_CONFIGr(unit, reg32_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Configure Read Request FIFOs (RRF).
 */
static shr_error_e
soc_jer2_fabric_rrf_init(int unit)
{
    soc_reg_above_64_val_t reg_above64_val;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    /*
     * Set Flow Control hysteresis thresholds from RRF to PDQs.
     */
    SHR_IF_ERR_EXIT(READ_IPT_RRF_FC_THr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_SRAM_HP_FC_HIGH_THf, SOC_JER2_FABRIC_RRF_SRAM_HP_FC_HIGH_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_SRAM_HP_FC_LOW_THf, SOC_JER2_FABRIC_RRF_SRAM_HP_FC_LOW_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_SRAM_LP_FC_HIGH_THf, SOC_JER2_FABRIC_RRF_SRAM_LP_FC_HIGH_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_SRAM_LP_FC_LOW_THf, SOC_JER2_FABRIC_RRF_SRAM_LP_FC_LOW_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_OCBO_FC_HIGH_THf, 70);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_OCBO_FC_LOW_THf, 70);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXS_HP_FC_HIGH_THf, SOC_JER2_FABRIC_RRF_MIXS_HP_FC_HIGH_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXS_HP_FC_LOW_THf, SOC_JER2_FABRIC_RRF_MIXS_HP_FC_LOW_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXS_LP_FC_HIGH_THf, SOC_JER2_FABRIC_RRF_MIXS_LP_FC_HIGH_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXS_LP_FC_LOW_THf, SOC_JER2_FABRIC_RRF_MIXS_LP_FC_LOW_TH);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXD_HP_FC_HIGH_THf, 90);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXD_HP_FC_LOW_THf, 70);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXD_LP_FC_HIGH_THf, 80);
    soc_reg_above_64_field32_set(unit, IPT_RRF_FC_THr, reg_above64_val, RRF_MIXD_LP_FC_LOW_THf, 60);
    SHR_IF_ERR_EXIT(WRITE_IPT_RRF_FC_THr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Initialize DTQs contexts:
 *     1. Set which DTQs are active.
 *     2. Set the size of each DTQ.
 *   There are 6 DTQs - MIXED+OCB for each of the 3 Fabric DTQs.
 *   The local DTQs are inside the FDA, and their size is fixed.
 */
shr_error_e
soc_jer2_dtqs_contexts_init(int unit)
{
    soc_reg_t dtq_base_address_regs[] = {FDT_DTQ_BADDR_0r, FDT_DTQ_BADDR_1r, FDT_DTQ_BADDR_2r, FDT_DTQ_BADDR_3r, FDT_DTQ_BADDR_4r, FDT_DTQ_BADDR_5r};
    soc_reg_t dtq_base_address_fields[] = {DTQ_BADDR_0f, DTQ_BADDR_1f, DTQ_BADDR_2f, DTQ_BADDR_3f, DTQ_BADDR_4f, DTQ_BADDR_5f};
    uint32 reg32_val = 0;
    uint32 mixed_dtq_size = 0;
    uint32 nof_active_ocb_dtqs = 0;
    uint32 nof_active_mixed_dtqs = 0;
    uint32 nof_active_dtqs = 0;
    int dtq = 0;
    uint32 dtq_start_addr = 0;
    uint32 dtq_en = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set number of active DTQs
     */
    if (SOC_DNX_FABRIC_CONNECT_MODE_MESH_OR_SINGLE_FAP(unit))
    {
        /** (Dest1, Dest2, Dest3/MC) x (MIXED, OCB) */
        nof_active_dtqs = SOC_JER2_FABRIC_DTQ_NOF;
    }
    else
    {
        soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;

        switch (map_type)
        {
            case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            {
                /** (FUC, FMC-H, FMC-L) x (MIXED, OCB) */
                nof_active_dtqs = SOC_JER2_FABRIC_DTQ_NOF;

                break;
            }
            case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
            case soc_dnxc_fabric_pipe_map_dual_uc_mc:
            {
                /** (FUC, FMC) x (MIXED, OCB) */
                nof_active_dtqs = SOC_JER2_FABRIC_DTQ_NOF - 2;

                break;
            }
            case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
            case soc_dnxc_fabric_pipe_map_single:
            {
                /** (FUC+FMC) x (MIXED, OCB) */
                nof_active_dtqs = SOC_JER2_FABRIC_DTQ_NOF - 4;

                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "fabric pipes map type is not supported");
                break;
            }
        }
    }

    /*
     * OCB-Only sub-DTQs are configured with fixed size.
     * Mixed sub-DTQs are configured equally with the remainings.
     */
    nof_active_ocb_dtqs = nof_active_dtqs/2;
    nof_active_mixed_dtqs = nof_active_dtqs/2;
    mixed_dtq_size = (SOC_JER2_FABRIC_DTQ_MAX_SIZE - (nof_active_ocb_dtqs*SOC_JER2_FABRIC_DTQ_OCB_ONLY_SIZE)) / nof_active_mixed_dtqs;

    /*
     * The size of the DTQ is defined until the start of the next DTQ
     */
    dtq_start_addr = 0;
    for (dtq = 0; dtq < nof_active_dtqs; ++dtq)
    {
        soc_reg32_get(unit, dtq_base_address_regs[dtq], REG_PORT_ANY, 0, &reg32_val);
        soc_reg_field_set(unit, dtq_base_address_regs[dtq], &reg32_val, dtq_base_address_fields[dtq], dtq_start_addr);
        soc_reg32_set(unit, dtq_base_address_regs[dtq], REG_PORT_ANY, 0, reg32_val);

        /** The DTQs are ordered by OCB/MIXED alternatively, where the first DTQ is MIXED */
        dtq_start_addr += (dtq % 2) ? SOC_JER2_FABRIC_DTQ_OCB_ONLY_SIZE : mixed_dtq_size;
    }

    /*
     * The last DTQ needs a specific register to tell where it ends
     */
    SHR_IF_ERR_EXIT(READ_FDT_DTQ_EADDR_5r(unit, &reg32_val));
    soc_reg_field_set(unit, FDT_DTQ_EADDR_5r, &reg32_val, DTQ_EADDR_5f, SOC_JER2_FABRIC_DTQ_MAX_SIZE - 1);
    SHR_IF_ERR_EXIT(WRITE_FDT_DTQ_EADDR_5r(unit, reg32_val));

    /*
     * Enable DTQs.
     * Each bit represent a DTQ to enable.
     */
    SHR_IF_ERR_EXIT(READ_FDT_DTQ_ENr(unit, &reg32_val)); 
    SHR_BITSET_RANGE(&dtq_en, 0, nof_active_dtqs);
    soc_reg_field_set(unit, FDT_DTQ_ENr, &reg32_val, DTQ_ENf, dtq_en);
    SHR_IF_ERR_EXIT(WRITE_FDT_DTQ_ENr(unit, reg32_val)); 

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Init configurations regarding IPT flow control debug
 */
static shr_error_e
soc_jer2_fabric_flow_control_debug_init(int unit)
{
    soc_reg_above_64_val_t reg_above64_val;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

    /*
     * Make flow control sticky in the debug register, meaning that once
     * flow control is set it will stay set in the debug register until
     * reading the register.
     * Was asked by ASIC for easier debugging.
     */
    SHR_IF_ERR_EXIT(READ_IPT_FLOW_CONTROL_DEBUGr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, IPT_FLOW_CONTROL_DEBUGr, reg_above64_val, STICKY_SELECTf, 1);
    SHR_IF_ERR_EXIT(WRITE_IPT_FLOW_CONTROL_DEBUGr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
soc_jer2_fabric_transmit_init(int unit)
{
    uint32 reg32 = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*Enable GCI*/
    SHR_IF_ERR_EXIT(READ_IPT_IPT_DEBUG_GCI_CONTROLSr(unit, &reg32));
    soc_reg_field_set(unit, IPT_IPT_DEBUG_GCI_CONTROLSr, &reg32, GCI_ENf, 0x1);
    soc_reg_field_set(unit, IPT_IPT_DEBUG_GCI_CONTROLSr, &reg32, GCI_CNT_SELf, 0x4);
    SHR_IF_ERR_EXIT(WRITE_IPT_IPT_DEBUG_GCI_CONTROLSr(unit, reg32));

    /** Configure DTQs FC thresholds */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_dtq_thresholds_init(unit));

    /** Configure PDQs contexts */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_pdqs_contexts_init(unit));

    /** Set which DTQ-FC affect each PDQ */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_pdqs_correspondence_to_dtqs_fc_init(unit));

    /** Set which DTQ-FC affect RDFs */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_rdf_correspondence_to_dtqs_fc_init(unit));

    /** Configure PDQs global shapers */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_pdq_global_shapers_init(unit));

    /** Configure PDQs WFQs */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_pdq_wfq_init(unit));

    /** Configure RRF */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_rrf_init(unit));
    
    /** Configure flow control debug settings */
    SHR_IF_ERR_EXIT(soc_jer2_fabric_flow_control_debug_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/* } */

#undef BSL_LOG_MODULE

