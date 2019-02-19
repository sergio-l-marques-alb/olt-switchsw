/* $Id: pb_tbl_access.h,v 1.7 Broadcom SDK $
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
*/

#ifndef __SOC_PB_TBL_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PB_TBL_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

#include <soc/dpp/Petra/petra_api_general.h>
/* 
 * soc_pb_tbl_access uses function defined in soc_petra_tbl_access.h, and vice versa. 
 * In order not to restuct the code, we need to include one from the other. 
 * 'coverity[include_recursion : FALSE]' doesn't work. Also added __SOC_PETRA_TBL_ACCESS_H_INCLUDED__
 * to ignore the issue. 
 */ 
#ifndef __SOC_PETRA_TBL_ACCESS_H_INCLUDED__ 
#include <soc/dpp/Petra/petra_tbl_access.h>
#endif
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_cnt.h>

#include <soc/dpp/Petra/PB_TM/pb_api_flow_control.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_NUM_OF_INDIRECT_MODULES  SOC_PB_NOF_MODULES

#define SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE  17

#define SOC_PB_IRR_GLAG_DEVISION_HASH_NOF_BITS  8

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PB_TBL(tbl_ptr)           tables->b_##tbl_ptr

#define SOC_PB_TBL_REF(tbl_ptr)       &(SOC_PB_TBL(tbl_ptr))

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint32 fap_port;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 contexts_bit_mapping[3];
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA;

