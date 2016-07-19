/* $Id: pb_egr_prog_editor.c,v 1.11 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_egr_prog_editor.c
*
* MODULE PREFIX:  soc_pb_pp
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/PB_TM/pb_stack.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>
#include <soc/dpp/Petra/petra_sw_db.h>


#include <soc/dpp/SAND/Utils/sand_bitstream.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_EGR_PROG_EDITOR_LFEM_TBL_SIZE (3)

#define SOC_PETRA_PKT_FRWRD_TYPE_TM          (0xe)
#define SOC_PETRA_PKT_FRWRD_TYPE_CPU_TRAP    (0x7)

#define SOC_PB_EGR_PROG_TM_PROG_BASE         (5)

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

static uint32
  Soc_pb_egr_prog_editor_lfem_cfg[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS][SOC_PB_EGR_PROG_EDITOR_LFEM_TBL_SIZE][SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS] =
  {
    {
      /* OTMH BASE */
      /* version(0) pph_present  action_type[1:0]  mc  reserved  dp[1]  dp[0]  reserved  tc[2:0]  reserved  reserved  (reserved or ftmh[1:0]) */
      {(0x20+0),(0x00+3),(0x00+5),(0x00+4),(0x00+0),(0x20+0),(0x00+7),(0x00+6),(0x20+0),(0x00+31),(0x00+30),(0x00+29),(0x20+0),(0x20+0),(0x00+1),(0x00+0)},
      /* reserved */
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      /* PMM 16 MSB bits (timestamp field in PMM header) */
      {(0x00+15),(0x00+14),(0x00+13),(0x00+12) ,(0x00+11) ,(0x00+10) ,(0x00+9) ,(0x00+8), (0x00+7), (0x00+6), (0x00+5), (0x00+4), (0x00+3), (0x00+2),  (0x00+1), (0x00+0)},
    },
    {
      /* Stacking Port 1 */
      {(0x00+15),(0x00+14),(0x00+13),(0x00+12),(0x00+11),(0x00+10),(0x00+9),(0x00+8),(0x00+7),(0x00+6),(0x00+5),(0x00+4),(0x00+3),(0x00+2),(0x00+1),(0x00+0)},
      /* Stacking Port 2 */
      {(0x00+15),(0x00+14),(0x00+13),(0x00+12),(0x00+11),(0x00+10),(0x00+9),(0x00+8),(0x00+7),(0x00+6),(0x00+5),(0x00+4),(0x00+3),(0x00+2),(0x00+1),(0x00+0)},
      /* PMM 16 LSB bits */
      /* SQ[5]  SQ[4]  SQ[3]  SQ[2]  SQ[1]  SQ[0]  GP[3]  GP[2]  GP[1]  GP[0]  CSI[4]  CSI[3]  CSI[2]  CSI[1]  CSI[0] */
      {(0x20+0),(0x20+1),(0x20+1)  ,(0x20+1),(0x20+1) ,(0x20+1) ,(0x20+1) ,(0x00+18) ,(0x00+17), (0x00+15), (0x00+14),(0x00+23),(0x00+22), (0x00+21),(0x00+20),  (0x00+19)},
    },
    {
      /* reserved */
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      /* reserved */
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
      /* PMM 16 LSB bits - necessary (Pmc Parity En) */
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    },
  };


