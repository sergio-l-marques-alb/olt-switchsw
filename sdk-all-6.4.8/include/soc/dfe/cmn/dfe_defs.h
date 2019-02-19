/*
 * $Id: dfe_defs.h,v 1.9 Broadcom SDK $
 *
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
 * DFE DEFS H
 */
#ifndef _SOC_DFE_FABRIC_DEFS_H_
#define _SOC_DFE_FABRIC_DEFS_H_

#include <soc/error.h>
#include <shared/port.h>
#include <shared/fabric.h>

/**********************************************************/
/*                     Defines                            */
/**********************************************************/
#define SOC_DFE_MODID_NOF           2048
#define SOC_DFE_MAX_NOF_PIPES       3
#define SOC_DFE_NOF_DCH             4
#define SOC_DFE_NOF_DCM             2
#define SOC_DFE_NOF_DCL             4
#define SOC_DFE_NOF_CCS             2
#define SOC_DFE_NOF_FIFOS_PER_DCM   4
#define SOC_DFE_MAX_NOF_LINKS       (144)
#define SOC_DFE_MAX_NOF_MAC         (36)
#define SOC_DFE_NOF_LCPLL           4

#define SOC_DFE_MODID_NOF_UINT32_SIZE   (SOC_DFE_MODID_NOF/32)
/**********************************************************/
/*                  Enumerators                           */
/**********************************************************/

typedef enum soc_dfe_fabric_link_cell_size_e
{
    soc_dfe_fabric_link_cell_size_VSC256_V1 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V1,
    soc_dfe_fabric_link_cell_size_VSC128 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC128,
    soc_dfe_fabric_link_cell_size_VSC256_V2 = _SHR_FABRIC_LINK_CELL_FORMAT_VSC256_V2
} soc_dfe_fabric_link_cell_size_t;

typedef enum soc_dfe_fabric_link_fifo_type_index_e
{
    soc_dfe_fabric_link_fifo_type_index_0 = 0,
    soc_dfe_fabric_link_fifo_type_index_1 = 1,
    soc_dfe_fabric_nof_link_fifo_types = 2
} soc_dfe_fabric_link_fifo_type_index_t;

typedef enum soc_dfe_fabric_device_mode_e
{
    soc_dfe_fabric_device_mode_single_stage_fe2 = 0,
    soc_dfe_fabric_device_mode_multi_stage_fe2, 
    soc_dfe_fabric_device_mode_multi_stage_fe13,
    soc_dfe_fabric_device_mode_repeater,
    soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric
} soc_dfe_fabric_device_mode_t;

typedef enum soc_dfe_fabric_priority_e
{
    soc_dfe_fabric_priority_0 = 0,
    soc_dfe_fabric_priority_1, 
    soc_dfe_fabric_priority_2,
    soc_dfe_fabric_priority_3,
    soc_dfe_fabric_priority_nof
} soc_dfe_fabric_priority_t;

typedef enum soc_dfe_multicast_mode_e
{
    soc_dfe_multicast_mode_direct = 0,
    soc_dfe_multicast_mode_indirect = 1
} soc_dfe_multicast_mode_t;

typedef enum soc_dfe_load_balancing_mode_e{
    soc_dfe_load_balancing_mode_normal    = 0,
    soc_dfe_load_balancing_mode_destination_unreachable,    
    soc_dfe_load_balancing_mode_balanced_input    
} soc_dfe_load_balancing_mode_t;


typedef enum soc_dfe_multicast_table_mode_e{
    soc_dfe_multicast_table_mode_64k    = 0, /*8x64  mode, 8 parallel accesses to the multicast table can be handled for fe1600, 4(parallel access)x64(mc id)x144(bits per id) for fe3200*/
    soc_dfe_multicast_table_mode_128k   = 1,/*4x128 mode, 4 parallel accesses to the multicast table can be handled, 2(parallel access)x128(mc id)x144(bits per id) for fe3200*/     
	soc_dfe_multicast_table_mode_128k_half = 2 /*4(parallel access)x128(multicast id)x72(bits per id ) mode - only fe3200*/
} soc_dfe_multicast_table_mode_t;



typedef enum soc_dfe_fabric_isolate_type_e {
    soc_dfe_fabric_isolate_type_none = 0,
    soc_dfe_fabric_isolate_type_isolate = 1,
    soc_dfe_fabric_isolate_type_shutdown = 2
} soc_dfe_fabric_isolate_type_t;

typedef enum soc_dfe_cosq_weight_mode_e
{
    soc_dfe_cosq_weight_mode_regular = 0,
    soc_dfe_cosq_weight_mode_dynamic_0 = 1,
    soc_dfe_cosq_weight_mode_dynamic_1 = 2
} soc_dfe_cosq_weight_mode_t;

typedef enum soc_dfe_cosq_shaper_mode_e
{
    soc_dfe_cosq_shaper_mode_byte = 0,
    soc_dfe_cosq_shaper_mode_packet = 1
} soc_dfe_cosq_shaper_mode_t;


#define SOC_DFE_MODID_LOCAL_NOF(unit)   (SOC_DFE_IS_FE13(unit) ? SOC_DFE_DEFS_GET(unit, nof_local_modid_fe13) : SOC_DFE_DEFS_GET(unit, nof_local_modid))
#define SOC_DFE_MODID_GROUP_NOF(unit)   (SOC_DFE_IS_FE13(unit) ? SOC_DFE_DEFS_GET(unit, nof_local_modid_fe13) : SOC_DFE_DEFS_GET(unit, nof_local_modid))

#endif /*_SOC_DFE_FABRIC_DEFS_H_*/