typedef struct
{
  uint32 fap_port;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 mode;
  uint32 cpu;
  uint32 header[2];
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRE_TDM_CONFIG_TBL_DATA;

typedef struct
{
  uint32 org_size;
  uint32 size;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IDR_CONTEXT_MRU_TBL_DATA;



typedef struct
{
  uint32 rate_mantissa;
  uint32 rate_exp;
  uint32 burst_mantissa;
  uint32 burst_exp;
  uint32 packet_mode;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA;

typedef struct
{
  uint32 enable;
  uint32 time_stamp;
  uint32 level;
  uint32 reserved;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA;

typedef struct
{
  uint32 profile;
  uint32 global_meter_ptr;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA;

typedef struct
{
  uint32 rate_mantissa;
  uint32 rate_exp;
  uint32 burst_mantissa;
  uint32 burst_exp;
  uint32 packet_mode;
  uint32 enable;
} __ATTRIBUTE_PACKED__ SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 level;
} __ATTRIBUTE_PACKED__ SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA;


typedef struct
{
  uint32 multicast_bitmap_ptr;
  uint32 out_port;
  uint32 out_lif[2];
  uint32 format_select;
  uint32 link_ptr;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA;

typedef struct
{
  uint32 multicast_bitmap_ptr[2];
  uint32 out_lif[2];
  uint32 format_select;
  uint32 link_ptr;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 tc;
  uint32 tc_ow;
  uint32 dp;
  uint32 dp_ow;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA;

typedef struct
{
  uint32 meter_ptr0;
  uint32 meter_ptr0_ow;
  uint32 meter_ptr1;
  uint32 meter_ptr1_ow;
  uint32 counter_ptr0;
  uint32 counter_ptr0_ow;
  uint32 counter_ptr1;
  uint32 counter_ptr1_ow;
  uint32 dp_cmd;
  uint32 dp_cmd_ow;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 pcp;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_FREE_PCB_MEMORY_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 pcp;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_PCB_LINK_TABLE_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 is_pcp;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 is_pcp;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 pointer;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_RPF_MEMORY_TBL_DATA;

typedef struct
{
  uint32 descriptor[3];
  uint32 ecc;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_MCR_MEMORY_TBL_DATA;

typedef struct
{
  uint32 descriptor[2];
  uint32 ecc;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_ISF_MEMORY_TBL_DATA;

typedef struct
{
  uint32 queue_number;
  uint32 queue_valid;
  uint32 tc_profile;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA;

typedef struct
{
  uint32 range;
  uint32 mode;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_LAG_MAPPING_TBL_DATA;

typedef struct
{
  uint32 offset;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA;

typedef struct
{
  uint32 member;
  uint32 parity;
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA;

typedef struct
{
  /*
   *	For each old traffic class, mapping to a new traffic class.
   */
  uint32 traffic_class_mapping[SOC_PETRA_NOF_TRAFFIC_CLASSES];
} __ATTRIBUTE_PACKED__ SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA;

typedef struct
{
  uint32 lag_port_mine0;
  uint32 lag_port_mine1;
  uint32 lag_port_mine2;
  uint32 lag_port_mine3;
} __ATTRIBUTE_PACKED__ SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA;

typedef struct
{
  uint32 pp_port_offset1;
  uint32 pp_port_offset2;
  uint32 pp_port_profile;
  uint32 pp_port_use_offset_directly;
} __ATTRIBUTE_PACKED__ SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA;

typedef struct
{
  uint32 system_port_offset1;
  uint32 system_port_offset2;
  uint32 system_port_value;
  uint32 system_port_profile;
  uint32 system_port_value_to_use;
} __ATTRIBUTE_PACKED__ SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA;

typedef struct
{
  uint32 pp_port_system_port_value;
} __ATTRIBUTE_PACKED__ SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA;



typedef struct
{
  uint32 qdr_dll_mem;
} __ATTRIBUTE_PACKED__ SOC_PB_QDR_QDR_DLL_MEM_TBL_DATA;


typedef struct
{
  uint32 cp_enable;
  uint32 cp_class;
  uint32 cp_id;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA;

typedef struct
{
  uint32 cp_enqued1;
  uint32 cp_qsize_old;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA;

typedef struct
{
  uint32 cp_qeq;
  uint32 cp_w;
  uint32 cp_fb_max_val;
  uint32 cp_quant_div;
  uint32 cp_sample_base[SOC_PB_CNM_CP_SAMPLE_BASE_SIZE];
  uint32 cp_fixed_sample_base;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA;

typedef struct
{
  uint32 iqm_dp;
  uint32 etm_de;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA;

typedef struct
{
  uint32 profile;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_PRFSEL_TBL_DATA;

typedef struct
{
  uint32 color_aware;
  uint32 coupling_flag;
  uint32 fairness_mode;
  uint32 cir_mantissa_64;
  uint32 cir_reverse_exponent;
  uint32 reset_cir;
  uint32 cbs_mantissa_64;
  uint32 cbs_exponent;
  uint32 eir_mantissa_64;
  uint32 eir_reverse_exponent;
  uint32 reset_eir;
  uint32 ebs_mantissa_64;
  uint32 ebs_exponent;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_PRFCFG_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_NORMAL_DYNAMICA_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_NORMAL_DYNAMICB_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_HIGH_DYNAMICA_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_HIGH_DYNAMICB_TBL_DATA;



typedef struct
{
  uint32 packets_counter;
  uint32 octets_counter;
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNTS_MEM_TBL_DATA;

typedef struct
{
  uint32 ovth_counter_bits[2];
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_OVTH_MEMA_TBL_DATA;

typedef struct
{
  uint32 ovth_counter_bits[2];
} __ATTRIBUTE_PACKED__ SOC_PB_IQM_OVTH_MEMB_TBL_DATA;

typedef struct
{
  uint32 sop_mmu[3];
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_SOP_MMU_TBL_DATA;


typedef struct
{
  uint32 cfg_byte_cnt[2];
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_CFG_BYTE_CNT_TBL_DATA;

typedef struct
{
  uint32 egq_txq_wr_addr;
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_DATA;

typedef struct
{
  uint32 egq_txq_rd_addr;
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA;

typedef struct
{
  uint32 dtq0_wr_addr;
  uint32 dtq1_wr_addr;
  uint32 dtq2_wr_addr;
  uint32 dtq3_wr_addr;
  uint32 dtq4_wr_addr;
  uint32 dtq5_wr_addr;
  uint32 dtq6_wr_addr;
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_DATA;

typedef struct
{
  uint32 dtq0_rd_addr;
  uint32 dtq1_rd_addr;
  uint32 dtq2_rd_addr;
  uint32 dtq3_rd_addr;
  uint32 dtq4_rd_addr;
  uint32 dtq5_rd_addr;
  uint32 dtq6_rd_addr;
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_DATA;

typedef struct
{
  uint32 mask0;
  uint32 mask1;
  uint32 mask2;
  uint32 mask3;
} __ATTRIBUTE_PACKED__ SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_DATA;


typedef struct
{
  uint32 ipt_contro_l_fifo[2];
} __ATTRIBUTE_PACKED__ SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_DATA;

typedef struct
{
  uint32 read_pointer;
  uint32 packet_size256to_eop;
  uint32 counter_decreament;
  uint32 copy_data;
  uint32 last_seg_size;
  uint32 before_last_seg_size;
  uint32 pqp_qnum;
  uint32 pqp_oc768_qnum;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_QM_TBL_DATA;

typedef struct
{
  uint32 qsm;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_QSM_TBL_DATA;

typedef struct
{
  uint32 dcm;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_DCM_TBL_DATA;

typedef struct
{
  uint32 mc_high_queue_weight;
  uint32 uc_high_queue_weight;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_DWM_NEW_TBL_DATA;


typedef struct
{
  uint32 prog_editor_value;
  uint32 out_pp_port;
  uint32 outbound_mirr;
  uint32 dest_port;
  uint32 port_profile;
  uint32 port_ch_num;
  uint32 port_type;
  uint32 cr_adjust_type;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PCT_TBL_DATA;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint32 map_profile; /* 2b */
  uint32 is_egr_mc; /* 1b */
  uint32 is_syst_mc; /* 1b */
  uint32 tc; /* 3b */
  uint32 dp; /* 2b */

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY;

typedef struct
{
  uint32 tc;
  uint32 dp;
} SOC_PB_EGQ_TC_DP_MAP_TBL_DATA;

typedef struct
{
  uint32 fqp_nif_port_mux;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA;

typedef struct
{
  uint32 key_profile_map_index;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA;

typedef struct
{
  uint32 key_select;
  uint32 key_and_value;
  uint32 key_or_value;
  uint32 tcam_profile;
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA;

typedef struct
{
  uint32 parser_last_sys_record[8];
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA;

typedef struct
{
  uint32 parser_last_nwk_record1[8];
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA;

typedef struct
{
  uint32 parser_last_nwk_record2[2];
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA;

typedef struct
{
  uint32 erpp_debug[8];
} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA;

typedef struct
{
  uint32 counter_compension;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_PINFO_LBP_TBL_DATA;


typedef struct
{
  uint32 port_pmf_profile;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_PINFO_PMF_TBL_DATA;

typedef struct
{
  uint32 packet_format_code_profile;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA;

typedef struct
{
  uint32 source_port_min;
  uint32 source_port_max;
  uint32 destination_port_min;
  uint32 destination_port_max;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA;

typedef struct
{
  uint32 direct_key_select;
  uint32 direct_db_and_value;
  uint32 direct_db_or_value;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA;

/* $Id: pb_tbl_access.h,v 1.7 Broadcom SDK $
 *	Generic FEM table data
 */
typedef struct
{
  uint32 program;
  uint32 key_select;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA;

typedef struct
{
  uint32 bit_select;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA;

typedef struct
{
  uint32 map_index;
  uint32 map_data;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 action_type;
  uint32 field_select_map[SOC_PB_IHB_FEM_MAX_OUTPUT_SIZE];
  uint32 offset_index;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA;

typedef struct
{
  uint32 offset;
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA;

/*
 *	End of generic
 */



typedef struct
{
  uint32 inst_valid[2];
  uint32 inst_source_select[2];
  uint32 inst_header_offset_select[2];
  uint32 inst_niblle_field_offset[2];
  uint32 inst_bit_count[2];
  uint32 inst_lfem_program[2];
} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA;

typedef struct
{
  uint32 lfem_field_select_map[16];
} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA;



typedef struct
{
  uint32 ofp_num;
  uint32 sch_hp_valid;
  uint32 sch_lp_valid;
  uint32 egq_hp_valid;
  uint32 egq_lp_valid;
} __ATTRIBUTE_PACKED__ SOC_PB_CFC_RCL2_OFP_TBL_DATA;

typedef struct
{
  struct
  {
    uint32 ofp_num;
    struct
    {
      uint32 hp_valid;
      uint32 lp_valid;
    }sch;
    struct
    {
      uint32 hp_valid;
      uint32 lp_valid;
    }egq;
  }data[4];
} __ATTRIBUTE_PACKED__ SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_dest_sel;
} __ATTRIBUTE_PACKED__ SOC_PB_CFC_CALRX_TBL_DATA;

typedef struct
{
  uint32 ofp_hr;
  uint32 lp_ofp_valid;
  uint32 hp_ofp_valid;
} __ATTRIBUTE_PACKED__ SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_source_sel;
} __ATTRIBUTE_PACKED__ SOC_PB_CFC_CALTX_TBL_DATA;


/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */


/*
 *  Set number of repetitions for table write for MMU block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_pb_mmu_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

/*
 *  Set number of repetitions for table write for FDT block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_pb_fdt_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

/*
 *  Set number of repetitions for table write for NBI block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_pb_ihb_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );



/*
 * Read indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table nif_port_to_ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table nif_port_to_ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_rcy_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_rcy_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table tdm_config_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_tdm_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table tdm_config_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_tdm_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table context_mru_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_context_mru_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table context_mru_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_context_mru_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  );


/*
 * Read indirect table ethernet_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ethernet_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_idr_ethernet_meter_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_idr_ethernet_meter_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ethernet_meter_config_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ethernet_meter_config_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table global_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table global_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table global_meter_status_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table global_meter_status_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA  *tbl_data
  );


uint32
  soc_pb_irr_mcdb_egress_format_b_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_mcdb_egress_format_b_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_mcdb_egress_format_c_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table snoop_mirror_table0_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table snoop_mirror_table0_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table snoop_mirror_table1_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table snoop_mirror_table1_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_free_pcb_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_free_pcb_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_pcb_link_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_pcb_link_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_is_free_pcb_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_is_free_pcb_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_is_pcb_link_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_is_pcb_link_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_rpf_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_RPF_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_rpf_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_RPF_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_mcr_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_MCR_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_mcr_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_MCR_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_isf_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_ISF_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_irr_isf_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_ISF_MEMORY_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_destination_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_destination_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table lag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table lag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table lag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT SOC_PB_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table lag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN  SOC_PB_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table lag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_next_member_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table lag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_next_member_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_smooth_division_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_OUT SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_smooth_division_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_IN  SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table traffic_class_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table traffic_class_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_traffic_class_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihp_port_mine_table_lag_port_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihp_port_mine_table_lag_port_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table tm_port_pp_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table tm_port_pp_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table tm_port_sys_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table tm_port_sys_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table pp_port_values_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_pp_port_values_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table pp_port_values_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_pp_port_values_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  );


/*
 * Read indirect table cnm_descriptor_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_static_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table cnm_descriptor_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_static_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table cnm_descriptor_dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_dynamic_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table cnm_descriptor_dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_dynamic_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table cnm_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table cnm_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table meter_processor_result_resolve_table_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table meter_processor_result_resolve_table_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table prfselb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfsel_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_PRFSEL_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table prfsel_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfsel_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_PRFSEL_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table prfcfg_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfcfg_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_PRFCFG_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table prfcfg_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfcfg_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_PRFCFG_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table normal_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamica_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_NORMAL_DYNAMICA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table normal_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamica_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_NORMAL_DYNAMICA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table normal_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamicb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_NORMAL_DYNAMICB_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table normal_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamicb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_NORMAL_DYNAMICB_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table high_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamica_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_HIGH_DYNAMICA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table high_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamica_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_HIGH_DYNAMICA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table high_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamicb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_HIGH_DYNAMICB_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table high_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamicb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_HIGH_DYNAMICB_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table cnts_mem_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnts_mem_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID   proc_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNTS_MEM_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table cnts_mem_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnts_mem_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID   proc_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNTS_MEM_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ovth_mema_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_mema_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_OVTH_MEMA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ovth_mema_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_mema_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_OVTH_MEMA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ovth_memb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_memb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_OVTH_MEMB_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ovth_memb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_memb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_OVTH_MEMB_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ipt_sop_mmu_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_SOP_MMU_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ipt_sop_mmu_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_SOP_MMU_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_cfg_byte_cnt_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_CFG_BYTE_CNT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_cfg_byte_cnt_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_CFG_BYTE_CNT_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_egq_txq_wr_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_egq_txq_wr_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_EGQ_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_egq_txq_rd_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_egq_txq_rd_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_fdt_txq_wr_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_fdt_txq_wr_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_fdt_txq_rd_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_fdt_txq_rd_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_gci_backoff_mask_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ipt_gci_backoff_mask_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ipt_contro_l_fifo_tbl from block FDT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_fdt_ipt_contro_l_fifo_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_DATA  *tbl_data
  );


/*
 * Read indirect table qm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_QM_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table qm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_QM_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table qsm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qsm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_QSM_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table qsm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qsm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_QSM_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table dcm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dcm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_DCM_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table dcm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dcm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_DCM_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table dwm_new_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dwm_new_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_DWM_NEW_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table dwm_new_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dwm_new_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_DWM_NEW_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_pct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PCT_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_pct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PCT_TBL_DATA  *tbl_data
  );


void
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *info
  );
/*
 * Read indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tc_dp_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
    SOC_SAND_OUT SOC_PB_EGQ_TC_DP_MAP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tc_dp_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
    SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_DATA  *tbl_data
  );


/*
 * Read indirect table fqp_nif_port_mux_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_fqp_nif_port_mux_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fqp_nif_port_mux_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_fqp_nif_port_mux_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  );


/*
 * Read indirect table key_profile_map_index_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_key_profile_map_index_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fwd_type_ndx,
    SOC_SAND_IN  uint32              acl_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table key_profile_map_index_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_key_profile_map_index_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fwd_type_ndx,
    SOC_SAND_IN  uint32              acl_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table tcam_key_resolution_profile_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tcam_key_resolution_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table tcam_key_resolution_profile_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tcam_key_resolution_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_parser_last_sys_record_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_parser_last_sys_record_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_parser_last_nwk_record1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_parser_last_nwk_record1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_parser_last_nwk_record2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_parser_last_nwk_record2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_erpp_debug_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_egq_erpp_debug_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table pinfo_pmf_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_pinfo_lbp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_pinfo_lbp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  );


uint32
  soc_pb_ihb_pinfo_pmf_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table pinfo_pmf_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_pinfo_pmf_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table packet_format_code_profile_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_packet_format_code_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table packet_format_code_profile_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_packet_format_code_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table src_dest_port_for_l3_acl_key_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table src_dest_port_for_l3_acl_key_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA  *tbl_data
  );



/*
 *	Generic access to the FEM table
 */
uint32
  soc_pb_ihb_fem_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 *	Symbolic function for non existing tables. Return error
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem1_4b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem0_4b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem1_4b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem0_4b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem1_4b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem0_4b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_ihb_fem1_4b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );



/*
 * Read indirect table direct_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table direct_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table direct_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table direct_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem0_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem0_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem1_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem1_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem2_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem2_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem3_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem3_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem4_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem4_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem5_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem5_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem6_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem6_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table fem7_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table fem7_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );




/*
 * Read indirect table copy_engine0_program_tbl from block EPNI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_epni_copy_engine_program_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table copy_engine0_program_tbl from block EPNI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_epni_copy_engine_program_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_epni_lfem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  soc_pb_epni_lfem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table rcl2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_rcl2_ofp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_RCL2_OFP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table rcl2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_rcl2_ofp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_RCL2_OFP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table nifclsb2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_nifclsb2_ofp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table nifclsb2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_nifclsb2_ofp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table calrxb_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table calrxb_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table oob0_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_sch_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table oob0_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_sch_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ilkn_calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ilkn_calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ilkn_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_sch_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ilkn_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_sch_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );

/*
 * Read indirect table ilkn_caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  );

/*
 * Write indirect table ilkn_caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_TBL_ACCESS_H_INCLUDED__*/
#endif
