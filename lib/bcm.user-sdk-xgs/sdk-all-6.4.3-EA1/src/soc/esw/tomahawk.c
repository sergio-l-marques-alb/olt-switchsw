/*
 * $Id: tomahawk.c,v 1.1.10.25 Broadcom SDK $
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
 * File:        tomahawk.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <soc/tdm_tomahawk.h>

#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/i2c.h>
#include <soc/l2x.h>

#ifdef BCM_TOMAHAWK_SUPPORT
#undef SER_TR_TEST_SUPPORT

#include <soc/mmu_config.h>
#include <soc/tomahawk.h>
#include <soc/trident2.h>

#define _TDM_LENGTH                    256
#define _OVS_GROUP_COUNT               6
#define _OVS_GROUP_TDM_LENGTH          12

/* Value hardcoded in set_tdm.c, definition needs to be matched */
#define PORT_STATE_UNUSED              0
#define PORT_STATE_LINERATE            1
#define PORT_STATE_OVERSUB             2
#define PORT_STATE_CONTINUATION        3 /* part of other port */

#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

static uint8 _soc_alpm_mode[SOC_MAX_NUM_DEVICES];
static int num_shared_alpm_banks = 4;

typedef struct _soc_tomahawk_tdm_s {
    int speed[_TH_NUM_EXT_PORTS];
    int port_state[_TH_NUM_EXT_PORTS]; /* 0-unused, 1-line rate, 2-oversub,
									   * 3-part of other port 
									   */
    /*
    int idb_tdm[_TH_PIPES_PER_DEV][_TDM_LENGTH];
    int idb_ovs_tdm[_TH_PIPES_PER_DEV][_OVS_GROUP_COUNT][_OVS_GROUP_TDM_LENGTH];
    int mmu_tdm[_TH_PIPES_PER_DEV][_TDM_LENGTH];
    int mmu_ovs_tdm[_TH_PIPES_PER_DEV][_OVS_GROUP_COUNT][_OVS_GROUP_TDM_LENGTH];
    */
    int *idb_tdm[_TH_PIPES_PER_DEV];
    int *idb_ovs_tdm[_TH_PIPES_PER_DEV][_OVS_GROUP_COUNT];
    int *mmu_tdm[_TH_PIPES_PER_DEV];
    int *mmu_ovs_tdm[_TH_PIPES_PER_DEV][_OVS_GROUP_COUNT];
    
	int pm_encap_type[_TH_NUM_PORT_MODULES];
	
    th_tdm_globals_t tdm_globals;
    th_tdm_pipes_t tdm_pipe_tables;	

    /* Following fields are not arguments to the TDM code */
    int port_ratio[_TH_PBLKS_PER_DEV];
    int ovs_ratio_x1000[_TH_PIPES_PER_DEV];
} _soc_tomahawk_tdm_t;

typedef void (*soc_mmu_config_buf_default_f)(int unit, _soc_mmu_cfg_buf_t *buf,
                                             _soc_mmu_device_info_t *devcfg,
                                             int lossless);
int
soc_tomahawk_mem_config(int unit)
{
    soc_persist_t *sop;
    int l2_entries, fixed_l2_entries, shared_l2_banks;
    int num_ecmp_rh_flowset_entries;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
#define _SOC_TH_DEFIP_MAX_TCAMS   8
#define _SOC_TH_DEFIP_TCAM_DEPTH 1024
    int l3_entries, fixed_l3_entries, shared_l3_banks;
    int fpem_entries, shared_fpem_banks;
    int alpm_enable;

    sop = SOC_PERSIST(unit);

    /*
     * bank 0-1 are dedicated L2 banks (4k entries per bank)
     * bank 6-9 are dedicated L3 banks (2k entries per bank)
     * bank 5-8 are shared banks (32k L2/L3 or 16k FPEM entres per bank)
     */
    l2_entries = soc_property_get(unit, spn_L2_MEM_ENTRIES, 8 * 1024);
    fixed_l2_entries = 8 * 1024;  /* 8k dedicated L2 entries */
    if (l2_entries < fixed_l2_entries) {
        l2_entries = fixed_l2_entries;
        shared_l2_banks = 0;
    } else {
        l2_entries -= fixed_l2_entries;
        shared_l2_banks = (l2_entries + (32 * 1024 - 1)) / (32 * 1024);
        l2_entries = fixed_l2_entries + shared_l2_banks * 32 * 1024;
    }

    l3_entries = soc_property_get(unit, spn_L3_MEM_ENTRIES, 8 * 1024);
    fixed_l3_entries = 8 * 1024;  /* 8k dedicated L2 entries */
    if (l3_entries < fixed_l3_entries) {
        l3_entries = fixed_l3_entries;
        shared_l3_banks = 0;
    } else {
        l3_entries -= fixed_l3_entries;
        shared_l3_banks = (l3_entries + (32 * 1024 - 1)) / (32 * 1024);
        l3_entries = fixed_l3_entries + shared_l3_banks * 32 * 1024;
    }

    fpem_entries = soc_property_get(unit, spn_FPEM_MEM_ENTRIES, 0);
    shared_fpem_banks = (fpem_entries + (16 * 1024 - 1)) / (16 * 1024);
    fpem_entries = shared_fpem_banks * 16 * 1024;

    if (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 4) {
        return SOC_E_PARAM;
    }
    
    alpm_enable = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);

    if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) &&
         (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 2)) {
        return SOC_E_PARAM;
    }

    _soc_alpm_mode[unit] = alpm_enable;

    /* Adjust L2 table size */
    sop->memState[L2Xm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_TILEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_TILEm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_ECCm].index_max = l2_entries - 1;
    sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_ENTRY_ISS_LPm].index_max =
        (l2_entries - fixed_l2_entries) / 4 - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_ONLYm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = l3_entries/ 2 - 1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ONLY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ISS_LPm].index_max =
        (l3_entries - fixed_l3_entries) / 4 - 1;

    /* Adjust FP exact match table size */
    sop->memState[EXACT_MATCH_2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE0m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE1m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_PIPE3m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLYm].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE0m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE1m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE2m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_2_ENTRY_ONLY_PIPE3m].index_max =
        fpem_entries - 1;
    sop->memState[EXACT_MATCH_4m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE0m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE1m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE2m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_PIPE3m].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE0m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE1m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE2m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_4_ENTRY_ONLY_PIPE3m].index_max =
        fpem_entries / 2 - 1;
    sop->memState[EXACT_MATCH_HIT_ONLYm].index_max = fpem_entries / 2 - 1;
    sop->memState[FPEM_LPm].index_max = fpem_entries / 2 - 1;

    /* LAG and ECMP resilient hashing features share the same flow set table.
     * The table can be configured in one of 3 modes:
     * - dedicated to LAG resilient hashing,
     * - dedicated to ECMP resilient hashing,
     * - split evenly between LAG and ECMP resilient hashing.
     */
    num_ecmp_rh_flowset_entries = soc_property_get(unit,
            spn_ECMP_RESILIENT_HASH_SIZE, 32768);
    switch (num_ecmp_rh_flowset_entries) {
        case 0:
            sop->memState[RH_ECMP_FLOWSETm].index_max = -1;
            break;
        case 32768:
            sop->memState[RH_LAG_FLOWSETm].index_max /= 2;
            sop->memState[RH_ECMP_FLOWSETm].index_max /= 2;
            break;
        case 65536:
            sop->memState[RH_LAG_FLOWSETm].index_max = -1;
            break;
        default:
            return SOC_E_CONFIG;
    }

    SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_TH_DEFIP_MAX_TCAMS;
    SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_TH_DEFIP_TCAM_DEPTH; 
    
    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        if (!soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

            num_ipv6_128b_entries = soc_property_get(unit, 
                                        spn_NUM_IPV6_LPM_128B_ENTRIES, 
                                        (defip_config ? 2048 : 0));  
            num_ipv6_128b_entries = num_ipv6_128b_entries + 
                                    (num_ipv6_128b_entries % 2);

            config_v6_entries = num_ipv6_128b_entries;
            if (soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
                num_ipv6_128b_entries = 0;
                if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                    config_v6_entries = ((config_v6_entries /
                                        SOC_CONTROL(unit)->l3_defip_tcam_size) +
                                        ((config_v6_entries %
                                         SOC_CONTROL(unit)->l3_defip_tcam_size)
                                        ? 1 : 0)) * SOC_CONTROL(unit)->l3_defip_tcam_size;
                }
            }
            sop->memState[L3_DEFIP_PAIR_128m].index_max = 
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = 
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = 
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = 
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLY_Xm].index_max = 
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLY_Ym].index_max = 
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIPm].index_max = 
                                     (SOC_CONTROL(unit)->l3_defip_max_tcams * 
                                     SOC_CONTROL(unit)->l3_defip_tcam_size) - 
                                     (num_ipv6_128b_entries * 2) - 1;

            sop->memState[L3_DEFIP_ONLYm].index_max = 
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLY_Xm].index_max = 
                                          sop->memState[L3_DEFIPm].index_max;
            sop->memState[L3_DEFIP_HIT_ONLY_Ym].index_max = 
                                          sop->memState[L3_DEFIPm].index_max;
            SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
        } else {
            if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 0)) {
                /* slightly different processing for v6-128 */
                num_ipv6_128b_entries = soc_property_get(unit, 
                                            spn_NUM_IPV6_LPM_128B_ENTRIES, 
                                            2048);  
                num_ipv6_128b_entries = num_ipv6_128b_entries + 
                                        (num_ipv6_128b_entries % 2);
                config_v6_entries = num_ipv6_128b_entries;
                sop->memState[L3_DEFIP_PAIR_128m].index_max = 
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = 
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = 
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = 
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLY_Xm].index_max = 
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLY_Ym].index_max = 
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIPm].index_max = 
                                  (SOC_CONTROL(unit)->l3_defip_max_tcams * 
                                  SOC_CONTROL(unit)->l3_defip_tcam_size) - 
                                  (num_ipv6_128b_entries * 2) - 1;
    
                sop->memState[L3_DEFIP_ONLYm].index_max = 
                                       sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 
                                       sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 
                                       sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_HIT_ONLY_Xm].index_max = 
                                       sop->memState[L3_DEFIPm].index_max;
                sop->memState[L3_DEFIP_HIT_ONLY_Ym].index_max = 
                                       sop->memState[L3_DEFIPm].index_max;
                
                if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0) == 2) {
                    SOC_CONTROL(unit)->l3_defip_index_remap = 
                                                  num_ipv6_128b_entries * 2;
                } else {
                    /* half the v6-128 entries, times the no. of v4 per 
                     * defip 
                     */
                    SOC_CONTROL(unit)->l3_defip_index_remap = 
                                              (num_ipv6_128b_entries/2)*2;
                }
            } else {
                sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
            }
        }
        soc_l3_defip_indexes_init(unit, config_v6_entries); 
    }

    /* Adjust ALPM table size */
    if (soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
        /* The check below is sufficient because if ALPM mode is enabled
         * and number of shared banks used is > 2 we return SOC_E_PARAM
         * in code at the top of this function.
         */
        if (shared_l2_banks + shared_l3_banks + shared_fpem_banks > 0) {
            sop->memState[L3_DEFIP_ALPM_RAWm].index_max =
                sop->memState[L3_DEFIP_ALPM_RAWm].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV4m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV4m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max =
                sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max/2;
            sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max =
                sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max/2;
            sop->memState[L3_DEFIP_ALPM_ECCm].index_max =
                sop->memState[L3_DEFIP_ALPM_ECCm].index_max/2;
        }
    } else {
        sop->memState[L3_DEFIP_ALPM_RAWm].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV4m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV4_1m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV6_64m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV6_64_1m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_IPV6_128m].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_HIT_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_ALPM_ECCm].index_max = -1;
    }
    return SOC_E_NONE;
}

int
soc_tomahawk_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                   int enable, int *orig_enable)
{
    soc_reg_t reg;
    soc_field_t field;
    int blk, port;
    uint32 rval;

    switch (mem) {
    case CLPORT_WC_UCMEM_DATAm:
        reg = CLPORT_WC_UCMEM_CTRLr;
        field = ACCESS_MODEf;
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            /* It will use the setting from the last block */
            *orig_enable = soc_reg_field_get(unit, reg, rval, field);
            soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
        return SOC_E_NONE;
    default:
        return _soc_trident2_mem_cpu_write_control(unit, mem, copyno,
                                                   enable, orig_enable);
    }
}


int
soc_tomahawk_reg_cpu_write_control(int unit, int enable)
{
    uint32 intfo_dis = 0;
    
    if (enable) {
        /* Disable HW updates */
        soc_reg_field_set(unit, INTFO_HW_UPDATE_DISr, &intfo_dis,
                          EG_SPf, 1);
        soc_reg_field_set(unit, INTFO_HW_UPDATE_DISr, &intfo_dis,
                          ING_SPf, 1);
        soc_reg_field_set(unit, INTFO_HW_UPDATE_DISr, &intfo_dis,
                          CONGST_STf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_INTFO_HW_UPDATE_DISr(unit, intfo_dis));

    return SOC_E_NONE;
}

uint32 _soc_th_mmu_port(int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_p2m_mapping[si->port_l2p_mapping[port]];
}

/*
 * cpu port (mmu port 0): 48 queues (2000-2047)
 * loopback port (mmu port 116): 8 queues (4048-4055)
 */
int
soc_tomahawk_num_cosq_init(int unit)
{
    soc_info_t *si;
    int port, mmu_port;

    si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        mmu_port = SOC_TH_MMU_PORT(unit, port);

        if (IS_CPU_PORT(unit, port)) {
            si->port_num_cosq[port] = 48;
            si->port_cosq_base[port] = (mmu_port & 0x3f) * 10;
        } else if (IS_LB_PORT(unit, port)) {
            si->port_num_cosq[port] = 10;
            si->port_cosq_base[port] = (mmu_port & 0x3f) * 10;
        } else {
            si->port_num_cosq[port] = 10;
            si->port_cosq_base[port] = (mmu_port & 0x3f) * 10;
            si->port_num_uc_cosq[port] = 10;
            si->port_uc_cosq_base[port] = (mmu_port & 0x3f) * 10;
            si->port_num_ext_cosq[port] = 0;
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_tomahawk_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                               int skew_id, int *frequency)
{
    switch (dev_id) {
    case BCM56960_DEVICE_ID:
        *frequency = 850;
        break;
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}

void
soc_tomahawk_tsc_map_get(int unit, uint32 *tsc_map)
{
    *tsc_map = 0xffffffff;
}

void
soc_tomahawk_pipe_map_get(int unit, uint32 *pipe_map)
{
    soc_info_t *si;
    int port;

    si = &SOC_INFO(unit);

    *pipe_map = 0;
    PBMP_LB_ITER(unit, port) {
        *pipe_map |= 1 << si->port_pipe[port];
    }
}

/*
 * #1   single: 100  -   x  -   x  -   x  SOC_TH_PORT_RATIO_SINGLE
 * #2   single:  40  -   x  -   x  -   x  SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #3   dual:    50  -   x  -  50  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #4   dual:    40  -   x  -  40  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #5   dual:    20  -   x  -  20  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #6   dual:    40  -   x  -  20  -   x  SOC_TH_PORT_RATIO_DUAL_2_1
 * #7   dual:    20  -   x  -  40  -   x  SOC_TH_PORT_RATIO_DUAL_1_2
 * #8   tri:     50  -   x  - 25/x - 25/x SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #9   tri:     20  -   x  - 10/x - 10/x SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     40  -   x  - 10/x - 10/x SOC_TH_PORT_RATIO_TRI_023_4_1_1
 * #11  tri:    25/x - 25/x -  50  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    10/x - 10/x -  20  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_2
 * #13  tri:    10/x - 10/x -  40  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_4
 * #14  quad:   25/x - 25/x - 25/x - 25/x SOC_TH_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_TH_PORT_RATIO_QUAD
 */
void
soc_tomahawk_port_ratio_get(int unit, int clport, int *mode)
{
    soc_info_t *si;
    int port, phy_port_base, lane;
    int num_lanes[_TH_PORTS_PER_PBLK];
    int speed_max[_TH_PORTS_PER_PBLK];

    si = &SOC_INFO(unit);
    phy_port_base = 1 + clport * _TH_PORTS_PER_PBLK;
    for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane += 2) {
        port = si->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
            speed_max[lane] = si->port_speed_max[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TH_PORT_RATIO_SINGLE;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        if (speed_max[0] == speed_max[2]) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else if (speed_max[0] > speed_max[2]) {
            *mode = SOC_TH_PORT_RATIO_DUAL_2_1;
        } else {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_2;
        }
    } else if (num_lanes[0] == 2) {
        if (num_lanes[2] == -1) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[0] == 20000 || speed_max[0] == 21000 ) ?
                SOC_TH_PORT_RATIO_TRI_023_2_1_1 : SOC_TH_PORT_RATIO_TRI_023_4_1_1;
        }
    } else if (num_lanes[2] == 2) {
        if (num_lanes[0] == -1) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[2] == 20000 || speed_max[2] == 21000 ) ?
                SOC_TH_PORT_RATIO_TRI_012_1_1_2 : SOC_TH_PORT_RATIO_TRI_012_1_1_4;
        }
    } else {
        *mode = SOC_TH_PORT_RATIO_QUAD;
    }
}

#if 0
STATIC void
_soc_tomahawk_pipe_bandwidth_get(int unit, int pipe,
                                 int *max_pipe_core_bandwidth,
                                 int *pipe_linerate_bandwidth,
                                 int *pipe_oversub_bandwidth)
{
    soc_info_t *si;
    int port;
    int bandwidth;

    si = &SOC_INFO(unit);

    *max_pipe_core_bandwidth = si->bandwidth / 4;

    *pipe_linerate_bandwidth = 0;
    *pipe_oversub_bandwidth = 0;
    PBMP_PORT_ITER(unit, port) {
        if (si->port_pipe[port] != pipe) {
            continue;
        }
        bandwidth = si->port_speed_max[port] >= 2500 ?
            si->port_speed_max[port] : 2500;
        if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            *pipe_oversub_bandwidth += bandwidth;
        } else {
            *pipe_linerate_bandwidth += bandwidth;
        }
    }
    /* 10G cpu/management, 5G loopback, 10G purge/refresh */
    *pipe_linerate_bandwidth += 25;
}
#endif

#if defined(BCM_TH_ASF_EXCLUDE)
/*
 * Cut-through class encoding
 * 0 - SAF
 * 1 - 10GE
 * 2 - HG[11]
 * 3 - 20GE
 * 4 - 21[HG}
 * 5 - 25GE
 * 6 - HG[27]
 * 7 - 40GE
 * 8 - HG[42]
 * 9 - 50GE
 * 10 - HG[53]
 * 11 - 100GE
 * 12 - HG[106]
 */
STATIC void
_soc_tomahawk_speed_to_ct_class_mapping(int unit, int speed, int *ct_class)
{
    if (speed >= 40000) {
        if (speed >= 100000) {
            *ct_class = speed > 100000 ? 12 : 11;
        } else if (speed >= 50000) {
            *ct_class = speed > 50000 ? 10 : 9;
        } else {
            *ct_class = speed > 40000 ? 8 : 7;
        }
    } else {
        if (speed >= 25000) {
            *ct_class = speed > 25000 ? 6 : 5;
        } else if (speed >= 20000) {
            *ct_class = speed > 20000 ? 4 : 3;
        } else {
            *ct_class = speed > 10000 ? 2 : 1;
        }
    }
}
#endif

/*
 * Oversubscription group speed class encoding
 * 0 - 0
 * 1 - 2 (10G)
 * 2 - 4 (20G)
 * 3 - 5 (25G)
 * 4 - 8 (40G)
 * 5 - 10 (50G)
 * 6 - 20 (100G)
 */
STATIC void
_soc_tomahawk_speed_to_ovs_class_mapping(int unit, int speed, int *ovs_class)
{
    if (speed >= 40000) {
        if (speed >= 100000) {
            *ovs_class = 6;
        } else if (speed >= 50000) {
            *ovs_class = 5;
        } else {
            *ovs_class = 4;
        }
    } else {
        if (speed >= 25000) {
            *ovs_class = 3;
        } else if (speed >= 20000) {
            *ovs_class = 2;
        } else {
            *ovs_class = 1;
        }
    }
}

STATIC void
_soc_tomahawk_speed_to_slot_mapping(int unit, int speed, int *slot)
{
    if (speed >= 40000) {
        if (speed >= 100000) {
            *slot = 40;
        } else if (speed >= 50000) {
            *slot = 20;
        } else {
            *slot = 16;
        }
    } else {
        if (speed >= 25000) {
            *slot = 10;
        } else if (speed >= 20000) {
            *slot = 8;
        } else if (speed >= 10000) {
            *slot = 4;
        } else {
            *slot = 1;
        }
    }
}

/*
 * Tomahawk port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          0/32       0        32
 *     CLPORT0-7     1-32       0/0-31     1-32     0-31
 *     CLPORT8-15    33-64      1/0-31     34-66    64-95
 *     CLPORT16-23   65-96      2/0-31     68-100   128-159
 *     CLPORT24-31   97-128     3/0-31     102-133  192-223
 *     XLPORT0/0     129        1/32       34-66    96
 *     hole          130        3/32       134      224
 *     XLPORT0/2     131        2/32       68-100   160
 *     LBPORT0       132        0/33       33       33
 *     LBPORT1       133        1/33       67       97
 *     LBPORT2       134        2/33       101      161
 *     LBPORT3       135        3/33       135      225
 * Software uses device port 135 for LBPORT3 although it can be 134
 * Device port number is flexible within the above range
 * Although MMU port number is flexible within the above range, it is
 *     configured as a fixed mapped to physical port number
 */
