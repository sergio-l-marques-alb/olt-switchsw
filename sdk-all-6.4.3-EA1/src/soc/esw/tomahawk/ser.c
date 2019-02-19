/*
 * $Id: ser.c $
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
 * File:      ser.c
 * Purpose:   SER enable/detect and test functionality.
 * Requires:  sal/soc/shared layer
 */


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>

#ifdef BCM_TOMAHAWK_SUPPORT
#undef SER_TR_TEST_SUPPORT

#include <soc/tomahawk.h>

/* #define _SOC_SER_ENABLE_CLI_DBG */

#define _SOC_MMU_BASE_INDEX_GLB 0

#define _SOC_MMU_BASE_INDEX_XPE0 0
#define _SOC_MMU_BASE_INDEX_XPE1 1
#define _SOC_MMU_BASE_INDEX_XPE2 2
#define _SOC_MMU_BASE_INDEX_XPE3 3

#define _SOC_MMU_BASE_INDEX_SC0 0
#define _SOC_MMU_BASE_INDEX_SC1 1

#define _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD 0xfffc7fff /* [17:15] = 3'b000 */
#define _SOC_MMU_ADDR_SEG0 0x00000000 /* [17:15] = 3'b000 */
#define _SOC_MMU_ADDR_SEG1 0x00008000 /* [17:15] = 3'b001 */
#define _SOC_MMU_ADDR_SEG2 0x00010000 /* [17:15] = 3'b010 */
#define _SOC_MMU_ADDR_SEG3 0x00018000 /* [17:15] = 3'b011 */

#define _SOC_FIND_MMU_XPE_SER_FIFO_MEM(mmu_base_index) \
            ((mmu_base_index == 0)? MMU_XCFG_MEM_FAIL_ADDR_64_XPE0m : \
             (mmu_base_index == 1)? MMU_XCFG_MEM_FAIL_ADDR_64_XPE1m : \
             (mmu_base_index == 2)? MMU_XCFG_MEM_FAIL_ADDR_64_XPE2m : \
                                    MMU_XCFG_MEM_FAIL_ADDR_64_XPE3m)

#define _SOC_FIND_MMU_SC_SER_FIFO_MEM(mmu_base_index) \
            ((mmu_base_index == 0)? MMU_SCFG_MEM_FAIL_ADDR_64_SC0m : \
                                    MMU_SCFG_MEM_FAIL_ADDR_64_SC1m)

typedef struct _soc_th_ser_mmu_intr_info_s {
    soc_reg_t   int_statf;
    int         mmu_base_index;
    int         ser_info_index;
} _soc_th_ser_mmu_intr_info_t;

typedef enum {
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_MMU_SER,
    _SOC_PARITY_TYPE_MMU_GLB,
    _SOC_PARITY_TYPE_MMU_XPE,
    _SOC_PARITY_TYPE_MMU_SC,
    _SOC_PARITY_TYPE_SER
} _soc_th_ser_info_type_t;

typedef struct _soc_th_ser_route_block_s {
    uint8               cmic_reg; /* 3: intr3, 4: intr4, 5: intr5 */
    uint32              cmic_bit;
    soc_block_t         blocktype;
    int                 pipe;
    soc_reg_t           enable_reg;
    soc_reg_t           status_reg;
    soc_field_t         enable_field;
    void                *info;
    uint8               id;
} _soc_th_ser_route_block_t;

/* These are now generated from the regsfile processing into bcm56960_a0.c */
extern _soc_mem_ser_en_info_t _soc_bcm56960_a0_ip_mem_ser_info[];
extern _soc_reg_ser_en_info_t _soc_bcm56960_a0_ip_reg_ser_info[];
extern _soc_mem_ser_en_info_t _soc_bcm56960_a0_ep_mem_ser_info[];
extern _soc_reg_ser_en_info_t _soc_bcm56960_a0_ep_reg_ser_info[];
extern _soc_mem_ser_en_info_t _soc_bcm56960_a0_mmu_mem_ser_info[];
/* Above structures are defined in chip.h */

_soc_bus_ser_en_info_t _soc_th_ip_bus_ser_info[] = {
    { "IPARS", IPARS_SER_CONTROLr, IPARS_BUS_PARITY_ENf },
    { "IVXLT", IVXLT_SER_CONTROLr, IVXLT_BUS_PARITY_ENf },
    { "ISW1", ISW1_SER_CONTROLr, ISW1_BUS_PARITY_ENf },
    { "IRSEL1", IRSEL1_SER_CONTROLr, IRSEL1_BUS_PARITY_ENf },
    { "IFP", IFP_PARITY_CONTROLr, IFP_BUS_PARITY_ENf },
    { "IRSEL2", IRSEL2_SER_CONTROLr, IRSEL2_BUS_PARITY_ENf },
    { "ISW2", ISW2_SER_CONTROL_1r, ISW2_BUS_PARITY_ENf },
    { "IFWD", IFWD_SER_CONTROLr, IFWD_BUS_PAR_ENf },
    { "VP", VP_SER_CONTROLr, VP_BUS_PAR_ENf },
    { "IPARS_IVP_PT", IVXLT_SER_CONTROLr, PT_HWY_PARITY_ENf },
    { "IVP_ISW1_PT", IRSEL1_SER_CONTROLr, IVP_TO_ISW1_PT_HWY_PARITY_ENf },
    { "IFWD_ISW1_PT", IRSEL1_SER_CONTROLr, IFWD_TO_ISW1_PT_HWY_PARITY_ENf },
    { "ISW1_ISW2_PT", IRSEL2_SER_CONTROLr, ISW1_TO_ISW2_PT_HWY_PARITY_ENf },
    { "", INVALIDr }
};
_soc_bus_ser_en_info_t _soc_th_ep_bus_ser_info[] = {
    { "EVLAN", EGR_VLAN_SER_CONTROLr, EVLAN_BUS_PARITY_ENf },
    { "EHCPM", EGR_EHCPM_SER_CONTROLr, EHCPM_BUS_PARITY_ENf },
    { "EPMOD", EGR_EPMOD_SER_CONTROLr, EPMOD_BUS_PARITY_ENf },
    { "EFPPARS", EGR_EFPPARS_SER_CONTROLr, EFPPARS_BUS_PARITY_ENf },
    { "EFP", EFP_PARITY_CONTROLr, EFP_BUS_PARITY_ENf },
    { "EP_PT", EGR_VLAN_SER_CONTROLr, PT_HWY_PAR_ENf },
    { "IDB_OBMn_STATS_WIN_RES", IDB_SER_CONTROLr, OBM_MON_PAR_ENf },
    { "OBM_QUEUE_FIFO", IDB_OBM0_QUEUE_SER_CONTROLr, ECC_ENABLEf },
    { "OBM_DATA_FIFO", IDB_OBM0_SER_CONTROLr, DATA_ECC_ENABLEf },
    { "CELL_ASSEM_BUFFER", IDB_OBM0_SER_CONTROLr, CA_FIFO_ECC_ENABLEf },
    { "CPU_CELL_ASSEM_BUFFER", IDB_SER_CONTROLr, CA_CPU_ECC_ENABLEf },
    { "LPBK_CELL_ASSEM_BUFFER", IDB_SER_CONTROLr, CA_LPBK_ECC_ENABLEf },
    { "LEARN_FIFO", LEARN_FIFO_ECC_CONTROLr, EN_COR_ERR_RPTf },
    { "", INVALIDr }
};

_soc_buffer_ser_en_info_t _soc_th_ip_buffer_ser_info[] = {
    { "ISW3_EOP_BUFFER_A", ISW3_SER_CONTROL_1r, SW3_EOP_BUFFER_A_PARITY_ENf },
    { "ISW3_EOP_BUFFER_B", ISW3_SER_CONTROL_1r, SW3_EOP_BUFFER_B_PARITY_ENf },
    { "BUBBLE_MOP", ISW2_SER_CONTROL_0_64r, CPB_PARITY_ENf },
    { "METER_STAGING", IFP_PARITY_CONTROLr, METER_MUX_DATA_STAGING_PARITY_ENf },
    { "COUNTER_STAGING", IFP_PARITY_CONTROLr, 
      COUNTER_MUX_DATA_STAGING_PARITY_ENf },
    /* No other good place for these for now */
    { "IS_TDM_CALENDAR", IDB_SER_CONTROLr, IS_TDM_ECC_ENf },
    { "", INVALIDr }
};

