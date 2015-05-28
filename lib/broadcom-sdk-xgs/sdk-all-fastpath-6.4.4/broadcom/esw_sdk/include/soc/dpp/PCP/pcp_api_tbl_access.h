/* $Id: pcp_api_tbl_access.h,v 1.5 Broadcom SDK $
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
*/

#ifndef __SOC_PCP_API_TBL_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PCP_API_TBL_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
                                                                    
#include <soc/dpp/PCP/pcp_api_framework.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

#include <soc/dpp/PCP/pcp_chip_tbls.h>

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

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef struct
{
  uint32 qdr_data;
} PCP_ECI_QDR_TBL_DATA;

typedef struct
{
  uint32 rld1_data[3];
} PCP_ECI_RLD1_TBL_DATA;

typedef struct
{
  uint32 rld2_data[3];
} PCP_ECI_RLD2_TBL_DATA;

typedef struct
{
  uint32 limit;
  uint32 interrupt_en;
  uint32 limit_en;
  uint32 is_ll_fid;
} PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA;

typedef struct
{
  uint32 entry_count;
  uint32 profile_pointer;
} PCP_ELK_FID_COUNTER_DB_TBL_DATA;

typedef struct
{
  uint32 port_is_mine;
} PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA;

typedef struct
{
  uint32 aging_cfg_info_delete_entry[PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];
  uint32 aging_cfg_info_create_aged_out_event[PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];
  uint32 aging_cfg_info_create_refresh_event[PCP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED];
} PCP_ELK_AGING_CFG_TABLE_TBL_DATA;

typedef struct
{
  uint32 compare_valid;
  uint32 key_fid;
  uint32 key_mask;
  uint32 payload[2];
  uint32 payload_mask[2];
  uint32 drop;
  uint32 new_payload[2];
  uint32 new_payload_mask[2];
} PCP_ELK_FLUSH_DB_TBL_DATA;

typedef struct
{
  uint32 to_lpbck;
  uint32 to_fifo2;
  uint32 to_fifo1;
} PCP_ELK_DSP_EVENT_TABLE_TBL_DATA;

typedef struct
{
  uint32 tc;
  uint32 dp;
} PCP_OAM_PR2_TCDP_TBL_DATA;

typedef struct
{
  uint32 mep_db_ptr;
  uint32 accelerate;
} PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA;

typedef struct
{
  uint32 out_ac;
  uint32 mepid;
  uint32 vid_vld;
  uint32 vid;
  uint32 ptc_header;
} PCP_OAM_EXT_MEP_DB_TBL_DATA;

typedef struct
{
  uint32 ccm_interval;
  uint32 maid;
  uint32 ccm_tx_start;
  uint32 dm_enable;
  uint32 dm_db_index;
  uint32 lm_enable;
  uint32 lmm_enable;
  uint32 lmr_enable;
  uint32 lm_db_index;
  uint32 destination;
  uint32 ccm_pr;
  uint32 up1down0;
  uint32 sa_lsb;
  uint32 rdi_indicator;
  uint32 cos_profile;
  uint32 md_level;
} PCP_OAM_MEP_DB_TBL_DATA;

typedef struct
{
  uint32 ccm_interval;
  uint32 ccm_defect;
  uint32 rcvd_rdi;
} PCP_OAM_RMEP_DB_TBL_DATA;

typedef struct
{
  uint32 rmep_db_ptr0;
  uint32 verifier0;
  uint32 valid_hash0;
} PCP_OAM_RMEP_HASH_0_DB_TBL_DATA;

typedef struct
{
  uint32 rmep_db_ptr1;
  uint32 verifier1;
  uint32 valid_hash1;
} PCP_OAM_RMEP_HASH_1_DB_TBL_DATA;

typedef struct
{
  uint32 tx_fcf_c;
  uint32 tx_fcf_p;
  uint32 rx_fcb_peer_c;
  uint32 rx_fcb_peer_p;
  uint32 rx_fcb_c;
  uint32 rx_fcb_p;
  uint32 tx_fcb_peer_c;
  uint32 tx_fcb_peer_p;
  uint32 rx_fcb_indx_msb;
  uint32 tx_fcf_indx_msb;
  uint32 lmm_da[2];
}PCP_OAM_LMDB_CMN_TBL_DATA;

typedef struct
{
  uint32 lmm_interval;
  uint32 lm_pr;
} PCP_OAM_LMDB_TX_TBL_DATA;

typedef struct
{
  uint32 dm_interval;
  uint32 dm_da[2];
  uint32 dm_pr;
} PCP_OAM_DMDB_TX_TBL_DATA;

typedef struct
{
  uint32 dm_delay;
  uint32 dm_delay_valid;
} PCP_OAM_DMDB_RX_TBL_DATA;

typedef struct
{
  uint32 packets[2];
  uint32 octets[2];
} PCP_STS_ING_COUNTERS_TBL_DATA;

typedef struct
{
  uint32 packets[2];
} PCP_STS_EGR_COUNTERS_TBL_DATA;

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

uint32
  pcp_indirect_module_init(void);

uint32
  pcp_indirect_module_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  );

uint32
  pcp_eci_qdr_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_QDR_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_qdr_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_QDR_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld1_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD1_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld1_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD1_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld2_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ECI_RLD2_TBL_DATA  *tbl_data
  );

uint32
  pcp_eci_rld2_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ECI_RLD2_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_profile_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_profile_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_fid_counter_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FID_COUNTER_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_sys_port_is_mine_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_sys_port_is_mine_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_SYS_PORT_IS_MINE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_aging_cfg_table_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_aging_cfg_table_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_AGING_CFG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_flush_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_flush_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_ELK_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_dsp_event_table_tbl_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      entry_offset,
    SOC_SAND_OUT PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_elk_dsp_event_table_tbl_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      entry_offset,
    SOC_SAND_IN  PCP_ELK_DSP_EVENT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_pr2_tcdp_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_pr2_tcdp_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_PR2_TCDP_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_index_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_index_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_INDEX_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_ext_mep_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_EXT_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_mep_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_mep_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_MEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_0_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_0_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_0_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_1_db_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_rmep_hash_1_db_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_RMEP_HASH_1_DB_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_cmn_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_cmn_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_CMN_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_tx_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_lmdb_tx_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_LMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_tx_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_tx_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_TX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_rx_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  );

uint32
  pcp_oam_dmdb_rx_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_OAM_DMDB_RX_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_ing_counters_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_ing_counters_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_ING_COUNTERS_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_egr_counters_tbl_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  );

uint32
  pcp_sts_egr_counters_tbl_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  PCP_STS_EGR_COUNTERS_TBL_DATA  *tbl_data
  );

/* } */
                                                                    
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
/* } __SOC_PCP_API_TBL_ACCESS_H_INCLUDED__*/
#endif
