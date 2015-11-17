/*
 * $Id:$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        trident2p.c
 * Purpose:
 * Requires:
 */
#include <shared/bsl.h>

#include <sal/core/boot.h>

#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/mspi.h>
#include <soc/l2x.h>
#include <soc/soc_ser_log.h>
#include <soc/devids.h>


#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <soc/trident2.h>
#include <soc/td2_td2p.h>
#include <bcm_int/esw/trident2plus.h>
#include <soc/tdm/core/tdm_top.h>
#include <soc/esw/portctrl.h>

#define IS_OVERSUB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))

#define BCM_TD2P_MAX_COE_MODULES 5

#define BCM_TD2P_MAX_BANK_SIZE      128
#define BCM_TD2P_SMALL_BANK_SIZE    64


#define _TD2P_PROB_DROP_SOP_XOFF          80
#define _TD2P_PROB_DROP_SOP_XON_ABOVE_THR 65
#define _TD2P_PROB_DROP_SOP_XON_BELOW_THR 0


#define _SOC_TRIDENT2P_UPDATE_TDM_PRE  (0)
#define _SOC_TRIDENT2P_UPDATE_TDM_POST (1)

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r
};

/* Default for the max port speed configured for device */
#define TD2P_FLEX_SPEED_MAX_DEFAULT    42000


STATIC soc_reg_t mmu_ovs_group_wt_regs_td2p_local[2][_MMU_OVS_WT_GROUP_COUNT] = {
    {
        ES_PIPE0_OVR_SUB_GRP_WT0r, ES_PIPE0_OVR_SUB_GRP_WT1r,
        ES_PIPE0_OVR_SUB_GRP_WT2r, ES_PIPE0_OVR_SUB_GRP_WT3r
    },
    {
        ES_PIPE1_OVR_SUB_GRP_WT0r, ES_PIPE1_OVR_SUB_GRP_WT1r,
        ES_PIPE1_OVR_SUB_GRP_WT2r, ES_PIPE1_OVR_SUB_GRP_WT3r
    }
};

STATIC soc_reg_t mmu_3dbg_c_regs[2] = {
    ES_PIPE0_MMU_3DBG_Cr,
    ES_PIPE1_MMU_3DBG_Cr
};

STATIC const soc_field_t mmu_ovr_group_wt_select_fields[] = {
    GRP0f, GRP1f, GRP2f, GRP3f,
    GRP4f, GRP5f, GRP6f, GRP7f
};

/* OverSub TDM Database */
typedef struct _soc_td2p_ovr_sub_pgw_tdm_s {
    int tdm_update[_PGW_MASTER_COUNT][_PGW_TDM_OVS_SIZE];
    int phy_port[_PGW_MASTER_COUNT][_PGW_TDM_OVS_SIZE];
    int spacing[_PGW_MASTER_COUNT][_PGW_TDM_OVS_SIZE];
} _soc_td2p_ovr_sub_pgw_tdm_t;

typedef struct _soc_td2p_ovr_sub_mmu_tdm_s {
    int grp_wt_sel[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT];
    int tdm_update[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT]
                                    [_MMU_OVS_GROUP_TDM_LENGTH];
    int grp_wt[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT]
                                    [_MMU_OVS_GROUP_TDM_LENGTH];
    int mmu_port[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT]
                                    [_MMU_OVS_GROUP_TDM_LENGTH];
} _soc_td2p_ovr_sub_mmu_tdm_t;

typedef struct _soc_td2p_ovr_sub_tdm_s {
    _soc_td2p_ovr_sub_pgw_tdm_t pgw_tdm;
    _soc_td2p_ovr_sub_mmu_tdm_t mmu_tdm;
} _soc_td2p_ovr_sub_tdm_t;

enum _td2p_ovs_wt_speed_group {
    td2p_ovs_wt_group_speed_10G,
    td2p_ovs_wt_group_speed_20G,
    td2p_ovs_wt_group_speed_40G,
    td2p_ovs_wt_group_speed_NA
};

#define NUM_TSC4_PORT_MACRO 32
static int soc_td2p_port_macro_to_port_mapping[NUM_TSC4_PORT_MACRO] = {1, 1, 1, 13, 17, 
                                        21, 21, 21, 33, 33, 33, 45, 49,
                                        53, 53, 53, 65, 65, 65, 77, 81,
                                        85, 85, 85, 97, 97, 97, 109, 113,
                                        117, 117, 117};

/* defines for Reserved Buffer for Flex Port Macro when new Flex Port is
   enabled */
/* lossless defines */
#define _TD2P_RSVD_BUF_FLEX_HDRM_LOSSLESS 182
#define _TD2P_RSVD_BUF_FLEX_PGMIN_LOSSLESS 8

#define _TD2P_RSVD_BUF_FLEX_QGRP 16
#define _TD2P_RSVD_BUF_FLEX_ASF 4

/* defines for Reserved Buffer for Ports when Flex Port for the macro is NOT
   enabled*/
#define _TD2P_RSVD_BUF_NON_FLEX_100G_HDRM_LOSSLESS 558
#define _TD2P_RSVD_BUF_NON_FLEX_40G_HDRM_LOSSLESS 461
#define _TD2P_RSVD_BUF_NON_FLEX_20G_HDRM_LOSSLESS 262
#define _TD2P_RSVD_BUF_NON_FLEX_10G_HDRM_LOSSLESS 182

#define _TD2P_RSVD_BUF_NON_FLEX_PGMIN 8
#define _TD2P_RSVD_BUF_NON_FLEX_QGRP 16
#define _TD2P_RSVD_BUF_NON_FLEX_ASF 16

#define _TD2P_RSVD_BUF_NON_FLEX_10G_ASF 4

int get_mmu_mode (int unit)
{
    int is16Mmode = 0;
    char *str, *str_end;
    int mmu_size = 0;

    if (SAL_BOOT_QUICKTURN) {
        if ((str = soc_property_get_str(unit, "mmu_mem_size")) != NULL) {
            mmu_size = sal_ctoi(str, &str_end);
            if (str != str_end) {
                if ( mmu_size == 16 ) {
                    is16Mmode = 1;
                }
            } 
        }
    } else {
        is16Mmode = 1;
    }

    return is16Mmode;
}

mmu_ovs_group_wt_regs_t 
get_mmu_ovs_group_wt_regs (int unit, mmu_ovs_group_wt_regs_t old)
{
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        return mmu_ovs_group_wt_regs_td2p_local;
    }
    return old;
}

int soc_td2p_if_full_chip (int unit)
{
    int full_chip = 1;

    return full_chip;
}

int soc_td2p_show_voltage(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, avg;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval)); 
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 4);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_sleep(1);
    avg = 0;

    /* Read Voltages. Ignores result of PVTMON8 */
    for (index = 0; index < COUNTOF(pvtmon_result_reg) - 1; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        /* (PVT_DATA * 880) / 1024 * 0.7 */
        fval = (fval * 880 * 10) / (1024 * 7);
        avg += fval;
        LOG_CLI((BSL_META_U(unit,
                            "Voltage monitor %d: voltage = %d.%03dV\n"),
                 index, (fval/1000), (fval %1000)));
    }

    avg /= (COUNTOF(pvtmon_result_reg) - 1);
    LOG_CLI((BSL_META_U(unit,
                        "Average voltage is = %d.%03dV\n"),
             (avg/1000), (avg %1000)));

    return SOC_E_NONE;
}

int soc_td2p_set_obm_registers (int unit, soc_reg_t reg, int speed, int index,
                int obm_inst,  int lossless , int default_flag)
{
    int xon, xoff;
    uint64 ctrl_rval64;
    int maxt, mint = 0;
    int lowprit;
    int div;

    if (PGW_OBM_INIT_HW_DEFAULT == default_flag) {
        maxt = 0;
        xoff = 0;
        xon  = 0;
        lowprit = 0;
    } else {
        if ( speed > 20000) {
            div = 1;
            xoff = 0x2C3;
            xon  = 0x2AB;
            lowprit = 0x2AE;
        } else if (speed > 10000) {
            div = 2;
            xoff = 0x13B;
            xon  = 0x12F;
            lowprit = 0x132;
        } else {
            div = 4;
            xoff = 0x4F;
            xon  = 0x49;
            lowprit = 0x4c;
        }
        maxt = GET_NUM_CELLS_PER_OBM(unit) / div;

        if ( !lossless ) {
            xon = xoff = 0x7FF;
            lowprit = maxt;
        }
    }

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          MIN_THRESHOLDf, mint);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          LOW_PRI_THRESHOLDf, lowprit);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          MAX_THRESHOLDf, maxt);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          FLOW_CTRL_XONf, xon);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          FLOW_CTRL_XOFFf, xoff);

    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));


    return SOC_E_NONE;
}

/* Probablistic Drop configuration for TD2PLUS:
 * These settings cannot be changed dynamically, According to Architecture team
 * these settings are optimized for all the OverSubscription ratios. Hence not
 * giving any configuration options for these fields. 
 * OverSub feature is enabled by default for TD2PLUS.
 */
int soc_td2p_obm_prob_drop_default_set (int unit, int xlp, int index,
                                         int obm_inst, int default_flag)
{
    static const soc_reg_t pgw_obm_prob_drop_regs[] = {
                PGW_OBM0_LOW_PRI_DROP_PROBr, PGW_OBM1_LOW_PRI_DROP_PROBr,
                PGW_OBM2_LOW_PRI_DROP_PROBr, PGW_OBM3_LOW_PRI_DROP_PROBr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;

    reg = pgw_obm_prob_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    if (PGW_OBM_INIT_HW_DEFAULT == default_flag) {
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, EN_DROP_PROBf, 0);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XOFFf, 0);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_ABOVE_THRf, 0);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_BELOW_THRf, 0);
    } else {
        /* By default enable Probabilistic dropping */
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, EN_DROP_PROBf, 1);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XOFFf,
                              _TD2P_PROB_DROP_SOP_XOFF);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_ABOVE_THRf,
                              _TD2P_PROB_DROP_SOP_XON_ABOVE_THR);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_BELOW_THRf,
                              _TD2P_PROB_DROP_SOP_XON_BELOW_THR);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

/* Cut through is allowed if usage is less than or equal to
 * CUT_THROUGH_OKf threshold.  Granularity is 16B cell.
 */
int soc_td2p_obm_cut_through_threshold_set (int unit, int xlp, int index,
                                            int obm_inst, int default_flag)
{
    static const soc_reg_t pgw_obm_threshold2_regs[] = {
        PGW_OBM0_THRESHOLD2r, PGW_OBM1_THRESHOLD2r,
        PGW_OBM2_THRESHOLD2r, PGW_OBM3_THRESHOLD2r
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;

    reg = pgw_obm_threshold2_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    if ( PGW_OBM_INIT_HW_DEFAULT == default_flag ) {
        soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                              CUT_THROUGH_OKf, 0);
    } else {
        soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                              CUT_THROUGH_OKf, 2);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));
    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_high_pri_byte_drop_default_set (int unit, int xlp, int index,
                                             int obm_inst)
{
    static const soc_reg_t pgw_obm_high_pri_byte_drop_regs[] = {
                PGW_OBM0_HIGH_PRI_BYTE_DROPr, PGW_OBM1_HIGH_PRI_BYTE_DROPr,
                PGW_OBM2_HIGH_PRI_BYTE_DROPr, PGW_OBM3_HIGH_PRI_BYTE_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_high_pri_byte_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_high_pri_pkt_drop_default_set (int unit, int xlp, int index,
                                            int obm_inst)
{
    static const soc_reg_t pgw_obm_high_pri_pkt_drop_regs[] = {
                PGW_OBM0_HIGH_PRI_PKT_DROPr, PGW_OBM1_HIGH_PRI_PKT_DROPr,
                PGW_OBM2_HIGH_PRI_PKT_DROPr, PGW_OBM3_HIGH_PRI_PKT_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_high_pri_pkt_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_low_pri_byte_drop_default_set (int unit, int xlp, int index,
                                            int obm_inst)
{
    static const soc_reg_t pgw_obm_low_pri_byte_drop_regs[] = {
                PGW_OBM0_LOW_PRI_BYTE_DROPr, PGW_OBM1_LOW_PRI_BYTE_DROPr,
                PGW_OBM2_LOW_PRI_BYTE_DROPr, PGW_OBM3_LOW_PRI_BYTE_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_low_pri_byte_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_low_pri_pkt_drop_default_set (int unit, int xlp, int index,
                                           int obm_inst)
{
    static const soc_reg_t pgw_obm_low_pri_pkt_drop_regs[] = {
                PGW_OBM0_LOW_PRI_PKT_DROPr, PGW_OBM1_LOW_PRI_PKT_DROPr,
                PGW_OBM2_LOW_PRI_PKT_DROPr, PGW_OBM3_LOW_PRI_PKT_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_low_pri_pkt_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}


int soc_td2p_mmu_delay_insertion_set (int unit, int port, int speed)
{
    int phy_port, mmu_port;    
    int pipe;
    int val = 0;
    uint32 rval;
    soc_info_t *si;

    si = &SOC_INFO(unit);      

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port] & 0x3F;

    if ( mmu_port < SOC_TD2P_MAX_MMU_PORTS_PER_PIPE ) {
        pipe = SOC_PBMP_MEMBER(si->ypipe_pbm, port) ? 1 : 0;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, 
                    mmu_3dbg_c_regs [ pipe ], REG_PORT_ANY, mmu_port, &rval));
        /* register format is the same for PIPE0 and PIPE1 */
        if (IS_OVERSUB_PORT(unit, port )) {
            if ( speed <= 10000 ) {
                val = 15;
            } else if ( speed <= 20000 ) {
                val = 30;
            } else if ( speed <= 25000 ) {
                val = 40;
            } else if ( speed <= 40000 ) {
                val = 60;
            } else {
            }
        } else {
            /* linerate port */
        }
        soc_reg_field_set(unit, ES_PIPE0_MMU_3DBG_Cr, &rval, FIELD_Af, val);

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, 
                    mmu_3dbg_c_regs [ pipe ], REG_PORT_ANY, mmu_port, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_subport_init
 * Purpose:
 *      Setup the pbm fields from config variables for the COE application
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      NONE
 */
void soc_td2p_subport_init(int unit)
{
    soc_port_t  port;
    soc_info_t *si=&SOC_INFO(unit);
    soc_pbmp_t  pbmp_subport;
    int         num_subport = 0;

    SOC_PBMP_CLEAR(si->subtag_pbm);
    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
    SOC_PBMP_CLEAR(si->subtag.bitmap);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        si->port_num_subport[port] = 0;
    }

    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);

    SOC_PBMP_ASSIGN(si->subtag_allowed_pbm, pbmp_subport);

    if (SOC_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
        /* Iterate through SubTag ports and reserve subport indices */
        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {

            
            num_subport =
                soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

            if (num_subport > _BCM_TD2P_MAX_SUBPORT_COE_PER_PORT) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "num_subports_%d on port %d exceed max value,"
                              "setting it to the max allowed value %d\n\n"), 
                              num_subport, port,
                              _BCM_TD2P_MAX_SUBPORT_COE_PER_PORT));
                num_subport = _BCM_TD2P_MAX_SUBPORT_COE_PER_PORT;
            }

            if (port < SOC_MAX_NUM_PORTS) {
                si->port_num_subport[port] = num_subport;

            }
        }
    }

    /* Initialize the number of COE modules */
    si->num_coe_modules = BCM_TD2P_MAX_COE_MODULES;
}