_soc_buffer_ser_en_info_t _soc_th_ep_buffer_ser_info[] = {
    { "EDB_PM0_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM0_ECC_ENf },
    { "EDB_PM1_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM1_ECC_ENf },
    { "EDB_PM2_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM2_ECC_ENf },
    { "EDB_PM3_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM3_ECC_ENf },
    { "EDB_PM4_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM4_ECC_ENf },
    { "EDB_PM5_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM5_ECC_ENf },
    { "EDB_PM6_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM6_ECC_ENf },
    { "EDB_PM7_MEM", EGR_EDATABUF_PARITY_CONTROLr, PM7_ECC_ENf },
    { "EDB_CM_MEM", EGR_EDATABUF_PARITY_CONTROLr, CM_ECC_ENf },
    { "EDB_LBP_MEM", EGR_EDATABUF_PARITY_CONTROLr, LBP_ECC_ENf },
    { "EDB_PM0_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM0_ECC_ENf },
    { "EDB_PM1_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM1_ECC_ENf },
    { "EDB_PM2_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM2_ECC_ENf },
    { "EDB_PM3_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM3_ECC_ENf },
    { "EDB_PM4_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM4_ECC_ENf },
    { "EDB_PM5_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM5_ECC_ENf },
    { "EDB_PM6_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM6_ECC_ENf },
    { "EDB_PM7_CTRL", EGR_EDB_CTRL_PARITY_ENr, PM7_ECC_ENf },
    { "EDB_CM_CTRL", EGR_EDB_CTRL_PARITY_ENr, CM_ECC_ENf },
    { "EDB_LBP_CTRL", EGR_EDB_CTRL_PARITY_ENr, LBP_ECC_ENf },
    { "INITBUF", EGR_EL3_ECC_PARITY_CONTROLr, INITBUF_ECC_ENf },
    { "EGR_MPB", EGR_EL3_ECC_PARITY_CONTROLr, EGR_MPB_ECC_ENf },
    { "EP_EDB_PM_RESI_BUFF", EGR_EDATABUF_PARITY_CONTROLr,
      PM_RESI_BUFFER_PAR_ENf },
    { "", INVALIDr }
};

typedef struct _soc_th_ser_reg_s {
    soc_reg_t reg;
    char      *mem_str;
} _soc_th_ser_reg_t;

typedef struct _soc_th_ser_info_s {
    _soc_th_ser_info_type_t   type;
    struct _soc_th_ser_info_s *info;
    int                       id;
    soc_field_t               group_reg_enable_field;
    soc_field_t               group_reg_status_field;
    soc_mem_t                 mem;
    char                      *mem_str;
    soc_reg_t                 enable_reg;
    soc_field_t               enable_field;
    soc_field_t               *enable_field_list;
    soc_reg_t                 intr_enable_reg;
    soc_field_t               intr_enable_field;
    soc_field_t               *intr_enable_field_list;
    soc_reg_t                 intr_status_reg;
    _soc_th_ser_reg_t         *intr_status_reg_list;
    soc_reg_t                 intr_status_field;
    soc_field_t               *intr_status_field_list;
    soc_reg_t                 intr_clr_reg;
    soc_field_t               intr_clr_field;
    soc_field_t               *intr_clr_field_list;
} _soc_th_ser_info_t;

#define _SOC_TH_SER_TYPE_REG 0
#define _SOC_TH_SER_TYPE_MEM 1
#define _SOC_TH_SER_TYPE_BUS 2
#define _SOC_TH_SER_TYPE_BUF 3

typedef struct _soc_th_ser_block_info_s {
    int         type;
    soc_block_t blocktype;
    char        name[16];
    soc_reg_t   fifo_reset_reg;
    void        *info;
} _soc_th_ser_block_info_t;

static soc_ser_functions_t _th_ser_functions;

static const
_soc_th_ser_block_info_t _soc_th_ser_block_info[] = {
    { _SOC_TH_SER_TYPE_REG, SOC_BLK_IPIPE, "IPIPE regs", INVALIDr,
      _soc_bcm56960_a0_ip_reg_ser_info },
    { _SOC_TH_SER_TYPE_MEM, SOC_BLK_IPIPE, "IPIPE mems", ING_SER_FIFO_CTRLr,
      _soc_bcm56960_a0_ip_mem_ser_info },
    { _SOC_TH_SER_TYPE_BUS, SOC_BLK_IPIPE, "IPIPE buses", INVALIDr,
      _soc_th_ip_bus_ser_info },
    { _SOC_TH_SER_TYPE_BUF, SOC_BLK_IPIPE, "IPIPE buffers", INVALIDr,
      _soc_th_ip_buffer_ser_info },
    { _SOC_TH_SER_TYPE_REG, SOC_BLK_EPIPE, "EPIPE regs", INVALIDr,
      _soc_bcm56960_a0_ep_reg_ser_info },
    { _SOC_TH_SER_TYPE_MEM, SOC_BLK_EPIPE, "EPIPE mems", EGR_SER_FIFO_CTRLr,
      _soc_bcm56960_a0_ep_mem_ser_info },
    { _SOC_TH_SER_TYPE_BUS, SOC_BLK_EPIPE, "EPIPE buses", INVALIDr,
      _soc_th_ep_bus_ser_info },
    { _SOC_TH_SER_TYPE_BUF, SOC_BLK_EPIPE, "EPIPE buffers", INVALIDr,
      _soc_th_ep_buffer_ser_info },
    { _SOC_TH_SER_TYPE_MEM, SOC_BLK_MMU_GLB, "MMU mems", INVALIDr,
      _soc_bcm56960_a0_mmu_mem_ser_info },
    /* xsm: we could have used MMU_GLB or MMU_XPE or MMU_SC */
    { -1, 0 }
};

static soc_field_t _soc_th_mmu_xpe_enable_fields[] = {
    THDM_PARITY_ENf,
    ENQX_PARITY_ENf,
    EPRG_PARITY_ENf,
    RQE_PARITY_ENf,
    THDI_PARITY_ENf,
    THDU_PARITY_ENf,
    PQE_PARITY_ENf,
    WRED_PARITY_ENf,
    DQX_PARITY_ENf,
    CCP_PARITY_ENf,
    CTR_PARITY_ENf,
    INVALIDf
};

static soc_field_t _soc_th_mmu_sc_enable_fields[] = {
    MB_A_PARITY_ENf,
    MB_B_PARITY_ENf,
    TDM_PARITY_ENf,
    ENQS_PARITY_ENf,
    MTRO_PARITY_ENf,
    VBS_PARITY_ENf,
    DQS_PARITY_ENf,
    TOQ_B_PARITY_ENf,
    TOQ_A_PARITY_ENf,
    CFAP_B_PARITY_ENf,
    CFAP_A_PARITY_ENf,
    INVALIDf
};
static soc_field_t _soc_th_mmu_sc_int_enable_fields[] = {
    MEM_PAR_ERR_ENf,
    CFAP_B_MEM_FAIL_ENf,
    CFAP_A_MEM_FAIL_ENf,
    INVALIDf
};
/*static soc_field_t _soc_th_mmu_sc_status_fields[] = {
    MEM_PAR_ERR_STATf,
    CFAP_B_MEM_FAIL_STATf,
    CFAP_A_MEM_FAIL_STATf,
    INVALIDf
};
static soc_field_t _soc_th_mmu_sc_clr_fields[] = {
    MEM_PAR_ERR_CLRf,
    CFAP_B_MEM_FAIL_CLRf,
    CFAP_A_MEM_FAIL_CLRf,
    INVALIDf
};*/

static _soc_th_ser_info_t _soc_th_mmu_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_GLB, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU GLB MEM PAR",
        MMU_GCFG_PARITY_ENr, INTFO_PARITY_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_GCFG_GLB_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_GCFG_GLB_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_XPE, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU XPE MEM PAR",
        MMU_XCFG_PARITY_ENr, INVALIDf, _soc_th_mmu_xpe_enable_fields,
        MMU_XCFG_XPE_CPU_INT_ENr, MEM_PAR_ERR_ENf, NULL,
        MMU_XCFG_XPE_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf, NULL,
        MMU_XCFG_XPE_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf, NULL
    },
    { _SOC_PARITY_TYPE_MMU_SC, NULL, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU SC MEM PAR",
        MMU_SCFG_PARITY_ENr, INVALIDf, _soc_th_mmu_sc_enable_fields,
        MMU_SCFG_SC_CPU_INT_ENr, INVALIDf, _soc_th_mmu_sc_int_enable_fields,
        MMU_SCFG_SC_CPU_INT_STATr, NULL, MEM_PAR_ERR_STATf/*INVALIDf*/, 
        NULL/*_soc_th_mmu_sc_status_fields*/,
        MMU_SCFG_SC_CPU_INT_CLEARr, MEM_PAR_ERR_CLRf/*INVALIDf*/, 
        NULL/*_soc_th_mmu_sc_clr_fields*/
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static soc_field_t _soc_th_mmu_top_int_enable_fields[] = {
    SC_R_CPU_INT_ENf,
    SC_S_CPU_INT_ENf,
    XPE_R_B_CPU_INT_ENf,
    XPE_R_A_CPU_INT_ENf,
    XPE_S_B_CPU_INT_ENf,
    XPE_S_A_CPU_INT_ENf,
    GLB_CPU_INT_ENf,
    INVALIDf
};

static soc_field_t _soc_th_mmu_top_int_status_fields[] = {
    SC_R_CPU_INT_STATf,
    SC_S_CPU_INT_STATf,
    XPE_R_B_CPU_INT_STATf,
    XPE_R_A_CPU_INT_STATf,
    XPE_S_B_CPU_INT_STATf,
    XPE_S_A_CPU_INT_STATf,
    GLB_CPU_INT_STATf,
    INVALIDf
};

static _soc_th_ser_info_t _soc_th_mmu_top_ser_info[] = {
    { _SOC_PARITY_TYPE_MMU_SER, _soc_th_mmu_ser_info, 0,
        INVALIDf, INVALIDf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf, NULL,
        MMU_GCFG_ALL_CPU_INT_ENr, INVALIDf,
        _soc_th_mmu_top_int_enable_fields,
        MMU_GCFG_ALL_CPU_INT_STATr,
        NULL, INVALIDf, _soc_th_mmu_top_int_status_fields,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static const
_soc_th_ser_mmu_intr_info_t mmu_intr_info[] = {
    { GLB_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_GLB, 0 },
    { XPE_R_A_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE0, 1 },
    { XPE_S_A_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE1, 1 },
    { XPE_R_B_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE2, 1 },
    { XPE_S_B_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_XPE3, 1 },
    { SC_R_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC0, 2 },
    { SC_S_CPU_INT_STATf, _SOC_MMU_BASE_INDEX_SC1, 2},
    { INVALIDf, -1, -1 } 
};

static _soc_th_ser_reg_t _soc_th_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC" },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC" }
};

static _soc_th_ser_reg_t _soc_th_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC" },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC" }
};

static _soc_th_ser_reg_t _soc_th_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC" }
};

static _soc_th_ser_reg_t _soc_th_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" }
};

static _soc_th_ser_info_t _soc_th_pm_clp_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MAC_RX_TS_CDC_MEM_ERRf, MAC_RX_TS_CDC_MEM_ERRf,
        INVALIDm, "MAC RX TimeStamp CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_RX_TS_MEM_ECC_STATUSr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MAC_RX_CDC_MEM_ERRf, MAC_RX_CDC_MEM_ERRf,
        INVALIDm, "MAC RX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_RX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MAC_TX_CDC_MEM_ERRf, MAC_TX_CDC_MEM_ERRf,
        INVALIDm, "MAC TX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        CLMAC_TX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_clp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_clp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_th_ser_info_t _soc_th_pm_xlp_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MAC_RX_CDC_MEM_ERRf, MAC_RX_CDC_MEM_ERRf,
        INVALIDm, "MAC RX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_RX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MAC_TX_CDC_MEM_ERRf, MAC_TX_CDC_MEM_ERRf,
        INVALIDm, "MAC TX CDC memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        XLMAC_TX_CDC_ECC_STATUSr, NULL, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
        INVALIDm, "MIB RX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_xlp_rx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
        INVALIDm, "MIB TX Statistic Counter memory",
        XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_pm_xlp_tx_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_th_ser_reg_t _soc_th_idb_status_reg[] = {
    { IDB_CA_CPU_ECC_STATUSr, "CA-CPU packet buffer memory" },
    { IDB_CA_LPBK_ECC_STATUSr, "CA-LPBK packet buffer memory" },
    { IDB_IS_TDM_CAL_ECC_STATUSr, "IDB IS TDM calendar" },
    { IDB_OBM0_DATA_ECC_STATUSr, "IDB OBM packet data memory" },
    { IDB_OBM0_QUEUE_ECC_STATUSr, "IDB OBM queue FIFO memory" },
    { IDB_OBM0_CA_ECC_STATUSr, "IDB CA packet buffer memory" }
};

static _soc_th_ser_info_t _soc_th_idb_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0, 
        MEM_ERROR_OCCURREDf, MEM_ERROR_OCCURREDf,
        INVALIDm, "IP IDB memory",
        INVALIDr, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL,
        INVALIDr, _soc_th_idb_status_reg, INVALIDf, NULL,
        INVALIDr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

/* Setup dynamically in enable ser */
static uint32 _soc_th_ip_pipe_fifo_bmask[] = {
    0, 0, 0, 0
};

static const
_soc_th_ser_route_block_t  _soc_th_ser_route_blocks[] = {
    { 3, 0x00000010, /* MMU_TO_CMIC_MEMFAIL_INTR */
      SOC_BLK_MMU, -1, INVALIDr, INVALIDr, INVALIDf,
      _soc_th_mmu_top_ser_info, 0 },
    /* xsm: not using MMU_GLB or MMU_XPE or MMU_SC is on purpose */
    { 3, 0x00000020, /* PIPE0_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 0, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000040, /* PIPE1_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 1, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000080, /* PIPE2_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 2, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000100, /* PIPE3_EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, 3, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf,
      NULL, 0 },
    { 3, 0x00000200, /* PIPE0_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 0, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, 0x00000400, /* PIPE1_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 1, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, 0x00000800, /* PIPE2_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 2, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, 0x00001000, /* PIPE3_IP0_TO_CMIC_PERR_INTR */
      /* Note: This is an exception for being legacy style in IP */
      SOC_BLK_IPIPE, 3, IP0_INTR_ENABLEr, IP0_INTR_STATUSr, INVALIDf,
      _soc_th_idb_ser_info, 0 },
    { 3, 0x00002000, /* PIPE0_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 0, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00004000, /* PIPE1_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 1, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00008000, /* PIPE2_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 2, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 3, 0x00010000, /* PIPE3_IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, 3, INVALIDr, INVALIDr, INVALIDf,
      NULL, 0 },
    { 4, 0x00000001, /* PM0_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 0 },
    { 4, 0x00000002, /* PM1_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 1 },
    { 4, 0x00000004, /* PM2_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 2 },
    { 4, 0x00000008, /* PM3_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 3 },
    { 4, 0x00000010, /* PM4_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 4 },
    { 4, 0x00000020, /* PM5_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 5 },
    { 4, 0x00000040, /* PM6_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 6 },
    { 4, 0x00000080, /* PM7_INTR */
      SOC_BLK_CLPORT, 0, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 7 },
    { 4, 0x00000100, /* PM8_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 8 },
    { 4, 0x00000200, /* PM9_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 9 },
    { 4, 0x00000400, /* PM10_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 10 },
    { 4, 0x00000800, /* PM11_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 11 },
    { 4, 0x00001000, /* PM12_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 12 },
    { 4, 0x00002000, /* PM13_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 13 },
    { 4, 0x00004000, /* PM14_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 14 },
    { 4, 0x00008000, /* PM15_INTR */
      SOC_BLK_CLPORT, 1, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 15 },
    { 4, 0x00010000, /* PM16_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 16 },
    { 4, 0x00020000, /* PM17_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 17 },
    { 4, 0x00040000, /* PM18_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 18 },
    { 4, 0x00080000, /* PM19_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 19 },
    { 4, 0x00100000, /* PM20_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 20 },
    { 4, 0x00200000, /* PM21_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 21 },
    { 4, 0x00400000, /* PM22_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 22 },
    { 4, 0x00800000, /* PM23_INTR */
      SOC_BLK_CLPORT, 2, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 23 },
    { 4, 0x01000000, /* PM24_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 24 },
    { 4, 0x02000000, /* PM25_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 25 },
    { 4, 0x04000000, /* PM26_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 26 },
    { 4, 0x08000000, /* PM27_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 27 },
    { 4, 0x10000000, /* PM28_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 28 },
    { 4, 0x20000000, /* PM29_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 29 },
    { 4, 0x40000000, /* PM30_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 30 },
    { 4, 0x80000000, /* PM31_INTR */
      SOC_BLK_CLPORT, 3, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_clp_ser_info, 31 },
    { 5, 0x00000001, /* PM32_INTR */
      SOC_BLK_XLPORT, 1, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
      _soc_th_pm_xlp_ser_info, 0 },
    { 0 } /* table terminator */
};

STATIC int
_soc_tomahawk_mmu_ser_config(int unit, _soc_th_ser_info_t *info_list,
                             int enable)
{
    int info_index, f, rv;
    soc_field_t *fields;
    uint32 rval;
    _soc_th_ser_info_t *info;

    switch(info_list->type) {
    case _SOC_PARITY_TYPE_MMU_SER:
        /* Interrupt enable */
        fields = info_list->intr_enable_field_list;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                          &rval));
        for (f=0 ;; f++) {
            if (fields[f] == INVALIDf) {
                break;
            }
            soc_reg_field_set(unit, info_list->intr_enable_reg, &rval,
                              fields[f], enable ? 1 : 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                           rval));
        /* Loop through each info entry in the list */
        for (info_index = 0; ; info_index++) {
            info = &(info_list->info[info_index]);
            if (info->type == _SOC_PARITY_TYPE_NONE) {
                /* End of table */
                break;
            }
            rv = _soc_tomahawk_mmu_ser_config(unit, info, enable);
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Unit %d: Error configuring %s !!\n"), unit,
                         info->mem_str));
                return rv;
            }
        }
        break;
    case _SOC_PARITY_TYPE_MMU_GLB:
    case _SOC_PARITY_TYPE_MMU_XPE:
    case _SOC_PARITY_TYPE_MMU_SC:
        /* Parity enable */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->enable_reg, REG_PORT_ANY, 0,
             &rval));
        if (info_list->enable_field_list) {
            fields = info_list->enable_field_list;
            for (f=0 ;; f++) {
                if (fields[f] == INVALIDf) {
                    break;
                }
                soc_reg_field_set(unit, info_list->enable_reg, &rval, fields[f],
                                  enable ? 1 : 0);
            }
        } else if (info_list->enable_reg != INVALIDr) {
            soc_reg_field_set(unit, info_list->enable_reg, &rval,
                              info_list->enable_field, enable ? 1 : 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, info_list->enable_reg, REG_PORT_ANY, 0, rval));
        /* Interrupt enable */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                           &rval));
        if (info_list->intr_enable_field_list) {
            fields = info_list->intr_enable_field_list;
            for (f=0 ;; f++) {
                if (fields[f] == INVALIDf) {
                    break;
                }
                soc_reg_field_set(unit, info_list->intr_enable_reg, &rval,
                                  fields[f], enable ? 1 : 0);
            }
        } else if (info_list->intr_enable_reg != INVALIDr) {
            soc_reg_field_set(unit, info_list->intr_enable_reg, &rval,
                              info_list->intr_enable_field, enable ? 1 : 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, info_list->intr_enable_reg, REG_PORT_ANY, 0,
                           rval));
        break;
    default: break;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                              soc_reg_t group_reg, uint64 *group_rval,
                              _soc_th_ser_info_t *info_list,
                              soc_mem_t mem, int enable)
{
    _soc_th_ser_info_t *info;
    int info_index, rv, rv1;
    uint32 rval;
    soc_reg_t reg;
    uint64 rval64;
    rv = SOC_E_NOT_FOUND;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }
        rv = SOC_E_NONE;

        if (group_reg != INVALIDr && info->group_reg_enable_field != INVALIDf) {
            /* Enable the info entry in the group register */
            soc_reg64_field32_set(unit, group_reg, group_rval,
                                  info->group_reg_enable_field, enable ? 1 : 0);
        }
        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_MMU_SER:
            rv1 = _soc_tomahawk_mmu_ser_config(unit, info, enable);
            if (SOC_FAILURE(rv1)) {
                LOG_CLI((BSL_META_U(unit,
                                    "Unit %d: Error configuring MMU SER !!\n"), unit));
                return rv1;
            }
            break;
        case _SOC_PARITY_TYPE_ECC:
            reg = info->enable_reg;
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, info->enable_field,
                                      enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                  enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                        (BSL_META_U(unit,
                                    "SER enable for: %s\n"), 
                         (info->mem == INVALIDm) ? info->mem_str : 
                         SOC_MEM_NAME(unit, info->mem)));
            break;
        default: break;
        }
    }
    return rv;
}