int
soc_tomahawk_port_config_init(int unit, uint16 dev_id)
{
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv;
    int num_port, num_phy_port, num_mmu_port;
    int port, phy_port, mmu_port, idb_port;
    int pipe, xpe, sc, index;
    int port_bandwidth;
    char option1, option2;
    uint32 pipe_map, xpe_map, sc_map, ipipe_map, epipe_map;
    soc_pbmp_t oversub_pbm;
    static const struct {
        int port;
        int phy_port;
        int pipe;
        int idb_port;
        int mmu_port;
    } fixed_ports[] = {
        { 0,   0,   0, 32, 32 },    /* cpu port */
        { 33,  132, 0, 33, 33 },    /* loopback port 0 */
        { 67,  133, 1, 33, 97 },    /* loopback port 1 */
        { 101, 134, 2, 33, 161 },   /* loopback port 2 */
        { 135, 135, 3, 33, 225 }    /* loopback port 3 */
    };

    si = &SOC_INFO(unit);

    num_phy_port = 136;
    num_port = 136;
    num_mmu_port = 226;

    soc_tomahawk_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);

    si->bandwidth = 2048000;


    oversub_pbm = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        si->port_p2l_mapping[phy_port] = -1;
        si->port_p2m_mapping[phy_port] = -1;
    }
    for (port = 0; port < num_port; port++) {
        si->port_l2p_mapping[port] = -1;
        si->port_l2i_mapping[port] = -1;
        si->port_speed_max[port] = -1;
        si->port_group[port] = -1;
        si->port_serdes[port] = -1;
        si->port_pipe[port] = -1;
        si->port_num_lanes[port] = -1;
    }
    for (mmu_port = 0; mmu_port < num_mmu_port; mmu_port++) {
        si->port_m2p_mapping[mmu_port] = -1;
    }
    SOC_PBMP_CLEAR(si->eq_pbm);
    SOC_PBMP_CLEAR(si->management_pbm);
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        SOC_PBMP_CLEAR(si->pipe_pbm[pipe]);
    };
    SOC_PBMP_CLEAR(si->oversub_pbm);
    SOC_PBMP_CLEAR(si->all.disabled_bitmap);

    /* Populate the fixed mapped ports */
    for (index = 0; index < COUNTOF(fixed_ports); index++) {
        port = fixed_ports[index].port;
        phy_port = fixed_ports[index].phy_port;
        pipe = fixed_ports[index].pipe;;

        si->port_l2p_mapping[port] = phy_port;
        si->port_l2i_mapping[port] = fixed_ports[index].idb_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_p2m_mapping[phy_port] = fixed_ports[index].mmu_port;
        si->port_pipe[port] = pipe;
    }

    rv = SOC_E_NONE;
    for (port = 1; port < num_port; port++) {
        if (si->port_l2p_mapping[port] != -1) { /* fixed mapped port */
            continue;
        }
        if (port == 134) { /* SDK uses device port 135 for loopback port 3 */
            continue;
        }

        config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }

        /*
         * portmap_<port>=<physical port number>:<bandwidth in Gb>[:<i(inactive)/1/2/4(num lanes)>][:<i>]
         * eg:    portmap_1=1:100
         *     or portmap_1=1:40
         *     or portmap_1=1:40:i
         *     or portmap_1=1:40:2
         *     or portmap_1=1:40:2:i
         */
        sub_str = config_str;

        /* Parse physical port number */
        phy_port = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Missing physical port information \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (phy_port < 0 || phy_port >= num_phy_port) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* coverity[check_after_sink : FALSE] */
        if (si->port_p2l_mapping[phy_port] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Physical port %d is used by port %d\n"),
                     port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Skip ':' between physical port number and bandwidth */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        /* Parse bandwidth */
        for (index = 0; index < sizeof(str_2p5) - 1; index++) {
            if (sub_str[index] == '\0') {
                break;
            }
            str_buf[index] = sub_str[index];
        }
        str_buf[index] = '\0';
        if (!sal_strcmp(str_buf, str_2p5)) {
            port_bandwidth = 2500;
            sub_str_end = &sub_str[sizeof(str_2p5) - 1];
        } else {
            port_bandwidth = sal_ctoi(sub_str, &sub_str_end) * 1000;
            if (sub_str == sub_str_end) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Missing bandwidth information \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            switch (port_bandwidth) {
            case 1000:
            case 10000:
            case 11000:
            case 20000:
            case 21000:
            case 25000:
            case 27000:
            case 40000:
            case 42000:
            case 50000:
            case 53000:
            case 100000:
            case 106000:
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Invalid bandwidth %d Gb\n"),
                         port, port_bandwidth / 1000));
                rv = SOC_E_FAIL;
                continue;
            }
        }

        /* Check if option presents */
        option1 = 0; option2 = 0;
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            /* Skip ':' between bandwidth and options */
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;

            if (*sub_str != '\0') {
                if (!(*sub_str == 'i' || *sub_str == '1' || *sub_str == '2' ||
                      *sub_str == '4')) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Port %d: Bad config string \"%s\"\n"),
                             port, config_str));
                    rv = SOC_E_FAIL;
                    continue;
                }
                option1 = *sub_str;
                sub_str++;
                
                /* Check if more options present */
                if (*sub_str != '\0') {
                    /* Skip ':' between options */
                    if (*sub_str != ':') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    sub_str++;
                    
                    if (*sub_str != 'i') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string \"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    option2 = *sub_str;
                    sub_str++;
                }
            }
        }

        /* Check trailing string */
        if (*sub_str != '\0') {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Bad config string \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Update soc_info */
        si->port_l2p_mapping[port] = phy_port;
        si->port_p2l_mapping[phy_port] = port;
        if (phy_port == 129) { /* management port 0 */
            pipe = 1;
            si->port_l2i_mapping[port] = 32;
            si->port_p2m_mapping[phy_port] = 96;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else if (phy_port == 131) { /* management port 1 */
            pipe = 2;
            si->port_l2i_mapping[port] = 32;
            si->port_p2m_mapping[phy_port] = 160;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else {
            idb_port = (phy_port - 1) % _TH_PORTS_PER_PIPE;
            pipe = (phy_port - 1) / _TH_PORTS_PER_PIPE;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] =
                (pipe << 6) | ((idb_port & 0x1) << 4) | (idb_port >> 1);
            si->port_serdes[port] = (phy_port - 1) / _TH_PORTS_PER_PBLK;
        }
        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        si->port_speed_max[port] = port_bandwidth;
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        } else {
            switch (option1) {
            case '1': si->port_num_lanes[port] = 1; break;
            case '2': si->port_num_lanes[port] = 2; break;
            case '4': si->port_num_lanes[port] = 4; break;
            default: si->port_num_lanes[port] = 4; break;
            }
        }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        }
        if (SOC_PBMP_MEMBER(oversub_pbm, port)) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }
    }

    for (index = 1; index < COUNTOF(fixed_ports); index++) {
        pipe = fixed_ports[index].pipe;
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            /* Remove loopback port if the entire pipe is not in use */
            port = fixed_ports[index].port;
            phy_port = fixed_ports[index].phy_port;
            si->port_l2p_mapping[port] = -1;
            si->port_l2i_mapping[port] = -1;
            si->port_p2l_mapping[phy_port] = -1;
            si->port_p2m_mapping[phy_port] = -1;
            si->port_pipe[port] = -1;
        } else {
            SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "physical to logical mapping:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %3d"), si->port_p2l_mapping[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        LOG_CLI((BSL_META_U(unit,
                            "physical port bandwidth:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            port = si->port_p2l_mapping[index];
            if (port == -1) {
                LOG_CLI((BSL_META_U(unit,
                                    "  -1")));
            } else if (si->port_speed_max[port] == 2500) {
                LOG_CLI((BSL_META_U(unit,
                                    " 2.5")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), si->port_speed_max[port] / 1000));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));

        LOG_CLI((BSL_META_U(unit,
                            "physical to mmu mapping:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %3d"), si->port_p2m_mapping[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    

    /* Setup port_num_lanes */
    for (port = 0; port < num_port; port++) {
        if (si->port_speed_max[port] > 53000) {
            si->port_num_lanes[port] = 4;
        } else if (si->port_speed_max[port] == 40000 ||
                   si->port_speed_max[port] == 42000) {
            /* Note: 40G, HG[42] can operate either in dual or quad lane mode 
                     Check if adjacent ports exist to set dual lane mode */
            phy_port = si->port_l2p_mapping[port];
            if (phy_port >= 1 && phy_port <= 127 && phy_port != -1) {
                int sis_port;

                if (phy_port % 4 == 1) {
                    sis_port = si->port_p2l_mapping[phy_port+2];
                    if ((si->port_speed_max[sis_port] > 0) &&
                        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, sis_port)) {
                        si->port_num_lanes[port] = 2;
                    }
                } else if (phy_port % 4 == 3) {
                    sis_port = si->port_p2l_mapping[phy_port-2];
                    if ((si->port_speed_max[sis_port] > 0) &&
                        !SOC_PBMP_MEMBER(si->all.disabled_bitmap, sis_port)) {
                        si->port_num_lanes[port] = 2;
                    }
                }
            }
            /* Else set to quad lane mode by default if the user did not 
               specify anything and no sister ports existed */
            if (si->port_num_lanes[port] == -1) {
                si->port_num_lanes[port] = 4;
            }
        } else if ((si->port_speed_max[port] >= 20000) && 
                    !(si->port_speed_max[port] == 25000 ||
                      si->port_speed_max[port] == 27000)) {
            /* 50G, HG[53], 20G MLD, HG[21] use 2 lanes */
            si->port_num_lanes[port] = 2;
        } else if (si->port_speed_max[port] > 0) {
            /* 10G XFI, HG[11], 25G XFI and HG[27] use 1 lane */
            si->port_num_lanes[port] = 1;
        }
    }

    /* Setup high speed port (HSP) pbm */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            continue;
        }
        if (si->port_speed_max[port] >= 40000) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }
    }

    /* Setup pipe/xpe mapping
     * XPE0 (SC_R/XPE_A): IP 0/3 EP 0/1
     * XPE1 (SC_S/XPE_A): IP 0/3 EP 2/3
     * XPE2 (SC_R/XPE_B): IP 1/2 EP 0/1
     * XPE3 (SC_S/XPE_B): IP 1/2 EP 2/3
     */
    pipe_map = 0;
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        pipe_map |= 1 << pipe;
    }
    xpe_map = 0;
    for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
        ipipe_map = (xpe < 2 ? 0x9 : 0x6) & pipe_map;
        epipe_map = (xpe & 1 ? 0xc : 0x3) & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        xpe_map |= 1 << xpe;
        si->xpe_ipipe_map[xpe] = ipipe_map;
        si->xpe_epipe_map[xpe] = epipe_map;
    }
    sc_map = 0;
    for (sc = 0; sc < NUM_SLICE(unit); sc++) {
        ipipe_map = pipe_map;
        epipe_map = (sc & 1 ? 0xc : 0x3) & pipe_map;
        if (ipipe_map == 0 || epipe_map == 0) {
            continue;
        }
        sc_map |= 1 << sc;
        si->sc_ipipe_map[sc] = ipipe_map;
        si->sc_epipe_map[sc] = epipe_map;
    }
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        /* Number of pipes is 4 in Tomahawk */
        /* coverity[overrun-local : FALSE] */
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        /* coverity[overrun-local : FALSE] */
        si->ipipe_xpe_map[pipe] = (pipe == 0 || pipe == 3 ? 0x3 : 0xc) &
            xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_xpe_map[pipe] = (pipe < 2 ? 0x5 : 0xa) & xpe_map;
        /* coverity[overrun-local : FALSE] */
        si->ipipe_sc_map[pipe] = sc_map;
        /* coverity[overrun-local : FALSE] */
        si->epipe_sc_map[pipe] = (pipe < 2 ? 0x1 : 0x2) & sc_map;
    }

    return rv;
}

#if !defined(BCM_TH_ASF_EXCLUDE)
int
soc_th_port_oversub_ratio_get(
    int              unit,
    soc_port_t       port,
    OUT int* const   ratio)
{
    soc_info_t          *si;
    _soc_tomahawk_tdm_t *tdm;
    int                  pipe;

    if (!ratio) {
        return SOC_E_PARAM;
    }

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (!si || !tdm) {
        return SOC_E_INTERNAL;
    }

    pipe = si->port_pipe[port];
    *ratio = tdm->ovs_ratio_x1000[pipe];

    return SOC_E_NONE;
}
#endif

static uint32 _soc_th_temp_mon_mask[SOC_MAX_NUM_DEVICES];

int
soc_tomahawk_chip_reset(int unit)
{
#define _SOC_TH_LCPLL_COUNT    4
#define _SOC_TH_PLL_CFG_FIELDS 4
#define _SOC_TH_DEF_TEMP_MAX   125
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
    soc_reg_t reg;
    int num_banks, index, shared_bank;
    int num_shared_l2_banks, num_shared_l3_banks, num_shared_fpem_banks;
    uint32 config = 0, map = 0, fpmap = 0;
    soc_field_t fields[_SOC_TH_PLL_CFG_FIELDS];
    uint32 values[_SOC_TH_PLL_CFG_FIELDS];
    static soc_field_t l2_fields[] = {
        L2_ENTRY_BANK_2f, L2_ENTRY_BANK_3f, L2_ENTRY_BANK_4f,
        L2_ENTRY_BANK_5f
    };
    static soc_field_t l3_fields[] = {
        L3_ENTRY_BANK_4f, L3_ENTRY_BANK_5f, L3_ENTRY_BANK_6f,
        L3_ENTRY_BANK_7f
    };
    static soc_field_t fpem_fields[] = {
        FPEM_ENTRY_BANK_0f, FPEM_ENTRY_BANK_1f, FPEM_ENTRY_BANK_2f,
        FPEM_ENTRY_BANK_3f
    };
    static const soc_reg_t lcpll_ctrl0_reg[] = {
        TOP_XG_PLL0_CTRL_0r, TOP_XG_PLL1_CTRL_0r,
        TOP_XG_PLL2_CTRL_0r, TOP_XG_PLL3_CTRL_0r
    };
    static const soc_reg_t lcpll_ctrl3_reg[] = {
        TOP_XG_PLL0_CTRL_3r, TOP_XG_PLL1_CTRL_3r,
        TOP_XG_PLL2_CTRL_3r, TOP_XG_PLL3_CTRL_3r
    };
    static const soc_reg_t lcpll_ctrl4_reg[] = {
        TOP_XG_PLL0_CTRL_4r, TOP_XG_PLL1_CTRL_4r,
        TOP_XG_PLL2_CTRL_4r, TOP_XG_PLL3_CTRL_4r
    };
    static const soc_reg_t lcpll_status_reg[] = {
        TOP_XG_PLL0_STATUSr, TOP_XG_PLL1_STATUSr,
        TOP_XG_PLL2_STATUSr, TOP_XG_PLL3_STATUSr
    };
    static const soc_reg_t temp_thr_reg[] = { 
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_7_INTR_THRESHOLDr,
        TOP_PVTMON_8_INTR_THRESHOLDr 
    };
    static const char *temp_thr_prop[] = { 
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold", "temp8_threshold" 
    };

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * ring 0: IP(1), LBPORT0(54), LBPORT1(51), LBPORT2(52), LBPORT3(53)
     * ring 1: EP(2)
     * ring 2: MMU_XPE(3), MMU_SC(4), MMU_GLB(5)
     * ring 3: PM7(22), PM6(21), PM5(20), PM4(19), PM3(18), PM2(17), PM1(16),
     *         PM0(15), PM31(46), PM30(45), PM29(44), PM28(43), PM27(42),
     *         PM26(41), PM25(40), PM24(39), CLPORT32(55)
     * ring 4: PM32(11), PM8(23), PM9(24), PM10(25), PM11(26), PM12(27),
     *         PM13(28), PM14(29), PM15(30), PM16(31), PM17(32), PM18(33),
     *         PM19(34), PM20(35), PM21(36), PM22(37), PM23(38)
     * ring 5: OTPC(6), AVS(59), TOP(7), SER(8)
     */
    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x55222100);
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x30004005);
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x43333333);
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x44444444);
    WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x34444444);
    WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x03333333);
    WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x30000000);
    WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x00005000);
    WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);

    sal_usleep(to_usec);


    fields[0] = ISO_INf;
    values[0] = 1;
    fields[1] = PWRONf;
    values[1] = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, TOP_CORE_PLL1_CTRL_1r,
                                 REG_PORT_ANY, 2, fields, values));

    /* Program LCPLL frequency */
    for (index = 0; index < COUNTOF(lcpll_ctrl0_reg); index++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl4_reg[index],
                                    REG_PORT_ANY, POST_RST_SELf, 3));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl0_reg[index],
                                    REG_PORT_ANY, PDIVf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl0_reg[index],
                                    REG_PORT_ANY, NDIV_INTf, 20));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, lcpll_ctrl3_reg[index],
                                    REG_PORT_ANY, MUX_OUT_SELf, 1));
    }

    /* Configure TS PLL */ 
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, POST_RESETB_SELECTf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_0r(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_PDIV, 2));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, 5));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_2r(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));    
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_TS_PLL_N, 100));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, NDIV_FRACf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_3r(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_TS_KA, 0));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_TS_KI, 2));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_TS_KP, 3));
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_4r(unit, rval));
    
    /* Set 250Mhz (implies 4ns resolution) default timesync clock to 
       calculate assymentric delays */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */
     
    /* Configure BS PLL */    
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, POST_RESETB_SELECTf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval));
    
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_BS_PDIV, 2));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, CH0_MDIVf,
                      soc_property_get(unit, spn_PTP_BS_MNDIV, 125));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, NDIV_INTf, 100);
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, NDIV_FRACf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KAf,
                      soc_property_get(unit, spn_PTP_BS_KA, 0));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KIf,
                      soc_property_get(unit, spn_PTP_BS_KI, 2));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KPf,
                      soc_property_get(unit, spn_PTP_BS_KP, 3));
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_4r(unit, rval));

    /* Bring LCPLL, time sync PLL, BroadSync PLL, AVS out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL2_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL3_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    if (!SAL_BOOT_SIMULATION) {
        /* Check LCPLL lock status */
        for (index = 0; index < _SOC_TH_LCPLL_COUNT; index++) {
            reg = lcpll_status_reg[index];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            if (!soc_reg_field_get(unit, reg, rval, TOP_XGPLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "LCPLL %d not locked on unit %d "
                                      "status = 0x%08x\n"), index, unit, rval));
            }
        }
        /* Check time sync lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TS_PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        /* Check BroadSync lock status */
        reg = TOP_BS_PLL0_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BS_PLL0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
    }

    /* De-assert LCPLL's post reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL1_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL2_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_XG_PLL3_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_usleep(to_usec);

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon6 (close to core_plls at center of die).
     */
    temp_mask = soc_property_get(unit, "temp_monitor_select", 1<<6);
    /* The above is a 9 bit mask to indicate which temp sensor to hook up to 
     * the interrupt.
     */
    rval = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg); index++) {
        uint32 trval;

        /* data = 410.04-(temp/0.48705) = (410040-(temp*1000))/487
         * Note: Since this is a high temp value we can safely assume it to 
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = soc_property_get(unit, temp_thr_prop[index], _SOC_TH_DEF_TEMP_MAX);
        if (temp_thr > 410) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unsupported temp value %d !! Max 410. Using %d.\n"),
                       temp_thr, _SOC_TH_DEF_TEMP_MAX));
            temp_thr = _SOC_TH_DEF_TEMP_MAX;
        }
        /* Convert temperature to config data */
        temp_thr = (410040-(temp_thr*1000))/487;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if (temp_mask & (1 << index)) {
            rval |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    _soc_th_temp_mon_mask[unit] = temp_mask;

    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASKr(unit, rval));
    /* Enable temp mon interrupt */
    (void)soc_cmicm_intr3_enable(unit, 0x4); /* PVTMON_INTR bit 2 */

    /* Bring port blocks out of reset */
    rval = 0;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM32_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffffffff));
    sal_usleep(to_usec);

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);
    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, L2Xm, &num_banks));
    num_shared_l2_banks = num_banks - 2; /* minus 2 dedicated L2 banks */
    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config, L2_ENTRY_BANK_CONFIGf,
                      (1 << num_shared_l2_banks) - 1);
    for (index = 0; index < num_shared_l2_banks; index++) {
        shared_bank = index;
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          l2_fields[index], shared_bank);
    }

    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, L3_ENTRY_ONLYm, &num_banks));
    num_shared_l3_banks = num_banks - 4; /* minus 4 dedicated L3 banks */
    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config, L3_ENTRY_BANK_CONFIGf,
                      ((1 << num_shared_l3_banks) - 1) <<
                      (4 - num_shared_l3_banks));
    for (index = 0; index < num_shared_l3_banks; index++) {
        shared_bank = 3 - index;
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          l3_fields[index], shared_bank);
    }

    SOC_IF_ERROR_RETURN
        (soc_tomahawk_hash_bank_count_get(unit, EXACT_MATCH_2m,
                                          &num_shared_fpem_banks));
    soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                      FPEM_ENTRY_BANK_CONFIGf,
                      ((1 << num_shared_fpem_banks) - 1) << num_shared_l2_banks);
    for (index = 0; index < num_shared_fpem_banks; index++) {
        shared_bank = index + num_shared_l2_banks;
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                          fpem_fields[index], shared_bank);
        soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, &fpmap,
                          fpem_fields[index], shared_bank);
    }
    
    if (soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        int non_alpm = num_shared_fpem_banks + num_shared_l3_banks + 
                       num_shared_l2_banks;

        if ((non_alpm) && (non_alpm <= 2)) {
            soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                              ALPM_ENTRY_BANK_CONFIGf, 0x3);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              ALPM_BANK_MODEf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 1));
             SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                        REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0x3));
            num_shared_alpm_banks = 2;
        } else {
            soc_reg_field_set(unit, ISS_BANK_CONFIGr, &config,
                              ALPM_ENTRY_BANK_CONFIGf, 0xf);
            soc_reg_field_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, &map,
                              ALPM_BANK_MODEf, 0);
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_LOG_TO_PHY_BANK_MAP_2r,
                                        REG_PORT_ANY, ALPM_BANK_MODEf, 0));
             SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ISS_MEMORY_CONTROL_84r,
                                        REG_PORT_ANY, BYPASS_ISS_MEMORY_LPf, 0xf));
             num_shared_alpm_banks = 4;
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_BANK_CONFIGr, REG_PORT_ANY, 0, config));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAPr, REG_PORT_ANY, 0, map));
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, ISS_LOG_TO_PHY_BANK_MAP_3r, REG_PORT_ANY, 0, fpmap));

    return SOC_E_NONE;
}

int
soc_tomahawk_port_reset(int unit)
{
    int blk, port;
    uint32 rval;

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
    }
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
    }

    return SOC_E_NONE;
}

int
soc_tomahawk_port_speed_update(int unit, soc_port_t port, int speed)
{
    soc_info_t *si;
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval, fval;
#if defined(BCM_TH_ASF_EXCLUDE)
    uint32 entry[SOC_MAX_MEM_WORDS];
#endif
    int pipe, phy_port, mmu_port, inst;
#if defined(BCM_TH_ASF_EXCLUDE)
    int class;
#else
    int rv;
#endif
    static soc_field_t t2oq_fields[] = {
        IS_MC_T2OQ_PORT0f, IS_MC_T2OQ_PORT1f
    };

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[port];

#if defined(BCM_TH_ASF_EXCLUDE)
    if (IS_CL_PORT(unit, port)) {
        /* Update cut-through speed class */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        if (soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, ENABLEf)) {
            _soc_tomahawk_speed_to_ct_class_mapping(unit, speed, &class);

            rval = 0;
            soc_reg_field_set(unit, ASF_IPORT_CFGr, &rval, ASF_PORT_SPEEDf,
                              class);
            SOC_IF_ERROR_RETURN(WRITE_ASF_IPORT_CFGr(unit, port, rval));

            sal_memset(entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
            soc_mem_field32_set(unit, EGR_IP_CUT_THRU_CLASSm, entry,
                                CUT_THRU_CLASSf, class);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_IP_CUT_THRU_CLASSm, MEM_BLOCK_ALL,
                               port, entry));
        }
    }
#endif

    /* Updtae HSP port multicast T2OQ setting */
    if (SOC_PBMP_MEMBER(si->eq_pbm, port)) {
        reg = MMU_SCFG_TOQ_MC_CFG1r;
        inst = (pipe >> 1) | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        field = t2oq_fields[pipe & 0x01];
        fval = soc_reg_field_get(unit, reg, rval, field);
        if (speed >= 40000) {
            fval |= 1 << (mmu_port & 0x0f);
        } else {
            fval &= ~(1 << (mmu_port & 0x0f));
        }
        soc_reg_field_set(unit, reg, &rval, field, fval);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

#if !defined(BCM_TH_ASF_EXCLUDE)
    if (soc_feature(unit, soc_feature_asf_multimode)) {
        rv = soc_th_port_asf_mode_set(unit, port,
                 speed, _SOC_TH_ASF_MODE_CFG_UPDATE);
        if ((SOC_E_NONE != rv) && (SOC_E_UNAVAIL != rv)) {
            return rv;
        }
    }
#endif

    return SOC_E_NONE;
}

int
soc_tomahawk_xpe_base_index_check(int unit, int base_type, int xpe,
                                  int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (xpe == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (xpe < NUM_XPE(unit) && si->xpe_ipipe_map[xpe] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: XPE%d is not in config\n"),
                     msg, xpe));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (xpe < NUM_XPE(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_xpe_map[pipe] : si->epipe_xpe_map[pipe];
            if (map & (1 << xpe)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in XPE%d\n"),
                         msg, base_name, base_index, xpe));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_xpe_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->epipe_xpe_map[base_index]; 
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (xpe < NUM_XPE(unit)) { /* Unique access type */
            if (map & (1 << xpe)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in XPE%d\n"),
                         msg, base_name, base_index, xpe));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* XPE */
        if (si->xpe_ipipe_map[base_index] != 0) {
            break;
        }
        if (msg != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: XPE%d is not in config\n"),
                     msg, base_index));
        }
        return SOC_E_PARAM;
    case 6: /* SLICE */
        if (si->sc_ipipe_map[base_index] == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: SLICE%d is not in config\n"),
                         msg, base_index));
        } else if (xpe < NUM_XPE(unit)) { /* Unique access type */
            if (xpe & 1) { /* XPE 1 and 3 are in slice 1 */
                if (base_index == 1) {
                    break;
                }
            } else { /* XPE 0 and 2 are in slice 0 */
                if (base_index == 0) {
                    break;
                }
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in SLICE%d\n"),
                         msg, xpe, base_index));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 7: /* LAYER */
        if (xpe & 2) { /* XPE 2 and 3 are in layer 1 */
            if (base_index == 1) {
                break;
            }
        } else { /* XPE 0 and 1 are in layer 0 */
            if (base_index == 0) {
                break;
            }
        }
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: XPE%d is not in LAYER%d\n"),
                     msg, xpe, base_index));
        }
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int soc_th_get_alpm_banks(int unit)
{
    return num_shared_alpm_banks;
}

int
soc_tomahawk_alpm_mode_get(int unit)
{
    return _soc_alpm_mode[unit];
}

int soc_tomahawk_mem_basetype_get(int unit, soc_mem_t mem)
{
    int base_type;

    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    return base_type;
}

int soc_tomahawk_mem_is_xpe(int unit, soc_mem_t mem)
{
    int block_info_index;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);

    if (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_XPE) {
        return 1;
    } else {
        return 0;
    }
}

/* Function to check if given XPE/PIPE combo matches for the given mem's base
 * type definition.
 */
int
soc_tomahawk_mem_xpe_pipe_check(int unit, soc_mem_t mem, int xpe, int pipe)
{
    int rv = SOC_E_NONE;
    int block_info_index, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_XPE) {
        return SOC_E_PARAM;
    }

    base_type = soc_tomahawk_mem_basetype_get(unit, mem);
    rv = soc_tomahawk_xpe_base_index_check(unit, base_type, xpe, pipe, NULL);
    
    if (rv == SOC_E_PARAM) {
        rv = SOC_E_UNAVAIL;
    }
    
    return rv;
}

