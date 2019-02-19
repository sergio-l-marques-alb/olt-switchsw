
/* $Id: soc_petra_shadow.h,v 1.6 Broadcom SDK $
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
*/


#ifndef __SOC_PETRA_SHADOW_INCLUDED__
/* { */
#define __SOC_PETRA_SHADOW_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/petra_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_SHD_ADDR_INVALID	0xffffffff

/* 
 * This value can be used as info->nof_entries for soc_petra_shd_scrub_range API,
 * to indicate that a all tables, and all entries at each table,
 * should be scrubbed (single iteration)
 */
#define SOC_PETRA_SHD_SCRUB_NOF_ENTRIES_ALL  0xFFFFFFFF

/* table index definitions { */
typedef enum
{
  /*
   *  IRE
   */
  SOC_PETRA_SHD_IRE_NIF_CTXT_MAP_TBL_ID,
  SOC_PETRA_SHD_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID,
  SOC_PETRA_SHD_IRE_RCY_CTXT_MAP_TBL_ID,

  /*
   *  IRR
   */
  SOC_PETRA_SHD_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID,
  SOC_PETRA_SHD_IRR_REPLICATION_MULTICAST_DB_TBL_ID, /* Also contains the egress DB */
  SOC_PETRA_SHD_IRR_MIRROR_TABLE_TBL_ID,
  SOC_PETRA_SHD_IRR_SNOOP_TABLE_TBL_ID,
  SOC_PETRA_SHD_IRR_GLAG_TO_LAG_RANGE_TBL_ID,
  SOC_PETRA_SHD_IRR_GLAG_MAPPING_TBL_ID,
  SOC_PETRA_SHD_IRR_SMOOTH_DIVISION_TBL_ID,
  SOC_PETRA_SHD_IRR_DESTINATION_TABLE_TBL_ID,
  SOC_PETRA_SHD_IRR_GLAG_NEXT_MEMBER_TBL_ID,
  SOC_PETRA_SHD_IRR_RLAG_NEXT_MEMBER_TBL_ID,

  /*
   *  IHP
   */
  SOC_PETRA_SHD_IHP_PORT_INFO_TBL_ID,
  SOC_PETRA_SHD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID,
  SOC_PETRA_SHD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID,
  SOC_PETRA_SHD_IHP_PTC_COMMANDS1_TBL_ID,
  SOC_PETRA_SHD_IHP_PTC_COMMANDS2_TBL_ID,
  SOC_PETRA_SHD_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID,
  SOC_PETRA_SHD_IHP_KEY_PROGRAM0_TBL_ID,
  SOC_PETRA_SHD_IHP_KEY_PROGRAM1_TBL_ID,
  SOC_PETRA_SHD_IHP_KEY_PROGRAM2_TBL_ID,
  SOC_PETRA_SHD_IHP_KEY_PROGRAM3_TBL_ID,
  SOC_PETRA_SHD_IHP_KEY_PROGRAM4_TBL_ID,
  SOC_PETRA_SHD_IHP_PROGRAMMABLE_COS_TBL_ID,
  /* SOC_PETRA_SHD_IHP_PROGRAMMABLE_COS1_TBL_ID, */
  
  /*
   *  IQM
   */
  SOC_PETRA_SHD_IQM_STATIC_TBL_ID,
  SOC_PETRA_SHD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID,
  SOC_PETRA_SHD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID,
  SOC_PETRA_SHD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_A_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_B_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_C_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_D_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_A_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_B_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_C_TBL_ID,
  SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_D_TBL_ID,
  SOC_PETRA_SHD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID,
  SOC_PETRA_SHD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID,
  SOC_PETRA_SHD_IQM_SYSTEM_RED_TBL_ID,

  /*
   *  IPS
   */
  SOC_PETRA_SHD_IPS_SYSTEM_PHYSICAL_PORT_LUT_TBL_ID,
  SOC_PETRA_SHD_IPS_DEST_DEVICE_AND_PORT_LUT_TBL_ID,
  SOC_PETRA_SHD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID,
  SOC_PETRA_SHD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID,
  SOC_PETRA_SHD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID,
  SOC_PETRA_SHD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID,
  SOC_PETRA_SHD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TBL_ID,
  SOC_PETRA_SHD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TBL_ID,
  SOC_PETRA_SHD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TBL_ID,
  SOC_PETRA_SHD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TBL_ID,

  /*
   *  EGQ
   */
  SOC_PETRA_SHD_EGQ_NIFA_CH0_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFA_CH1_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFA_CH2_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFA_CH3_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFB_CH0_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFB_CH1_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFB_CH2_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFB_CH3_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_NIFAB_NCH_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_RCY_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_CPU_SCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_CCM_TBL_ID,
  SOC_PETRA_SHD_EGQ_PMC_TBL_ID,
  SOC_PETRA_SHD_EGQ_DWM_TBL_ID,
  SOC_PETRA_SHD_EGQ_PCT_TBL_ID,
  SOC_PETRA_SHD_EGQ_VLAN_TABLE_TBL_ID,

  /*
   *  CFC
   */
  SOC_PETRA_SHD_CFC_RCY_TO_OFP_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_OOB_A_SCH_BASED_FC_TO_OFP_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_OOB_B_SCH_BASED_FC_TO_OFP_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID,
  SOC_PETRA_SHD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID,

  /*
   *  SCH
   */
  SOC_PETRA_SHD_SCH_CAL_NIF0_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF0_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF1_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF1_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF2_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF2_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF3_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF3_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF4_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF4_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF5_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF5_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF6_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF6_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF7_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_NIF7_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_CPU_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_CPU_B_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_RCY_A_TBL_ID,
  SOC_PETRA_SHD_SCH_CAL_RCY_B_TBL_ID,
  SOC_PETRA_SHD_SCH_DRM_TBL_ID,
  SOC_PETRA_SHD_SCH_DSM_TBL_ID,
  SOC_PETRA_SHD_SCH_FDMS_TBL_ID,
  SOC_PETRA_SHD_SCH_SHDS_TBL_ID,
  SOC_PETRA_SHD_SCH_SEM_TBL_ID,
  SOC_PETRA_SHD_SCH_FSF_TBL_ID,
  SOC_PETRA_SHD_SCH_FGM_TBL_ID,
  SOC_PETRA_SHD_SCH_SHC_TBL_ID,
  SOC_PETRA_SHD_SCH_SCC_TBL_ID,
  SOC_PETRA_SHD_SCH_SCT_TBL_ID,
  SOC_PETRA_SHD_SCH_FQM_TBL_ID,
  SOC_PETRA_SHD_SCH_FFM_TBL_ID,

#ifdef PETRA_PP
  /*
   *  OLP
   */
  SOC_PETRA_SHD_OLP_PGE_MEM_TBL_ID,

  /*
   *  IHP
   */
  SOC_PETRA_SHD_IHP_ETH_PORT_INFO1_TBL_ID,
  SOC_PETRA_SHD_IHP_ETH_PORT_INFO2_TBL_ID,
  SOC_PETRA_SHD_IHP_CID_INFO_TBL_ID,
  SOC_PETRA_SHD_IHP_CID_TO_TOPOLOGY_TBL_ID,
  SOC_PETRA_SHD_IHP_STP_TBL_ID,
  SOC_PETRA_SHD_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID,
  SOC_PETRA_SHD_IHP_CID_INDEX_TO_CID_TBL_ID,
  SOC_PETRA_SHD_IHP_BRIDGE_CONTROL_TO_FWD_PARAMS_TBL_ID,
  SOC_PETRA_SHD_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID,
  SOC_PETRA_SHD_IHP_CID_PORT_MEMBERSHIP_TBL_ID,
  SOC_PETRA_SHD_IHP_IS_CID_SHARED_TBL_ID,
  SOC_PETRA_SHD_IHP_TOS_TO_TC_TBL_ID,
  SOC_PETRA_SHD_IHP_FEC_TBL_ID,
  SOC_PETRA_SHD_IHP_SMOOTH_DIVISION_TBL_ID,
  SOC_PETRA_SHD_IHP_LPM0_TBL_ID,
  SOC_PETRA_SHD_IHP_LPM1_TBL_ID,
  SOC_PETRA_SHD_IHP_LPM2_TBL_ID,
  SOC_PETRA_SHD_IHP_TCAM_ACTION_A_TBL_ID,
  SOC_PETRA_SHD_IHP_TCAM_ACTION_B_TBL_ID,
  SOC_PETRA_SHD_IHP_TCAM_ACTION_C_TBL_ID,
  SOC_PETRA_SHD_IHP_TCAM_ACTION_D_TBL_ID,
  SOC_PETRA_SHD_IHP_TOS_TO_COS_TBL_ID,
  SOC_PETRA_SHD_IHP_IPV6_TO_COS_TBL_ID,
  SOC_PETRA_SHD_IHP_CID_SUBNET_TBL_ID,
  SOC_PETRA_SHD_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID,

  /*
   *  EGQ
   */
  SOC_PETRA_SHD_EGQ_PPCT_TBL_ID,
  SOC_PETRA_SHD_EGQ_STP_TBL_ID,

  /*
   *  EPNI
   */
  SOC_PETRA_SHD_EPNI_ARP_TBL_ID,
  SOC_PETRA_SHD_EPNI_PTT_TBL_ID,
  SOC_PETRA_SHD_EPNI_PROCESSOR_CFG_PARAMS_TBL_ID,
#endif

  /*
   *  Keep last
   */
  SOC_PETRA_SHD_TBL_ID_LAST
}SOC_PETRA_SHD_TBL_IDS;