int
soc_tomahawk_ser_enable_all(int unit, int enable)
{
    uint8                           rbi, bcount, type;
    uint16                          pcount;
    uint32                          rval, cmic_bit;
    uint32                          cmic_rval3, cmic_rval4, cmic_rval5;
    int                             port = REG_PORT_ANY;
    int                             rv, block_info_idx;
    uint64                          rval64;
    soc_reg_t                       reg, ecc1b_reg;
    soc_field_t                     field, ecc1b_field;
    _soc_reg_ser_en_info_t          *reg_info;
    _soc_mem_ser_en_info_t          *mem_info;
    _soc_bus_ser_en_info_t          *bus_info;
    _soc_buffer_ser_en_info_t       *buf_info;
    char                            *str_type;
    char                            *str_name;
    const _soc_th_ser_route_block_t *rb;
    static char *parity_module_str[4] = {"REG", "MEM", "BUS", "BUF"};

    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK3r(unit, &cmic_rval3));
    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK4r(unit, &cmic_rval4));
    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK5r(unit, &cmic_rval5));
    /* Enable new fifo mechanism based SER stuff */
    for (bcount = 0; _soc_th_ser_block_info[bcount].blocktype; bcount++) {
        int done = 0;

        LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                    (BSL_META_U(unit,
                                "Unit %d SER enable for: %s\n"),
                     unit, _soc_th_ser_block_info[bcount].name));
        type = _soc_th_ser_block_info[bcount].type;
        reg_info = (_soc_reg_ser_en_info_t*)_soc_th_ser_block_info[bcount].info;
        mem_info = (_soc_mem_ser_en_info_t*)_soc_th_ser_block_info[bcount].info;
        bus_info = (_soc_bus_ser_en_info_t*)_soc_th_ser_block_info[bcount].info;
        buf_info = (_soc_buffer_ser_en_info_t*)
                   _soc_th_ser_block_info[bcount].info;
        for (pcount = 0;;pcount++) { /* walk thro each entry of block_info */
            ecc1b_reg = INVALIDr;    
            ecc1b_field = INVALIDf;
            switch (type) {
            case _SOC_TH_SER_TYPE_REG: 
                if (reg_info[pcount].reg == INVALIDr) { 
                    done = 1;
                } else {
                    reg = reg_info[pcount].en_reg;
                    field = reg_info[pcount].en_fld;
                    str_name = SOC_REG_NAME(unit, reg_info[pcount].reg);
                }
                break;
            case _SOC_TH_SER_TYPE_MEM:
                if (mem_info[pcount].mem == INVALIDm) { 
                    done = 1;
                } else if ((mem_info[pcount].mem == TRUNK_RR_CNTm) ||
                           (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE0m) ||
                           (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE1m) ||
                           (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE2m) ||
                           (mem_info[pcount].mem == TRUNK_RR_CNT_PIPE3m)) {
                    continue;
                } else {
                    reg = mem_info[pcount].en_reg;
                    field = mem_info[pcount].en_fld;
                    ecc1b_reg = mem_info[pcount].ecc_1b_report_en_reg;
                    ecc1b_field = mem_info[pcount].ecc_1b_report_en_fld;
                    str_name = SOC_MEM_NAME(unit, mem_info[pcount].mem);

#ifdef _SOC_SER_ENABLE_CLI_DBG
                    if ((ecc1b_reg != INVALIDr) &&
                        ((20 == SOC_REG_ACC_TYPE(unit, ecc1b_reg)) ||
                         (SOC_REG_ACC_TYPE(unit, ecc1b_reg) < 4))) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Unit %d: For mem %s, ecc1b_reg %s"
                                            " has acc_type = %d !!\n"),
                                unit, str_name, SOC_REG_NAME(unit, ecc1b_reg),
                                SOC_REG_ACC_TYPE(unit, ecc1b_reg)));
                    }
#endif /* _SOC_SER_ENABLE_CLI_DBG */
                    
                }
                break;
            case _SOC_TH_SER_TYPE_BUS:
                if (bus_info[pcount].en_reg == INVALIDr) { 
                    done = 1;
                } else {
                    reg = bus_info[pcount].en_reg;
                    field = bus_info[pcount].en_fld;
                    str_name = bus_info[pcount].bus_name;
                }
                break;
            case _SOC_TH_SER_TYPE_BUF:
                if (buf_info[pcount].en_reg == INVALIDr) {
                    done = 1;
                } else {
                    reg = buf_info[pcount].en_reg;
                    field = buf_info[pcount].en_fld;
                    str_name = buf_info[pcount].buffer_name;
                }
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Unknown parity module "
                                    "[bcount: %d][pcount: %d].\n"), bcount, pcount));
                return SOC_E_INTERNAL;
            }
            if (done) { 
                break;
            } else {
                str_type = parity_module_str[type];
            }
            /* NOTE: Do not use the field modify routine in the following as
                     some regs do not return the correct value due to which the
                     modify routine skips the write */
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, field, 
                                      enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
            if (ecc1b_reg != INVALIDr && ecc1b_field != INVALIDf) {
                if (SOC_REG_IS_64(unit, ecc1b_reg)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, ecc1b_reg, port, 0, &rval64));
                    soc_reg64_field32_set(unit, ecc1b_reg, &rval64, ecc1b_field,
                                          enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, ecc1b_reg, port, 0, rval64));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, ecc1b_reg, port, 0, &rval));
                    soc_reg_field_set(unit, ecc1b_reg, &rval, ecc1b_field, 
                                      enable ? 1 : 0);
                    if (20 == SOC_REG_ACC_TYPE(unit, ecc1b_reg)) {
                        /* ??? What is 20? ACC_TYPE_SINGLE
                         *     20 means not bcst - so need to write to each
                         *     instances
                         *     Check to see if some ecc1b_reg is DUPLICATE,
                         *     UNIQUE - basically we need to make sure all
                         *     instances of this register get 'enable = 1'
                         *     Is following correct way to make sure all
                         *     instances of reg get enable?
                         */
                        int inst, pipe;
                        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
                            inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, ecc1b_reg, inst, 0, rval));
                        }
                    } else {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, ecc1b_reg, port, 0, rval));
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                        (BSL_META_U(unit,
                                    "SER enable for %s: %s\n"),
                         str_type, str_name));
        } /* walk thro each entry of block_info */

        /* Loop through each place-and-route block entry */
        for (rbi = 0; ; rbi++) {
            rb = &_soc_th_ser_route_blocks[rbi];
            cmic_bit = rb->cmic_bit;
            if (cmic_bit == 0) {
                /* End of table */
                break;
            }
            if (rb->blocktype == _soc_th_ser_block_info[bcount].blocktype) {
                /* LHS has only SOC_BLK_MMU and * RHS has only
                 * SOC_BLK_MMU_GLB. Thus for MMU, following will not be
                 * executed.
                 */
                /* New SER mechanism (except IP.IDB) */
                cmic_rval3 |= cmic_bit;
                /* block_info has entries only for BLK_IPIPE, BLK_EPIPE,
                 * BLK_MMU_GLB.
                 * route_block_info on other hand has BLK_MMU, BLK_IPIPE,
                 * BLK_EPIPE, BLK_CLPORT.
                 * So, we can he here only for BLK_IPIPE, BLK_EPIPE and for
                 * these cmic reg is always cmic_rval3 - see block_info[] array
                 */
                if (rb->enable_reg != INVALIDr && 
                    rb->enable_field != INVALIDf) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, rb->enable_reg,
                            REG_PORT_ANY, rb->enable_field, enable ? 1 : 0));
                }
                if (rb->blocktype == SOC_BLK_IPIPE) {
                    _soc_th_ip_pipe_fifo_bmask[rb->pipe] |= cmic_bit;
                }
            }
        }

        /* reset (toggle) fifo if applicable */
        if (_soc_th_ser_block_info[bcount].fifo_reset_reg != INVALIDr) {
            SOC_IF_ERROR_RETURN
                 (soc_reg_field32_modify(unit,
                     _soc_th_ser_block_info[bcount].fifo_reset_reg,
                                         REG_PORT_ANY, FIFO_RESETf, 1));
            SOC_IF_ERROR_RETURN
                 (soc_reg_field32_modify(unit,
                     _soc_th_ser_block_info[bcount].fifo_reset_reg,
                                         REG_PORT_ANY, FIFO_RESETf, 0));
        }
    }
    /* Enable 1B error reporting for some special items */

    /* Loop through each place-and-route block entry to enable legacy 
       style SER stuff */
    /* BLK_MMU, BLK_CLPORT will be handled here */ 
    for (rbi = 0; ; rbi++) {
        rb = &_soc_th_ser_route_blocks[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (rb->info == NULL) {
            continue;
        }
        if (enable) {
            switch (rb->cmic_reg) {
            case 3: cmic_rval3 |= cmic_bit; break;
            case 4: cmic_rval4 |= cmic_bit; break;
            case 5: cmic_rval5 |= cmic_bit; break;
            default: break;
            }
        }

        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }
        if (rb->enable_reg != INVALIDr) {
            if (SOC_BLOCK_IN_LIST(SOC_REG_INFO(unit, rb->enable_reg).block,
                SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                    /* This port block is not configured */
                    continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, port, 0, &rval64));
        }

        rv = _soc_tomahawk_ser_enable_info(unit, block_info_idx, rb->id, port,
                                           rb->enable_reg, &rval64,
                                           rb->info, INVALIDm, enable);
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }

        if (rb->enable_reg != INVALIDr) {
            /* Write per route block parity enable register */
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rval64));
        }
    }

    if (enable) {
        /* MMU enables */
        /* check trident2.c - may have to add some similar code here */

        /* Write CMIC enable register */
        (void)soc_cmicm_intr3_enable(unit, cmic_rval3);
        (void)soc_cmicm_intr4_enable(unit, cmic_rval4);
        (void)soc_cmicm_intr5_enable(unit, cmic_rval5);
    } else {
        /* MMU disables */
        /* check trident2.c - may have to add some similar code here */

        /* Write CMIC disable register */
        (void)soc_cmicm_intr3_disable(unit, cmic_rval3);
        (void)soc_cmicm_intr4_disable(unit, cmic_rval4);
        (void)soc_cmicm_intr5_disable(unit, cmic_rval5);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th_ser_reg32_get(int unit, soc_reg_t reg, int port, int index,
                      uint32 *data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
       return soc_tomahawk_xpe_reg32_get(unit, reg, block_num,
                                         base_type_index, index, data);
       break;
    case SOC_BLK_MMU_SC:
       return soc_tomahawk_sc_reg32_get(unit, reg, block_num,
                                        base_type_index, index, data);
       break;
    default:
       return soc_reg32_get(unit, reg, port, index, data);
       break;
    }
}

