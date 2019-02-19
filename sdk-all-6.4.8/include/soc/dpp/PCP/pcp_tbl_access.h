/* $Id: pcp_tbl_access.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PCP_TBL_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PCP_TBL_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
                                                                    
#include <soc/dpp/PCP/pcp_api_framework.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_api_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define PCP_ECI_QDR_TBL_ENTRY_SIZE                                                       1
#define PCP_ECI_RLD1_TBL_ENTRY_SIZE                                                      3
#define PCP_ECI_RLD2_TBL_ENTRY_SIZE                                                      3
#define PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE                                    1
#define PCP_ELK_FID_COUNTER_DB_TBL_ENTRY_SIZE                                            1
#define PCP_ELK_SYS_PORT_IS_MINE_TBL_ENTRY_SIZE                                          1
#define PCP_ELK_AGING_CFG_TABLE_TBL_ENTRY_SIZE                                           1
#define PCP_ELK_FLUSH_DB_TBL_ENTRY_SIZE                                                  7
#define PCP_ELK_DSP_EVENT_TABLE_TBL_ENTRY_SIZE                                           1
#define PCP_OAM_PR2_TCDP_TBL_ENTRY_SIZE                                                  1
#define PCP_OAM_EXT_MEP_INDEX_DB_TBL_ENTRY_SIZE                                          1
#define PCP_OAM_EXT_MEP_DB_TBL_ENTRY_SIZE                                                3
#define PCP_OAM_MEP_DB_TBL_ENTRY_SIZE                                                    3
#define PCP_OAM_RMEP_DB_TBL_ENTRY_SIZE                                                   1
#define PCP_OAM_RMEP_HASH_0_DB_TBL_ENTRY_SIZE                                            1
#define PCP_OAM_RMEP_HASH_1_DB_TBL_ENTRY_SIZE                                            1
#define PCP_OAM_LMDB_CMN_TBL_ENTRY_SIZE                                                  11
#define PCP_OAM_LMDB_TX_TBL_ENTRY_SIZE                                                   1
#define PCP_OAM_DMDB_TX_TBL_ENTRY_SIZE                                                   2
#define PCP_OAM_DMDB_RX_TBL_ENTRY_SIZE                                                   1
#define PCP_STS_ING_COUNTERS_TBL_ENTRY_SIZE                                              3
#define PCP_STS_EGR_COUNTERS_TBL_ENTRY_SIZE                                              2

#define PCP_NUM_OF_INDIRECT_MODULES  4
/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define PCP_TBL_NUM(tbl_ptr)       (PCP_TBL(tbl_ptr))
#define PCP_TBL(tbl_ptr)           0 /* currently not working. in pb there were "tables->b_##tbl_ptr" insted of 0 */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*************
 * GLOBALS   *
 *************/
/* { */