typedef enum
{
  SOC_PETRA_SHD_SCRUB_LVL1, /* The highest priority for scrubbing */
  SOC_PETRA_SHD_SCRUB_LVL2,
  SOC_PETRA_SHD_SCRUB_LVL3,
  SOC_PETRA_SHD_SCRUB_LVL_NONE, /* The appropriate table is skipped during scrubbing */
  SOC_PETRA_SHD_SCRUB_LVL_ALL,   /* Cannot be used to set a table scrubbing level. 
                               Is used to activate the scrubbing API on all tables, regardless their level  */
  SOC_PETRA_SHD_NOF_SCRUB_LVLS
} SOC_PETRA_SHD_SCRUB_LVL;

/* 
 * The maximal value that can be set to SOC_PETRA_SHD_SCRUB_LVL
 */
#define SOC_PETRA_SHD_SCRUB_LVL_MAX SOC_PETRA_SHD_SCRUB_LVL_NONE

/* 
 * A separate marker is used for scrubbing all counters.
 * This allows scrubbing with one task for "all", and another
 * for a specific scrubbing level.
 */
#define SOC_PETRA_SHD_MARK_LVL_ALL SOC_PETRA_SHD_SCRUB_LVL_NONE
#define SOC_PETRA_SHD_NOF_MARK_LVLS (SOC_PETRA_SHD_MARK_LVL_ALL+1)