int
soc_td2p_mem_config(int unit, uint16 dev_id, uint8 rev_id)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;

    int         index_max;
    int         ipmc_tbl [] = { L3_IPMC_1m, L3_IPMC_REMAPm, L3_IPMCm, 
                                EGR_IPMCm};
    int         ipmc_tindex;
            
    
    sop = SOC_PERSIST(unit);
    memState = sop->memState;

    switch ( dev_id ) {
        case BCM56760_DEVICE_ID :
            index_max = 4 * 1024;
            break;
        case BCM56832_DEVICE_ID :
            index_max = 8 * 1024;
            break;
        default :
            index_max = 16 * 1024;
            break;
    }
    index_max--; /* index is a last element so = size - 1 */

    for (ipmc_tindex = 0; ipmc_tindex < sizeof(ipmc_tbl)/sizeof(ipmc_tbl[0]); 
                          ipmc_tindex++ ) {
        memState[ipmc_tbl[ipmc_tindex]].index_max = index_max;
    }

    if (dev_id == BCM56832_DEVICE_ID) { 
        index_max = 8 * 1024;
    } else {
        index_max = 12 * 1024;
    }
    index_max--;
    memState[L3_IIFm].index_max = index_max;

    switch (dev_id) {
        case BCM56833_DEVICE_ID:
            /* SubPort Tag (CoE) */
            memState[SUBPORT_TAG_SGPP_MAPm].index_max = -1;
            memState[SUBPORT_TAG_SGPP_MAP_ONLYm].index_max = -1;
            memState[SUBPORT_TAG_SGPP_MAP_DATA_ONLYm].index_max = -1;
            memState[EGR_LPORT_PROFILEm].index_max = -1;
            memState[EGR_SUBPORT_TAG_DOT1P_MAPm].index_max = -1;

            /* VXLAN */
            /* memState[MODID_BASE_PTRm].index_max = -1; */

            /* coverity[fallthrough:FALSE] */
        case BCM56867_DEVICE_ID:
            /* FCoE */
            memState[ING_FC_HEADER_TYPEm].index_max = -1;
            memState[ING_VFT_PRI_MAPm].index_max = -1;
            memState[ING_VSANm].index_max = -1;
            memState[FC_MAP_PROFILEm].index_max = -1;
            memState[FCOE_HOP_COUNT_FNm].index_max = -1;
            memState[EGR_VSAN_INTPRI_MAPm].index_max = -1;
            memState[EGR_FCOE_CONTROL_1m].index_max = -1;
            memState[EGR_VFT_FIELDS_PROFILEm].index_max = -1;
            memState[EGR_VFT_PRI_MAPm].index_max = -1;
            memState[EGR_FC_HEADER_TYPEm].index_max = -1;

            /* NAT */
            memState[ING_SNATm].index_max = -1;
            memState[ING_SNAT_DATA_ONLYm].index_max = -1;
            memState[ING_SNAT_HIT_ONLYm].index_max = -1;
            memState[ING_SNAT_HIT_ONLY_Xm].index_max = -1;
            memState[ING_SNAT_HIT_ONLY_Ym].index_max = -1;
            memState[ING_SNAT_ONLYm].index_max = -1;
            memState[ING_DNAT_ADDRESS_TYPEm].index_max = -1;
            memState[EGR_NAT_PACKET_EDIT_INFOm].index_max = -1;

            /* TRILL */
            memState[ING_TRILL_PARSE_CONTROLm].index_max = -1;
            memState[ING_TRILL_PAYLOAD_PARSE_CONTROLm].index_max = -1;
            memState[EGR_TRILL_PARSE_CONTROLm].index_max = -1;
            memState[EGR_TRILL_PARSE_CONTROL_2m].index_max = -1;
            memState[EGR_TRILL_RBRIDGE_NICKNAMESm].index_max = -1;
            memState[EGR_TRILL_TREE_PROFILEm].index_max = -1;
            break;
        case BCM56860_DEVICE_ID:
            if (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE) {
                if (SOC_SWITCH_BYPASS_MODE(unit) ==
                        SOC_SWITCH_BYPASS_MODE_L3_AND_FP) {
    
                    /* IFP */
                    memState[FP_RANGE_CHECKm].index_max = -1;
                    memState[FP_GLOBAL_MASK_TCAMm].index_max = -1;
                    memState[FP_GM_FIELDSm].index_max = -1;
                    memState[FP_TCAMm].index_max = -1;
                    memState[FP_SLICE_MAPm].index_max = -1;
                    memState[FP_POLICY_TABLEm].index_max = -1;
                    memState[FP_METER_TABLEm].index_max = -1;
                    memState[FP_COUNTER_TABLEm].index_max = -1;
                    memState[FP_COUNTER_TABLE_Xm].index_max = -1;
                    memState[FP_COUNTER_TABLE_Ym].index_max = -1;
                    memState[FP_STORM_CONTROL_METERSm].index_max = -1;
                    memState[FP_SLICE_KEY_CONTROLm].index_max = -1;
                    memState[FP_PORT_METER_MAPm].index_max = -1;
                    memState[FP_I2E_CLASSID_SELECTm].index_max = -1;
                    memState[FP_HG_CLASSID_SELECTm].index_max = -1;
                     
                    /* EFP */
                    memState[EFP_TCAMm].index_max = -1;
                    memState[EFP_POLICY_TABLEm].index_max = -1;
                    memState[EFP_METER_TABLEm].index_max = -1;
                    memState[EGR_PW_INIT_COUNTERSm].index_max = -1;

                }
    
            /* IVXLT */
            memState[VLAN_PROTOCOLm].index_max = -1;
            memState[VLAN_PROTOCOL_DATAm].index_max = -1;
            memState[VLAN_SUBNETm].index_max = -1;
            memState[VLAN_SUBNET_ONLYm].index_max = -1;
            memState[VLAN_SUBNET_DATA_ONLYm].index_max = -1;
            memState[VLAN_MACm].index_max = -1;
            memState[VLAN_XLATEm].index_max = -1;
            memState[VFP_TCAMm].index_max = -1;
            memState[VFP_POLICY_TABLEm].index_max = -1;
            memState[ING_VLAN_TAG_ACTION_PROFILEm].index_max = -1;
            memState[MPLS_ENTRYm].index_max = -1;
            memState[UDF_CONDITIONAL_CHECK_TABLE_CAMm].index_max = -1;
            memState[UDF_CONDITIONAL_CHECK_TABLE_RAMm].index_max = -1;
            memState[ING_ETAG_PCP_MAPPINGm].index_max = -1;
            memState[VLAN_XLATE_ECCm].index_max = -1;
            memState[MPLS_ENTROPY_LABEL_DATAm].index_max = -1;
            memState[ING_ETAG_PCP_MAPPINGm].index_max = -1;

            }
            break;
        default:
            ;

    }

    return SOC_E_NONE;
}

int
soc_td2p_get_shared_bank_size(int unit, uint16 dev_id, uint8 rev_id)
{
    int bank_size = BCM_TD2P_SMALL_BANK_SIZE;


    switch ( dev_id ) {
    case BCM56867_DEVICE_ID :
        bank_size = BCM_TD2P_MAX_BANK_SIZE;
        break;
    }

    return bank_size;
}

uint32* soc_td2p_mmu_params_arr_get (uint16 dev_id, uint8 rev_id)
{

    switch (dev_id) {
        case BCM56860_DEVICE_ID:
        case BCM56861_DEVICE_ID:
        case BCM56862_DEVICE_ID:
        case BCM56864_DEVICE_ID:
        case BCM56865_DEVICE_ID:
        case BCM56867_DEVICE_ID:
            return soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_16MB];
            break;
        case BCM56832_DEVICE_ID:
            return soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_9MB];
            break;
        case BCM56833_DEVICE_ID:
            return soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_12MB];
            break;
    }

    return NULL;
}

/*
 * Function:
 *      trident2p_ovstb_toggle
 * Purpose:
 *      Execute the OVST sequence for Trident2Plus_A0 devices
 * Parameters:
 *      unit - StrataSwitch unit #
*/
void
trident2p_ovstb_toggle(int unit)
{
    int i = 0;
    int array_length = 0;
    int write_data_count = 0;
    uint32 read_data = 0;

    /* coverity[stack_use_local_overflow : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    STATIC uint32 trident2p_write_data[] = {
        0x1fffffc0, 0xbffffbfe, 0x1dfcbe80, 0xbffffbfe, 0x40000000, 0x40000200,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1dfcfe80, 0xbffffbfe, 0x40000000, 0x40000200, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0x1dfc7e80, 0xbffffbfe, 0x40000000, 0x40080000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x80000180, 0x1dfc3e80, 0xbffffbfe, 0x40000000, 0x40080000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcde80, 0xbffffbfe,
        0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x80000180, 0x1dfc9e80, 0xbffffbfe, 0x44000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0xbfcbe80, 0xbffffbfe, 0x40000000, 0x40001000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x80000180, 0xbfc7e80, 0xbffffbfe, 0x40000000, 0x40001000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfc3e80, 0xbffffbfe,
        0x40000000, 0x40080000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0xbfcde80, 0xbffffbfe, 0x40000000, 0x41000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfc3e80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfcde80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x17fc7e80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x17fc3e80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x13fcbe80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x13fc7e80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xffcfe80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xffcbe80,
        0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc5e80,
        0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc1e80, 0xbffffbfe,
        0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc5e80, 0xbffffbfe, 0x40000000,
        0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc1e80, 0xbffffbfe, 0x40000000,
        0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1bfcfe80, 0xbffffbfe, 0x40000000,
        0x48000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfcbe80,
        0xbffffbfe, 0x40000000, 0x48000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xdfc5e80, 0xbffffbfe, 0x40000000, 0x40000002, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0xdfc1e80, 0xbffffbfe, 0x40000000, 0x40200000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xbfcfe80, 0xbffffbfe, 0x40000000,
        0x40200000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0xbfc9e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1dfcbe80, 0xbffffbfe, 0x40000000, 0x40000200, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0x1dfcbe80, 0xbffffbfe, 0x40000000, 0x40000200, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcfe80,
        0xbffffbfe, 0x40000000, 0x40000200, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcfe80, 0xbffffbfe,
        0x40000000, 0x40000200, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc7e80, 0xbffffbfe, 0x40000000,
        0x40080000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfc7e80,
        0xbffffbfe, 0x40000000, 0x40080000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1dfc3e80, 0xbffffbfe, 0x40000000, 0x40080000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc3e80, 0xbffffbfe, 0x40000000,
        0x40080000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcde80,
        0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfcde80, 0xbffffbfe, 0x44000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1dfc9e80, 0xbffffbfe, 0x44000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfc9e80,
        0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xbfcbe80, 0xbffffbfe, 0x40000000,
        0x40001000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfcbe80,
        0xbffffbfe, 0x40000000, 0x40001000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xbfc7e80, 0xbffffbfe, 0x40000000, 0x40001000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xbfc7e80, 0xbffffbfe, 0x40000000,
        0x40001000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfc3e80,
        0xbffffbfe, 0x40000000, 0x40080000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xbfc3e80, 0xbffffbfe, 0x40000000, 0x40080000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xbfcde80, 0xbffffbfe, 0x40000000,
        0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000,
        0x40000000, 0x80000180, 0xbfcde80, 0xbffffbfe, 0x40000000, 0x41000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1bfc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1bfc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1bfcde80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1bfcde80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x17fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x17fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x17fc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x17fc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x13fcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x13fcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x13fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x13fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xffcfe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xffcfe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xffcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0xffcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1ffc5e80, 0xbffffbfe, 0x44000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0x1ffc5e80, 0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc1e80,
        0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc1e80, 0xbffffbfe,
        0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc5e80, 0xbffffbfe, 0x40000000,
        0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc5e80, 0xbffffbfe, 0x40000000,
        0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc1e80, 0xbffffbfe, 0x40000000,
        0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1dfc1e80, 0xbffffbfe, 0x40000000,
        0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1bfcfe80, 0xbffffbfe, 0x40000000,
        0x48000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfcfe80,
        0xbffffbfe, 0x40000000, 0x48000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x80000180, 0x1bfcbe80, 0xbffffbfe, 0x40000000, 0x48000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0x1bfcbe80, 0xbffffbfe, 0x40000000,
        0x48000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xdfc5e80,
        0xbffffbfe, 0x40000000, 0x40000002, 0x40000000, 0x40000000, 0x41000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xdfc5e80, 0xbffffbfe,
        0x40000000, 0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xdfc1e80, 0xbffffbfe, 0x40000000,
        0x40200000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0xdfc1e80, 0xbffffbfe, 0x40000000, 0x40200000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xbfcfe80, 0xbffffbfe, 0x40000000,
        0x40200000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
        0xbfcfe80, 0xbffffbfe, 0x40000000, 0x40200000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x80000180, 0xbfc9e80, 0xbffffbfe, 0x40000000,
        0x40000008, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000,
        0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfc9e80, 0xbffffbfe,
        0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180
    };
      
    array_length = sizeof(trident2p_write_data) / sizeof(uint32); 
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2e);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2e);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2f);
    READ_TOP_UC_TAP_CONTROLr(unit, &read_data);

    for ( i=0; i < array_length; i++)
    {
        WRITE_TOP_UC_TAP_WRITE_DATAr(unit, trident2p_write_data[write_data_count]);
        WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2f);
        READ_TOP_UC_TAP_READ_DATAr(unit, &read_data);
        write_data_count++;
    
    }
      
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2f);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2e);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2c);

    return;    
}


/*
 * Function:
 *      soc_td2p_idb_buf_reset
 * Purpose:
 *      Poll until Cell Assembly and OBM buffers are empty.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_idb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    soc_field_t field;
    uint64 rval64;
    int phy_port;
    int pgw;
    int xlp;
    int port_index;
    int obm;
    soc_timeout_t to;
    int use_count;
    int fifo_empty;
    static const soc_reg_t pgw_obm_use_counter_regs[] = {
        PGW_OBM0_USE_COUNTERr, PGW_OBM1_USE_COUNTERr,
        PGW_OBM2_USE_COUNTERr, PGW_OBM3_USE_COUNTERr
    };
    static const soc_field_t pgw_obm_use_counter_fields[] = {
        PORT0_USE_COUNTf, PORT1_USE_COUNTf,
        PORT2_USE_COUNTf, PORT3_USE_COUNTf
    };

    /* If not reset, nothing to do */
    if (!reset) {
        return SOC_E_NONE;
    }

    {
        /* Temporary check until new port up/down sequence is fully tested */
        int new_sequence;
        new_sequence = soc_property_get(unit, "new_up_down", 1);
        if (!new_sequence) {
            return SOC_E_NONE;
        }
    }

    /* Get physical port PGW information */
    phy_port = si->port_l2p_mapping[port];
    SOC_IF_ERROR_RETURN
        (soc_td2p_phy_port_pgw_info_get(unit, phy_port,
                                        &pgw, &xlp, &port_index));

    /* Assume values returned above are valid */
    reg = pgw_obm_use_counter_regs[xlp];
    field = pgw_obm_use_counter_fields[port_index];
    obm = pgw | SOC_REG_ADDR_INSTANCE_MASK;

    if (!SAL_BOOT_SIMULATION) {
        /* Poll until Cell Assembly and OBM buffers are empty */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            /* Get PGW use counter */
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm, port_index, &rval64));
            use_count = soc_reg64_field32_get(unit, reg, rval64, field);

            /* Get PGW fifo empty flag */
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_BOD_STATUS1r,
                                            obm, port_index, &rval64));
            fifo_empty = soc_reg64_field32_get(unit, PGW_BOD_STATUS1r,
                                               rval64, PGW_BOD_FIFO_EMPTYf);

            if ((use_count == 0) && (fifo_empty == 1)) {
                break;
            }

            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "PGW buffer reset timeout: port %d, %s, "
                                      "timeout (use_count: %d) "
                                      "(pgw_bod_fifo_emty:%d)\n"),
                           unit, SOC_PORT_NAME(unit, port),
                           use_count, fifo_empty));
                return SOC_E_INTERNAL;
            }
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_mmu_ep_credit_reset
 * Purpose:
 *      Initalize EP credits in MMU so EP is in charge
 *      of distributing the correct number of credits.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_mmu_ep_credit_reset(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;
    int mmu_port;
    soc_reg_t reg;
    uint32 rval;

    {
        /* Temporary check until new port up/down sequence is fully tested */
        int new_sequence;
        new_sequence = soc_property_get(unit, "new_up_down", 1);
        if (!new_sequence) {
            return SOC_E_NONE;
        }
    }

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    /* Get MMU port */
    mmu_port = si->port_p2m_mapping[phy_port];
    if (mmu_port == -1) {
        return SOC_E_INTERNAL;
    }
    mmu_port &= 0x3F;    /* Convert mmu port number for pipe */

    if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) {
        reg = ES_PIPE0_MMU_PORT_CREDITr;
    } else {
        reg = ES_PIPE1_MMU_PORT_CREDITr;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, mmu_port, &rval));
    soc_reg_field_set(unit, reg, &rval, INIT_CREDITf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_edb_buf_reset
 * Purpose:
 *      Initialize EP credits in MMU, release EDB port buffer and
 *      enable cell request in EP.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_edb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port;
    uint32 rval;
    int level;
    soc_timeout_t to;

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    if (reset) {
        /* Set register to get number of used entries in EDB buffer */
        SOC_IF_ERROR_RETURN
            (READ_EGR_EDB_MISC_CTRLr(unit, &rval));
        soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval,
                          SELECT_CURRENT_USED_ENTRIESf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_EDB_MISC_CTRLr(unit, rval));

        soc_timeout_init(&to, 250000, 0);
        
        /* Poll until EDB buffer is empty */
        for (;;) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL,
                                            phy_port, entry));
            level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm,
                                        entry, LEVELf);
            if (level == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "EDB buffer drain timeout: "
                                      "port %d, %s, "
                                      "timeout (pending: %d)\n"),
                           unit, SOC_PORT_NAME(unit, port), level));
                return SOC_E_INTERNAL;
            }
        }

        /*
         * Hold EDB port buffer in reset state and disable cell
         * request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));

        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));
    } else {

        /* Initialize EP credits in MMU */
        SOC_IF_ERROR_RETURN(_soc_td2p_mmu_ep_credit_reset(unit, port));

        /*
         * Release EDB port buffer reset and
         * enable cell request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));

        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
    }

    return SOC_E_NONE;
}


STATIC int
_soc_td2p_resource_data_check(int unit, int num_res, soc_port_resource_t *res)
{
    int i, j;
    /* null check for all resource data before accessing register */
    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];
        if (NULL == p) {
            return SOC_E_PARAM;
        }
        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];
            if (NULL == p_lane_info) {
                return SOC_E_PARAM;
            }
        }
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_pgw_obm_default_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_CONTROL
 *          PGW_OBM[0-3]_HIGH_PRI_BYTE_DROP[]
 *          PGW_OBM[0-3]_HIGH_PRI_PKT_DROP[]
 *          PGW_OBM[0-3]_LOW_PRI_BYTE_DROP[]
 *          PGW_OBM[0-3]_LOW_PRI_DROP_PROB[]
 *          PGW_OBM[0-3]_LOW_PRI_PKT_DROP[]
 *          PGW_OBM[0-3]_USE_COUNTER
 *          PGW_OBM[0-3]_SHARED_CONFIG
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_INIT_SW_DEFAULT
 *                          Use SW default value
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. master/slave pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw+1)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_td2p_pgw_obm_default_set(int unit, int num_res, soc_port_resource_t *res,
                             int default_flag)
{
    int obm_inst;
    int xlp, port_index;
    int i, j;

    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];

        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];

            xlp = p_lane_info->xlp;
            port_index = p_lane_info->port_index;
            obm_inst = p_lane_info->pgw | SOC_REG_ADDR_INSTANCE_MASK;

            /* PGW_OBM[0-3]_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_ctrl_reg_default_set(unit, xlp,
                               port_index, obm_inst, p->oversub, default_flag));
            /* PGW_OBM[0-3]_LOW_PRI_DROP_PROB[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_prob_drop_default_set (unit, xlp,
                               port_index, obm_inst, default_flag));
            /* PGW_OBM[0-3]_HIGH_PRI_BYTE_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_high_pri_byte_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_HIGH_PRI_PKT_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_high_pri_pkt_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_LOW_PRI_BYTE_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_low_pri_byte_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_LOW_PRI_PKT_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_low_pri_pkt_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_USE_COUNTER */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_use_counter_reg_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_SHARED_CONFIG */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_shared_config_reg_default_set(unit, xlp,
                               port_index, obm_inst, default_flag));
        }
    }
    return SOC_E_NONE;
}

