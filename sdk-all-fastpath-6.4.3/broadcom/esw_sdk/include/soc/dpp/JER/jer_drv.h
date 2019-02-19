/*
 * $Id: dfe_drv.h Exp $
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
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>

#define SOC_JER_CORE_FREQ_KHZ_DEFAULT 720000

/* Stracture access*/
#define SOC_DPP_JER_CONFIG(unit)        (SOC_DPP_CONFIG(unit)->jer)

typedef struct soc_jer_pll_config_s {
    uint32 ref_clk_pmh;/*should be removed*/
    uint32 ref_clk_pml[2];/*should be removed*/
    uint32 ref_clk_fabric;/*should be removed*/
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmh_in;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pmh_out;
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pml_in[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_pml_out[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_fabric_in[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_fabric_out[2];
    soc_dcmn_init_serdes_ref_clock_t ref_clk_synce_out[2];
} soc_jer_pll_config_t;

typedef struct soc_jer_excluded_mem_bmap_s {
    SHR_BITDCLNAME( excluded_mems_bmap, NUM_SOC_MEM );
} soc_jer_excluded_mem_bmap_t;


/* Jericho specific PP configuration */
typedef struct soc_dpp_config_jer_pp_s {
  /* Protection */
  uint8   protection_ingress_coupled_mode;          /* Coupled/Decoupled mode for Ingress Protection */
  uint8   protection_egress_coupled_mode;           /* Coupled/Decoupled mode for Egress Protection */
  uint8   protection_fec_accelerated_reroute_mode;  /* FEC Protection accelerated reroute mode */
  uint8   roo_host_arp_msbs;                        /* MSBs of arp pointer for ROO applications */
} soc_dpp_config_jer_pp_t;

/* Jericho NIF configuration */

#define SOC_JERICHO_PM_4x25     6
#define SOC_JERICHO_PM_4x10     12
#define SOC_JERICHO_PM_FABRIC   9
#define SOC_JERICHO_NOF_PMS (SOC_JERICHO_PM_4x25 + SOC_JERICHO_PM_4x10 + SOC_JERICHO_PM_FABRIC)

typedef struct soc_jer_config_nif_s {
    int fw_verify[SOC_JERICHO_NOF_PMS];
} soc_jer_config_nif_t;

/* Main Jericho configuration structure */
typedef struct soc_dpp_config_jer_s {
    soc_jer_ocb_dram_dbuffs_t   dbuffs;
    soc_jer_pll_config_t        pll;
    soc_dpp_config_jer_pp_t     pp;
    soc_jer_excluded_mem_bmap_t excluded_mems;
    soc_jer_config_nif_t        nif;
} soc_dpp_config_jer_t;

/* 
 * Init functions 
 */
int soc_dpp_jericho_init(int unit, int reset_action);
int soc_jer_device_reset(int unit, int mode, int action);
int soc_jer_specific_info_config_direct(int unit);
int soc_jer_specific_info_config_derived(int unit);
int soc_jer_init_reset(int unit, int reset_action);
int soc_jer_ports_config(int unit);

#endif /* !_SOC_DPP_JER_DRV_H  */

