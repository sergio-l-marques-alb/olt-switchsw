/* $Id: pcp_chip_tbls.c,v 1.9 Broadcom SDK $
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
 
#include <soc/dpp/PCP/pcp_chip_tbls.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_api_framework.h>
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

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_chip_tbls[] =
{
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_TBLS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_TBLS_INIT),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_chip_tbls[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_TBLS_NOT_INITIALIZED_ERR,
    "PCP_TBLS_NOT_INITIALIZED_ERR",
    "pcp tables haven't been initialized.\n\r "
    "Check Management init.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static PCP_TBLS  Pcp_tbls;
static uint8    Pcp_tbls_initialized = FALSE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* pcp_tbls_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT PCP_TBLS  **pcp_tbls - pointer to pcp
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *pcp_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    pcp_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  pcp_tbls_get(
    SOC_SAND_OUT PCP_TBLS  **pcp_tbls
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_TBLS_GET);

  res = pcp_tbls_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *pcp_tbls = &Pcp_tbls;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_is_tbl_db_initialized()", 0, 0);
}

/*****************************************************
*NAME
* pcp_tbls
*TYPE:
*  PROC
*FUNCTION:
*  Return a pointer to tables database.
*  The database is per chip-version. Without Error Checking
*INPUT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    *pcp_tbls.
*REMARKS:
*    No checking is performed whether the DB is initialized
*SEE ALSO:
*****************************************************/
PCP_TBLS*
  pcp_tbls(void)
{

  PCP_TBLS*
    tbls = NULL;

  pcp_tbls_get(
    &tbls
  );

  return tbls;
}

/*****************************************************
*NAME
* pcp_tbl_fld_set
*TYPE:
*  PROC
*FUNCTION:
*  Sets a pcp table field
*INPUT:
*  SOC_SAND_DIRECT:
*    PCP_TBL_FIELD *field - pointer to pcp table field structure
*    uint16  msb -  field most significant bit
*    uint16  lsb -  field least significant bit
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    field.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  pcp_tbl_fld_set(
    PCP_TBL_FIELD *field,
    uint16       msb,
    uint16       lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}



/* Block tables initialization: ECI 	 */
STATIC void
  pcp_tbls_init_ECI(void)
{

  /* Qdr */
  Pcp_tbls.eci.qdr_tbl.addr.base = 0x00000000;
  Pcp_tbls.eci.qdr_tbl.addr.size = 0x100000;
  Pcp_tbls.eci.qdr_tbl.addr.width_bits = 18;
  pcp_tbl_fld_set( &(Pcp_tbls.eci.qdr_tbl.qdr_data), 17, 0);

  /* Rld1 */
  Pcp_tbls.eci.rld1_tbl.addr.base = 0x00800000;
  Pcp_tbls.eci.rld1_tbl.addr.size = 0x800000;
  Pcp_tbls.eci.rld1_tbl.addr.width_bits = 72;
  pcp_tbl_fld_set( &(Pcp_tbls.eci.rld1_tbl.rld1_data), 71, 0);

  /* Rld2 */
  Pcp_tbls.eci.rld2_tbl.addr.base = 0x01000000;
  Pcp_tbls.eci.rld2_tbl.addr.size = 0x800000;
  Pcp_tbls.eci.rld2_tbl.addr.width_bits = 72;
  pcp_tbl_fld_set( &(Pcp_tbls.eci.rld2_tbl.rld2_data), 71, 0);

  /* Rld3 */
  Pcp_tbls.eci.rld3_tbl.addr.base = 0x01800000;
  Pcp_tbls.eci.rld3_tbl.addr.size = 0x800000;
  Pcp_tbls.eci.rld3_tbl.addr.width_bits = 72;
  pcp_tbl_fld_set( &(Pcp_tbls.eci.rld3_tbl.rld3_data), 71, 0);

}