typedef enum
{

 PCP_ECI_QDR_TBL_GET = PCP_PROC_DESC_BASE_TBL_ACCESS_FIRST,
 PCP_ECI_QDR_TBL_SET,
 PCP_ECI_RLD1_TBL_GET,
 PCP_ECI_RLD1_TBL_SET,
 PCP_ECI_RLD2_TBL_GET,
 PCP_ECI_RLD2_TBL_SET,
 PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_GET,
 PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_SET,
 PCP_ELK_FID_COUNTER_DB_TBL_GET,
 PCP_ELK_FID_COUNTER_DB_TBL_SET,
 PCP_ELK_SYS_PORT_IS_MINE_TBL_GET,
 PCP_ELK_SYS_PORT_IS_MINE_TBL_SET,
 PCP_ELK_AGING_CFG_TABLE_TBL_GET,
 PCP_ELK_AGING_CFG_TABLE_TBL_SET,
 PCP_ELK_FLUSH_DB_TBL_GET,
 PCP_ELK_FLUSH_DB_TBL_SET,
 PCP_ELK_DSP_EVENT_TABLE_TBL_GET,
 PCP_ELK_DSP_EVENT_TABLE_TBL_SET,
 PCP_OAM_PR2_TCDP_TBL_GET,
 PCP_OAM_PR2_TCDP_TBL_SET,
 PCP_OAM_MEP_DB_TBL_GET,
 PCP_OAM_MEP_DB_TBL_SET,
 PCP_OAM_EXT_MEP_INDEX_DB_TBL_GET,
 PCP_OAM_EXT_MEP_INDEX_DB_TBL_SET,
 PCP_OAM_EXT_MEP_DB_TBL_GET,
 PCP_OAM_EXT_MEP_DB_TBL_SET,
 PCP_OAM_RMEP_DB_TBL_GET,
 PCP_OAM_RMEP_DB_TBL_SET,
 PCP_OAM_RMEP_HASH_0_DB_TBL_GET,
 PCP_OAM_RMEP_HASH_0_DB_TBL_SET,
 PCP_OAM_RMEP_HASH_1_DB_TBL_GET,
 PCP_OAM_RMEP_HASH_1_DB_TBL_SET,
 PCP_OAM_LMDB_CMN_TBL_GET,
 PCP_OAM_LMDB_CMN_TBL_SET,
 PCP_OAM_LMDB_TX_TBL_GET,
 PCP_OAM_LMDB_TX_TBL_SET,
 PCP_OAM_DMDB_TX_TBL_SET,
 PCP_OAM_DMDB_TX_TBL_GET,
 PCP_OAM_DMDB_RX_TBL_GET,
 PCP_OAM_DMDB_RX_TBL_SET,
 PCP_STS_ING_COUNTERS_TBL_GET,
 PCP_STS_ING_COUNTERS_TBL_SET,
 PCP_STS_EGR_COUNTERS_TBL_GET,
 PCP_STS_EGR_COUNTERS_TBL_SET,
 PCP_OAM_FIELD_IN_PLACE_SET,
 PCP_OAM_FIELD_IN_PLACE_GET,
 PCP_ECI_SET_REPS_FOR_TBL_UNSAFE,
 PCP_ELK_SET_REPS_FOR_TBL_UNSAFE,
 PCP_OAM_SET_REPS_FOR_TBL_UNSAFE,
 PCP_STS_SET_REPS_FOR_TBL_UNSAFE,
 PCP_ECI_QDR_TBL_GET_UNSAFE,
 PCP_ECI_QDR_TBL_SET_UNSAFE,
 PCP_ECI_RLD1_TBL_GET_UNSAFE,
 PCP_ECI_RLD1_TBL_SET_UNSAFE,
 PCP_ECI_RLD2_TBL_GET_UNSAFE,
 PCP_ECI_RLD2_TBL_SET_UNSAFE,
 PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_GET_UNSAFE,
 PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_SET_UNSAFE,
 PCP_ELK_FID_COUNTER_DB_TBL_GET_UNSAFE,
 PCP_ELK_FID_COUNTER_DB_TBL_SET_UNSAFE,
 PCP_ELK_SYS_PORT_IS_MINE_TBL_GET_UNSAFE,
 PCP_ELK_SYS_PORT_IS_MINE_TBL_SET_UNSAFE,
 PCP_ELK_AGING_CFG_TABLE_TBL_GET_UNSAFE,
 PCP_ELK_AGING_CFG_TABLE_TBL_SET_UNSAFE,
 PCP_ELK_FLUSH_DB_TBL_GET_UNSAFE,
 PCP_ELK_FLUSH_DB_TBL_SET_UNSAFE,
 PCP_ELK_DSP_EVENT_TABLE_TBL_GET_UNSAFE,
 PCP_ELK_DSP_EVENT_TABLE_TBL_SET_UNSAFE,
 PCP_OAM_PR2_TCDP_TBL_GET_UNSAFE,
 PCP_OAM_PR2_TCDP_TBL_SET_UNSAFE,
 PCP_OAM_EXT_MEP_INDEX_DB_TBL_GET_UNSAFE,
 PCP_OAM_EXT_MEP_INDEX_DB_TBL_SET_UNSAFE,
 PCP_OAM_EXT_MEP_DB_TBL_GET_UNSAFE,
 PCP_OAM_EXT_MEP_DB_TBL_SET_UNSAFE,
 PCP_OAM_MEP_DB_TBL_GET_UNSAFE,
 PCP_OAM_MEP_DB_TBL_SET_UNSAFE,
 PCP_OAM_RMEP_DB_TBL_GET_UNSAFE,
 PCP_OAM_RMEP_DB_TBL_SET_UNSAFE,
 PCP_OAM_RMEP_HASH_0_DB_TBL_GET_UNSAFE,
 PCP_OAM_RMEP_HASH_0_DB_TBL_SET_UNSAFE,
 PCP_OAM_RMEP_HASH_1_DB_TBL_GET_UNSAFE,
 PCP_OAM_RMEP_HASH_1_DB_TBL_SET_UNSAFE,
 PCP_OAM_LMDB_CMN_TBL_GET_UNSAFE,
 PCP_OAM_LMDB_CMN_TBL_SET_UNSAFE,
 PCP_OAM_LMDB_TX_TBL_GET_UNSAFE,
 PCP_OAM_LMDB_TX_TBL_SET_UNSAFE,
 PCP_OAM_DMDB_TX_TBL_GET_UNSAFE,
 PCP_OAM_DMDB_TX_TBL_SET_UNSAFE,
 PCP_OAM_DMDB_RX_TBL_GET_UNSAFE,
 PCP_OAM_DMDB_RX_TBL_SET_UNSAFE,
 PCP_STS_ING_COUNTERS_TBL_GET_UNSAFE,
 PCP_STS_ING_COUNTERS_TBL_SET_UNSAFE,
 PCP_STS_EGR_COUNTERS_TBL_GET_UNSAFE,
 PCP_STS_EGR_COUNTERS_TBL_SET_UNSAFE,

  /*
   * Last element. Do no touch.
   */
  PCP_TBL_ACCESS_PROCEDURE_DESC_LAST
} PCP_TBL_ACCESS_PROCEDURE_DESC;