STATIC int
_soc_tomahawk_xpe_reg_check(int unit, soc_reg_t reg, int xpe, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_XPE)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not XPE register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (xpe == -1 || xpe >= NUM_XPE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad XPE value %d\n"),
                     SOC_REG_NAME(unit, reg), xpe));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_XPE(unit)) { /* UNIQUE per XPE register */
            if (xpe != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride XPE value %d with ACC_TYPE of %s\n"),
                         xpe, SOC_REG_NAME(unit, reg)));
            }
            xpe = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_tomahawk_xpe_base_index_check(unit, base_type, xpe, base_index,
                                             SOC_REG_NAME(unit, reg));
}

#if 0
STATIC int
_soc_tomahawk_xpe_mem_check(int unit, soc_mem_t mem, int xpe, int base_index)
{
    int block_info_index, acc_type, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_XPE) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not XPE register\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) { /* UNIQUE base memory */
        if (xpe == -1 || xpe >= NUM_XPE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad XPE value %d\n"),
                     SOC_MEM_NAME(unit, mem), xpe));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        if (acc_type < NUM_XPE(unit)) { /* UNIQUE per XPE memory */
            if (xpe != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride XPE value %d with ACC_TYPE of %s\n"),
                         xpe, SOC_MEM_NAME(unit, mem)));
                xpe = acc_type;
            }
        } else { /* non-UNIQUE memory */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    return soc_tomahawk_xpe_base_index_check(unit, base_type, xpe, base_index,
                                             SOC_MEM_NAME(unit, mem));
}
#endif

/*
 * Parameters for soc_tomahawk_xpe_reg_access and soc_tomahawk_xpe_mem_access:
 *
 *    base unique  reg/  xpe   write action          read action
 *   index  type   mem
 *    ==== ====== ===== ===   ================      =================
 * #1   -1  no     -     -    all XPEs/pipes        first pipe in the first XPE
 * #2   -1  yes   base   -1   (same as #1)
 * #3   -1  yes   base  0-3   all pipes in the XPE  first pipe in the XPE
 * #4   -1  yes  xpe0-3  -    (same as #3)
 * #5  0-3  no     -     -    applicable XPEs       first XPE has the pipe
 * #6  0-3  yes   base   -1   (same as #5)
 * #7  0-3  yes   base  0-3   the XPE/pipe          the XPE/pipe
 * #8  0-3  yes  xpe0-3  -    (same as #7)
 *
 * - S/W will loop through base types (i.e. pipes) if base_index==-1
 * - S/W will loop through applicable XPEs if xpe==-1 on unique access type
 *   base view (H/W will do the loop for other access types)
 * - base_index will be ignored if the table does not have multiple sections
 *   for example ACC_TYPE==UNIQUE BASE_TYPE==XPE
 */
STATIC int
_soc_tomahawk_xpe_reg_access(int unit, soc_reg_t reg, int xpe, int base_index,
                             int index, uint64 *data, int write)
{
    soc_info_t *si;
    soc_reg_t orig_reg;
    int port;
    int base_type, break_after_first;
    int base_index_count;
    uint32 base_index_map, xpe_map=0;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    orig_reg = reg;
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;
    break_after_first = TRUE;

    if (xpe >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_xpe_reg_check(unit, reg, xpe, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        if (xpe >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[xpe];
        }

        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            /* Loop through XPE(s) only on UNIQUE type base register */
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                if (base_index < -1) {
                    return SOC_E_PARAM;
                } else if (base_index == -1) {
                    xpe_map = 0xf;
                } else {
                    xpe_map = base_type == 0 ?
                        si->ipipe_xpe_map[base_index] :
                        si->epipe_xpe_map[base_index];
                }
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
                }
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, port, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, port, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (xpe >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[xpe];
        }

        soc_tomahawk_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Loop through XPE(s) only on UNIQUE type base register */
            if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                xpe_map = base_type == 2 ?
                    si->ipipe_xpe_map[base_index] :
                    si->epipe_xpe_map[base_index];
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
                }
                inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, inst, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, inst, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* XPE */
        if (xpe >= 0 && SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
            reg = SOC_REG_UNIQUE_ACC(unit, reg)[xpe];
        }

        if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL ||
            SOC_REG_ACC_TYPE(unit, reg) < NUM_XPE(unit)) {
            base_index_map = 1;
        } else if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        /* Loop through XPE(s) only on UNIQUE type base register */
        if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
            xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
            break_after_first = FALSE;
        }

        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_REG_UNIQUE_ACC(unit, orig_reg) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    reg = SOC_REG_UNIQUE_ACC(unit, orig_reg)[xpe];
                }
                inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, reg, inst, index, *data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, reg, inst, index, data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 6: /* SLICE */
    case 7: /* LAYER */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = base_type == 6 ? si->ipipe_sc_map[0] : 0x3;
        } else {
            base_index_map = 1 << base_index;
        }

        base_index_count = base_type == 6 ? NUM_SLICE(unit) : NUM_LAYER(unit);
        for (base_index = 0; base_index < base_index_count; base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    /* No default case as base_type can have value only between 0-7 */
    }

    return SOC_E_NONE;
}
#if 0
STATIC int
_soc_tomahawk_xpe_mem_access(int unit, soc_mem_t mem, int xpe, int base_index,
                             int copyno, int offset_in_section,
                             void *entry_data, int write)
{
    soc_info_t *si;
    soc_mem_t orig_mem;
    int index, section_size;
    int base_type, break_after_first;
    uint32 base_index_map, xpe_map=0;

    si = &SOC_INFO(unit);
    orig_mem = mem;
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    section_size = SOC_MEM_INFO(unit, mem).section_size;
    break_after_first = TRUE;

    if (xpe >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_xpe_mem_check(unit, mem, xpe, base_index));
    }

    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (xpe >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[xpe];
        }

        /* All tables with such base_type should have multiple sections */
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Loop through XPE(s) only on UNIQUE type base memory */
            if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                xpe_map = base_type == 2 ?
                    si->ipipe_xpe_map[base_index] :
                    si->epipe_xpe_map[base_index];
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    mem = SOC_MEM_UNIQUE_ACC(unit, orig_mem)[xpe];
                }
                index = base_index * section_size + offset_in_section;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, copyno, index, entry_data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, copyno, index, entry_data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 4: /* CHIP */
        index = offset_in_section;
        if (write) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, copyno, index, entry_data));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, copyno, index, entry_data));
        }
        break;
    case 5: /* XPE */
        if (xpe >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[xpe];
        }

        if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL ||
            SOC_MEM_ACC_TYPE(unit, mem) < NUM_XPE(unit)) {
            /* Single section table handling for unique acc_type */
            base_index_map = 1;
        } else if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        /* Loop through XPE(s) only on UNIQUE type base memory */
        if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            xpe_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
            break_after_first = FALSE;
        }

        for (base_index = 0; base_index < NUM_XPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                    if (!(xpe_map & (1 << xpe))) {
                        continue;
                    }
                    mem = SOC_MEM_UNIQUE_ACC(unit, orig_mem)[xpe];
                }
                index = base_index * section_size + offset_in_section;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, copyno, index, entry_data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, copyno, index, entry_data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 6: /* SLICE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_sc_map[0];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            index = base_index * section_size + offset_in_section;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, copyno, index, entry_data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, copyno, index, entry_data));
            }
        }
        break;
    case 0: /* IPORT */
    case 1: /* EPORT */
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
#endif
int
soc_tomahawk_sc_base_index_check(int unit, int base_type, int sc,
                                 int base_index, char *msg)
{
    soc_info_t *si;
    int pipe;
    uint32 map;
    char *base_name;

    si = &SOC_INFO(unit);

    if (sc == -1 || base_index == -1) {
        return SOC_E_NONE;
    }

    if (sc < NUM_SLICE(unit) && si->sc_ipipe_map[sc] == 0) {
        if (msg) {
            LOG_CLI((BSL_META_U(unit,
                                "%s: SC%d is not in config\n"),
                     msg, sc));
        }
        return SOC_E_PARAM;
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        base_name = base_type == 0 ? "IPORT" : "EPORT";
        pipe = si->port_pipe[base_index];
        if (pipe == -1) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sc < NUM_SLICE(unit)) { /* Unique access type */
            map = base_type == 0 ?
                si->ipipe_sc_map[pipe] : si->epipe_sc_map[pipe];
            if (map & (1 << sc)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SC%d\n"),
                         msg, base_name, base_index, sc));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (base_type == 2) {
            base_name = "IPIPE";
            map = si->ipipe_sc_map[base_index];
        } else {
            base_name = "EPIPE";
            map = si->epipe_sc_map[base_index]; 
        }
        if (map == 0) {
            if (msg) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in config\n"),
                         msg, base_name, base_index));
            }
        } else if (sc < NUM_SLICE(unit)) { /* Unique access type */
            if (map & (1 << sc)) {
                break;
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: %s%d is not in SC%d\n"),
                         msg, base_name, base_index, sc));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 4: /* CHIP */
        break;
    case 5: /* XPE */
        if (si->xpe_ipipe_map[base_index] == 0) {
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in config\n"),
                         msg, base_index));
            }
        } else if (sc < NUM_SLICE(unit)) { /* Unique access type */
            if (sc == 0) { /* XPE 0 and 2 are in slice 0 */
                if (base_index == 0 || base_index == 2) {
                    break;
                }
            } else { /* XPE 1 and 3 are in slice 1 */
                if (base_index == 1 || base_index == 3) {
                    break;
                }
            }
            if (msg != NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s: XPE%d is not in SLICE%d\n"),
                         msg, base_index, sc));
            }
        } else {
            break;
        }
        return SOC_E_PARAM;
    case 6: /* SLICE */
    case 7: /* LAYER, not used */
        break;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_sc_reg_check(int unit, soc_reg_t reg, int sc, int base_index)
{
    int acc_type, base_type;

    if (!SOC_BLOCK_IN_LIST(SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU_SC)) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not SC register\n"), SOC_REG_NAME(unit, reg)));
        return SOC_E_PARAM;
    }

    if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) { /* UNIQUE base register */
        if (sc == -1 || sc >= NUM_SLICE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad SC value %d\n"),
                     SOC_REG_NAME(unit, reg), sc));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_REG_ACC_TYPE(unit, reg);
        if (acc_type < NUM_SLICE(unit)) { /* UNIQUE per SC register */
            if (sc != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride SC value %d with ACC_TYPE of %s\n"),
                         sc, SOC_REG_NAME(unit, reg)));
            }
            sc = acc_type;
        } else { /* non-UNIQUE register */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    return soc_tomahawk_sc_base_index_check(unit, base_type, sc, base_index,
                                            SOC_REG_NAME(unit, reg));
}

#if 0
STATIC int
_soc_tomahawk_sc_mem_check(int unit, soc_mem_t mem, int sc, int base_index)
{
    int block_info_index, acc_type, base_type;

    block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
    if (SOC_BLOCK_TYPE(unit, block_info_index) != SOC_BLK_MMU_SC) {
        LOG_CLI((BSL_META_U(unit,
                            "%s is not SC register\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    if (SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) { /* UNIQUE base memory */
        if (sc == -1 || sc >= NUM_SLICE(unit)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s bad SC value %d\n"),
                     SOC_MEM_NAME(unit, mem), sc));
            return SOC_E_PARAM;
        }
    } else {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
        if (acc_type < NUM_SLICE(unit)) { /* UNIQUE per SC memory */
            if (sc != acc_type) {
                LOG_CLI((BSL_META_U(unit,
                                    "Ovveride SC value %d with ACC_TYPE of %s\n"),
                         sc, SOC_MEM_NAME(unit, mem)));
                sc = acc_type;
            }
        } else { /* non-UNIQUE memory */
            return SOC_E_NONE;
        }
    }
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;

    return soc_tomahawk_sc_base_index_check(unit, base_type, sc, base_index,
                                            SOC_MEM_NAME(unit, mem));
}
#endif

STATIC int
_soc_tomahawk_sc_reg_access(int unit, soc_reg_t reg, int sc, int base_index,
                            int index, uint64 *data, int write)
{
    soc_info_t *si;
    int port;
    int base_type;
    uint32 base_index_map;
    soc_pbmp_t base_index_pbmp;
    uint32 inst;

    si = &SOC_INFO(unit);
    base_type = ((SOC_REG_INFO(unit, reg).offset) >> 23) & 0x7;

    if (sc >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_sc_reg_check(unit, reg, sc, base_index));
    }

    switch (base_type) {
    case 0: /* IPORT */
    case 1: /* EPORT */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            SOC_PBMP_ASSIGN(base_index_pbmp, PBMP_ALL(unit));
        } else {
            /* This argument is logical port, same as soc_reg_get/set */
            SOC_PBMP_PORT_SET(base_index_pbmp, base_index);
        }

        SOC_PBMP_ITER(base_index_pbmp, port) {
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, index, data));
            }
        }
        break;
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        /* No unique access type for such base_type */
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 4: /* CHIP */
        if (write) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, 0, index, *data));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, 0, index, data));
        }
        break;
    case 5: /* XPE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 6: /* SLICE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_sc_map[0];;
        } else {
            base_index_map = 1 << base_index;
        }
        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            inst = base_index | SOC_REG_ADDR_INSTANCE_MASK;
            if (write) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst, index, *data));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, inst, index, data));
            }
        }
        break;
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
#if 0
STATIC int
_soc_tomahawk_sc_mem_access(int unit, soc_mem_t mem, int sc, int base_index,
                            int copyno, int offset_in_section, void *entry_data,
                            int write)
{
    soc_info_t *si;
    soc_mem_t orig_mem;
    int index, section_size;
    int base_type, break_after_first;
    uint32 base_index_map, sc_map=0;

    si = &SOC_INFO(unit);
    orig_mem = mem;
    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    section_size = SOC_MEM_INFO(unit, mem).section_size;
    break_after_first = TRUE;

    if (sc >= 0 && base_index >= 0) {
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_sc_mem_check(unit, mem, sc, base_index));
    }

    switch (base_type) {
    case 2: /* IPIPE */
    case 3: /* EPIPE */
        if (sc >= 0 && SOC_MEM_UNIQUE_ACC(unit, mem) != NULL) {
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[sc];
        }

        /* All tables with such base_type should have multiple sections */
        soc_tomahawk_pipe_map_get(unit, &base_index_map);
        if (base_index != -1) {
            base_index_map &= 1 << base_index;
            if (base_index_map == 0) {
                return SOC_E_PARAM;
            }
        }

        for (base_index = 0; base_index < NUM_PIPE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            /* Loop through SC(s) only on UNIQUE type base memory */
            if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                sc_map = base_type == 2 ?
                    si->ipipe_sc_map[base_index] :
                    si->epipe_sc_map[base_index];
                if (write) {
                    break_after_first = FALSE;
                }
            }
            for (sc = 0; sc < NUM_SLICE(unit); sc++) {
                if (SOC_MEM_UNIQUE_ACC(unit, orig_mem) != NULL) {
                    if (!(sc_map & (1 << sc))) {
                        continue;
                    }
                    mem = SOC_MEM_UNIQUE_ACC(unit, orig_mem)[sc];
                }
                index = base_index * section_size + offset_in_section;
                if (write) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, copyno, index, entry_data));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, copyno, index, entry_data));
                }
                if (break_after_first) {
                    break;
                }
            }
        }
        break;
    case 5: /* XPE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_xpe_map[0] | si->ipipe_xpe_map[1];
        } else {
            base_index_map = 1 << base_index;
        }

        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            index = base_index * section_size + offset_in_section;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, copyno, index, entry_data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, copyno, index, entry_data));
            }
        }
        break;
    case 6: /* SLICE */
        /* No unique access type for such base_type */
        if (base_index == -1) {
            base_index_map = si->ipipe_sc_map[0];
        } else {
            base_index_map = 1 << base_index;
        }
        for (base_index = 0; base_index < NUM_SLICE(unit); base_index++) {
            if (!(base_index_map & (1 << base_index))) {
                continue;
            }
            index = base_index * section_size + offset_in_section;
            if (write) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, copyno, index, entry_data));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, copyno, index, entry_data));
            }
        }
        break;
    case 0: /* IPORT */
    case 1: /* EPORT */
    case 4: /* CHIP */
    case 7: /* LAYER */
    default:
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}
#endif
int
soc_tomahawk_xpe_reg32_set(int unit, soc_reg_t reg, int xpe, int base_index,
                           int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                                     index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_tomahawk_xpe_reg32_get(int unit, soc_reg_t reg, int xpe, int base_index,
                           int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                                     index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_tomahawk_xpe_reg_set(int unit, soc_reg_t reg, int xpe, int base_index,
                         int index, uint64 data)
{
    return _soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                        index, &data, TRUE);
}

int
soc_tomahawk_xpe_reg_get(int unit, soc_reg_t reg, int xpe, int base_index,
                         int index, uint64 *data)
{
    return _soc_tomahawk_xpe_reg_access(unit, reg, xpe, base_index,
                                        index, data, FALSE);
}
#if 0
int
soc_tomahawk_xpe_mem_write(int unit, soc_mem_t mem, int xpe, int base_index,
                           int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_xpe_mem_access(unit, mem, xpe, base_index, copyno,
                                        offset_in_section, entry_data, TRUE);
}

int
soc_tomahawk_xpe_mem_read(int unit, soc_mem_t mem, int xpe, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_xpe_mem_access(unit, mem, xpe, base_index, copyno,
                                        offset_in_section, entry_data, FALSE);
}
#endif
int
soc_tomahawk_sc_reg32_set(int unit, soc_reg_t reg, int sc, int base_index,
                          int index, uint32 data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                                    index, &data64, TRUE));

    return SOC_E_NONE;
}

int
soc_tomahawk_sc_reg32_get(int unit, soc_reg_t reg, int sc, int base_index,
                          int index, uint32 *data)
{
    uint64 data64;

    COMPILER_64_SET(data64, 0, *data);
    SOC_IF_ERROR_RETURN(_soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                                    index, &data64, FALSE));
    *data = COMPILER_64_LO(data64);

    return SOC_E_NONE;
}

int
soc_tomahawk_sc_reg_set(int unit, soc_reg_t reg, int sc, int base_index,
                        int index, uint64 data)
{
    return _soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                       index, &data, TRUE);
}

int
soc_tomahawk_sc_reg_get(int unit, soc_reg_t reg, int sc, int base_index,
                        int index, uint64 *data)
{
    return _soc_tomahawk_sc_reg_access(unit, reg, sc, base_index,
                                       index, data, FALSE);
}
#if 0
int
soc_tomahawk_sc_mem_write(int unit, soc_mem_t mem, int sc, int base_index,
                          int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_sc_mem_access(unit, mem, sc, base_index, copyno,
                                       offset_in_section, entry_data, TRUE);
}

int
soc_tomahawk_sc_mem_read(int unit, soc_mem_t mem, int sc, int base_index,
                         int copyno, int offset_in_section, void *entry_data)
{
    return _soc_tomahawk_sc_mem_access(unit, mem, sc, base_index, copyno,
                                       offset_in_section, entry_data, FALSE);
}
#endif

#if defined(BCM_TH_ASF_EXCLUDE)
int
soc_tomahawk_cut_through_update(int unit, soc_port_t port, int enable)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    int pipe, class, index;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int *start_count;
    static int start_count_saf[13] =
        { 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18 };
    static int start_count_linerate[13] =
        { 18, 27, 27, 21, 21, 18, 18, 12, 12, 12, 12, 15, 15 };
    static int start_count_oversub_2_1[13] =
        { 18, 57, 57, 51, 51, 45, 45, 39, 39, 33, 33, 33, 33 };
    static int start_count_oversub_3_2[13] =
        { 18, 45, 45, 39, 39, 36, 36, 27, 27, 24, 24, 24, 24 };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    if (enable) {
        if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            pipe = si->port_pipe[port];
            if (tdm->ovs_ratio_x1000[pipe] > 1500) {
                /* More than 3:2 oversubscription ratio */
                start_count = start_count_oversub_2_1;
            } else {
                start_count = start_count_oversub_3_2;
            }
        } else {
            start_count = start_count_linerate;
        }
    } else {
        start_count = start_count_saf;
    }

    sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
    for (class = 0; class < 13; class++) {
        soc_mem_field32_set(unit, EGR_XMIT_START_COUNTm, entry, THRESHOLDf,
                            start_count[class]);
        index = (port % 34) * 16 + class;
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_XMIT_START_COUNTm(unit, MEM_BLOCK_ALL, index, entry));
    }

    return SOC_E_NONE;
}
#endif