/* Block tables initialization: ELK 	 */
STATIC void
  pcp_tbls_init_ELK(void)
{

  /* Fid Counter Profile Db */
  Pcp_tbls.elk.fid_counter_profile_db_tbl.addr.base = 0x00000000;
  Pcp_tbls.elk.fid_counter_profile_db_tbl.addr.size = 0x0008;
  Pcp_tbls.elk.fid_counter_profile_db_tbl.addr.width_bits = 16;
  pcp_tbl_fld_set( &(Pcp_tbls.elk.fid_counter_profile_db_tbl.limit), 12, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.fid_counter_profile_db_tbl.interrupt_en), 13, 13);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.fid_counter_profile_db_tbl.limit_en), 14, 14);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.fid_counter_profile_db_tbl.is_ll_fid), 15, 15);

  /* Fid Counter Db */
  Pcp_tbls.elk.fid_counter_db_tbl.addr.base = 0x00800000;
  Pcp_tbls.elk.fid_counter_db_tbl.addr.size = 0x4000;
  Pcp_tbls.elk.fid_counter_db_tbl.addr.width_bits = 16;
  pcp_tbl_fld_set( &(Pcp_tbls.elk.fid_counter_db_tbl.entry_count), 12, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.fid_counter_db_tbl.profile_pointer), 15, 13);

  /* Sys Port Is Mine */
  Pcp_tbls.elk.sys_port_is_mine_tbl.addr.base = 0x01000000;
  Pcp_tbls.elk.sys_port_is_mine_tbl.addr.size = 0x2000;
  Pcp_tbls.elk.sys_port_is_mine_tbl.addr.width_bits = 1;
  pcp_tbl_fld_set( &(Pcp_tbls.elk.sys_port_is_mine_tbl.port_is_mine), 0, 0);

  /* Aging Cfg Table */
  Pcp_tbls.elk.aging_cfg_table_tbl.addr.base = 0x01800000;
  Pcp_tbls.elk.aging_cfg_table_tbl.addr.size = 0x0020;
  Pcp_tbls.elk.aging_cfg_table_tbl.addr.width_bits = 6;
  pcp_tbl_fld_set( &(Pcp_tbls.elk.aging_cfg_table_tbl.aging_cfg_info_delete_entry[TRUE]), 0, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.aging_cfg_table_tbl.aging_cfg_info_create_aged_out_event[TRUE]), 1, 1);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.aging_cfg_table_tbl.aging_cfg_info_create_refresh_event[TRUE]), 2, 2);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.aging_cfg_table_tbl.aging_cfg_info_delete_entry[FALSE]), 3, 3);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.aging_cfg_table_tbl.aging_cfg_info_create_aged_out_event[FALSE]), 4, 4);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.aging_cfg_table_tbl.aging_cfg_info_create_refresh_event[FALSE]), 5, 5);

  /* Flush DB */
  Pcp_tbls.elk.flush_db_tbl.addr.base = 0x02000000;
  Pcp_tbls.elk.flush_db_tbl.addr.size = 0x0002;
  Pcp_tbls.elk.flush_db_tbl.addr.width_bits = 196;
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.compare_valid), 0, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.key_fid), 15, 1);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.key_mask), 30, 16);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.payload), 71, 31);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.payload_mask), 112, 72);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.drop), 113, 113);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.new_payload), 154, 114);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.flush_db_tbl.new_payload_mask), 195, 155);

  /* Dsp Event Table */
  Pcp_tbls.elk.dsp_event_table_tbl.addr.base = 0x02800000;
  Pcp_tbls.elk.dsp_event_table_tbl.addr.size = 0x0040;
  Pcp_tbls.elk.dsp_event_table_tbl.addr.width_bits = 3;
  pcp_tbl_fld_set( &(Pcp_tbls.elk.dsp_event_table_tbl.to_lpbck), 0, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.dsp_event_table_tbl.to_fifo2), 1, 1);
  pcp_tbl_fld_set( &(Pcp_tbls.elk.dsp_event_table_tbl.to_fifo1), 2, 2);
}

