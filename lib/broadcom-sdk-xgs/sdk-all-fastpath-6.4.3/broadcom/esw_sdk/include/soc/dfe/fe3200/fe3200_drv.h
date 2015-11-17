/*
 * $Id: fe3200_drv.h,v 1.1.2.1 Broadcom SDK $
 *
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
 * FE3200 DRV H
 */
 
#ifndef _SOC_FE3200_DRV_H_
#define _SOC_FE3200_DRV_H_

/**********************************************************/
/*                  Includes                              */
/**********************************************************/

#include <soc/error.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_defs.h>

/**********************************************************/
/*                  Defines                               */
/**********************************************************/

#define SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_0        (0xffffffff)
#define SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_1        (0x03ffffff)
#define SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_2        (0x00000000)

/**********************************************************/
/*                  Functions                             */
/**********************************************************/

int soc_fe3200_reset_device(int unit);
int soc_fe3200_drv_soc_properties_validate(int unit);
int soc_fe3200_drv_rings_map_set(int unit);
int soc_fe3200_drv_pll_config_set(int unit);
int soc_fe3200_drv_mdio_config_set(int unit);
int soc_fe3200_drv_pvt_monitor_enable(int unit);
int soc_fe3200_drv_mac_broadcast_id_get(int unit, int block_num,int *broadcast_id);
int soc_fe3200_bist_all(const int unit, const int skip_errors);
int soc_fe3200_drv_soft_init(int unit, uint32 soft_reset_mode_flags);
int soc_fe3200_drv_reg_access_only_reset(int unit);
int soc_fe3200_drv_sbus_broadcast_config(int unit);
int soc_fe3200_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap);
int soc_fe3200_drv_sw_ver_set(int unit);
int soc_fe3200_drv_temperature_monitor_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
int soc_fe3200_drv_test_reg_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe3200_drv_test_reg_default_val_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe3200_drv_test_mem_filter(int unit, soc_mem_t reg, int *is_filtered);
int soc_fe3200_drv_test_brdc_blk_filter(int unit, soc_reg_t reg, int *is_filtered);
int soc_fe3200_drv_test_brdc_blk_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size);
int soc_fe3200_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances);
int soc_fe3200_drv_asymmetrical_quad_get(int unit, int link, int *asymmetrical_quad);
int soc_fe3200_drv_fe13_isolate_set(int unit, soc_pbmp_t unisolated_links_pbmp, int isolate);
int soc_fe3200_drv_fe13_graceful_shutdown_set(int unit, soc_pbmp_t active_links, soc_pbmp_t unisolated_links, int shutdown);
int soc_fe3200_drv_mbist(int unit, int skip_errors);
int soc_fe3200_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp);


/**********************************************************/
/*                  Structures                            */
/**********************************************************/

typedef struct soc_dfe_drv_dch_default_thresholds_s{

	uint32 fifo_size[SOC_DFE_MAX_NOF_PIPES]; /* each entry represents a pipe */
    uint32 full_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 llfc_threshold[SOC_DFE_MAX_NOF_PIPES]; 
    uint32 mc_low_prio_threshold[SOC_DFE_MAX_NOF_PIPES];

} soc_dfe_drv_dch_default_thresholds_t;


typedef struct soc_dfe_drv_dcm_default_thresholds_s{

	uint32 fifo_size[SOC_DFE_MAX_NOF_PIPES];
    uint32 prio_0_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 prio_1_threshold[SOC_DFE_MAX_NOF_PIPES]; 
    uint32 prio_2_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 prio_3_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 full_threshold[SOC_DFE_MAX_NOF_PIPES];
    uint32 almost_full_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 gci_low_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 gci_med_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 gci_high_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 rci_low_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 rci_med_threshold[SOC_DFE_MAX_NOF_PIPES];
	uint32 rci_high_threshold[SOC_DFE_MAX_NOF_PIPES];

} soc_dfe_drv_dcm_default_thresholds_t;

typedef struct soc_dfe_drv_dcl_default_thresholds_s{

	uint32 fifo_size[3]; 
    uint32 llfc_threshold[3]; 
	uint32 almost_full_threshold[3];
	uint32 gci_low_threshold[3];
	uint32 gci_med_threshold[3];
	uint32 gci_high_threshold[3];
	uint32 rci_low_threshold[3];
	uint32 rci_med_threshold[3];
	uint32 rci_high_threshold[3];
    uint32 prio_0_threshold[3];
    uint32 prio_1_threshold[3]; 
    uint32 prio_2_threshold[3];
    uint32 prio_3_threshold[3];

} soc_dfe_drv_dcl_default_thresholds_t;


#endif /*!_SOC_FE3200_DRV_H_*/