STATIC int
soc_td2p_pgw_obm_hw_default_set(int unit, int num_res, soc_port_resource_t *res)
{
    int rv;
    rv = soc_td2p_pgw_obm_default_set(unit, num_res, res,
                                      PGW_OBM_INIT_HW_DEFAULT);
    return rv;
}

STATIC int
soc_td2p_pgw_obm_sw_default_set(int unit, int num_res, soc_port_resource_t *res)
{
    int rv;
    rv = soc_td2p_pgw_obm_default_set(unit, num_res, res,
                                      PGW_OBM_INIT_SW_DEFAULT);
    return rv;
}

/*
 * Function:
 *      soc_td2p_pgw_obm_set
 * Purpose:
 *      Reconfigure PGW_OBM registers in flex port sequence
 *      This function will do two things,
 *      1. clear all registers for old ports which exists in pre-Flex status
 *      2. initialize all registers for new ports which exists in post-Flex status
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      pre_num_res  - (IN) Number of resource data which exist in pre-Flex status
 *      post_num_res - (IN) Number of resource data which exist in post-Flex status
 *      pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *      post_res     - (IN) Resource data structure pointer in post-Flex status
 *
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_pgw_obm_set(int unit,
                     int pre_num_res,  soc_port_resource_t *pre_res,
                     int post_num_res, soc_port_resource_t *post_res)
{
    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, post_num_res, post_res));

    SOC_IF_ERROR_RETURN(soc_td2p_pgw_obm_hw_default_set(unit,
                                                        pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(soc_td2p_pgw_obm_sw_default_set(unit,
                                                        post_num_res, post_res));
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_obm_threshold_default_set
 * Purpose:
 *      Reconfigure PGW Threshold in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_THRESHOLD
 *          PGW_OBM[0-3]_THRESHOLD2
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      lossless     - (IN) Lossless configuration of device
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_INIT_SW_DEFAULT
 *                          Use SW default value
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. master/slave pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw+1)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC
soc_error_t soc_td2p_obm_threshold_default_set(int unit,
                                               int num_res,
                                               soc_port_resource_t *res,
                                               int lossless,
                                               int default_flag)
{
    pgw_obm_threshold_regs_t pgw_obm_threshold_regs =
                             soc_trident2_pgw_obm_threshold_regs_get(unit);
    soc_reg_t  reg;
    int obm_inst;
    int xlp, port_index, speed;
    int i, j;

    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];

        speed = p->speed;
        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];

            xlp        = p_lane_info->xlp;
            port_index = p_lane_info->port_index;
            obm_inst   = p_lane_info->pgw | SOC_REG_ADDR_INSTANCE_MASK;

            reg = pgw_obm_threshold_regs[xlp];
            /* PGW_OBM[0-3]_THRESHOLD */
            SOC_IF_ERROR_RETURN
                (soc_td2p_set_obm_registers(unit, reg, speed, port_index,
                                            obm_inst, lossless, default_flag));

            /* PGW_OBM[0-3]_THRESHOLD2 */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_cut_through_threshold_set(unit, xlp, port_index,
                                                     obm_inst, default_flag));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_threshold_hw_default_set(int unit, int num_res,
                                      soc_port_resource_t *res,
                                      int lossless)
{

    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_default_set(unit, num_res, res,
                                               lossless,
                                               PGW_OBM_INIT_HW_DEFAULT));
    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_threshold_sw_default_set(int unit, int num_res,
                                      soc_port_resource_t *res,
                                      int lossless)
{
    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_default_set(unit, num_res, res,
                                               lossless,
                                               PGW_OBM_INIT_SW_DEFAULT));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_obm_threshold_set
 * Purpose:
 *  Reconfigure PGW registers in flex port sequence
 *  In this function PGW OBM threshold registers will be initialized.
 *  This function will do two things,
 *  1. clear all registers for old ports which exists in pre-Flex status
 *  2. initialize all registers for new ports which exists in post-Flex status
 *
 * Parameters:
 *  unit         - (IN) Unit number.
 *  pre_num_res  - (IN) Number of resource data which exist in pre-Flex status
 *  post_num_res - (IN) Number of resource data which exist in post-Flex status
 *  pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *  post_res     - (IN) Resource data structure pointer in post-Flex status
 *  lossless     - (IN) mmu lossless information
 *
 * Returns:
 *  SOC_E_XXX
 *
 * Note : For 100G port, this functions expects the ovsersub field to be
 * disabled.
 */
soc_error_t soc_td2p_obm_threshold_set(int unit,
                         int pre_num_res, soc_port_resource_t *pre_res,
                         int post_num_res, soc_port_resource_t *post_res,
                         int lossless)
{
    
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, pre_num_res,
                                                      pre_res));
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, post_num_res,
                                                      post_res));
    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_hw_default_set(unit,
                        pre_num_res, pre_res, lossless));

    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_sw_default_set(unit, 
                        post_num_res, post_res, lossless));

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_pgw_port_default_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_MAX_USAGE:
 *          PGW_OBM_PORT[0-15]_FC_CONFIG
 *          PGW_OBM[0-3]_PRIORITY_MAP[]
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_FLEXPORT_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_FLEXPORT_INIT_SW_DEFAULT
 *                          Use SW default value
 *      lossless     - (IN) mmu lossless information
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. master/slave pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw+1)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_td2p_pgw_port_default_set(int unit, int num_res, soc_port_resource_t *res,
                              int lossless, int default_flag)
{
    int obm_inst;
    int xlp, port_index;
    int i, j;

    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];

        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];

            xlp = p_lane_info->xlp;
            port_index = p_lane_info->port_index;
            obm_inst = p_lane_info->pgw | SOC_REG_ADDR_INSTANCE_MASK;
            /* PGW_OBM[0-3]_MAX_USAGE */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_max_usage_reg_default_set(unit, xlp,
                                       port_index, obm_inst));

            if (!lossless) {
                continue;
            }

            /* PGW_OBM_PORT[0-15]_FC_CONFIG */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_fc_config_reg_default_set(unit, xlp, port_index,
                                       obm_inst, p->logical_port, p->oversub,
                                       default_flag));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_td2p_pgw_port_hw_default_set(int unit, int num_res, soc_port_resource_t *res,
                                 int lossless)
{
    int rv;
    rv = soc_td2p_pgw_port_default_set(unit, num_res, res,
                                       lossless, PGW_OBM_INIT_HW_DEFAULT);
    return rv;
}

STATIC int
soc_td2p_pgw_port_sw_default_set(int unit, int num_res, soc_port_resource_t *res,
                                 int lossless)
{
    int rv;
    rv = soc_td2p_pgw_port_default_set(unit, num_res, res,
                                       lossless, PGW_OBM_INIT_SW_DEFAULT);
    return rv;
}


STATIC
int soc_td2p_ovr_sub_tdm_config_get(int unit,
                                    _soc_td2p_ovr_sub_tdm_t *ovs_tdm)
{
    int pipe;
    int group;
    int slot, base;
    uint32 wt_group_select;
    soc_reg_t reg;
    uint32 rval;
    uint64 rval64;
    int index, obm_inst;
    int pgw, pgw_master;
    mmu_ovs_group_wt_regs_t mmu_ovs_group_wt_regs =
                                        mmu_ovs_group_wt_regs_td2p_local;

    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;
    _soc_td2p_ovr_sub_pgw_tdm_t *pgw_tdm = &ovs_tdm->pgw_tdm;
    mmu_ovs_group_regs_t mmu_ovs_group_regs =
                           soc_trident2_mmu_ovs_group_regs_get(unit);
    mmu_ovs_group_wt_select_regs_t mmu_ovs_group_wt_select_regs =
                           soc_trident2_mmu_ovs_group_wt_select_regs_get(unit);
    pgw_ovs_tdm_regs_t pgw_ovs_tdm_regs =
                           soc_trident2_pgw_ovs_tdm_regs_get(unit);
    pgw_ovs_spacing_regs_t pgw_ovs_spacing_regs =
                           soc_trident2_pgw_ovs_spacing_regs_get(unit);
    pgw_tdm_fields_t pgw_tdm_fields =
                           soc_trident2_pgw_tdm_fields_get(unit);
    pgw_spacing_fields_t pgw_spacing_fields =
                           soc_trident2_pgw_spacing_fields_get(unit);


    /* Initialize MMU OverSub TDM Ports Database */
    for (pipe = 0; pipe < TD2P_PIPES_PER_DEV; pipe++) {
        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                /* Invalid MMU Port 0x3f */
                mmu_tdm->mmu_port[pipe][group][slot] = 0x3f;
            }
        }
    }


    /* Extract MMU Over Subscription TDM */
    for (pipe = 0; pipe < TD2P_PIPES_PER_DEV; pipe++) {
        wt_group_select = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, mmu_ovs_group_wt_select_regs[pipe],
                               REG_PORT_ANY, 0, &wt_group_select));

        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            reg = mmu_ovs_group_regs[pipe][group];
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                mmu_tdm->mmu_port[pipe][group][slot] =
                                soc_reg_field_get(unit, reg, rval, MMU_PORTf);
            }
        }
        /* TD2P supports 4 oversub speeds only 10/20 and 40*/
        for (group = 0; group < _MMU_OVS_WT_GROUP_COUNT; group++) {
            reg = mmu_ovs_group_wt_regs[pipe][group];
            for (index = 0; index < _MMU_OVS_GROUP_TDM_LENGTH; index++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
                mmu_tdm->grp_wt[pipe][group][index] =
                     soc_reg_field_get(unit, reg, rval, WEIGHTf);
            }
            /*Read the Group Weight Sel fields, Used later while
             * configuring the MMU TDM.
             */
            mmu_tdm->grp_wt_sel[pipe][group] =
                soc_reg_field_get(unit,  mmu_ovs_group_wt_select_regs[pipe],
                                  wt_group_select,
                                  mmu_ovr_group_wt_select_fields[group]);
        }
    }

    /* Extract PGW Over Subscription TDM */
    for (pgw = 0; pgw < TD2P_PGWS_PER_DEV; pgw += TD2P_PGWS_PER_QUAD) {
        pgw_master = pgw;

        if (SOC_IS_TRIDENT2PLUS(unit)) {
            if (( pgw == 2 ) || ( pgw == 6 )) {
                pgw_master = pgw + 1;
            }
        }

        obm_inst = pgw_master | SOC_REG_ADDR_INSTANCE_MASK;

        /* Extract PGW oversubscription ports TDM */
        for (base = 0; base < _PGW_TDM_OVS_SIZE;
             base += _PGW_TDM_SLOTS_PER_REG) {
            reg = pgw_ovs_tdm_regs[base / _PGW_TDM_SLOTS_PER_REG];
            COMPILER_64_ZERO(rval64);
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, 0, &rval64));
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                pgw_tdm->phy_port[pgw_master / 2][slot] =
                             soc_reg64_field32_get(unit, reg, rval64,
                                                   pgw_tdm_fields[slot]);
            }
        }
        /* If Ovr Sub is Enabled then OS_TDM_ENABLEf shall also be enabled.
         * So Skip the reading of OS_TDM_ENABLEf.
         */

        /* Extract PGW oversubscription port spacing */
        for (base = 0; base < _PGW_TDM_OVS_SIZE;
             base += _PGW_TDM_SLOTS_PER_REG) {
            reg = pgw_ovs_spacing_regs[base / _PGW_TDM_SLOTS_PER_REG];
            COMPILER_64_ZERO(rval64);
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, 0, &rval64));
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                pgw_tdm->spacing[pgw_master / 2][slot] =
                            soc_reg64_field32_get(unit, reg, rval64,
                                                  pgw_spacing_fields[slot]);
            }
        }
    }

    return SOC_E_NONE;
}

