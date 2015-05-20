/*
 * $Id: arad_wb_db.h,v 1.15 Broadcom SDK $
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
 * File:        arad_wb_db_lag.h
 * Purpose:     WarmBoot - Level 2 support (LAG Module)
 */

#ifndef __ARAD_WB_DB_INCLUDED__
#define __ARAD_WB_DB_INCLUDED__

#ifdef BCM_WARM_BOOT_SUPPORT


#include <soc/drv.h>

#include <soc/dpp/ARAD/arad_sw_db.h>

/*
 * runtime information
 */
typedef struct arad_wb_db_info_s {
    int                        init_done;
    ARAD_SW_DB_DEVICE         *arad_sw_db;

    int                        is_dirty;

    uint16                     version;
    uint8                     *scache_ptr;
    int                        size;



    uint32                   port_priority_cal_num;
    uint32                   port_priority_cal_off;
  
    uint32                   tcg_cal_num;
    uint32                   tcg_cal_off;
  
    uint32                   rates_num;
    uint32                   rates_off;
  
    uint32                   nof_calcal_instances_num;
    uint32                   nof_calcal_instances_off;
  
    uint32                   calcal_length_num;
    uint32                   calcal_length_off;
  
    uint32                   update_device_num;
    uint32                   update_device_off;

    uint32                   egq_tcg_qpair_shaper_enable_num;
    uint32                   egq_tcg_qpair_shaper_enable_off;

    uint32                   erp_interface_num;
    uint32                   erp_interface_off;

    uint32                   dsp_pp_to_base_queue_pair_mapping_num;
    uint32                   dsp_pp_to_base_queue_pair_mapping_off;
  
    uint32                   dsp_pp_nof_queue_pairs_num;
    uint32                   dsp_pp_nof_queue_pairs_off;  
      
    uint32                   ports_prog_editor_profile_num;
    uint32                   ports_prog_editor_profile_off;
  


    uint32                   in_use_num;
    uint32                   in_use_off;

    uint32                   local_to_sys_num;
    uint32                   local_to_sys_off;

    uint32                   local_to_reassembly_context_num;
    uint32                   local_to_reassembly_context_off;



   


    uint32                   current_cell_ident_num;
    uint32                   current_cell_ident_off;

    uint32                   context_map_num;
    uint32                   context_map_off;

    uint32                   deleted_buff_list_num;
    uint32                   deleted_buff_list_off;   

    uint32                   dram_deleted_buff_list_num;
    uint32                   dram_deleted_buff_list_off;

/* The following 6 variables are not used and remain for future use */
    uint32                   cmc_irq2_mask_num;
    uint32                   cmc_irq2_mask_off;
    uint32                   cmc_irq3_mask_num;
    uint32                   cmc_irq3_mask_off;
    uint32                   cmc_irq4_mask_num;
    uint32                   cmc_irq4_mask_off;
    uint32                   interrupt_flags_num;
    uint32                   interrupt_flags_off;
    uint32                   interrupt_storm_timed_count_num;
    uint32                   interrupt_storm_timed_count_off;
    uint32                   interrupt_storm_timed_period_num;
    uint32                   interrupt_storm_timed_period_off;

    uint32                   egress_groups_open_data_num;
    uint32                   egress_groups_open_data_off;

    uint32                   q_type_ref_count_num;
    uint32                   q_type_ref_count_off;

    uint32                   q_type_map_num;
    uint32                   q_type_map_off;

    uint32                   vsi_to_isid_num;
    uint32                   vsi_to_isid_off;

    uint32                   queue_to_rate_class_mapping_is_simple_num;
    uint32                   queue_to_rate_class_mapping_is_simple_off;

    uint32                   queue_to_rate_class_mapping_ref_count_num;
    uint32                   queue_to_rate_class_mapping_ref_count_off;

    uint32                   sysport2basequeue_num;
    uint32                   sysport2basequeue_off;

    uint32                   modport2sysport_num;
    uint32                   modport2sysport_off;
} arad_wb_db_info_t;

extern arad_wb_db_info_t   *_arad_wb_db_info_p[SOC_MAX_NUM_DEVICES];

/*
 * Functions
 */
extern uint32
arad_wb_db_init(SOC_SAND_IN int unit, 
                      SOC_SAND_IN ARAD_SW_DB_DEVICE *egr_port_config);