STATIC int
_soc_th_ser_reg32_set(int unit, soc_reg_t reg, int port, int index,
                      uint32 data, int mmu_base_index)
{
    int block_num = mmu_base_index;
    int base_type_index = mmu_base_index;
    soc_block_t blocktype = *SOC_REG_INFO(unit, reg).block;
    switch (blocktype) {
    case SOC_BLK_MMU_XPE:
       return soc_tomahawk_xpe_reg32_set(unit, reg, block_num,
                                         base_type_index, index, data);
       break;
    case SOC_BLK_MMU_SC:
       return soc_tomahawk_sc_reg32_set(unit, reg, block_num,
                                        base_type_index, index, data);
       break;
    default:
       return soc_reg32_set(unit, reg, port, index, data);
       break;
    }
}

STATIC int
_soc_th_ser_reg_field32_modify(int unit, soc_reg_t reg, soc_port_t port,
                               soc_field_t field, uint32 value,
                               int index, int mmu_base_index)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN
        (_soc_th_ser_reg32_get(unit, reg, port, index, &rval, mmu_base_index));
    soc_reg_field_set(unit, reg, &rval, field, value);
    SOC_IF_ERROR_RETURN
        (_soc_th_ser_reg32_set(unit, reg, port, index, rval, mmu_base_index));
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_mmu_mem_remap(_soc_ser_correct_info_t *spci)
{
    if (spci == NULL) {
        return SOC_E_PARAM;
    }

    /* LHS views are physical views of mem instances which encounter error and
     * are thus reported by HW.
     *
     * RHS views are logical views for which SW maintains cache and does
     * ser_correction writes.
     *
     * remap function thus maps physical mem views to logical mem views.
     */

    switch (spci->mem) {
    /* WRED */
    case MMU_WRED_DROP_CURVE_PROFILE_0_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_0_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_0m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_1_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_1_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_1m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_2_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_2_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_2m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_3_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_3_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_3m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_4_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_4_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_4m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_5_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_5_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_5m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_6_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_6_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_6m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_7_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_7_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_7m; break;
    case MMU_WRED_DROP_CURVE_PROFILE_8_Am:
    case MMU_WRED_DROP_CURVE_PROFILE_8_Bm:
        spci->mem = MMU_WRED_DROP_CURVE_PROFILE_8m; break;


    /* THDU */
    case MMU_THDU_RESUME_PORT0_PIPE0m:
    case MMU_THDU_RESUME_PORT1_PIPE0m:
    case MMU_THDU_RESUME_PORT2_PIPE0m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE0m; break;
    case MMU_THDU_RESUME_PORT0_PIPE1m:
    case MMU_THDU_RESUME_PORT1_PIPE1m:
    case MMU_THDU_RESUME_PORT2_PIPE1m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE1m; break;
    case MMU_THDU_RESUME_PORT0_PIPE2m:
    case MMU_THDU_RESUME_PORT1_PIPE2m:
    case MMU_THDU_RESUME_PORT2_PIPE2m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE2m; break;
    case MMU_THDU_RESUME_PORT0_PIPE3m:
    case MMU_THDU_RESUME_PORT1_PIPE3m:
    case MMU_THDU_RESUME_PORT2_PIPE3m:
        spci->mem = MMU_THDU_RESUME_PORT_PIPE3m; break;

    case MMU_THDU_CONFIG_PORT0_PIPE0m:
    case MMU_THDU_CONFIG_PORT1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE0m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE1m:
    case MMU_THDU_CONFIG_PORT1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE1m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE2m:
    case MMU_THDU_CONFIG_PORT1_PIPE2m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE2m; break;
    case MMU_THDU_CONFIG_PORT0_PIPE3m:
    case MMU_THDU_CONFIG_PORT1_PIPE3m:
        spci->mem = MMU_THDU_CONFIG_PORT_PIPE3m; break;

    case MMU_THDU_CONFIG_QGROUP0_PIPE0m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE0m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE1m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE1m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE2m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE2m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE2m; break;
    case MMU_THDU_CONFIG_QGROUP0_PIPE3m:
    case MMU_THDU_CONFIG_QGROUP1_PIPE3m:
        spci->mem = MMU_THDU_CONFIG_QGROUP_PIPE3m; break;

    case MMU_THDU_OFFSET_QGROUP0_PIPE0m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE0m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE0m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE1m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE1m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE1m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE2m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE2m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE2m; break;
    case MMU_THDU_OFFSET_QGROUP0_PIPE3m:
    case MMU_THDU_OFFSET_QGROUP1_PIPE3m:
        spci->mem = MMU_THDU_OFFSET_QGROUP_PIPE3m; break;

    case MMU_THDU_CONFIG_QUEUE0_PIPE0m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE0m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE0m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE1m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE1m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE1m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE2m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE2m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE2m; break;
    case MMU_THDU_CONFIG_QUEUE0_PIPE3m:
    case MMU_THDU_CONFIG_QUEUE1_PIPE3m:
        spci->mem = MMU_THDU_CONFIG_QUEUE_PIPE3m; break;

    case MMU_THDU_OFFSET_QUEUE0_PIPE0m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE0m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE0m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE1m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE1m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE1m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE2m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE2m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE2m; break;
    case MMU_THDU_OFFSET_QUEUE0_PIPE3m:
    case MMU_THDU_OFFSET_QUEUE1_PIPE3m:
        spci->mem = MMU_THDU_OFFSET_QUEUE_PIPE3m; break;

    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE0m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE0m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE0m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE0m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE1m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE1m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE1m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE1m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE2m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE2m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE2m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE2m; break;
    case MMU_THDU_Q_TO_QGRP_MAP0_PIPE3m:
    case MMU_THDU_Q_TO_QGRP_MAP1_PIPE3m:
    case MMU_THDU_Q_TO_QGRP_MAP2_PIPE3m:
        spci->mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE3m; break;


    /* THDI */
    case THDI_PORT_SP_CONFIG0_PIPE0m:
    case THDI_PORT_SP_CONFIG1_PIPE0m:
    case THDI_PORT_SP_CONFIG2_PIPE0m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE0m; break;
    case THDI_PORT_SP_CONFIG0_PIPE1m:
    case THDI_PORT_SP_CONFIG1_PIPE1m:
    case THDI_PORT_SP_CONFIG2_PIPE1m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE1m; break;
    case THDI_PORT_SP_CONFIG0_PIPE2m:
    case THDI_PORT_SP_CONFIG1_PIPE2m:
    case THDI_PORT_SP_CONFIG2_PIPE2m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE2m; break;
    case THDI_PORT_SP_CONFIG0_PIPE3m:
    case THDI_PORT_SP_CONFIG1_PIPE3m:
    case THDI_PORT_SP_CONFIG2_PIPE3m:
        spci->mem = THDI_PORT_SP_CONFIG_PIPE3m; break;


    /* MTRO */
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE0m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE0m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE0m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE1m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE1m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE1m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE2m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE2m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE2m; break;
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_A_PIPE3m:
    case MMU_MTRO_EGRMETERINGCONFIG_MEM_B_PIPE3m:
        spci->mem = MMU_MTRO_EGRMETERINGCONFIG_MEM_PIPE3m; break;


    /* THDM */
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE1m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE2m; break;
    case MMU_THDM_MCQE_QUEUE_CONFIG_A_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_B_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_MCQE_QUEUE_CONFIG_PIPE3m; break;

    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE0m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE0m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE0m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE1m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE1m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE1m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE2m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE2m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE2m; break;
    case MMU_THDM_DB_QUEUE_CONFIG_A_PIPE3m:
    case MMU_THDM_DB_QUEUE_CONFIG_B_PIPE3m:
    case MMU_THDM_DB_QUEUE_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE3m; break;

    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE1m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE1m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE1m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE2m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE2m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE2m; break;
    case MMU_THDM_MCQE_PORTSP_CONFIG_A_PIPE3m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_B_PIPE3m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_MCQE_PORTSP_CONFIG_PIPE3m; break;

    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE0m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE0m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE0m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE0m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE1m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE1m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE1m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE1m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE2m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE2m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE2m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE2m; break;
    case MMU_THDM_DB_PORTSP_CONFIG_A_PIPE3m:
    case MMU_THDM_DB_PORTSP_CONFIG_B_PIPE3m:
    case MMU_THDM_DB_PORTSP_CONFIG_C_PIPE3m:
        spci->mem = MMU_THDM_DB_PORTSP_CONFIG_PIPE3m; break;

    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE0m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE0m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE0m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE1m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE1m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE1m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE2m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE2m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE2m; break;
    case MMU_THDM_MCQE_QUEUE_OFFSET_A_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_B_PIPE3m:
    case MMU_THDM_MCQE_QUEUE_OFFSET_C_PIPE3m:
        spci->mem = MMU_THDM_MCQE_QUEUE_OFFSET_PIPE3m; break;

    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE0m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE0m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE0m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE0m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE1m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE1m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE1m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE1m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE2m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE2m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE2m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE2m; break;
    case MMU_THDM_DB_QUEUE_OFFSET_A_PIPE3m:
    case MMU_THDM_DB_QUEUE_OFFSET_B_PIPE3m:
    case MMU_THDM_DB_QUEUE_OFFSET_C_PIPE3m:
        spci->mem = MMU_THDM_DB_QUEUE_OFFSET_PIPE3m; break;


    /* RQE */
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m:
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m:
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC0m; break;

    case MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC1m:
    case MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC1m:
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT_SC1m; break;


    default:
        break;
    }
    return SOC_E_NONE;
}

/* correction routine for MMU_REPL_GROUP_INITIAL_COPY_COUNT mem */
STATIC int
_soc_mmu_repl_group_correction(int mmu_base_index,
                               _soc_ser_correct_info_t *spci, uint32 *addr)
{
    if ((spci == NULL) || (addr == NULL)) {
        return SOC_E_PARAM;
    }
    if (spci->mem == MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC0m &&
        ((mmu_base_index == _SOC_MMU_BASE_INDEX_XPE1) ||
         (mmu_base_index == _SOC_MMU_BASE_INDEX_XPE3))) {
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT0_SC1m;
        *addr &= _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_MMU_ADDR_SEG1;
        spci->addr = *addr;
    }
    if (spci->mem == MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC0m &&
        ((mmu_base_index == _SOC_MMU_BASE_INDEX_XPE1) ||
         (mmu_base_index == _SOC_MMU_BASE_INDEX_XPE3))) {
        spci->mem = MMU_REPL_GROUP_INITIAL_COPY_COUNT1_SC1m;
        *addr &= _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_MMU_ADDR_SEG1;
        spci->addr = *addr;
    }
    return SOC_E_NONE;
}