/* TD2P OverSub Scheduler */
/* This function expects the oversub field in port_resource_t structure to be 0
 * for 100G ports. Since oversub on 100G ports is not supported for TD2Plus.
 *
 * This function Updates the Over Sub TDM calender.
 */
STATIC
int soc_td2p_ovr_sub_tdm_db_update(int unit, int num_port_res,
                                   soc_port_resource_t *port_res,
                                   _soc_td2p_ovr_sub_tdm_t *ovs_tdm,
                                   int db_type)
{
    int pgw_master;
    int pipe, res_count;
    int group;
    int slot;
    int index, count, no_of_lanes;
    int pgw;
    int speed_max;
    int group_wt;
    int phy_port, phy_port_update;
    int mmu_port, mmu_port_update;
    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;
    _soc_td2p_ovr_sub_pgw_tdm_t *pgw_tdm = &ovs_tdm->pgw_tdm;

    for ( res_count = 0; res_count < num_port_res; res_count++) {
        soc_port_resource_t *p = &port_res[res_count];
        /* Since the OBM Ports reside in same PGW for Td2Plus, recover the
         * PGW of lane-0.
         */
        soc_port_lane_info_t *p_lane_info = p->lane_info[0];
        /* If OBM is not enabled then continue */
        if (!p->oversub) {
            continue;
        }

        no_of_lanes = p->num_lanes;
        speed_max   = p->speed;
        pipe        = p->pipe;
        pgw         = p_lane_info->pgw;

        if (_SOC_TRIDENT2P_UPDATE_TDM_PRE  == db_type) {
            mmu_port        = p->mmu_port;
            mmu_port_update = 0x3f; /* Invalid MMU Port */
            phy_port        = p->physical_port;
            phy_port_update = 0;    /* Invalid physical port */
        } else {
            mmu_port        = 0x3f;
            mmu_port_update = p->mmu_port;      /* Valid MMU Port */
            phy_port        = 0;
            phy_port_update = p->physical_port; /* Valid physical port */
        }

        if ( speed_max <= 10000 ) {
            group_wt = td2p_ovs_wt_group_speed_10G;
        } else if ( speed_max <= 20000 ) {
            group_wt = td2p_ovs_wt_group_speed_20G;
        } else if ( speed_max <= 40000 ) {
            group_wt = td2p_ovs_wt_group_speed_40G;
        } else {
            group_wt = td2p_ovs_wt_group_speed_NA; /* "not" valid group */
        }

        /* Update MMU OverSub TDM Ports Database */
        count = 0;
        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            /* Identify the group that needs to be updated */
            if (mmu_tdm->grp_wt_sel[pipe][group] != group_wt) {
                continue;
            }
            /* Update MMU OverSub TDM Ports Database */
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                /* Invalid MMU Port 0x3f */
                if (mmu_tdm->mmu_port[pipe][group][slot] == mmu_port) {
                    mmu_tdm->tdm_update[pipe][group][slot] = 1;
                    mmu_tdm->mmu_port[pipe][group][slot] = mmu_port_update;
                    count++;
                    break;
                }
            }
            if (count) {
                break;
            }
        }

        /* MMU Port should be configured only once */
        if (1 != count) {
            return SOC_E_PARAM;
        }
        pgw_master = pgw / TD2P_PGWS_PER_QUAD;

        if (( pgw_master == 2 ) || ( pgw_master == 6 )) {
            pgw_master = pgw_master + 1;
        }

        count = 0;
        for (index = 0; index < _PGW_TDM_OVS_SIZE; index++) {
            /* Update the Physical Port */
            if (pgw_tdm->phy_port[pgw_master / 2][index] == phy_port) {
                pgw_tdm->phy_port[pgw_master / 2][index] = phy_port_update;
                pgw_tdm->tdm_update[pgw_master / 2][index] = 1;
                count++;
            }
            if (count == no_of_lanes) {
                break;
            }
        }

        if (count != no_of_lanes) {
            return SOC_E_PARAM;
        }
    }
    return SOC_E_NONE;
}

STATIC
int soc_td2p_ovr_sub_tdm_hw_update(int unit,
                                   _soc_td2p_ovr_sub_tdm_t *ovs_tdm)
{
    int pgw_master;
    int pipe;
    int group;
    int slot;
    int index = 0;
    int pgw;
    int base;
    soc_reg_t reg;
    uint32 rval;
    uint64 rval64;
    int obm_inst;
    int phy_port;
    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;
    _soc_td2p_ovr_sub_pgw_tdm_t *pgw_tdm = &ovs_tdm->pgw_tdm;
    mmu_ovs_group_regs_t mmu_ovs_group_regs =
                           soc_trident2_mmu_ovs_group_regs_get(unit);
    pgw_ovs_tdm_regs_t pgw_ovs_tdm_regs =
                           soc_trident2_pgw_ovs_tdm_regs_get(unit);
    pgw_tdm_fields_t pgw_tdm_fields =
                           soc_trident2_pgw_tdm_fields_get(unit);

    

    /* Initialize MMU OverSub TDM Ports Database */
    for (pipe = 0; pipe < TD2P_PIPES_PER_DEV; pipe++) {
        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                if (mmu_tdm->tdm_update[pipe][group][slot]) {
                    /* Invalid MMU Port 0x3f */
                    reg = mmu_ovs_group_regs[pipe][group];

                    SOC_IF_ERROR_RETURN
                     (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));

                    soc_reg_field_set(unit, reg, &rval, MMU_PORTf,
                             mmu_tdm->tdm_update[pipe][group][slot] & 0x3f);
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
                }
            }
        }
    }

    /* Extract PGW Over Subscription TDM */
    for (pgw = 0; pgw < TD2P_PGWS_PER_DEV; pgw += TD2P_PGWS_PER_QUAD) {
        pgw_master = pgw;

        if (( pgw == 2 ) || ( pgw == 6 )) {
            pgw_master = pgw + 1;
        }

        obm_inst = pgw_master | SOC_REG_ADDR_INSTANCE_MASK;

        /* Update PGW oversubscription ports TDM */
        for (base = 0; base < _PGW_TDM_OVS_SIZE;
                       base += _PGW_TDM_SLOTS_PER_REG) {
            reg = pgw_ovs_tdm_regs[base / _PGW_TDM_SLOTS_PER_REG];
            COMPILER_64_ZERO(rval64);
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                if (!pgw_tdm->tdm_update[pgw_master / 2][slot]) {
                    continue;
                }
                phy_port =  pgw_tdm->phy_port[pgw_master / 2][slot];
                soc_reg64_field32_set(unit, reg, &rval64,
                                                   pgw_tdm_fields[slot],
                                                   phy_port);
            }
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, obm_inst, 0, rval64));
        }
        
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_ovr_sub_tdm_update
 * Description:
 *      Update PGW and MMU TDM Over subscription programming for flex port
 * Parameters:
 *      unit          - Device number
 *      pre_num_res   - Number of Previous resources
 *      pre_res       - Previous resource information.
 *      post_num_res  - Number of Current resource configured
 *      post_res      - Current port resource information.
 */
int
soc_td2p_ovr_sub_tdm_update(int unit, 
                         int pre_num_res, soc_port_resource_t *pre_res,
                         int post_num_res, soc_port_resource_t *post_res)
{
    static _soc_td2p_ovr_sub_tdm_t ovr_sub_tdm;

    sal_memset(&ovr_sub_tdm, 0, sizeof(_soc_td2p_ovr_sub_tdm_t));
    
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, pre_num_res,
                                                      pre_res));
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, post_num_res,
                                                      post_res));

    /* Recover the Over_Sub PGW and MMU TDM configuration from registers.
     * Construct software database.
     */
    SOC_IF_ERROR_RETURN(soc_td2p_ovr_sub_tdm_config_get(unit,
                                                     &ovr_sub_tdm));
    /* Update the Pre OBM TDM configuration
     * Free's the pre-occupied slots*/
    SOC_IF_ERROR_RETURN(soc_td2p_ovr_sub_tdm_db_update(unit, pre_num_res,
                                               pre_res,
                                               &ovr_sub_tdm,
                                               _SOC_TRIDENT2P_UPDATE_TDM_PRE));
    /* Update the Post OBM TDM configuration
     * Updates the Empty slots*/
    SOC_IF_ERROR_RETURN(soc_td2p_ovr_sub_tdm_db_update(unit, post_num_res,
                                             post_res,
                                             &ovr_sub_tdm,
                                             _SOC_TRIDENT2P_UPDATE_TDM_POST));

    /* Updates the OBM TDM configuration in the Hardware */
    SOC_IF_ERROR_RETURN(soc_td2p_ovr_sub_tdm_hw_update(unit,
                                                       &ovr_sub_tdm));

    return SOC_E_NONE;
}


/*
 * This function is used to enable or disable cut through when a port is 
 * operating in store and forward mode. 
 * Refer TD2_Cutthrough_programming_guideline.pdf
 * Caller must pass the correct port_info pointer, corresponding to the port
 * being configured (and sent by the BCM layer)
 * To enable cut-through, pass enable = 1
 * To disable cut-through, pass enable = 0
 */
STATIC int soc_td2p_cut_thru_enable_disable(int unit,
                                            soc_port_resource_t *port_info,
                                            int enable)
{
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si;
    uint32 speed;

    si = &SOC_INFO(unit);

    /* Mask unnecessary bits */
    enable &= 0x1;

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port_info->logical_port,
                        &rval));

    speed = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, ASF_PORT_SPEEDf);

    /* Set port speed to 0 */
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, ASF_PORT_SPEEDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port_info->logical_port,
                        rval));

    if (!enable) {
        /* Wait 8ms for draining all cells */
        sal_usleep(8000);
    }

    sal_memset(entry, 0, sizeof(egr_edb_xmit_ctrl_entry_t));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_EDB_XMIT_CTRLm,
                                     MEM_BLOCK_ALL,
                                     port_info->physical_port, &entry));


    if (!enable) {
        
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, START_CNTf, 0);
    }

    if (port_info->oversub) {
        if (!enable) {
            soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry,
                                WAIT_FOR_2ND_MOPf, 1);
        } else {
            soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, START_CNTf,
                                _TD2P_OVERSUB_START_CNT);
        }
    } else {
        if (!enable) {
            soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry,
                                WAIT_FOR_MOPf, 1);
        } else {
            soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, START_CNTf,
                                _TD2P_LINERATE_START_CNT(port_info->speed));
        }
    }

    if (enable) {
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, WAIT_FOR_MOPf, 0);
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, WAIT_FOR_2ND_MOPf,
                            0);
    }

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_EDB_XMIT_CTRLm,
                                      MEM_BLOCK_ALL,
                                      port_info->physical_port, &entry));

    /* Set UC_ASF_ENABLE, MC_ASF_ENABLE bits to 1 */
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, UC_ASF_ENABLEf, enable);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, MC_ASF_ENABLEf, enable);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port_info->logical_port,
                                            rval));

    if (enable) {
        /* Wait 1 micro-sec */
        sal_usleep(1);
    }

    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, ASF_PORT_SPEEDf, speed);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port_info->logical_port,
                                            rval));

    return SOC_E_NONE;
}

int soc_td2p_max_speed_port_find(int phy_port, 
                                        soc_port_resource_t *port_info,
                                        int port_info_arr_size) 
{
    int idx, index, speed;
  
    index = 0;
    speed = port_info[index].speed;

    for (idx = 1; idx < port_info_arr_size; idx++) {
        /* If all speeds are equal, '>' will return the 1st index,
         * '>=' will return the last index
         */
        if (port_info[idx].speed > speed) {
            speed = port_info[idx].speed;
            index = idx;
        }
    }

    return index;
}

/* This function fetches the current egress scheduler's TDM table number being
 * used by the hardware. This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_egr_tdm_table_get(int unit, int pipe,
                                           uint32 *table_num)
{
    soc_reg_t reg;
    uint32 rval;

    /* Return value of CURR_CALf in table_num */

    reg = (pipe == 0) ? ES_PIPE0_TDM_CONFIGr : ES_PIPE1_TDM_CONFIGr;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    *table_num = soc_reg_field_get(unit, reg, rval, CURR_CALf);

    return SOC_E_NONE;
}

/* This function fetches the current ingress scheduler's TDM table number being
 * used by the hardware. This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_ingr_tdm_table_get(int unit, int pgw_inst,
                                            uint32 *table_num)
{
    uint64 rval64;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, pgw_inst, 0,
                                    &rval64));

    /* Return value of LR_TDM_SEL in table_num */
    *table_num = soc_reg64_field32_get(unit, PGW_TDM_CONTROLr, rval64, 
                                       LR_TDM_SELf);

    return SOC_E_NONE;
}

