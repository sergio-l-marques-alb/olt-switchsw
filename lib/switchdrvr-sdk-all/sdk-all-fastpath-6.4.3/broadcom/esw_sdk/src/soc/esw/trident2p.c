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
    char *str, *str_end;
    int tmp = 0;

    if (SAL_BOOT_QUICKTURN) {
        if ((str = soc_property_get_str(unit, "half_chip")) != NULL) {
            tmp = sal_ctoi(str, &str_end);
            if (str != str_end) {
                if ( tmp == 1 ) {
                    full_chip = 0;
                }
            } 
        }
    }

    return full_chip;
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
        xoff = 0x2C3;
        div = 1;
    } else if (speed > 10000) {
        xoff = 0x13B;
        div = 2;
    } else {
        xoff = 0x4F;
        div = 4;
    }

    maxt = GET_NUM_CELLS_PER_OBM(unit) / div;

    if ( !lossless ) {
        xon = xoff = 0x7FF;
        lowprit = maxt;
    } else {
        xon     = xoff - 0x20;
        lowprit = xoff - 0x10;
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
    int         index_max;
    int         ipmc_tbl [] = { L3_IPMC_1m, L3_IPMC_REMAPm, L3_IPMCm, 
                                EGR_IPMCm, EGR_IPMC_CFG2m };
    int         ipmc_tindex;
            
    
    sop = SOC_PERSIST(unit);

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
        sop->memState[ ipmc_tbl[ ipmc_tindex ]].index_max = index_max;
    }

    if (dev_id == BCM56832_DEVICE_ID) { 
        index_max = 8 * 1024;
    } else {
        index_max = 12 * 1024;
    }
    index_max--;
    sop->memState[L3_IIFm].index_max = index_max;

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

#endif /* BCM_TRIDENT2PLUS_SUPPORT */