/* Block tables initialization: OAM 	 */
STATIC void
  pcp_tbls_init_OAM(void)
{

  /* PR2 TCDP */
  Pcp_tbls.oam.pr2_tcdp_tbl.addr.base = 0x00000000;
  Pcp_tbls.oam.pr2_tcdp_tbl.addr.size = 0x0020;
  Pcp_tbls.oam.pr2_tcdp_tbl.addr.width_bits = 5;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.pr2_tcdp_tbl.tc), 2, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.pr2_tcdp_tbl.dp), 4, 3);

  /* Ext MEP index DB */
  Pcp_tbls.oam.ext_mep_index_db_tbl.addr.base = 0x00080000;
  Pcp_tbls.oam.ext_mep_index_db_tbl.addr.size = 0x40000;
  Pcp_tbls.oam.ext_mep_index_db_tbl.addr.width_bits = 13;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_index_db_tbl.mep_db_ptr), 11, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_index_db_tbl.accelerate), 12, 12);

  /* Ext MEP DB */
  Pcp_tbls.oam.ext_mep_db_tbl.addr.base = 0x000c0000;
  Pcp_tbls.oam.ext_mep_db_tbl.addr.size = 0x1000;
  Pcp_tbls.oam.ext_mep_db_tbl.addr.width_bits = 72;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_db_tbl.out_ac), 13, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_db_tbl.mepid), 26, 14);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_db_tbl.vid_vld), 27, 27);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_db_tbl.vid), 39, 28);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.ext_mep_db_tbl.ptc_header), 71, 40);

  /* MEP DB */
  Pcp_tbls.oam.mep_db_tbl.addr.base = 0x00800000;
  Pcp_tbls.oam.mep_db_tbl.addr.size = 0x1000;
  Pcp_tbls.oam.mep_db_tbl.addr.width_bits = 79;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.ccm_interval), 2, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.maid), 18, 3);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.ccm_tx_start), 28, 19);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.dm_enable), 29, 29);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.dm_db_index), 36, 30);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.lm_enable), 37, 37);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.lmm_enable), 38, 38);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.lmr_enable), 39, 39);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.lm_db_index), 46, 40);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.destination), 62, 47);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.ccm_pr), 65, 63);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.up1down0), 66, 66);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.sa_lsb), 73, 67);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.rdi_indicator), 74, 74);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.cos_profile), 76, 75);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.mep_db_tbl.md_level), 79, 77);

  /* RMEP DB */
  Pcp_tbls.oam.rmep_db_tbl.addr.base = 0x01000000;
  Pcp_tbls.oam.rmep_db_tbl.addr.size = 0x2000;
  Pcp_tbls.oam.rmep_db_tbl.addr.width_bits = 5;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_db_tbl.ccm_interval), 2, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_db_tbl.ccm_defect), 3, 3);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_db_tbl.rcvd_rdi), 4, 4);

  /* RMEP HASH 0 DB */
  Pcp_tbls.oam.rmep_hash_0_db_tbl.addr.base = 0x01800000;
  Pcp_tbls.oam.rmep_hash_0_db_tbl.addr.size = 0x2000;
  Pcp_tbls.oam.rmep_hash_0_db_tbl.addr.width_bits = 26;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_hash_0_db_tbl.rmep_db_ptr0), 12, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_hash_0_db_tbl.verifier0), 24, 13);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_hash_0_db_tbl.valid_hash0), 25, 25);

  /* RMEP HASH 1 DB */
  Pcp_tbls.oam.rmep_hash_1_db_tbl.addr.base = 0x02000000;
  Pcp_tbls.oam.rmep_hash_1_db_tbl.addr.size = 0x2000;
  Pcp_tbls.oam.rmep_hash_1_db_tbl.addr.width_bits = 26;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_hash_1_db_tbl.rmep_db_ptr1), 12, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_hash_1_db_tbl.verifier1), 24, 13);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.rmep_hash_1_db_tbl.valid_hash1), 25, 25);

  /* LMDB CMN */
  Pcp_tbls.oam.lmdb_cmn_tbl.addr.base = 0x02800000;
  Pcp_tbls.oam.lmdb_cmn_tbl.addr.size = 0x0080;
  Pcp_tbls.oam.lmdb_cmn_tbl.addr.width_bits = 326;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.tx_fcf_c), 31, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.tx_fcf_p), 63, 32);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.rx_fcb_peer_c), 95, 64);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.rx_fcb_peer_p), 127, 96);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.rx_fcb_c), 159, 128);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.rx_fcb_p), 191, 160);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.tx_fcb_peer_c), 223, 192);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.tx_fcb_peer_p), 255, 224);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.rx_fcb_indx_msb), 266, 256);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.tx_fcf_indx_msb), 277, 267);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_cmn_tbl.lmm_da), 325, 278);

  /* LMDB tx */
  Pcp_tbls.oam.lmdb_tx_tbl.addr.base = 0x03000000;
  Pcp_tbls.oam.lmdb_tx_tbl.addr.size = 0x0080;
  Pcp_tbls.oam.lmdb_tx_tbl.addr.width_bits = 6;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_tx_tbl.lmm_interval), 2, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.lmdb_tx_tbl.lm_pr), 5, 3);

  /* DMDBtx */
  Pcp_tbls.oam.dmdb_tx_tbl.addr.base = 0x03800000;
  Pcp_tbls.oam.dmdb_tx_tbl.addr.size = 0x0080;
  Pcp_tbls.oam.dmdb_tx_tbl.addr.width_bits = 54;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.dmdb_tx_tbl.dm_interval), 2, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.dmdb_tx_tbl.dm_da), 50, 3);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.dmdb_tx_tbl.dm_pr), 53, 51);

  /* DMDBrx */
  Pcp_tbls.oam.dmdb_rx_tbl.addr.base = 0x04000000;
  Pcp_tbls.oam.dmdb_rx_tbl.addr.size = 0x0080;
  Pcp_tbls.oam.dmdb_rx_tbl.addr.width_bits = 21;
  pcp_tbl_fld_set( &(Pcp_tbls.oam.dmdb_rx_tbl.dm_delay), 19, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.oam.dmdb_rx_tbl.dm_delay_valid), 20, 20);
}