STATIC int
_soc_tomahawk_clear_all_memory(int unit)
{
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg;
    int pipe, port, index, count;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;
    static const struct {
        soc_mem_t mem;
        uint32 skip_flags; /* always skip on QUICKTURN or XGSSIM */
    } cam_list[] = {
        { CPU_COS_MAPm,                     BOOT_F_PLISIM },
        { DST_COMPRESSIONm,                 BOOT_F_PLISIM },
        { EFP_TCAMm,                        BOOT_F_PLISIM },
        { EXACT_MATCH_LOGICAL_TABLE_SELECTm, BOOT_F_PLISIM },
        { IFP_LOGICAL_TABLE_SELECTm,        BOOT_F_PLISIM },
        { IFP_TCAMm,                        BOOT_F_PLISIM },
        { ING_SNATm,                        BOOT_F_PLISIM },
        { IP_MULTICAST_TCAMm,               BOOT_F_PLISIM },
        { L2_USER_ENTRYm,                   BOOT_F_PLISIM },
        { L3_DEFIPm,                        BOOT_F_PLISIM },
        { L3_DEFIP_PAIR_128m,               BOOT_F_PLISIM },
        { L3_TUNNELm,                       BOOT_F_PLISIM },
        { MY_STATION_TCAMm,                 BOOT_F_PLISIM },
        { SRC_COMPRESSIONm,                 BOOT_F_PLISIM },
        { UDF_CONDITIONAL_CHECK_TABLE_CAMm, BOOT_F_PLISIM },
        { VFP_TCAMm,                        BOOT_F_PLISIM },
        { VLAN_SUBNETm,                     0 },/* VLAN API needs all 0 mask */
    };

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    /* Initial IPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries of largest IPIPE table */
    count = soc_mem_index_count(unit, ING_L3_NEXT_HOPm);
    if (count < soc_mem_index_count(unit, L2Xm)) {
        count = soc_mem_index_count(unit, L2Xm);
    }
    if (count < soc_mem_index_count(unit, L3_ENTRY_ONLYm)) {
        count = soc_mem_index_count(unit, L3_ENTRY_ONLYm);
    }
    if (count < soc_mem_index_count(unit, FPEM_ECCm)) {
        count = soc_mem_index_count(unit, FPEM_ECCm);
    }
    if (count < soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m)) {
        count = soc_mem_index_count(unit, L3_DEFIP_ALPM_IPV4m);
    }
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* Initial EPIPE memory */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table (EGR_L3_NEXT_HOP) */
    count = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                    if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                        in_progress ^= 1 << pipe;
                    }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    /* Wait for EPIPE memory initialization done */
    in_progress = pipe_map;
    do {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV && in_progress; pipe++) {
            reg = SOC_REG_UNIQUE_ACC(unit, EGR_HW_RESET_CONTROL_1r)[pipe];
            if (in_progress & (1 << pipe)) { /* not done yet */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                    if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                        in_progress ^= 1 << pipe;
                    }
            }
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (in_progress != 0);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* Initial IDB memory */
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, 0));
    rval = 0;
    soc_reg_field_set(unit, IDB_HW_CONTROLr, &rval, IS_MEM_INITf, 1);
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_IDB_HW_CONTROLr(unit, 0));

    /* Initial MMU memory */
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));
    rval = 0;
    soc_reg_field_set(unit, MMU_GCFG_MISCCONFIGr, &rval, INIT_MEMf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_MMU_GCFG_MISCCONFIGr(unit, 0));

    /* Initial PORT MIB counter */
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        soc_reg_field_set(unit, CLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, 0));
    }
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));
    }

    /* TCAM tables are not handled by hardware reset control */
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
        for (index = 0; index < COUNTOF(cam_list); index++) {
            if (sal_boot_flags_get() & cam_list[index].skip_flags) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, cam_list[index].mem, COPYNO_ALL, TRUE));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, idb_port;
    int num_port, num_phy_port;
    soc_reg_t reg;
    soc_mem_t mem;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe;

    /*
     * 96 entries MMU_CHFC_SYSPORT_MAPPINGm.SYS_PORT
     * 256 entries SYS_PORTMAPm.DEVICE_PORT_NUMBER
     */

    si = &SOC_INFO(unit);

    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;
    num_phy_port = 136;

    /* Ingress physical to device port mapping */
    sal_memset(&entry, 0,
               sizeof(ing_idb_to_device_port_number_mapping_table_entry_t));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            pipe = si->port_pipe[port];
            idb_port = si->port_l2i_mapping[port];
        } else if (phy_port >= 132) { /* loopback port */
            pipe = phy_port - 132;
            idb_port = 33;
        } else if (phy_port == 129) { /* management port 0 (if not in use) */
            pipe = 1;
            idb_port = 32;
        } else if (phy_port == 130) { /* undefined physical port */
            pipe = 3;
            idb_port = 32;
        } else if (phy_port == 131) { /* management port 1 (if not in use) */
            pipe = 2;
            idb_port = 32;
        } else {
            pipe = (phy_port - 1) / _TH_PORTS_PER_PIPE;
            idb_port = (phy_port -1 ) % _TH_PORTS_PER_PIPE;
        }
        mem = SOC_MEM_UNIQUE_ACC
            (unit, ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm)[pipe];
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf,
                            port == -1 ? 0xff : port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, idb_port, &entry));
    }

    /* Ingress GPP port to device port mapping */
    mem = SYS_PORTMAPm;
    sal_memset(&entry, 0, sizeof(sys_portmap_entry_t));
    for (port = 0; port < num_port; port++) {
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &entry));
    }

    /* Ingress device port to GPP port mapping
     * PORT_TAB.SRC_SYS_PORT_ID is programmed in the general port config
     * init routine _bcm_fb_port_cfg_init()
     */

    /* Egress device port to physical port mapping */
    rval = 0;
    reg = EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, PHYSICAL_PORT_NUMBERf,
                          si->port_l2p_mapping[port]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to device port mapping */
    rval = 0;
    reg = MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, DEVICE_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to physical port mapping */
    rval = 0;
    reg = MMU_PORT_TO_PHY_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, PHY_PORTf,
                          si->port_l2p_mapping[port]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    /* MMU port to system port mapping */
    rval = 0;
    reg = MMU_PORT_TO_SYSTEM_PORT_MAPPINGr;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, reg, &rval, SYSTEM_PORTf, port);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_idb_calendar_set(int unit,  int calendar_id)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t field;
    int pipe, slot, id, length;
    int port, phy_port, idb_port;
    uint32 rval, entry[SOC_MAX_MEM_WORDS];
    static const soc_mem_t calendar_mems[] = {
        IS_TDM_CALENDAR0m, IS_TDM_CALENDAR1m
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        for (length = _TDM_LENGTH; length > 0; length--) {
            if (tdm->idb_tdm[pipe][length - 1] != _TH_NUM_EXT_PORTS) {
                break;
            }
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_TDM_CONFIGr)[pipe];
        if (calendar_id == -1) { /* choose "the other one" */
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, calendar_mems[calendar_id])[pipe];
        field = calendar_end_fields[calendar_id];
        rval = 0;
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < length; slot += 2) {
            phy_port = tdm->idb_tdm[pipe][slot];
            id = -1;
            if (phy_port == OVSB_TOKEN) {
                idb_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == IDL1_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == IDL2_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                idb_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= _TH_NUM_EXT_PORTS) {
                idb_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                port = si->port_p2l_mapping[phy_port];
                idb_port = si->port_l2i_mapping[port];
                id = si->port_serdes[port];
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_EVENf, idb_port);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_EVENf, id & 0xf);
            phy_port = tdm->idb_tdm[pipe][slot + 1];
            id = -1;
            if (phy_port == OVSB_TOKEN) {
                idb_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == IDL1_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == IDL2_TOKEN) {
                idb_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                idb_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= _TH_NUM_EXT_PORTS) {
                idb_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                port = si->port_p2l_mapping[phy_port];
                idb_port = si->port_l2i_mapping[port];
                id = si->port_serdes[port];
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_ODDf, idb_port);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_ODDf, id & 0xf);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
            if (tdm->idb_tdm[pipe][slot + 2] == _TH_NUM_EXT_PORTS) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  phy_port == _TH_NUM_EXT_PORTS ? slot : slot + 1);
                break;
            }
        }
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_idb_oversub_group_set(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg;
    int pipe, clport, group, lane, slot, id, mode, ovs_class;
    int port, phy_port, phy_port_base, idb_port;
    int speed_max;
    uint32 rval;
    static const soc_reg_t grp_tbl_regs[] = {
        IS_OVR_SUB_GRP0_TBLr, IS_OVR_SUB_GRP1_TBLr,
        IS_OVR_SUB_GRP2_TBLr, IS_OVR_SUB_GRP3_TBLr,
        IS_OVR_SUB_GRP4_TBLr, IS_OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t pblk_calendar_regs[] ={
        IS_PBLK0_CALENDARr, IS_PBLK1_CALENDARr,
        IS_PBLK2_CALENDARr, IS_PBLK3_CALENDARr,
        IS_PBLK4_CALENDARr, IS_PBLK5_CALENDARr,
        IS_PBLK6_CALENDARr, IS_PBLK7_CALENDARr
    };
    static int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            reg = SOC_REG_UNIQUE_ACC(unit, grp_tbl_regs[group])[pipe];
            rval = 0;
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->idb_ovs_tdm[pipe][group][slot];
                if (phy_port >= _TH_NUM_EXT_PORTS) {
                    idb_port = 0x3f;
                    id = -1;
                } else {
                    port = si->port_p2l_mapping[phy_port];
                    idb_port = si->port_l2i_mapping[port];
                    id = si->port_serdes[port];
                }
                soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id & 0x7);
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
            }
        }
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            phy_port = tdm->idb_ovs_tdm[pipe][group][0];
            if (phy_port >= _TH_NUM_EXT_PORTS) {
                continue;
            }
            port = si->port_p2l_mapping[phy_port];
            speed_max = 25000 * si->port_num_lanes[port];
            if (speed_max > si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
            _soc_tomahawk_speed_to_ovs_class_mapping(unit, speed_max,
                                                     &ovs_class);
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, group, rval));
        }
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);

        mode = tdm->port_ratio[clport];
        reg = SOC_REG_UNIQUE_ACC(unit, pblk_calendar_regs[clport & 0x7])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
        soc_reg_field_set(unit, reg, &rval, SPACINGf, 4);
        for (slot = 0; slot < 7; slot++) {
            lane = pblk_slots[mode][slot];
            if (lane == -1) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, 0));
                continue;
            }
            port = si->port_p2l_mapping[phy_port_base + lane];
            idb_port = si->port_l2i_mapping[port];
            soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_idb_opportunistic_set(int unit, int enable)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_CPU_LB_OPP_CFGr)[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        reg = SOC_REG_UNIQUE_ACC(unit, IS_OPP_SCHED_CFGr)[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_idb_hsp_set(int unit)
{
    soc_info_t *si;
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    int port, idb_port;
    uint32 port_map[_TH_PIPES_PER_DEV];
    uint32 rval;

    si = &SOC_INFO(unit);

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        port_map[pipe] = 0;
    }
    SOC_PBMP_ITER(si->eq_pbm, port) {
        pipe = si->port_pipe[port];
        idb_port = si->port_l2i_mapping[port];
        port_map[pipe] |= 1 << idb_port;
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        reg = SOC_REG_UNIQUE_ACC(unit, IS_TDM_HSPr)[pipe];
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_mmu_calendar_set(int unit, int calendar_id)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t field;
    int pipe, slot, id, length;
    int port, phy_port, mmu_port, inst;
    uint32 rval, entry[SOC_MAX_MEM_WORDS];
    static const soc_mem_t calendar_mems[] = {
        TDM_CALENDAR0m, TDM_CALENDAR1m
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        for (length = _TDM_LENGTH; length > 0; length--) {
            if (tdm->mmu_tdm[pipe][length - 1] != _TH_NUM_EXT_PORTS) {
                break;
            }
        }

        reg = TDM_CONFIGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        if (calendar_id == -1) { /* choose "the other one" */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, calendar_mems[calendar_id])[pipe];
        field = calendar_end_fields[calendar_id];
        rval = 0;
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < length; slot += 2) {
            phy_port = tdm->mmu_tdm[pipe][slot];
            id = -1;
            if (phy_port == OVSB_TOKEN) {
                mmu_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == IDL1_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == IDL2_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                mmu_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= _TH_NUM_EXT_PORTS) {
                mmu_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                mmu_port = si->port_p2m_mapping[phy_port];
                port = si->port_p2l_mapping[phy_port];
                id = si->port_serdes[port];
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_EVENf,
                                mmu_port & 0x3f);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_EVENf, id & 0xf);
            phy_port = tdm->mmu_tdm[pipe][slot + 1];
            id = -1;
            if (phy_port == OVSB_TOKEN) {
                mmu_port = _SOC_TH_TDM_OVERSUB_TOKEN;
            } else if (phy_port == IDL1_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL1_TOKEN;
            } else if (phy_port == IDL2_TOKEN) {
                mmu_port = _SOC_TH_TDM_IDL2_TOKEN;
            } else if (phy_port == NULL_TOKEN) {
                mmu_port = _SOC_TH_TDM_NULL_TOKEN;
            } else if (phy_port >= _TH_NUM_EXT_PORTS) {
                mmu_port = _SOC_TH_TDM_UNUSED_TOKEN;
            } else {
                mmu_port = si->port_p2m_mapping[phy_port];
                port = si->port_p2l_mapping[phy_port];
                id = si->port_serdes[port];
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_ODDf,
                                mmu_port & 0x3f);
            soc_mem_field32_set(unit, mem, entry, PHY_PORT_ID_ODDf, id & 0xf);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
            if (tdm->mmu_tdm[pipe][slot + 2] == _TH_NUM_EXT_PORTS) {
                soc_reg_field_set(unit, reg, &rval, field,
                                  phy_port == _TH_NUM_EXT_PORTS ? slot : slot + 1);
                break;
            }
        }
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_mmu_oversub_group_set(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    uint32 pipe_map;
    int block_info_idx;
    soc_reg_t reg;
    int pipe, clport, group, lane, slot, id, mode, ovs_class;
    int port, phy_port, phy_port_base, mmu_port, inst;
    int speed_max;
    uint32 rval;
    static const soc_reg_t grp_tbl_regs[] = {
        OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
        OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
        OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t pblk_calendar_regs[] = {
        PBLK0_CALENDARr, PBLK1_CALENDARr, PBLK2_CALENDARr, PBLK3_CALENDARr,
        PBLK4_CALENDARr, PBLK5_CALENDARr, PBLK6_CALENDARr, PBLK7_CALENDARr
    };
    static const int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            reg = grp_tbl_regs[group];
            rval = 0;
            for (slot = 0; slot < _OVS_GROUP_TDM_LENGTH; slot++) {
                phy_port = tdm->mmu_ovs_tdm[pipe][group][slot];
                if (phy_port >= _TH_NUM_EXT_PORTS) {
                    mmu_port = 0x3f;
                    id = -1;
                } else {
                    mmu_port = si->port_p2m_mapping[phy_port];
                    port = si->port_p2l_mapping[phy_port];
                    id = si->port_serdes[port];
                }
                soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id & 0x7);
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf,
                                  mmu_port & 0x3f);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
            }
        }
    }

    reg = OVR_SUB_GRP_CFGr;
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            phy_port = tdm->mmu_ovs_tdm[pipe][group][0];
            if (phy_port >= _TH_NUM_EXT_PORTS) {
                continue;
            }
            port = si->port_p2l_mapping[phy_port];
            speed_max = 25000 * si->port_num_lanes[port];
            if (speed_max > si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
            _soc_tomahawk_speed_to_ovs_class_mapping(unit, speed_max,
                                                     &ovs_class);
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
                              speed_max >= 40000 ? 4 : 8);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, inst, group, rval));
        }
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        speed_max = 25000 * si->port_num_lanes[port];
        if (speed_max > si->port_speed_max[port]) {
            speed_max = si->port_speed_max[port];
        }
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        mode = tdm->port_ratio[clport];
        reg = pblk_calendar_regs[clport & 0x7];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, VALIDf, 1);
        soc_reg_field_set(unit, reg, &rval, SPACINGf,
                          speed_max >= 40000 ? 4 : 8);
        for (slot = 0; slot < 7; slot++) {
            lane = pblk_slots[mode][slot];
            if (lane == -1) {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, 0));
                continue;
            }
            mmu_port = si->port_p2m_mapping[phy_port_base + lane];
            soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_mmu_opportunistic_set(int unit, int enable)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int inst;
    uint32 rval;

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = CPU_LB_OPP_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));

        reg = OPP_SCHED_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf, enable ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_mmu_hsp_set(int unit)
{
    soc_info_t *si;
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int port, phy_port, mmu_port, inst;
    uint32 port_map[_TH_PIPES_PER_DEV];
    uint32 rval;

    si = &SOC_INFO(unit);

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        port_map[pipe] = 0;
    }
    SOC_PBMP_ITER(si->eq_pbm, port) {
        pipe = si->port_pipe[port];
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        port_map[pipe] |= 1 << (mmu_port & 0x1f);
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = TDM_HSPr;
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_tdm_calculation(int unit, _soc_tomahawk_tdm_t *tdm)
{
    soc_info_t *si;
    int port, phy_port;
    int pipe, clport, lane, group, block_info_idx, index;
    int length, ovs_core_slot_count, ovs_io_slot_count, port_slot_count;
    int mode[_TH_PIPES_PER_DEV];
    soc_pbmp_t pbmp;

    si = &SOC_INFO(unit);

    sal_memset(tdm, 0, sizeof(_soc_tomahawk_tdm_t));

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        SOC_PBMP_ASSIGN(pbmp, si->pipe_pbm[pipe]);
        SOC_PBMP_AND(pbmp, PBMP_HG_ALL(unit));
        SOC_PBMP_REMOVE(pbmp, si->oversub_pbm);
        mode[pipe] = SOC_PBMP_NOT_NULL(pbmp) ? PORT_HIGIG2 : PORT_ETHERNET;
    }

    for (index = 0; index < COUNTOF(tdm->pm_encap_type); index++) {
        tdm->pm_encap_type[index] = PORT_ETHERNET;
    }
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        phy_port = si->port_l2p_mapping[port];
        tdm->port_state[phy_port] =
            SOC_PBMP_MEMBER(si->oversub_pbm, port) ?
            PORT_STATE_OVERSUB : PORT_STATE_LINERATE;
        for (lane = 1; lane < si->port_num_lanes[port]; lane++) {
            tdm->port_state[phy_port + lane] = PORT_STATE_CONTINUATION;
        }
        tdm->speed[phy_port] = si->port_speed_max[port];
    }

    /* CLPORT */
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        soc_tomahawk_port_ratio_get(unit, clport, &tdm->port_ratio[clport]);
        tdm->pm_encap_type[clport] = mode[si->port_pipe[port]];
    }

    /* Management ports */
    SOC_PBMP_ITER(si->management_pbm, port) {
        phy_port = si->port_l2p_mapping[port];
        tdm->pm_encap_type[_TH_PBLKS_PER_DEV] = mode[si->port_pipe[port]];
        tdm->speed[phy_port] = si->port_speed_max[port];
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_CLI((BSL_META_U(unit,
                            "frequency: %dMHz\n"), si->frequency));
        LOG_CLI((BSL_META_U(unit,
                            "port speed:")));
        for (index = 0; index < _TH_NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %6d"), tdm->speed[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        LOG_CLI((BSL_META_U(unit,
                            "port state map:")));
        for (index = 0; index < _TH_NUM_EXT_PORTS; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            if (index == 0 || index == (_TH_NUM_EXT_PORTS - 1)) {
                LOG_CLI((BSL_META_U(unit,
                                    " ---")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), tdm->port_state[index]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
        LOG_CLI((BSL_META_U(unit,
                            "pm encap type:")));
        for (index = 0; index < _TH_NUM_PORT_MODULES; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                " %3d"), tdm->pm_encap_type[index]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
	
    for (index = 0; index < _TH_NUM_EXT_PORTS; index++) {
        tdm->tdm_globals.speed[index] = tdm->speed[index];
    }
    tdm->tdm_globals.clk_freq = si->frequency;
    for (index = 1; index < _TH_NUM_EXT_PORTS; index++) {
        tdm->tdm_globals.port_rates_array[index-1] = tdm->port_state[index];
    }
    for (index = 1; index < _TH_NUM_PORT_MODULES; index++) {
        tdm->tdm_globals.pm_encap_type[index] = tdm->pm_encap_type[index];
    }
	
    if ( TH_set_tdm_tbl(&tdm->tdm_globals, &tdm->tdm_pipe_tables) == 0 ) {
        LOG_CLI((BSL_META_U(unit,
                            "Unable to configure TDM, please contact your "
                            "Field Applications Engineer or Sales Manager for "
                            "additional assistance.\n")));
        return SOC_E_FAIL;
    } else {
        tdm->idb_tdm[0] = tdm->tdm_pipe_tables.idb_tdm_tbl_0;
        tdm->idb_ovs_tdm[0][0] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_a;
        tdm->idb_ovs_tdm[0][1] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_b;
        tdm->idb_ovs_tdm[0][2] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_c;
        tdm->idb_ovs_tdm[0][3] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_d;
        tdm->idb_ovs_tdm[0][4] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_e;
        tdm->idb_ovs_tdm[0][5] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_f;
      /*tdm->idb_ovs_tdm[0][6] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_g;
        tdm->idb_ovs_tdm[0][7] = tdm->tdm_pipe_tables.idb_tdm_ovs_0_h;*/
        tdm->idb_tdm[1] = tdm->tdm_pipe_tables.idb_tdm_tbl_1;
        tdm->idb_ovs_tdm[1][0] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_a;
        tdm->idb_ovs_tdm[1][1] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_b;
        tdm->idb_ovs_tdm[1][2] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_c;
        tdm->idb_ovs_tdm[1][3] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_d;
        tdm->idb_ovs_tdm[1][4] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_e;
        tdm->idb_ovs_tdm[1][5] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_f;
      /*tdm->idb_ovs_tdm[1][6] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_g;
        tdm->idb_ovs_tdm[1][7] = tdm->tdm_pipe_tables.idb_tdm_ovs_1_h;*/
        tdm->idb_tdm[2] = tdm->tdm_pipe_tables.idb_tdm_tbl_2;
        tdm->idb_ovs_tdm[2][0] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_a;
        tdm->idb_ovs_tdm[2][1] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_b;
        tdm->idb_ovs_tdm[2][2] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_c;
        tdm->idb_ovs_tdm[2][3] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_d;
        tdm->idb_ovs_tdm[2][4] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_e;
        tdm->idb_ovs_tdm[2][5] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_f;
      /*tdm->idb_ovs_tdm[2][6] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_g;
        tdm->idb_ovs_tdm[2][7] = tdm->tdm_pipe_tables.idb_tdm_ovs_2_h;*/
        tdm->idb_tdm[3] = tdm->tdm_pipe_tables.idb_tdm_tbl_3;
        tdm->idb_ovs_tdm[3][0] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_a;
        tdm->idb_ovs_tdm[3][1] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_b;
        tdm->idb_ovs_tdm[3][2] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_c;
        tdm->idb_ovs_tdm[3][3] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_d;
        tdm->idb_ovs_tdm[3][4] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_e;
        tdm->idb_ovs_tdm[3][5] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_f;
      /*tdm->idb_ovs_tdm[3][6] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_g;
        tdm->idb_ovs_tdm[3][7] = tdm->tdm_pipe_tables.idb_tdm_ovs_3_h;*/
        tdm->mmu_tdm[0] = tdm->tdm_pipe_tables.mmu_tdm_tbl_0;
        tdm->mmu_ovs_tdm[0][0] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_a;
        tdm->mmu_ovs_tdm[0][1] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_b;
        tdm->mmu_ovs_tdm[0][2] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_c;
        tdm->mmu_ovs_tdm[0][3] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_d;
        tdm->mmu_ovs_tdm[0][4] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_e;
        tdm->mmu_ovs_tdm[0][5] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_f;
      /*tdm->mmu_ovs_tdm[0][6] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_g;
        tdm->mmu_ovs_tdm[0][7] = tdm->tdm_pipe_tables.mmu_tdm_ovs_0_h;*/
        tdm->mmu_tdm[1] = tdm->tdm_pipe_tables.mmu_tdm_tbl_1;
        tdm->mmu_ovs_tdm[1][0] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_a;
        tdm->mmu_ovs_tdm[1][1] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_b;
        tdm->mmu_ovs_tdm[1][2] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_c;
        tdm->mmu_ovs_tdm[1][3] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_d;
        tdm->mmu_ovs_tdm[1][4] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_e;
        tdm->mmu_ovs_tdm[1][5] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_f;
      /*tdm->mmu_ovs_tdm[1][6] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_g;
        tdm->mmu_ovs_tdm[1][7] = tdm->tdm_pipe_tables.mmu_tdm_ovs_1_h;*/
        tdm->mmu_tdm[2] = tdm->tdm_pipe_tables.mmu_tdm_tbl_2;
        tdm->mmu_ovs_tdm[2][0] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_a;
        tdm->mmu_ovs_tdm[2][1] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_b;
        tdm->mmu_ovs_tdm[2][2] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_c;
        tdm->mmu_ovs_tdm[2][3] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_d;
        tdm->mmu_ovs_tdm[2][4] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_e;
        tdm->mmu_ovs_tdm[2][5] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_f;
      /*tdm->mmu_ovs_tdm[2][6] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_g;
        tdm->mmu_ovs_tdm[2][7] = tdm->tdm_pipe_tables.mmu_tdm_ovs_2_h;*/
        tdm->mmu_tdm[3] = tdm->tdm_pipe_tables.mmu_tdm_tbl_3;
        tdm->mmu_ovs_tdm[3][0] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_a;
        tdm->mmu_ovs_tdm[3][1] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_b;
        tdm->mmu_ovs_tdm[3][2] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_c;
        tdm->mmu_ovs_tdm[3][3] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_d;
        tdm->mmu_ovs_tdm[3][4] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_e;
        tdm->mmu_ovs_tdm[3][5] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_f;
      /*tdm->mmu_ovs_tdm[3][6] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_g;
        tdm->mmu_ovs_tdm[3][7] = tdm->tdm_pipe_tables.mmu_tdm_ovs_3_h;*/
	}

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        /* Count number of OVSB_TOKEN assigned by the TDM code */
        for (length = _TDM_LENGTH; length > 0; length--) {
            if (tdm->mmu_tdm[pipe][length - 1] != _TH_NUM_EXT_PORTS) {
                break;
            }
        }
        ovs_core_slot_count = 0;
        for (index = 0; index < length; index++) {
            if (tdm->mmu_tdm[pipe][index] == OVSB_TOKEN) {
                ovs_core_slot_count++;
            }
        }

        /* Count number of slot needed for the oversub I/O bandwidth */
        ovs_io_slot_count = 0;
        for (group = 0; group < _OVS_GROUP_COUNT; group++) {
            port_slot_count = 0;
            for (index = 0; index < _OVS_GROUP_TDM_LENGTH; index++) {
                phy_port = tdm->mmu_ovs_tdm[pipe][group][index];
                if (phy_port == _TH_NUM_EXT_PORTS) {
                    continue;
                }
                if (port_slot_count == 0) {
                    _soc_tomahawk_speed_to_slot_mapping
                        (unit, tdm->speed[phy_port], &port_slot_count);
                }
                ovs_io_slot_count += port_slot_count;
            }
        }

        if (ovs_core_slot_count != 0) {
            tdm->ovs_ratio_x1000[pipe] =
                ovs_io_slot_count * 1000 / ovs_core_slot_count;
        }
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
            LOG_CLI((BSL_META_U(unit,
                                "Pipe %d idb_tdm:"), pipe));
            for (index = 0; index < _TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), tdm->idb_tdm[pipe][index]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            for (group = 0; group < _OVS_GROUP_COUNT; group++) {
                LOG_CLI((BSL_META_U(unit,
                                    "Pipe %d group %d idb_ovs_tdm"), pipe, group));
                for (index = 0; index < _OVS_GROUP_TDM_LENGTH; index++) {
                    if (index % 16 == 0) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        " %3d"), tdm->idb_ovs_tdm[pipe][group][index]));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }

            LOG_CLI((BSL_META_U(unit,
                                "Pipe %d mmu_tdm:"), pipe));
            for (index = 0; index < _TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %3d"), tdm->mmu_tdm[pipe][index]));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            for (group = 0; group < _OVS_GROUP_COUNT; group++) {
                LOG_CLI((BSL_META_U(unit,
                                    "Pipe %d group %d mmu_ovs_tdm"),
                         pipe, group));
                for (index = 0; index < _OVS_GROUP_TDM_LENGTH; index++) {
                    if (index % 16 == 0) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        " %3d"), tdm->mmu_ovs_tdm[pipe][group][index]));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "Pipe %d oversubscribe ratio %d.%-2d\n"),
                     pipe, tdm->ovs_ratio_x1000[pipe] / 1000,
                     tdm->ovs_ratio_x1000[pipe] % 1000));
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_hash_init(int unit)
{
    soc_field_t fields[4];
    uint32 values[4];

    /* L2 dedicated banks */
    fields[0] = BANK0_HASH_OFFSETf;
    values[0] = 0;  /* CRC32_LOWER */
    fields[1] = BANK1_HASH_OFFSETf;
    values[1] = 16; /* CRC32_UPPER */
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_TABLE_HASH_CONTROLr, REG_PORT_ANY, 2,
                                 fields, values));

    /* L3 dedicated banks */
    fields[0] = BANK6_HASH_OFFSETf;
    values[0] = 0;  /* CRC32_LOWER */
    fields[1] = BANK7_HASH_OFFSETf;
    values[1] = 8;  /* CRC32 >> 8 */
    fields[2] = BANK8_HASH_OFFSETf;
    values[2] = 16; /* CRC32_UPPER */
    fields[3] = BANK9_HASH_OFFSETf;
    values[3] = 32; /* CRC16 */
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_TABLE_HASH_CONTROLr, REG_PORT_ANY, 4,
                                 fields, values));

    /* L2/L3/ALPM shared banks */
    fields[0] = BANK2_HASH_OFFSETf;
    values[0] = 4;  /* CRC32 >> 4 */
    fields[1] = BANK3_HASH_OFFSETf;
    values[1] = 12; /* CRC32 >> 12 */
    fields[2] = BANK4_HASH_OFFSETf;
    values[2] = 20; /* CRC32 >> 20 */
    fields[3] = BANK5_HASH_OFFSETf;
    values[3] = 24; /* CRC32 >> 24 */
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, SHARED_TABLE_HASH_CONTROLr,
                                 REG_PORT_ANY, 4, fields, values));

    

    return SOC_E_NONE;
}