extern void
arad_wb_db_deinit(SOC_SAND_IN int unit);

extern uint32
arad_wb_db_sync(SOC_SAND_IN int unit);



extern uint32
arad_wb_db_egr_port_update_queue_rate_state(SOC_SAND_IN int unit,
                                       SOC_SAND_IN uint32 arr_indx);
extern uint32
arad_wb_db_egr_port_update_tcg_rate_state(SOC_SAND_IN int unit, 
                                       SOC_SAND_IN uint32 arr_indx);
extern uint32
arad_wb_db_egr_port_update_rates_state(SOC_SAND_IN int unit, 
                                       SOC_SAND_IN uint32 arr_indx1,
                                       SOC_SAND_IN uint32 arr_indx2);
extern uint32
arad_wb_db_egr_port_update_nof_calcal_instances_state(SOC_SAND_IN int unit, 
                                     SOC_SAND_IN uint32 arr_indx1,
                                     SOC_SAND_IN uint32 arr_indx2);
extern uint32
arad_wb_db_egr_port_update_calcal_length_state(SOC_SAND_IN int unit, 
                                               SOC_SAND_IN int core);
extern uint32
arad_wb_db_egr_port_update_egq_tcg_qpair_shaper_enable_state(SOC_SAND_IN int unit);

extern uint32
arad_wb_db_egr_port_update_dsp_pp_nof_queue_pairs_state(SOC_SAND_IN int unit, 
                                                           SOC_SAND_IN uint32 arr_indx1, SOC_SAND_IN uint32 arr_indx2);
extern uint32
arad_wb_db_egr_port_update_erp_interface_state(SOC_SAND_IN int unit);

extern uint32
arad_wb_db_egr_port_update_ports_prog_editor_profile_state(SOC_SAND_IN int unit, 
                                       SOC_SAND_IN uint32 arr_indx);


extern uint32
arad_wb_db_lag_update_in_use_state(SOC_SAND_IN int unit, 
                                SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_lag_update_local_to_sys_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_lag_update_local_to_reassembly_context_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_tdm_update_context_map_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_multicast_update_nof_unoccupied_state(SOC_SAND_IN int unit);

extern uint32
arad_wb_db_multicast_update_unoccupied_list_head_state(SOC_SAND_IN int unit);

extern uint32
arad_wb_db_multicast_update_next_unoccupied_ptr_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_multicast_update_backwards_ptr_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_multicast_update_eg_mult_nof_vlan_bitmaps_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 arr_indx);



extern uint32
arad_wb_db_cell_update_current_cell_ident_state(SOC_SAND_IN int unit);

extern uint32
arad_wb_db_dram_update_dram_deleted_buff_list_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 indx);

/* The following 3 functions are not used and remain for future use */
extern uint32
arad_wb_db_interrupts_update_cmc_irq2_mask_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 cmc);

extern uint32
arad_wb_db_interrupts_update_cmc_irq3_mask_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 cmc);

extern uint32
arad_wb_db_interrupts_update_cmc_irq4_mask_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 cmc);

extern uint32
arad_wb_db_interrupts_update_interrupt_flags_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 intr_id);

extern uint32
arad_wb_db_interrupts_update_interrupt_storm_timed_period_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 intr_id);

extern uint32
arad_wb_db_interrupts_update_interrupt_storm_timed_count_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 intr_id);

extern uint32
arad_wb_db_multicast_update_egress_groups_open_data_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_update_q_type_ref_count_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx);
extern uint32
arad_wb_db__update_q_type_map_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx);

extern uint32
arad_wb_db_vsi_update_isid_state(SOC_SAND_IN int unit, 
                                         SOC_SAND_IN uint32 arr_indx);

extern uint32 
arad_sw_db_tm_update_queue_to_rate_class_mapping_is_simple_state(SOC_SAND_IN int unit);

extern uint32 
arad_sw_db_tm_update_queue_to_rate_class_mapping_ref_count_state(SOC_SAND_IN int unit, 
                                                                 SOC_SAND_IN uint32 arr_indx);
extern uint32 
arad_sw_db_tm_update_sysport2basequeue_state(SOC_SAND_IN int unit, 
                                             SOC_SAND_IN uint32 arr_indx);
extern uint32 
arad_wb_db_update_modport2sysport_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 arr_indx);


#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* __ARAD_WB_DB_INCLUDED__ */