/* correction routine for MMU_WRED_CONFIG mem */
STATIC int
_soc_mmu_wred_correction(int unit, _soc_ser_correct_info_t *spci,
                         uint32 *addr, int *second_pass)
{
    if ((spci == NULL) || (addr == NULL) || (second_pass == NULL)) {
        return SOC_E_PARAM;
    }
    switch (spci->mem) {
    case MMU_WRED_CONFIG_PIPE0m:
        spci->mem = MMU_WRED_CONFIG_PIPE1m;
        *addr &= _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_MMU_ADDR_SEG1;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    case MMU_WRED_CONFIG_PIPE2m:
        spci->mem = MMU_WRED_CONFIG_PIPE3m;
        *addr &= _SOC_MMU_ADDR_ZERO_OUT_SEG_FIELD;
        *addr |= _SOC_MMU_ADDR_SEG3;
        spci->index = *addr - soc_mem_base(unit, spci->mem);
        spci->addr = *addr;
        *second_pass = 1;
        break;
    default:
        *second_pass = 0;
        break;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_process_mmu_err(int unit, int block_info_idx,
                                  const _soc_th_ser_info_t *info_list,
                                  char *prefix_str, int mmu_base_index)
{
    int rv, type = 0, multi = 0;
    uint32 addr = 0, rval, rval_fifo_status;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_reg_t fifo_status_reg, mem_fail_ctr_reg;
    soc_mem_t ser_fifo_mem;
    soc_block_t blocktype = SOC_BLK_NONE;
    uint8 blk_idx;
    uint32 sblk = 0;
    _soc_ser_correct_info_t spci;
    int second_pass = 0;
    int i;
    static int acc_type[] = {_SOC_ACC_TYPE_PIPE_ANY, _SOC_ACC_TYPE_PIPE_ANY};

    switch(info_list->type) {
    case _SOC_PARITY_TYPE_MMU_SER: {
        /* When we are here, we have been called by _soc_tomahawk_process_ser.
	     * info_list points to 1st entry in _soc_th_mmu_top_ser_info[].
	     * Goal is to find src_sub_block in mmu: MMU_GLB, XPE0,1,2,3, SC0,1 */
        _soc_th_ser_info_t *info = NULL;

        /* find src of interrupt in mmu */
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, info_list->intr_status_reg, REG_PORT_ANY, 0,
                           &rval)); /* status_reg has BT,AT = CHIP,SINGLE */
        for (i = 0; mmu_intr_info[i].int_statf != INVALIDf; i++) {
            if (soc_reg_field_get(unit, info_list->intr_status_reg, rval,
                                  mmu_intr_info[i].int_statf)) {
#ifdef _SOC_SER_ENABLE_CLI_DBG
                LOG_CLI((BSL_META_U(unit,
                                    "Unit %d: intr_status_reg = 0x%08x, "
                                    "int_statf = %d is set, mmu_intr_info_idx "
                                    "= %d !!\n"),
                         unit, rval, mmu_intr_info[i].int_statf, i));
#endif /* _SOC_SER_ENABLE_CLI_DBG */
                info = &(info_list->info[mmu_intr_info[i].ser_info_index]);
                    /* 'info' now points to entry info_index[i]
                     * in _soc_th_mmu_ser_info[] */
                rv = _soc_tomahawk_ser_process_mmu_err(unit, block_info_idx,
                                                       info, prefix_str,
                                                       mmu_intr_info[i].mmu_base_index);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Unit %d: Error processing %s !!\n"),
                             unit, info->mem_str));
                    return rv;
                }
            }
        }
        if (info == NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "unit %d In MMU SER processing with no error bit set "
                                "0x%08x !!\n"), unit, rval));
            return SOC_E_NONE;
        }
    }   break;
    case _SOC_PARITY_TYPE_MMU_GLB:
    case _SOC_PARITY_TYPE_MMU_XPE:
    case _SOC_PARITY_TYPE_MMU_SC: {
        /* we are here when we were recursively called by process_mmu_ser itself
         * and info_list is pointing to one of the entries in
         * _soc_th_mmu_ser_info[]
         */
        switch(info_list->type) {
        case _SOC_PARITY_TYPE_MMU_GLB: /* all of these regs,mem have BT,AT = CHIP,SINGLE */
            fifo_status_reg = MMU_GCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = MMU_GCFG_MEM_FAIL_ADDR_64m;
            mem_fail_ctr_reg = MMU_GCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_GLB;
            acc_type[0] = _SOC_ACC_TYPE_PIPE_ANY;
            break;
        case _SOC_PARITY_TYPE_MMU_XPE: /* all of these regs,mem have BT,AT = XPE,SINGLE */
            fifo_status_reg = MMU_XCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_XPE_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_XCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_XPE;
            acc_type[0] = mmu_base_index;
            break;
        case _SOC_PARITY_TYPE_MMU_SC: /* all of these regs,mem have BT,AT = SLICE,SINGLE */
            fifo_status_reg = MMU_SCFG_MEM_SER_FIFO_STSr;
            ser_fifo_mem = _SOC_FIND_MMU_SC_SER_FIFO_MEM(mmu_base_index);
            mem_fail_ctr_reg = MMU_SCFG_MEM_FAIL_INT_CTRr;
            blocktype = SOC_BLK_MMU_SC;
            acc_type[0] = mmu_base_index;
            break;
        default:
            return SOC_E_NONE;
        }
        SOC_IF_ERROR_RETURN
            (_soc_th_ser_reg32_get(unit, info_list->intr_status_reg,
                                   REG_PORT_ANY, 0, &rval, mmu_base_index));
        if (soc_reg_field_get(unit, info_list->intr_status_reg, rval,
                              info_list->intr_status_field)) {
            /* Now we know that one of the reasons for interrupt from MMU is
             * parity error for sure, so mmu_ser_fifo has to be non-empty */
            SOC_IF_ERROR_RETURN
                (_soc_th_ser_reg32_get(unit, fifo_status_reg, REG_PORT_ANY, 0,
                                       &rval_fifo_status, mmu_base_index));
            if (soc_reg_field_get(unit, fifo_status_reg, rval_fifo_status, EMPTYf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "unit %d %s SER interrupt with empty fifo !!\n"),
                           unit, info_list->mem_str));
                SOC_IF_ERROR_RETURN
                    (_soc_th_ser_reg_field32_modify(unit,
                                                    info_list->intr_clr_reg,
                                                    REG_PORT_ANY,
                                                    info_list->intr_clr_field,
                                                    1, 0, mmu_base_index));
                return SOC_E_NONE;
            }
            SOC_IF_ERROR_RETURN
                (_soc_th_ser_reg32_get(unit, mem_fail_ctr_reg, REG_PORT_ANY, 0,
                                       &rval, mmu_base_index));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d %s mem error interrupt count: %d\n"),
                       unit, info_list->mem_str, rval));
            SOC_BLOCK_ITER(unit, blk_idx, blocktype) {
                sblk = SOC_BLOCK2SCH(unit, blk_idx);
                break;
            }
            do {
                if (!second_pass) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_pop(unit, ser_fifo_mem, MEM_BLOCK_ANY, entry));
                    /* process entry */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s\n"), info_list->mem_str));
                    if (soc_mem_field32_get(unit, ser_fifo_mem, entry, ERR_MULTf)) {
                        multi = 1;
                    }
                    type = soc_mem_field32_get(unit, ser_fifo_mem, entry,
                                               ERR_TYPEf);
                    addr = soc_mem_field32_get(unit, ser_fifo_mem, entry, EADDRf);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d %s %s %s error at address 0x%08x\n"),
                               unit, info_list->mem_str, multi ? "multiple" : "",
                               type ? ((type == 1) ? "1bit": "2bit") : "", addr));

                    sal_memset(&spci, 0, sizeof(spci));
                    spci.flags |= SOC_SER_SRC_MEM;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blocktype;
                    spci.sblk = sblk;
                    spci.detect_time = sal_time_usecs();
                    spci.addr = addr;
                    
                    /* Try to decode memory */
                    for (i = 0; ; i++) {
                        spci.mem = soc_addr_to_mem_extended(unit, sblk,
                                                            acc_type[i],
                                                            addr);
                        if (spci.mem != INVALIDm) {
#ifdef _SOC_SER_ENABLE_CLI_DBG
                            LOG_CLI((BSL_META_U(unit,
                                                "unit %d acc_type = %d "
                                                "got us mem %s \n"),
                                     unit, acc_type[i],
                                     SOC_MEM_NAME(unit,spci.mem)));
#endif /* _SOC_SER_ENABLE_CLI_DBG */
                            break;
                        }
                        if (_SOC_ACC_TYPE_PIPE_ANY == acc_type[i]) {
                            break;
                        }
                    }
                    if (spci.mem != INVALIDm) {
                        SOC_IF_ERROR_RETURN
                            (_soc_mmu_repl_group_correction(mmu_base_index,
                                                            &spci, &addr));
                        spci.index = addr - soc_mem_base(unit, spci.mem);
                        spci.flags |= SOC_SER_REG_MEM_KNOWN;
                        SOC_IF_ERROR_RETURN
                            (_soc_tomahawk_ser_mmu_mem_remap(&spci));
#ifdef _SOC_SER_ENABLE_CLI_DBG
                        LOG_CLI((BSL_META_U(unit,
                                            "unit %d mem for ser_correction ="
                                            " %s \n"),
                                 unit, SOC_MEM_NAME(unit,spci.mem)));
#endif /* _SOC_SER_ENABLE_CLI_DBG */
                    } else {
                        spci.flags |= SOC_SER_REG_MEM_UNKNOWN;
                        
                    }
                } /* !second_pass */
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                                   sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                   addr);
                
                rv = soc_ser_correction(unit, &spci);
                if (SOC_FAILURE(rv)) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                       sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       addr);
                    return rv;
                }
                if (spci.log_id != 0) { 
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                                       SOC_SWITCH_EVENT_DATA_ERROR_LOG, 
                                       spci.log_id, 0);
                }

                if (second_pass) {
                    /* already in second_pass, move to next fifo entry */
                    SOC_IF_ERROR_RETURN
                        (_soc_th_ser_reg32_get(unit, fifo_status_reg,
                                               REG_PORT_ANY, 0,
                                               &rval_fifo_status,
                                               mmu_base_index));
                } else { /* in first_pass */
                    SOC_IF_ERROR_RETURN
                        (_soc_mmu_wred_correction(unit, &spci, &addr,
                                                  &second_pass));
                    if (!second_pass) {
                        /* second_pass not needed */
                        SOC_IF_ERROR_RETURN
                            (_soc_th_ser_reg32_get(unit, fifo_status_reg,
                                                   REG_PORT_ANY, 0,
                                                   &rval_fifo_status,
                                                   mmu_base_index));
                    }
                    /* else, second_pass is needed, don't read next fifo entry
                     * yet */
                }
            } while (!soc_reg_field_get(unit, fifo_status_reg, rval_fifo_status, EMPTYf));
            SOC_IF_ERROR_RETURN
                (_soc_th_ser_reg_field32_modify(unit, info_list->intr_clr_reg,
                                                REG_PORT_ANY,
                                                info_list->intr_clr_field, 1,
                                                0, mmu_base_index));
        } else {
            /* interrupt from mmu sub_block is not related to parity error
             * so process_mmu_ser should not have been called */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d, ser_process_mmu_err: "
                                  "field %s in reg %s not set !!"
                                  "reg_value read is %0x !!\n"),
                       unit,
                       SOC_FIELD_NAME(unit, info_list->intr_status_field),
                       SOC_REG_NAME(unit, info_list->intr_status_reg), rval));
            return SOC_E_FAIL;
        }
    }   break;
    default:
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_process_ecc(int unit, int block_info_idx, int pipe, int port,
                              const _soc_th_ser_info_t *info,
                              char *prefix_str, char *mem_str)
{
    _soc_th_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    uint32 rval/*, minfo*/;
    uint32 multiple, double_bit, entry_idx, idx, has_error;
    char *mem_str_ptr;
    /*_soc_ser_correct_info_t spci;*/

    if (info->intr_status_reg != INVALIDr) {
        reg_entry[0].reg = info->intr_status_reg;
        reg_entry[0].mem_str = NULL;
        reg_entry[1].reg = INVALIDr;
        reg_ptr = reg_entry;
    } else if (info->intr_status_reg_list != NULL) {
        reg_ptr = info->intr_status_reg_list;
    } else {
        return SOC_E_NONE;
    }

    has_error = FALSE;
    for (idx = 0; info->intr_status_reg != INVALIDr; idx++) {
        reg = info->intr_status_reg;
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ?
            reg_ptr[idx].mem_str : mem_str;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, port, 0, &rval));

        if (soc_reg_field_get(unit, reg, rval, ECC_ERRf)) {
            has_error = TRUE;
            multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
            double_bit = soc_reg_field_get(unit, reg, rval, DOUBLE_BIT_ERRf);
            entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            /*soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR, 
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 
                               entry_idx, minfo);
            sal_memset(&spci, 0, sizeof(spci));*/
            if (double_bit) {
                /*spci.double_bit = 1;*/
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s entry %d double-bit ECC error\n"),
                           prefix_str, mem_str_ptr, entry_idx));
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s entry %d ECC error\n"),
                           prefix_str, mem_str_ptr, entry_idx));
            }
            if (multiple) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s has multiple ECC errors\n"),
                           prefix_str, mem_str_ptr));
            }
          /*if (idx == 0 && info->mem != INVALIDm) {
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = info->mem;
                spci.blk_type = -1;
                spci.index = entry_idx;
                (void)soc_ser_correction(unit, &spci);
            }*/
        }

        /* Clear parity status */
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, 0));
    }

    if (!has_error) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s %s ECC hardware inconsistency\n"),
                   prefix_str, mem_str));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_process_ser(int unit, int block_info_idx, int inst, int pipe,
                          int port, soc_reg_t group_reg, uint64 group_rval,
                          const _soc_th_ser_info_t *info_list,
                          char *prefix_str)
{
    const _soc_th_ser_info_t *info;
    int info_index;
    char *mem_str;
    /*uint32 minfo;*/

    /* This func (_soc_tomahawk_process_ser) is called by
     * _soc_tomahawk_ser_process_all which passes
     * _soc_th_mmu_top_ser_info[] as param to this func.
     * And, _soc_th_mmu_top_ser_info[] list has only one entry with type
     * _SOC_PARITY_TYPE_MMU_SER and 'info' for that entry pointing to
     * _soc_th_mmu_ser_info
     */
    for (info_index = 0; ; info_index++) { /* Loop through each info entry in the mmu_top_ser_info list */
        info = &info_list[info_index]; /* LHS points to 1st entry in _soc_th_mmu_top_ser_info[] */
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        if (group_reg != INVALIDr) {
            /* Check status for the info entry in the group register */
            /* for SOC_BLK_MMU, group_reg is INVALIDr */
            if (!soc_reg64_field32_get(unit, group_reg, group_rval,
                                       info->group_reg_status_field)) {
                continue;
            }
        }

        if (info->mem_str) {
            mem_str = info->mem_str; /* "MMU MME PAR" for SOC_BLK_MMU */
        } else if (info->mem != INVALIDm) {
            mem_str = SOC_MEM_NAME(unit, info->mem);
        } else {
            mem_str = SOC_FIELD_NAME(unit, info->group_reg_status_field);
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        
        case _SOC_PARITY_TYPE_MMU_SER:
            /* we can never be here for following 3 types.
             * _soc_th_mmu_top_ser_info has only one entry with type
             * _SOC_PARITY_TYPE_MMU_SER
             */
        case _SOC_PARITY_TYPE_MMU_GLB:
        case _SOC_PARITY_TYPE_MMU_XPE:
        case _SOC_PARITY_TYPE_MMU_SC:
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_mmu_err(unit, block_info_idx,
                                                   info, prefix_str, -1));
                /* info points to _soc_th_mmu_top_ser_info[0] */
            break;
        case _SOC_PARITY_TYPE_ECC:
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_ser_process_ecc(unit, block_info_idx, pipe,
                                               port, info, prefix_str,
                                               mem_str));
            break;
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the list */

    return SOC_E_NONE;
}