#define _TH_BYTES_PER_OBM_CELL    16
#define _TH_CELLS_PER_OBM         1012

STATIC int
_soc_tomahawk_idb_init(int unit)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    soc_reg_t reg;
    soc_field_t field;
    int block_info_idx, pipe, clport, obm, lane;
    int port, phy_port_base;
    int lossless;
    uint32 rval0, rval1, fval;
    uint64 rval64;
    int num_lanes, oversub_ratio_idx;
    static const struct _obm_setting_s {
        int discard_limit;
        int lossless_xoff;
        int lossy_only_lossy_low_pri;
        int port_xoff[3];          /* for oversub ratio 1.5, 1.8, and above */
        int lossy_low_pri[3];      /* for oversub ratio 1.5, 1.8, and above */
        int lossy_discard[3];      /* for oversub ratio 1.5, 1.8, and above */
    } obm_settings[] = { /* indexed by number of lanes */
        { /* 0 - invalid  */ 0 },
        { /* 1 lane */
            _TH_CELLS_PER_OBM / 4, 45, 157,
            { 129,  87,  67 }, /* port_xoff */
            { 100,  76,  56 }, /* lossy_low_pri */
            { 196, 152, 112 }  /* lossy_discard */
        },
        { /* 2 lanes */
            _TH_CELLS_PER_OBM / 2, 108, 410,
            { 312, 248, 215 }, /* port_xoff */
            { 100, 100, 100 }, /* lossy_low_pri */
            { 196, 196, 196 }  /* lossy_discard */
        },
        { /* 3 - invalid  */ 0 },
        { /* 4 lanes */
            _TH_CELLS_PER_OBM, 258, 916,
            { 682, 572, 517 }, /* port_xoff */
            { 100, 100, 100 }, /* lossy_low_pri */
            { 196, 196, 196 }  /* lossy_discard */
        }
    };
    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr,
        IDB_OBM2_CA_CONTROLr, IDB_OBM3_CA_CONTROLr,
        IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr
    };
    static const soc_reg_t obm_thresh_regs[] = {
        IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr
    };
    static const soc_reg_t obm_fc_thresh_regs[] = {
        IDB_OBM0_FC_THRESHOLDr, IDB_OBM1_FC_THRESHOLDr,
        IDB_OBM2_FC_THRESHOLDr, IDB_OBM3_FC_THRESHOLDr,
        IDB_OBM4_FC_THRESHOLDr, IDB_OBM5_FC_THRESHOLDr,
        IDB_OBM6_FC_THRESHOLDr, IDB_OBM7_FC_THRESHOLDr
    };
    static const soc_reg_t obm_shared_regs[] = {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr, IDB_OBM5_SHARED_CONFIGr,
        IDB_OBM6_SHARED_CONFIGr, IDB_OBM7_SHARED_CONFIGr
    };
    static const soc_field_t obm_bypass_fields[] = {
        PORT0_BYPASS_ENABLEf, PORT1_BYPASS_ENABLEf,
        PORT2_BYPASS_ENABLEf, PORT3_BYPASS_ENABLEf
    };
    static const soc_field_t obm_oversub_fields[] = {
        PORT0_OVERSUB_ENABLEf, PORT1_OVERSUB_ENABLEf,
        PORT2_OVERSUB_ENABLEf, PORT3_OVERSUB_ENABLEf
    };
    static const int hw_mode_values[SOC_TH_PORT_RATIO_COUNT] = {
        0, 1, 1, 1, 4, 6, 3, 5, 2
    };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);
        pipe = si->port_pipe[port];
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        /* obm number is reversed (mirrored) in odd pipe */
        obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;

        /* Set cell assembly mode then toggle reset to send initial credit
         * to EP */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ca_ctrl_regs[obm])[pipe];
        rval0 = 0;
        soc_reg_field_set(unit, reg, &rval0, PORT_MODEf,
                          hw_mode_values[tdm->port_ratio[clport]]);
        rval1 = rval0;
        soc_reg_field_set(unit, reg, &rval0, PORT0_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT1_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT2_RESETf, 1);
        soc_reg_field_set(unit, reg, &rval0, PORT3_RESETf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval1));

        if (!SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            continue;
        }

        /* Enable oversub */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_ctrl_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval0));
        for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] == -1) {
                continue;
            }
            field = obm_bypass_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
            field = obm_oversub_fields[lane];
            soc_reg_field_set(unit, reg, &rval0, field, 1);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));

        /* Configure shared config */
        reg = SOC_REG_UNIQUE_ACC(unit, obm_shared_regs[obm])[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, DISCARD_THRESHOLDf, 1023);
        soc_reg64_field32_set(unit, reg, &rval64, SOP_THRESHOLDf, 1023);
        soc_reg64_field32_set(unit, reg, &rval64, SOP_DISCARD_MODEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

        if (lossless) {
            if (tdm->ovs_ratio_x1000[pipe] >= 1800) { /* ratio >= 1.8 */
                oversub_ratio_idx = 2;
            } else if (tdm->ovs_ratio_x1000[pipe] >= 1500) { /* ratio >= 1.5 */
                oversub_ratio_idx = 1;
            } else {
                oversub_ratio_idx = 0;
            }

            for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      1023);
                fval = obm_settings[num_lanes].lossy_low_pri[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, fval);
                fval = obm_settings[num_lanes].lossy_discard[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].port_xoff[oversub_ratio_idx];
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, fval - 10);
                fval = obm_settings[num_lanes].lossless_xoff;
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf,
                                      fval - 10);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
            }
        } else { /* lossy */
            for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
                port = si->port_p2l_mapping[phy_port_base + lane];
                if (port == -1) {
                    continue;
                }
                num_lanes = si->port_num_lanes[port];

                /* Configure threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                fval = obm_settings[num_lanes].discard_limit;
                soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, fval);
                /* same as DISCARD_LIMIT setting */
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, fval);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, 1023);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));

                /* Configure flow control threshold */
                reg = SOC_REG_UNIQUE_ACC(unit, obm_fc_thresh_regs[obm])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, REG_PORT_ANY, lane, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, 1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf,
                                      1023);
                soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf, 1023);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, REG_PORT_ANY, lane, rval64));
            }
        }
    }

    /* Toggle cpu port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_CPU_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));

    /* Toggle loopback port cell assembly reset to send initial credit to EP */
    reg = IDB_CA_LPBK_CONTROLr;
    rval0 = 0;
    soc_reg_field_set(unit, reg, &rval0, PORT_RESETf, 1);
    PBMP_LB_ITER(unit, port) {
        reg = SOC_REG_UNIQUE_ACC(unit, IDB_CA_LPBK_CONTROLr)
            [si->port_pipe[port]];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
    }

    return SOC_E_NONE;
}

extern int 
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int 
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

STATIC int
_soc_tomahawk_tscx_firmware_set(int unit, int port, uint8 *array, int datalen)
{
    soc_mem_t mem = IS_CL_PORT(unit, port) ? 
                    CLPORT_WC_UCMEM_DATAm : XLPORT_WC_UCMEM_DATAm;
    soc_reg_t reg = IS_CL_PORT(unit, port) ? 
                    CLPORT_WC_UCMEM_CTRLr : XLPORT_WC_UCMEM_CTRLr;

    return soc_warpcore_firmware_set(unit, port, array, datalen, 0, mem, reg);
}

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

STATIC int
_soc_tomahawk_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset;
    
    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }
    
    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to Physical port 1 to 24
     * bus 1 phy 1 to 16 are mapped to Physical port 25 to 40
     * bus 2 phy 1 to 24 are mapped to Physical port 41 to 84
     * bus 3 phy 1 to 24 are mapped to Physical port 85 to 88
     * bus 4 phy 1 to 16 are mapped to Physical port 89 to 104
     * bus 5 phy 1 to 24 are mapped to Physical port 105 to 128
     * bus 6 phy 1 is mapped to Physical port 129 and 
     *       phy 3 is mapped to Physical port 131
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 6) {
        return 0;
    }
    switch (bus) {
    case 0: offset = 0;
        break;
    case 1: offset = 24;
        break;
    case 2: offset = 40;
        break;
    case 3: offset = 84;
        break;
    case 4: offset = 88;
        break;
    case 5: offset = 104;
        break;
    case 6: offset = 128;
        if ((phy_addr & 0x1f) == 2 || (phy_addr & 0x1f) > 3) {
            return 0;
        }
        break;
    default: 
        return 0;
    }
    
    return (phy_addr & 0x1f) + offset;
}

STATIC int
_soc_tomahawk_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_tomahawk_mdio_addr_to_port(phy_addr);
    
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_tomahawk_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr, 
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_tomahawk_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_tomahawk_mdio_addr_to_port(phy_addr);
    
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_tomahawk_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr, 
                                phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_th_ledup_init(int unit)
{
    uint32 rval = 0;
    int ix, pval1, pval2, pval_size;

    typedef struct led_remap_s {
        uint32 reg_addr;
        uint32 port0;
        uint32 port1;
        uint32 port2;
        uint32 port3;
    } led_remap_t;

    led_remap_t led0_remap[] = {
        { CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f },
        { CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f }
    };

    led_remap_t led1_remap[] = {
        { CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f }, 
        { CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f },
        { CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f }
    };

    led_remap_t led2_remap[] = {
        { CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r,
         REMAP_PORT_0f, REMAP_PORT_1f, REMAP_PORT_2f, REMAP_PORT_3f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r,
         REMAP_PORT_4f, REMAP_PORT_5f, REMAP_PORT_6f, REMAP_PORT_7f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r,
         REMAP_PORT_8f, REMAP_PORT_9f, REMAP_PORT_10f, REMAP_PORT_11f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r,
         REMAP_PORT_12f, REMAP_PORT_13f, REMAP_PORT_14f, REMAP_PORT_15f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r,
         REMAP_PORT_16f, REMAP_PORT_17f, REMAP_PORT_18f, REMAP_PORT_19f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r,
         REMAP_PORT_20f, REMAP_PORT_21f, REMAP_PORT_22f, REMAP_PORT_23f }, 
        { CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r,
         REMAP_PORT_24f, REMAP_PORT_25f, REMAP_PORT_26f, REMAP_PORT_27f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r,
         REMAP_PORT_28f, REMAP_PORT_29f, REMAP_PORT_30f, REMAP_PORT_31f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r,
         REMAP_PORT_32f, REMAP_PORT_33f, REMAP_PORT_34f, REMAP_PORT_35f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r,
         REMAP_PORT_36f, REMAP_PORT_37f, REMAP_PORT_38f, REMAP_PORT_39f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r,
         REMAP_PORT_40f, REMAP_PORT_41f, REMAP_PORT_42f, REMAP_PORT_43f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r,
         REMAP_PORT_44f, REMAP_PORT_45f, REMAP_PORT_46f, REMAP_PORT_47f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r,
         REMAP_PORT_48f, REMAP_PORT_49f, REMAP_PORT_50f, REMAP_PORT_51f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r,
         REMAP_PORT_52f, REMAP_PORT_53f, REMAP_PORT_54f, REMAP_PORT_55f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r,
         REMAP_PORT_56f, REMAP_PORT_57f, REMAP_PORT_58f, REMAP_PORT_59f },
        { CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r,
         REMAP_PORT_60f, REMAP_PORT_61f, REMAP_PORT_62f, REMAP_PORT_63f }
    };


    pval_size = COUNTOF(led0_remap);
    pval2 = (pval_size >> 1) - 1;

    /* Configure LED-0 remap register settings.
     * LED-0 chain provides port status for tile0 (ports 1-32) 
     * and tile 3 (ports 97-128).
     * Scan chain order: 32, 31, ...., 1, 128, 127, ...., 97
     * LED port status for port 32 arrives first at the CMIC block and 
     * port 97's LED status arrives last.
     */
    for (ix = 0; ix < pval_size ; ix++) {
        if (ix == pval_size >> 1) {
            pval2 = pval_size -1;
        }
 
        pval1 = ix << 2;
 
        rval = 0;
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port3, pval1);
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port2, pval1 + 1);
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port1, pval1 + 2);
        soc_reg_field_set(unit, led0_remap[pval2].reg_addr, &rval,
                led0_remap[pval2].port0, pval1 + 3);
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit, soc_reg_addr(unit, 
                    led0_remap[pval2].reg_addr, REG_PORT_ANY, 0), rval));
        pval2--;
    }

    /* Configure LED-1 remap register settings.
     * LED-1 chain provides port status for tile1 (ports 33-64) 
     * and tile 2 (ports 65-96).
     * Scan chain order: 33, 34, 35, ...., 96
     * LED port status for port 33 arrives first at the CMIC block and 
     * port 96's LED status arrives last.
     */
    for (ix = 0; ix < pval_size ; ix++) {
        pval1 = ix << 2;

        rval = 0;
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port0, pval1);
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port1, pval1 + 1);
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port2, pval1 + 2);
        soc_reg_field_set(unit, led1_remap[ix].reg_addr, &rval,
                led1_remap[ix].port3, pval1 + 3);
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit, soc_reg_addr(unit, 
                    led1_remap[ix].reg_addr, REG_PORT_ANY, 0), rval));
    }

    /* Configure LED-2 remap register settings. 
     * LED-2 chain provides port status for management ports (129-132).
     * Scan chain port order is - 132 (unused), 131, 130 (unused), 129.
     * Unused remapping port registers are programmed to a larger value.
     */
    for (ix = 0; ix < pval_size ; ix++) {
        rval = 0;
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port0, 0x3F);
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port1, 0x3F);
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port2, 0x3F);
        soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                led2_remap[ix].port3, 0x3F);

        if (ix == 0) {
            soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                    led2_remap[ix].port1, 1);
            soc_reg_field_set(unit, led2_remap[ix].reg_addr, &rval,
                    led2_remap[ix].port3, 0);
        }
        
        SOC_IF_ERROR_RETURN
                (soc_pci_write(unit, soc_reg_addr(unit, 
                    led2_remap[ix].reg_addr, REG_PORT_ANY, 0), rval));
    }
 
    /* initialize the LED processors data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_DATA_RAMr(unit,ix, rval));
        SOC_IF_ERROR_RETURN
            (soc_pci_write(unit,
                soc_reg_addr(unit, CMIC_LEDUP2_DATA_RAMr, REG_PORT_ANY, ix),
                rval));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_misc_init(int unit)
{
    soc_control_t *soc;
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    int block_info_idx;
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 rval, fval;
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int clport, lane, mode, class, index;
    int port, phy_port_base;
    int parity_enable;
    static soc_field_t port_fields[_TH_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    static const int mode_encodings[SOC_TH_PORT_RATIO_COUNT] = {
        4, 3, 3, 3, 2, 2, 1, 1, 0
    };

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);
    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {
#ifdef TH_SER
            /* Certain mems/regs need to be cleared before enabling SER */
            SOC_IF_ERROR_RETURN(_soc_tomahawk_clear_mmu_memory(unit, INVALIDm));
            SOC_IF_ERROR_RETURN(_soc_tomahawk_clear_all_port_data(unit));
#endif
        }
        
        /* SOC_IF_ERROR_RETURN(soc_tomahawk_tcam_ser_init(unit)); */
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, TRUE));
        soc_tomahawk_ser_register(unit);
#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        soc_th_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/
    }

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_tomahawk_clear_all_memory(unit));
    }

    _soc_tomahawk_port_mapping_init(unit);
    if (!SAL_BOOT_XGSSIM) {
        if (soc->tdm_info == NULL) {
            soc->tdm_info = sal_alloc(sizeof(_soc_tomahawk_tdm_t),
                                      "Tomahakw TDM info");
            if (soc->tdm_info == NULL) {
                return SOC_E_MEMORY;
            }
        }
        tdm = soc->tdm_info;

        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation(unit, tdm));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_calendar_set(unit, 0));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_oversub_group_set(unit));
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_tdm_idb_opportunistic_set(unit, TRUE));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_hsp_set(unit));

        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_mmu_calendar_set(unit, 0));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_mmu_oversub_group_set(unit));
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_tdm_mmu_opportunistic_set(unit, TRUE));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_mmu_hsp_set(unit));
        _soc_tomahawk_idb_init(unit);
    }

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(device_loopback_ports_bitmap_entry_t));
    soc_mem_pbmp_field_set(unit, DEVICE_LOOPBACK_PORTS_BITMAPm, entry, BITMAPf,
                           &PBMP_LB(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, DEVICE_LOOPBACK_PORTS_BITMAPm, MEM_BLOCK_ALL, 0,
                       entry));

    PBMP_LB_ITER(unit, port) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MULTIPASS_LOOPBACK_BITMAPm)
            [si->port_pipe[port]];
        sal_memset(entry, 0, sizeof(multipass_loopback_bitmap_entry_t));
        SOC_PBMP_PORT_SET(pbmp, port);
        soc_mem_pbmp_field_set(unit, mem, &entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry));
    }

    sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, si->cpu_hg_index,
                       entry));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 2);
    PBMP_LB_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }

    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        clport = SOC_BLOCK_NUMBER(unit, block_info_idx);
        phy_port_base = PORT_BLOCK_BASE_PORT(port);

        /* Assert CLPORT soft reset */
        rval = 0;
        for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode */
        soc_tomahawk_port_ratio_get(unit, clport, &mode);
        rval = 0;
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          mode_encodings[mode]);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          mode_encodings[mode]);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable CLPORT */
        rval = 0;
        for (lane = 0; lane < _TH_PORTS_PER_PBLK; lane++) {
            if (si->port_p2l_mapping[phy_port_base + lane] != -1) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval,
                                  port_fields[lane], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_ENABLE_REGr(unit, port, rval));
    }

    SOC_PBMP_ITER(si->management_pbm, port) {
        /* Assert XLPORT soft reset */
        rval = 0;
        if (si->port_p2l_mapping[129] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[131] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode to DUAL */
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          3);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          3);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        if (si->port_p2l_mapping[129] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[131] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
    }

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(_soc_tomahawk_hash_init(unit));

    /* Configure EP credit */
    reg = EGR_MMU_CELL_CREDITr;
    rval = 0;
    PBMP_ALL_ITER(unit, port) {
        if (si->port_speed_max[port] >= 100000 || IS_LB_PORT(unit, port)) {
            fval = 44;
        } else if (si->port_speed_max[port] >= 50000) {
            fval = 27;
        } else if (si->port_speed_max[port] >= 40000) {
            fval = 25;
        } else if (si->port_speed_max[port] >= 25000) {
            fval = 16;
        } else if (si->port_speed_max[port] >= 20000) {
            fval = 18;
        } else {
            fval = 13;
        }
        soc_reg_field_set(unit, reg, &rval, CREDITf, fval);
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITr(unit, port, rval));
    }

    /* Configure egress transmit start count */
    mem = EGR_XMIT_START_COUNTm;
    sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
    soc_mem_field32_set(unit, EGR_XMIT_START_COUNTm, entry, THRESHOLDf, 18);
    PBMP_ALL_ITER(unit, port) {
        for (class = 0; class < 13; class++) {
            index = (port % 34) * 16 + class;
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_XMIT_START_COUNTm(unit, MEM_BLOCK_ALL, index,
                                             entry));
        }
    }

    /* Enable egress */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));
    }

    sal_memset(entry, 0, sizeof(epc_link_bmap_entry_t));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf,
                           &PBMP_CMIC(unit));
    soc_mem_field32_set(unit, EPC_LINK_BMAPm, entry, ENABLE_SOBMH_BLOCKINGf,
                        soc_property_get(unit, "epc_sobmh_block", 1));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_CONFIG_1r, REG_PORT_ANY, RING_MODEf,
                                1));

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              soc_mem_index_count(unit, L2MCm) / 2),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              soc_mem_index_count(unit, L2MCm) / 2)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_mcast_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L2_RANGE, soc_mem_index_count(unit, L2MCm) / 2)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L3_RANGE, soc_mem_index_count(unit, L2MCm) / 2)));


    SOC_IF_ERROR_RETURN(_soc_th_ledup_init(unit));

    
    _phy_tsce_firmware_set_helper[unit] = _soc_tomahawk_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = _soc_tomahawk_tscx_firmware_set;

    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        }
    }

    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));
    return SOC_E_NONE;
}

/* 
 * Function used for index calculation of certain memories,
 * where index is derived from more than one key.
 * Eg. (Port, SP) or (Port, PG)
 * Defined as Macro: SOC_TH_MMU_PIPED_MEM_INDEX
 * Note: If no special indexig required, return the same index.
 */
uint32 _soc_th_piped_mem_index(int unit, soc_port_t port,
                               soc_mem_t mem, int arr_off)
{
    int     mmu_port, index;

    mmu_port = SOC_TH_MMU_PORT(unit, port);

    switch (mem) {
        case MMU_THDM_DB_PORTSP_CONFIGm:
        case MMU_THDM_MCQE_PORTSP_CONFIGm:
            index = (_TH_PORTS_PER_PIPE * arr_off) + (mmu_port & 0x3f);
            break;
        case THDI_PORT_SP_CONFIGm:
        case MMU_THDU_CONFIG_PORTm:
        case MMU_THDU_RESUME_PORTm:
            index = ((mmu_port & 0x3f) * _TH_MMU_NUM_POOL) + arr_off;
            break;
        case THDI_PORT_PG_CONFIGm:
            index = ((mmu_port & 0x3f) * _TH_MMU_NUM_PG) + arr_off;
            break;
        default:
            return arr_off;
    }

    return index;
}

STATIC void
_soc_th_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
                            int lossless)
{
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _TH_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _TH_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _TH_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _TH_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _TH_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _TH_MMU_TOTAL_CELLS_PER_XPE;
    devcfg->num_pg = _TH_MMU_NUM_PG;
    devcfg->num_service_pool = _TH_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _TH_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = 11;
}

STATIC int
_soc_th_default_pg_headroom(int unit, soc_port_t port,
                            int lossless)
{
    int speed = 1000, hdrm = 0;
    uint8 port_oversub = 0;

    if (IS_CPU_PORT(unit, port)) {
        return 77;
    } else if (!lossless) {
        return 0;
    } else if (IS_LB_PORT(unit, port)) {
        return 150;
    }

    speed = SOC_INFO(unit).port_speed_max[port];

    if (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port)) {
        port_oversub = 1;
    }
    if ((speed >= 1000) && (speed < 10000)) {
        hdrm = port_oversub ? 189 : 166;
    } else if ((speed >= 10000) && (speed < 20000)) {
        hdrm = port_oversub ? 189 : 166;
    } else if ((speed >= 20000) && (speed <= 42000)) {
        hdrm = port_oversub ? 340 : 286;
    } else if (speed >= 100000) {
        hdrm = port_oversub ? 648 : 519;
    } else {
        hdrm = port_oversub ? 189 : 166;
    }
    return hdrm;
}

/*
 * Default MMU values: Version 4.0
 */