/* CE_INSTRUCTION: {lfem_program, bitcount, niblle_field_offset, header_offset_select, source_select, valid} */
static SOC_PB_EGR_PROG_EDITOR_CE_INSTRUCTION
  Soc_pb_egr_prog_editor_ce_cfg[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_TBLS][SOC_PB_EGR_PROG_EDITOR_CE_TBL_SIZE][SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS] =
  {
    /* Copy engine 0 */
    {
      /* 0: */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 1: LB key with CUD ext: LSB inst -Copy bytes 1-4 from the FTMH MSB, MSB inst -copy bytes 5-8 from the FTMH MSB */
      {{3,31,8,0,0,1}, {3,31,0,0,0,1}},
      /* 2: LB key without CUD ext: Copy bytes 1-4 from the FTMH MSB  + copy bytes 5-6 from the FTMH MSB */
      {{3,15,4,0,0,1}, {3,31,0,0,0,1}},
      /* 3: CPU stamping CUD into FTMH:  Copy bytes 1-4 from the FTMH MSB  + copy bytes 5-6 from the FTMH MSB */
      {{3,15,4,0,0,1}, {3,31,0,0,0,1}},
      /* 4: BUILD PMM */
      {{0,0,0,0,0,0}, {2,15,0,0,0,1}},
      /* 5: Build OTMH BASE  */
      {{0,0,0,0,0,0}, {0,15,4,0,0,1}},
      /* 6: Build OTMH BASE (LSB inst) + Build CUD */
      {{3,15,8,0,1,1}, {0,15,4,0,0,1}},
      /* 7: Build OTMH BASE + Src*/
      {{0,0,0,0,0,0}, {0,15,4,0,0,1}},
      /* 8: Build OTMH BASE (LSB inst) + Build CUD + Src*/
      {{3,15,8,0,1,1}, {0,15,4,0,0,1}},
      /* 9: Build OTMH BASE + Dest*/
      {{0,0,0,0,0,0}, {0,15,4,0,0,1}},
      /* 10: Build OTMH BASE (LSB inst) + Build CUD + Dest*/
      {{3,15,8,0,1,1}, {0,15,4,0,0,1}},
      /* 11: Build OTMH BASE + Src + Dest*/
      {{0,0,0,0,0,0}, {0,15,4,0,0,1}},
      /* 12: Build OTMH BASE (LSB inst) + Build CUD + Src + Dest*/
      {{3,15,8,0,1,1}, {0,15,4,0,0,1}},
      /* 13: LB key with CUD ext: LSB inst -Copy bytes 1-4 from the FTMH MSB, MSB inst -copy bytes 5-8 from the FTMH MSB */
      {{3,31,8,0,0,1}, {3,31,0,0,0,1}},
      /* 14: LB key without CUD ext: Copy bytes 1-4 from the FTMH MSB  + copy bytes 5-6 from the FTMH MSB */
      {{3,15,4,0,0,1}, {3,31,0,0,0,1}},
      /* 15: Injected FTMH: Copy bytes 1-4 from the FTMH MSB  + copy bytes 5-6 from the FTMH MSB */
      {{3,15,4,0,0,1}, {3,31,0,0,0,1}},
    },
    /* Copy engine 1 */
    {
      /* 0: CPU without editing */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 1: LB key with CUD ext: Copy the LB key extension + edit with LFEM */
      {{0,0,0,0,0,0}, {0,15,12,0,0,1}},
      /* 2: Copy the LB key extension + edit with LFEM */
      {{0,0,0,0,0,0}, {0,15,8,0,0,1}},
      /* 3: CPU stamping CUD into FTMH */
      {{0,0,0,0,0,0}, {3,15,8,0,1,1}},
      /* 4: BUILD PMM */
      {{0,0,0,0,0,0}, {2,14,8,0,0,1}},
      /* 5: Build OTMH BASE  */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 6: Build OTMH BASE (LSB inst) + Build CUD */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 7: Build OTMH BASE + Src*/
      {{0,0,0,0,0,0}, {3,15,0,0,1,1}},
      /* 8: Build OTMH BASE (LSB inst) + Build CUD + Src*/
      {{0,0,0,0,0,0}, {3,15,0,0,1,1}},
      /* 9: Build OTMH BASE + Dest*/
      {{3,15,4,0,1,1}, {0,0,0,0,0,0}}  ,
      /* 10: Build OTMH BASE (LSB inst) + Build CUD + Dest*/
      {{3,15,4,0,1,1}, {0,0,0,0,0,0}},
      /* 11: Build OTMH BASE + Src + Dest*/
      {{3,15,4,0,1,1}, {3,15,0,0,1,1}},
      /* 12: Build OTMH BASE (LSB inst) + Build CUD + Src + Dest*/
      {{3,15,4,0,1,1}, {3,15,0,0,1,1}},
      /* 13: LB key with CUD ext: Copy the LB key extension + edit with LFEM */
      {{0,0,0,0,0,0}, {1,15,12,0,0,1}},
      /* 14: Copy the LB key extension + edit with LFEM */
      {{0,0,0,0,0,0}, {1,15,8,0,0,1}},
      /* 15: Add the wired Out-LIF */
      {{0,0,0,0,0,0}, {3,15,8,0,1,1}},
    },
    /* Copy engine 2 */
    {
      /* 0 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 1 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 2 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 3 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 4 */
      {{0,0,0,0,0,0}, {2,0,0,0,0,1}},
      /* 5: Build OTMH BASE  */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 6: Build OTMH BASE (LSB inst) + Build CUD */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 7: Build OTMH BASE + Src*/
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 8: Build OTMH BASE (LSB inst) + Build CUD + Src*/
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 9: Build OTMH BASE + Dest*/
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 10: Build OTMH BASE (LSB inst) + Build CUD + Dest*/
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 11: Build OTMH BASE + Src + Dest*/
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 12: Build OTMH BASE (LSB inst) + Build CUD + Src + Dest*/
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 13 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 14 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
      /* 15 */
      {{0,0,0,0,0,0}, {0,0,0,0,0,0}},
    }
  };