/* This function sets the new egress scheduler's TDM table number which will be
 * used by the hardware. This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_egr_tdm_table_set(int unit, int pipe, 
                                            uint32 table_num)
{
    uint32 rval;
    soc_reg_t reg;

    reg = (pipe == 0) ? ES_PIPE0_TDM_CONFIGr : ES_PIPE1_TDM_CONFIGr;

    SOC_IF_ERROR_RETURN (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    /* Mask unwanted bits */
    table_num &= 0x1;

    /* Set CURR_CALf */
    soc_reg_field_set(unit, reg, &rval, CURR_CALf, table_num);

    SOC_IF_ERROR_RETURN (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/* This function fetches the current ingress scheduler's TDM table number being
 * used by the hardware.  This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_ingr_tdm_table_set(int unit, int pgw_inst,
                                            int table_num)
{
    uint64 rval64;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, pgw_inst, 0,
                                    &rval64));

    /* Mask unwanted bits */
    table_num &= 0x1;

    /* Set LR_TDM_SEL */
    soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &rval64, LR_TDM_SELf,
                          table_num);

    SOC_IF_ERROR_RETURN (soc_reg_set(unit, PGW_TDM_CONTROLr, pgw_inst, 0,
                         rval64));

    return SOC_E_NONE;
}

/*
 * This function checks if cut-through is enabled for a given port.
 * If either unicast or multicast cut-through is enabled, the function
 * returns TRUE else FALSE is returned
 */
STATIC int soc_td2p_is_cut_thru_enabled(int unit, soc_port_t port, int *enable)
{
    uint32 rval;
    uint32 uc_enable, mc_enable;

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port, &rval));

    uc_enable = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, UC_ASF_ENABLEf);
    mc_enable = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, MC_ASF_ENABLEf);

    if (uc_enable || mc_enable) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }

    return SOC_E_NONE;
}

/*
 * This function is used to clear various counters in EP block. The 
 * counters to clear are mentioned in 'Reconfigure EP' section of TD2+ Flexport
 */
STATIC int soc_td2p_ep_counters_clear(int unit, soc_port_resource_t *port_info)
{
    uint64 val64;
    uint32 val = 0;

    COMPILER_64_ZERO(val64);
    SOC_IF_ERROR_RETURN(WRITE_EGR_TRILL_TX_PKTSr(unit, port_info->logical_port,
                        val));
    SOC_IF_ERROR_RETURN(WRITE_EGR_TRILL_TX_ACCESS_PORT_TRILL_PKTS_DISCARDEDr(unit, port_info->logical_port, val));
    SOC_IF_ERROR_RETURN(WRITE_EGR_FCOE_DELIMITER_ERROR_FRAMESr(unit,
                        port_info->logical_port, val));
    SOC_IF_ERROR_RETURN(WRITE_EGR_FCOE_INVALID_CRC_FRAMESr(unit, 
                        port_info->logical_port, val));

    SOC_IF_ERROR_RETURN(WRITE_TPCEr(unit, port_info->logical_port, val64));

    SOC_IF_ERROR_RETURN(WRITE_TDBGC0r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC1r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC2r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC3r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC4r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC5r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC6r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC7r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC8r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC9r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC10r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC11r(unit, port_info->logical_port, val64));

    return SOC_E_NONE;
}

STATIC int soc_td2p_port_resource_tdm_calculate(int unit, 
                                            int curr_port_info_size,
                                            soc_port_resource_t *curr_port_info,
                                            int new_port_info_size,
                                            soc_port_resource_t *new_port_info,
                                            soc_td2p_info_t *si_info)
{
    int rv;
    soc_info_t *si;
    _soc_trident2_tdm_t *tdm;
    tdm_soc_t _chip_pkg;
    tdm_mod_t *_tdm_pkg;
    int phy_port;
    int port;
    int speed_max, index, slot;
    soc_pbmp_t pbmp;

    /* soc info has new values */
    si = &SOC_INFO(unit);

    tdm = soc_td2_td2p_tdm_sched_info_get(unit);

    sal_memset(tdm, 0, sizeof(_soc_trident2_tdm_t));

    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }

        /* Since soc info structure has been updated before this function is
         * called, we use the information provided by it
         */
        phy_port = si->port_l2p_mapping[port];

        tdm->speed[phy_port] = si->port_speed_max[port];
 
        tdm->port_state[phy_port] = SOC_PBMP_MEMBER(si->oversub_pbm, port) ?
                                                    PORT_STATE_OVERSUB : 
                                                    PORT_STATE_LINERATE;

        if (tdm->speed[phy_port] >= 100000) {
           tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 2] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 3] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 4] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 5] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 6] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 7] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 8] = PORT_STATE_SUBPORT;
           tdm->port_state[phy_port + 9] = PORT_STATE_SUBPORT;
        } else if (tdm->speed[phy_port] >= 40000) {
            tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
            tdm->port_state[phy_port + 2] = PORT_STATE_SUBPORT;
            tdm->port_state[phy_port + 3] = PORT_STATE_SUBPORT;
        } else if (tdm->speed[phy_port] >= 20000) {
            tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
        }
    }

    tdm->speed[0] = 1000;
    tdm->speed[129] = 1000;
    tdm->tdm_bw = si->bandwidth / 1000;

    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
    SOC_PBMP_AND(pbmp, si->xpipe_pbm);
    if (tdm->tdm_bw == 720) { /* 720g special handling */
        /* tell tdm code the pipe is oversub only if all ports are oversub */
        SOC_PBMP_PORT_ADD(pbmp, 0); /* cpu port */
        tdm->pipe_ovs_state[0] = SOC_PBMP_EQ(pbmp, si->xpipe_pbm);
    } else {
        /* tell tdm code the pipe is oversub if any ports is oversub */
        tdm->pipe_ovs_state[0] = SOC_PBMP_NOT_NULL(pbmp);
    }
    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
    SOC_PBMP_AND(pbmp, si->ypipe_pbm);
    if (tdm->tdm_bw == 720) { /* 720g special handling */
        /* tell tdm code the pipe is oversub only if all ports are oversub */
        SOC_PBMP_PORT_ADD(pbmp, 105); /* loopback port */
        tdm->pipe_ovs_state[1] = SOC_PBMP_EQ(pbmp, si->ypipe_pbm);
    } else {
        /* tell tdm code the pipe is oversub if any ports is oversub */
        tdm->pipe_ovs_state[1] = SOC_PBMP_NOT_NULL(pbmp);
    }

    tdm->manage_port_type = 0;
    if (SOC_PBMP_NOT_NULL(si->management_pbm)) {
        speed_max = 0;
        for (phy_port = 13; phy_port <= 16; phy_port++) {
            port = si->port_p2l_mapping[phy_port];
            if (port == -1) {
                continue;
            }
            if (speed_max < si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
        }
        if (speed_max <= 1000) {
            tdm->manage_port_type = 1; /* 4 x 1g */
        } else if (speed_max <= 2500) {
            tdm->manage_port_type = 2; /* 4 * 2.5g */
        } else {
            tdm->manage_port_type = 3; /* 1 x 10g */
        }
        if (tdm->manage_port_type > 0) {
            tdm->pgw_tdm[0][0] = 1234; /* magic number used by tdm code */
        }
    }

    for (slot = 0; slot <= _MMU_TDM_LENGTH; slot++) {
        tdm->mmu_tdm[0][slot] = NUM_EXT_PORTS;
        tdm->mmu_tdm[1][slot] = NUM_EXT_PORTS;
    }

    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#1, bullet #1) */
    _chip_pkg.unit = unit;
    _chip_pkg.num_ext_ports = TD2P_NUM_EXT_PORTS;
    _chip_pkg.state=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *), "port state list");
    _chip_pkg.speed=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *), "port speed list");
    for (index=1; index<(_chip_pkg.num_ext_ports); index++) {
        _chip_pkg.state[index] = tdm->port_state[index];
    }
    _chip_pkg.state[0]=1; /* enable cpu port */
    _chip_pkg.state[_chip_pkg.num_ext_ports-1]=1; /* enable loopback port */
    for (index=0; index<(_chip_pkg.num_ext_ports); index++) {
        _chip_pkg.speed[index] = tdm->speed[index];
    }

    /*
     * Map detected core clk frequency to TDM algorithm internal code
     * value.
     */
    switch (si->frequency) {
           case 790:
           case 760:
               _chip_pkg.clk_freq = 760;
               break;
           case 635:
               _chip_pkg.clk_freq = 608;
               break;
           case 537:
               _chip_pkg.clk_freq = 518;
               break;
           case 421:
               _chip_pkg.clk_freq = 415;
               break;
           default:
               _chip_pkg.clk_freq = (si->frequency > 760) ? (760)
                                       : si->frequency;
    }

    _tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&_chip_pkg));
    if (!_tdm_pkg) {
        LOG_CLI((BSL_META_U(unit,
                "Unable to configure TDM, please contact your "
                "Field Applications Engineer or Sales Manager for "
                "additional assistance.\n")));
        return SOC_E_FAIL;
    }

    sal_memcpy(tdm->pgw_tdm[0], _tdm_pkg->_chip_data.cal_0.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[0], _tdm_pkg->_chip_data.cal_0.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[0], _tdm_pkg->_chip_data.cal_0.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_tdm[1], _tdm_pkg->_chip_data.cal_1.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[1], _tdm_pkg->_chip_data.cal_1.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[1], _tdm_pkg->_chip_data.cal_1.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_tdm[2], _tdm_pkg->_chip_data.cal_2.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[2], _tdm_pkg->_chip_data.cal_2.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[2], _tdm_pkg->_chip_data.cal_2.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_tdm[3], _tdm_pkg->_chip_data.cal_3.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[3], _tdm_pkg->_chip_data.cal_3.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[3], _tdm_pkg->_chip_data.cal_3.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->mmu_tdm[0], _tdm_pkg->_chip_data.cal_4.cal_main, sizeof(int)*_MMU_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][0], _tdm_pkg->_chip_data.cal_4.cal_grp[0], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][1], _tdm_pkg->_chip_data.cal_4.cal_grp[1], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][2], _tdm_pkg->_chip_data.cal_4.cal_grp[2], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][3], _tdm_pkg->_chip_data.cal_4.cal_grp[3], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][4], _tdm_pkg->_chip_data.cal_4.cal_grp[4], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][5], _tdm_pkg->_chip_data.cal_4.cal_grp[5], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][6], _tdm_pkg->_chip_data.cal_4.cal_grp[6], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][7], _tdm_pkg->_chip_data.cal_4.cal_grp[7], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_tdm[1], _tdm_pkg->_chip_data.cal_5.cal_main, sizeof(int)*_MMU_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][0], _tdm_pkg->_chip_data.cal_5.cal_grp[0], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][1], _tdm_pkg->_chip_data.cal_5.cal_grp[1], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][2], _tdm_pkg->_chip_data.cal_5.cal_grp[2], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][3], _tdm_pkg->_chip_data.cal_5.cal_grp[3], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][4], _tdm_pkg->_chip_data.cal_5.cal_grp[4], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][5], _tdm_pkg->_chip_data.cal_5.cal_grp[5], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][6], _tdm_pkg->_chip_data.cal_5.cal_grp[6], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][7], _tdm_pkg->_chip_data.cal_5.cal_grp[7], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_free(_chip_pkg.state);
    sal_free(_chip_pkg.speed);
    _tdm_pkg->_chip_exec[TDM_CHIP_EXEC__FREE](_tdm_pkg);

    rv = tdm_td2p_set_iarb_tdm_table(tdm->tdm_bw,
                                     tdm->pipe_ovs_state[0],
                                     tdm->pipe_ovs_state[1],
                                     tdm->manage_port_type == 1,
                                     tdm->manage_port_type == 2,
                                     tdm->manage_port_type == 3,
                                     &tdm->iarb_tdm_wrap_ptr[0],
                                     &tdm->iarb_tdm_wrap_ptr[1],
                                     tdm->iarb_tdm[0],
                                     tdm->iarb_tdm[1]);

    if (rv == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Unable to configure IARB TDM, please contact your "
                            "Field Applications Engineer or Sales Manager for "
                            "additional assistance.\n")));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_resource_tdm_set
 * Purpose:
 *      Reconfigure the ingress and egress schedulers for
 *      the new port configuration.
 *
 *      This includes reconfiguration of:
 *      - OBM Thresholds
 *      - EDB Prebuffer
 *      - Ingress Oversub Scheduler
 *      - Egress Oversub Scheduler
 *      - Line-Rate Scheduler
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      curr_port_info_size  - (IN) Size of current port information array
 *      curr_port_info       - (IN) Per-port array with current flexing info
 *      new_port_info_size   - (IN) Size of new port information array
 *      new_port_info        - (IN) Per-port array with new flexing info
 *      si_info              - (IN) Contains subset of fields from
 *                             soc_info_t structure
 *                             prior to the flexing operation
 *      lossless             - (IN) MMU lossless information
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the 
 *        arrays passed, that is, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_td2p_port_resource_tdm_set(int unit, int curr_port_info_size,
                                   soc_port_resource_t *curr_port_info,
                                   int new_port_info_size,
                                   soc_port_resource_t *new_port_info,
                                   soc_td2p_info_t *si_info,
                                   int lossless)
{
    soc_info_t *si;
    _soc_trident2_tdm_t *tdm;
    int pipe;
    int phy_port;
    int count;
    uint32 table_num = 0;
    uint64 rval64, ctrl_rval64;
    uint32 rval;
    int idx, mmu_port, length, profile_x, profile_y, port;
    int  index, pgw, pgw_master, pgw_slave, obm_inst, base, slot;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_reg_t reg;
    soc_field_t fields[4];
    uint32 values[4];
    soc_pbmp_t pbmp;

    /* Egress scheduler calendar memories */
    static const soc_mem_t mmu_tdm_mems_0[2] ={
        ES_PIPE0_TDM_TABLE_0m, ES_PIPE1_TDM_TABLE_0m
    };

    static const soc_mem_t mmu_tdm_mems_1[2] ={
        ES_PIPE0_TDM_TABLE_1m, ES_PIPE1_TDM_TABLE_1m
    };

    static const soc_mem_t mmu_tdm_regs[2] ={
        ES_PIPE0_TDM_CONFIGr, ES_PIPE1_TDM_CONFIGr
    };

    static const soc_mem_t iarb_tdm_mems[2] ={
        IARB_MAIN_TDM_Xm, IARB_MAIN_TDM_Ym
    };

    static soc_reg_t pgw_tdm_regs[] = {
        PGW_LR_TDM_REG_0r, PGW_LR_TDM_REG_1r,
        PGW_LR_TDM_REG_2r, PGW_LR_TDM_REG_3r,
        PGW_LR_TDM_REG_4r, PGW_LR_TDM_REG_5r,
        PGW_LR_TDM_REG_6r, PGW_LR_TDM_REG_7r,
        PGW_LR_TDM_REG_8r,  PGW_LR_TDM_REG_9r,
        PGW_LR_TDM_REG_10r, PGW_LR_TDM_REG_11r,
        PGW_LR_TDM_REG_12r, PGW_LR_TDM_REG_13r,
        PGW_LR_TDM_REG_14r, PGW_LR_TDM_REG_15r
    };

    /* Used for programming back up calender */
    static soc_reg_t pgw_tdm2_regs[] = {
        PGW_LR_TDM2_REG_0r, PGW_LR_TDM2_REG_1r,
        PGW_LR_TDM2_REG_2r, PGW_LR_TDM2_REG_3r,
        PGW_LR_TDM2_REG_4r, PGW_LR_TDM2_REG_5r,
        PGW_LR_TDM2_REG_6r, PGW_LR_TDM2_REG_7r,
        PGW_LR_TDM2_REG_8r,  PGW_LR_TDM2_REG_9r,
        PGW_LR_TDM2_REG_10r, PGW_LR_TDM2_REG_11r,
        PGW_LR_TDM2_REG_12r, PGW_LR_TDM2_REG_13r,
        PGW_LR_TDM2_REG_14r, PGW_LR_TDM2_REG_15r
    };

    static const soc_field_t pgw_tdm_fields[] = {
        TDM_ENTRY0_PORT_IDf, TDM_ENTRY1_PORT_IDf,
        TDM_ENTRY2_PORT_IDf, TDM_ENTRY3_PORT_IDf,
        TDM_ENTRY4_PORT_IDf, TDM_ENTRY5_PORT_IDf,
        TDM_ENTRY6_PORT_IDf, TDM_ENTRY7_PORT_IDf,
        TDM_ENTRY8_PORT_IDf, TDM_ENTRY9_PORT_IDf,
        TDM_ENTRY10_PORT_IDf, TDM_ENTRY11_PORT_IDf,
        TDM_ENTRY12_PORT_IDf, TDM_ENTRY13_PORT_IDf,
        TDM_ENTRY14_PORT_IDf, TDM_ENTRY15_PORT_IDf,
        TDM_ENTRY16_PORT_IDf, TDM_ENTRY17_PORT_IDf,
        TDM_ENTRY18_PORT_IDf, TDM_ENTRY19_PORT_IDf,
        TDM_ENTRY20_PORT_IDf, TDM_ENTRY21_PORT_IDf,
        TDM_ENTRY22_PORT_IDf, TDM_ENTRY23_PORT_IDf,
        TDM_ENTRY24_PORT_IDf, TDM_ENTRY25_PORT_IDf,
        TDM_ENTRY26_PORT_IDf, TDM_ENTRY27_PORT_IDf,
        TDM_ENTRY28_PORT_IDf, TDM_ENTRY29_PORT_IDf,
        TDM_ENTRY30_PORT_IDf, TDM_ENTRY31_PORT_IDf,
        TDM_ENTRY32_PORT_IDf, TDM_ENTRY33_PORT_IDf,
        TDM_ENTRY34_PORT_IDf, TDM_ENTRY35_PORT_IDf,
        TDM_ENTRY36_PORT_IDf, TDM_ENTRY37_PORT_IDf,
        TDM_ENTRY38_PORT_IDf, TDM_ENTRY39_PORT_IDf,
        TDM_ENTRY40_PORT_IDf, TDM_ENTRY41_PORT_IDf,
        TDM_ENTRY42_PORT_IDf, TDM_ENTRY43_PORT_IDf,
        TDM_ENTRY44_PORT_IDf, TDM_ENTRY45_PORT_IDf,
        TDM_ENTRY46_PORT_IDf, TDM_ENTRY47_PORT_IDf,
        TDM_ENTRY48_PORT_IDf, TDM_ENTRY49_PORT_IDf,
        TDM_ENTRY50_PORT_IDf, TDM_ENTRY51_PORT_IDf,
        TDM_ENTRY52_PORT_IDf, TDM_ENTRY53_PORT_IDf,
        TDM_ENTRY54_PORT_IDf, TDM_ENTRY55_PORT_IDf,
        TDM_ENTRY56_PORT_IDf, TDM_ENTRY57_PORT_IDf,
        TDM_ENTRY58_PORT_IDf, TDM_ENTRY59_PORT_IDf,
        TDM_ENTRY60_PORT_IDf, TDM_ENTRY61_PORT_IDf,
        TDM_ENTRY62_PORT_IDf, TDM_ENTRY63_PORT_IDf
    };

    if (!(SOC_IS_TRIDENT2PLUS(unit))) {
        /* Do nothing */
        return SOC_E_NONE;
    }

    /* Sec 6.6.3.2, #1 and #2 will be filled as part of oversubscription
     * code changes
     */

    /* Reconfigure EDB prebuffer 6.6.3.2 (#3) */

    /* Note both pipelines' registers are written by hardware internally */
    SOC_IF_ERROR_RETURN(READ_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, &rval));
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      CLPORT_CELL_COUNTSf, 0x1);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      XLPORT_CELL_COUNTSf, 0x3);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      ENABLEf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, rval));

    /* Reconfigure EDB prebuffer back up calendar 6.6.3.2 (#4), for line rate
     * TDM calendar. Oversub related portion will be added later
     */
    SOC_IF_ERROR_RETURN(soc_td2p_port_resource_tdm_calculate(unit,
                        curr_port_info_size, curr_port_info,
                        new_port_info_size, new_port_info, si_info));

    /* Reconfigure oversub scheduler (Sections 6.6.3.2.3, 6.6.3.2.4).
     * Function internally determines if oversub is enabled/disabled
     */
    SOC_IF_ERROR_RETURN(soc_td2p_ovr_sub_tdm_update(unit, 
                        curr_port_info_size, curr_port_info,
                        new_port_info_size, new_port_info));

    tdm = soc_td2_td2p_tdm_sched_info_get(unit);

    /* soc info has new values */
    si = &SOC_INFO(unit);

    
    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#1, bullet #2) */
    for (pgw = 0; pgw < _TD2_PGWS_PER_DEV; pgw += _TD2_PGWS_PER_QUAD) {
        pgw_master = pgw;
        pgw_slave  = pgw + 1;

        if ((pgw == 2) || (pgw == 6)) {
            pgw_master = pgw + 1;
            pgw_slave  = pgw;
        }

        /* coverity[negative_returns : FALSE] */
        if (!si->block_valid[PGW_CL_BLOCK(unit, pgw_master)] &&
            !si->block_valid[PGW_CL_BLOCK(unit, pgw_slave)]) {
            /* Both master and slave are not in use */
            continue;
        }

        obm_inst = pgw_master | SOC_REG_ADDR_INSTANCE_MASK;

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, obm_inst, 0,
                                        &ctrl_rval64));

        SOC_IF_ERROR_RETURN(soc_td2p_curr_ingr_tdm_table_get(unit,
                                                            obm_inst,
                                                            &table_num)); 
        /* Configure PGW line rate TDM backup calendar */
        count = 0;
        for (base = 0; base < GET_PGW_TDM_LENGTH(unit);
            base += _PGW_TDM_SLOTS_PER_REG) {

            /* Point to back up calendar */
            if (table_num == 0) {
                reg = pgw_tdm2_regs[base / _PGW_TDM_SLOTS_PER_REG];
            } else {
                reg = pgw_tdm_regs[base / _PGW_TDM_SLOTS_PER_REG];
            }

            COMPILER_64_ZERO(rval64);
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                phy_port = tdm->pgw_tdm[pgw_master / 2][slot];
                if (phy_port == NUM_EXT_PORTS) {
                    break;
                }
                soc_reg64_field32_set(unit, reg, &rval64, pgw_tdm_fields[slot],
                                      phy_port);
                count++;
            }
            if (index != 0) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, obm_inst, 0, rval64));
            }
            if (index != _PGW_TDM_SLOTS_PER_REG) {
                break;
            }
        }

        if (count > 0) {
            if (table_num == 0) {
                soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                      LR_TDM2_WRAP_PTRf, count - 1);
            } else {
                soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                      LR_TDM_WRAP_PTRf, count - 1);
            }
        }

        if (table_num == 0) {
            soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                  LR_TDM2_ENABLEf, count ? 1 : 0);
        } else {
            soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                  LR_TDM_ENABLEf, count ? 1 : 0);
        }

        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, PGW_TDM_CONTROLr, obm_inst, 0, ctrl_rval64));
    }

    /* Note: this loop is for oversub, code not brought in here yet.
     * for (pgw = 0; pgw < _TD2_PGWS_PER_DEV; pgw++)
     */

    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#1, bullet #3) */
    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {

        SOC_IF_ERROR_RETURN(soc_td2p_curr_egr_tdm_table_get(unit, pipe,
                                                            &table_num)); 

        reg = mmu_tdm_regs[pipe];

        /* Program the _other_ (back up) TDM table (not the one in use)*/
        if (table_num == 0) {
            mem = mmu_tdm_mems_1[pipe];
        } else {
            mem = mmu_tdm_mems_0[pipe];
        }

        length = _MMU_TDM_LENGTH;
        for (slot = _MMU_TDM_LENGTH - 1; slot >= 0; slot--) {
            if (tdm->mmu_tdm[pipe][slot] != NUM_EXT_PORTS) {
                length = slot + 1;
                break;
            }
        }
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < length; slot += 2) {
            phy_port = tdm->mmu_tdm[pipe][slot];
            if (phy_port == GET_OVS_TOKEN(unit)) {
                mmu_port = 57; /* oppurtunist port */
            } else if (phy_port == GET_IDL_TOKEN(unit)) {
                mmu_port = 58; /* scheduler will not pick anything */
            } else if (phy_port >= NUM_EXT_PORTS) {
                mmu_port = 0x3f;
            } else {
                /* The BCM layer has updated the p2m mapping before calling
                 * tdm_set, so we use it. Other way may be to find mmu_port
                 * from the subset of new ports in new_port_info.
                 * For other ports, updated p2m mapping may be used 
                 */
                mmu_port = si->port_p2m_mapping[phy_port];
            }

            soc_mem_field32_set(unit, mem, entry, PORT_NUM_EVENf,
                                mmu_port & 0x3F);

            phy_port = tdm->mmu_tdm[pipe][slot + 1];
            if (phy_port == GET_OVS_TOKEN(unit)) {
                mmu_port = 57; /* oppurtunist port */
            } else if (phy_port == GET_IDL_TOKEN(unit)) {
                mmu_port = 58; /* scheduler will not pick anything */
            } else if (phy_port >= NUM_EXT_PORTS) {
                mmu_port = 0x3f;
            } else {
                /* See comment above for EVENf slots */
                mmu_port = si->port_p2m_mapping[phy_port];
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_ODDf,
                                mmu_port & 0x3f);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
        }

        /* We are programming the back up table, so we set it's
         * 'end' field (that is, not the current table's end field)
         */
        if (table_num == 0) {
            soc_reg_field_set(unit, reg, &rval, CAL1_ENDf, slot / 2);

            if (length & 1) {
                soc_reg_field_set(unit, reg, &rval, CAL1_END_SINGLEf, 1);
            }
        } else {
            soc_reg_field_set(unit, reg, &rval, CAL0_ENDf, slot / 2);

            if (length & 1) {
                soc_reg_field_set(unit, reg, &rval, CAL0_END_SINGLEf, 1);
            }
        }