STATIC void
_soc_th_mmu_config_buf_default(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int max_packet_cells, default_mtu_cells;
    int port, idx, per_q_guarentee;
    int total_pool_size = 0, egr_shared_total = 0;
    int in_reserved = 0;
    uint32 color_limit = 0;
#if 0
    int mcq_entry_shared_total, jumbo_frame_cells, per_q_mcq_entry_guarantee = 0;
#endif
    int rqe_entry_shared_total;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config (u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
#if 0
    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
#endif
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */

    buf->headroom = 2 * max_packet_cells;

    in_reserved += buf->headroom;

    per_q_guarentee = ((lossless) ? 0 :8);
#if 0
    per_q_mcq_entry_guarantee = ((lossless) ? 0 :8);
#endif

    egr_shared_total = total_pool_size - ((lossless) ? 88 : 0);

    rqe_entry_shared_total = _TH_MMU_TOTAL_RQE_ENTRY(unit) - 88;

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_mcq_entry = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_rqe_entry = rqe_entry_shared_total;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
        }
    }

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
    for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        queue_grp->guarantee = 0;
        /* resume limits - accounted for 8 cell granularity */
        queue_grp->pool_resume = 16;
        queue_grp->yellow_resume = 16;
        queue_grp->red_resume = 16;
        if (lossless) {
            queue_grp->pool_limit = egr_shared_total;
            queue_grp->discard_enable = 0;
            queue_grp->pool_scale = -1;
            queue_grp->red_limit = egr_shared_total;
            queue_grp->yellow_limit = egr_shared_total;
        } else {
            queue_grp->pool_limit = 0;
            queue_grp->discard_enable = 1;
            queue_grp->pool_scale = 8;
            queue_grp->red_limit = color_limit;
            queue_grp->yellow_limit = color_limit;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < 16; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == 0) {
                buf_port_pool->pool_limit = total_pool_size;
                buf_port_pool->pool_resume =
                            total_pool_size - 16;
            }
            in_reserved += buf_port_pool->guarantee;
        }

        buf_port->pkt_size = max_packet_cells;

        /* priority group */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->pool_scale = -1;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == 7) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->flow_control_enable = lossless;
                if (lossless) {
                    buf_prigroup->guarantee = default_mtu_cells;
                    buf_prigroup->headroom =
                        _soc_th_default_pg_headroom(unit, port, lossless);
                    buf_prigroup->pool_scale = 8;
                    /* resume limits - accounted for 8 cell granularity */
                    buf_prigroup->pool_resume = 16;
                }
            }
            in_reserved += buf_prigroup->guarantee + buf_prigroup->headroom;
        }

        /* multicast queue */
        color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            buf_queue->qgroup_id = -1;
            buf_queue->mcq_entry_guarantee = 4;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            } else {
                buf_queue->guarantee = per_q_guarentee;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = color_limit;
                buf_queue->red_limit = color_limit;
                buf_queue->color_discard_enable = 1;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
            }
        }

        /* unicast queue */
        color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            buf_queue->qgroup_id = -1;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                buf_queue->guarantee = per_q_guarentee;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = color_limit;
                buf_queue->yellow_limit = color_limit;
                buf_queue->color_discard_enable = 1;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        buf_prigroup = &buf_port->prigroups[7];
        if (!lossless) {
            buf_prigroup->pool_limit = total_pool_size - in_reserved;
        }
    }

    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        buf_rqe_queue->pool_idx = 0;
        buf_rqe_queue->red_limit = egr_shared_total;
        buf_rqe_queue->yellow_limit = egr_shared_total;
        if (lossless) {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 0;
            buf_rqe_queue->pool_scale = -1;
            buf_rqe_queue->pool_limit = egr_shared_total;
        } else {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 1;
            buf_rqe_queue->pool_scale = 7;
            buf_rqe_queue->pool_limit = 0;
        }
    }

}

/* Function to get the number of ports present in a given Port Macro */
STATIC int
soc_th_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port_base, num_ports = 0;

    si = &SOC_INFO(unit);
    if (pm_id >= _TH_PBLKS_PER_DEV) {
       return SOC_E_PARAM;
    }
    phy_port_base = 1 + (pm_id * _TH_PORTS_PER_PBLK);

    if (si->port_p2l_mapping[phy_port_base] != -1) {
        num_ports = 1;
        if ((si->port_num_lanes[phy_port_base] == 2) &&
            (si->port_p2l_mapping[phy_port_base + 2] != -1)) {
            /* for cases when num_lanes is given in the config (40g).
             */
            num_ports = 2;
        }

        /* When phy_port_base + 1 is valid,
         *      All 4 ports of the PM(port macro) are valid.
         * When phy_port_base + 2 is valid,
         *      The PM is operating in Dual lane mode
         */
        if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
            num_ports = 4;
            if (si->port_p2l_mapping[phy_port_base + 2] == -1) {
                num_ports = 2;
            }
        } else if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
            num_ports = 2;
        }
    }
    return num_ports;
}

/* Function to reserve cells for CutThru(CT) and Flex ports
 */
STATIC int
soc_th_mmu_additional_buffer_reserve(int unit, int pipe)
{
    /* array indexed by num_ports/pm 1, 2, 4 */
    int similar_speed_asf_cells[] = {42, 42, 42};
    int extreme_speed_asf_cells[] = {42, 48, 56};
    int hdrm_lossless_cells[] = {648, 346, 209};
    int zero_cells[] = {0, 0, 0};
    int *rsvd_asf_cells_p, *headroom_cells_p;
    int id, min_pm_id = 0, max_pm_id = 0;
    static int lossless[SOC_MAX_NUM_DEVICES] = {-2};
    /* asf_profile: 0 - No ASF, 1 - Similar Speed, 2 - Extreme Speed ASF */
    static int asf_profile[SOC_MAX_NUM_DEVICES];
    int pg_min_cells = 0, qgrp_min_cells = 0, hdrm_cells = 0, asf_cells = 0;
    int total_reserve = 0, num_ports = 0;

    if (pipe >= NUM_PIPE(unit)) {
        return SOC_E_PARAM;
    }

    if (lossless[0] == -2) {
        sal_memset(lossless, -1, sizeof(lossless));
        sal_memset(asf_profile, -1, sizeof(asf_profile));
    }

    if (lossless[unit] == -1) {
        lossless[unit] = soc_property_get(unit, spn_MMU_LOSSLESS, 1);
    }
    if (asf_profile[unit] == -1) {
        asf_profile[unit] = soc_property_get(unit, spn_ASF_MEM_PROFILE, 1);
    }

    rsvd_asf_cells_p = zero_cells;
    if (asf_profile[unit] == 1) {
        rsvd_asf_cells_p = similar_speed_asf_cells;
    } else if (asf_profile[unit] == 2) {
        rsvd_asf_cells_p = extreme_speed_asf_cells;
    }

    headroom_cells_p = zero_cells;
    if (lossless[unit] != 0) {
        headroom_cells_p = hdrm_lossless_cells;
    }

    min_pm_id = pipe * _TH_PBLKS_PER_PIPE;
    max_pm_id = min_pm_id + _TH_PBLKS_PER_PIPE;

    for (id = min_pm_id; id < max_pm_id; id++) {
        num_ports = soc_th_ports_per_pm_get(unit, id);
        switch (num_ports) {
            case 1:
                hdrm_cells = headroom_cells_p[0];
                asf_cells = rsvd_asf_cells_p[0];
                break;
            case 2:
                hdrm_cells = headroom_cells_p[1];
                asf_cells = rsvd_asf_cells_p[1];
                break;
            case 4:
                hdrm_cells = headroom_cells_p[2];
                asf_cells = rsvd_asf_cells_p[2];
                break;
            default:
                return SOC_E_PARAM;
        }
        pg_min_cells = (lossless[unit]) ? 8 : 0;
        qgrp_min_cells = 16;
        total_reserve += (hdrm_cells + asf_cells + pg_min_cells +
                          qgrp_min_cells) * num_ports;
    }

    return total_reserve;
}

/*
 * Default MMU values: Version 4.0
 */
STATIC void
_soc_th_mmu_config_buf_default_flex_port(int unit, _soc_mmu_cfg_buf_t *buf,
                                         _soc_mmu_device_info_t *devcfg,
                                         int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int max_packet_cells, default_mtu_cells;
    int port, idx, per_q_guarentee;
    int total_pool_size = 0, egr_shared_total = 0;
    int in_reserved = 0, total_reserved = 0;
    int lossless_pg = 7;
    uint32 color_limit = 0;
    
    int rqe_entry_shared_total;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config(with FlexPort\
                            (u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    total_pool_size = devcfg->mmu_total_cell; /* per XPE limit */

    for (idx = 0; idx < NUM_PIPE(unit); idx++) {
        total_reserved += soc_th_mmu_additional_buffer_reserve(unit, idx);
    }
    /* total_reserve is for all pipes. Each XPE operates on a pair of pipes.
     * Hence total_reserved = total_reserved / 2 for each XPE
     */
    total_reserved /= 2;

    buf->headroom = 2 * max_packet_cells;

    in_reserved += buf->headroom;

    per_q_guarentee = ((lossless) ? 0 :8);

    egr_shared_total = total_pool_size - ((lossless) ? 88 : 0) - total_reserved;

    rqe_entry_shared_total = _TH_MMU_TOTAL_RQE_ENTRY(unit) - 88;

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_mcq_entry = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->total_rqe_entry = rqe_entry_shared_total;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
        }
    }

    color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
    for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        queue_grp->guarantee = 0;
        /* resume limits - accounted for 8 cell granularity */
        queue_grp->pool_resume = 16;
        queue_grp->yellow_resume = 16;
        queue_grp->red_resume = 16;
        if (lossless) {
            queue_grp->guarantee = 16;
            queue_grp->pool_limit = 0;
            queue_grp->discard_enable = 1;
            queue_grp->pool_scale = 8;
            queue_grp->red_limit = color_limit;
            queue_grp->yellow_limit = color_limit;
        } else {
            queue_grp->pool_limit = 0;
            queue_grp->discard_enable = 1;
            queue_grp->pool_scale = 8;
            queue_grp->red_limit = color_limit;
            queue_grp->yellow_limit = color_limit;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        if (port >= SOC_MMU_CFG_PORT_MAX) {
            break;
        }
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < 16; idx++) {
            buf_port->pri_to_prigroup[idx] = idx & 0x7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == 0) {
                buf_port_pool->pool_limit = total_pool_size;
                buf_port_pool->pool_resume =
                            total_pool_size - 16;
            }
            in_reserved += buf_port_pool->guarantee;
        }

        buf_port->pkt_size = max_packet_cells;

        /* priority group */
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->pool_scale = -1;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == lossless_pg) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->flow_control_enable = lossless;
                if (lossless) {
                    buf_prigroup->guarantee = default_mtu_cells;
                    buf_prigroup->headroom =
                        _soc_th_default_pg_headroom(unit, port, lossless);
                    buf_prigroup->pool_scale = 8;
                    /* pool_resume - accounted for 8 cell granularity */
                    buf_prigroup->pool_resume = 16;
                }
            }
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            int lossless_queue = 0, pri_pg_idx = 0;
            buf_queue = &buf_port->queues[idx];

            if (idx < _TH_MMU_NUM_PG) {
                /* In TH, Pri-PG-Cos are unity mapped by default.
                 * But Pri-PG mapping can be changed using MMU Config tool.
                 * Hence finding the Queue's respective Pri and mapping
                 * to its PG.
                 */
                pri_pg_idx = buf_port->pri_to_prigroup[idx];
                buf_prigroup = &buf_port->prigroups[pri_pg_idx];
                if (buf_prigroup->flow_control_enable) {
                    lossless_queue = 1;
                }
            }

            buf_queue->qgroup_id = -1;
            buf_queue->mcq_entry_guarantee = 4;
            color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
            if ((lossless) && (lossless_queue)) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = 16; 
            } else {
                buf_queue->guarantee = per_q_guarentee;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = color_limit;
                buf_queue->red_limit = color_limit;
                buf_queue->color_discard_enable = 1;
                /* pool_resume - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;

                buf_queue->qgroup_id = 0;
                buf_queue->qgroup_min_enable = 1;
            }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            int lossless_queue = 0, pri_pg_idx = 0;
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];

            if (idx < _TH_MMU_NUM_PG) {
                /* In TH, Pri-PG-Cos are unity mapped by default.
                 * But Pri-PG mapping can be changed using MMU Config tool.
                 * Hence finding the Queue's respective Pri and mapping
                 * to its PG.
                 */
                pri_pg_idx = buf_port->pri_to_prigroup[idx];
                buf_prigroup = &buf_port->prigroups[pri_pg_idx];
                if (buf_prigroup->flow_control_enable) {
                    lossless_queue = 1;
                }
            }

            buf_queue->qgroup_id = -1;
            color_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
            if ((lossless) && (lossless_queue)) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;
            } else {
                buf_queue->guarantee = per_q_guarentee;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = color_limit;
                buf_queue->yellow_limit = color_limit;
                buf_queue->color_discard_enable = 1;
                /* resume limits - accounted for 8 cell granularity */
                buf_queue->pool_resume = 16;
                buf_queue->yellow_resume = 16;
                buf_queue->red_resume = 16;

                buf_queue->qgroup_id = 0;
                buf_queue->qgroup_min_enable = 1;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        buf_prigroup = &buf_port->prigroups[7];
        if (!lossless) {
            buf_prigroup->pool_limit = total_pool_size - in_reserved -
                                       total_reserved;
        }
    }

    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        buf_rqe_queue->pool_idx = 0;
        buf_rqe_queue->red_limit = egr_shared_total;
        buf_rqe_queue->yellow_limit = egr_shared_total;
        if (lossless) {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 0;
            buf_rqe_queue->pool_scale = -1;
            buf_rqe_queue->pool_limit = egr_shared_total;
        } else {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 1;
            buf_rqe_queue->pool_scale = 8;
            buf_rqe_queue->pool_limit = 0;
        }
    }
}

STATIC int
_soc_th_pool_scale_to_limit(int size, int scale)
{
    int factor = 1000;

    switch (scale) {
        case 7: factor = 875; break;
        case 6: factor = 750; break;
        case 5: factor = 625; break;
        case 4: factor = 500; break;
        case 3: factor = 375; break;
        case 2: factor = 250; break;
        case 1: factor = 125; break;
        case 0:
        default:
            factor = 1000; break;
    }
    return (size * factor)/1000;
}

STATIC int
_soc_th_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                              _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    mmu_thdo_offset_qgroup_entry_t offset_qgrp;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    thdi_port_sp_config_entry_t thdi_sp_config;
    thdi_port_pg_config_entry_t pg_config_mem;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    soc_reg_t reg = INVALIDr;
    soc_mem_t mem0, mem1, mem2, mem3;
    uint32 fval, rval, rval2, rval3;
    int base, numq, qbase, rqlen;
    int port, idx, midx, pval, pri, index1;
    int jumbo_frame_cells, default_mtu_cells, limit, rlimit;
    int pipe;
    static const soc_mem_t mmu_thdi_port_mem[] = {
        THDI_PORT_SP_CONFIGm,
        THDI_PORT_PG_CONFIGm
    };
    static const soc_mem_t mmu_thdo_q_uc_mem[] = {
        MMU_THDU_CONFIG_QUEUEm,
        MMU_THDU_OFFSET_QUEUEm,
        MMU_THDU_Q_TO_QGRP_MAPm
    };
    static const soc_mem_t mmu_thdo_qgrp_uc_mem[] = {
        MMU_THDU_CONFIG_QGROUPm,
        MMU_THDU_OFFSET_QGROUPm
    };
    static const soc_mem_t mmu_thdo_port_uc_mem[] = {
        MMU_THDU_CONFIG_PORTm,
        MMU_THDU_RESUME_PORTm
    };
    static const soc_mem_t mmu_thdo_q_mc_mem[] = {
        MMU_THDM_DB_QUEUE_CONFIGm,
        MMU_THDM_DB_QUEUE_OFFSETm,
        MMU_THDM_MCQE_QUEUE_CONFIGm,
        MMU_THDM_MCQE_QUEUE_OFFSETm
    };

    static const soc_mem_t mmu_thdo_port_mc_mem[] = {
        MMU_THDM_DB_PORTSP_CONFIGm,
        MMU_THDM_MCQE_PORTSP_CONFIGm
    };

    static const soc_field_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    int pool_resume = 0;
    uint16      dev_id;
    uint8       rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    si = &SOC_INFO(unit);
    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                    devcfg->mmu_hdr_byte,
                                                    devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    pool_resume = 2 * jumbo_frame_cells;
    rval = 0;
    fval = _TH_MMU_PHYSICAL_CELLS_PER_XPE - _TH_MMU_RSVD_CELLS_CFAP_PER_XPE;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDSETr, &rval, CFAPFULLSETPOINTf,
                      fval);
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, CFAPFULLTHRESHOLDSETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDRESETr, &rval,
                      CFAPFULLRESETPOINTf, fval - (2 * jumbo_frame_cells));
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, CFAPFULLTHRESHOLDRESETr, -1, -1,
                                   -1, rval));

    rval = 0;
    soc_reg_field_set(unit, CFAPBANKFULLr, &rval, LIMITf, 767);
    for (idx = 0; idx < 15; idx++) {
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, CFAPBANKFULLr, -1, -1,
                                       idx, rval));
    }
    /* internal priority to priority group mapping */

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        for (idx = 0; idx < 16; idx++) {
            if (idx % 8 == 0) { /* 8 fields per register */
                reg = prigroup_reg[idx / 8];
                rval = 0;
            }
            soc_reg_field_set(unit, reg, &rval, prigroup_field[idx],
                              buf_port->pri_to_prigroup[idx]);
            if (idx % 8 == 7) { /* 8 fields per register */
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
            }
        }
    }

    /* Input thresholds */
    rval = 0;
    soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                      &rval, GLOBAL_HDRM_LIMITf, buf->headroom / 2);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, THDI_GLOBAL_HDRM_LIMITr,
                                      REG_PORT_ANY, 0, rval));

    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total - ((buf_pool->prigroup_headroom +
                                    buf_pool->prigroup_guarantee) / 2 +
                                   buf->headroom);

        rval = 0;
        soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf,
                          limit);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                          OFFSETf, pool_resume);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr,
                                       -1, -1, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr, &rval,
                          LIMITf, buf_pool->prigroup_headroom / 2);
        SOC_IF_ERROR_RETURN(
            soc_tomahawk_xpe_reg32_set(unit, THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                       -1, -1, idx, rval));
    }
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_tomahawk_xpe_reg32_set(unit, THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr,
                                   -1, -1, 0, rval));

    /* output thresholds */
    SOC_IF_ERROR_RETURN(READ_OP_THDU_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval,
                      ENABLE_QUEUE_AND_GROUP_TICKETf, 1);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval,
                      ENABLE_UPDATE_COLOR_RESUMEf, 0);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Af, 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THDU_CONFIGr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                      MOP_POLICYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));

    /* per service pool settings */
    for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total - buf_pool->queue_guarantee;

        if (limit <= 0) {
            limit = 0;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, &rval,
                          SHARED_LIMITf, limit);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_SHARED_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_SHARED_LIMITr(unit,
                                                                     idx,
                                                                     rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RESUME_LIMITr(unit,
                                                                 idx,
                                                                 rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                        idx,
                                                                        rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                          &rval, RED_RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_RESUME_LIMITr(unit,
                                                                     idx,
                                                                     rval));

        /* mcq entries */
        limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                          &rval, SHARED_LIMITf, limit/4);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr,
                          &rval, YELLOW_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr,
                          &rval, RED_SHARED_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RESUME_LIMITr,
                          &rval, RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RESUME_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr,
                          &rval, YELLOW_RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr(unit,
                                                                   idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr,
                                       &rval, RED_RESUME_LIMITf, limit/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr(unit,
                                                                   idx, rval));
    }

    /* configure Q-groups */
    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        for (idx = 0; idx < SOC_TH_MMU_CFG_QGROUP_MAX; idx++) {
            mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_qgrp_uc_mem[0])[pipe];
            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_qgrp_uc_mem[1])[pipe];
            if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
                continue;
            }
            queue_grp = &buf->qgroups[idx];


            sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));
            sal_memset(&offset_qgrp, 0, sizeof(offset_qgrp));

            soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                Q_MIN_LIMIT_CELLf, queue_grp->guarantee);

            if (queue_grp->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_SHARED_ALPHA_CELLf,
                                    queue_grp->pool_scale);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_LIMIT_DYNAMIC_CELLf, 1);
            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_SHARED_LIMIT_CELLf,
                                    queue_grp->pool_limit);
            }

            if ((queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }

            if (queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_th_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_RED_CELLf, pval/8);

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                        LIMIT_RED_CELLf,
                                        queue_grp->red_limit/8);
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_th_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf, pval/8);

            } else {
                soc_mem_field32_set(unit, mem0, &cfg_qgrp,
                                    LIMIT_YELLOW_CELLf,
                                    queue_grp->yellow_limit/8);
            }

            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_CELLf,
                                queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_YELLOW_CELLf,
                                queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp,
                                RESET_OFFSET_RED_CELLf,
                                queue_grp->pool_resume / 8);
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, idx, &cfg_qgrp));
            SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, idx, &offset_qgrp));
        }
    }

    /* Input port per port settings */
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        buf_port = &buf->ports[port];

        rval = 0;
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                              prigroup_spid_field[idx],
                              buf_port->prigroups[idx].pool_idx);
        }
        SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

        /* Per port per pool settings */
        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[0])[pipe];
        if (mem0 == INVALIDm) {
            continue;
        }
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[0],
                                              idx);
            sal_memset(&thdi_sp_config, 0, sizeof(thdi_sp_config));
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                                PORT_SP_MIN_LIMITf, buf_port_pool->guarantee);
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                           PORT_SP_RESUME_LIMITf, buf_port_pool->pool_resume / 8);
            soc_mem_field32_set(unit, mem0, &thdi_sp_config,
                               PORT_SP_MAX_LIMITf, buf_port_pool->pool_limit);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                               midx, &thdi_sp_config));
        }

        fval = 0;
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            if (buf_port->prigroups[idx].flow_control_enable != 0) {
                for (pri=0; pri < 16; pri++) {
                    if (buf_port->pri_to_prigroup[pri] == idx) {
                        fval |= 1 << pri;
                    }
                }
            }
        }

        rval = 0;
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                                          INPUT_PORT_RX_ENABLEf, 1);
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                          PORT_PRI_XON_ENABLEf, fval);
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                          PORT_PAUSE_ENABLEf, fval ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_THDI_INPUT_PORT_XON_ENABLESr(unit,
                                                               port, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_PORT_MAX_PKT_SIZEr, &rval,
                            PORT_MAX_PKT_SIZEf, buf_port->pkt_size);
        SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_MAX_PKT_SIZEr(unit, rval));

        /* Input port per port per priority group settings */
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdi_port_mem[1])[pipe];
        if (mem1 == INVALIDm) {
            continue;
        }
        for (idx = 0; idx < _TH_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port, mmu_thdi_port_mem[1],
                                              idx);
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_MIN_LIMITf, buf_prigroup->guarantee);

            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_scale);
            } else {
                soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                    PG_SHARED_LIMITf,
                                    buf_prigroup->pool_limit);
            }

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_GBL_HDRM_ENf,
                                buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_RESET_OFFSETf, buf_prigroup->pool_resume / 8);

            soc_mem_field32_set(unit, mem1, &pg_config_mem,
                                PG_RESET_FLOORf, buf_prigroup->pool_floor);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1,
                                              MEM_BLOCK_ALL, midx,
                                              &pg_config_mem));
        }
    }


    /***********************************
     * THDO
     ***********************************/
    /* Output port per port per queue setting for regular multicast queue */
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        numq = si->port_num_cosq[port];
        qbase = si->port_cosq_base[port];
        if (numq == 0) {
            continue;
        }

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[1])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
            continue;
        }
        base = qbase;
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_0_entry_t));

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMITf, buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem0, entry0, Q_COLOR_LIMIT_ENABLEf,
                                    1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0,
                           Q_SHARED_ALPHAf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_LIMITf, buf_queue->pool_limit);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit / 8);
                soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit / 8);
            }

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx,
                               entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem1, entry0,
                                RESUME_OFFSETf, (default_mtu_cells * 2)/8);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx,
                               entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                    (buf_queue->yellow_limit - buf_queue->yellow_resume) / 8));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0,
                        (buf_queue->red_limit - buf_queue->red_resume) / 8));
        }

        mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[2])[pipe];
        mem3 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_mc_mem[3])[pipe];
        if ((mem2 == INVALIDm) || (mem3 == INVALIDm)) {
            continue;
        }
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];
            buf_pool = &buf->pools[buf_queue->pool_idx];
            if (buf_pool->total == 0) {
                continue;
            }
            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_entry_t));

            soc_mem_field32_set(unit, mem2, entry0,
                                Q_MIN_LIMITf,
                                buf_queue->mcq_entry_guarantee/4);
            limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0, Q_COLOR_LIMIT_ENABLEf,
                                    1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_SHARED_ALPHAf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_SHARED_LIMITf, limit/4);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf,
                                    limit / 8);
                soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf,
                                    limit / 8);

            }

            soc_mem_field32_set(unit, mem2, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx,
                               entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_offset_entry_t));

            soc_mem_field32_set(unit, mem3, entry0, RESUME_OFFSETf, 1);
            soc_mem_field32_set(unit, mem3, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem3, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem3, MEM_BLOCK_ALL, base + idx,
                               entry0));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr(unit, 0,
                    (buf_queue->yellow_limit - buf_queue->yellow_resume) / 8));

            rval = 0;
            SOC_IF_ERROR_RETURN(
                WRITE_MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr(unit, 0,
                    (buf_queue->red_limit - buf_queue->red_resume) / 8));
        }

        /* Per  port per pool */
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf->ports[port].pools[idx];
            if (buf_port_pool->pool_limit == 0) {
                continue;
            }

            limit = buf_port_pool->pool_limit;
            rlimit = limit - (default_mtu_cells * 2);

            mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[0])[pipe];
            if (mem0 == INVALIDm) {
                continue;
            }

            index1 = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port,
                                                mmu_thdo_port_mc_mem[0],
                                                idx);
            sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_entry_t));

            soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, mem0, entry0, RED_SHARED_LIMITf,
                                limit/8);
            soc_mem_field32_set(unit, mem0, entry0, YELLOW_SHARED_LIMITf,
                                limit/8);

            soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMIT_ENABLEf,
                                !lossless);

            soc_mem_field32_set(unit, mem0, entry0, SHARED_RESUME_LIMITf,
                                rlimit/8);
            soc_mem_field32_set(unit, mem0, entry0, YELLOW_RESUME_LIMITf,
                                rlimit/8);
            soc_mem_field32_set(unit, mem0, entry0, RED_RESUME_LIMITf,
                                rlimit/8);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                                index1, entry0));

            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_mc_mem[1])[pipe];
            if (mem1 == INVALIDm) {
                continue;
            }
            buf_pool = &buf->pools[idx];
            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_portsp_config_entry_t));

            limit = buf_pool->total_mcq_entry;

            soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMITf, (limit/4) - 1);
            soc_mem_field32_set(unit, mem1, entry0, YELLOW_SHARED_LIMITf,
                                (limit/8) - 1);
            soc_mem_field32_set(unit, mem1, entry0, RED_SHARED_LIMITf,
                                (limit/8) - 1);

            soc_mem_field32_set(unit, mem1, entry0, SHARED_LIMIT_ENABLEf,
                                !lossless);

            soc_mem_field32_set(unit, mem1, entry0, SHARED_RESUME_LIMITf,
                                (limit/8) - 2);
            soc_mem_field32_set(unit, mem1, entry0, YELLOW_RESUME_LIMITf,
                                (limit/8) - 2);
            soc_mem_field32_set(unit, mem1, entry0, RED_RESUME_LIMITf,
                                (limit/8) - 2);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                                index1, entry0));
        }
    }

    /* Output port per port per queue setting for regular unicast queue */
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        /* per port regular unicast queue */
        numq = si->port_num_uc_cosq[port];
        qbase = si->port_uc_cosq_base[port];

        if (numq == 0) {
            continue;
        }
        base = qbase;

        mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[0])[pipe];
        mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[1])[pipe];
        mem2 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_q_uc_mem[2])[pipe];
        if ((mem0 == INVALIDm) || (mem1 == INVALIDm) || (mem2 == INVALIDm)) {
            continue;
        }
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            sal_memset(entry0, 0, sizeof(mmu_thdu_config_queue_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdu_offset_queue_entry_t));

            soc_mem_field32_set(unit, mem0, entry0,
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem0, entry0, Q_LIMIT_DYNAMIC_CELLf,
                                    1);
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_ALPHA_CELLf,
                                    buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_SHARED_LIMIT_CELLf,
                                    buf_queue->pool_limit);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_CELLf,
                                buf_queue->pool_resume / 8);

            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem0, entry0,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                     buf_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                     buf_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_YELLOW_CELLf,
                                    buf_queue->yellow_limit / 8);
                soc_mem_field32_set(unit, mem0, entry0, LIMIT_RED_CELLf,
                                    buf_queue->red_limit / 8);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_YELLOW_CELLf,
                                buf_queue->yellow_resume / 8);
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_RED_CELLf,
                                buf_queue->red_resume / 8);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, base + idx, entry0));

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry1));

            sal_memset(entry0, 0, sizeof(mmu_thdo_q_to_qgrp_map_entry_t));
            soc_mem_field32_set(unit, mem2, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem2, entry0,
                                    Q_COLOR_ENABLE_CELLf, 1);
            }


            if (buf_queue->qgroup_id >= 0) {
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, 1);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf, 1);
                }
            }

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                              base + idx, entry0));
        }

        /* Per  port per pool unicast */
        for (idx = 0; idx < _TH_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];

            if (buf_pool->total == 0) {
                continue;
            }

            limit = buf_pool->total;

            mem0 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[0])[pipe];
            mem1 = SOC_MEM_UNIQUE_ACC(unit, mmu_thdo_port_uc_mem[1])[pipe];
            if ((mem0 == INVALIDm) || (mem1 == INVALIDm)) {
                continue;
            }
            index1 = SOC_TH_MMU_PIPED_MEM_INDEX(unit, port,
                                                mmu_thdo_port_uc_mem[0],
                                                idx);
            sal_memset(entry0, 0, sizeof(mmu_thdu_config_port_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdu_resume_port_entry_t));

            soc_mem_field32_set(unit, mem0, entry0, SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, mem1, entry1,
                                SHARED_RESUMEf,
                                (limit - (default_mtu_cells * 2))/8);

            soc_mem_field32_set(unit, mem0, entry0, YELLOW_LIMITf, limit/8);
            soc_mem_field32_set(unit, mem1, entry1,
                                YELLOW_RESUMEf,
                                (limit - (default_mtu_cells*2))/8);

            soc_mem_field32_set(unit, mem0, entry0, RED_LIMITf, limit/8);
            soc_mem_field32_set(unit, mem1, entry1,
                                RED_RESUMEf,
                                (limit - (default_mtu_cells * 2))/8);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem0, MEM_BLOCK_ALL,
                                index1, entry0));
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL,
                                index1, entry1));
        }
    }

    /* RQE */
    for (idx = 0; idx < _TH_MMU_NUM_RQE_QUEUES; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                            &rval, SPIDf, buf_rqe_queue->pool_idx);

        if ((buf_rqe_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
            (buf_rqe_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, COLOR_LIMIT_DYNAMICf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_RED_LIMITf,
                buf_rqe_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_YELLOW_LIMITf,
                buf_rqe_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_RED_LIMITf, buf_rqe_queue->red_limit / 8);

            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                SHARED_YELLOW_LIMITf, buf_rqe_queue->yellow_limit / 8);
        }

        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                          (buf_rqe_queue->discard_enable ? 1 : 0));

        if (buf_rqe_queue->pool_scale != -1) {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                              &rval, DYNAMIC_ENABLEf, 1);
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_ALPHAf, buf_rqe_queue->pool_scale);
        } else {
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                              SHARED_LIMITf, buf_rqe_queue->pool_limit);
        }
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 2);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_PRIQr(unit, idx, rval2));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_COLOR_PRIQr(unit,
                                                                idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf,
                          (default_mtu_cells * 2)/8);
        soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf,
                          (default_mtu_cells * 2)/8);
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

        /* queue entry */
        buf_pool = &buf->pools[buf_rqe_queue->pool_idx];
        fval = (buf_pool->total_rqe_entry + 8 * 11 - 1) / (8 * 11);
        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr,
                          &rval, MIN_LIMITf, buf_rqe_queue->guarantee/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_MIN_PRIQr(unit, idx, rval));

        rval = 0;
        rval2 = 0;
        rval3 = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, SPIDf, buf_rqe_queue->pool_idx);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                          &rval, COLOR_LIMIT_DYNAMICf, 0);
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_RED_LIMITf, fval);
        soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                          &rval3, SHARED_YELLOW_LIMITf, fval);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval,
                          LIMIT_ENABLEf, 1);

        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          SHARED_LIMITf, fval);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                          RESET_OFFSETf, 1);

        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG1_PRIQr(unit, idx, rval));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_PRIQr(unit, idx, rval2));
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_COLOR_PRIQr(unit, idx, rval3));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_REDf, default_mtu_cells/8);
        soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                          &rval, RESET_OFFSET_YELLOWf, default_mtu_cells/8);
        SOC_IF_ERROR_RETURN(
                  WRITE_MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

    }

    /* per pool RQE settings */
    for (idx = 0; idx < 4; idx++) {
        buf_pool = &buf->pools[idx];
        if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
            (buf_pool->total == 0)) {
            continue;
        }

        limit = buf_pool->total - buf_pool->queue_guarantee;

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr,
                          &rval, SHARED_LIMITf, limit);
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr, &rval, RESUME_LIMITf,
                          (limit - (default_mtu_cells * 2))/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_SPr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_RED_LIMITf,
                          (limit - (default_mtu_cells * 2))/8);
        soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                          SHARED_YELLOW_LIMITf,
                          (limit - (default_mtu_cells * 2))/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_SP_SHARED_LIMITr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf,
                          (limit - (default_mtu_cells * 2))/8);
        soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf,
                          (limit - (default_mtu_cells * 2))/8);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr(unit,
                                                                     idx, rval));

        rqlen = buf_pool->total_rqe_entry/8;
        if (rqlen == 0) {
            continue;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval,
                          SHARED_LIMITf, rqlen);
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval,
                          RESUME_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_SPr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                          SHARED_RED_LIMITf, rqlen);
        soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                          SHARED_YELLOW_LIMITf, rqlen);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr(unit,
                                                                     idx,
                                                                     rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_RED_LIMITf, rqlen - 1);
        soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                          RESUME_YELLOW_LIMITf, rqlen - 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr(unit,
                                                                     idx, rval));
    }


    /* Device level config setting */
