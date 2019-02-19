/*
 * $Id:$
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
#include <soc/td2_td2p.h>
#include <bcm_int/esw/trident2plus.h>

#define IS_OVERSUB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))

#define BCM_TD2P_MAX_COE_MODULES 5

#define BCM_TD2P_MAX_BANK_SIZE      128
#define BCM_TD2P_SMALL_BANK_SIZE    64


#define _TD2P_PROB_DROP_SOP_XOFF          80
#define _TD2P_PROB_DROP_SOP_XON_ABOVE_THR 65
#define _TD2P_PROB_DROP_SOP_XON_BELOW_THR 0

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r
};

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

int soc_td2p_set_obm_registers (int unit, soc_reg_t reg, int speed, int index, int obm_inst, 
        int lossless )
{
    int xon, xoff;
    uint64 ctrl_rval64;
    int maxt, mint = 0;
    int lowprit;
    int div;


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
                                         int obm_inst)  
                                         
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
    /* By default enable Probabilistic dropping */
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, EN_DROP_PROBf, 1);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XOFFf, 
                          _TD2P_PROB_DROP_SOP_XOFF);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_ABOVE_THRf,
                          _TD2P_PROB_DROP_SOP_XON_ABOVE_THR);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_BELOW_THRf,
                          _TD2P_PROB_DROP_SOP_XON_BELOW_THR);
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

            /* SNAT */
            memState[ING_SNATm].index_max = -1;
            memState[ING_SNAT_DATA_ONLYm].index_max = -1;
            memState[ING_SNAT_HIT_ONLYm].index_max = -1;
            memState[ING_SNAT_HIT_ONLY_Xm].index_max = -1;
            memState[ING_SNAT_HIT_ONLY_Ym].index_max = -1;
            memState[ING_SNAT_ONLYm].index_max = -1;
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
 *      soc_td2p_edb_buf_reset
 * Purpose:
 *      Reset EDB port buffer.
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

    phy_port = si->port_l2p_mapping[port];

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
            if (!level) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "EDBBufferDrainTimeOut:port %d,%s, "
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

        /*
         * Release EDB port buffer reset and enable cell request
         * generation in EP.
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

#endif /* BCM_TRIDENT2PLUS_SUPPORT */