#if 0
        /* This code may not be reqd. In case some other task has set any of
         * the fields below to some other value, we will end up overwriting
         * the field(s)
         */
        
        /* 
         *  Control strict priority scheduling between CPU/Loopback port
         *  and Oversub port. 
         *  Setting - Oversub has highest priority
         */
        soc_reg_field_set(unit, reg, &rval, OPP_STRICT_PRIf, 1);
        soc_reg_field_set(unit, reg, &rval, OPP_PORT_ENf, 1);

        soc_reg_field_set(unit, reg, &rval, OPP_CPULB_ENf, 1);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
#endif /* 0*/
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    /* Configure IARB TDM */
    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        mem = iarb_tdm_mems[pipe];
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < _IARB_TDM_LENGTH; slot++) {
            if (slot > tdm->iarb_tdm_wrap_ptr[pipe]) {
                break;
            }
            soc_mem_field32_set(unit, mem, entry, TDM_SLOTf,
                                tdm->iarb_tdm[pipe][slot]);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot, entry));
        }
    }

    /* Both pipe are expected to have same IARB TDM table length */
    fields[0] = TDM_WRAP_PTRf;
    values[0] = tdm->iarb_tdm_wrap_ptr[0];
    fields[1] = DISABLEf;
    values[1] = 0;
    fields[2] = AUX_CMICM_SLOT_ENf;
    values[2] = 1;
    fields[3] = AUX_EP_LB_SLOT_ENf;
    values[3] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, IARB_TDM_CONTROLr, REG_PORT_ANY, 4,
                                 fields, values));

    
    /* Configure oversubscription port with WAIT_FOR_2ND_MOP */
    /* Configure linerate port with WAIT_FOR_MOP */

    /* 6.6.3.2.2 (#2) */

    /* Find ports with speeds >= 100G in any or both of the pipes */ 
    /* In the new flex config, check if we are adding >= 100G speeds in a pipe,
     * or removing >=100G speeds from a pipe. If we are removing, profile_ will 
     * set to '1'. If we are adding, profile_ will be set to '0' in the code
     * below. If there's no change in the pipe as far as speeds >= 100G is
     * concerned, we will set the same value of profile_ again.
     */

    profile_x = 1; /* For enforcing a minimum of 11 cycle same-port spacing */
    profile_y = 1; /* For enforcing a minimum of 11 cycle same-port spacing */

    for (idx = 0; idx < new_port_info_size; idx++) {
        if (new_port_info[idx].speed >= 100000) {
            /* Set profile_* value for enforcing a minimum of 4 cycle
             * same-port spacing for speeds >= 100G
             */
            if (new_port_info[idx].pipe == 0) {
                /* Port belongs to x-pipe */
                profile_x = 0;
            } else {
                /* Port belongs to y-pipe */
                profile_y = 0;
            }
        }
    }

    /* If there was any port >= 100G configured during a past flexing within a 
     * pipe, then we need to retain the same-port spacing value for 100G (0)
     * Search in the bigger (whole) set of ports
     */
    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        pbmp = pipe ? si->ypipe_pbm : si->xpipe_pbm;
        SOC_PBMP_ITER(pbmp, port) {
            
            if (si->port_l2p_mapping[port] == -1) {
                continue;
            }

            if (si->port_speed_max[port] >= 100000) {
                if (pipe == 0) {
                    profile_x = 0;
                } else {
                    profile_y = 0;
                }
            }
        }
    }

    /* X-pipe */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ES_PIPE0_MIN_SPACINGr,
                        REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, ES_PIPE0_MIN_SPACINGr, &rval, PROFILEf, profile_x);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ES_PIPE0_MIN_SPACINGr,
                                      REG_PORT_ANY, 0, rval));

    /* Y-pipe */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ES_PIPE1_MIN_SPACINGr,
                        REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, ES_PIPE1_MIN_SPACINGr, &rval, PROFILEf, profile_y);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ES_PIPE1_MIN_SPACINGr,
                                      REG_PORT_ANY, 0, rval));

    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#3) */

    for (idx = 0; idx < new_port_info_size; idx++) {

        if (new_port_info[idx].speed <= 21000) {
            int cut_thru_en = FALSE;

            SOC_IF_ERROR_RETURN(soc_td2p_is_cut_thru_enabled(unit, port,
                                &cut_thru_en));

            if (cut_thru_en == TRUE) {
                /* Disable cut-through for this port */
                SOC_IF_ERROR_RETURN(soc_td2p_cut_thru_enable_disable(unit,
                                    &new_port_info[idx], 0));
            }
        }
    }

    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#4, #5) */
    /* Note we are providing 80Usec delay only once, not twice as mentioned in
     * the Uarch spec
     */
    sal_usleep(80);
    idx = 0;

    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        int masters[_TD2_PGWS_PER_DEV / 2] = {0, 3, 4, 7}; /* pgw masters */
        int masters_per_pipe = (sizeof(masters) / sizeof(int)) /
                                _TD2_PIPES_PER_DEV;
         int pgw_num;

        /* Switch over egress calendar */
        SOC_IF_ERROR_RETURN(soc_td2p_curr_egr_tdm_table_get(unit, pipe,
                                                            &table_num));
        table_num ^= 0x1;
        
        SOC_IF_ERROR_RETURN(soc_td2p_curr_egr_tdm_table_set(unit, pipe,
                                                            table_num));

        for (pgw_num = 0; pgw_num  < masters_per_pipe; pgw_num++, idx++) { 
            obm_inst = masters[idx] | SOC_REG_ADDR_INSTANCE_MASK;

            /* Switch over ingress calendar */

            SOC_IF_ERROR_RETURN(soc_td2p_curr_ingr_tdm_table_get(unit,
                                obm_inst, &table_num));
            table_num ^= 0x1;

            SOC_IF_ERROR_RETURN(soc_td2p_curr_ingr_tdm_table_set(unit,
                                obm_inst, table_num));
            }
    }

    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#6) */
    for (idx = 0; idx < new_port_info_size; idx++) {

        if (new_port_info[idx].speed <= 21000) {
            int cut_thru_en = FALSE;

            SOC_IF_ERROR_RETURN(soc_td2p_is_cut_thru_enabled(unit, port,
                                &cut_thru_en));

            if (cut_thru_en == FALSE) {
                /* Enable cut-through for this port */
                SOC_IF_ERROR_RETURN(soc_td2p_cut_thru_enable_disable(unit,
                                    &new_port_info[idx], 1));
            }
        }
    }

    /* Reconfigure OBM thresholds (Section 6.6.3.2, step #5) */
    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_set(unit,
                        curr_port_info_size, curr_port_info,
                        new_port_info_size, new_port_info,
                        lossless));

    /* Reconfigure EDB prebuffer 6.6.3.2 (#6) */

    sal_usleep(1);

    /* Note both pipelines' registers are written by hardware internally */
    SOC_IF_ERROR_RETURN(READ_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, &rval));
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      CLPORT_CELL_COUNTSf, 0x1);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      XLPORT_CELL_COUNTSf, 0x3);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      ENABLEf, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, rval));


    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_pgw_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *      This function will do two things,
 *      1. clear all registers for old ports which exists in pre-Flex status
 *      2. initialize all registers for new ports which exists in post-Flex
 *         status
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      pre_num_res  - (IN) Number of resource data which exist in pre-Flex
 *                          status
 *      post_num_res - (IN) Number of resource data which exist in post-Flex
 *                          status
 *      pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *      post_res     - (IN) Resource data structure pointer in post-Flex status
 *      lossless     - (IN) MMU lossless information
 *
 * Returns:
 *      SOC_E_XXX
 * Note: 
 *      - Assumes FlexPort cases are valid.
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int
soc_td2p_port_resource_pgw_set(int unit,
                      int pre_num_res,  soc_port_resource_t *pre_res,
                      int post_num_res, soc_port_resource_t *post_res,
                      int lossless)
{
    soc_esw_portctrl_pgw_t data;
    int i;

    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, post_num_res, post_res));

    for (i = 0; i < post_num_res; i++) {
        data.mode       = post_res[i].mode;
        data.lanes      = post_res[i].num_lanes;
        data.port_index = post_res[i].lane_info[0]->port_index;
        data.flags      = 0x0;

        SOC_IF_ERROR_RETURN(soc_esw_portctrl_pgw_reconfigure(unit,
                            post_res[i].logical_port, &data));
    }

    SOC_IF_ERROR_RETURN(soc_td2p_pgw_port_hw_default_set(unit,
                                                         pre_num_res, pre_res,
                                                         lossless));
    SOC_IF_ERROR_RETURN(soc_td2p_pgw_port_sw_default_set(unit,
                                                         post_num_res, post_res,
                                                         lossless));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_ip_set
 * Purpose:
 *      Reconfigure IP.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      curr_port_info_size - (IN) Size of current port information array
 *      curr_port_info - (IN) Per-port array with current flexing info
 *      new_port_info_size - (IN) Size of new port information array
 *      new_port_info - (IN) Per-port array with new flexing info
 *      si_info - (IN) Contains subset of fields from soc_info_t structure
 *                     prior to the flexing operation
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the 
 *        arrays passed, example, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_td2p_port_resource_ip_set(int unit, int curr_port_info_size,
                                  soc_port_resource_t *curr_port_info,
                                  int new_port_info_size,
                                  soc_port_resource_t *new_port_info,
                                  soc_td2p_info_t *si_info)
{
    soc_info_t *si;
    soc_mem_t mem;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_phy_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        sal_memset(&entry, 0, sizeof(entry));

        /* The p2l mapping has been updated before this function is called */
        port = si->port_p2l_mapping[phy_port];

        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x7F : port);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port,
                                          &entry));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_ep_set
 * Purpose:
 *      Reconfigure EP.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      curr_port_info_size - (IN) Size of current port information array
 *      curr_port_info - (IN) Per-port array with current flexing info
 *      new_port_info_size - (IN) Size of new port information array
 *      new_port_info - (IN) Per-port array with new flexing info
 *      si_info - (IN) Contains subset of fields from soc_info_t structure
 *                     prior to the flexing operation
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the 
 *        arrays passed, example, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_td2p_port_resource_ep_set(int unit, int curr_port_info_size,
                                  soc_port_resource_t *curr_port_info,
                                  int new_port_info_size,
                                  soc_port_resource_t *new_port_info,
                                  soc_td2p_info_t *si_info)
{
    soc_info_t *si;
    uint32 rval;
    int num_port, port, idx, phy_port;

    si = &SOC_INFO(unit);

    /* Ingress logical to physical port mapping */
    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;

    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        /*
         * Use logical to physical number mapping from soc info structure
         * since it has been updated before this function is called
         */
        phy_port = si->port_l2p_mapping[port];

        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0xFF : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }

    /* Clear counters for the newly added ports */
    for (idx = 0; idx < new_port_info_size; idx++) {
        SOC_IF_ERROR_RETURN(soc_td2p_ep_counters_clear(unit,
                                                       &new_port_info[idx]));
    }

    return SOC_E_NONE;
}