STATIC char *_soc_th_ser_hwmem_base_info[] = {
    "OBM_QUEUE_FIFO - In Idb",                                       /* 0x0 */
    "OBM_DATA_FIFO - In Idb",                                        /* 0x1 */
    "CELL_ASSEM_BUFFER - In Idb",                                    /* 0x2 */
    "CPU_CELL_ASSEM_BUFFER - In Idb",                                /* 0x3 */
    "LPBK_CELL_ASSEM_BUFFER - In Idb",                               /* 0x4 */
    "Invalid value",                                                 /* 0x5 */
    "Invalid value",                                                 /* 0x6 */
    "INGRESS_PACKET_BUFFER - In Ipars",                              /* 0x7 */
    "ISW3_EOP_BUFFER_A - In Isw3",                                   /* 0x8 */
    "ISW3_EOP_BUFFER_B - In Isw3",                                   /* 0x9 */
    "BUBBLE_MOP - In Isw2",                                          /* 0xa */
    "IPARS_IVP_PT_BUS - In Ivp, Passthru Highway bus",               /* 0xb */
    "IVP_ISW1_PT_BUS - In Isw1, Passthru Highway bus",               /* 0xc */
    "IFWD_ISW1_PT_BUS - In Isw1, Passthru Highway bus",              /* 0xd */
    "Invalid value",                                                 /* 0xe */
    "ISW1_ISW2_PT_BUS - In Isw2, Passthru Highway bus",              /* 0xf */
    "ICFG_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x10 */
    "IPARS_BUS - PassThrought bus parity check, MEMINDEX is 0",      /* 0x11 */
    "IVXLT_BUS - PassThrought bus parity check, MEMINDEX is 0",      /* 0x12 */
    "IVP_BUS - PassThrought bus parity check, MEMINDEX is 0",        /* 0x13 */
    "IFWD_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x14 */
    "IRSEL1_BUS - PassThrought bus parity check, MEMINDEX is 0",     /* 0x15 */
    "ISW1_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x16 */
    "IFP_BUS - PassThrought bus parity check, MEMINDEX is 0",        /* 0x17 */
    "IRSEL2_BUS - PassThrought bus parity check, MEMINDEX is 0",     /* 0x18 */
    "ISW2_BUS - PassThrought bus parity check, MEMINDEX is 0",       /* 0x19 */
    "Invalid value",                                                 /* 0x20 */
    "Invalid value",                                                 /* 0x21 */
    "Invalid value",                                                 /* 0x22 */
    "Invalid value",                                                 /* 0x23 */
    "Invalid value",                                                 /* 0x24 */
    "Invalid value",                                                 /* 0x25 */
    "Invalid value",                                                 /* 0x26 */
    "Invalid value",                                                 /* 0x27 */
    "Invalid value",                                                 /* 0x28 */
    "Invalid value",                                                 /* 0x29 */
    "Invalid value",                                                 /* 0x2a */
    "Invalid value",                                                 /* 0x2b */
    "Invalid value",                                                 /* 0x2c */
    "Invalid value",                                                 /* 0x2d */
    "Invalid value",                                                 /* 0x2e */
    "Invalid value",                                                 /* 0x2f */
    "LEARN_FIFO - In L2MM",                                          /* 0x30 */
    "Invalid value",                                                 /* 0x31 */
    "Invalid value",                                                 /* 0x32 */
    "Invalid value",                                                 /* 0x33 */
    "Invalid value",                                                 /* 0x34 */
    "Invalid value",                                                 /* 0x35 */
    "Invalid value",                                                 /* 0x36 */
    "Invalid value",                                                 /* 0x37 */
    "CENTRAL_EVICTION_FIFO - In CEV",                                /* 0x38 */
    "Invalid value",                                                 /* 0x39 */
    "Invalid value",                                                 /* 0x3a */
    "Invalid value",                                                 /* 0x3b */
    "Invalid value",                                                 /* 0x3c */
    "Invalid value",                                                 /* 0x3d */
    "Invalid value",                                                 /* 0x3e */
    "Invalid value",                                                 /* 0x3f */
    "EP_MPB_DATA - In El3",                                          /* 0x40 */
    "EP_INITBUF - In Ehcpm",                                         /* 0x41 */
    "CM_DATA_BUFFER - In Edatabuf",                                  /* 0x42 */
    "PM0_DATA_BUFFER - In Edatabuf",                                 /* 0x43 */
    "PM1_DATA_BUFFER - In Edatabuf",                                 /* 0x44 */
    "PM2_DATA_BUFFER - In Edatabuf",                                 /* 0x45 */
    "PM3_DATA_BUFFER - In Edatabuf",                                 /* 0x46 */
    "PM4_DATA_BUFFER - In Edatabuf",                                 /* 0x47 */
    "PM5_DATA_BUFFER - In Edatabuf",                                 /* 0x48 */
    "PM6_DATA_BUFFER - In Edatabuf",                                 /* 0x49 */
    "PM7_DATA_BUFFER - In Edatabuf",                                 /* 0x4a */
    "LBP_DATA_BUFFER - In Edatabuf",                                 /* 0x4b */
    "PM_RESI0_DATA_BUFFER - In Edatabuf",                            /* 0x4c */
    "PM_RESI1_DATA_BUFFER - In Edatabuf",                            /* 0x4d */
    "Invalid value",                                                 /* 0x4e */
    "Invalid value",                                                 /* 0x4f */
    "CM_EDB_CTRL_BUFFER - In Edatabuf",                              /* 0x50 */
    "PM0_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x51 */
    "PM1_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x52 */
    "PM2_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x53 */
    "PM3_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x54 */
    "PM4_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x55 */
    "PM5_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x56 */
    "PM6_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x57 */
    "PM7_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x58 */
    "LBP_EDB_CTRL_BUFFER - In Edatabuf",                             /* 0x59 */
    "EGR_VLAN_BUS - PassThrought bus parity check, MEMINDEX is 0",   /* 0x5a */
    "EGR_HCPM_BUS - PassThrought bus parity check, MEMINDEX is 0",   /* 0x5b */
    "EGR_PMOD_BUS - PassThrought bus parity check, MEMINDEX is 0",   /* 0x5c */
    "EGR_FPPARS_BUS - PassThrought bus parity check, MEMINDEX is 0", /* 0x5d */
    "EFP_BUS - PassThrought bus parity check, MEMINDEX is 0",        /* 0x5e */
    "EP_PT_BUS - In Esw, Passthru Highway bus"                       /* 0x5f */
};

#define _SOC_TH_SER_REG 1
#define _SOC_TH_SER_MEM 0

STATIC void
_soc_tomahawk_print_ser_fifo_details(int unit, uint8 regmem, soc_block_t blk,
                                     uint32 sblk, int pipe, uint32 address,
                                     uint32 stage, uint32 base, uint32 index,
                                     uint32 type, uint8 drop, uint8 non_sbus)
{
    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityError, unit)) {
        switch (type) {
        case 0:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: SOP cell.\n")));
            break;
        case 1:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: MOP cell.\n")));
            break;
        case 2:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: EOP cell.\n")));
            break;
        case 3:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: SBUS transaction.\n")));
            break;
        case 4:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: transaction - refresh, aging etc.\n"))); 
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Invalid error reported !!\n")));
        }
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Blk: %d, Pipe: %d, Address: 0x%08x, base: 0x%x, "
                              "stage: %d, index: %d\n"),
                   sblk, pipe, address, base, stage, index));
        if (regmem == _SOC_TH_SER_MEM) {
            if (non_sbus) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Mem hwbase: 0x%x [%s]\n"), base,
                           _soc_th_ser_hwmem_base_info[base]));
            }
        }
        if (drop) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SER caused packet drop.\n")));
        }
    }
}