/* 
 * The last point to which we got in the scrubbing 
 * process
 */
typedef struct
{
  SOC_PETRA_SHD_TBL_IDS tbl_id;
  /* 
   * The next offset inside the table
   * when continuing the scrubbing.
   */
  uint32 offset;
}SOC_PETRA_SHD_SCRUB_MARK;

/* 
 * Scrubbing configuration
 */
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /* 
   * The level of tables to scrub. 
   * Tables of other scrubbing level are skipped.
   * This allows scrubbing different tables with different rate,
   * by using different task calling the scrubbing API with the appropriate level.
   * A task scrubbing high level tables may be of a higher priority.
   * Note: setting the scrubbing level to 'SOC_PETRA_SHD_SCRUB_LVL_ALL'
   * results in scrubbing all tables regardless their scrubbing level. 
   */
  SOC_PETRA_SHD_SCRUB_LVL level;
  /* 
   * Number of table entries to scrub. Scrubbing is sequential,
   * from the last mark. After scrubbing a certain table, the 
   * scrubber moves to the next, up to a total of nof_entries entries 
   */
  uint32 nof_entries;
  /* 
   * If TRUE, when a Bit Flip is observed, it is reported, but not corrected.
   * Otherwise, it is corrected by writing the shadowed value to the device. 
   */
  uint8 is_no_repair;
  /* 
   * If TRUE, the scrubber API exits once a Bit Flip is observed (and repaired if is_no_repair is FALSE) 
   * If FALSE, the scrubbing will continue up to 'nof_entries', but only the first error will be logged
   * (table/entry and bit flip index) 
   */
  uint8 is_exit_on_repair; 
} SOC_PETRA_SHD_SCRUB_INFO;