uint32
  soc_pb_egr_prog_editor_profile_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT SOC_PB_EGR_PROG_TM_PORT_PROFILE  *eg_profile,
    SOC_SAND_OUT uint32                     *internal_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_PROG_EDITOR_PROFILE_GET);

  res = soc_pb_egq_pct_tbl_get_unsafe(unit, port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *internal_profile = pct_tbl.port_profile;
  *eg_profile = soc_pb_sw_db_eg_editor_index_profile_get(
                  unit,
                  pct_tbl.port_profile
                );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_prog_editor_profile_get()", port_ndx, 0);
}

uint32
  soc_pb_egr_prog_editor_programs_set(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    res = SOC_SAND_OK,
    tbl_ndx,
    entry_ndx,
    fld_idx,
    fld_ndx;
  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA
    lfem_tbl_data;
  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA
    ce_tbl_data;
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_PROG_EDITOR_PROGRAMS_SET);


  /* LFEM Config */
  for (tbl_ndx = 0; tbl_ndx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS; ++tbl_ndx)
  {
    for (entry_ndx = 0; entry_ndx < SOC_PB_EGR_PROG_EDITOR_LFEM_TBL_SIZE; ++entry_ndx)
    {
      sal_memset(&lfem_tbl_data, 0x0, sizeof(SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA));

      for (fld_idx = 0; fld_idx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS; ++fld_idx)
      {
        fld_ndx = SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS - fld_idx - 1;

        lfem_tbl_data.lfem_field_select_map[fld_idx] =
          Soc_pb_egr_prog_editor_lfem_cfg[tbl_ndx][entry_ndx][fld_ndx];
      }

      res = soc_pb_epni_lfem_field_select_map_tbl_set_unsafe(
              unit,
              tbl_ndx,
              entry_ndx,
              &lfem_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  /* Copy engines config */
  for (tbl_ndx = 0; tbl_ndx < SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_TBLS; ++tbl_ndx)
  {
    for (entry_ndx = 0; entry_ndx < SOC_PB_EGR_PROG_EDITOR_CE_TBL_SIZE; ++entry_ndx)
    {
      sal_memset(&ce_tbl_data, 0x0, sizeof(SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA));

      for (fld_idx = 0; fld_idx < SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS; ++fld_idx)
      {
        /*
         *	Correct the Copy Engine instructions in the right order
         */
        fld_ndx = SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS - fld_idx - 1;

        ce_tbl_data.inst_bit_count[fld_idx] =
          Soc_pb_egr_prog_editor_ce_cfg[tbl_ndx][entry_ndx][fld_ndx].bitcount;
        ce_tbl_data.inst_header_offset_select[fld_idx] =
          Soc_pb_egr_prog_editor_ce_cfg[tbl_ndx][entry_ndx][fld_ndx].header_offset_select;
        ce_tbl_data.inst_lfem_program[fld_idx] =
          Soc_pb_egr_prog_editor_ce_cfg[tbl_ndx][entry_ndx][fld_ndx].lfem_program;
        ce_tbl_data.inst_niblle_field_offset[fld_idx] =
          Soc_pb_egr_prog_editor_ce_cfg[tbl_ndx][entry_ndx][fld_ndx].niblle_field_offset;
        ce_tbl_data.inst_source_select[fld_idx] =
          Soc_pb_egr_prog_editor_ce_cfg[tbl_ndx][entry_ndx][fld_ndx].source_select;
        ce_tbl_data.inst_valid[fld_idx] =
          Soc_pb_egr_prog_editor_ce_cfg[tbl_ndx][entry_ndx][fld_ndx].valid;
      }

      res = soc_pb_epni_copy_engine_program_tbl_set_unsafe(
              unit,
              tbl_ndx,
              entry_ndx,
              &ce_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_prog_editor_programs_set()", 0, 0);
}


 
uint32
  soc_pb_egr_prog_editor_port_profile_to_hw_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PB_EGR_PROG_TM_PORT_PROFILE  eg_profile,
    SOC_SAND_IN  uint32                    internal_eg_profile_ndx,
    SOC_SAND_IN  uint8                    first_appear
  )
{
  uint32
    res = SOC_SAND_OK,
    idx,
    prog_index,
    new_header_size,
    add_nw_header,
    remove_nw_hdr,
    sys_hdr_size,
    sys_hdr_size_tmp,
    ftmh_outlif_ext_exist,
    tm_port_profile,
    tm_port_profile2,
    system_mc,
    pph_valid,
    eei_valid,
    learn_valid,
    is_tm_ext_src,
    is_tm_ext_dest,
    is_tm_cud_to_add,
    fwd_code,
    php_eep_ext_exist;
  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF
    ftmh_ext;
  SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA
    program_vars_tbl_data;
  uint8
    is_tdm_opt;
  SOC_PB_EGQ_PCT_TBL_DATA
    pct_tbl;
  uint32
    lb_ext_bytes = 0;
  SOC_PB_STACK_GLBL_INFO
    stack_glbl_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_PROG_EDITOR_TM_CONFIG);

  /*
   *  Write the Port profile
   */
  res = soc_pb_egq_pct_tbl_get_unsafe(unit, port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  pct_tbl.port_profile = internal_eg_profile_ndx;

  res = soc_pb_egq_pct_tbl_set_unsafe(unit, port_ndx, &pct_tbl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *  Update the SW DB
   */
  soc_pb_sw_db_eg_editor_index_profile_set(
    unit,
    internal_eg_profile_ndx,
    eg_profile
  );

  /*
   *  Write the Profile in the HW only if it is the first time
   */
  if (first_appear != TRUE)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  if (soc_petra_sw_db_tdm_mode_get(unit) == SOC_PETRA_MGMT_TDM_MODE_TDM_OPT)
  {
    is_tdm_opt = TRUE;
  }
  else
  {
    is_tdm_opt = FALSE;
  }
  
  res = soc_pb_ports_ftmh_extension_get_unsafe(
          unit,
          &ftmh_ext
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  php_eep_ext_exist =
    (!soc_petra_sw_db_pp_enable_get(unit) ? 0x0 :
     (soc_petra_sw_db_add_pph_eep_ext_get(unit) ? 0x1 : 0x0));

  /* idx[10:0] = {out_tm_port_profile[3:0],system_mc[0], pph_valid[0],eei_valid[0],learn_valid[0],fwd_code[3:0]} */

  idx = 0;
  prog_index = 0;
  new_header_size = 0;
  add_nw_header = 0;
  remove_nw_hdr = 0;
  sys_hdr_size = 0;

  idx = 0;
  SOC_PB_STACK_GLBL_INFO_clear(&stack_glbl_info);
  res = soc_pb_stack_global_info_get_unsafe(
          unit,
          &stack_glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  switch (stack_glbl_info.max_nof_tm_domains)
  {
  case SOC_PB_STACK_MAX_NOF_TM_DOMAINS_8:
    lb_ext_bytes = 1;
    break;

  case SOC_PB_STACK_MAX_NOF_TM_DOMAINS_16:
    lb_ext_bytes = 2;
    break;
  
  case SOC_PB_STACK_MAX_NOF_TM_DOMAINS_1:
  default:
    lb_ext_bytes = 0;
    break;
  }

  tm_port_profile = eg_profile;
  for (system_mc = 0; system_mc <= 1; ++system_mc) /* idx[7] */
  {
    for (pph_valid = 0; pph_valid <= 1; ++pph_valid) /* idx[6] */
    {
      for (eei_valid = 0; eei_valid <= 1; ++eei_valid) /* idx[5] */
      {
        for (learn_valid = 0; learn_valid <= 1; ++learn_valid) /* idx[4] */
        {
          for (fwd_code = 0; fwd_code < 16; ++fwd_code) /* idx[3:0] */
          {
            ftmh_outlif_ext_exist =
              ((ftmh_ext == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_ALWAYS) ||
               ((ftmh_ext == SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC) && system_mc));
            sys_hdr_size_tmp = 6 /*FTMH*/ + (2 * ftmh_outlif_ext_exist) + lb_ext_bytes;

            if (pph_valid)
            {
              sys_hdr_size_tmp += 8;
              if (php_eep_ext_exist)
              {
                /* when soc_petra_sw_db_pp_enable_get(unit) is true and soc_petra_sw_db_add_pph_eep_ext_get(unit) is true also,
                   php_eep_ext_exist will be 1 */
                /* coverity[dead_error_line : FALSE] */
                sys_hdr_size_tmp += 2;
              }
            }
            sys_hdr_size_tmp += (3*eei_valid);
            sys_hdr_size_tmp += (5*learn_valid);
            
            switch (tm_port_profile)
            {
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU: /* CPU without editing maybe need to add the stamping of the cud ?? */
              prog_index = 0;
              new_header_size = 0;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = 0;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_RAW: /* Raw without editing remove all system headers including PPH */
              prog_index = 0;
              new_header_size = 0;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = sys_hdr_size_tmp;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1: /* Stacking Port With/Without CUD ext */
              if (ftmh_outlif_ext_exist)
              {
                prog_index = 1; /* 1 with cud ext 2 - without cud ext */
                new_header_size = 10;
                sys_hdr_size = 10;
              }
              else
              {
                prog_index = 2;
                new_header_size = 8;
                sys_hdr_size = 8;
              }
              add_nw_header    = 0;
              remove_nw_hdr = 0;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK2: /* Stacking Port With/Without CUD ext */
              if (ftmh_outlif_ext_exist)
              {
                prog_index = 13; /* 13 with cud ext 14 - without cud ext */
                new_header_size = 10;
                sys_hdr_size = 10;
              }
              else
              {
                prog_index = 14;
                new_header_size = 8;
                sys_hdr_size = 8;
              }
              add_nw_header    = 0;
              remove_nw_hdr = 0;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_FAP20: /* fap20b rcy only increase the packet in two bytes */
              prog_index = 0;
              new_header_size = 10;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = 8;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU_CUD: /* CPU without editing but add the stamping of the cud */
              prog_index = 3;
              new_header_size = 8;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = 8;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM1: /* TDM  mode: removing FTMH */
              prog_index = 0;
              new_header_size = 0;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = (is_tdm_opt ? 2 : 8 );
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM2: /* TDM  mode: remove standard FTMH and generating PMM header */
              prog_index = 4;
              new_header_size = 4;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = (is_tdm_opt ? 2 : 8 );
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_ETH: /* Remove system headers and old Network header and Add new Network Header */
              prog_index = 0;
              new_header_size = 0;
              sys_hdr_size = 0;
              if ((pph_valid==0) ||
                  (fwd_code == SOC_PETRA_PKT_FRWRD_TYPE_TM) ||
                  (fwd_code == SOC_PETRA_PKT_FRWRD_TYPE_CPU_TRAP)) /* TM packet */
              {
                add_nw_header = 0;
                remove_nw_hdr = 0;
                sys_hdr_size = sys_hdr_size_tmp;
              }
              else
              {
                add_nw_header = 1;
                remove_nw_hdr = 1;
              }
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_INJECTED: /* Injected: FTMH+ add wired OutLIF (2B) */
              prog_index = 15;
              new_header_size = 8;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = 6;
              break;

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV: /* TM base only */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC:  /* No source, No Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW: /* No Source, No Destination, CUD always */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_NEV:    /* No Source, Destination, Never CUD */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_MC:     /* No source, Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_ALW:    /* No Source, Destination, CUD always */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV:    /* Source, No Destination, Never CUD. */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC:     /* Source, No Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW:    /* Source, No Destination, CUD always. */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_NEV:       /* Source, Destination, Never CUD */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_MC:        /* Source, Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_ALW:       /* Source, Destination, CUD always. */

            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV_WITH_PPH: /* TM base only */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC_WITH_PPH:  /* No source, No Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW_WITH_PPH: /* No Source, No Destination, CUD always */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_NEV_WITH_PPH:    /* No Source, Destination, Never CUD */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_MC_WITH_PPH:     /* No source, Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_ALW_WITH_PPH:    /* No Source, Destination, CUD always */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV_WITH_PPH:    /* Source, No Destination, Never CUD. */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC_WITH_PPH:     /* Source, No Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW_WITH_PPH:    /* Source, No Destination, CUD always. */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_NEV_WITH_PPH:       /* Source, Destination, Never CUD */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_MC_WITH_PPH:        /* Source, Destination, CUD if Multicast */
            case SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_ALW_WITH_PPH:       /* Source, Destination, CUD always. */

              tm_port_profile2 = tm_port_profile;
              if (tm_port_profile >= SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV_WITH_PPH)
              {
                tm_port_profile2 = (tm_port_profile - SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV_WITH_PPH)
                   + SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV;
                /*
                 * Keep the PPH if exists
                 */
                sys_hdr_size_tmp = 6 /*FTMH*/ + (2 * ftmh_outlif_ext_exist);
              }

              SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_CUD_PRESENT_GET(tm_port_profile2, is_tm_cud_to_add);
              SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_GET(tm_port_profile2, is_tm_ext_src);
              SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_DEST_GET(tm_port_profile2, is_tm_ext_dest);
              prog_index = SOC_PB_EGR_PROG_TM_PROG_BASE + is_tm_cud_to_add + (2 * is_tm_ext_src) + (4 * is_tm_ext_dest);
              new_header_size = 2 + (2 * (is_tm_cud_to_add + is_tm_ext_src + is_tm_ext_dest));
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = sys_hdr_size_tmp;
              break;

            default:
              prog_index = 0;
              new_header_size = 0;
              add_nw_header = 0;
              remove_nw_hdr = 0;
              sys_hdr_size = 0;
              break;
            }

            res = soc_pb_pp_epni_program_vars_tbl_get_unsafe(
                    unit,
                    idx + (256 * internal_eg_profile_ndx),
                    &program_vars_tbl_data
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            program_vars_tbl_data.program_index = prog_index;
            program_vars_tbl_data.new_header_size = new_header_size;
            program_vars_tbl_data.add_network_header = add_nw_header;
            program_vars_tbl_data.remove_network_header = remove_nw_hdr;
            program_vars_tbl_data.system_header_size = sys_hdr_size;

            res = soc_pb_pp_epni_program_vars_tbl_set_unsafe(
                    unit,
                    idx + (256 * internal_eg_profile_ndx),
                    &program_vars_tbl_data
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            ++idx;
          }
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_prog_editor_port_profile_to_hw_add()", 0, 0);
}

uint32
  soc_pb_egr_prog_editor_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    port_ndx;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_PROG_EDITOR_UNSAFE);

  /*
   * Set the Egress Editor programs
   */
  res = soc_pb_egr_prog_editor_programs_set(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   * Set all ports to CPU
   */
  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PB_PROFILE_PARAMS_clear(&profile_params);
  profile_params.eg_profile = SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU;
  for (port_ndx = 0; port_ndx < SOC_PETRA_NOF_LOCAL_PORTS; port_ndx++)
  {
    profile_ndx.port_ndx = port_ndx;
    res = soc_pb_profile_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
            &profile_ndx,
            &profile_params,
            TRUE,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_prog_editor_unsafe()", 0, 0);
}

uint32
  soc_pb_egr_prog_editor_stack_prune_bitmap_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 stack_profile,
    SOC_SAND_IN uint32  bitmap[SOC_PB_STACK_PRUN_BMP_LEN]
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ndx;
  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA
    lfem_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_PROG_EDITOR_STACK_PRUNE_BITMAP_SET);

  res = soc_pb_epni_lfem_field_select_map_tbl_get_unsafe(
          unit,
          1,
          stack_profile - 1,
          &lfem_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (ndx = 0; ndx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS; ++ndx)
  {
    if (soc_sand_bitstream_test_bit(bitmap, ndx) == 1)
    {
      lfem_tbl_data.lfem_field_select_map[ndx] = 0x21;
    }
    else
    {
      lfem_tbl_data.lfem_field_select_map[ndx] = 0x00 | ndx;
    }
  }

  res = soc_pb_epni_lfem_field_select_map_tbl_set_unsafe(
          unit,
          1,
          stack_profile - 1,
          &lfem_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_prog_editor_stack_prune_bitmap_set()", 0, 0);
}

uint32
  soc_pb_egr_prog_editor_stack_prune_bitmap_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 stack_profile,
    SOC_SAND_OUT uint32  bitmap[SOC_PB_STACK_PRUN_BMP_LEN]
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ndx;
  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA
    lfem_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGR_PROG_EDITOR_STACK_PRUNE_BITMAP_GET);

  res = soc_pb_epni_lfem_field_select_map_tbl_get_unsafe(
          unit,
          1,
          stack_profile - 1,
          &lfem_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (ndx = 0; ndx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS; ++ndx)
  {
    if (lfem_tbl_data.lfem_field_select_map[ndx] & 0x20)
    {
      soc_sand_bitstream_set_bit(bitmap, ndx);
    }
    else
    {
      soc_sand_bitstream_reset_bit(bitmap, ndx);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egr_prog_editor_stack_prune_bitmap_get()", 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