STATIC int
_soc_tomahawk_process_ser_fifo(int unit, soc_block_t blk, int pipe,
                               char *prefix_str)
{
    /*int i;*/
    int rv;
    uint8 bidx;
    soc_mem_t mem;
    char blk_str[10];
    /*int *acc_type_ptr;*/
    soc_reg_t reg = INVALIDr;
    _soc_ser_correct_info_t spci;
    /*static int acc_type1[] = { 3, 2, 6, 0, -1 };
    static int acc_type0[] = { 1, 3, 6, 4, 0, -1 };*/
    uint32 ecc_parity = 0, address = 0;
    uint32 reg_val, mask, entry[SOC_MAX_MEM_WORDS];
    uint32 stage = 0, addrbase = 0, index = 0, type = 0;
    uint32 sblk = 0, regmem = 0, non_sbus = 0, drop = 0;
    soc_mem_t ipsf[] = { ING_SER_FIFO_PIPE0m, ING_SER_FIFO_PIPE1m,
                         ING_SER_FIFO_PIPE2m, ING_SER_FIFO_PIPE3m };
    soc_mem_t epsf[] = { EGR_SER_FIFO_PIPE0m, EGR_SER_FIFO_PIPE1m,
                         EGR_SER_FIFO_PIPE2m, EGR_SER_FIFO_PIPE3m };
    soc_reg_t epsr[] = { EGR_INTR_STATUS_PIPE0r, EGR_INTR_STATUS_PIPE1r,
                         EGR_INTR_STATUS_PIPE2r, EGR_INTR_STATUS_PIPE3r };

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "In process fifo.\n")));
    switch (blk) {
    case SOC_BLK_IPIPE:
        mem = ipsf[pipe];
        mask = _soc_th_ip_pipe_fifo_bmask[pipe];
        sal_sprintf(blk_str, "IPIPE");
        break;
    case SOC_BLK_EPIPE:
        mem = epsf[pipe];
        mask = 0x00000001; /* SER_FIFO_NON_EMPTYf */
        reg = epsr[pipe];
        sal_sprintf(blk_str, "EPIPE");
        break;
    default: return SOC_E_PARAM;
    }

    do {
        SOC_IF_ERROR_RETURN
            (soc_mem_pop(unit, mem, MEM_BLOCK_ANY, entry));
        /* process entry */
        if (soc_mem_field32_get(unit, mem, entry, VALIDf)) {
            ecc_parity = soc_mem_field32_get(unit, mem, entry, ECC_PARITYf);
            regmem = soc_mem_field32_get(unit, mem, entry, MEM_TYPEf);
            address = soc_mem_field32_get(unit, mem, entry, ADDRESSf);
            stage = soc_mem_field32_get(unit, mem, entry, PIPE_STAGEf);
            type = soc_mem_field32_get(unit, mem, entry, INSTRUCTION_TYPEf);
            drop = soc_mem_field32_get(unit, mem, entry, DROPf);
            SOC_BLOCK_ITER(unit, bidx, blk) {
                sblk = SOC_BLOCK2SCH(unit, bidx);
                break;
            }
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), prefix_str));
            if (soc_mem_field32_get(unit, mem, entry, MULTIPLEf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Multiple: ")));
            }
            if (regmem == _SOC_TH_SER_REG) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Reg: ")));
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Mem: ")));
            }
            spci.double_bit = 0;
            switch (ecc_parity) {
            case 0:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Parity error..\n")));
                break;
            case 1:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Corrected single bit ECC error..\n")));
                /* NOTE: This is supressed by default, 
                         we can choose not to supress it */
                break;
            case 2:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Double or Multiple bit ECC error..\n")));
                spci.double_bit = 1;
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Invalid SER issue !!\n")));
                return SOC_E_INTERNAL;
            }
            if (regmem == _SOC_TH_SER_MEM) {
                /* process mem */
                non_sbus = soc_mem_field32_get(unit, mem, entry, NON_SBUSf);
                addrbase = soc_mem_field32_get(unit, mem, entry, MEMBASEf);
                index = soc_mem_field32_get(unit, mem, entry, MEMINDEXf);
                if (non_sbus == 0) {
                    /*acc_type_ptr = pipe ? acc_type1 : acc_type0;*/
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                       SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       address);
                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk, 
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus);
                    sal_memset(&spci, 0, sizeof(spci));
                    spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_UNKNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blk;
                    spci.pipe_num = pipe;
                    spci.sblk = sblk;
                    spci.addr = address - index;
                    spci.index = index;
                    spci.stage = stage;
                    spci.acc_type = -1;
                    rv = soc_ser_correction(unit, &spci);
                    /*for (i = 0; acc_type_ptr[i] != -1; i++) {
                        spci.acc_type = acc_type_ptr[i];
                        rv = soc_ser_correction(unit, &spci);
                        if (rv != SOC_E_INTERNAL) {
                            break;
                        }
                    }*/
                    if (SOC_FAILURE(rv)) {
                        /* Add reporting failed to correct event flag to 
                         * application */
                        soc_event_generate(unit, 
                                SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT, 
                                sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                        return rv;
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s SER mem address un-accessable !!\n"),
                               blk_str));
                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus);
                }
            } else {
                /* process reg */
                non_sbus = soc_mem_field32_get(unit, mem, entry, NON_SBUSf);
                addrbase = soc_mem_field32_get(unit, mem, entry, REGBASEf);
                index = soc_mem_field32_get(unit, mem, entry, REGINDEXf);
                if (non_sbus == 0) {
                    /*acc_type_ptr = pipe ? acc_type1 : acc_type0;*/
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                       SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       address);
                    _soc_tomahawk_print_ser_fifo_details(unit, 1, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus);
                    sal_memset(&spci, 0, sizeof(spci));
                    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_UNKNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.pipe_num = pipe;
                    spci.blk_type = blk;
                    spci.sblk = sblk;
                    spci.addr = address;
                    spci.index = index;
                    spci.stage = stage;
                    spci.acc_type = -1;
                    rv = soc_ser_correction(unit, &spci);
                    /*for (i = 0; acc_type_ptr[i] != -1; i++) {
                        spci.acc_type = acc_type_ptr[i];
                        rv = soc_ser_correction(unit, &spci);
                        if (rv != SOC_E_INTERNAL) {
                            break;
                        }
                    }*/
                    if (SOC_FAILURE(rv)) {
                        /* Add reporting failed to correct event flag to 
                         * application */
                        soc_event_generate(unit, 
                                SOC_SWITCH_EVENT_PARITY_ERROR,
                                SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT, 
                                sblk | (pipe << SOC_SER_ERROR_PIPE_BP) |
                                SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                        return rv;
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s SER reg address un-accessable !!\n"),
                               blk_str));
                    _soc_tomahawk_print_ser_fifo_details(unit, 0, blk, sblk,
                                                         pipe, address,
                                                         stage, addrbase, index,
                                                         type, drop, non_sbus);
                }
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d Got invalid mem pop from %s !!\n"),
                       unit, SOC_MEM_NAME(unit, mem)));
        }
        /* check if any more pending */
        if (reg == INVALIDr) {
            SOC_IF_ERROR_RETURN
                (READ_CMIC_CMC0_IRQ_STAT3r(unit, &reg_val));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val));
        }
    } while (reg_val & mask);
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_ser_process_all(int unit, int reg_type, int bit)
{
    uint8      rbi;
    int        port = REG_PORT_ANY;
    uint32     cmic_rval, cmic_bit;
    uint64     rb_enable64, rb_rval64, tmp64;
    const      _soc_th_ser_route_block_t *rb;
    char       prefix_str[10];
    int        block_info_idx;
    soc_stat_t *stat = SOC_STAT(unit);
    COMPILER_64_ZERO(rb_rval64);

    sal_sprintf(prefix_str, "Unit: %d \n", unit);

    switch (reg_type) {
    case 3:
        /* Read CMIC parity status register */
        SOC_IF_ERROR_RETURN
            (READ_CMIC_CMC0_IRQ_STAT3r(unit, &cmic_rval));
        if (cmic_rval == 0) {
            return SOC_E_NONE;
        }
        break;
    case 4:
        /* Read PM0-31 parity status register */
        SOC_IF_ERROR_RETURN
            (READ_CMIC_CMC0_IRQ_STAT4r(unit, &cmic_rval));
        if (cmic_rval == 0) {
            return SOC_E_NONE;
        }
        break;
    case 5:
        /* Read PM32 parity status register */
        SOC_IF_ERROR_RETURN
            (READ_CMIC_CMC0_IRQ_STAT5r(unit, &cmic_rval));
        if (cmic_rval == 0) {
            return SOC_E_NONE;
        }
        break;
    default:
        return SOC_E_NONE;
    }

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_th_ser_route_blocks[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (!((rb->cmic_reg == reg_type) && (cmic_bit == 1<<bit))) {
            continue;
        }
        if (!(cmic_rval & cmic_bit)) {
            /* Indicated bit not asserted for the route block */
            break;
        }
        if (rb->blocktype == SOC_BLK_IPIPE || rb->blocktype == SOC_BLK_EPIPE) {
            /* New fifo style processing */
            (void)_soc_tomahawk_process_ser_fifo(unit, rb->blocktype, rb->pipe,
                                                 prefix_str);
            stat->ser_err_fifo++;
        } else {
            /* Legacy processing */
            /* for rb->blocktype = SOC_BLK_MMU, we will be here */
            SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
                if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                    port = SOC_BLOCK_PORT(unit, block_info_idx);
                    break;
                }
                /* ??? Because entry for mmu in _soc_th_ser_route_blocks array uses
                 * SOC_BLK_MMU which is illegal for Tomahawk, so block_info_idx
                 * determined above is incorrect for mmu. Does it matter ?
                 */
            }
            if (rb->enable_reg != INVALIDr) {
		        /* for SOC_BLK_MMU, enable_reg is INVALIDr */
                if (SOC_BLOCK_IN_LIST(SOC_REG_INFO(unit, rb->enable_reg).block,
                    SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                        /* This port block is not configured */
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "unit %d SER error on disabled "
                                              "port block %d !!\n"),
                                   unit, block_info_idx));
                        continue;
                }
            }
            /* Read per route block parity status register */
            if (rb->status_reg != INVALIDr) {
		        /* for SOC_BLK_MMU, status_reg is INVALIDr */
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, rb->status_reg, port, 0, &rb_rval64));

		        /* ??? BUG: Following check was outside if (rb->status_reg != INVALIDr)
                 * For SOC_BLK_MMU, rb->status_reg is INVALIDr,
                 * so rb_rval64 will always be 0, so we will skip all of following for mmu
		         * so we will never call _soc_tomahawk_process_ser,
                 * and hence process_mmu_err at all !!
		         */
                if (COMPILER_64_IS_ZERO(rb_rval64)) {
                    continue;
                }
            }
            SOC_IF_ERROR_RETURN
                (_soc_tomahawk_process_ser(unit, block_info_idx, rb->id, 
                                           rb->pipe, port, rb->status_reg,
                                           rb_rval64, rb->info, prefix_str));

            if (rb->enable_reg != INVALIDr) {
		        /* ??? BUG: for SOC_BLK_MMU, rb->enable_reg is INVALIDr so
                 * following will not be executed. Is this problem?
		         */
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, rb->enable_reg, port, 0, &rb_enable64));
                COMPILER_64_SET(tmp64, COMPILER_64_HI(rb_rval64),
                                COMPILER_64_LO(rb_rval64));
                COMPILER_64_NOT(tmp64); /* tmp64 = ~rb_rval64 */
                COMPILER_64_AND(rb_enable64, tmp64); /* rb_enable64 &= ~rb_rval64; */
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, rb->enable_reg, port, 0, rb_enable64));
                COMPILER_64_OR(rb_enable64, rb_rval64);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, rb->enable_reg, port, 0, rb_enable64));
                /* ??? BTW, is above sequence writing Ones, and then Zeros
                 * to bits which were 1 in rb_rval64 ??
                 */
            }
            stat->ser_err_int++;
        }
    }
    return SOC_E_NONE;
}

void
soc_tomahawk_ser_error(void *unit_vp, void *d1, void *d2, void *d3,
                       void *d4)
{
    int unit = PTR_TO_INT(unit_vp);

    (void)_soc_tomahawk_ser_process_all(unit, PTR_TO_INT(d3), PTR_TO_INT(d4));
    sal_usleep(SAL_BOOT_QUICKTURN ? 1000000 : 10000);
    switch (PTR_TO_INT(d3)) {
    case 3: (void)soc_cmicm_intr3_enable(unit, PTR_TO_INT(d2));
        break;
    case 4: (void)soc_cmicm_intr4_enable(unit, PTR_TO_INT(d2));
        break;
    case 5: (void)soc_cmicm_intr5_enable(unit, PTR_TO_INT(d2));
        break;
    default:
        break;
    }
}

static _soc_generic_ser_info_t _soc_th_tcam_ser_info_template[] = {
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 105}, {1, 105}, {106, 210}, {107, 210} } },
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} } },
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 140}, {1, 140}, {141, 280}, {142, 280} } },
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} } },
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 195}, {1, 195}, {196, 387}, {197, 387} } },
    /* Note: Add global view first and then per-pipe */
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} } },
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { IFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} } },
    { IFP_TCAM_WIDEm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 161}, {1, 161}, {162, 321}, {163, 321} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} } },
    { IFP_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 96}, {1, 96}, {97, 192}, {98, 192} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },
    { SRC_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK, 4 },
    { DST_COMPRESSIONm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK, 4 },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} } },
    { EXACT_MATCH_LOGICAL_TABLE_SELECTm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} } },
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 241}, {1, 241}, {242, 481}, {243, 481} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_ACC_TYPE_CHK | _SOC_SER_FLAG_VIEW_DISABLE, 4 },
    { INVALIDm },
};

static _soc_generic_ser_info_t *_soc_th_tcam_ser_info[SOC_MAX_NUM_DEVICES];

int
soc_tomahawk_tcam_ser_init(int unit)
{
    int alloc_size;
    uint32 map[] = {0, 1, 2, 3};

    /* First, make per-unit copy of the master TCAM list */
    alloc_size = sizeof(_soc_th_tcam_ser_info_template);
    if (NULL == _soc_th_tcam_ser_info[unit]) {
        if ((_soc_th_tcam_ser_info[unit] =
             sal_alloc(alloc_size, "th tcam list")) == NULL) {
            return SOC_E_MEMORY;
        }
    }
    /* Make a fresh copy of the TCAM template info */
    sal_memcpy(_soc_th_tcam_ser_info[unit],
               &(_soc_th_tcam_ser_info_template),
               alloc_size);
    SOC_IF_ERROR_RETURN(soc_generic_ser_at_map_init(unit, map, COUNTOF(map)));
    return soc_generic_ser_init(unit, _soc_th_tcam_ser_info[unit]);
}

void
soc_tomahawk_ser_fail(int unit)
{
    soc_generic_ser_process_error(unit, _soc_th_tcam_ser_info[unit],
                                  _SOC_PARITY_TYPE_SER);
}

int
soc_tomahawk_mem_ser_control(int unit, soc_mem_t mem, int copyno,
                             int enable)
{
    return SOC_E_NONE;
}

void
soc_tomahawk_ser_register(int unit)
{
#ifdef INCLUDE_MEM_SCAN
    soc_mem_scan_ser_list_register(unit, TRUE,
                                   _soc_th_tcam_ser_info[unit]);
#endif
    memset(&_th_ser_functions, 0, sizeof(soc_ser_functions_t));
    _th_ser_functions._soc_ser_fail_f = &soc_tomahawk_ser_fail;
    _th_ser_functions._soc_ser_parity_error_cmicm_intr_f =
        &soc_tomahawk_ser_error;
    soc_ser_function_register(unit, &_th_ser_functions);
}