/*
 * Function: 
 *      soc_td2p_port_resource_mmu_mapping_set
 * Purpose:
 *      Set the HW MMU Port mappings.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      nport     - (IN) Number of elements in array resource.
 *      resource  - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes FlexPort cases are valid.
 *      Assumes SOC INFO data structure has been updated.
 */
int
soc_td2p_port_resource_mmu_mapping_set(int unit, int nport,
                                       soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;
    uint32 rval;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        if (pr->physical_port == -1) {
            logic_port = 0x7F;
            phy_port = 0xFF;
        } else {
            logic_port = pr->logical_port;
            phy_port = pr->physical_port;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
                          PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, logic_port,
                           0, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, &rval,
                          LOGIC_PORTf, logic_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, logic_port,
                           0, rval));

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "MMU port mappings: "
                                "mmu=%d logical=%d physical=%d\n"),
                     si->port_p2m_mapping
                     [si->port_l2p_mapping[pr->logical_port]],
                     logic_port, phy_port));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_resource_mmu_set
 * Purpose:
 *      Called to reconfigure the MMU hardware.
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN) An array of port changes that are being done
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 * Note:
 *      Assumes FlexPort cases are valid.
 *      Assumes SOC INFO data structure has been updated.
 */
int
soc_td2p_port_resource_mmu_set(int unit, int nport,
                               soc_port_resource_t *flexport_data)
{
    int i;

    /* Update MMU Port Mappings */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_mmu_mapping_set(unit, nport,
                                                flexport_data));

    /* Configure all MMU registers for new ports */
    for (i = 0; i < nport; i++) {
        if (flexport_data[i].physical_port != -1) {
            SOC_IF_ERROR_RETURN(
            soc_td2p_mmu_port_resource_set(unit, flexport_data[i].logical_port,
                                           flexport_data[i].speed));
        }
    }

    return SOC_E_NONE;
}


/* Tells if a port macro (indexed by TSC4, not TSC12) is flex port enabled.
 *  
 * Port macro flex on or off is determined by the first port in that macro. 
 * For TSC12 it's the first port in the first TSC4. For a TSC4 that is NOT 
 * part of a TSC12, it's also the first port of that port macro.  For a TSC4 
 * that is a part of a TSC12, refer to the first port of the first port macro 
 * for that TSC12. 
 *  
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port_macro    - (IN) port macro desired
 *      flex_enabled  - (OUT) whether the port macro is enabled or disabled
 *  
 *  Returns: 
 *      SOC_E_xxx
 */
int
soc_td2p_port_macro_flex_enabled(int unit, int port_macro, int * flex_enabled)
{
    int port_num = -1;

    if ((port_macro < 0) || (port_macro > 31)) {
        return SOC_E_PARAM;
    }

    port_num = soc_td2p_port_macro_to_port_mapping[port_macro];

    *flex_enabled = soc_td2p_is_port_flex_enable(unit, port_num);

    return SOC_E_NONE;
}

/* Function to reserve cells for CutThru(CT) and Flex ports
 * Parameters:
 *      unit          - (IN) Unit number.
 *      pipe          - (IN) Pipe number
 *      flex          - (IN) Config has Flex ports or Not
 *      rsvd_buffer   - (OUT) Buffers reserved per resource (Ing/Egr/ASF)
 * Returns:
 *      SOC_E_xxx
 */
int
soc_td2p_mmu_additional_buffer_reserve(int unit, int pipe, int flex,
                                     _soc_mmu_rsvd_buffer_t *rsvd_buffer)
{
    soc_info_t *si;
    int pg_min_cells = 0, qgrp_min_cells = 0, hdrm_cells = 0, asf_cells = 0;
    int total_reserve = 0;
    int lossless = 0;

    int i, j, start_port, logical_port, start_pm, end_pm;
    int pm_flex_enabled = 0;

    #define TD2P_NUM_TSC4_PER_PIPE 16
    #define TD2P_NUM_PORTS_PER_TSC4 4

    COMPILER_REFERENCE(si);
    si = &SOC_INFO(unit);

    if ((pipe >= NUM_PIPE(unit)) || (!rsvd_buffer)) {
        return SOC_E_PARAM;
    }

    /* returning if flex is true.  flex true indicates the legacy flexport ':i' case is
       enabled */
    if (flex) {
        return SOC_E_NONE;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    /* if we're on pipe 1, start at the 2nd half of the macros.  Otherwise,
       start at the 1st half */
    if(pipe) {
        start_pm = TD2P_NUM_TSC4_PER_PIPE;
        end_pm = 2*TD2P_NUM_TSC4_PER_PIPE;
    } else {
        start_pm = 0;
        end_pm = TD2P_NUM_TSC4_PER_PIPE;
    }
    
    /* Per pipe, calculate the memory required on a per port macro (TSC4) basis */    
    for (i = start_pm; i < end_pm; i++) {
        SOC_IF_ERROR_RETURN(
            soc_td2p_port_macro_flex_enabled(unit, i, &pm_flex_enabled));

        /* if this port macro is flex enabled, allocate memory for it accordingly */
        if (pm_flex_enabled == 1) {
            /* Reserved Buffer for Flex Port Macro when Flex Port is enabled in
               device */
            if (lossless) {
                hdrm_cells      = (_TD2P_RSVD_BUF_FLEX_HDRM_LOSSLESS 
                                   * TD2P_NUM_PORTS_PER_TSC4);

                pg_min_cells    = (_TD2P_RSVD_BUF_FLEX_PGMIN_LOSSLESS 
                                   * TD2P_NUM_PORTS_PER_TSC4);

                qgrp_min_cells  = (_TD2P_RSVD_BUF_FLEX_QGRP 
                                   * TD2P_NUM_PORTS_PER_TSC4);

                asf_cells       = (_TD2P_RSVD_BUF_FLEX_ASF
                                   * TD2P_NUM_PORTS_PER_TSC4);
            } else {
                hdrm_cells      = 0;
                pg_min_cells    = 0;
                qgrp_min_cells  = (_TD2P_RSVD_BUF_FLEX_QGRP 
                                   * TD2P_NUM_PORTS_PER_TSC4);

                asf_cells       = (_TD2P_RSVD_BUF_FLEX_ASF
                                   * TD2P_NUM_PORTS_PER_TSC4);
            }

        } else {
            /*For port modules that are in non-flex mode, buffer is reserved 
              per port (not port macro).
             
              Here we are calculating the buffer requirements for this TSC4 on
              a per port basis. */
            hdrm_cells = 0;
            pg_min_cells = 0;
            qgrp_min_cells = 0;
            asf_cells = 0;

            start_port = i * TD2P_NUM_PORTS_PER_TSC4;
            for (j = start_port; j < start_port + TD2P_NUM_PORTS_PER_TSC4; j++) {

                /* figure out from the logical port what the speed of this port is */
                logical_port = si->port_p2l_mapping[j];

                if (si->port_speed_max[logical_port] >= 100000) {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_100G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    }
                } else if (si->port_speed_max[logical_port] >= 40000) {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_40G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    }
                } else if (si->port_speed_max[logical_port] >= 20000) {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_20G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    }
                } else {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_10G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_10G_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_10G_ASF;
                    }
                }
            }
        }

        /* Add what was calculated for the port macro to the total */
        rsvd_buffer->ing_rsvd_cells += (hdrm_cells + pg_min_cells + qgrp_min_cells);
        rsvd_buffer->egr_rsvd_cells += qgrp_min_cells;
        rsvd_buffer->asf_rsvd_cells += asf_cells;
    }

    total_reserve = rsvd_buffer->ing_rsvd_cells;
    total_reserve += rsvd_buffer->egr_rsvd_cells;
    total_reserve += rsvd_buffer->asf_rsvd_cells;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Cells rsvd for Pipe %d,"
                            " Flex/ASF per pipe: %d, Ing: %d,"
                            " Egr: %d, ASF: %d\n"),
                            pipe, total_reserve, rsvd_buffer->ing_rsvd_cells,
                            rsvd_buffer->egr_rsvd_cells,
                            rsvd_buffer->asf_rsvd_cells));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_mmu_flexport_map_validate
 * Purpose:
 *      Validate and allocate/deallocate MMU port assignments.
 *      New MMU port assignments are returned in argument.
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN/OUT) An array of port changes that are being done.
 *                       Returns MMU ports assignment.
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 */
int
soc_td2p_mmu_flexport_map_validate(int unit, int nport, 
                                   soc_port_resource_t * flexport_data)
{
    return soc_td2p_mmu_flexport_allocate_deallocate_ports(unit, nport, 
                                                           flexport_data, 0);
}

/*
 * Function:
 *      soc_td2p_mmu_flexport_map_set
 * Purpose:
 *      Apply the port allocation/deallocation parameters settings
 *      and update SOC INFO data structure.
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN) An array of port changes that are being done
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 */
int
soc_td2p_mmu_flexport_map_set(int unit, int nport, 
                              soc_port_resource_t * flexport_data)
{
    return soc_td2p_mmu_flexport_allocate_deallocate_ports(unit, nport, 
                                                           flexport_data, 1);
}

