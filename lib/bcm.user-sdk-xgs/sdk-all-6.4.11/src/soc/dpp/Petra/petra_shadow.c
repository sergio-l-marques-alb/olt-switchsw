/* $Id: petra_shadow.c,v 1.12 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_shadow.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_SHD_NOF_UINT32S_IN_LINE_MAX 16
#define SOC_PETRA_SHD_SCRUB_LVL_LOW         SOC_PETRA_SHD_SCRUB_LVL2
#define SOC_PETRA_SHD_SCRUB_LVL_HIGH        SOC_PETRA_SHD_SCRUB_LVL1
#define SOC_PETRA_SHD_TBL_INFO_LENGTH       (sizeof(Soc_petra_shd_table_info) / sizeof(SOC_PETRA_SHD_TABLE_INFO))

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
/* TRUE if the scrub level of the table matches the requested level: scrb_lvl_filter or ALL   */
#define SOC_PETRA_SHD_IS_SCRUB_LVL_MATCH(scrb_lvl_tbl, scrb_lvl_filter)  \
  (((scrb_lvl_tbl) == (scrb_lvl_filter)) || ((scrb_lvl_filter) == SOC_PETRA_SHD_SCRUB_LVL_ALL))
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct
{
  uint32 module_id;
  uint32 start_address;
  uint32 end_address;

  int nof_longs_to_move;
  uint32 nof_bits;

  SOC_PETRA_SHD_SCRUB_LVL scrub_lvl;

  /*
   * The actual address
   */
  uint32* base[SOC_SAND_MAX_DEVICE] ;
} SOC_PETRA_SHD_INDIRECT_BLOCK;

typedef struct
{
  SOC_PETRA_SHD_TBL_IDS   tbl_id;
  uint32            module_id;
  uint32            address;
  uint32           nof_entries;
  uint32           nof_bits_per_entry;
  SOC_PETRA_SHD_SCRUB_LVL scrub_lvl;
} SOC_PETRA_SHD_TABLE_INFO;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

static uint8
  Soc_petra_shd_initialized = FALSE;

static SOC_SAND_TBL_ACCESS
  Soc_petra_shd_real_tbl_hook;

static SOC_PETRA_SHD_INDIRECT_BLOCK
  Soc_petra_shd_indirect_blocks[SOC_PETRA_SHD_TBL_ID_LAST + 1];

static uint32
  Soc_petra_shd_nof_repetitions[SOC_SAND_MAX_DEVICE];

static SOC_PETRA_SHD_SCRUB_MARK
  Soc_petra_shd_scrub_mark[SOC_SAND_MAX_DEVICE][SOC_PETRA_SHD_NOF_MARK_LVLS];