int
soc_tomahawk_mem_sram_info_get(int unit, soc_mem_t mem, int index, 
                                _soc_ser_sram_info_t *sram_info)
{
    int i, base, base_index, offset, base_bucket, bkt_offset;
    int entries_per_ram = 0, entries_per_bank, contiguous = 0;
    uint32 rval = 0;
    
    sram_info->disable_reg = INVALIDr;
    sram_info->disable_field = INVALIDf;
    sram_info->force_reg = INVALIDr;
    sram_info->force_field = INVALIDf;
    switch (mem) {
    case L2_ENTRY_ONLY_ECCm:
        if (index < SOC_TH_NUM_ENTRIES_L2_BANK) {
            /* dedicated L2 entries - hash table */
            sram_info->ram_count = 1;
            entries_per_ram = SOC_TH_NUM_ENTRIES_L2_BANK;
            base = index;
        } else {
            /* hash_xor mems, stride 8K */
            /*
             * sram_info->disable_reg = L2_ENTRY_PARITY_CONTROLr;
             * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
             * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
             * sram_info->force_field = FORCE_XOR_GENERATIONf;
             */
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
                            /* #indexes to be corrected */
            entries_per_ram = SOC_TH_NUM_ENTRIES_XOR_BANK;
                            /* #entries / xor_bank in a shared bank */
            entries_per_bank = entries_per_ram * sram_info->ram_count;
                            /* #entries / uft bank */
            base_index = index - SOC_TH_NUM_ENTRIES_L2_BANK;
                            /* index in uft space */
            base = (base_index/entries_per_bank)*entries_per_bank;
                            /* base addr of uft bank */
            offset = base_index % entries_per_ram;
                            /* 1st entry within uft bank */
            base += SOC_TH_NUM_ENTRIES_L2_BANK + offset;
                            /* 1st index for correction */
        }
        break;
    case L3_ENTRY_ONLY_ECCm:
        if (index < SOC_TH_NUM_ENTRIES_L3_BANK) {
            /* dedicated L3 entries - hash table */
            sram_info->ram_count = 1;
            entries_per_ram = SOC_TH_NUM_ENTRIES_L3_BANK;
            base = index;
        } else {
            /* hash_xor mems, stride 8K */
            /*
             * sram_info->disable_reg = L3_ENTRY_PARITY_CONTROLr;
             * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
             * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
             * sram_info->force_field = FORCE_XOR_GENERATIONf;
             */
            sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
                            /* #indexes to be corrected */
            entries_per_ram = SOC_TH_NUM_ENTRIES_XOR_BANK;
                            /* #entries / xor_bank in a shared bank */
            entries_per_bank = entries_per_ram * sram_info->ram_count;
                            /* #entries / uft bank */
            base_index = index - SOC_TH_NUM_ENTRIES_L3_BANK;
                            /* index in uft space */
            base = (base_index/entries_per_bank)*entries_per_bank;
                            /* base addr of uft bank */
            offset = base_index % entries_per_ram;
                            /* 1st entry within uft bank */
            base += SOC_TH_NUM_ENTRIES_L3_BANK + offset;
                            /* 1st index for correction */
        }
        break;
    case FPEM_ECCm:
        /* hash_xor mems, stride 8K */
        /*
         * sram_info->disable_reg = FPEM_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
         * sram_info->force_field = FORCE_XOR_GENERATIONf;
         */
        sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TH_NUM_ENTRIES_XOR_BANK;
                        /* #entries / xor_bank in a shared bank */
        entries_per_bank = entries_per_ram * sram_info->ram_count;
                        /* #entries / uft bank */
        base_index = index;
                        /* index - (SOC_TH_NUM_ENTRIES_FPEM_BANK=0) */
                        /* index in uft space */
        base = (base_index/entries_per_bank)*entries_per_bank;
                        /* base addr of uft bank */
        offset = base_index % entries_per_ram;
                        /* 1st entry within uft bank */
        base += offset;
                        /* base += (SOC_TH_NUM_ENTRIES_FPEM_BANK=0) + offset; */
                        /* 1st index for correction */
        break;
    case L3_DEFIP_ALPM_ECCm:
        if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) == 0) {
            return SOC_E_PARAM;
        }
        /*
         * sram_info->disable_reg = ILPM_SER_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         * sram_info->force_reg = ISS_MEMORY_CONTROL_84r;
         * sram_info->force_field = FORCE_XOR_GENERATIONf;
         */
        sram_info->ram_count = SOC_TH_NUM_EL_SHARED;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0, &rval));
        

        if (soc_reg_field_get(unit, ISS_BANK_CONFIGr, rval,
                              ALPM_ENTRY_BANK_CONFIGf) == 0x3) {
            base = index & 0x1; /* 2 uft_bank mode */
            base_bucket = ((index >> 1) & SOC_TH_ALPM_BKT_MASK);
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x, uft_bank:%d\n"),
                     base_bucket, base));
            base_bucket = base_bucket % SOC_TH_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < SOC_TH_NUM_EL_SHARED; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TH_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TH_ALPM_MODE1_BKT_MASK) |
                    (bkt_offset << 1) | base;
            }
        } else {
            base = index & 0x3; /* 4 uft_bank mode */
            base_bucket = ((index >> 2) & SOC_TH_ALPM_BKT_MASK);
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "reported bucket: 0x%08x, uft_bank:%d\n"),
                     base_bucket, base));
            base_bucket = base_bucket % SOC_TH_ALPM_BKT_OFFFSET;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "base bucket: 0x%08x\n"),
                     base_bucket));
            for (i = 0; i < SOC_TH_NUM_EL_SHARED; i++) {
                sram_info->view[i] = mem;
                sram_info->index_count[i] = 1;
                bkt_offset = base_bucket + SOC_TH_ALPM_BKT_OFFFSET*i;
                sram_info->mem_indexes[i][0] =
                    (index & SOC_TH_ALPM_MODE0_BKT_MASK) |
                    (bkt_offset << 2) | base;
            }
        }
        return SOC_E_NONE;
    case FPEM_LPm:
        sram_info->disable_reg = FPEM_CONTROLr;
        sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
        /* sram_info->force_reg = ISS_MEMORY_CONTROL_84r; */
        /* sram_info->force_field = FORCE_XOR_GENERATIONf; */

        sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TH_LP_ENTRIES_IN_XOR_BANK;;
                        /* #lp_entries / lp_xor_bank of uft bank */
        base = index % entries_per_ram;
                        /* 1st lp_entry within uft bank */
        entries_per_bank = SOC_TH_NUM_ENTRIES_XOR_BANK * sram_info->ram_count;
                        /* #phy_entries / uft bank */
        offset = entries_per_bank * (index / SOC_TH_LP_ENTRIES_IN_UFT_BANK);
                        /* of 1st_phy_entry in uft space */
        for (i = 0; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] = offset;
            if (i < 2) {
                sram_info->view[i] = EXACT_MATCH_2m;
                sram_info->mem_indexes[i][0] += 8*(base + i*entries_per_ram);
                /* indexes 0 to 4K-1 correspond to em2 type entries */
                /* each index in FPEM_LP corresponds to 4 consecutive em2
                 * entries */
            } else if (i == 2) {
                sram_info->view[i] = EXACT_MATCH_4m;
                sram_info->mem_indexes[i][0] += 16*base;
                /* indexes 4K to 6K-1 correspond to em4 type entries */
                /* each index in FPEM_LP corresponds to 4 consecutive em4
                 * entries */
            } else {
                sram_info->view[i] = mem;
                offset = SOC_TH_LP_ENTRIES_IN_UFT_BANK *
                         (index / SOC_TH_LP_ENTRIES_IN_UFT_BANK);
                        /* of 1st_lp_entry in uft_space */
                offset += base;
                        /* of 1st_lp_entry to be corrected */
                sram_info->mem_indexes[i][0] = offset + i*entries_per_ram;
                        /* of 4th_lp_entry to be corrected */
            }
        }
        return SOC_E_NONE;
    case L2_ENTRY_LPm:
    case L3_ENTRY_LPm:
    case VLAN_XLATE_LPm:
    case EGR_VLAN_XLATE_LPm:
        sram_info->view[0] = mem;
        sram_info->index_count[0] = 1;
        sram_info->ram_count = 1;
        entries_per_ram = 0; /* x */
        base = index; /* index of 1st lp_entry */
        break;
    case L2_ENTRY_ISS_LPm:
    case L3_ENTRY_ISS_LPm:
        /* hash_xor mems, stride 2K */
        for (i=0; i < SOC_TH_NUM_EL_SHARED; i++) {
           sram_info->view[i] = mem;
           sram_info->index_count[i] = 1;
        }
        sram_info->ram_count = SOC_TH_NUM_EL_SHARED;
                        /* #indexes to be corrected */
        entries_per_ram = SOC_TH_LP_ENTRIES_IN_XOR_BANK;
        entries_per_bank = SOC_TH_LP_ENTRIES_IN_UFT_BANK;
        base_index = index; /* index of lp_entry */
        base = (base_index/entries_per_bank)*entries_per_bank;
                        /* lp_entry0 in uft bank */
        offset = base_index % entries_per_ram;
                        /* lp_entry in 1st xor_bank */
        base += offset; /* index of 1st lp_entry */
        break;
    case VLAN_XLATE_ECCm:
        contiguous = 1;
        /*
         * sram_info->disable_reg = VLAN_XLATE_DBGCTRL_0r;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         * sram_info->force_reg = VLAN_XLATE_DBGCTRL_0r;
         * sram_info->force_field = FORCE_XOR_GENf;
         */
        sram_info->ram_count = SOC_TH_NUM_EL_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case EGR_VLAN_XLATE_ECCm:
        contiguous = 1;
        /*
         * sram_info->disable_reg = EGR_VLAN_XLATE_CONTROLr;
         * sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
         * sram_info->force_reg = EGR_VLAN_XLATE_CONTROLr;
         * sram_info->force_field = FORCE_XOR_GENf;
         */
        sram_info->ram_count = SOC_TH_NUM_EL_EGR_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case ING_L3_NEXT_HOPm:
        sram_info->ram_count = SOC_TH_NUM_EL_ING_L3_NEXT_HOP;
        entries_per_ram = SOC_TH_RAM_OFFSET_ING_L3_NEXT_HOP;
        base = index % entries_per_ram;
        break;
    default: return SOC_E_PARAM;
    }
    sram_info->mem_indexes[0][0] = base;
    if (contiguous) {
        for (i=1; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] = sram_info->mem_indexes[i-1][0] + 1;
        }
    } else {
        for (i=1; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] =  sram_info->mem_indexes[i-1][0] + 
                                            entries_per_ram;
        }
    }
    return SOC_E_NONE;
}


#if defined(SER_TR_TEST_SUPPORT)
static soc_ser_test_functions_t ser_tomahawk_test_fun;

/*
 * Function:
 *      soc_th_ser_inject_error
 * Purpose:
 *      Injects an error into a single th memory
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 */
STATIC soc_error_t
soc_th_ser_inject_error (int unit, soc_mem_t mem, int pipe_target,
                         int block, int index)
{
#if 0
    return soc_ser_inject_or_test_mem(unit, mem, pipe_target, block, index,
                                      SER_SINGLE_INDEX, TRUE, FALSE);
#endif
    return 0;
}

/*
 * Function:
 *      soc_th_ser_test_mem
 * Purpose:
 *      Perform SER test on a single memory, or generate a test the user can
 *      enter by the command line.
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      test_type   - (IN) How many indices to test in the passes memory
 *      cmd         - (IN) TRUE if a command-line test is desired.
 * Returns:
 *      SOC_E_NONE if test passes, an error otherwise (multiple types of errors
 *      are possible.)
 */
STATIC soc_error_t
soc_th_ser_test_mem(int unit, soc_mem_t mem, _soc_ser_test_t test_type,
                     int cmd)
{
#if 0
    return soc_ser_inject_or_test_mem(unit, mem, -1, MEM_BLOCK_ANY, 0,
                                      test_type, FALSE, cmd);
#endif
    return 0;
}

/*
 * Function:
 *      soc_th_ser_test
 * Purpose:
 *      Tests that SER is working for all supported memories.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  SOC_E_NONE if test passes, an error otherwise (multiple types of errors are
 *      possible.)
 */
STATIC soc_error_t
soc_th_ser_test(int unit, _soc_ser_test_t test_type) {
    int errors = 0;
    /*Test TCAM memories*/
    errors += soc_th_ser_tcam_test(unit, test_type);
    /*Test for FIFO memories*/
    errors += soc_th_ser_hardware_test(unit, test_type);
    if (errors == 0) {
        return SOC_E_NONE;
    } else {
        return SOC_E_FAIL;
    }
}

void
soc_th_ser_test_register(int unit)
{
    /*Initialize chip-specific functions for SER testing*/
    memset(&ser_tomahawk_test_fun, 0, sizeof(soc_ser_test_functions_t));
    ser_tomahawk_test_fun.inject_error_f = &soc_th_ser_inject_error;
    ser_tomahawk_test_fun.test_mem = &soc_th_ser_test_mem;
    ser_tomahawk_test_fun.test = &soc_th_ser_test;
    ser_tomahawk_test_fun.parity_control = &_ser_test_parity_control_reg_set;
    soc_ser_test_functions_register(unit, &ser_tomahawk_test_fun);
}

#endif /*defined(SER_TR_TEST_SUPPORT)*/
#endif /* BCM_TOMAHAWK_SUPPORT */