typedef enum
{
  PCP_TBL_RANGE_OUT_OF_LIMIT_ERR = PCP_ERR_DESC_BASE_TBL_ACCESS_FIRST,
  PCP_OAM_REGS_FIELD_VAL_OUT_OF_RANGE_ERR,
 
  /*
   * Last element. Do no touch.
   */
  PCP_TBL_ACCESS_ERR_LAST
} PCP_TBL_ACCESS_ERR;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  pcp_eci_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

uint32
  pcp_elk_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

uint32
  pcp_oam_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

uint32
  pcp_sts_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );

uint32
  pcp_eci_qdr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_QDR_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_qdr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_QDR_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD1_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD2_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD2_TBL_DATA  *tbl_data
  );

/* write to both D1 and D2 same data */
uint32
  pcp_eci_rld_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld_helper_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              rld_ndx,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data,/* data in lsbs */
    SOC_SAND_IN  uint32              side /*1-left, 2-right, 3-both */
  );


uint32
  pcp_eci_rld_lpm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,      /* LPM bank */
    SOC_SAND_IN  uint32              entry_offset, /* relative to the bank, consider entry size is 36 and not 72 */
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data, /* includes lpm data, size is 36 bits*/
    SOC_SAND_IN  uint8             full/*0-only 36 lsb is valid, 1-entry in include full line*/
  );

uint32
  pcp_eci_lpm4_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              mem_ndx,      /* not used*/
    SOC_SAND_IN  uint32              entry_offset,  /* offset in bank */
    SOC_SAND_IN  uint32              tbl_data[3], /* data in 36 lsb bits*/
    SOC_SAND_IN  uint32              flags  /*not used*/
  );

uint32
  pcp_elk_fid_counter_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_sys_port_is_mine_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_sys_port_is_mine_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_aging_cfg_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_aging_cfg_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_flush_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_flush_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_dsp_event_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_dsp_event_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_pr2_tcdp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_pr2_tcdp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_index_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_index_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_mep_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_mep_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_0_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_1_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_cmn_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_cmn_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_tx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_tx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_tx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_tx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_rx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_rx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_ing_counters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_ing_counters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_egr_counters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_egr_counters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  );

CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_tbl_access_get_procs_ptr(void);

CONST SOC_ERROR_DESC_ELEMENT*
  pcp_tbl_access_get_errs_ptr(void);

/* } */
                                                                    
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
/* } __SOC_PCP_TBL_ACCESS_H_INCLUDED__*/
#endif
