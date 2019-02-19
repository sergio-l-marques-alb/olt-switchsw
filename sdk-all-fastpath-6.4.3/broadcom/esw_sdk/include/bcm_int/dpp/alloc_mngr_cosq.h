/*
 * $Id: alloc_mngr_cosq.h,v 1.45 Broadcom SDK $
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
 * File:        alloc_mngr_cosq.h
 * Purpose:     Resource allocation for cosq.
 *
 */

#ifndef  INCLUDE_ALLOC_MNGR_COSQ_H
#define  INCLUDE_ALLOC_MNGR_COSQ_H

#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>

#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION 3
#define BCM_DPP_AM_RES_TYPE_COSQ_E2E_DYNAMIC_PER_REGION 3
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION_MAX_ENTRIES (DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit) * BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION)
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_CONNECTOR_OFFSET 0
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SE_OFFSET 1
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_SYNC_OFFSET 2
#define BCM_DPP_AM_RES_POOL_COSQ_E2E_TOTAL_DYNAMIC_PETRAB (DPP_DEVICE_PETRA_COSQ_TOTAL_FLOW_REGIONS*BCM_DPP_AM_RES_POOL_COSQ_E2E_DYNAMIC_PER_REGION)

#define BCM_DPP_AM_RES_POOL_COSQ_E2E_TOTAL_DYNAMIC_ARAD (DPP_DEVICE_ARAD_COSQ_TOTAL_FLOW_REGIONS*BCM_DPP_AM_RES_TYPE_COSQ_E2E_DYNAMIC_PER_REGION)
#define BCM_DPP_AM_RES_TYPE_COSQ_E2E_TOTAL_DYNAMIC_ARAD (DPP_DEVICE_ARAD_COSQ_TOTAL_FLOW_REGIONS*BCM_DPP_AM_RES_TYPE_COSQ_E2E_DYNAMIC_PER_REGION)

#define BCM_DPP_AM_RES_POOL_COSQ_QUEUE_TOTAL_DYNAMIC_PETRAB 32
#define BCM_DPP_AM_RES_TYPE_COSQ_QUEUE_TOTAL_DYNAMIC_PETRAB 32

#define BCM_DPP_AM_RES_POOL_COSQ_QUEUE_TOTAL_DYNAMIC_ARAD 96
#define BCM_DPP_AM_RES_TYPE_COSQ_QUEUE_TOTAL_DYNAMIC_ARAD 96


/* The resouces order is:
   - Normal resources
   - (Arad and above) lif pools
   - (Jer and above)  global lif pools
   - Cosq pools 
   */
#define DPP_AM_RES_COSQ_START(_unit)    \
    (SOC_IS_JERICHO(_unit) ? _BCM_DPP_AM_GLOBAL_LIF_INGRESS_RES_ID_END(_unit) : \
        SOC_IS_ARAD(_unit) ? dpp_res_arad_pool_ingress_lif_bank_end : dpp_am_res_count)

#define DPP_AM_RES_ID_IS_COSQ(_unit, _res_id)  \
    (_res_id >= DPP_AM_RES_COSQ_START(_unit))


/*
 * TM (COSQ)
 */
int 
_bcm_dpp_am_cosq_get_first_base_pool_id(int unit, int *curr_index);
int
bcm_dpp_am_cosq_init(int unit, CONST dpp_am_cosq_resource_info_t *resource_info_p);

int
bcm_dpp_am_cosq_deinit(int unit);

int
bcm_dpp_am_ingress_voq_allocate(int unit,
                                int core,
                                uint32 flags,
                                int is_non_contiguous,
                                int num_cos,
                                int queue_config_type,
                                int *voq_base);

int
bcm_dpp_am_ingress_voq_deallocate(int unit,
                                  int core,
                                  uint32 flags,
                                  int is_non_contiguous,
                                  int num_cos,
                                  int voq_base);

int
bcm_dpp_am_cosq_scheduler_allocate(int unit,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       uint8* src_modid,
                                       int *flow_id);
int
bcm_dpp_am_cosq_scheduler_deallocate(int unit,
                                       uint32 flags,
                                       int is_composite,
                                       int is_enhanced,
                                       int is_dual,
                                       int is_non_contiguous,
                                       int num_cos,
                                       SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                       int flow_id);
int 
bcm_dpp_am_cosq_get_region_type(int unit,
                           int flow_id,
                           int *region_type);

int 
bcm_dpp_am_cosq_fetch_quad(int unit,
                           int flow_id,
                           int region_type,
                           bcm_dpp_am_cosq_quad_allocation_t *is_allocated);


int     
bcm_dpp_am_cosq_fetch_allocated_resources(int unit,
                                        SOC_TMC_AM_SCH_FLOW_TYPE flow_type,
                                        bcm_dpp_am_cosq_pool_ref_t* allocated_ref);

int
bcm_dpp_am_cosq_queue_region_config_get(int unit,
                                        int core,
                                        int *queue_region_config,
                                        int queue_config_type);
int
_bcm_dpp_am_cosq_process_queue_region(int unit, 
                                      int core,
                                      int *queue_region_config);


int
_bcm_dpp_resources_fill_type_cosq(int unit, int core, _dpp_res_type_cosq_t* dpp_res_cosq_type);

int
_bcm_dpp_resources_fill_pool_cosq(int unit, int core, _dpp_res_pool_cosq_t* dpp_res_cosq_pool);

#endif /*INCLUDE_ALLOC_MNGR_COSQ_H*/