static SOC_PETRA_SHD_TABLE_INFO Soc_petra_shd_table_info[] = {
  /*
   *  IRE
   */
  {SOC_PETRA_SHD_IRE_NIF_CTXT_MAP_TBL_ID,                       SOC_PETRA_IRE_ID,  0x00010000, 0x0200, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID,              SOC_PETRA_IRE_ID,  0x00020000, 0x0008, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRE_RCY_CTXT_MAP_TBL_ID,                       SOC_PETRA_IRE_ID,  0x00030000, 0x0040, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                                                                            
  /*                                                                                                                        
   *  IRR                                                                                                                   
   */                                                                                                                       
  {SOC_PETRA_SHD_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID,          SOC_PETRA_IRR_ID,  0x00010000, 0x1000, 4,   SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_IRR_REPLICATION_MULTICAST_DB_TBL_ID,           SOC_PETRA_IRR_ID,  0x00020000, 0x8000, 84,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_MIRROR_TABLE_TBL_ID,                       SOC_PETRA_IRR_ID,  0x00030000, 0x0060, 24,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_SNOOP_TABLE_TBL_ID,                        SOC_PETRA_IRR_ID,  0x00060000, 0x0010, 24,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_GLAG_TO_LAG_RANGE_TBL_ID,                  SOC_PETRA_IRR_ID,  0x000f0000, 0x0100, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_GLAG_MAPPING_TBL_ID,                       SOC_PETRA_IRR_ID,  0x00200000, 0x1000, 17,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_SMOOTH_DIVISION_TBL_ID,                    SOC_PETRA_IRR_ID,  0x00100000, 0x1000, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_DESTINATION_TABLE_TBL_ID,                  SOC_PETRA_IRR_ID,  0x00300000, 0x1000, 16,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_IRR_GLAG_NEXT_MEMBER_TBL_ID,                   SOC_PETRA_IRR_ID,  0x00400000, 0x0100, 5,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IRR_RLAG_NEXT_MEMBER_TBL_ID,                   SOC_PETRA_IRR_ID,  0x00500000, 0x0400, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                                                                            
  /*                                                                                                                        
   *  IHP                                                                                                                   
   */                                                                                                                       
  {SOC_PETRA_SHD_IHP_PORT_INFO_TBL_ID,                          SOC_PETRA_IHP_ID,  0x000e0000, 0x0050, 39,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID,             SOC_PETRA_IHP_ID,  0x00120000, 0x0050, 13,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID,          SOC_PETRA_IHP_ID,  0x00140000, 0x0400, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_PTC_COMMANDS1_TBL_ID,                      SOC_PETRA_IHP_ID,  0x00500000, 0x0200, 40,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_PTC_COMMANDS2_TBL_ID,                      SOC_PETRA_IHP_ID,  0x00510000, 0x0200, 40,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID,                SOC_PETRA_IHP_ID,  0x00520000, 0x0200, 72,  SOC_PETRA_SHD_SCRUB_LVL_LOW }, /* Data sheet error */
  {SOC_PETRA_SHD_IHP_KEY_PROGRAM0_TBL_ID,                       SOC_PETRA_IHP_ID,  0x00530000, 0x0040, 13,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_KEY_PROGRAM1_TBL_ID,                       SOC_PETRA_IHP_ID,  0x00540000, 0x0040, 26,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_KEY_PROGRAM2_TBL_ID,                       SOC_PETRA_IHP_ID,  0x00550000, 0x0040, 29,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_KEY_PROGRAM3_TBL_ID,                       SOC_PETRA_IHP_ID,  0x00560000, 0x0040, 26,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_KEY_PROGRAM4_TBL_ID,                       SOC_PETRA_IHP_ID,  0x00570000, 0x0040, 45,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_PROGRAMMABLE_COS_TBL_ID,                   SOC_PETRA_IHP_ID,  0x00580000, 0x0100, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW }, /* Data sheet error */
  /* {SOC_PETRA_SHD_IHP_PROGRAMMABLE_COS1_TBL_ID,                  SOC_PETRA_IHP_ID,  0x00620000, 0x0007, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW }, */
                                                                                                                                        
  /*                                                                                                                                    
   *  IQM                                                                                                                               
   */                                                                                                                                   
  {SOC_PETRA_SHD_IQM_STATIC_TBL_ID,                             SOC_PETRA_IQM_ID,  0x00200000, 0x8000, 20,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID,      SOC_PETRA_IQM_ID,  0x00400000, 0x0040, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID,              SOC_PETRA_IQM_ID,  0x00500000, 0x0010, 8,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID,  SOC_PETRA_IQM_ID,  0x00800000, 0x0100, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID,  SOC_PETRA_IQM_ID,  0x01100000, 0x0004, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID,  SOC_PETRA_IQM_ID,  0x01200000, 0x0020, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID,  SOC_PETRA_IQM_ID,  0x01300000, 0x0040, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID,  SOC_PETRA_IQM_ID,  0x01400000, 0x0100, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_A_TBL_ID,              SOC_PETRA_IQM_ID,  0x01d00000, 0x0010, 46,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_B_TBL_ID,              SOC_PETRA_IQM_ID,  0x01e00000, 0x0010, 46,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_C_TBL_ID,              SOC_PETRA_IQM_ID,  0x01f00000, 0x0010, 46,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_FC_PARAMS_GROUP_D_TBL_ID,              SOC_PETRA_IQM_ID,  0x02000000, 0x0010, 46,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_A_TBL_ID,           SOC_PETRA_IQM_ID,  0x02100000, 0x0040, 89,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_B_TBL_ID,           SOC_PETRA_IQM_ID,  0x02200000, 0x0040, 89,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_C_TBL_ID,           SOC_PETRA_IQM_ID,  0x02300000, 0x0040, 89,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_VSQ_QUEUE_PARAMS_GROUP_D_TBL_ID,           SOC_PETRA_IQM_ID,  0x02400000, 0x0040, 89,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID,        SOC_PETRA_IQM_ID,  0x02500000, 0x0100, 21,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID, SOC_PETRA_IQM_ID,  0x02600000, 0x0010, 16,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IQM_SYSTEM_RED_TBL_ID,                         SOC_PETRA_IQM_ID,  0x02700000, 0x0040, 180, SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                                                                                        
  /*                                                                                                                                    
   *  IPS                                                                                                                               
   */                                                                                                                                   
  {SOC_PETRA_SHD_IPS_SYSTEM_PHYSICAL_PORT_LUT_TBL_ID,           SOC_PETRA_IPS_ID,  0x00000000, 0x2000, 12,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_IPS_DEST_DEVICE_AND_PORT_LUT_TBL_ID,           SOC_PETRA_IPS_ID,  0x00008000, 0x1000, 19,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID,               SOC_PETRA_IPS_ID,  0x00010000, 0x2000, 15,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID,            SOC_PETRA_IPS_ID,  0x00018000, 0x8000, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID,          SOC_PETRA_IPS_ID,  0x00020000, 0x0200, 2,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID,          SOC_PETRA_IPS_ID,  0x00028000, 0x0004, 64,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TBL_ID,        SOC_PETRA_IPS_ID,  0x00030000, 0x0010, 36,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TBL_ID,    SOC_PETRA_IPS_ID,  0x00038000, 0x0010, 32,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TBL_ID,         SOC_PETRA_IPS_ID,  0x00040000, 0x0010, 33,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TBL_ID,         SOC_PETRA_IPS_ID,  0x00048000, 0x0010, 8,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                                                                            
  /*                                                                                                                        
   *  EGQ                                                                                                                   
   */                                                                                                                       
  {SOC_PETRA_SHD_EGQ_NIFA_CH0_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00010000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFA_CH1_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00020000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFA_CH2_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00030000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFA_CH3_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00040000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFB_CH0_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00050000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFB_CH1_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00060000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFB_CH2_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00070000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFB_CH3_SCM_TBL_ID,                       SOC_PETRA_EGQ_ID,  0x00080000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_NIFAB_NCH_SCM_TBL_ID,                      SOC_PETRA_EGQ_ID,  0x00090000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_RCY_SCM_TBL_ID,                            SOC_PETRA_EGQ_ID,  0x000a0000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_CPU_SCM_TBL_ID,                            SOC_PETRA_EGQ_ID,  0x000b0000, 0x0100, 25,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_CCM_TBL_ID,                                SOC_PETRA_EGQ_ID,  0x000c0000, 0x0100, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_PMC_TBL_ID,                                SOC_PETRA_EGQ_ID,  0x000d0000, 0x00a0, 16,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_DWM_TBL_ID,                                SOC_PETRA_EGQ_ID,  0x00110000, 0x0050, 16,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_PCT_TBL_ID,                                SOC_PETRA_EGQ_ID,  0x00140000, 0x0050, 72,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_EGQ_VLAN_TABLE_TBL_ID,                         SOC_PETRA_EGQ_ID,  0x00150000, 0x1000, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                                                                            
  /*                                                                                                                        
   *  CFC                                                                                                                   
   */                                                                                                                       
  {SOC_PETRA_SHD_CFC_RCY_TO_OFP_MAPPING_TBL_ID,                 SOC_PETRA_CFC_ID,  0x00000000, 0x0020, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID,   SOC_PETRA_CFC_ID,  0x00100000, 0x0010, 14,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID,   SOC_PETRA_CFC_ID,  0x00200000, 0x0010, 14,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_OOB_A_SCH_BASED_FC_TO_OFP_MAPPING_TBL_ID,  SOC_PETRA_CFC_ID,  0x00400000, 0x0080, 9,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_OOB_B_SCH_BASED_FC_TO_OFP_MAPPING_TBL_ID,  SOC_PETRA_CFC_ID,  0x00500000, 0x0080, 9,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID,  SOC_PETRA_CFC_ID,  0x00600000, 0x0200, 9,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID,  SOC_PETRA_CFC_ID,  0x00700000, 0x0200, 9,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID,    SOC_PETRA_CFC_ID,  0x00800000, 0x0200, 11,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                                                                            
  /*                                                                                                                        
   *  SCH                                                                                                                   
   */                                                                                                                       
  {SOC_PETRA_SHD_SCH_CAL_NIF0_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40000000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF0_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40001000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF1_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40002000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF1_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40003000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF2_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40004000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF2_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40005000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF3_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40006000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF3_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40007000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF4_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40008000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF4_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x40009000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF5_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x4000a000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF5_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x4000b000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF6_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x4000c000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF6_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x4000d000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF7_A_TBL_ID,                         SOC_PETRA_SCH_ID,  0x4000e000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_NIF7_B_TBL_ID,                         SOC_PETRA_SCH_ID,  0x4000f000, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_CPU_A_TBL_ID,                          SOC_PETRA_SCH_ID,  0x40000800, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_CPU_B_TBL_ID,                          SOC_PETRA_SCH_ID,  0x40001800, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_RCY_A_TBL_ID,                          SOC_PETRA_SCH_ID,  0x40002800, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_CAL_RCY_B_TBL_ID,                          SOC_PETRA_SCH_ID,  0x40003800, 0x0400, 7,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_DRM_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40010000, 0x0128, 18,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_DSM_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40020000, 0x0200, 16,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_FDMS_TBL_ID,                               SOC_PETRA_SCH_ID,  0x40030000, 0xe000, 23,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_SCH_SHDS_TBL_ID,                               SOC_PETRA_SCH_ID,  0x40040000, 0x7000, 40,  SOC_PETRA_SHD_SCRUB_LVL_LOW }, /* Data sheet error; omitting 2 write-only bits */
  {SOC_PETRA_SHD_SCH_SEM_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40050000, 0x0800, 8,   SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_SCH_FSF_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40060000, 0x0700, 16,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_SCH_FGM_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40070000, 0x0800, 16,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_SCH_SHC_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40080000, 0x0100, 4,   SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_SCH_SCC_TBL_ID,                                SOC_PETRA_SCH_ID,  0x40090000, 0x2000, 8,   SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_SCH_SCT_TBL_ID,                                SOC_PETRA_SCH_ID,  0x400a0000, 0x0100, 48,  SOC_PETRA_SHD_SCRUB_LVL_HIGH},
  {SOC_PETRA_SHD_SCH_FQM_TBL_ID,                                SOC_PETRA_SCH_ID,  0x400b0000, 0x3800, 18,  SOC_PETRA_SHD_SCRUB_LVL_HIGH}, /* Data sheet error */
  {SOC_PETRA_SHD_SCH_FFM_TBL_ID,                                SOC_PETRA_SCH_ID,  0x400c0000, 0x0800, 11,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                           
#ifdef PETRA_PP                                                               
  /*                                                                       
   *  OLP                                                                  
   */                                                                      
  {SOC_PETRA_SHD_OLP_PGE_MEM_TBL_ID,                            SOC_PETRA_OLP_ID,  0x00010000, 0x0020, 32,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                           
  /*                                                                       
   *  IHP                                                                  
   */                                                                      
  {SOC_PETRA_SHD_IHP_ETH_PORT_INFO1_TBL_ID,                     SOC_PETRA_IHP_ID,  0x00100000, 0x0050, 38,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_ETH_PORT_INFO2_TBL_ID,                     SOC_PETRA_IHP_ID,  0x00110000, 0x0050, 50,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_CID_INFO_TBL_ID,                           SOC_PETRA_IHP_ID,  0x00160000, 0x1000, 14,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_CID_TO_TOPOLOGY_TBL_ID,                    SOC_PETRA_IHP_ID,  0x00170000, 0x1000, 8,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_STP_TBL_ID,                                SOC_PETRA_IHP_ID,  0x00180000, 0x2000, 8,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID,     SOC_PETRA_IHP_ID,  0x00190000, 0x0050, 40,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_CID_INDEX_TO_CID_TBL_ID,                   SOC_PETRA_IHP_ID,  0x001a0000, 0x0020, 16,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_BRIDGE_CONTROL_TO_FWD_PARAMS_TBL_ID,       SOC_PETRA_IHP_ID,  0x001e0000, 0x0100, 20,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID,        SOC_PETRA_IHP_ID,  0x00210000, 0x0010, 40,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_CID_PORT_MEMBERSHIP_TBL_ID,                SOC_PETRA_IHP_ID,  0x00220000, 0x1000, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_IS_CID_SHARED_TBL_ID,                      SOC_PETRA_IHP_ID,  0x00230000, 0x0080, 32,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_TOS_TO_TC_TBL_ID,                          SOC_PETRA_IHP_ID,  0x00240000, 0x0200, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_FEC_TBL_ID,                                SOC_PETRA_IHP_ID,  0x00290000, 0x2000, 45,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_SMOOTH_DIVISION_TBL_ID,                    SOC_PETRA_IHP_ID,  0x002a0000, 0x1000, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_LPM0_TBL_ID,                               SOC_PETRA_IHP_ID,  0x002b0000, 0x1000, 22,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_LPM1_TBL_ID,                               SOC_PETRA_IHP_ID,  0x002c0000, 0x6000, 22,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_LPM2_TBL_ID,                               SOC_PETRA_IHP_ID,  0x002d0000, 0x6000, 22,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_TCAM_ACTION_A_TBL_ID,                      SOC_PETRA_IHP_ID,  0x002e0000, 0x0200, 35,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_TCAM_ACTION_B_TBL_ID,                      SOC_PETRA_IHP_ID,  0x002f0000, 0x0200, 35,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_TCAM_ACTION_C_TBL_ID,                      SOC_PETRA_IHP_ID,  0x00300000, 0x0200, 35,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_TCAM_ACTION_D_TBL_ID,                      SOC_PETRA_IHP_ID,  0x00310000, 0x0200, 35,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_TOS_TO_COS_TBL_ID,                         SOC_PETRA_IHP_ID,  0x00320000, 0x0100, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_IPV6_TO_COS_TBL_ID,                        SOC_PETRA_IHP_ID,  0x00440000, 0x0100, 4,   SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_CID_SUBNET_TBL_ID,                         SOC_PETRA_IHP_ID,  0x00600000, 0x0010, 56,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID,    SOC_PETRA_IHP_ID,  0x00610000, 0x0010, 40,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
                                                                           
  /*                                                                       
   *  EGQ                                                                  
   */                                                                      
  {SOC_PETRA_SHD_EGQ_PPCT_TBL_ID,                               SOC_PETRA_EGQ_ID,  0x00160000, 0x0050, 82,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EGQ_STP_TBL_ID,                                SOC_PETRA_EGQ_ID,  0x00170000, 0x0050, 64,  SOC_PETRA_SHD_SCRUB_LVL_LOW },

  /*
   *  EPNI
   */
  {SOC_PETRA_SHD_EPNI_ARP_TBL_ID,                               SOC_PETRA_EPNI_ID, 0x00010000, 0x2000, 48,  SOC_PETRA_SHD_SCRUB_LVL_LOW },
  {SOC_PETRA_SHD_EPNI_PTT_TBL_ID,                               SOC_PETRA_EPNI_ID, 0x00020000, 0x1000, 80,  SOC_PETRA_SHD_SCRUB_LVL_LOW }, /* Data sheet error */
  {SOC_PETRA_SHD_EPNI_PROCESSOR_CFG_PARAMS_TBL_ID,              SOC_PETRA_EPNI_ID, 0x00030000, 0x0001, 192, SOC_PETRA_SHD_SCRUB_LVL_LOW },
#endif
};

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

void 
  soc_petra_shd_tbl_scrub_lvl_set(
    SOC_SAND_IN  SOC_PETRA_SHD_TBL_IDS tbl_id,
    SOC_SAND_IN  SOC_PETRA_SHD_SCRUB_LVL lvl_id
  )
{
  if ((tbl_id >= SOC_PETRA_SHD_TBL_ID_LAST) || (lvl_id > SOC_PETRA_SHD_SCRUB_LVL_MAX))
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT; /* Out of range */
  }

  Soc_petra_shd_indirect_blocks[tbl_id].scrub_lvl = lvl_id;

exit: 
  return;
}

SOC_PETRA_SHD_SCRUB_LVL 
  soc_petra_shd_tbl_scrub_lvl_get(
    SOC_SAND_IN  SOC_PETRA_SHD_TBL_IDS tbl_id
  )
{
  SOC_PETRA_SHD_SCRUB_LVL
    level = SOC_PETRA_SHD_SCRUB_LVL_NONE;

  if (tbl_id >= SOC_PETRA_SHD_TBL_ID_LAST)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT; /* Out of range */
  }

  level = Soc_petra_shd_indirect_blocks[tbl_id].scrub_lvl;;

exit: 
  return level;
}

void SOC_PETRA_SHD_SCRUB_MARK_clear(SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_MARK *mark)
{
  if (mark != NULL)
  {
    mark->tbl_id        = SOC_PETRA_SHD_TBL_FIRST;
    mark->offset = 0;
  }
}

void
  soc_petra_shd_mark_set(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_LVL lvl,
    SOC_SAND_IN SOC_PETRA_SHD_TBL_IDS tbl_id,
    SOC_SAND_IN uint32          offset
  )
{
  if(lvl < SOC_PETRA_SHD_NOF_MARK_LVLS)
  {
    Soc_petra_shd_scrub_mark[unit][lvl].tbl_id = tbl_id;
    Soc_petra_shd_scrub_mark[unit][lvl].offset = offset;
  }
}

void
  soc_petra_shd_mark_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_LVL lvl,
    SOC_SAND_OUT SOC_PETRA_SHD_TBL_IDS *tbl_id,
    SOC_SAND_OUT uint32          *offset
  )
{
  if((lvl < SOC_PETRA_SHD_NOF_MARK_LVLS) && (tbl_id != NULL) && (offset != NULL))
  {
    *tbl_id = Soc_petra_shd_scrub_mark[unit][lvl].tbl_id;
    *offset = Soc_petra_shd_scrub_mark[unit][lvl].offset;
  }
}

STATIC uint32
  soc_petra_shd_indirect_op_write(
    SOC_SAND_IN  int unit,
	  SOC_SAND_IN  uint32 module_id,
	  SOC_SAND_IN  uint32  offset,
    SOC_SAND_IN  uint32  *data_ptr
  );

void 
  soc_petra_shd_indirect_set_nof_repetitions_unsafe(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           nof_repetitions  
  )
{
  Soc_petra_shd_nof_repetitions[unit] = nof_repetitions;
}

uint32 
  soc_petra_shd_indirect_get_nof_repetitions(
    SOC_SAND_IN  int          unit
  )
{
  return Soc_petra_shd_nof_repetitions[unit];
}

STATIC void
  soc_petra_shd_indirect_blocks_init(
    SOC_SAND_IN  SOC_PETRA_SHD_TABLE_INFO     tbl_info[],
    SOC_SAND_IN  uint32                tbl_info_length,
    SOC_SAND_OUT SOC_PETRA_SHD_INDIRECT_BLOCK indirect_blocks[]
  )
{
  uint32
    idx,
    unit;
  const SOC_PETRA_SHD_TABLE_INFO
    *tbl_info_entry;

  for (idx = 0; idx < tbl_info_length; ++idx)
  {
    tbl_info_entry = &tbl_info[idx];
    indirect_blocks[tbl_info_entry->tbl_id].module_id         = tbl_info_entry->module_id;
    indirect_blocks[tbl_info_entry->tbl_id].start_address     = tbl_info_entry->address;
    indirect_blocks[tbl_info_entry->tbl_id].end_address       = tbl_info_entry->address + tbl_info_entry->nof_entries - 1;
    indirect_blocks[tbl_info_entry->tbl_id].nof_longs_to_move = SOC_SAND_DIV_ROUND_UP(tbl_info_entry->nof_bits_per_entry, SOC_SAND_NOF_BITS_IN_UINT32);
    indirect_blocks[tbl_info_entry->tbl_id].nof_bits          = tbl_info_entry->nof_bits_per_entry;
    indirect_blocks[tbl_info_entry->tbl_id].scrub_lvl          = tbl_info_entry->scrub_lvl;
    for (unit = 0; unit < SOC_SAND_MAX_DEVICE; ++unit)
    {
      indirect_blocks[tbl_info_entry->tbl_id].base[unit] = NULL;
    }
  }
  indirect_blocks[SOC_PETRA_SHD_TBL_ID_LAST].module_id = SOC_PETRA_NOF_MODULES;
}

STATIC SOC_SAND_RET
  soc_petra_shd_tbl_write_callback(
    SOC_SAND_IN     int   unit,
    SOC_SAND_IN     uint32    *data_ptr,
    SOC_SAND_IN     uint32    offset,
    SOC_SAND_IN     uint32    size,
    SOC_SAND_IN     uint32   module_id,
    SOC_SAND_IN     uint32    indirect_options
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    res = SOC_SAND_OK;

  if(data_ptr == NULL)
  {
    return SOC_SAND_ERR;
  }


  /*
   *  Use real callback to write to the hardware
   */
  ret = Soc_petra_shd_real_tbl_hook.write(
          unit,
          data_ptr,
          offset,
          size,
          module_id,
          indirect_options
        );
  if(ret != SOC_SAND_OK) {
    return ret;
  }

  /*
   *  Write shadow copy
   */
  res = soc_petra_shd_indirect_op_write(
          unit,
          module_id,
          offset,
          data_ptr
        );

  return soc_sand_get_error_code_from_error_word(res);
}

/*****************************************************
*NAME
* soc_petra_shd_indirect_init
*TYPE:
*  PROC
*DATE:
*  12/23/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_shd_indirect_init(void)
{
  SOC_SAND_RET
    res = SOC_SAND_OK;
  SOC_SAND_TBL_ACCESS
    hook;
  uint32
    dev_i,
    lvl_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_INIT);

  if (Soc_petra_shd_initialized)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /*
   *  Initialize the table information
   */
  soc_petra_shd_indirect_blocks_init(
    Soc_petra_shd_table_info,
    SOC_PETRA_SHD_TBL_INFO_LENGTH,
    Soc_petra_shd_indirect_blocks
  );

  for (dev_i = 0; dev_i < SOC_SAND_MAX_DEVICE; dev_i++)
  {
    soc_petra_shd_indirect_set_nof_repetitions_unsafe(dev_i, 0);
    for(lvl_i = 0; lvl_i < SOC_PETRA_SHD_NOF_MARK_LVLS; lvl_i++)
    {
      SOC_PETRA_SHD_SCRUB_MARK_clear(&(Soc_petra_shd_scrub_mark[dev_i][lvl_i]));
    }
  }

  /*
   *  Retrieve the table access hooks to the hardware and replace them with the shadow copies
   */
  res = soc_sand_tbl_hook_get(
          &Soc_petra_shd_real_tbl_hook
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  hook.read  = Soc_petra_shd_real_tbl_hook.read;
  hook.write = soc_petra_shd_tbl_write_callback;
  res = soc_sand_tbl_hook_set(
          &hook
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  Soc_petra_shd_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_init", 0, 0);
}

uint32
  soc_petra_shd_indirect_close(void)
{
  uint32
    dev_i;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_CLOSE);

  for (dev_i = 0; dev_i < SOC_SAND_MAX_DEVICE; dev_i++)
  {
    res = soc_petra_shd_indirect_free(
            dev_i
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    Soc_petra_shd_initialized = FALSE;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_close", 0, 0);
}

uint32
  soc_petra_shd_indirect_free(
    SOC_SAND_IN int unit
  )
{
  SOC_PETRA_SHD_INDIRECT_BLOCK*
    block_p;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_FREE);

  for (block_p=Soc_petra_shd_indirect_blocks; block_p->module_id!=SOC_PETRA_NOF_MODULES; block_p++)
  {
	  SOC_PETRA_FREE_ANY_SIZE(block_p->base[unit]);
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_free", 0, 0);
}

uint32
  soc_petra_shd_indirect_malloc(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nof_addresses =0;
  SOC_PETRA_SHD_INDIRECT_BLOCK*
    block_p;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_MALLOC);

  if (Soc_petra_shd_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SHD_NOT_INITIALIZED_ERR, 5, exit);        
  }

  for (block_p=Soc_petra_shd_indirect_blocks; block_p->module_id!=SOC_PETRA_NOF_MODULES; block_p++)
  {
    nof_addresses = block_p->end_address - block_p->start_address + 1;
	  SOC_PETRA_ALLOC_ANY_SIZE(block_p->base[unit], uint32, nof_addresses*sizeof(uint32)*block_p->nof_longs_to_move);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_malloc", 0, 0);
}


STATIC uint32
  soc_petra_shd_indirect_address_to_offset(
    SOC_SAND_IN  uint32                   access_address,
    SOC_SAND_IN  SOC_PETRA_SHD_INDIRECT_BLOCK   *block_p,
    SOC_SAND_OUT uint32                   *access_offset
    )
{  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(block_p);
  SOC_SAND_CHECK_NULL_INPUT(access_offset);

  *access_offset = SOC_PETRA_SHD_ADDR_INVALID;

  if(block_p == NULL)
  {
    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[dead_error_begin] */
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }
  
  /*1*/

  /*2*/
  if (SOC_SAND_IS_VAL_IN_RANGE(access_address, block_p->start_address, block_p->end_address))
  {
    /*3*/
    *access_offset = access_address - block_p->start_address ;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_address_to_offset", 0, 0);
}

STATIC uint32
  soc_petra_shd_last_word_mask_get(
    SOC_SAND_IN uint32 nof_bits
  )
{
  return nof_bits % SOC_SAND_NOF_BITS_IN_UINT32 == 0 ? SOC_SAND_BITS_MASK(SOC_SAND_NOF_BITS_IN_UINT32 - 1, 0)
                                                   : SOC_SAND_BITS_MASK(nof_bits % SOC_SAND_NOF_BITS_IN_UINT32 - 1, 0);
}

STATIC uint32
  soc_petra_shd_indirect_write(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN uint32 access_address,
    SOC_SAND_IN uint32 write_value[SOC_PETRA_SHD_NOF_UINT32S_IN_LINE_MAX],
    SOC_SAND_IN SOC_PETRA_SHD_INDIRECT_BLOCK* block_p
  )
{
  uint32
    res = SOC_SAND_OK ;
  uint32
    long_offset,
    word_size,
    i,
    mask;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_WRITE);
  SOC_SAND_CHECK_NULL_INPUT(block_p);

  res = soc_petra_shd_indirect_address_to_offset(access_address, block_p, &long_offset);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (long_offset == SOC_PETRA_SHD_ADDR_INVALID)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_INDIRECT_OFFSET_SIZE_MISMATCH_ERR, 10, exit);
  }
  
  word_size = block_p->nof_longs_to_move;
  for (i=0; i<word_size - 1; i++)
  {
    *(block_p->base[unit]+(long_offset * word_size)+i) = write_value[i] ;    
  }
  mask = soc_petra_shd_last_word_mask_get(block_p->nof_bits);
  *(block_p->base[unit]+((long_offset + 1) * word_size) - 1) = write_value[word_size - 1] & mask;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_write", 0, 0);
}


STATIC uint32
  soc_petra_shd_indirect_op_write(
    SOC_SAND_IN  int unit,
	  SOC_SAND_IN  uint32 module_id,
	  SOC_SAND_IN  uint32  offset,
    SOC_SAND_IN  uint32  *data_ptr
  )
{  
  SOC_PETRA_SHD_INDIRECT_BLOCK*
    block_p;
  uint32
	  indirect_access_address = offset,
	  dummy_val,
	  nof_indirect_accesses = 0,
    res = SOC_SAND_OK;
  uint32
      command_count = 0,
      i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_OP_WRITE);
  SOC_SAND_CHECK_NULL_INPUT(data_ptr);

  for (block_p=Soc_petra_shd_indirect_blocks; ((block_p->module_id!=SOC_PETRA_NOF_MODULES)&&(nof_indirect_accesses == 0)); block_p++)
  {
	  if(block_p->module_id == module_id)
	  {
		  /* This is the module; check offset */
		  soc_petra_shd_indirect_address_to_offset(indirect_access_address, block_p, &dummy_val);
		  if(dummy_val == SOC_PETRA_SHD_ADDR_INVALID)
		  {
		    continue;
		  }
		  else
		  {
		    nof_indirect_accesses++;
		  }
	  }
    if(nof_indirect_accesses > 0)
    {
      /* Found the table */
      /*
       *  Support multiple writes .
       */
       command_count = soc_petra_shd_indirect_get_nof_repetitions(unit);
       
       if (command_count == 0) {
          /* Write value once in this case */
          command_count++;
       }
       if (command_count > 1)
       {
         command_count--;
       }
       for( i = 0; i < command_count; i++ )
       {
         if (block_p->start_address + i > block_p->end_address )
         {
           break;
         }
         res = soc_petra_shd_indirect_write(
                unit, 
                indirect_access_address, 
                data_ptr, 
                block_p
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
         
         indirect_access_address++;
       }
    }
  }
 
  soc_petra_shd_indirect_set_nof_repetitions_unsafe(unit, 0);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_op_write", 0, 0);
}

STATIC uint32
  soc_petra_shd_entry_read(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PETRA_SHD_SCRUB_MARK *entry,
    SOC_SAND_OUT uint32             *data
  )
{
  SOC_SAND_RET
    ret;
  SOC_PETRA_SHD_INDIRECT_BLOCK
    *block_p = &Soc_petra_shd_indirect_blocks[entry->tbl_id];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_ENTRY_READ);
  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(data);

  ret = Soc_petra_shd_real_tbl_hook.read(
          unit,
          data,
          block_p->start_address + entry->offset,
          block_p->nof_longs_to_move * sizeof(uint32),
          block_p->module_id,
          block_p->nof_longs_to_move * sizeof(uint32)
        );
  if (ret != SOC_SAND_OK)
  {
    SOC_SAND_SET_ERROR_CODE(7, 10, exit);
  }

  data[block_p->nof_longs_to_move - 1] &= soc_petra_shd_last_word_mask_get(block_p->nof_bits);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_entry_read", 0, 0);
}

STATIC uint32
  soc_petra_shd_entry_compare(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  SOC_PETRA_SHD_SCRUB_MARK *entry,
    SOC_SAND_OUT uint8            *is_correct,
    SOC_SAND_OUT uint32             *bit_flip_offset,
    SOC_SAND_OUT uint8             *is_flipped_0to1
  )
{
  uint32
    res = SOC_SAND_OK,
    hw_val[SOC_PETRA_SHD_NOF_UINT32S_IN_LINE_MAX],
    xor_val[SOC_PETRA_SHD_NOF_UINT32S_IN_LINE_MAX];
  uint32
    *shadow_ptr;
  SOC_PETRA_SHD_INDIRECT_BLOCK
    *block_p = &Soc_petra_shd_indirect_blocks[entry->tbl_id];
  uint8
    is_no_bitflip,
    is_curr_bit_flipped;
  uint32
    bit_i,
    flipped_bit_i = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_ENTRY_COMPARE);
  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(is_correct);
  SOC_SAND_CHECK_NULL_INPUT(bit_flip_offset);
  SOC_SAND_CHECK_NULL_INPUT(is_flipped_0to1);


  *is_flipped_0to1 = 0; /* Just clear */

  /*
   *  Read the value stored in the hardware
   */
  res = soc_petra_shd_entry_read(
          unit,
          entry,
          hw_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  shadow_ptr = block_p->base[unit] + entry->offset * block_p->nof_longs_to_move;

  /*
   *  Compare with the shadow copy
   */
  SOC_PETRA_COMP(hw_val, shadow_ptr, uint32, block_p->nof_longs_to_move, is_no_bitflip);

  if (!is_no_bitflip)
  {
    SOC_PETRA_COPY(xor_val, hw_val, uint32, SOC_PETRA_SHD_NOF_UINT32S_IN_LINE_MAX);
    res = soc_sand_bitstream_xor(
            xor_val,
            shadow_ptr,
            block_p->nof_longs_to_move
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for(bit_i = 0; bit_i < (block_p->nof_longs_to_move * sizeof(uint32)); bit_i++)
    {
      is_curr_bit_flipped = SOC_SAND_NUM2BOOL(soc_sand_bitstream_test_bit(xor_val, bit_i));
      if (is_curr_bit_flipped)
      {
        flipped_bit_i = bit_i;
        *is_flipped_0to1 = SOC_SAND_NUM2BOOL(soc_sand_bitstream_test_bit(hw_val, bit_i));
        break;
      }
    }
  }

  *is_correct = is_no_bitflip;
  *bit_flip_offset = flipped_bit_i;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_entry_compare", 0, 0);
}

STATIC uint32
  soc_petra_shd_entry_write(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_MARK *entry
  )
{
  SOC_SAND_RET
    ret;
  SOC_PETRA_SHD_INDIRECT_BLOCK
    *block_p = &Soc_petra_shd_indirect_blocks[entry->tbl_id];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_ENTRY_WRITE);
  SOC_SAND_CHECK_NULL_INPUT(entry);

  ret = Soc_petra_shd_real_tbl_hook.write(
          unit,
          block_p->base[unit] + entry->offset * block_p->nof_longs_to_move, /* The value in shadoe DB*/
          block_p->start_address + entry->offset, /* table-offset + offset-inside-the-table */
          block_p->nof_longs_to_move * sizeof(uint32),
          block_p->module_id,
          block_p->nof_longs_to_move * sizeof(uint32)
        );
  if (ret != SOC_SAND_OK)
  {
    SOC_SAND_SET_ERROR_CODE(8, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_entry_write", 0, 0);
}

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
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    line;
  SOC_PETRA_SHD_SCRUB_MARK
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  entry.tbl_id        = tbl_id;
  entry.offset = offset;

  for (line = 0; line < nof_lines; ++line)
  {
    res = soc_petra_shd_entry_write(
            unit,
            &entry
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ++entry.offset;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_range_write", 0, 0);
}

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
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    line;
  SOC_PETRA_SHD_SCRUB_MARK
    entry;
  uint8
    is_correct;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_INDIRECT_RANGE_COMPARE);
  SOC_SAND_CHECK_NULL_INPUT(compare_report);

  entry.tbl_id        = tbl_id;
  entry.offset = offset;

  for (line = 0; line < nof_lines; ++line)
  {
    res = soc_petra_shd_entry_compare(
            unit,
            &entry,
            &is_correct,
            &(compare_report->flip_bit_offset),
            &(compare_report->is_flipped_zero_to_one)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!is_correct)
    {
      compare_report->nof_errors++;
    }
    ++entry.offset;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_indirect_range_compare", 0, 0);
}

/*
 * Go to the next shadow entry:
 * next offset of the table (cyclic), or next table
 */
STATIC void
  soc_petra_shd_scrub_mark_advance(
    SOC_SAND_INOUT SOC_PETRA_SHD_SCRUB_MARK *scrub_mark
  )
{
  SOC_PETRA_SHD_INDIRECT_BLOCK
    *block_p;

  if(scrub_mark == NULL)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  block_p = &Soc_petra_shd_indirect_blocks[scrub_mark->tbl_id];

  ++scrub_mark->offset;
  if (scrub_mark->offset > block_p->end_address - block_p->start_address)
  {
    ++scrub_mark->tbl_id;
    scrub_mark->offset = 0;
  }
  if (scrub_mark->tbl_id == SOC_PETRA_SHD_TBL_ID_LAST)
  {
    scrub_mark->tbl_id        = SOC_PETRA_SHD_TBL_FIRST;
  }

exit:
  return;
}

STATIC uint8
  soc_petra_shd_scrub_mark_is_cleared(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_MARK *scrub_mark
  )
{
  uint8
    result = FALSE;

  if(scrub_mark == NULL)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

result =  SOC_SAND_NUM2BOOL((scrub_mark->tbl_id == SOC_PETRA_SHD_TBL_FIRST) && (scrub_mark->offset == 0));

exit:
  return result;
}
 
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
  )
{
  uint32
    res = SOC_SAND_OK,
    dummy_u32;
  uint32
    entry = 0;
  uint8
    is_correct,
    is_wrap_arround = FALSE,
    dummy_bool;
  SOC_PETRA_SHD_SCRUB_MARK
    start_mark;
  SOC_PETRA_SHD_SCRUB_LVL
    mark_lvl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SHD_SCRUB_RANGE);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit_no_sem);

  if (Soc_petra_shd_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SHD_NOT_INITIALIZED_ERR, 5, exit_no_sem);        
  }

  if((info == NULL) || (report == NULL))
  {
    goto exit_no_sem;
  }

  SOC_PETRA_SHD_SCRUB_REPORT_clear(report);
 
  /* 
   * Note! for efficiency, a semaphore is taken only on this level.
   * If lower-level functions, such as range_write, are called explicitly,
   * the caller must take and release the device semaphore
   */
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  if (info->level == SOC_PETRA_SHD_SCRUB_LVL_NONE)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  mark_lvl = (info->level == SOC_PETRA_SHD_SCRUB_LVL_ALL)?SOC_PETRA_SHD_MARK_LVL_ALL:info->level;

  SOC_PETRA_COPY(&start_mark, &(Soc_petra_shd_scrub_mark[unit][mark_lvl]), SOC_PETRA_SHD_SCRUB_MARK, 1);

  report->nof_errors = 0;

  while((entry < info->nof_entries) && !(is_wrap_arround))
  {
    if (SOC_PETRA_SHD_IS_SCRUB_LVL_MATCH(Soc_petra_shd_indirect_blocks[Soc_petra_shd_scrub_mark[unit][mark_lvl].tbl_id].scrub_lvl, info->level))
    {
      entry++;
      res = soc_petra_shd_entry_compare(
              unit,
              &Soc_petra_shd_scrub_mark[unit][mark_lvl],
              &is_correct,
              &(report->flip_bit_offset),
              &(report->is_flipped_zero_to_one)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


      if (!is_correct)
      {
        if (report->nof_errors == 0)
        {
          SOC_PETRA_COPY(&(report->first_error), &(Soc_petra_shd_scrub_mark[unit][mark_lvl]), SOC_PETRA_SHD_SCRUB_MARK, 1);
        }
        ++report->nof_errors;

        if (!info->is_no_repair)
        {
          res = soc_petra_shd_entry_write(
                  unit,
                  &Soc_petra_shd_scrub_mark[unit][mark_lvl]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          res = soc_petra_shd_entry_compare(
                  unit,
                  &Soc_petra_shd_scrub_mark[unit][mark_lvl],
                  &is_correct,
                  &dummy_u32,
                  &dummy_bool
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
          report->is_bit_stuck = is_correct ? FALSE : TRUE;
        }
      }
    } /* SOC_PETRA_SHD_IS_SCRUB_LVL_MATCH */

    soc_petra_shd_scrub_mark_advance(&Soc_petra_shd_scrub_mark[unit][mark_lvl]);
    
    if ((info->is_exit_on_repair) && (report->nof_errors > 0))
    {
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    /* 
     * Check for wrap-wround to prevent infinite loops 
     */
    SOC_PETRA_COMP(&start_mark, &Soc_petra_shd_scrub_mark[unit][mark_lvl], SOC_PETRA_SHD_SCRUB_MARK, 1, is_wrap_arround);
    if(entry == 0xFFFFFFFF)
    {
      is_wrap_arround = TRUE; /* Should never get here - protect just in case */
    }
  }

exit:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit_no_sem:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_scrub_range", 0, 0);
}

/*
 * Retrieve current configuration on the device.
 * Synchronize the SW shadow with the current device configuration
 */
uint32
  soc_petra_shd_shadow_retrieve(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK,
    *buffer;
  SOC_PETRA_SHD_INDIRECT_BLOCK
    *block_p;
  SOC_PETRA_SHD_SCRUB_LVL
    lvl_i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit_no_sem);

  if (Soc_petra_shd_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SHD_NOT_INITIALIZED_ERR, 5, exit_no_sem);        
  }

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  for(lvl_i = SOC_PETRA_SHD_SCRUB_LVL1; lvl_i < SOC_PETRA_SHD_NOF_MARK_LVLS; lvl_i++)
  {
    SOC_PETRA_SHD_SCRUB_MARK_clear(&Soc_petra_shd_scrub_mark[unit][lvl_i]);
  }

  do
  {
    block_p = &Soc_petra_shd_indirect_blocks[Soc_petra_shd_scrub_mark[unit][SOC_PETRA_SHD_MARK_LVL_ALL].tbl_id];

    buffer = block_p->base[unit]
               + Soc_petra_shd_scrub_mark[unit][SOC_PETRA_SHD_MARK_LVL_ALL].offset * block_p->nof_longs_to_move;

    res = soc_petra_shd_entry_read(
            unit,
            &Soc_petra_shd_scrub_mark[unit][SOC_PETRA_SHD_MARK_LVL_ALL],
            buffer
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    soc_petra_shd_scrub_mark_advance(&Soc_petra_shd_scrub_mark[unit][SOC_PETRA_SHD_MARK_LVL_ALL]);
  }
  while (!soc_petra_shd_scrub_mark_is_cleared(&Soc_petra_shd_scrub_mark[unit][SOC_PETRA_SHD_MARK_LVL_ALL]));

exit:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit_no_sem:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_shd_shadow_retrieve", 0, 0);
}

void 
	SOC_PETRA_SHD_SCRUB_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->is_exit_on_repair = FALSE;
  info->is_no_repair = FALSE;
  info->level = SOC_PETRA_SHD_SCRUB_LVL1;
  info->nof_entries = 1;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
	SOC_PETRA_SHD_SCRUB_REPORT_clear(
    SOC_SAND_OUT SOC_PETRA_SHD_SCRUB_REPORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(*info));
  info->nof_errors = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG
const char*
  SOC_PETRA_SHD_SCRUB_LVL_to_string(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_LVL enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_SHD_SCRUB_LVL1:
    str = "LVL1";
  break;

  case SOC_PETRA_SHD_SCRUB_LVL2:
    str = "LVL2";
  break;

  case SOC_PETRA_SHD_SCRUB_LVL3:
    str = "LVL3";
  break;

  case SOC_PETRA_SHD_SCRUB_LVL_NONE:
    str = "NONE";
  break;

  case SOC_PETRA_SHD_SCRUB_LVL_ALL:
    str = "ALL";
  break;


  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_PETRA_SHD_SCRUB_INFO_print(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Number of entries: %u\n\r",info->nof_entries);
  soc_sand_os_printf("Scrubbing level: %s\n\r",SOC_PETRA_SHD_SCRUB_LVL_to_string(info->level));
  soc_sand_os_printf("Is exit-on-repair: %s\n\r",info->is_exit_on_repair?"TRUE":"FALSE");
  soc_sand_os_printf("Is no-repair: %s\n\r",info->is_no_repair?"TRUE":"FALSE");
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_SHD_SCRUB_MARK_print(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_MARK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("  Table-id: %u\n\r",info->tbl_id);
  soc_sand_os_printf("  Offset: %u\n\r", info->offset);


exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PETRA_SHD_SCRUB_REPORT_print(
    SOC_SAND_IN SOC_PETRA_SHD_SCRUB_REPORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Number of errors: %u\n\r",info->nof_errors);
  if(info->nof_errors > 0)
  {
    soc_sand_os_printf("First error:\n\r");
    SOC_PETRA_SHD_SCRUB_MARK_print(&(info->first_error));
    soc_sand_os_printf("Bit Flip offset: %u\n\r",info->flip_bit_offset);
    soc_sand_os_printf("Is flipped zero-to one: %s\n\r",info->is_flipped_zero_to_one?"TRUE":"FALSE");
    soc_sand_os_printf("Bit stuck: %s\n\r",info->is_bit_stuck ? "TRUE" : "FALSE");
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
