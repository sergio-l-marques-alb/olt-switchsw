/*
 * $Id: diag_dnx_diag.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    diag_dnx_diag.c
 * Purpose:    Routines for handling misc diagnostic
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
#include <soc/counter.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif

/* Local includes */
#include "diag_dnx_diag.h"

/*************
 *  MACROES  *
 *************/
/** Length of block name */
#define DNX_DIAG_BLOCK_NAME_LENGTH 6
/** Maximum index of block */
#define DNX_DIAG_BLOCK_MAX_INDEX 28
/** Length of graphical print buffer */
#define DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH 50
/** a flag which indicates that register has replication register in CDUM */
#define DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS (0x1)
#define DNX_DIAG_COUNTER_REG_NOF_REGS (4)
#define COUNTER_GRAPHIC_BLOCK_SET(_block, _counter_table, _blk_name)              \
{                                                                                 \
    _block->counter_table = _counter_table;                                     \
    sal_snprintf(_block->block_name, DNX_DIAG_BLOCK_NAME_LENGTH, "%s", _blk_name);         \
    _block->num_entries = sizeof(_counter_table)/sizeof(sh_dnx_counter_register_t); \
}

/*************
 *  DEFINES  *
 *************/
typedef struct sh_dnx_counter_graphical_options_s
{
    int cdsp;
    int non_zero;
    int in_hex;
    uint32 interval;
} sh_dnx_counter_graphical_options_t;

typedef struct sh_dnx_counter_register_s
{
    char *reg_name;
    soc_block_type_t block[DNX_DIAG_COUNTER_REG_NOF_REGS];
    soc_reg_t reg_arr[DNX_DIAG_COUNTER_REG_NOF_REGS];
    unsigned reg_index;         /* register index, should be 0 when not a register array. */
    soc_field_t cnt_field[DNX_DIAG_COUNTER_REG_NOF_REGS];
    soc_field_t overflow_field;
    uint32 flags;
} sh_dnx_counter_register_t;

typedef struct sh_dnx_counter_graphical_block_s
{
    sh_dnx_counter_register_t *counter_table;   /* pointer to one counter table of one block */
    char block_name[DNX_DIAG_BLOCK_NAME_LENGTH];
    int num_entries;            /* Indicate how many counters are there in this counter table */
} sh_dnx_counter_graphical_block_t;

typedef struct reason_buffer_s
{
    char buffer[DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH];
} reason_buffer_t;

typedef struct counter_packet_dropped_reason_s
{
    int nof_reason_cgm;
    int nof_reason_rqp;
    int nof_reason_pqp;
    reason_buffer_t *reason_buffer_cgm;
    reason_buffer_t *reason_buffer_rqp;
    reason_buffer_t *reason_buffer_pqp;
} counter_packet_dropped_reason_t;

typedef enum counter_graphic_blk_idx_s
{
    COUNTER_GRAPHIC_BLK_1ST = 0,
    COUNTER_GRAPHIC_BLK_2ND,
    COUNTER_GRAPHIC_BLK_3RD,
    COUNTER_GRAPHIC_BLK_4TH,
    COUNTER_GRAPHIC_BLK_5TH,
    COUNTER_GRAPHIC_BLK_6TH,
    COUNTER_GRAPHIC_BLK_MAX
} counter_graphic_blk_idx_t;

typedef enum counter_graphic_core_idx_s
{
    COUNTER_GRAPHIC_CORE_1ST = 0,
    COUNTER_GRAPHIC_CORE_2ND,
    COUNTER_GRAPHIC_CORE_NUM
} counter_graphic_core_idx_t;

/*************
 * GLOBALS   *
 *************/