#if 0
    if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0)) {
        SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                          UC_MC_PORTSP_COMB_ACCT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));
    }
#endif
    return SOC_E_NONE;
}

STATIC int
soc_th_mmu_config_init(int unit, int test_only)
{
    int rv;
    int lossless = 1;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    soc_mmu_config_buf_default_f soc_mmu_config_buf_default = NULL;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    soc_mmu_config_buf_default = _soc_th_mmu_config_buf_default;

    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all))) {
        /* Config has Flex ports */
        soc_mmu_config_buf_default = _soc_th_mmu_config_buf_default_flex_port;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    _soc_th_mmu_init_dev_config(unit, &devcfg, lossless);
    soc_mmu_config_buf_default(unit, buf, &devcfg, lossless);

    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        /* Override default config */
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (!test_only) {
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MMU config: Use default setting\n")));
            soc_mmu_config_buf_default(unit, buf, &devcfg, lossless);
            SOC_IF_ERROR_RETURN
                (_soc_mmu_cfg_buf_calculate(unit, buf, &devcfg));
        }
        rv = _soc_th_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless);
    }

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_tomahawk_mmu_init(int unit)
{
    soc_info_t *si;
    uint32 pipe_map;
    int pipe;
    int port, phy_port, mmu_port;
    uint64 enable[_TH_PIPES_PER_DEV];

    si = &SOC_INFO(unit);

    soc_tomahawk_pipe_map_get(unit, &pipe_map);

    SOC_IF_ERROR_RETURN(soc_th_mmu_config_init(unit, FALSE));
    sal_memset(enable, 0, sizeof(enable));
    PBMP_ALL_ITER(unit, port) {
        pipe = si->port_pipe[port];
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        if (mmu_port & 0x20) {
            u64_H(enable[pipe]) |= 1 << (mmu_port & 0x1f);
        } else {
            u64_L(enable[pipe]) |= 1 << (mmu_port & 0x1f);
        }
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg_set(unit, THDU_OUTPUT_PORT_RX_ENABLE_64r,
                                      -1, pipe, 0, enable[pipe]));
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg_set(unit, MMU_THDM_DB_PORT_RX_ENABLE_64r,
                                      -1, pipe, 0, enable[pipe]));
    }

    for (pipe = 0; pipe < _TH_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_xpe_reg_set(unit, MMU_THDM_MCQE_PORT_RX_ENABLE_64r,
                                      -1, pipe, 0, enable[pipe]));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    *age_seconds = soc->l2x_age_interval ? (soc->l2x_age_interval) : 0;
    *enabled = soc->l2x_age_pid != SAL_THREAD_ERROR ? 1 : 0;
    return SOC_E_NONE; 
}

STATIC int
_soc_tomahawk_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds = 0x7fffffff;
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_age_timer_set(int unit, int age_seconds, int enable)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    if (soc->l2x_age_interval) {
        if (age_seconds) {
            soc->l2x_age_interval = age_seconds;
        }
        if (!enable) {
            SOC_IF_ERROR_RETURN(soc_th_l2_bulk_age_stop(unit));
        }
    } else {
        if (age_seconds) {
            soc->l2x_age_interval = age_seconds;
            if (enable) {
                SOC_IF_ERROR_RETURN(soc_th_l2_bulk_age_start(unit, 
                           soc->l2x_age_interval));
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
soc_tomahawk_oversub_group_find(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    int port_base, phy_port_base, idb_port_base, mmu_port_base;
    int port, mmu_port, id, inst;
    uint32 rval, fval;
    int pipe, group, slot, count, i;
    int cur_ids[4], cur_ids_len;
    int ids_len;
    int slots[4], slots_len;
    int speed_max, cur_speed_max;
    static const soc_reg_t idb_grp_tbl_regs[] = {
        IS_OVR_SUB_GRP0_TBLr, IS_OVR_SUB_GRP1_TBLr,
        IS_OVR_SUB_GRP2_TBLr, IS_OVR_SUB_GRP3_TBLr,
        IS_OVR_SUB_GRP4_TBLr, IS_OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t mmu_grp_tbl_regs[] = {
        OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
        OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
        OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr
    };

    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    idb_port_base = si->port_l2i_mapping[port_base];
    mmu_port_base = si->port_p2m_mapping[phy_port_base];
    pipe = si->port_pipe[port_base];

    /* Figure out the new speed group */
    speed_max = 25000 * lanes_ctrl->lanes;
    if (speed_max > si->port_speed_max[port_base]) {
        speed_max = si->port_speed_max[port_base];
    }
    _soc_tomahawk_speed_to_ovs_class_mapping(unit, speed_max,
                                             &lanes_ctrl->class);

    /* Figure out the current speed group */
    cur_speed_max = 25000 * lanes_ctrl->cur_lanes;
    if (cur_speed_max > si->port_speed_max[port_base]) {
        cur_speed_max = si->port_speed_max[port_base];
    }
    _soc_tomahawk_speed_to_ovs_class_mapping(unit, cur_speed_max,
                                             &lanes_ctrl->cur_class);

    /* Construct new and old idb port list for oversub group replacement */
    cur_ids[0] = idb_port_base;
    ids_len = cur_ids_len = 1;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* idb port(s) to be removed */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            cur_ids[1 + i] = si->port_l2i_mapping[port];
        }
        cur_ids_len += lanes_ctrl->phy_ports_len;
    } else { /* idb port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
        }
        ids_len += lanes_ctrl->phy_ports_len;
    }

    lanes_ctrl->idb_group = -1;
    lanes_ctrl->idb_slots[0] = -1;
    lanes_ctrl->cur_idb_slots[0] = -1;
    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, group, &rval));
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (fval == 0) { /* unused idb port oversub group */
            if (lanes_ctrl->idb_group == -1) {
                lanes_ctrl->idb_group = group;
            }
            continue;
        } else if (fval == lanes_ctrl->class) {
            if (lanes_ctrl->idb_slots[0] == -1) { /* hasn't found the group */
                /* Find slot(s) for new idb port oversub group */
                reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TH_PORTS_PER_PBLK / lanes_ctrl->lanes);
                slots_len = 0;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                    if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                        continue;
                    }
                    slots[slots_len] = slot;
                    slots_len++;
                    if (slots_len == ids_len) {
                        /* Enough free slots in an existing group */
                        lanes_ctrl->idb_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->idb_slots[i] = slots[i];
                        }
                        break;
                    }
                }
            }
        } else if (fval == lanes_ctrl->cur_class) {
            if (lanes_ctrl->cur_idb_slots[0] == -1) {
                /* Find slot(s) used by the currnet idb port oversub group */
                reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TH_PORTS_PER_PBLK / lanes_ctrl->cur_lanes);
                slots_len = 0;
                lanes_ctrl->cur_idb_empty = TRUE;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                    id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                    if (id == 0x3f) {
                        continue;
                    }
                    for (i = 0; i < cur_ids_len; i++) {
                        if (id == cur_ids[i]) {
                            slots[slots_len] = slot;
                            slots_len++;
                            break;
                        }
                    }
                    if (i == cur_ids_len) {
                        lanes_ctrl->cur_idb_empty = FALSE;
                    } else if (slots_len == cur_ids_len) {
                        lanes_ctrl->cur_idb_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->cur_idb_slots[i] = slots[i];
                        }
                    }
                    if (lanes_ctrl->cur_idb_slots[0] != -1 &&
                        !lanes_ctrl->cur_idb_empty) {
                        break;
                    }
                }
            }
        }
        if (lanes_ctrl->idb_slots[0] != -1 &&
            lanes_ctrl->cur_idb_slots[0] != -1) {
            break;
        }
    }

    if (lanes_ctrl->idb_group == -1) {
        return SOC_E_FULL;
    }

    /* Construct new and old mmu port list for oversub group replacement */
    cur_ids[0] = mmu_port_base & 0x3f;
    ids_len = cur_ids_len = 1;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* mmu port(s) to be removed */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
            cur_ids[1 + i] = mmu_port & 0x3f;
        }
        cur_ids_len += lanes_ctrl->phy_ports_len;
    } else { /* mmu port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
        }
        ids_len += lanes_ctrl->phy_ports_len;
    }

    lanes_ctrl->mmu_group = -1;
    lanes_ctrl->mmu_slots[0] = -1;
    lanes_ctrl->cur_mmu_slots[0] = -1;
    for (group = 0; group < _OVS_GROUP_COUNT; group++) {
        reg = OVR_SUB_GRP_CFGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, group, &rval));
        fval = soc_reg_field_get(unit, reg, rval, SPEEDf);
        if (fval == 0) { /* unused mmu port oversub group */
            if (lanes_ctrl->mmu_group == -1) {
                lanes_ctrl->mmu_group = group;
            }
            continue;
        } else if (fval == lanes_ctrl->class) {
            if (lanes_ctrl->mmu_slots[0] == -1) { /* hasn't found the group */
                /* Find slot(s) for new mmu port oversub group */
                inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                reg = mmu_grp_tbl_regs[group];
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TH_PORTS_PER_PBLK / lanes_ctrl->lanes);
                slots_len = 0;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, inst, slot, &rval));
                    if (soc_reg_field_get(unit, reg, rval, PORT_NUMf) != 0x3f) {
                        continue;
                    }
                    slots[slots_len] = slot;
                    slots_len++;
                    if (slots_len == ids_len) {
                        /* Enough free slots in an existing group */
                        lanes_ctrl->mmu_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->mmu_slots[i] = slots[i];
                        }
                        break;
                    }
                }
            }
        } else if (fval == lanes_ctrl->cur_class) {
            if (lanes_ctrl->cur_mmu_slots[0] == -1) {
                /* Find slot(s) used by the currnet mmu port oversub group */
                reg = mmu_grp_tbl_regs[group];
                inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
                count = _OVS_GROUP_TDM_LENGTH /
                    (_TH_PORTS_PER_PBLK / lanes_ctrl->cur_lanes);
                slots_len = 0;
                lanes_ctrl->cur_mmu_empty = TRUE;
                for (slot = 0; slot < count; slot++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, inst, slot, &rval));
                    id = soc_reg_field_get(unit, reg, rval, PORT_NUMf);
                    if (id == 0x3f) {
                        continue;
                    }
                    for (i = 0; i < cur_ids_len; i++) {
                        if (id == cur_ids[i]) {
                            slots[slots_len] = slot;
                            slots_len++;
                            break;
                        }
                    }
                    if (i == cur_ids_len) {
                        lanes_ctrl->cur_mmu_empty = FALSE;
                    } else if (slots_len == cur_ids_len) {
                        lanes_ctrl->cur_mmu_group = group;
                        for (i = 0; i < slots_len; i++) {
                            lanes_ctrl->cur_mmu_slots[i] = slots[i];
                        }
                    }
                    if (lanes_ctrl->cur_mmu_slots[0] != -1 &&
                        !lanes_ctrl->cur_mmu_empty) {
                        break;
                    }
                }
            }
        }
        if (lanes_ctrl->mmu_slots[0] != -1 &&
            lanes_ctrl->cur_mmu_slots[0] != -1) {
            break;
        }
    }

    if (lanes_ctrl->mmu_group == -1) {
        return SOC_E_FULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_tomahawk_port_lanes_validate
 * Description:
 *      Validate if the specified number of lanes can be configured on a port,
 *      If the request can be done, fill in the control structure which will
 *      be used by the routines programming the hardware.
 * Parameters:
 *      unit          - Device number
 *      lanes_ctrl    - Control structure
 *
 * Each TSC can be configured into following 5 mode:
 *   Lane number    0              1         2              3
 *   ------------   -----------    --------  ------------   --------
 *    single port   100G/40G       x         x              x
 *      dual port   50G/d40G/20G   x         50G/d40G/20G   x
 *   tri_023 port   50G/d40g/20G   x         25G/10G        25G/10G
 *   tri_012 port   25G/10G        25G/10G   50G/d40G/20G   x
 *      quad port   25G/10G        25G/10G   25G/10G        25G/10G
 *
 * (Note: All of the above apply to equivalent HG line rate speeds as well)
 *
 *          lanes                mode         valid lane index
 *       ------------      ----------------   ----------------
 *       new  current        new    current
 *       ---  -------      -------  -------
 * #1     4      1         single    quad            0
 * #2     4      1         single   tri_012          0
 * #3     4      2         single   tri_023          0
 * #4     4      2         single    dual            0
 * #5     2      1         tri_023   quad            0
 * #6     2      1         tri_012   quad            2
 * #7     2      1          dual    tri_023          2
 * #8     2      1          dual    tri_012          0
 * #9     2      4          dual    single           0
 * #10    1      2         tri_023   dual            2
 * #11    1      2         tri_012   dual            0
 * #12    1      2          quad    tri_023          0
 * #13    1      2          quad    tri_012          2
 * #14    1      4          quad    single           0
 * Following mode change requires 2 transition
 *   - from single to tri_023: #9 + #10
 *   - from single to tri_012: #9 + #11
 * Following mode change are the result of lane change on multiple ports
 *   - from quad to dual: #12 + #7 or #13 + #8
 *   - from dual to quad: #10 + #12 or #11 + # 13
 *   - from tri_023 to tri_012: #7 + #11 or #12 + #6
 *   - from tri_012 to tri_023: #8 + #10 or #13 + #5
 *
 * Logical port number will stay the same after conversion, for example
 *     converting single port to dual port, the logical port number of lane 0
 *     will be changed.
 */
int
soc_tomahawk_port_lanes_validate(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port_base, phy_port_base;
    uint32 rval;
    int i;

    /* Find physical and mmu port number for the specified port */
    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }
    lanes_ctrl->oversub = SOC_PBMP_MEMBER(si->oversub_pbm, port_base);

    /* Find lane index for the specified port (base port) */
    lanes_ctrl->bindex = SOC_PORT_BINDEX(unit, phy_port_base);

    /* Get the current mode */
    SOC_IF_ERROR_RETURN(READ_CLPORT_MODE_REGr(unit, port_base, &rval));
    lanes_ctrl->cur_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval,
                                             XPORT0_CORE_PORT_MODEf);

    /* Figure out the current number of lanes */
    switch (lanes_ctrl->cur_mode) {
    case SOC_TH_PORT_MODE_QUAD:
        lanes_ctrl->cur_lanes = 1;
        break;
    case SOC_TH_PORT_MODE_TRI_012:
        lanes_ctrl->cur_lanes = lanes_ctrl->bindex == 0 ? 1 : 2;
        break;
    case SOC_TH_PORT_MODE_TRI_023:
        lanes_ctrl->cur_lanes = lanes_ctrl->bindex == 0 ? 2 : 1;
        break;
    case SOC_TH_PORT_MODE_DUAL:
        lanes_ctrl->cur_lanes = 2;
        break;
    case SOC_TH_PORT_MODE_SINGLE:
        lanes_ctrl->cur_lanes = 4;
        break;
    default:
        return SOC_E_FAIL;
    }
    /* Validate new number of lanes */
    if (lanes_ctrl->lanes == 4 || lanes_ctrl->cur_lanes == 4) {
        if (lanes_ctrl->bindex & 0x3) {
            return SOC_E_PARAM;
        }
    } else if (lanes_ctrl->lanes == 2 || lanes_ctrl->cur_lanes == 2) {
        if (lanes_ctrl->bindex & 0x1) {
            return SOC_E_PARAM;
        }
    } else if (lanes_ctrl->lanes != 1) {
        return SOC_E_PARAM;
    }

    /* No change to number of lanes */
    if (lanes_ctrl->lanes == lanes_ctrl->cur_lanes) {
        return SOC_E_NONE;
    }

    if (lanes_ctrl->oversub) {
        SOC_IF_ERROR_RETURN(soc_tomahawk_oversub_group_find(unit, lanes_ctrl));
    }

    /* Figure out new mode */
    if (lanes_ctrl->lanes == 4) {
        lanes_ctrl->mode = 4;
    } else if (lanes_ctrl->lanes == 2) {
        if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_QUAD) {
            lanes_ctrl->mode = lanes_ctrl->bindex == 0 ?
                SOC_TH_PORT_MODE_TRI_023 : SOC_TH_PORT_MODE_TRI_012;
        } else {
            lanes_ctrl->mode = SOC_TH_PORT_MODE_DUAL;
        }
    } else{
        if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_DUAL) {
            lanes_ctrl->mode = lanes_ctrl->bindex == 0 ?
                SOC_TH_PORT_MODE_TRI_012 : SOC_TH_PORT_MODE_TRI_023;
        } else {
            lanes_ctrl->mode = SOC_TH_PORT_MODE_QUAD;
        }
    }

    lanes_ctrl->phy_ports_len = 0;
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* Figure out which port(s) to be removed */
        if (lanes_ctrl->lanes == 4) {
            if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_TRI_012 ||
                lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 1;
                }
            }
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 2;
            }
            if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_TRI_023 ||
                lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 3] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 3;
                }
            }
        } else {
            if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 1;
            }
        }
    } else { /* Figure out which port(s) to be added */
        if (lanes_ctrl->lanes == 2) {
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                                      phy_port_base + 2;
            }
        } else {
            if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                    phy_port_base + 1;
            }
            if (lanes_ctrl->cur_mode == SOC_TH_PORT_MODE_SINGLE) {
                if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 2;
                }
                if (si->port_p2l_mapping[phy_port_base + 3] != -1) {
                    lanes_ctrl->phy_ports[lanes_ctrl->phy_ports_len++] =
                        phy_port_base + 3;
                }
            }
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        static char *mode_name[] = {
            "QUAD", "TRI_012", "TRI_023", "DUAL", "SINGLE"
        };
        LOG_CLI((BSL_META_U(unit,
                            "port %d physical port %d bindex %d\n"),
                 port_base, phy_port_base, lanes_ctrl->bindex));
        LOG_CLI((BSL_META_U(unit,
                            "  mode (new:%s cur:%s) lanes (new:%d cur:%d)\n"),
                 mode_name[lanes_ctrl->mode],
                 mode_name[lanes_ctrl->cur_mode],
                 lanes_ctrl->lanes, lanes_ctrl->cur_lanes));
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "  %s physical port %d (port %d)\n"),
                     lanes_ctrl->lanes > lanes_ctrl->cur_lanes ?
                     "del" : "add",
                     lanes_ctrl->phy_ports[i],
                     si->port_p2l_mapping[lanes_ctrl->phy_ports[i]]));
        }
        if (lanes_ctrl->idb_group != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "new idb group %d slot %d"),
                     lanes_ctrl->idb_group, lanes_ctrl->idb_slots[0]));
            if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes &&
                lanes_ctrl->idb_slots[0] != -1) {
                for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                    LOG_CLI((BSL_META_U(unit,
                                        " slot %d"), lanes_ctrl->idb_slots[i]));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "cur idb group %d slot %d"),
                 lanes_ctrl->cur_idb_group, lanes_ctrl->cur_idb_slots[0]));
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " slot %d"), lanes_ctrl->idb_slots[i]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));

        if (lanes_ctrl->mmu_group != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "new mmu group %d slot %d"),
                     lanes_ctrl->mmu_group, lanes_ctrl->mmu_slots[0]));
            if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes &&
                lanes_ctrl->mmu_slots[0] != -1) {
                /* port(s) to be added */
                for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                    LOG_CLI((BSL_META_U(unit,
                                        " slot %d"), lanes_ctrl->mmu_slots[i]));
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "cur mmu group %d slot %d"),
                 lanes_ctrl->cur_mmu_group, lanes_ctrl->cur_mmu_slots[0]));
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                LOG_CLI((BSL_META_U(unit,
                                    " slot %d"), lanes_ctrl->mmu_slots[i]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk_update_ep_credit(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_reg_t reg = EGR_MMU_CELL_CREDITr;
    uint32 vco, fval, rval = 0;
    int i, n = 1, pc = 1, port = lanes_ctrl->port_base;
    int sis_port_2, sis_port_speed;
    int sis_port = SOC_INFO(unit).port_l2p_mapping[port];

    /* Under various circumstances check either the max speed value coming
     * from the port map or query the serdes driver for the vco frequency of 
     * the other active ports and determine what will be the new speed for the 
     * ports being flexed.
     */    
    if (lanes_ctrl->lanes == 1) {
        /* Possibilities are 25G/10G */
        if (lanes_ctrl->mode == SOC_TH_PORT_MODE_TRI_012 ||
            lanes_ctrl->mode == SOC_TH_PORT_MODE_TRI_023) {
            if (lanes_ctrl->mode == SOC_TH_PORT_MODE_TRI_012) {
                sis_port = SOC_INFO(unit).port_p2l_mapping[sis_port+2];
            } else {
                sis_port = SOC_INFO(unit).port_p2l_mapping[sis_port-1];
            }
            if (sis_port >= 0) {
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_control_get(unit, sis_port, SOC_PHY_CONTROL_VCO_FREQ,
                                             &vco));
                if ((vco >= 20625) && (SOC_INFO(unit).port_speed_max[sis_port] >= 25000)) {
                    fval = 16;
                } else {
                    fval = 13;
                }
                pc = 2;
            } else {
                fval = 16;
                pc = 2;
            }
        } else {
            sis_port = SOC_INFO(unit).port_p2l_mapping[sis_port+1];
            if (sis_port >= 0) {
                sis_port_speed = SOC_INFO(unit).port_speed_max[sis_port];
                sis_port_2 = (lanes_ctrl->bindex == 0) ? 
                             SOC_INFO(unit).port_p2l_mapping[sis_port+2] :
                             SOC_INFO(unit).port_p2l_mapping[sis_port-2];
                if (sis_port_2 > 0) {
                    if (SOC_INFO(unit).port_speed_max[sis_port] > 
                        SOC_INFO(unit).port_speed_max[sis_port_2]) {
                        sis_port_speed = SOC_INFO(unit).port_speed_max[sis_port_2];
                    }
                }
                if (sis_port_speed >= 25000) {
                    fval = 16;
                } else {
                    fval = 13;
                }
                if (lanes_ctrl->bindex == 0) {
                    pc = 4;
                } else {
                    pc = 2;
                }
            } else { /* Only two lanes configured */
                fval = 16;
                pc = 2;
            }
        }        
    } else if (lanes_ctrl->lanes == 2) {
        /* Possibilities are 50G/40G/20G */
        if (lanes_ctrl->mode == SOC_TH_PORT_MODE_TRI_012 ||
            lanes_ctrl->mode == SOC_TH_PORT_MODE_TRI_023) {
            if (lanes_ctrl->mode == SOC_TH_PORT_MODE_TRI_012) {
                sis_port = SOC_INFO(unit).port_p2l_mapping[sis_port-1];
            } else {
                sis_port = SOC_INFO(unit).port_p2l_mapping[sis_port+2];
            }
            if (sis_port >= 0) {
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_control_get(unit, sis_port, SOC_PHY_CONTROL_VCO_FREQ,
                                             &vco));
                if ((vco >= 25781) && (SOC_INFO(unit).port_speed_max[sis_port] >= 50000)) {
                    fval = 27;
                } else if ((vco >= 20625) && (SOC_INFO(unit).port_speed_max[sis_port] >= 40000)) {
                    fval = 25;
                } else {
                    fval = 18;
                }
            } else {
                fval = 27;
            }
        } else {
            sis_port = SOC_INFO(unit).port_p2l_mapping[sis_port+2];
            if (sis_port >= 0) {
                if (SOC_INFO(unit).port_speed_max[sis_port] >= 50000) {
                    fval = 27;
                } else if (SOC_INFO(unit).port_speed_max[sis_port] >= 40000) {
                    fval = 25;
                } else {
                    fval = 18;
                }
                pc = 3;
                n = 2;
            } else { /* Only two lanes configured */
                fval = 27;
                pc = 1;
            }
        }
    } else {
        /* Possibilities are 100G/40G */
        if (SOC_INFO(unit).port_speed_max[port] >= 100000) {
            fval = 44;
        } else {
            fval = 25;
        }
    }
    soc_reg_field_set(unit, reg, &rval, CREDITf, fval);
    for (i = 0; i < pc; i+=n) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITr(unit, port+i, rval));
    }
    return SOC_E_NONE;
}