/* 
 * A report log returned by the scrubber API.
 * Note: it is assumed that a single entry does not have more 
 * then a single Bit Flip event.
 * If this is not the case, all bits will be repaired,
 * but only the first will be reported.
 */
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /* 
   * Total number of errors (Bit Flip events) encountered.
   * If 'is_exit_on_repair' is set, this field can be at most '1'
   */
  uint32            nof_errors;
  /* 
   * The mark (Table and entry inside the table)
   * of the first Flipped Bit.
   */
  SOC_PETRA_SHD_SCRUB_MARK first_error;  
  /* 
   * The offset in bits, inside the table entry, of the Flipped Bit.
   * LSB is offset '0'
   */
  uint32             flip_bit_offset;
  /* 
   * If TRUE, the Flip Bit event was from '0' value to '1' value. 
   */
  uint8            is_flipped_zero_to_one;
  /*
   * If TRUE, the flipped bit couldn't be repaired.
   */
  uint8            is_bit_stuck;
} SOC_PETRA_SHD_SCRUB_REPORT;

/* 
 * The first table in SOC_PETRA_SHD_TBL_IDS enumerator
 */
#define SOC_PETRA_SHD_TBL_FIRST 0
 /* 
 * The last table in SOC_PETRA_SHD_TBL_IDS enumerator
 */
/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

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

void 
  soc_petra_shd_tbl_scrub_lvl_set(
    SOC_SAND_IN  SOC_PETRA_SHD_TBL_IDS tbl_id,
    SOC_SAND_IN  SOC_PETRA_SHD_SCRUB_LVL lvl_id
  );

SOC_PETRA_SHD_SCRUB_LVL 
  soc_petra_shd_tbl_scrub_lvl_get(
    SOC_SAND_IN  SOC_PETRA_SHD_TBL_IDS tbl_id
  );

uint32
  soc_petra_shd_indirect_init(void);

uint32
  soc_petra_shd_indirect_malloc(
    SOC_SAND_IN int unit
  );

uint32
  soc_petra_shd_indirect_free(
    SOC_SAND_IN int unit
  );

/* 
 * Write 'nof_lines' entries from the shadow DB to the device,
 * starting from the entry of table 'tbl_id',
 * at offset 'offset' relative to the start of the table
 */
uint32
  soc_petra_shd_indirect_range_write(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN SOC_PETRA_SHD_TBL_IDS tbl_id,
    SOC_SAND_IN uint32         offset,
    SOC_SAND_IN uint32         nof_lines
  );

/*
 * Compare 'nof_lines' entries from the shadow DB to the values read from the device,
 * starting from the entry of table 'tbl_id',
 * at offset 'offset' relative to the start of the table
 */
uint32
  soc_petra_shd_indirect_range_compare(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PETRA_SHD_TBL_IDS tbl_id,
    SOC_SAND_IN  uint32         offset,
    SOC_SAND_IN  uint32         nof_lines,
    SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_REPORT  *compare_report
  );

void 
  soc_petra_shd_indirect_set_nof_repetitions_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           nof_repetitions  
  );

uint32 
  soc_petra_shd_indirect_get_nof_repetitions(
    SOC_SAND_IN  int          unit
  );

/* 
 *  Read the specified number of entries from the device.
 *  Start reading at current scrubbing mark-point. If end-of-table is reached,
 *  continue reading the next table, cyclically. 
 *  Compare each entry to the shadow. If the device value is different from the shadow,
 *  write the shadowed value to the device, unless 'is_no_repair' flag is set.
 *  If repair actions took place, the number of repair actions is indicated in the nof_repairs'
 *  parameter. In this case, 'first_repair_mark' is the mark (module+offset) of the first repair
 *  action that took place. 
 */
uint32
  soc_petra_shd_scrub_range(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_SHD_SCRUB_INFO   *info,
    SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_REPORT *report
  );

/*
 * Retrieve current configuration on the device.
 * Synchronize the SW shadow with the current device configuration
 */
uint32
  soc_petra_shd_shadow_retrieve(
    SOC_SAND_IN int unit
  );

void 
	SOC_PETRA_SHD_SCRUB_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_INFO *info
  );

void 
	SOC_PETRA_SHD_SCRUB_REPORT_clear(
    SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_REPORT *info
  );

#if SOC_PETRA_DEBUG
const char*
  SOC_PETRA_SHD_SCRUB_LVL_to_string(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_LVL enum_val
  );

void
  SOC_PETRA_SHD_SCRUB_INFO_print(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_INFO *info
  );

void
  SOC_PETRA_SHD_SCRUB_MARK_print(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_MARK *info
  );

void
  SOC_PETRA_SHD_SCRUB_REPORT_print(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_REPORT *info
  );

#endif

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_SHADOW_INCLUDED__*/
#endif