sh_dnx_counter_register_t sh_dnx_counter_graphic_cdu_rx_jer2[] = {
    {
     "RX_NUM_TOTAL_DROPPED_EOPS",
     {SOC_BLK_CDU, SOC_BLK_CDUM},
     {CDU_RX_NUM_TOTAL_DROPPED_EOPSr, CDUM_RX_NUM_TOTAL_DROPPED_EOPSr, INVALIDr, INVALIDr}, 0,
     {RX_NUM_TOTAL_DROPPED_EOPSf, RX_NUM_TOTAL_DROPPED_EOPSf},
     INVALIDf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS},
    {
     "RX_TOTAL_BYTE_COUNTER",
     {SOC_BLK_CDU, SOC_BLK_CDUM},
     {CDU_RX_TOTAL_BYTE_COUNTERr, CDUM_RX_TOTAL_BYTE_COUNTERr, INVALIDr, INVALIDr}, 0,
     {RX_TOTAL_BYTE_COUNTERf, RX_TOTAL_BYTE_COUNTERf},
     INVALIDf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS},
    {
     "RX_TOTAL_PKT_COUNTER",
     {SOC_BLK_CDU, SOC_BLK_CDUM},
     {CDU_RX_TOTAL_PKT_COUNTERr, CDUM_RX_TOTAL_PKT_COUNTERr, INVALIDr, INVALIDr}, 0,
     {RX_TOTAL_PKT_COUNTERf, RX_TOTAL_PKT_COUNTERf},
     INVALIDf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_cdu_tx_jer2[] = {
    {
     "TX_TOTAL_BYTE_COUNTER",
     {SOC_BLK_CDU, SOC_BLK_CDUM},
     {CDU_TX_TOTAL_BYTE_COUNTERr, CDUM_TX_TOTAL_BYTE_COUNTERr, INVALIDr, INVALIDr}, 0,
     {TX_TOTAL_BYTE_COUNTERf, TX_TOTAL_BYTE_COUNTERf},
     INVALIDf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS},
    {
     "TX_TOTAL_PKT_COUNTER",
     {SOC_BLK_CDU, SOC_BLK_CDUM},
     {CDU_TX_TOTAL_PKT_COUNTERr, CDUM_TX_TOTAL_PKT_COUNTERr, INVALIDr, INVALIDr}, 0,
     {TX_TOTAL_PKT_COUNTERf, TX_TOTAL_PKT_COUNTERf},
     INVALIDf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_ire_jer2[] = {
    {
     "FDT_CREDITS",
     {SOC_BLK_IRE},
     {IRE_FDT_INTERFACE_COUNTERr}, 0,
     {FDT_CREDITSf},
     INVALIDf,
     0},
    {
     "CPU_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_CPU_PACKET_COUNTERr}, 0,
     {CPU_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "OLP_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_OLP_PACKET_COUNTERr}, 0,
     {OLP_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "NIF_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_NIF_PACKET_COUNTERr}, 0,
     {NIF_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "OAMP_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_OAMP_PACKET_COUNTERr}, 0,
     {OAMP_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "RCYH_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_RCYH_PACKET_COUNTERr}, 0,
     {RCYH_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "RCYL_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_RCYL_PACKET_COUNTERr}, 0,
     {RCYL_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "SAT_PACKET_COUNTER",
     {SOC_BLK_IRE},
     {IRE_SAT_PACKET_COUNTERr}, 0,
     {SAT_PACKET_COUNTERf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_epni_jer2[] = {
    {
     "EPNI_DISC_CNT",
     {SOC_BLK_EPNI},
     {EPNI_EPNI_DISC_CNTr}, 0,
     {EPNI_DISC_CNTf},
     INVALIDf,
     0},
    {
     "EPNI_IFC_CNT",
     {SOC_BLK_EPNI},
     {EPNI_EPNI_IFC_CNTr}, 0,
     {EPNI_IFC_CNTf},
     INVALIDf,
     0},
    {
     "EPNI_MIRR_CNT",
     {SOC_BLK_EPNI},
     {EPNI_EPNI_MIRR_CNTr}, 0,
     {EPNI_MIRR_CNTf},
     INVALIDf,
     0},
    {
     "EPNI_Q_CNT",
     {SOC_BLK_EPNI},
     {EPNI_EPNI_Q_CNTr}, 0,
     {EPNI_Q_CNTf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_spb_jer2[] = {
    {
     "CGM_REPORT_COUNTER",
     {SOC_BLK_SPB},
     {SPB_BRC_DEBUGr}, 0,
     {CGM_REPORT_COUNTERf},
     INVALIDf,
     0},
    {
     "IRE_PACKETS",
     {SOC_BLK_SPB},
     {SPB_PEC_DEBUG_2r}, 0,
     {IRE_PACKETSf},
     INVALIDf,
     0},
    {
     "DDP_PACKETS",
     {SOC_BLK_SPB},
     {SPB_PTC_S_2D_DEBUGr}, 0,
     {DDP_PACKETSf},
     INVALIDf,
     0},
    {
     "IPT_PACKETS",
     {SOC_BLK_SPB},
     {SPB_PTC_S_2F_DEBUGr}, 0,
     {IPT_PACKETSf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_ddp_jer2[] = {
    {
     "PACKET_DISCARD",
     {SOC_BLK_DDP},
     {DDP_ITE_DEBUGr}, 0,
     {PACKET_DISCARDf},
     INVALIDf,
     0},
    {
     "SPB_GOOD_PACKET_CNT",
     {SOC_BLK_DDP},
     {DDP_PACKET_ERROR_COUNTERSr}, 0,
     {SPB_GOOD_PACKET_CNTf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_cgm_jer2[] = {
    {
     "VOQ_SRAM_DEL_PKT_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_SRAM_DEL_PKT_CTRr}, 0,
     {VOQ_SRAM_DEL_PKT_CTRf},
     INVALIDf,
     0},
    {
     "VOQ_SRAM_DEQ_TO_DRAM_PKT_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRr}, 0,
     {VOQ_SRAM_DEQ_TO_DRAM_PKT_CTRf},
     INVALIDf,
     0},
    {
     "VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRr}, 0,
     {VOQ_SRAM_DEQ_TO_FABRIC_PKT_CTRf},
     INVALIDf,
     0},
    {
     "VOQ_SRAM_ENQ_PKT_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_SRAM_ENQ_PKT_CTRr}, 0,
     {VOQ_SRAM_ENQ_PKT_CTRf},
     INVALIDf,
     0},
    {
     "VOQ_SRAM_ENQ_RJCT_PKT_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_SRAM_ENQ_RJCT_PKT_CTRr}, 0,
     {VOQ_SRAM_ENQ_RJCT_PKT_CTRf},
     INVALIDf,
     0},
    {
     "VOQ_DRAM_DEL_BUNDLE_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_DRAM_DEL_BUNDLE_CTRr}, 0,
     {VOQ_DRAM_DEL_BUNDLE_CTRf},
     INVALIDf,
     0},
    {
     "VOQ_DRAM_DEQ_TO_FABRIC_BUNDLE_CTR",
     {SOC_BLK_CGM},
     {CGM_VOQ_DRAM_DEQ_TO_FABRIC_BUNDLE_CTRr}, 0,
     {VOQ_DRAM_DEQ_TO_FABRIC_BUNDLE_CTRf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_ipt_jer2[] = {
    {
     "CGM_RXI_PACKET_COUNT",
     {SOC_BLK_IPT},
     {IPT_PDQ_DEBUGr}, 0,
     {CGM_RXI_PACKET_COUNTf},
     CELL_IN_CNT_Of,
     0},
    {
     "SPB_PACKET_COUNT",
     {SOC_BLK_IPT},
     {IPT_SRAM_RRF_GLB_DEBUGr}, 0,
     {SPB_PACKET_COUNTf},
     INVALIDf,
     0},
    {
     "DDP_TX_BUFFER_COUNTER",
     {SOC_BLK_IPT},
     {IPT_SRAM_RRF_GLB_DEBUGr}, 0,
     {DDP_TX_BUFFER_COUNTERf},
     INVALIDf,
     0},
    {
     "PACKET_DISCARD",
     {SOC_BLK_IPT},
     {IPT_ITE_DEBUGr}, 0,
     {PACKET_DISCARDf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_fdr_jer2[] = {
    {
     "P_1_CELL_IN_CNT",
     {SOC_BLK_FDR},
     {FDR_P_CELL_IN_CNTr}, 1,
     {P_N_CELL_IN_CNTf},
     INVALIDf,
     0},
    {
     "P_2_CELL_IN_CNT",
     {SOC_BLK_FDR},
     {FDR_P_CELL_IN_CNTr}, 2,
     {P_N_CELL_IN_CNTf},
     INVALIDf,
     0},
    {
     "P_3_CELL_IN_CNT",
     {SOC_BLK_FDR},
     {FDR_P_CELL_IN_CNTr}, 3,
     {P_N_CELL_IN_CNTf},
     INVALIDf,
     0},
    {
     "CELL_IN_CNT_TOTAL",
     {SOC_BLK_FDR},
     {FDR_CELL_IN_CNT_TOTALr}, 0,
     {CELL_IN_CNTf},
     CELL_IN_CNT_Of,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_fdt_jer2[] = {
    {
     "UNRCH_DEST_CNT_N",
     {SOC_BLK_FDT},
     {FDT_UNREACHABLE_DESTINATION_DISCARDED_CELLS_COUNTERr}, 0,
     {UNRCH_DEST_CNT_Nf},
     CELL_IN_CNT_Of,
     0},
    {
     "FDT_FABRIC_CELL_COUNT",
     {SOC_BLK_FDT},
     {FDT_FABRIC_CELLS_CNTr}, 0,
     {FABRIC_CELL_CORE_N_CELL_CNTf},
     INVALIDf,
     0},
    {
     "FDT_MESH_CELL_COUNT",
     {SOC_BLK_FDT},
     {FDT_FABRIC_CELLS_CNTr, FDT_FABRIC_CELLS_CNTr, INVALIDr, INVALIDr}, 0,
     {MESH_DEST_1_CORE_N_CELL_CNTf, MESH_DEST_2_CORE_N_CELL_CNTf},
     INVALIDf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_rqp_jer2[] = {
    {
     "CDC_FABRIC_PACKET_CNT",
     {SOC_BLK_RQP},
     {RQP_CELL_DECODER_DEBUG_COUNTERSr}, 0,
     {CDC_FABRIC_PACKET_CNTf},
     INVALIDf,
     0},
    {
     "CDC_IPT_PACKET_CNT",
     {SOC_BLK_RQP},
     {RQP_CELL_DECODER_DEBUG_COUNTERSr}, 0,
     {CDC_IPT_PACKET_CNTf},
     INVALIDf,
     0},
    {
     "CDC_MC_MESH_PACKET_CNT",
     {SOC_BLK_RQP},
     {RQP_CELL_DECODER_DEBUG_COUNTERSr}, 0,
     {CDC_MC_MESH_PACKET_CNTf},
     INVALIDf,
     0},
    {
     "CDC_TDM_PACKET_CNT",
     {SOC_BLK_RQP},
     {RQP_CELL_DECODER_DEBUG_COUNTERSr}, 0,
     {CDC_TDM_PACKET_CNTf},
     INVALIDf,
     0},
    {
     "EMR_DISCARDS_PACKET_COUNTER",
     {SOC_BLK_RQP},
     {RQP_EMR_DISCARDS_PACKET_COUNTERr}, 0,
     {EMR_DISCARDS_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "EMR_MC_HIGH_PACKET_COUNTER",
     {SOC_BLK_RQP},
     {RQP_EMR_MC_HIGH_PACKET_COUNTERr}, 0,
     {EMR_MC_HIGH_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "EMR_MC_LOW_PACKET_COUNTER",
     {SOC_BLK_RQP},
     {RQP_EMR_MC_LOW_PACKET_COUNTERr}, 0,
     {EMR_MC_LOW_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "EMR_UNICAST_PACKET_COUNTER",
     {SOC_BLK_RQP},
     {RQP_EMR_UNICAST_PACKET_COUNTERr}, 0,
     {EMR_UNICAST_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "EMR_TDM_PACKET_COUNTER",
     {SOC_BLK_RQP},
     {RQP_EMR_TDM_PACKET_COUNTERr}, 0,
     {EMR_TDM_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "PRP_PACKET_IN_CNT",
     {SOC_BLK_RQP},
     {RQP_PRP_DEBUG_COUNTERSr}, 0,
     {PRP_PACKET_IN_CNTf},
     INVALIDf,
     0},
    {
     "PRP_SOP_DISCARD_MC_CNT",
     {SOC_BLK_RQP},
     {RQP_PRP_DEBUG_COUNTERSr}, 0,
     {PRP_SOP_DISCARD_MC_CNTf},
     INVALIDf,
     0},
    {
     "PRP_SOP_DISCARD_TDM_CNT",
     {SOC_BLK_RQP},
     {RQP_PRP_DEBUG_COUNTERSr}, 0,
     {PRP_SOP_DISCARD_TDM_CNTf},
     INVALIDf,
     0},
    {
     "PRP_SOP_DISCARD_UC_CNT",
     {SOC_BLK_RQP},
     {RQP_PRP_DEBUG_COUNTERSr}, 0,
     {PRP_SOP_DISCARD_UC_CNTf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_pqp_jer2[] = {
    {
     "VISIBILITY_PACKET_COUNTER",
     {SOC_BLK_PQP},
     {PQP_VISIBILITY_PACKET_COUNTERr}, 0,
     {VISIBILITY_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "PQP_MULTICAST_PACKET_COUNTER",
     {SOC_BLK_PQP},
     {PQP_PQP_MULTICAST_PACKET_COUNTERr}, 0,
     {PQP_MULTICAST_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "PQP_UNICAST_PACKET_COUNTER",
     {SOC_BLK_PQP},
     {PQP_PQP_UNICAST_PACKET_COUNTERr}, 0,
     {PQP_UNICAST_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "PQP_DISCARD_MULTICAST_PACKET_COUNTER",
     {SOC_BLK_PQP},
     {PQP_PQP_DISCARD_MULTICAST_PACKET_COUNTERr}, 0,
     {PQP_DISCARD_MULTICAST_PACKET_COUNTERf},
     INVALIDf,
     0},
    {
     "PQP_DISCARD_UNICAST_PACKET_COUNTER",
     {SOC_BLK_PQP},
     {PQP_PQP_DISCARD_UNICAST_PACKET_COUNTERr}, 0,
     {PQP_DISCARD_UNICAST_PACKET_COUNTERf},
     INVALIDf,
     0}
};

sh_dnx_counter_register_t sh_dnx_counter_graphic_fda_jer2[] = {
    {
     "FDA_OUT_FABRIC",
     {SOC_BLK_FDA},
     {FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_0r, FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_1r, FDA_EGQ_CELLS_OUT_CNT_FAB_PIPE_2r,
      INVALIDr}, 0,
     {EGQ_N_CELLS_OUT_CNT_FAB_PIPE_0f, EGQ_N_CELLS_OUT_CNT_FAB_PIPE_1f, EGQ_N_CELLS_OUT_CNT_FAB_PIPE_2f},
     EGQ_N_CELLS_OUT_CNT_FAB_PIPE_0_OVERFLOWf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS},
    {
     "FDA_OUT_LOCAL_UC",
     {SOC_BLK_FDA},
     {FDA_EGQ_CELLS_OUT_CNT_IPT_0r, FDA_EGQ_CELLS_OUT_CNT_IPT_1r, INVALIDr, INVALIDr}, 0,
     {EGQ_N_CELLS_OUT_CNT_IPT_0f, EGQ_N_CELLS_OUT_CNT_IPT_1f},
     EGQ_N_CELLS_OUT_CNT_IPT_0_OVERFLOWf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS},
    {
     "FDA_OUT_LOCAL_MC_0",
     {SOC_BLK_FDA},
     {FDA_EGQ_CELLS_OUT_CNT_MESHMC_0r, FDA_EGQ_CELLS_OUT_CNT_MESHMC_1r, INVALIDr, INVALIDr}, 0,
     {EGQ_N_CELLS_OUT_CNT_MESHMC_0f, EGQ_N_CELLS_OUT_CNT_MESHMC_1f},
     EGQ_N_CELLS_OUT_CNT_MESHMC_0_OVERFLOWf,
     DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS}
};

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief
 *   To check how many blocks are loaded in the counter_block_group 
 * \param [in] unit - Unit #
 * \param [in] blk_group - to specify which block group to check
 * \param [out] nof_valid_block - a pointer to return number of valid blocks
 * \return 
 *      Standard error handling
 */
static shr_error_e
counter_graphical_valid_block_get(
    int unit,
    sh_dnx_counter_graphical_block_t * blk_group,
    int *nof_valid_block)
{
    int i;

    for (i = 0; i < COUNTER_GRAPHIC_BLK_MAX; i++)
    {
        /*
         * block is valid if it has at least one counter register 
         */
        if (0 == blk_group[i].num_entries)
        {
            break;
        }
    }
    *nof_valid_block = i;

    return _SHR_E_NONE;
}

static shr_error_e
dnx_diag_overflow_check(
    int unit,
    int len,
    soc_reg_above_64_val_t temp_fld_value,
    int *overflow)
{
    if ((len > 64) && (temp_fld_value[2] > 0))
    {
        *overflow = TRUE;
    }
    if ((len > 96) && (temp_fld_value[3] > 0))
    {
        *overflow = TRUE;
    }
    return _SHR_E_NONE;
}

/**
 * \brief
 *   To calculate how many rows to print out in one counter graphical section 
 * \param [in] unit - Unit #
 * \param [in] blk_group_left - a pointer to specify the left block group data
 * \param [in] blk_group_right - a pointer to specify the right block group data
 * \param [in] nof_block_left - to specify number of blocks in the left block group
 * \param [in] nof_block_right - to specify number of blocks in the right block group
 * \param [out] row_of_section - a pointer to return row of this block section, which is 
 *              MAX(row of left, row of right), row of left/right is number of all the counters
 *              in all the blocks + number of parting lines which is number of blocks - 1.
 * \return 
 *      Standard error handling
 */
static shr_error_e
counter_graphical_section_row_get(
    int unit,
    sh_dnx_counter_graphical_block_t * blk_group_left,
    sh_dnx_counter_graphical_block_t * blk_group_right,
    int nof_block_left,
    int nof_block_right,
    int *row_of_section)
{
    int i;
    int row_of_group_left = 0, row_of_group_right = 0;

    if (nof_block_left > COUNTER_GRAPHIC_BLK_MAX || nof_block_right > COUNTER_GRAPHIC_BLK_MAX)
    {
        LOG_CLI(("Invalid parameter: nof_block_left is %d, nof_block_right is %d \n", nof_block_left, nof_block_right));
        return _SHR_E_PARAM;
    }

    /*
     * calculate the row of the left group 
     */
    for (i = 0; i < nof_block_left; i++)
    {
        if (0 != i)
        {
            /*
             * add one row for the parting line 
             */
            row_of_group_left++;
        }
        row_of_group_left += blk_group_left[i].num_entries;
    }
    /*
     * calculate the row of the right group 
     */
    for (i = 0; i < nof_block_right; i++)
    {
        if (0 != i)
        {
            /*
             * add one row for the parting line 
             */
            row_of_group_right++;
        }
        row_of_group_right += blk_group_right[i].num_entries;
    }
    /*
     * the row of section is the max between row_of_group_left and row_of_group_right 
     */
    *row_of_section = (row_of_group_left > row_of_group_right) ? row_of_group_left : row_of_group_right;

    return _SHR_E_NONE;
}

/**
 * \brief
 *   to print out the reasons for dropped and rejected packers in block  
 *   CGM, RQP and PQP.
 * \param [in] unit - Unit #
 * \param [in] cdsp - indicate if reasons were displayed per core
 * \param [in] prt_ctr - print control pointer
 * \param [in] sand_control - pointer to framework control structure
 * \return 
 *      Standard error handling
 */
static shr_error_e
sh_dnx_packet_dropped_reason_print(
    int unit,
    int cdsp,
    prt_control_t * prt_ctr,
    sh_sand_control_t * sand_control)
{
    int field_len = 0, nof_cores = 0;
    int i_fld = 0, i_bit = 0, i_reason = 0, i_core = 0;
    int nof_reason = 0, nof_reason_cgm = 0, nof_reason_rqp = 0, nof_reason_pqp = 0;
    uint32 bit_mask = 0;
    soc_reg_t reg;
    soc_field_t field;
    soc_field_info_t *fldinfo;
    soc_reg_info_t *reginfo;
    soc_reg_above_64_val_t reg_value[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    soc_reg_above_64_val_t fld_value[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    counter_packet_dropped_reason_t reason[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];

    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    sal_memset(reason, 0, sizeof(counter_packet_dropped_reason_t) * DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        nof_reason = dnx_data_ingr_congestion.info.nof_dropped_reasons_cgm_get(unit);
        reason[i_core].reason_buffer_cgm = sal_alloc(nof_reason * sizeof(reason_buffer_t), "alloc reason buffer");
        sal_memset(reason[i_core].reason_buffer_cgm, 0, nof_reason * sizeof(reason_buffer_t));
        nof_reason = dnx_data_ecgm.info.nof_dropped_reasons_rqp_get(unit);
        reason[i_core].reason_buffer_rqp = sal_alloc(nof_reason * sizeof(reason_buffer_t), "alloc reason buffer");
        sal_memset(reason[i_core].reason_buffer_rqp, 0, nof_reason * sizeof(reason_buffer_t));
        nof_reason = dnx_data_ecgm.info.nof_dropped_reasons_pqp_get(unit);
        reason[i_core].reason_buffer_pqp = sal_alloc(nof_reason * sizeof(reason_buffer_t), "alloc reason buffer");
        sal_memset(reason[i_core].reason_buffer_pqp, 0, nof_reason * sizeof(reason_buffer_t));
    }

    /*
     * To get reasons from register CGM_REJECT_STATUS_BITMAP for block CGM, and
     * then put these reasons to reason[].reason_buffer_cgm for printing later
     */
    reg = CGM_REJECT_STATUS_BITMAPr;
    reginfo = &SOC_REG_INFO(unit, reg);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
    }
    for (i_fld = 0; i_fld < reginfo->nFields; i_fld++)
    {
        fldinfo = &reginfo->fields[i_fld];
        for (i_core = 0; i_core < nof_cores; i_core++)
        {
            SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
            soc_reg_above_64_field_get(unit, reg, reg_value[i_core], fldinfo->field, fld_value[i_core]);
        }
        for (i_core = 0; i_core < nof_cores; i_core++)
        {
            if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
            {
                break;
            }
        }
        if (i_core == nof_cores)
        {
            continue;
        }
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
            }
        }
        for (i_core = 0; i_core <= cdsp; i_core++)
        {
            if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
            {
                sal_snprintf(reason[i_core].reason_buffer_cgm[reason[i_core].nof_reason_cgm++].buffer,
                             DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s", SOC_FIELD_NAME(unit, fldinfo->field));
            }
        }
    }

    /*
     * To get reasons from register ECGM_RQP_DISCARD_REASONS for block RQP, and
     * then put these reasons to reason[].reason_buffer_rqp for printing later
     */
    reg = ECGM_RQP_DISCARD_REASONSr;
    field = RQP_DISCARD_REASONSf;
    field_len = soc_reg_field_length(unit, reg, field);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
        SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
        soc_reg_above_64_field_get(unit, reg, reg_value[i_core], field, fld_value[i_core]);
    }
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
        {
            break;
        }
    }
    if (i_core < nof_cores)
    {
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
            }
        }
        for (i_bit = 0; i_bit < field_len; i_bit++)
        {
            bit_mask = 0x1 << (i_bit % 32);
            for (i_core = 0; i_core <= cdsp; i_core++)
            {
                if (fld_value[i_core][i_bit / 32] & bit_mask)
                {
                    sal_snprintf(reason[i_core].reason_buffer_rqp[reason[i_core].nof_reason_rqp++].buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s",
                                 dnx_data_ecgm.info.dropped_reason_rqp_get(unit, i_bit)->reason);
                }
            }
        }
    }

    /*
     * To get reasons from register ECGM_RQP_DISCARD_REASONS for block RQP, and
     * then put these reasons to reason[].reason_buffer_rqp for printing later
     */
    reg = ECGM_PQP_DISCARD_REASONSr;
    field = PQP_DISCARD_REASONSf;
    field_len = soc_reg_field_length(unit, reg, field);
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_value[i_core]);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, i_core, 0, reg_value[i_core]));
        SOC_REG_ABOVE_64_CLEAR(fld_value[i_core]);
        soc_reg_above_64_field_get(unit, reg, reg_value[i_core], field, fld_value[i_core]);
    }
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        if (!SOC_REG_ABOVE_64_IS_ZERO(fld_value[i_core]))
        {
            break;
        }
    }
    if (i_core < nof_cores)
    {
        if (!cdsp)
        {
            for (i_core = nof_cores - 1; i_core >= 1; i_core--)
            {
                SOC_REG_ABOVE_64_OR(fld_value[i_core - 1], fld_value[i_core]);
            }
        }
        for (i_bit = 0; i_bit < field_len; i_bit++)
        {
            bit_mask = 0x1 << (i_bit % 32);
            for (i_core = 0; i_core <= cdsp; i_core++)
            {
                if (fld_value[i_core][i_bit / 32] & bit_mask)
                {
                    sal_snprintf(reason[i_core].reason_buffer_pqp[reason[i_core].nof_reason_pqp++].buffer,
                                 DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH, "%s",
                                 dnx_data_ecgm.info.dropped_reason_pqp_get(unit, i_bit)->reason);
                }
            }
        }
    }

    /*
     * To get max number of reasons between 2 cores for block CGM, RQP and PQP.
     */
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        nof_reason_cgm =
            nof_reason_cgm > reason[i_core].nof_reason_cgm ? nof_reason_cgm : reason[i_core].nof_reason_cgm;
        nof_reason_rqp =
            nof_reason_rqp > reason[i_core].nof_reason_rqp ? nof_reason_rqp : reason[i_core].nof_reason_rqp;
        nof_reason_pqp =
            nof_reason_pqp > reason[i_core].nof_reason_pqp ? nof_reason_pqp : reason[i_core].nof_reason_pqp;
    }

    /*
     * EXIT when no reason to print in all the blocks (CGM, RQP and PQP).
     */
    if ((0 == nof_reason_cgm) && (0 == nof_reason_rqp) && (0 == nof_reason_pqp))
    {
        SHR_EXIT();
    }

    /*
     * To accurate the lines for block RQP and PQP.
     * one empty line printed when no reason to print for block RQP and PQP.
     */
    nof_reason_rqp = (0 == nof_reason_rqp) ? 1 : nof_reason_rqp;
    nof_reason_pqp = (0 == nof_reason_pqp) ? 1 : nof_reason_pqp;

    /*
     * One partion line printed between block RQP and PQP.
     * To get max number of reasons between CGM and sum(RQP, PQP and partion line).
     */
    nof_reason =
        nof_reason_cgm > (nof_reason_rqp + nof_reason_pqp + 1) ? nof_reason_cgm : (nof_reason_rqp + nof_reason_pqp + 1);

    /*
     * Draw table to print reasons for the dropped packets 
     */
    PRT_TITLE_SET("Reason for dropped and rejected packets");
    PRT_COLUMN_ADD("Block");
    if (cdsp)
    {
        PRT_COLUMN_ADD(" Reason(core_0)                  ");
        PRT_COLUMN_ADD(" Reason(core_1)                 ");
    }
    else
    {
        PRT_COLUMN_ADD("Reason                                             ");
    }
    PRT_COLUMN_ADD("Block");
    if (cdsp)
    {
        PRT_COLUMN_ADD(" Reason(core_0)                   ");
        PRT_COLUMN_ADD(" Reason(core_1)                  ");
    }
    else
    {
        PRT_COLUMN_ADD("Reason                                              ");
    }

    for (i_reason = 0; i_reason < nof_reason; i_reason++)
    {
        /*
         * Add new row 
         */
        if (nof_reason - 1 == i_reason)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        /*
         * Print block name only in first line for each block 
         */
        if (0 == i_reason)
        {
            PRT_CELL_SET("CGM");
            for (i_core = 0; i_core <= cdsp; i_core++)
            {
                PRT_CELL_SET("%s", reason[i_core].reason_buffer_cgm[i_reason].buffer);
            }
            PRT_CELL_SET("RQP");
            for (i_core = 0; i_core <= cdsp; i_core++)
            {
                PRT_CELL_SET("%s", reason[i_core].reason_buffer_rqp[i_reason].buffer);
            }
        }
        else
        {
            /*
             * Print in left for block CGM 
             */
            if (i_reason < nof_reason_cgm)
            {
                PRT_CELL_SKIP(1);
                for (i_core = 0; i_core <= cdsp; i_core++)
                {
                    PRT_CELL_SET("%s", reason[i_core].reason_buffer_cgm[i_reason].buffer);
                }
            }
            else
            {
                PRT_CELL_SKIP(cdsp ? 3 : 2);
            }
            /*
             * Print in right for block RQP and PQP
             */
            if (i_reason < nof_reason_rqp)
            {
                /*
                 * Print for block RQP
                 */
                PRT_CELL_SKIP(1);
                for (i_core = 0; i_core <= cdsp; i_core++)
                {
                    PRT_CELL_SET("%s", reason[i_core].reason_buffer_rqp[i_reason].buffer);
                }
            }
            else if (i_reason == nof_reason_rqp)
            {
                /*
                 * Print partion line between block RQP and PQP
                 */
                PRT_CELL_SET("-----");
                if (cdsp)
                {
                    PRT_CELL_SET("----------------------------------");
                    PRT_CELL_SET("---------------------------------");
                }
                else
                {
                    PRT_CELL_SET("----------------------------------------------------");
                }
            }
            else if (i_reason == nof_reason_rqp + 1)
            {
                /*
                 * Print first line for block RQP
                 */
                PRT_CELL_SET("PQP");
                for (i_core = 0; i_core <= cdsp; i_core++)
                {
                    PRT_CELL_SET("%s", reason[i_core].reason_buffer_pqp[i_reason - nof_reason_rqp - 1].buffer);
                }
            }
            else if (i_reason < nof_reason_pqp + nof_reason_pqp + 1)
            {
                /*
                 * Print the remaining for block RQP
                 */
                PRT_CELL_SKIP(1);
                for (i_core = 0; i_core <= cdsp; i_core++)
                {
                    PRT_CELL_SET("%s", reason[i_core].reason_buffer_pqp[i_reason - nof_reason_rqp - 1].buffer);
                }
            }
            else
            {
                PRT_CELL_SKIP(cdsp ? 3 : 2);
            }
        }
    }

    PRT_COMMITX;

exit:
    for (i_core = 0; i_core < nof_cores; i_core++)
    {
        sal_free(reason[i_core].reason_buffer_cgm);
        sal_free(reason[i_core].reason_buffer_rqp);
        sal_free(reason[i_core].reason_buffer_pqp);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to print out half an row (half left or half right), given counter index and block index 
 *   in one counter block group.
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks in the counter block group 
 * \param [in] blk_group - a pointer to an array which contains counter block group information 
 * \param [in] options - options set (cdsp, non_zero, in_hex, and interval) for graphical printing
 * \param [in] prt_ctr - print control pointer
 * \param [in] cnt_idx - counter index within one block
 * \param [in] blk_idx - block index within block goup
 * \param [in] is_parting_line - parting line flag
 * \return 
 *      Standard error handling
 */
static shr_error_e
sh_dnx_counter_graphic_half_row_print(
    int unit,
    int nof_block,
    sh_dnx_counter_graphical_block_t * blk_group,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr,
    int *cnt_idx,
    int *blk_idx,
    int *is_parting_line)
{
    int field_len = 0;
    soc_reg_above_64_val_t reg_value;
    soc_reg_above_64_val_t fld_value[COUNTER_GRAPHIC_CORE_NUM];
    soc_reg_above_64_val_t temp_fld_value;
    char value_str[PRT_COLUMN_WIDTH_BIG];
    int instance_id, nof_instance;
    int reg_idx, nof_regs = 1;
    int overflow[COUNTER_GRAPHIC_CORE_NUM];
    uint64 temp_fld_value_u64, prev_fld_val, fld_value_u64[COUNTER_GRAPHIC_CORE_NUM];
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    if (*blk_idx >= nof_block)
    {
        /*
         * skip all the cells when exhausting all the blocks 
         */
        PRT_CELL_SKIP((options.cdsp ? 4 : 3));
    }
    else if (TRUE == *is_parting_line)
    {
        /*
         * set one parting line between 2 blocks with one block group 
         */
        PRT_CELL_SET("----");
        PRT_CELL_SET("----------------------------------");
        PRT_CELL_SET("---------------");
        if (options.cdsp)
        {
            PRT_CELL_SET("---------------");
        }
    }
    else
    {
        /*
         * set block name only for the first counter within one block 
         */
        if (0 == *cnt_idx)
        {
            PRT_CELL_SET("%s", blk_group[*blk_idx].block_name);
        }
        else
        {
            PRT_CELL_SKIP(1);
        }
        /** for CDU, need to cosider CDU and CDUM. therefore, two regs */
        if (blk_group[*blk_idx].counter_table[*cnt_idx].flags == DNX_DIAG_COUNTER_REG_FLAGS_MULTI_REGS)
        {
            nof_regs = DNX_DIAG_COUNTER_REG_NOF_REGS;
        }
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            SOC_REG_ABOVE_64_CLEAR(fld_value[core_idx]);
            COMPILER_64_ZERO(fld_value_u64[core_idx]);
            overflow[core_idx] = FALSE;
        }

        for (reg_idx = 0; reg_idx < nof_regs; reg_idx++)
        {
            if (INVALIDr == blk_group[*blk_idx].counter_table[*cnt_idx].reg_arr[reg_idx])
            {
                continue;
            }
            /** if CDU, need to sum all instance values and devide them to core 0/1 according to the instance id */
            if (blk_group[*blk_idx].counter_table[*cnt_idx].block[reg_idx] == SOC_BLK_CDU)
            {
                nof_instance = dnx_data_nif.eth.cdu_nof_get(unit) - dnx_data_nif.eth.cdum_nof_get(unit);
            }
            else if (blk_group[*blk_idx].counter_table[*cnt_idx].block[reg_idx] == SOC_BLK_CDUM)
            {
                nof_instance = dnx_data_nif.eth.cdum_nof_get(unit);
            }
            else
            {
                nof_instance = dnx_data_device.general.nof_cores_get(unit);
            }

            for (instance_id = 0; instance_id < nof_instance; instance_id++)
            {
                /** read counter register and set counter value */
                SOC_REG_ABOVE_64_CLEAR(reg_value);
                field_len = soc_reg_field_length(unit, blk_group[*blk_idx].counter_table[*cnt_idx].reg_arr[reg_idx],
                                                 blk_group[*blk_idx].counter_table[*cnt_idx].cnt_field[reg_idx]);
                if (blk_group[*blk_idx].counter_table[*cnt_idx].block[reg_idx] == SOC_BLK_FDT)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                                    (unit, blk_group[*blk_idx].counter_table[*cnt_idx].reg_arr[reg_idx], 0, instance_id,
                                     reg_value));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get
                                    (unit, blk_group[*blk_idx].counter_table[*cnt_idx].reg_arr[reg_idx], instance_id,
                                     blk_group[*blk_idx].counter_table[*cnt_idx].reg_index, reg_value));
                }
                SOC_REG_ABOVE_64_CLEAR(temp_fld_value);
                soc_reg_above_64_field_get(unit, blk_group[*blk_idx].counter_table[*cnt_idx].reg_arr[reg_idx],
                                           reg_value, blk_group[*blk_idx].counter_table[*cnt_idx].cnt_field[reg_idx],
                                           temp_fld_value);

                if (instance_id < (nof_instance / dnx_data_device.general.nof_cores_get(unit)))
                {
                    /** some counter may are 128 bits. need to verify that it didn't exceed 64 bits value */
                    SHR_IF_ERR_EXIT(dnx_diag_overflow_check
                                    (unit, field_len, temp_fld_value, &overflow[COUNTER_GRAPHIC_CORE_1ST]));
                    COMPILER_64_ZERO(temp_fld_value_u64);
                    COMPILER_64_ZERO(prev_fld_val);
                    COMPILER_64_COPY(prev_fld_val, fld_value_u64[0]);
                    COMPILER_64_SET(temp_fld_value_u64, temp_fld_value[1], temp_fld_value[0]);
                    COMPILER_64_ADD_64(fld_value_u64[0], temp_fld_value_u64);
                    if (COMPILER_64_LT(fld_value_u64[0], prev_fld_val) ||
                        COMPILER_64_LT(fld_value_u64[0], temp_fld_value_u64))
                    {
                        overflow[COUNTER_GRAPHIC_CORE_1ST] = TRUE;
                    }
                }
                else
                {
                    /** some counter may are 128 bits. need to verify that it didn't exceed 64 bits value */
                    SHR_IF_ERR_EXIT(dnx_diag_overflow_check
                                    (unit, field_len, temp_fld_value, &overflow[COUNTER_GRAPHIC_CORE_2ND]));
                    COMPILER_64_ZERO(temp_fld_value_u64);
                    COMPILER_64_ZERO(prev_fld_val);
                    COMPILER_64_COPY(prev_fld_val, fld_value_u64[1]);
                    COMPILER_64_SET(temp_fld_value_u64, temp_fld_value[1], temp_fld_value[0]);
                    COMPILER_64_ADD_64(fld_value_u64[1], temp_fld_value_u64);
                    if (COMPILER_64_LT(fld_value_u64[1], prev_fld_val) ||
                        COMPILER_64_LT(fld_value_u64[1], temp_fld_value_u64))
                    {
                        overflow[COUNTER_GRAPHIC_CORE_2ND] = TRUE;
                    }
                }
            }
        }

        /** set the fld value */
        fld_value[COUNTER_GRAPHIC_CORE_1ST][0] = COMPILER_64_LO(fld_value_u64[COUNTER_GRAPHIC_CORE_1ST]);
        fld_value[COUNTER_GRAPHIC_CORE_1ST][1] = COMPILER_64_HI(fld_value_u64[COUNTER_GRAPHIC_CORE_1ST]);
        fld_value[COUNTER_GRAPHIC_CORE_2ND][0] = COMPILER_64_LO(fld_value_u64[COUNTER_GRAPHIC_CORE_2ND]);
        fld_value[COUNTER_GRAPHIC_CORE_2ND][1] = COMPILER_64_HI(fld_value_u64[COUNTER_GRAPHIC_CORE_2ND]);

        if (field_len > 64)
        {
            field_len = 64;
        }

        /*
         * print graphical counter on both cores separately 
         */
        if (options.cdsp)
        {
            /*
             * option NonZero enabled and counter on one core at least is zero 
             */
            if (options.non_zero && (SOC_REG_ABOVE_64_IS_ZERO(fld_value[COUNTER_GRAPHIC_CORE_1ST]) ||
                                     SOC_REG_ABOVE_64_IS_ZERO(fld_value[COUNTER_GRAPHIC_CORE_2ND])))
            {
                /*
                 * counters on both cores are zero 
                 */
                if (SOC_REG_ABOVE_64_IS_ZERO(fld_value[COUNTER_GRAPHIC_CORE_1ST]) &&
                    SOC_REG_ABOVE_64_IS_ZERO(fld_value[COUNTER_GRAPHIC_CORE_2ND]))
                {
                    PRT_CELL_SKIP(3);
                }
                /*
                 * counter on core 0 is zero 
                 */
                else if (SOC_REG_ABOVE_64_IS_ZERO(fld_value[COUNTER_GRAPHIC_CORE_1ST]))
                {
                    /*
                     * set counter name 
                     */
                    PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);
                    PRT_CELL_SKIP(1);
                    format_value_to_str(fld_value[COUNTER_GRAPHIC_CORE_2ND], field_len, options.in_hex, value_str);
                    if (overflow[COUNTER_GRAPHIC_CORE_2ND] == TRUE)
                    {
                        PRT_CELL_SET("%s%s", value_str, "(ovf)");
                    }
                    else
                    {
                        PRT_CELL_SET("%s", value_str);
                    }
                }
                /*
                 * counter on core 1 is zero 
                 */
                else
                {
                    /*
                     * set counter name 
                     */
                    PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);
                    format_value_to_str(fld_value[COUNTER_GRAPHIC_CORE_1ST], field_len, options.in_hex, value_str);
                    if (overflow[COUNTER_GRAPHIC_CORE_1ST] == TRUE)
                    {
                        PRT_CELL_SET("%s%s", value_str, "(ovf)");
                    }
                    else
                    {
                        PRT_CELL_SET("%s", value_str);
                    }
                    PRT_CELL_SKIP(1);
                }
            }
            /*
             * option NonZero disabled or none of counter is zero 
             */
            else
            {
                /*
                 * set counter name 
                 */
                PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);
                format_value_to_str(fld_value[COUNTER_GRAPHIC_CORE_1ST], field_len, options.in_hex, value_str);
                if (overflow[COUNTER_GRAPHIC_CORE_1ST] == TRUE)
                {
                    PRT_CELL_SET("%s%s", value_str, "(ovf)");
                }
                else
                {
                    PRT_CELL_SET("%s", value_str);
                }
                format_value_to_str(fld_value[COUNTER_GRAPHIC_CORE_2ND], field_len, options.in_hex, value_str);
                if (overflow[COUNTER_GRAPHIC_CORE_2ND] == TRUE)
                {
                    PRT_CELL_SET("%s%s", value_str, "(ovf)");
                }
                else
                {
                    PRT_CELL_SET("%s", value_str);
                }
            }
        }
        /*
         * print graphical counter with option cdsp disabled 
         */
        else
        {
            if (options.non_zero && SOC_REG_ABOVE_64_IS_ZERO(fld_value[COUNTER_GRAPHIC_CORE_1ST]))
            {
                PRT_CELL_SKIP(2);
            }
            else
            {
                /*
                 * set counter name 
                 */
                int overflow_sum;
                PRT_CELL_SET("%s", blk_group[*blk_idx].counter_table[*cnt_idx].reg_name);
                sum_value_to_str(fld_value[COUNTER_GRAPHIC_CORE_1ST], fld_value[COUNTER_GRAPHIC_CORE_2ND], field_len,
                                 options.in_hex, value_str, &overflow_sum);
                if (overflow_sum == TRUE || overflow[COUNTER_GRAPHIC_CORE_1ST] == TRUE ||
                    overflow[COUNTER_GRAPHIC_CORE_2ND] == TRUE)
                {
                    PRT_CELL_SET("%s%s", value_str, "(ovf)");
                }
                else
                {
                    PRT_CELL_SET("%s", value_str);
                }
            }
        }
    }
    if (TRUE == *is_parting_line)
    {
        /*
         * unmarked parting_line flag 
         */
        *is_parting_line = FALSE;
    }
    else
    {
        /*
         * increase counter index only when drawing non_parting_line row 
         */
        (*cnt_idx)++;
    }
    /*
     * increase block index, reset counter index and set parting_line flag when one block is finished 
     */
    if (*cnt_idx >= blk_group[*blk_idx].num_entries)
    {
        (*blk_idx)++;
        *cnt_idx = 0;
        *is_parting_line = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Below is the sketch of counter graphic
 * --------------------------------------------------------------------------
 * | Block |    Counter      |  Value  | Block |    Counter       |  Value  |
 * --------------------------------------------------------------------------   --------
 * | blk_1 | blk_1_cnt_1     |  00000  | blk_5 | blk_5_cnt_1      | 00000   |
 * |       | blk_1_cnt_2     |  00000  |       | blk_5_cnt_2      | 00000   | <--section_1
 * |       |                 |         |       | blk_5_cnt_3      | 00000   |
 * --------------------------------------------------------------------------   --------
 * | blk_2 | blk_2_cnt_1     |  00000  | blk_6 | blk_6_cnt_1      | 00000   |
 * |       | blk_2_cnt_2     |  00000  |       | blk_6_cnt_2      | 00000   |
 * |-----------------------------------|       | blk_6_cnt_3      | 00000   | <--section_2
 * | blk_3 | blk_3_cnt_1     |  00000  |       | blk_6_cnt_4      | 00000   |
 * |       | blk_3_cnt_2     |  00000  |       |                  |         |
 * --------------------------------------------------------------------------   --------
 * | blk_4 | blk_4_cnt_1     |  00000  | blk_7 | blk_7_cnt_1      | 00000   |
 * |       | blk_4_cnt_2     |  00000  |       | blk_7_cnt_2      | 00000   |
 * |       | blk_4_cnt_3     |  00000  |       | blk_7_cnt_3      | 00000   |
 * |       | blk_4_cnt_4     |  00000  |------------------------------------| <--section_3
 * |       | blk_4_cnt_5     |  00000  | blk_8 | blk_8_cnt_1      | 00000   |
 * |       | blk_4_cnt_6     |  00000  |       |                  |         |
 * --------------------------------------------------------------------------   --------
 * |                                   |                                    |
 * | <------      Left      ------>    | <------      Right      ------>    |
 */
/**
 * \brief
 *   to print out one section of the couter graphic which contains x blocks 
 *   in left side and y blocks in right side.
 *   the entire counter graphic consists of serval sections.
 * \param [in] unit - Unit #
 * \param [in] blk_grp_left - a pointer to an array which contains counter informantion 
 *             on the base of block for left side of this section.
 * \param [in] blk_grp_right - a pointer to an array which contains counter informantion 
 *             on the base of block for right side of this section.
 * \param [in] options - options set (cdsp, non_zero, in_hex, and interval) for graphical printing
 * \param [in] prt_ctr - print control pointer
 * \return 
 *      Standard error handling
 */
static shr_error_e
sh_dnx_counter_graphic_section_print(
    int unit,
    sh_dnx_counter_graphical_block_t * blk_grp_left,
    sh_dnx_counter_graphical_block_t * blk_grp_right,
    sh_dnx_counter_graphical_options_t options,
    prt_control_t * prt_ctr)
{
    int i_total = 0;
    int iblk_left = 0, icnt_left = 0;
    int iblk_right = 0, icnt_right = 0;
    int is_parting_line_left = FALSE, is_parting_line_right = FALSE;
    int nof_blk_left, nof_blk_right, row_of_section;
    int idx = 0, i_block = 0, nof_block = 0, nof_block_total = 0;
    int *pblock_ids, block_ids[BLOCK_MAX_INDEX];
    uint32 nof_clocks;
    dnxcmn_time_t time_given_usec;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * calculate number of blocks in the left and right block group 
     */
    SHR_IF_ERR_EXIT(counter_graphical_valid_block_get(unit, blk_grp_left, &nof_blk_left));
    SHR_IF_ERR_EXIT(counter_graphical_valid_block_get(unit, blk_grp_right, &nof_blk_right));
    /*
     * calculate row of this counter graphical section 
     */
    SHR_IF_ERR_EXIT(counter_graphical_section_row_get
                    (unit, blk_grp_left, blk_grp_right, nof_blk_left, nof_blk_right, &row_of_section));

    if (options.interval)
    {
        time_given_usec.time_units = DNXCMN_TIME_UNIT_SEC;
        time_given_usec.time = options.interval;
        SHR_IF_ERR_EXIT_WITH_LOG(dnxcmn_time_to_clock_cycles_get(unit, &time_given_usec, &nof_clocks),
                                 "Failed to get clock cycles %s%s%s\n", EMPTY, EMPTY, EMPTY);

        /*
         * Get all the block instances for the blocks in counter graphical 
         */
        sal_memset(block_ids, 0, BLOCK_MAX_INDEX * sizeof(int));
        pblock_ids = block_ids;
        for (i_block = 0; i_block < nof_blk_left; i_block++)
        {
            if (counter_block_name_match(unit, blk_grp_left[i_block].block_name, &nof_block, pblock_ids) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
            }
            nof_block_total += nof_block;
            pblock_ids = block_ids + nof_block_total;
        }
        for (i_block = 0; i_block < nof_blk_right; i_block++)
        {
            if (counter_block_name_match(unit, blk_grp_right[i_block].block_name, &nof_block, pblock_ids) !=
                _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "no matched block found\n");
            }
            nof_block_total += nof_block;
            pblock_ids = block_ids + nof_block_total;
        }

        /*
         * Config and triger the gtimer for the blocks 
         */
        for (idx = 0; idx < nof_block_total; idx++)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_set(unit, nof_clocks, SOC_BLOCK_TYPE(unit, block_ids[idx]),
                                                    SOC_BLOCK_NUMBER(unit, block_ids[idx])),
                                     "Failed to config gtimer in block(%s%s%s)\n", SOC_BLOCK_NAME(unit, block_ids[idx]),
                                     EMPTY, EMPTY);
        }
        for (idx = 0; idx < nof_block_total; idx++)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_wait(unit, SOC_BLOCK_TYPE(unit, block_ids[idx]),
                                                     SOC_BLOCK_NUMBER(unit, block_ids[idx])),
                                     "Failed to wait gtimer in block(%s%s%s)\n", SOC_BLOCK_NAME(unit, block_ids[idx]),
                                     EMPTY, EMPTY);
        }
    }

    for (i_total = 0; i_total < row_of_section; i_total++)
    {
        if (row_of_section - 1 == i_total)
        {
            /*
             * the last row drawed with underscore 
             */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        }
        else
        {
            /*
             * the other rows drawed with non_line 
             */
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }
        /*
         * print the half left row 
         */
        SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_half_row_print
                        (unit, nof_blk_left, blk_grp_left, options, prt_ctr, &icnt_left, &iblk_left,
                         &is_parting_line_left));
        /*
         * print the half right row 
         */
        SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_half_row_print
                        (unit, nof_blk_right, blk_grp_right, options, prt_ctr, &icnt_right, &iblk_right,
                         &is_parting_line_right));
    }

    if (options.interval)
    {
        /*
         * Clear the gtimer for the blocks 
         */
        for (idx = 0; idx < nof_block_total; idx++)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_gtimer_clear(unit, SOC_BLOCK_TYPE(unit, block_ids[idx]),
                                                      SOC_BLOCK_NUMBER(unit, block_ids[idx])),
                                     "Failed to clear gtimer in block(%s%s%s)\n", SOC_BLOCK_NAME(unit, block_ids[idx]),
                                     EMPTY, EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   to display the counter graphic in which counters are placed
 *   in packet-flow-going sequence
 * \param [in] unit - Unit #
 * \param [in] cdsp - indicate if counters were displayed per core
 * \param [in] non_zero - indicate to display non_zero counters only
 * \param [in] in_hex - indicate to display counters in hex format
 * \param [in] interval - indicate the interval of g-timer
 * \param [in] sand_control - pointer to framework control structure
 * \return 
 *      Standard error handling
 */
shr_error_e
sh_dnx_diag_counter_graphical_print(
    int unit,
    int cdsp,
    int non_zero,
    int in_hex,
    uint32 interval,
    sh_sand_control_t * sand_control)
{
    char *printout_buffer_1[DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH];
    char *printout_buffer_2[DNX_DIAG_GRAPHICAL_PRINT_BUFFER_LENGTH];
    sh_dnx_counter_graphical_block_t *working_block = NULL;
    sh_dnx_counter_graphical_block_t group_left[COUNTER_GRAPHIC_BLK_MAX];
    sh_dnx_counter_graphical_block_t group_right[COUNTER_GRAPHIC_BLK_MAX];
    sh_dnx_counter_graphical_options_t options;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(NO_UNIT);

    options.cdsp = cdsp;
    options.non_zero = non_zero;
    options.in_hex = in_hex;
    options.interval = interval;

    *printout_buffer_1 =
        cdsp ? "                                              " : "                                    ";
    *printout_buffer_2 = cdsp ? "                       " : "               ";
    LOG_CLI(("%s||%s                 %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    LOG_CLI(("%s||%sNetwork Interface%s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    LOG_CLI(("%s\\/%s                 %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));

    /*
     * add column for counter graphical 
     */
    PRT_TITLE_SET("");
    PRT_COLUMN_ADD("   ");
    PRT_COLUMN_ADD("   Counter  ");
    if (cdsp)
    {
        PRT_COLUMN_ADD(" Value(core_0) ");
        PRT_COLUMN_ADD(" Value(core_1) ");
    }
    else
    {
        PRT_COLUMN_ADD("   Value   ");
    }
    PRT_COLUMN_ADD("   ");
    PRT_COLUMN_ADD("   Counter  ");
    if (cdsp)
    {
        PRT_COLUMN_ADD(" Value(core_0) ");
        PRT_COLUMN_ADD(" Value(core_1) ");
    }
    else
    {
        PRT_COLUMN_ADD("   Value   ");
    }

    /******************************************************************
     *  section 1: including NIF counter in both left and right side  *
     ******************************************************************/
    /*
     * set counter block group to 0 before loading data of section 1 
     */
    sal_memset(group_left, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    sal_memset(group_right, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    /*
     * section 1: the 1st block NIF_RX in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_cdu_rx_jer2, "CDU");

    /*
     * section 1: the 1st block NIF_TX in right group 
     */
    working_block = &group_right[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_cdu_tx_jer2, "CDU");
    /*
     * print out counter graphical section 1 
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_section_print(unit, group_left, group_right, options, prt_ctr));

    /******************************************************************************
     *  section 2: including IRE counter in left side, EPNI counter inright side  *
     ******************************************************************************/
    /*
     * set counter block group to 0 before loading data of section 2 
     */
    sal_memset(group_left, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    sal_memset(group_right, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    /*
     * section 2: the 1st block IRE in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_ire_jer2, "IRE");

    /*
     * section 2: the 1st block EPNI in right group 
     */
    working_block = &group_right[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_epni_jer2, "EPNI");
    /*
     * print out counter graphical section 2 
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_section_print(unit, group_left, group_right, options, prt_ctr));

    /******************************************************************************
     *  section 3: including SPB, DDP, CGM and IPT counter in left side, and RQP  *
     *  and PQP counter inright side                                              *
     ******************************************************************************/
    /*
     * set counter block group to 0 before loading data of section 3 
     */
    sal_memset(group_left, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    sal_memset(group_right, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    /*
     * section 3: the 1st block SPB in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_spb_jer2, "SPB");
    /*
     * section 3: the 2nd block DDP in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_2ND];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_ddp_jer2, "DDP");
    /*
     * section 3: the 3rd block CGM in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_3RD];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_cgm_jer2, "CGM");
    /*
     * section 3: the 4th block IPT in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_4TH];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_ipt_jer2, "IPT");

    /*
     * section 3: the 1st block RQP in right group 
     */
    working_block = &group_right[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_pqp_jer2, "PQP");
    /*
     * section 3: the 2nd block PQP in right group 
     */
    working_block = &group_right[COUNTER_GRAPHIC_BLK_2ND];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_rqp_jer2, "RQP");
    /*
     * section 3: the 3rd block FDA in right group
     */
    working_block = &group_right[COUNTER_GRAPHIC_BLK_3RD];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_fda_jer2, "FDA");

    /*
     * print out counter graphical section 3 
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_section_print(unit, group_left, group_right, options, prt_ctr));

    /******************************************************************************
     *  section 4: including FDT counter in left side, FDR counter inright side  *
     ******************************************************************************/
    /*
     * set counter block group to 0 before loading data of section 2 
     */
    sal_memset(group_left, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    sal_memset(group_right, 0, sizeof(sh_dnx_counter_graphical_block_t) * COUNTER_GRAPHIC_BLK_MAX);
    /*
     * section 4: the 1st block FDT in left group 
     */
    working_block = &group_left[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_fdt_jer2, "FDT");

    /*
     * section 4: the 1st block FDR in right group 
     */
    working_block = &group_right[COUNTER_GRAPHIC_BLK_1ST];
    COUNTER_GRAPHIC_BLOCK_SET(working_block, sh_dnx_counter_graphic_fdr_jer2, "FDR");
    /*
     * print out counter graphical section 4 
     */
    SHR_IF_ERR_EXIT(sh_dnx_counter_graphic_section_print(unit, group_left, group_right, options, prt_ctr));

    PRT_COMMITX;

    /*
     * print reasons for dropped packets in block CGM, RQP and PQP 
     */
    SHR_IF_ERR_EXIT(sh_dnx_packet_dropped_reason_print(unit, cdsp, prt_ctr, sand_control));

    LOG_CLI(("%s||%s                  %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    LOG_CLI(("%s||%s Fabric Interface %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    LOG_CLI(("%s\\/%s                  %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   For DNX device only
 *   To start gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \param [in] interval - indicate how long gtimer keeps enabled (second)
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnx_diag_block_gtimer_start(
    int unit,
    int nof_block,
    int *block_ids,
    int interval)
{
    int i, is_supported;
    int rv = _SHR_E_NONE;
    uint32 nof_clocks;
    dnxcmn_time_t time_given_usec;
    SHR_FUNC_INIT_VARS(unit);

    time_given_usec.time_units = DNXCMN_TIME_UNIT_SEC;
    time_given_usec.time = interval;
    rv = dnxcmn_time_to_clock_cycles_get(unit, &time_given_usec, &nof_clocks);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) to get clock cycles %s%s\n", rv, EMPTY, EMPTY);

    /*
     * Config gtimer for all the blocks
     */
    for (i = 0; i < nof_block; i++)
    {
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }
        rv = dnx_gtimer_set(unit, nof_clocks, SOC_BLOCK_TYPE(unit, block_ids[i]), SOC_BLOCK_NUMBER(unit, block_ids[i]));
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) config gtimer in block(%s%s)\n", rv,
                                 SOC_BLOCK_NAME(unit, block_ids[i]), EMPTY);
    }

    /*
     * Wait gtimer trigger to indicate counting finished
     */
    for (i = 0; i < nof_block; i++)
    {
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }
        rv = dnx_gtimer_wait(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), SOC_BLOCK_NUMBER(unit, block_ids[i]));

        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) wait gtimer in block(%s%s)\n", rv, SOC_BLOCK_NAME(unit, block_ids[i]),
                                 EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   For DNX device only
 *   To stop gtimer of the blocks specified by block index array
 * \param [in] unit - Unit #
 * \param [in] nof_block - number of blocks contained in block_ids array
 * \param [in] block_ids - an array to contain the blockes
 * \return
 *      Standard error handling
 */
shr_error_e
sh_dnx_diag_block_gtimer_stop(
    int unit,
    int nof_block,
    int *block_ids)
{
    int i, is_supported;
    int rv = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Stop gtimer
     */
    for (i = 0; i < nof_block; i++)
    {
        /** verify if block supports gtimer */
        SHR_IF_ERR_EXIT(dnx_gtimer_block_type_is_supported(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), &is_supported));
        if (is_supported == FALSE)
        {
            continue;
        }

        rv = dnx_gtimer_clear(unit, SOC_BLOCK_TYPE(unit, block_ids[i]), SOC_BLOCK_NUMBER(unit, block_ids[i]));
        SHR_IF_ERR_EXIT_WITH_LOG(rv, "Failed(%d) clear gtimer in block(%s%s)\n", rv, SOC_BLOCK_NAME(unit, block_ids[i]),
                                 EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}
