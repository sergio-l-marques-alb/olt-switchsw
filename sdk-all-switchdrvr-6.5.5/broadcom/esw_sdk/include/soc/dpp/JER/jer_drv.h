/*
 * $Id: dfe_drv.h Exp $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef _SOC_DPP_JER_DRV_H
#define _SOC_DPP_JER_DRV_H

/* 
 * General defines
 */
#include <shared/bitop.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>

#define SOC_JER_CORE_FREQ_KHZ_DEFAULT 720000

/* Stracture access*/
#define SOC_DPP_JER_CONFIG(unit)        (SOC_DPP_CONFIG(unit)->jer)

typedef struct soc_jer_pll_config_s {
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmh_in;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmh_out;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pml_in[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pml_out[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_fabric_in[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_fabric_out[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_synce_out[2];
} soc_jer_pll_config_t;

typedef struct soc_jer_excluded_mem_bmap_s {
    SHR_BITDCL    excluded_mems_bmap[_SHR_BITDCLSIZE(NUM_SOC_MEM)];
} soc_jer_excluded_mem_bmap_t;


/* Jericho specific PP configuration */
typedef struct soc_dpp_config_jer_pp_s {
  /* Protection */
  uint8   protection_ingress_coupled_mode;          /* Coupled/Decoupled mode for Ingress Protection */
  uint8   protection_egress_coupled_mode;           /* Coupled/Decoupled mode for Egress Protection */
  uint8   protection_fec_accelerated_reroute_mode;  /* FEC Protection accelerated reroute mode */
  uint8   roo_host_arp_msbs;                        /* MSBs of arp pointer for ROO applications */
  uint32  kaps_public_ip_frwrd_table_size;          /* size of public forwarding table in number of entries */
  uint32  kaps_private_ip_frwrd_table_size;         /* size of private forwarding table in number of entries */
  uint32  kaps_large_db_size;                       /* size of direct access DB in number of entries */
  uint8   truncate_delta_in_pp_counter_0;           /* enable/disable packet size compensation for IRPP source in counter command 0 */
  uint8   truncate_delta_in_pp_counter_1;           /* enable/disable packet size compensation for IRPP source in counter command 1 */
} soc_dpp_config_jer_pp_t;

/* Jericho NIF configuration */

#define SOC_JERICHO_PM_4x25          6
#define SOC_JERICHO_PM_4x10          12
#define SOC_JERICHO_PM_QSGMII        4
#define SOC_JERICHO_PM_FABRIC        9
#define SOC_QMX_PM_FABRIC            4
#define SOC_QAX_PM_FABRIC            4
#define SOC_JERICHO_PLUS_PM_FABRIC   12
#define SOC_JERICHO_PLUS_PM_4x25     12
#define SOC_JERICHO_PLUS_PM_4x10     6
#define SOC_JERICHO_PLUS_PM_QSGMII   2
#define SOC_JERICHO_CPU_PHY_CORE_0 0
#define SOC_JERICHO_CPU_PHY_CORE_1 500
#define SOC_JERICHO_MAX_ILKN_PORTS_OVER_FABRIC 2
#define SOC_JERICHO_NOF_QMLFS (18)

/* each ILKN requires both pysical resources (serdes) and memory resources */
typedef struct soc_jer_ilkn_qmlf_resources_s{
    SHR_BITDCLNAME (serdes_qmlfs, SOC_JERICHO_NOF_QMLFS);
    SHR_BITDCLNAME (memory_qmlfs, SOC_JERICHO_NOF_QMLFS);
}soc_jer_ilkn_qmlf_resources_t;

typedef struct soc_jer_config_nif_s {
    int                                 fw_verify[SOC_DPP_DEFS_MAX(NOF_PMS)];
    int                                 ilkn_over_fabric[SOC_JERICHO_MAX_ILKN_PORTS_OVER_FABRIC];
    soc_jer_ilkn_qmlf_resources_t       ilkn_qmlf_resources[SOC_DPP_DEFS_MAX(NOF_INTERLAKEN_PORTS)];
} soc_jer_config_nif_t;

typedef struct soc_jer_tm_config_s {
    uint16 nof_remote_faps_with_remote_credit_value;
    SOC_TMC_ITM_CGM_MGMT_GUARANTEE_MODE cgm_mgmt_guarantee_mode;
    int is_ilkn_big_cal;
    int truncate_delta_in_pp_counter[NUM_OF_COUNTERS_CMDS]; /* enable/disable truncate delta (part of packet size compensation feature) in pp for crps use */
} soc_jer_tm_config_t;



/* Main Jericho configuration structure */
typedef struct soc_dpp_config_jer_s {
    soc_jer_ocb_dram_dbuffs_t   dbuffs;
    soc_jer_pll_config_t        pll;
    soc_dpp_config_jer_pp_t     pp;
    soc_jer_excluded_mem_bmap_t excluded_mems;
    soc_jer_config_nif_t        nif;
	soc_jer_tm_config_t         tm;
} soc_dpp_config_jer_t;

/* 
 * Init functions 
 */
void soc_restore_bcm88x7x(int unit, int core); 
int soc_dpp_jericho_init(int unit, int reset_action);
int soc_jer_device_reset(int unit, int mode, int action);
int soc_jer_specific_info_config_direct(int unit);
int soc_jer_specific_info_config_derived(int unit);
int soc_jer_init_reset(int unit, int reset_action);
int soc_jer_ports_config(int unit);
int soc_jer_info_config_custom_reg_access(int unit);

#endif /* !_SOC_DPP_JER_DRV_H  */