/*
 * Function:
 *      soc_td2p_mmu_flexport_allocate_deallocate_ports
 * Purpose:
 *      Allocate or deallocate ports in software.  This function frees up
 *      mmu ports or allocates them in software.  When apply is 0,
 *      flexport_data[x].mmu_port needs to be filled in.  When apply is 1,
 *      flexport_data[x].mmu_port is valid and needs to be assigned in the
 *      port_p2m_mapping[].
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN/OUT) An array of port changes that are being done
 *      apply          - (IN) 0 if only validating, 1 if settings are
 *                            to be written to SOC info structure.
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 */
int
soc_td2p_mmu_flexport_allocate_deallocate_ports(int unit, int nport, 
                             soc_port_resource_t * flexport_data, int apply)
{
    int i, j;
    int phy_port;
    int mmu_port;
    int pipe;
    int start_mmu_port;
    int end_mmu_port;
    int port_max_speed = 0;
    int mmu_ports_used[TD2P_PIPES_PER_DEV * _TD2_PORTS_PER_PIPE];

    soc_info_t *si = &SOC_INFO(unit);

    sal_memset(mmu_ports_used, 0, sizeof(int) * COUNTOF(mmu_ports_used));

    /* Iterate through all the MMU ports and mark the used mmu ports */
    for (j = 0; j < TD2P_PIPES_PER_DEV * _TD2_PORTS_PER_PIPE; j++) {

        mmu_port = si->port_p2m_mapping[j];

        /* if the mmu port is assigned, mark it as used */
        if(mmu_port != -1) {
            mmu_ports_used[mmu_port] = 1;
        }
    }

    /* Iterate through the array to deallocate and allocate ports */
    for (i = 0; i < nport; i++) {

        if ((flexport_data[i].flags & SOC_PORT_RESOURCE_I_MAP) ||
            (flexport_data[i].physical_port == -1)) {

            /*
             * Get the current physical port and MMU Port associated
             * with the logical port passed in */
            phy_port = si->port_l2p_mapping[flexport_data[i].logical_port];
            if (phy_port == -1) { 
                LOG_VERBOSE(BSL_LS_SOC_MMU,
                    (BSL_META_U(unit,
                                "MMU port deallocation failure: logical "
                                "port %d is not mapped to a physical port\n"), 
                                flexport_data[i].logical_port));

                return SOC_E_PARAM;
            }

            mmu_port = si->port_p2m_mapping[phy_port];

            /* Use current MMU port assignments for inactive mapping */
            if (flexport_data[i].flags & SOC_PORT_RESOURCE_I_MAP) {
                if (mmu_port == -1) { 
                    LOG_ERROR(BSL_LS_SOC_MMU,
                              (BSL_META_U(unit,
                                          "Invalid MMU port on inactive "
                                          "port configuration: "
                                          "port=%d mmu=%d\n"),
                               flexport_data[i].logical_port, mmu_port));
                    return SOC_E_INTERNAL;
                }

                flexport_data[i].mmu_port = mmu_port;
                LOG_VERBOSE(BSL_LS_SOC_MMU,
                            (BSL_META_U(unit,
                                        "Inactive port configuration, "
                                        "keep same mapping: port=%d mmu=%d\n"),
                             flexport_data[i].logical_port, mmu_port));
                continue;
            }

            /*
             * Continue with next case.
             * If the phy_port == -1, we're deallocating if we're told to
             */

            /* Check to make sure we aren't indexing into mmu_ports_used with
               a negative mmu port number. Skip this if p2m returns negative. */
            if (mmu_port < 0) {
                continue;
            }

            /* Mark the MMU port unused so that it can be allocated again
               in this routine */
            mmu_ports_used[si->port_p2m_mapping[phy_port]] = 0;

            /* Only apply these settings if we're told to */
            if (apply == 1) {
                /* deallocate the mmu port mapping for this port */
                si->port_p2m_mapping[phy_port] = -1;
            }
        } else {
            /* otherwise we're allocating a port.
               1. Go through all the phy to mmu port mappings and mark the mmu
               ports in use
               2. Find out if we need to allocate a HSP given the port speed
               and the clock frequency
               3. Assign a free MMU port appropriate to the pipe the physical
               port is on.  A phy port on X
                  pipe must have an MMU port on X pipe, Y on Y.  */


            /* If this is an apply, we're just mapping the settings in the
               passed in ports.  We are not iterating through to find a
               free MMU port. */
            if (apply == 1) {
                /* map the mmu port number to the physical port */
                si->port_p2m_mapping[flexport_data[i].physical_port] = 
                    flexport_data[i].mmu_port;
            } else {

                /* This is a validate, so we're looking for free MMU ports to
                   assign */

                /* 1. Find the first unused mmu port and map it to our phy
                   port.  If we aren't allocating a HSP, skip MMU Ports 0-15
                   (start at 16). If we are, start at 0 and end at 15.
                 
                   We use SOC_TD2_NUM_MMU_PORTS_PER_PIPE instead of
                   _TD2_PORTS_PER_PIPE because
                   the last 11 MMU ports in each pipe are not used.
                 
                   **** THIS IS PER PIPE!!! ****
                 
                   */
                if (si->bandwidth >= 960000) {
                    if(flexport_data[i].speed >= 100000) {
                        start_mmu_port = 0;
                        end_mmu_port = SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE;
                    } else {
                        start_mmu_port = SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE;
                        /* 53 MMU ports per pipe, MMU port 52 is CPU port,
                           MMU port 116 is Loopback port*/
                        end_mmu_port = SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1;
                    }
                } else if (si->bandwidth >= 480000) {
                    /* both 480G and 720G SKU need a HSP at 40Gb and above */
                    if (flexport_data[i].speed >= 40000) {
                        start_mmu_port = 0;
                        end_mmu_port = SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE;
                    } else {
                        start_mmu_port = SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE;
                        /* 53 MMU ports per pipe, MMU port 52 is CPU port,
                           MMU port 116 is Loopback port */
                        end_mmu_port = SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1;
                    }
                } else {
                    LOG_VERBOSE(BSL_LS_SOC_MMU,
                        (BSL_META_U(unit,
                                    "MMU port allocation failure: "
                                    "SKU %d is not supported\n"), 
                                    si->bandwidth));

                    return SOC_E_INTERNAL;
                }

                /* 2. check pipe for a free mmu port */
                pipe = SOC_PBMP_MEMBER(si->ypipe_pbm,
                                       flexport_data[i].logical_port) ? 1 : 0;

                /* Add the necessary offset for the pipe we're in */
                start_mmu_port += (pipe * _TD2_PORTS_PER_PIPE);
                end_mmu_port += (pipe * _TD2_PORTS_PER_PIPE);

                /* loop from start mmu port to end mmu port to find
                   a free mmu port
                   0-51 is range for X Pipe
                   52 is CPU port
                   53-63 are NOT used
                   64-115 is range for Y Pipe
                   116 is Loopback port */
                for(j = start_mmu_port; j < end_mmu_port; j++) {
                    /* Case where SKU == 720G and
                       40 Gig <= max flexed speed < 100 Gig doesn't
                       require T2OQ to be reserved, so we
                       can use mmu ports 36 - 51 in that case,
                       but MUST skip for all others */
                    SOC_IF_ERROR_RETURN
                        (soc_td2p_port_resource_speed_max_get(unit,
                                                              &port_max_speed));
                    if (!((si->bandwidth == 720000) && 
                          (port_max_speed >= 40000) &&
                          (port_max_speed < 100000))){
                        if ((j >= 36 + (pipe * _TD2_PORTS_PER_PIPE)) &&
                            j <= (51 + (pipe * _TD2_PORTS_PER_PIPE))) {
                            continue;
                        }
                    }

                    /* We found an unused MMU port! Let's assign it to
                       a phy port! */
                    if (mmu_ports_used[j] == 0) {
                        /* Mark the port as used. This is done in case
                           we're passed multiple ports
                           to allocate to make sure we have enough ports. */
                        mmu_ports_used[j] = 1;
                        flexport_data[i].mmu_port = j;

                        break;
                    }
                }

                /* if we didn't find a free mmu port, return error. */
                if (j == end_mmu_port) {
                    return SOC_E_RESOURCE;
                }
            }
        }
    }
    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_port_asf_set
* Purpose:
*     Enable/Disable CT/ASF sequence
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
*     speed   - (IN) Speed.
*     enable  - (IN) Enable or Disable.
* Returns:
*     SOC_E_XXX
* */
int
soc_td2p_port_asf_set (int unit, soc_port_t port, int speed, int enable)
{
    uint32 rval;
    int phy_port;
    uint32 asf_speed_mode;
    egr_edb_xmit_ctrl_entry_t entry;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_PORT;
    }

    /*Stop enquing any packets into the cut-through FIFO */
    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port, &rval));
    asf_speed_mode = soc_reg_field_get(unit, ASF_PORT_CFGr, rval,
            ASF_PORT_SPEEDf);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval,
            ASF_PORT_SPEEDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));

    if (!enable) {
        /*Wait for 8ms for all the data drained from the cut-through FIFO*/
        sal_usleep(8000);
    }

    /*Configure the EP block*/
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_EDB_XMIT_CTRLm,
                MEM_BLOCK_ALL, phy_port, &entry));
    if (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port)) {
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, &entry,
                WAIT_FOR_2ND_MOPf,  enable ? 0 : 1);
    } else {
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, &entry,
                WAIT_FOR_MOPf,  enable ? 0: 1);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_EDB_XMIT_CTRLm,
                MEM_BLOCK_ALL, phy_port, &entry));

    /*Set/clear the cut-through enable bit*/
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval,
            MC_ASF_ENABLEf, enable ? 1 : 0);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval,
            UC_ASF_ENABLEf, enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));

    if (enable) {
        /*Wait for 1us for UC_ASF_ENABLE/MC_ASF_ENABLE change takes effect*/
        sal_usleep(1);
    }

    /*Restore ASF_PORT_CFG.ASF_PORT_SPEED to the original value*/
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval,
            ASF_PORT_SPEEDf, asf_speed_mode);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));

    return SOC_E_NONE;

}

/*
* Function:
*     soc_td2p_port_icc_width_set
* Purpose:
*     Program PORT_INITIAL_COPY_COUNT_WIDTHr
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
* Returns:
*     SOC_E_XXX
* */
STATIC int
soc_td2p_port_icc_width_set(int unit, soc_port_t port)
{
    int num_lanes;
    int count_width = 0;

    if (SOC_REG_IS_VALID(unit, PORT_INITIAL_COPY_COUNT_WIDTHr)) {
        num_lanes = SOC_INFO(unit).port_num_lanes[port];
        switch (num_lanes) {
            case 1: count_width = 1;
                    break;
            case 2: count_width = 2;
                    break;
            case 4: count_width = 3;
                    break;
            default: count_width = 0;
                     break;
        }
        if (SOC_REG_FIELD_VALID(unit, PORT_INITIAL_COPY_COUNT_WIDTHr, 
                    BIT_WIDTHf)) {
            SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, PORT_INITIAL_COPY_COUNT_WIDTHr,
                        port, BIT_WIDTHf, count_width ? (count_width - 1) : 0));                    
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_td2p_repl_port_agg_map_init
 * Purpose:
 *     Initialize L3MC Port Agg Map.
 * Parameters:
 *     unit    - (IN) Unit number.
 *     port    - (IN) Logical SOC port number.
 * Returns:
 *     SOC_E_xxx
 * */
STATIC int
soc_td2p_repl_port_agg_map_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    int phy_port, mmu_port;
    uint32 regval;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    /* configure mmu port to repl aggregateId map */
    regval = 0;
    soc_reg_field_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, &regval,
            L3MC_PORT_AGG_IDf, mmu_port % 64);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr,
                port,0,regval));

    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_ipmc_repl_init
* Purpose:
*     Initialize IPMC Replication settings
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
* Returns:
*     SOC_E_XXX
* */
STATIC int
soc_td2p_ipmc_repl_init(int unit, soc_port_t port)
{
    /* Program MMU_TOQ_REPL_PORT_AGG_MAP
     * */
    SOC_IF_ERROR_RETURN(soc_td2p_repl_port_agg_map_init(unit, port));

    /* Clear replication group table */
    if (soc_mem_is_valid(unit, MMU_REPL_GROUP_INFO0m) &&
            soc_mem_is_valid(unit, MMU_REPL_GROUP_INFO1m)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_REPL_GROUP_INFO0m, MEM_BLOCK_ALL,
                           0));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_REPL_GROUP_INFO1m, MEM_BLOCK_ALL,
                           0));
    } 

    /* Clear replication group initial count table */
    if (soc_mem_is_valid(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNTm)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_REPL_GROUP_INITIAL_COPY_COUNTm,
                           MEM_BLOCK_ALL, 0));
    }

    return SOC_E_NONE;
}

/*
* Function:
* soc_td2p_mmu_port_resource_set
* Purpose:
* Reconfigure MMU for flexport operation
* Parameters:
* unit    - (IN) Unit number.
* port    - (IN) Logical SOC port number.
* reset   - (IN) Reset.
* Returns:
* SOC_E_XXX
* */
int soc_td2p_mmu_port_resource_set(int unit, soc_port_t port, int speed)
{

    /* Flex port operation not allowed on CPU or loopback port
     * */
     if (IS_CPU_PORT(unit, port) && IS_LB_PORT(unit, port)) {
         return SOC_E_PARAM;
     }

    /* Configure all MMU registers for new ports whose setting is
     * based on port speed.
     * */
    SOC_IF_ERROR_RETURN(soc_trident2_port_speed_update(unit, port, speed));

    /* Program PORT_INITIAL_COPY_COUNT_WIDTH
     * */
    SOC_IF_ERROR_RETURN(soc_td2p_port_icc_width_set(unit, port));

    /* Configure Scheduling on the port
     * */
    SOC_IF_ERROR_RETURN(soc_trident2_port_sched_set(unit, port));

    /* Reconfigure IPMC replication
     * */
    SOC_IF_ERROR_RETURN(soc_td2p_ipmc_repl_init(unit, port));

    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_is_any_port_flex_enable
* Purpose:
*     Find out if
*     port_flex_enable=1 OR
*     port_flex_enable{physical port num}=1
*     config is present in config.bcm
* Parameters:
*     unit    - (IN) Unit number.
* Returns:
*     TRUE or FALSE
*     If such a config is present, this function
*     returns 1 - implying that flex is enable on
*     at least 1 port (port macro) in the system
*     This function returns 0, if flex is not enabled
*     on any port (port macro) in the system
* */
int soc_td2p_is_any_port_flex_enable(int unit)
{
    int phy_port, num_phy_port;

    num_phy_port = 130;

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        if (soc_property_phys_port_get(unit,
                                       phy_port,
                                       spn_PORT_FLEX_ENABLE, 0)) {
            return TRUE;
        }
    }

    return FALSE;
}

/*
* Function:
*     soc_td2p_is_port_flex_enable
* Purpose:
*     Find out if port_flex_enable{physical port num} config
*     is present on the given physical port in config.bcm
* Parameters:
*     unit    - (IN) Unit number.
*     phy_port - (IN) physical port num
* Returns:
*     TRUE or FALSE
* */
int soc_td2p_is_port_flex_enable(int unit, int phy_port)
{
    int enable = 0;

    /* This property does not apply to cpu port &
     * loopback port
     * */

    if (phy_port > 0 && phy_port < 129) {
        enable = soc_property_phys_port_get(unit,
                                            phy_port,
                                            spn_PORT_FLEX_ENABLE, 0);
    }

    return enable;
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT */
