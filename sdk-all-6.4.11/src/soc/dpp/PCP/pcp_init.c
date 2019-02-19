/* $Id: pcp_init.c,v 1.7 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>

#include <soc/dpp/PCP/pcp_mgmt.h>
#include <soc/dpp/PCP/pcp_oam_api_general.h>

#include <soc/dpp/PCP/pcp_chip_tbls.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_sw_db.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#if PCP_DEBUG_IS_LVL1
  #define PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(f_res, err_num, exit_label, size)  \
  {                                                                             \
    SOC_SAND_CHECK_FUNC_RESULT(f_res,err_num,exit_label)                            \
    if(!silent)                                                                 \
    {                                                                           \
      Pcp_mgmt_tbls_curr_workload += size;                                                 \
      soc_sand_workload_status_run(unit, Pcp_mgmt_tbls_curr_workload);                    \
    }                                                                           \
  }
#else
  #define PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(f_res, err_num, exit_label, size)  \
  {                                                                             \
    SOC_SAND_CHECK_FUNC_RESULT(f_res,err_num,exit_label)                            \
  }
#endif
/*
 *  max value of the count
 *   it's equal to the maximum value count field may get - 1
*/

#define PCP_INIT_COUNT_MAX(val)   \
            (PCP_FLD_MAX(val) - 1)

/*
 *  Prints phase 1 initialization advance.
 *  Assumes the following variables are defined:
 *   - uint8 silent
 *   - uint32 stage_id
 */


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

/*
 *  Total number of lines in pcp tables.
 */
static uint32
    Pcp_mgmt_tbls_total_lines = 0;
static uint32
    Pcp_mgmt_tbls_curr_workload = 0;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 *  Tables Initialization {
 */
STATIC void
  pcp_mgmt_tbl_total_lines_set(void)
{
  uint32
    lines_count_total = 0;

  PCP_TBLS
    *tables;

  tables = pcp_tbls();

  lines_count_total =
    tables->eci.qdr_tbl.addr.size +
    tables->eci.rld1_tbl.addr.size +
    tables->eci.rld2_tbl.addr.size +
    tables->elk.fid_counter_profile_db_tbl.addr.size +
    tables->elk.fid_counter_db_tbl.addr.size +
    tables->elk.sys_port_is_mine_tbl.addr.size +
    tables->elk.aging_cfg_table_tbl.addr.size +
    tables->elk.flush_db_tbl.addr.size +
    tables->elk.dsp_event_table_tbl.addr.size +
    tables->oam.pr2_tcdp_tbl.addr.size +
    tables->oam.ext_mep_index_db_tbl.addr.size +
    tables->oam.ext_mep_db_tbl.addr.size +
    tables->oam.mep_db_tbl.addr.size +
    tables->oam.rmep_db_tbl.addr.size +
    tables->oam.rmep_hash_0_db_tbl.addr.size +
    tables->oam.rmep_hash_1_db_tbl.addr.size +
    tables->oam.lmdb_cmn_tbl.addr.size +
    tables->oam.lmdb_tx_tbl.addr.size +
    tables->oam.dmdb_tx_tbl.addr.size +
    tables->oam.dmdb_rx_tbl.addr.size +
    tables->sts.ing_counters_tbl.addr.size +
    tables->sts.egr_counters_tbl.addr.size + 0;

  Pcp_mgmt_tbls_total_lines = lines_count_total;
}

STATIC uint32
  pcp_mgmt_tbl_total_lines_get(void)

{
  return Pcp_mgmt_tbls_total_lines;
}

STATIC uint32
  pcp_mgmt_eci_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    lines_count_total = 0,
    first_entry = 0;
  PCP_TBLS
    *tables;
  PCP_REGS
    *regs;
  PCP_ECI_QDR_TBL_DATA
    eci_qdr_tbl_data;
  PCP_ECI_RLD1_TBL_DATA
    eci_rld1_tbl_data;
  PCP_ECI_RLD2_TBL_DATA
    eci_rld2_tbl_data;
  uint8
	  op_mode_elk_enable,
	  op_mode_oam_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_ECI_TBLS_INIT);

  op_mode_elk_enable   = soc_petra_sw_db_op_mode_elk_enable_get(unit);
  op_mode_oam_enable = soc_petra_sw_db_op_mode_oam_enable_get(unit);

  res = pcp_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = pcp_regs();

  ret = SOC_SAND_OK; sal_memset(&eci_qdr_tbl_data, 0x0, sizeof(eci_qdr_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&eci_rld1_tbl_data, 0x0, sizeof(eci_rld1_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&eci_rld2_tbl_data, 0x0, sizeof(eci_rld2_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  lines_count_total = pcp_mgmt_tbl_total_lines_get();
#if PCP_DEBUG_IS_LVL1
  if (!silent)
  {
    soc_sand_workload_status_start(
       unit,
       lines_count_total
     );
  }
#endif
  if ((op_mode_elk_enable == TRUE) || (op_mode_oam_enable == TRUE)) {
	  /*Qdr*/
	  nof_lines_total = tables->eci.qdr_tbl.addr.size;
	  lines_max = PCP_INIT_COUNT_MAX(regs->eci.indirect_command_reg.indirect_command_count);
	  do {
		   nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
		   nof_lines_total -= nof_lines;
	
		   res = pcp_eci_set_reps_for_tbl_unsafe(unit, nof_lines);
		   SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);
	
		   res = pcp_eci_qdr_tbl_set_unsafe(
					 unit,
					 first_entry,
					 &eci_qdr_tbl_data
				 );
		   PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 102, exit, nof_lines);
		   first_entry += nof_lines;
	  } while(nof_lines_total > 0);
	  first_entry = 0;
	
	  /*Rld1*/
	  nof_lines_total = tables->eci.rld1_tbl.addr.size;
	  lines_max = PCP_INIT_COUNT_MAX(regs->eci.indirect_command_reg.indirect_command_count);
	  do {
		   nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
		   nof_lines_total -= nof_lines;
	
		   res = pcp_eci_set_reps_for_tbl_unsafe(unit, nof_lines);
		   SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);
	
		   res = pcp_eci_rld1_tbl_set_unsafe(
					 unit,
					 first_entry,
					 &eci_rld1_tbl_data
				 );
		   PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 104, exit, nof_lines);
		   first_entry += nof_lines;
	  } while(nof_lines_total > 0);
	  first_entry = 0;
	
	  /*Rld2*/
	  nof_lines_total = tables->eci.rld2_tbl.addr.size;
	  lines_max = PCP_INIT_COUNT_MAX(regs->eci.indirect_command_reg.indirect_command_count);
	  do {
		   nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
		   nof_lines_total -= nof_lines;
	
		   res = pcp_eci_set_reps_for_tbl_unsafe(unit, nof_lines);
		   SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);
	
		   res = pcp_eci_rld2_tbl_set_unsafe(
					 unit,
					 first_entry,
					 &eci_rld2_tbl_data
				 );
		   PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 106, exit, nof_lines);
		   first_entry += nof_lines;
	  } while(nof_lines_total > 0);
	  first_entry = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_eci_tbls_init()",0,0);
}

STATIC uint32
  pcp_mgmt_elk_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    lines_count_total = 0,
    first_entry = 0;
  PCP_TBLS
    *tables;
  PCP_REGS
    *regs;
  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA
    elk_fid_counter_profile_db_tbl_data;
  PCP_ELK_FID_COUNTER_DB_TBL_DATA
    elk_fid_counter_db_tbl_data;
  PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA
    elk_sys_port_is_mine_tbl_data;
  PCP_ELK_AGING_CFG_TABLE_TBL_DATA
    elk_aging_cfg_table_tbl_data;
  PCP_ELK_FLUSH_DB_TBL_DATA
    elk_flush_db_tbl_data;
  PCP_ELK_DSP_EVENT_TABLE_TBL_DATA
    elk_dsp_event_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_ELK_TBLS_INIT);

  res = pcp_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = pcp_regs();

  ret = SOC_SAND_OK; sal_memset(&elk_fid_counter_profile_db_tbl_data, 0x0, sizeof(elk_fid_counter_profile_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&elk_fid_counter_db_tbl_data, 0x0, sizeof(elk_fid_counter_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&elk_sys_port_is_mine_tbl_data, 0x0, sizeof(elk_sys_port_is_mine_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&elk_aging_cfg_table_tbl_data, 0x0, sizeof(elk_aging_cfg_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&elk_flush_db_tbl_data, 0x0, sizeof(elk_flush_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&elk_dsp_event_table_tbl_data, 0x0, sizeof(elk_dsp_event_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  lines_count_total = pcp_mgmt_tbl_total_lines_get();
#if PCP_DEBUG_IS_LVL1
  if (!silent)
  {
    soc_sand_workload_status_start(
       unit,
       lines_count_total
     );
  }
#endif
  /*Fid Counter Profile Db*/
  nof_lines_total = tables->elk.fid_counter_profile_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->elk.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_elk_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = pcp_elk_fid_counter_profile_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &elk_fid_counter_profile_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 102, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Fid Counter Db*/
  nof_lines_total = tables->elk.fid_counter_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->elk.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_elk_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = pcp_elk_fid_counter_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &elk_fid_counter_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 104, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Sys Port Is Mine*/
  nof_lines_total = tables->elk.sys_port_is_mine_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->elk.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_elk_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = pcp_elk_sys_port_is_mine_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &elk_sys_port_is_mine_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 106, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Aging Cfg Table*/
  nof_lines_total = tables->elk.aging_cfg_table_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->elk.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_elk_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = pcp_elk_aging_cfg_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &elk_aging_cfg_table_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 108, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Flush DB*/
  nof_lines_total = tables->elk.flush_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->elk.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_elk_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = pcp_elk_flush_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &elk_flush_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 110, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Dsp Event Table*/
  nof_lines_total = tables->elk.dsp_event_table_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->elk.indirect_command_reg.indirect_command_count);
  do {
    nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
    nof_lines_total -= nof_lines;

    res = pcp_elk_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

    res = pcp_elk_dsp_event_table_tbl_set_unsafe(
      unit,
      first_entry,
      &elk_dsp_event_table_tbl_data
      );
    PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 110, exit, nof_lines);
    first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_elk_tbls_init()",0,0);
}

STATIC uint32
  pcp_mgmt_oam_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    lines_count_total = 0,
    first_entry = 0;
  PCP_TBLS
    *tables;
  PCP_REGS
    *regs;
  PCP_OAM_PR2_TCDP_TBL_DATA
    oam_pr2_tcdp_tbl_data;
  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA
    oam_ext_mep_index_db_tbl_data;
  PCP_OAM_EXT_MEP_DB_TBL_DATA
    oam_ext_mep_db_tbl_data;
  PCP_OAM_MEP_DB_TBL_DATA
    oam_mep_db_tbl_data;
  PCP_OAM_RMEP_DB_TBL_DATA
    oam_rmep_db_tbl_data;
  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA
    oam_rmep_hash_0_db_tbl_data;
  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA
    oam_rmep_hash_1_db_tbl_data;
  PCP_OAM_LMDB_CMN_TBL_DATA
    oam_lmdb_cmn_tbl_data;
  PCP_OAM_LMDB_TX_TBL_DATA
    oam_lmdb_tx_tbl_data;
  PCP_OAM_DMDB_TX_TBL_DATA
    oam_dmdb_tx_tbl_data;
  PCP_OAM_DMDB_RX_TBL_DATA
    oam_dmdb_rx_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OAM_TBLS_INIT);

  res = pcp_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = pcp_regs();

  ret = SOC_SAND_OK; sal_memset(&oam_pr2_tcdp_tbl_data, 0x0, sizeof(oam_pr2_tcdp_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_ext_mep_index_db_tbl_data, 0x0, sizeof(oam_ext_mep_index_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_ext_mep_db_tbl_data, 0x0, sizeof(oam_ext_mep_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_mep_db_tbl_data, 0x0, sizeof(oam_mep_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_rmep_db_tbl_data, 0x0, sizeof(oam_rmep_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_rmep_hash_0_db_tbl_data, 0x0, sizeof(oam_rmep_hash_0_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_rmep_hash_1_db_tbl_data, 0x0, sizeof(oam_rmep_hash_1_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_lmdb_cmn_tbl_data, 0x0, sizeof(oam_lmdb_cmn_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_lmdb_tx_tbl_data, 0x0, sizeof(oam_lmdb_tx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 22, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_dmdb_tx_tbl_data, 0x0, sizeof(oam_dmdb_tx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  ret = SOC_SAND_OK; sal_memset(&oam_dmdb_rx_tbl_data, 0x0, sizeof(oam_dmdb_rx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 24, exit);

  lines_count_total = pcp_mgmt_tbl_total_lines_get();
#if PCP_DEBUG_IS_LVL1
  if (!silent)
  {
    soc_sand_workload_status_start(
       unit,
       lines_count_total
     );
  }
#endif
  /*PR2 TCDP*/
  nof_lines_total = tables->oam.pr2_tcdp_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = pcp_oam_pr2_tcdp_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_pr2_tcdp_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 102, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ext MEP index DB*/
  nof_lines_total = tables->oam.ext_mep_index_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = pcp_oam_ext_mep_index_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_ext_mep_index_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 104, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ext MEP DB*/
  nof_lines_total = tables->oam.ext_mep_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = pcp_oam_ext_mep_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_ext_mep_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 106, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*MEP DB*/
  nof_lines_total = tables->oam.mep_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = pcp_oam_mep_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_mep_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 114, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*RMEP DB*/
  nof_lines_total = tables->oam.rmep_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = pcp_oam_rmep_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_rmep_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 116, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*RMEP HASH 0 DB*/
  nof_lines_total = tables->oam.rmep_hash_0_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = pcp_oam_rmep_hash_0_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_rmep_hash_0_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 118, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*RMEP HASH 1 DB*/
  nof_lines_total = tables->oam.rmep_hash_1_db_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = pcp_oam_rmep_hash_1_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_rmep_hash_1_db_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 120, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*LMDB CMN*/
  nof_lines_total = tables->oam.lmdb_cmn_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = pcp_oam_lmdb_cmn_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_lmdb_cmn_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 122, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*LMDB tx*/
  nof_lines_total = tables->oam.lmdb_tx_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = pcp_oam_lmdb_tx_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_lmdb_tx_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 124, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*DMDBtx*/
  nof_lines_total = tables->oam.dmdb_tx_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = pcp_oam_dmdb_tx_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_dmdb_tx_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 126, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*DMDB rx*/
  nof_lines_total = tables->oam.dmdb_rx_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->oam.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_oam_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 127, exit);

       res = pcp_oam_dmdb_rx_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &oam_dmdb_rx_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 128, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_oam_tbls_init()",0,0);
}

uint32
  pcp_mgmt_sts_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    lines_count_total = 0,
    first_entry = 0;
  PCP_TBLS
    *tables;
  PCP_REGS
    *regs;
  PCP_STS_ING_COUNTERS_TBL_DATA
    sts_ing_counters_tbl_data;
  PCP_STS_EGR_COUNTERS_TBL_DATA
    sts_egr_counters_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_STS_TBLS_INIT);

  res = pcp_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = pcp_regs();

  ret = SOC_SAND_OK; sal_memset(&sts_ing_counters_tbl_data, 0x0, sizeof(sts_ing_counters_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&sts_egr_counters_tbl_data, 0x0, sizeof(sts_egr_counters_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  lines_count_total = pcp_mgmt_tbl_total_lines_get();
#if PCP_DEBUG_IS_LVL1
  if (!silent)
  {
    soc_sand_workload_status_start(
       unit,
       lines_count_total
     );
  }
#endif
  /*Ing Counters*/
  nof_lines_total = tables->sts.ing_counters_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->sts.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_sts_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = pcp_sts_ing_counters_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &sts_ing_counters_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 102, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egr Counters*/
  nof_lines_total = tables->sts.egr_counters_tbl.addr.size;
  lines_max = PCP_INIT_COUNT_MAX(regs->sts.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = pcp_sts_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = pcp_sts_egr_counters_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &sts_egr_counters_tbl_data
             );
       PCP_CHECK_FUNC_RESULT_AND_PROMOTE_DISPLAY(res, 104, exit, nof_lines);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_sts_tbls_init()",0,0);
}

/* tables dump */
uint32
  pcp_diag_tbls_dump_tables_get(
      SOC_SAND_OUT PCP_TBL_PRINT_INFO  *indirect_print,
      SOC_SAND_IN  uint8              block_id,
      SOC_SAND_IN  uint32             table_offset,
      SOC_SAND_OUT uint32            *nof_tbls
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_TBLS
    *tables;
  uint32
    counter = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_DIAG_TBLS_DUMP_TABLES_GET);
  
  SOC_SAND_CHECK_NULL_INPUT(indirect_print);
  SOC_SAND_CHECK_NULL_INPUT(nof_tbls);

  res = pcp_tbls_get(
    &tables
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  if (block_id == PCP_ECI_ID || block_id == PCP_NOF_MODULES)
  {
    /* Qdr */
    if (table_offset == PCP_TBL_NUM(tables->eciqdr_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->eciqdr_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->eciqdr_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->eciqdr_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ECI_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ECI.qdr_tbl";
#endif
      counter++;
    }
    /* Rld1 */
    if (table_offset == PCP_TBL_NUM(tables->ecirld1_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->ecirld1_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->ecirld1_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->ecirld1_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ECI_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ECI.rld1_tbl";
#endif
      counter++;
    }
    /* Rld2 */
    if (table_offset == PCP_TBL_NUM(tables->ecirld2_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->ecirld2_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->ecirld2_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->ecirld2_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ECI_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ECI.rld2_tbl";
#endif
      counter++;
    }
  }
  if (block_id == PCP_ELK_ID || block_id == PCP_NOF_MODULES)
  {
    /* Fid Counter Profile Db */
    if (table_offset == PCP_TBL_NUM(tables->elkfid_counter_profile_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->elkfid_counter_profile_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->elkfid_counter_profile_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->elkfid_counter_profile_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ELK_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ELK.fid_counter_profile_db_tbl";
#endif
      counter++;
    }
    /* Fid Counter Db */
    if (table_offset == PCP_TBL_NUM(tables->elkfid_counter_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->elkfid_counter_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->elkfid_counter_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->elkfid_counter_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ELK_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ELK.fid_counter_db_tbl";
#endif
      counter++;
    }
    /* Sys Port Is Mine */
    if (table_offset == PCP_TBL_NUM(tables->elksys_port_is_mine_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->elksys_port_is_mine_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->elksys_port_is_mine_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->elksys_port_is_mine_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ELK_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ELK.sys_port_is_mine_tbl";
#endif
      counter++;
    }
    /* Aging Cfg Table */
    if (table_offset == PCP_TBL_NUM(tables->elkaging_cfg_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->elkaging_cfg_table_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->elkaging_cfg_table_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->elkaging_cfg_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ELK_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ELK.aging_cfg_table_tbl";
#endif
      counter++;
    }
    /* Flush DB */
    if (table_offset == PCP_TBL_NUM(tables->elkflush_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->elkflush_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->elkflush_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->elkflush_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ELK_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ELK.flush_db_tbl";
#endif
      counter++;
    }
    /* Dsp Event Table */
    if (table_offset == PCP_TBL_NUM(tables->elkdsp_event_table_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->elkdsp_event_table_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->elkdsp_event_table_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->elkfdsp_event_table_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_ELK_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "ELK.dsp_event_table_tbl";
#endif
      counter++;
    }
  }
  if (block_id == PCP_OAM_ID || block_id == PCP_NOF_MODULES)
  {
    /* PR2 TCDP */
    if (table_offset == PCP_TBL_NUM(tables->oampr2_tcdp_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oampr2_tcdp_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oampr2_tcdp_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oampr2_tcdp_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.pr2_tcdp_tbl";
#endif
      counter++;
    }
    /* Ext MEP index DB */
    if (table_offset == PCP_TBL_NUM(tables->oamext_mep_index_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamext_mep_index_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamext_mep_index_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamext_mep_index_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.ext_mep_index_db_tbl";
#endif
      counter++;
    }
    /* Ext MEP DB */
    if (table_offset == PCP_TBL_NUM(tables->oamext_mep_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamext_mep_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamext_mep_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamext_mep_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.ext_mep_db_tbl";
#endif
      counter++;
    }
    /* Ext MEP T1 DB */
    if (table_offset == PCP_TBL_NUM(tables->oamext_mep_t1_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamext_mep_t1_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamext_mep_t1_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamext_mep_t1_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.ext_mep_t1_db_tbl";
#endif
      counter++;
    }
    /* Ext MEP T2 DB */
    if (table_offset == PCP_TBL_NUM(tables->oamext_mep_t2_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamext_mep_t2_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamext_mep_t2_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamext_mep_t2_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.ext_mep_t2_db_tbl";
#endif
      counter++;
    }
    /* Ext MEP T3 DB */
    if (table_offset == PCP_TBL_NUM(tables->oamext_mep_t3_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamext_mep_t3_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamext_mep_t3_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamext_mep_t3_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.ext_mep_t3_db_tbl";
#endif
      counter++;
    }
    /* MEP DB */
    if (table_offset == PCP_TBL_NUM(tables->oammep_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oammep_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oammep_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oammep_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.mep_db_tbl";
#endif
      counter++;
    }
    /* RMEP DB */
    if (table_offset == PCP_TBL_NUM(tables->oamrmep_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamrmep_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamrmep_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamrmep_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.rmep_db_tbl";
#endif
      counter++;
    }
    /* RMEP HASH 0 DB */
    if (table_offset == PCP_TBL_NUM(tables->oamrmep_hash_0_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamrmep_hash_0_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamrmep_hash_0_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamrmep_hash_0_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.rmep_hash_0_db_tbl";
#endif
      counter++;
    }
    /* RMEP HASH 1 DB */
    if (table_offset == PCP_TBL_NUM(tables->oamrmep_hash_1_db_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamrmep_hash_1_db_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamrmep_hash_1_db_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamrmep_hash_1_db_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.rmep_hash_1_db_tbl";
#endif
      counter++;
    }
    /* LMDB CMN */
    if (table_offset == PCP_TBL_NUM(tables->oamlmdb_cmn_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamlmdb_cmn_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamlmdb_cmn_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamlmdb_cmn_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.lmdb_cmn_tbl";
#endif
      counter++;
    }
    /* LMDB tx */
    if (table_offset == PCP_TBL_NUM(tables->oamlmdb_tx_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamlmdb_tx_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamlmdb_tx_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamlmdb_tx_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.lmdb_tx_tbl";
#endif
      counter++;
    }
    /* DMDB tx */
    if (table_offset == PCP_TBL_NUM(tables->oamdmdb_tx_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamdmdb_tx_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamdmdb_tx_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamdmdb_tx_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.dmdb_tx_tbl";
#endif
      counter++;
    }
    /* DMDB rx */
    if (table_offset == PCP_TBL_NUM(tables->oamdmdb_rx_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->oamdmdb_rx_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->oamdmdb_rx_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->oamdmdb_rx_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_OAM_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OAM.dmdb_rx_tbl";
#endif
      counter++;
    }
  }
  if (block_id == PCP_STS_ID || block_id == PCP_NOF_MODULES)
  {
    /* Ing Counters */
    if (table_offset == PCP_TBL_NUM(tables->stsing_counters_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->stsing_counters_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->stsing_counters_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->stsing_counters_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_STS_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "STS.ing_counters_tbl";
#endif
      counter++;
    }
    /* Egr Counters */
    if (table_offset == PCP_TBL_NUM(tables->stsegr_counters_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = PCP_TBL_NUM(tables->stsegr_counters_tbl.addr.base);
      indirect_print[counter].size   = PCP_TBL_NUM(tables->stsegr_counters_tbl.addr.size);
      indirect_print[counter].wrd_sz   = PCP_WIDTH_BITS_TO_WIDTH_BYTES(PCP_TBL_NUM(tables->stsegr_counters_tbl.addr.width_bits));
      indirect_print[counter].mod_id   = PCP_STS_ID;
#ifdef PCP_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "STS.egr_counters_tbl";
#endif
      counter++;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_diag_tbls_dump_tables_get()",0,0);
}
uint32
  pcp_mgmt_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
	  op_mode_elk_enable,
	  op_mode_oam_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_TBLS_INIT);

  pcp_mgmt_tbl_total_lines_set();

  op_mode_elk_enable   = soc_petra_sw_db_op_mode_elk_enable_get(unit);
  op_mode_oam_enable = soc_petra_sw_db_op_mode_oam_enable_get(unit);

  res = pcp_mgmt_eci_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  if (op_mode_elk_enable == TRUE) {
	  res = pcp_mgmt_elk_tbls_init(unit, silent);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }

  if (op_mode_oam_enable == TRUE) {
	  res = pcp_mgmt_oam_tbls_init(unit, silent);
	  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
  }

  res = pcp_mgmt_sts_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_tbls_init()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