/* Block tables initialization: STS 	 */
STATIC void
  pcp_tbls_init_STS(void)
{

  /* Ing Counters */
  Pcp_tbls.sts.ing_counters_tbl.addr.base = 0x00000000;
  Pcp_tbls.sts.ing_counters_tbl.addr.size = 0x4000;
  Pcp_tbls.sts.ing_counters_tbl.addr.width_bits = 80;
  pcp_tbl_fld_set( &(Pcp_tbls.sts.ing_counters_tbl.packets), 35, 0);
  pcp_tbl_fld_set( &(Pcp_tbls.sts.ing_counters_tbl.octets), 79, 36);

  /* Egr Counters */
  Pcp_tbls.sts.egr_counters_tbl.addr.base = 0x00004000;
  Pcp_tbls.sts.egr_counters_tbl.addr.size = 0x4000;
  Pcp_tbls.sts.egr_counters_tbl.addr.width_bits = 36;
  pcp_tbl_fld_set( &(Pcp_tbls.sts.egr_counters_tbl.packets), 35, 0);
}
/*****************************************************
*NAME
*  pcp_tbls_init
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Dynamically allocates and initializes Pcp tables database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized tables will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  pcp_tbls_init(
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_TBLS_INIT);

  if (Pcp_tbls_initialized == TRUE)
  {
    goto exit;
  }

  pcp_tbls_init_ECI();
  pcp_tbls_init_ELK();
  pcp_tbls_init_OAM();
  pcp_tbls_init_STS();

  Pcp_tbls_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_tbls_init",0,0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_chip_tbls_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_chip_tbls;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_eth module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_chip_tbls_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_chip_tbls;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