#if !defined(BCM_TH_ASF_EXCLUDE)
STATIC int
_soc_tomahawk_xmt_start_cnt_update(int unit, int extra_delay)
{
    int                 port;
    int                 port_speed = 0;
    soc_th_asf_mode_e   mode = _SOC_TH_ASF_MODE_CFG_UPDATE;

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(
            soc_th_port_asf_xmit_start_count_set(unit, port, port_speed,
                mode, extra_delay));
    }

    return SOC_E_NONE;
}
#else
STATIC int
_soc_tomahawk_xmt_start_cnt_update(int unit, int extra_delay)
{
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    int pipe, port, class, index;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int *start_count;
    static int start_count_saf[13] =
        { 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18 };
    static int start_count_linerate[13] =
        { 18, 27, 27, 21, 21, 18, 18, 12, 12, 12, 12, 15, 15 };
    static int start_count_oversub_2_1[13] =
        { 18, 57, 57, 51, 51, 45, 45, 39, 39, 33, 33, 33, 33 };
    static int start_count_oversub_3_2[13] =
        { 18, 45, 45, 39, 39, 36, 36, 27, 27, 24, 24, 24, 24 };

    si = &SOC_INFO(unit);
    tdm = SOC_CONTROL(unit)->tdm_info;

    sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
    PBMP_ALL_ITER(unit, port) {
        if (IS_CL_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
            if (soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, ENABLEf)) {
                if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
                    pipe = si->port_pipe[port];
                    if (tdm->ovs_ratio_x1000[pipe] > 1500) {
                        /* More than 3:2 oversubscription ratio */
                        start_count = start_count_oversub_2_1;
                    } else {
                        start_count = start_count_oversub_3_2;
                    }
                } else {
                    start_count = start_count_linerate;
                }
            } else {
                start_count = start_count_saf;
            }
        } else {
            start_count = start_count_saf;
        }

        for (class = 0; class < 13; class++) {
            soc_mem_field32_set(unit, EGR_XMIT_START_COUNTm, entry, THRESHOLDf,
                                start_count[class] + extra_delay);
            index = (port % 34) * 16 + class;
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_XMIT_START_COUNTm(unit, MEM_BLOCK_ALL, index,
                                             entry));
        }
    }

    return SOC_E_NONE;
}
#endif

STATIC int
_soc_tomahawk_port_lanes_update_tdm(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    soc_control_t *soc;
    soc_info_t *si;
    _soc_tomahawk_tdm_t *tdm;
    int port_base, port, phy_port, idb_port, mmu_port;
    int group, pipe, slot, id, i;
    soc_reg_t reg;
    uint32 rval;
    uint32 inst;
    static const soc_reg_t idb_grp_tbl_regs[] = {
        IS_OVR_SUB_GRP0_TBLr, IS_OVR_SUB_GRP1_TBLr,
        IS_OVR_SUB_GRP2_TBLr, IS_OVR_SUB_GRP3_TBLr,
        IS_OVR_SUB_GRP4_TBLr, IS_OVR_SUB_GRP5_TBLr
    };
    static const soc_reg_t mmu_grp_tbl_regs[] = {
        OVR_SUB_GRP0_TBLr, OVR_SUB_GRP1_TBLr,
        OVR_SUB_GRP2_TBLr, OVR_SUB_GRP3_TBLr,
        OVR_SUB_GRP4_TBLr, OVR_SUB_GRP5_TBLr
    };

    soc = SOC_CONTROL(unit);
    si = &SOC_INFO(unit);
    tdm = soc->tdm_info;
    port_base = lanes_ctrl->port_base;

    if (!SOC_PBMP_MEMBER(si->oversub_pbm, port_base)) {
        SOC_IF_ERROR_RETURN(_soc_tomahawk_xmt_start_cnt_update(unit, 9));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_calculation(unit, tdm));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_idb_calendar_set(unit, -1));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_tdm_mmu_calendar_set(unit, -1));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_update_ep_credit(unit, lanes_ctrl));
        SOC_IF_ERROR_RETURN(_soc_tomahawk_xmt_start_cnt_update(unit, 0));
    } else {
        pipe = si->port_pipe[port_base];
        id = si->port_serdes[port_base] & 7;

        /* Remove idb port(s) from current group */
        group = lanes_ctrl->cur_idb_group;
        reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, 0x3f);
        slot = lanes_ctrl->cur_idb_slots[0];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                slot = lanes_ctrl->cur_idb_slots[i + 1];
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
            }
        }
        if (lanes_ctrl->cur_idb_empty) { /* old group become empty */
            reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, group, 0));
        }

        /* Add idb port(s) to new group */
        group = lanes_ctrl->idb_group;
        reg = SOC_REG_UNIQUE_ACC(unit, idb_grp_tbl_regs[group])[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
        idb_port = si->port_l2i_mapping[port_base];
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
        slot = lanes_ctrl->idb_slots[0] != -1 ? lanes_ctrl->idb_slots[0] : 0;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes) {
            /* port(s) to be added */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
                idb_port = si->port_l2i_mapping[port];
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
                slot = lanes_ctrl->idb_slots[0] != -1 ?
                    lanes_ctrl->idb_slots[i + 1] : i + 1;
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
            }
        }
        if (lanes_ctrl->idb_slots[0] == -1) { /* new group */
            reg = SOC_REG_UNIQUE_ACC(unit, IS_OVR_SUB_GRP_CFGr)[pipe];
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, lanes_ctrl->class);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, group, rval));
        }

        /* Remove mmu port(s) from current group */
        group = lanes_ctrl->cur_mmu_group;
        reg = mmu_grp_tbl_regs[group];
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, 0x3f);
        slot = lanes_ctrl->cur_mmu_slots[0];
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
            /* port(s) to be removed */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                slot = lanes_ctrl->cur_mmu_slots[i + 1];
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
            }
        }
        if (lanes_ctrl->cur_idb_empty) { /* old group became empty */
            reg = OVR_SUB_GRP_CFGr;
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, group, 0));
        }

        /* Add mmu port(s) to new group */
        group = lanes_ctrl->mmu_group;
        reg = mmu_grp_tbl_regs[group];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
        phy_port = si->port_l2p_mapping[port_base];
        mmu_port = si->port_p2m_mapping[phy_port];
        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
        slot = lanes_ctrl->mmu_slots[0] != -1 ? lanes_ctrl->mmu_slots[0] : 0;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
        if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes) {
            /* port(s) to be added */
            for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
                mmu_port = si->port_p2m_mapping[lanes_ctrl->phy_ports[i]];
                soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port & 0x3f);
                slot = lanes_ctrl->mmu_slots[0] != -1 ?
                    lanes_ctrl->mmu_slots[i + 1] : i + 1;
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, slot, rval));
            }
        }
        if (lanes_ctrl->mmu_slots[0] == -1) { /* new group */
            reg = OVR_SUB_GRP_CFGr;
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
                              lanes_ctrl->class >= 4 ? 4 : 8);
            soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf, 4);
            soc_reg_field_set(unit, reg, &rval, SPEEDf, lanes_ctrl->class);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, group, rval));
        }
        SOC_IF_ERROR_RETURN(_soc_tomahawk_update_ep_credit(unit, lanes_ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_tomahawk_port_lanes_set
 * Description:
 *      Configure number of lanes used by a port
 * Parameters:
 *      unit          - Device number
 *      lanes_ctrl    - Control structure
 * Notes: Caller needs to call soc_tomahawk_port_lanes_validate to fill
 *        in the control structure.
 */
int
soc_tomahawk_port_lanes_set(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
    uint32 rval;
    soc_reg_t reg;
    uint32 values[2];
    soc_port_t it_port;
    soc_field_t fields[2];
    egr_enable_entry_t entry;
    soc_info_t *si = &SOC_INFO(unit);
    int i, pipe, port_base, phy_port_base, port, port_rename = 0;
    static soc_field_t egr_reset_fields[] = {
        PM0_RESETf, PM1_RESETf, PM2_RESETf, PM3_RESETf,
        PM4_RESETf, PM5_RESETf, PM6_RESETf, PM7_RESETf
    };

    /* Find physical port number and lane index of the specified port */
    port_base = lanes_ctrl->port_base;
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }
    pipe = si->port_pipe[port_base];

    /* Update soc_control information */
    SOC_CONTROL_LOCK(unit);
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* port(s) to be removed */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        }
    } else { /* port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
        }
    }

    /* Update num of lanes info which is used by SerDes driver */
    si->port_num_lanes[port_base] = lanes_ctrl->lanes;
    for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
        port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
        si->port_num_lanes[port] =
            lanes_ctrl->lanes > lanes_ctrl->cur_lanes ? 0 : lanes_ctrl->lanes;
    }

#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

    /* Update bitmaps and port names when switching b/w CE <=> XE ports */
    if ((lanes_ctrl->cur_lanes == 4) && IS_CE_PORT(unit, port_base)) {
        /* Convert to XE ports */
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port_base);
        for (i = 0; i < 4; i++) {
            if (si->port_p2l_mapping[phy_port_base+i] < 0) {
                continue;
            }
            SOC_PBMP_PORT_ADD(si->xe.bitmap,
                              si->port_p2l_mapping[phy_port_base+i]);
        }
        port_rename = 1;
    } else if ((lanes_ctrl->lanes == 4) && IS_XE_PORT(unit, port_base) &&
               (si->port_speed_max[port_base] == 100000)) {
        /* Convert to CE ports */
        SOC_PBMP_PORT_ADD(si->ce.bitmap, port_base);
        for (i = 0; i < 4; i++) {
            if (si->port_p2l_mapping[phy_port_base+i] < 0) {
                continue;
            }
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap,
                                 si->port_p2l_mapping[phy_port_base+i]);
        }
        port_rename = 1;
    }
    if (port_rename) {
        RECONFIGURE_PORT_TYPE_INFO(xe);
        RECONFIGURE_PORT_TYPE_INFO(ce);
        soc_dport_map_update(unit);
    }
#undef RECONFIGURE_PORT_TYPE_INFO
    SOC_CONTROL_UNLOCK(unit);

    /* De-assert EGR_ENABLE */
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port_base, &entry));
    if (lanes_ctrl->lanes > lanes_ctrl->cur_lanes) {
        /* port(s) to be removed */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, &entry));
        }
    }
    /* Change CLPORT mode */
    fields[0] = XPORT0_CORE_PORT_MODEf;
    values[0] = lanes_ctrl->mode;
    fields[1] = XPORT0_PHY_PORT_MODEf;
    values[1] = lanes_ctrl->mode;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, CLPORT_MODE_REGr,
                                                port_base, 2, fields, values));

    if (lanes_ctrl->phy_ports_len > 0) {
        /* Update TDM */
        SOC_IF_ERROR_RETURN
            (_soc_tomahawk_port_lanes_update_tdm(unit, lanes_ctrl));
    }

    /* Clear MMU port credit before Resetting egress */
    reg = MMU_PORT_CREDITr;
    port = si->port_p2l_mapping[phy_port_base];
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, INITIALIZEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, 0));
    if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes) { /* port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, INITIALIZEf, 1);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, 0));
        }
    }

    /* Reset egress hardware resource */
    reg = SOC_REG_UNIQUE_ACC(unit, EGR_PORT_BUFFER_SFT_RESET_0r)[pipe];
    fields[0] = egr_reset_fields[si->port_serdes[port_base] % 8];
    if (lanes_ctrl->lanes == 4 || lanes_ctrl->cur_lanes == 4) {
        /* reset all 4 lanes */
        values[0] = 4;
    } else if (lanes_ctrl->bindex == 0) { /* reset lanes 0 and 1 */
        values[0] = 1;
    } else { /* reset lanes 2 and 3 */
        values[0] = 2;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, fields[0], values[0]);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    soc_reg_field_set(unit, reg, &rval, fields[0], 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* Assert EGR_ENABLE */
    soc_mem_field32_set(unit, EGR_ENABLEm, &entry, PRT_ENABLEf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port_base, &entry));
    if (lanes_ctrl->lanes < lanes_ctrl->cur_lanes) { /* port(s) to be added */
        for (i = 0; i < lanes_ctrl->phy_ports_len; i++) {
            port = si->port_p2l_mapping[lanes_ctrl->phy_ports[i]];
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, &entry));
        }
    }

    return SOC_E_NONE;
}

static const soc_reg_t pvtmon_result_reg[] = { 
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r 
};

void
soc_tomahawk_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    uint32 rval, trval;
    int i, rv, unit = PTR_TO_INT(unit_vp);
    
    if ((rv = READ_TOP_PVTMON_INTR_STATUSr(unit, &rval)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TH Temp Intr, Reg access error.\n")));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "High temp interrupt: 0x%08x\n"), rval));
    /* Raise event to app for it to take further graceful actions */
    for (i = 0; i < 9; i++) {
        if (i & _soc_th_temp_mon_mask[unit]) {
            if ((rv = soc_reg32_get(unit, pvtmon_result_reg[i], REG_PORT_ANY, 0, 
                                    &trval)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, Reg access error.\n")));
            }
            trval = soc_reg_field_get(unit, pvtmon_result_reg[i], trval,
                                      PVT_DATAf);
            /* Convert data to temperature.
             * temp = 410.04-(data*0.48705) = (410040-(data*487))/1000
             * Note: Since this is a high temp interrupt we can safely assume
             * that this will end up being a +ive value.
             */            
            trval = (410040-(trval*487))/1000;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TempMon %d: %d deg C.\n"), i, trval));
            (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                     SOC_SWITCH_EVENT_ALARM_HIGH_TEMP, i, trval);
        }
    }
    /* Optionally hold system in reset.
     * Note: The main intention is to stop the chip from getting fried and halt
     *       all schan accesses as the h/w will not respond anymore.
     *       We are not implementing a gracefull recovery, the unit needs to be 
     *       rebooted after this.
     */
    if (soc_property_get(unit, "temp_monitor_shutdown", 1)) {
        if ((rv = WRITE_TOP_PVTMON_INTR_MASKr(unit, 0)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        if ((rv = WRITE_TOP_SOFT_RESET_REGr(unit, 0)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        if ((rv = READ_TOP_SOFT_RESET_REG_2r(unit, &rval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL2_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL3_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                          1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
        if ((rv = WRITE_TOP_SOFT_RESET_REG_2r(unit, rval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }
        if ((rv = WRITE_TOP_SOFT_RESET_REG_3r(unit, 0)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, Reg access error.\n")));
        }

        /* Stop all schan transactions on this unit */
#ifdef  INCLUDE_I2C
        if ((rv = soc_i2c_detach(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, i2c detach error.\n")));
        }
#endif
        if (!SOC_IS_RCPU_ONLY(unit)) {
            /* Free up DMA memory */
            if ((rv = soc_dma_detach(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, dma detach error.\n")));
            }
        }
        if ((rv = soc_mem_scan_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, mem scan stop error.\n")));
        }
        /* Terminate counter module */
        if ((rv = soc_counter_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, counter stop error.\n")));
        }
        if (SOC_SBUSDMA_DM_INFO(unit)) {
            if ((rv = soc_sbusdma_desc_detach(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, sbusdma stop error.\n")));
            }
        }
        if (soc_feature(unit, soc_feature_arl_hashed)) {
            /* Stop L2X thread */
            if ((rv = soc_l2x_stop(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH Temp Intr, L2x stop error.\n")));
            }

        }
        if ((rv = soc_th_l2_bulk_age_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TH Temp Intr, L2 age stop error.\n")));
        }
        /* Disable all interrupts */        
        soc_cmicm_intr0_disable(unit, ~0);
        soc_cmicm_intr1_disable(unit, ~0);
        soc_cmicm_intr2_disable(unit, ~0);
        soc_cmicm_intr3_disable(unit, ~0);
        soc_cmicm_intr4_disable(unit, ~0);
        soc_cmicm_intr5_disable(unit, ~0);
        SCHAN_LOCK(unit);
        /* Clear all outstanding DPCs owned by this unit */
        sal_dpc_cancel(INT_TO_PTR(unit));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "\nReboot the system..")));
    }
}

int
soc_tomahawk_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak;
    int num_entries_out;
    
    *temperature_count = 0;
    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
                                PVTMON_SELECTf, 0));
    sal_usleep(1000);

    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        cur = (4100400-(fval*4870))/1000;
        fval = soc_reg_field_get(unit, reg, rval, MIN_PVT_DATAf);
        peak = (4100400-(fval*4870))/1000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak = peak;
    }
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    *temperature_count=num_entries_out;

    return SOC_E_NONE;
}

/*
 * Tomahawk chip driver functions. 
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_tomahawk_drv_funs = {
    _soc_tomahawk_misc_init,
    _soc_tomahawk_mmu_init,
    _soc_tomahawk_age_timer_get,
    _soc_tomahawk_age_timer_max_get,
    _soc_tomahawk_age_timer_set,
    _soc_tomahawk_tscx_firmware_set,
    _soc_tomahawk_tscx_reg_read,
    _soc_tomahawk_tscx_reg_write,
};
#endif /* BCM_TOMAHAWK_SUPPORT */
