/* $Id: pb_ingress_traffic_mgmt.c,v 1.6 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_ingress_traffic_mgmt.h>

#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_INGRESS_TRAFFIC_MGMT_RT_CLS4_NDX_MAX                (3)
#define SOC_PB_ITM_STAG_INFO_ENABLE_MODE_MAX                       (SOC_PB_ITM_NOF_STAG_ENABLE_MODES-1)
#define SOC_PB_ITM_STAG_INFO_OFFSETB__MAX                          (63)
#define SOC_PB_ITM_GRNT_BYTES_MAX                                  (256*1024*1024)
#define SOC_PB_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX               (128*1024*1024)
#define SOC_PB_ITM_DISCARD_DP_MAX                                  (4)

#define SOC_PB_ITM_COMMITTED_MANTISSA_NOF_BITS                     (6)
#define SOC_PB_ITM_COMMITTED_EXPONENT_NOF_BITS                     (5)
#define SOC_PB_ITM_COMMITTED_BYTES_RESOLUTION                      (16)

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

/*********************************************************************
* NAME:
*     soc_pb_itm_init
* FUNCTION:
*     Initialization of the Soc_petra-B blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. Called as part of the initialization sequence.
*   2. This functions contains only the additional features, on top
*      of the Soc_petra-A initialization as implemented in soc_petra_itm_init()
*********************************************************************/
uint32
  soc_pb_itm_init(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    dp_idx;
  SOC_PETRA_THRESH_WITH_HYST_INFO
    global_drop[SOC_PETRA_NOF_DROP_PRECEDENCE],
    global_drop_exact[SOC_PETRA_NOF_DROP_PRECEDENCE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_INIT);

  for (dp_idx = 0; dp_idx < SOC_PETRA_NOF_DROP_PRECEDENCE; dp_idx++)
  {
    soc_petra_PETRA_THRESH_WITH_HYST_INFO_clear(&(global_drop[dp_idx]));
    global_drop[dp_idx].set = SOC_PB_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX;
    global_drop[dp_idx].clear = SOC_PB_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX - (SOC_PB_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX/8);
  }

  res = soc_pb_itm_glob_rcs_drop_set_unsafe(
          unit,
          global_drop,
          global_drop_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_itm_init()",0,0);
}


/*********************************************************************
*     Defines the way the Statistics Tag is used. The
 *     statistics tag can be used in the Statistics Interface,
 *     for mapping to VSQs and further used for VSQ-based
 *     drop/FC decisions.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_itm_stag_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint8
    stag_en,
    stag_vsq_en;
  SOC_PB_ITM_STAG_INFO
    stag_info_previous;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_STAG_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   * Get the previous configuration
   */
  res = soc_pb_itm_stag_get_unsafe(
          unit,
          &stag_info_previous
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(info->enable_mode)
  {
  case SOC_PB_ITM_STAG_ENABLE_MODE_DIS:
    stag_en = FALSE;
    stag_vsq_en = FALSE;
    break;
  
  case SOC_PB_ITM_STAG_ENABLE_MODE_EN_NO_VSQ:
    stag_en = TRUE;
    stag_vsq_en = FALSE;
    break;

  case SOC_PB_ITM_STAG_ENABLE_MODE_EN_WITH_VSQ:
    stag_en = TRUE;
    stag_vsq_en = TRUE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_ITM_ENABLE_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_PB_FLD_SET(regs->eci.general_controls_reg.stat_tag_en, stag_en, 20, exit);
  SOC_PB_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_en, stag_vsq_en, 30, exit);

  /*
   * Set the Statistic-Tag offset and update all the related TM PMF-Programs
   * if necessary
   */
  soc_pb_sw_db_stag_offset_nibble_set(
    unit,
    info->offset_4bits
  );

  if (info->offset_4bits != stag_info_previous.offset_4bits)
  {
    res = soc_pb_pmf_pgm_mgmt_update(
            unit,
            SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_stag_set_unsafe()", 0, 0);
}

uint32
  soc_pb_itm_stag_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_STAG_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_ITM_STAG_INFO, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_stag_set_verify()", 0, 0);
}

/*********************************************************************
*     Defines the way the Statistics Tag is used. The
 *     statistics tag can be used in the Statistics Interface,
 *     for mapping to VSQs and further used for VSQ-based
 *     drop/FC decisions.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_itm_stag_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_ITM_STAG_INFO            *info
  )
{
  uint32
    stag_en,
    stag_vsq_en,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_STAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_ITM_STAG_INFO_clear(info);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->eci.general_controls_reg.stat_tag_en, stag_en, 10, exit);
  SOC_PB_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_en, stag_vsq_en, 20, exit);

  if (stag_en == FALSE)
  {
    info->enable_mode = SOC_PB_ITM_STAG_ENABLE_MODE_DIS;
  }
  else if (stag_vsq_en == FALSE)
  {
    info->enable_mode = SOC_PB_ITM_STAG_ENABLE_MODE_EN_NO_VSQ;
  }
  else
  {
    info->enable_mode = SOC_PB_ITM_STAG_ENABLE_MODE_EN_WITH_VSQ;
  }

  /*
   * Get the Stag-Offset
   */
  info->offset_4bits = soc_pb_sw_db_stag_offset_nibble_get(unit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_stag_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the size of committed queue size (i.e., the
 *     guaranteed memory) for each VOQ, even in the case that a
 *     set of queues consume most of the memory resources.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_itm_committed_q_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32        			     rt_cls4_ndx,
    SOC_SAND_IN  uint32                    grnt_bytes,
    SOC_SAND_OUT uint32                    *exact_grnt_bytes
  )
{
  uint32
    gt_bytes_exact,
    res = SOC_SAND_OK;
  uint32
    fld_ndx,
    reg_ndx,
    gr_bytes_mnt = 0,
    gr_bytes_exp = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_COMMITTED_Q_SIZE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exact_grnt_bytes);

  regs = soc_petra_regs();

  /*
   *	Compute the exponent and the mantissa
   */
  res = soc_sand_break_to_mnt_exp_round_up(
          grnt_bytes / SOC_PB_ITM_COMMITTED_BYTES_RESOLUTION,
          SOC_PB_ITM_COMMITTED_MANTISSA_NOF_BITS,
          SOC_PB_ITM_COMMITTED_EXPONENT_NOF_BITS,
          0,
          &(gr_bytes_mnt),
          &(gr_bytes_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Write them according to the rate class % 4
   */
  reg_ndx = rt_cls4_ndx / SOC_PB_REG_IQM_GRNT_BYTES_SET_REG_NOF_FLDS;
  fld_ndx = rt_cls4_ndx % SOC_PB_REG_IQM_GRNT_BYTES_SET_REG_NOF_FLDS;
  SOC_PB_FLD_SET(regs->iqm.grnt_bytes_set_reg[reg_ndx].grnt_bytes_mnt[fld_ndx], gr_bytes_mnt, 20, exit);
  SOC_PB_FLD_SET(regs->iqm.grnt_bytes_set_reg[reg_ndx].grnt_bytes_exp[fld_ndx], gr_bytes_exp, 30, exit);

  gt_bytes_exact = gr_bytes_mnt * (1 << gr_bytes_exp);
  gt_bytes_exact = gt_bytes_exact * SOC_PB_ITM_COMMITTED_BYTES_RESOLUTION;
  *exact_grnt_bytes = gt_bytes_exact;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_committed_q_size_set_unsafe()", 0, 0);
}

uint32
  soc_pb_itm_committed_q_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rt_cls4_ndx,
    SOC_SAND_IN  uint32                    grnt_bytes
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_COMMITTED_Q_SIZE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls4_ndx, SOC_PB_INGRESS_TRAFFIC_MGMT_RT_CLS4_NDX_MAX, SOC_PB_INGRESS_TRAFFIC_MGMT_RT_CLS4_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(grnt_bytes, SOC_PB_ITM_GRNT_BYTES_MAX, SOC_PB_ITM_GRNT_BYTES_OUT_OF_RANGE_ERR, 11, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_committed_q_size_set_verify()", rt_cls4_ndx, 0);
}

uint32
  soc_pb_itm_committed_q_size_get_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        rt_cls4_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_COMMITTED_Q_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls4_ndx, SOC_PB_INGRESS_TRAFFIC_MGMT_RT_CLS4_NDX_MAX, SOC_PB_INGRESS_TRAFFIC_MGMT_RT_CLS4_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_committed_q_size_get_verify()", rt_cls4_ndx, 0);
}

/*********************************************************************
*     Set the size of committed queue size (i.e., the
 *     guaranteed memory) for each VOQ, even in the case that a
 *     set of queues consume most of the memory resources.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_itm_committed_q_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rt_cls4_ndx,
    SOC_SAND_OUT uint32                    *grnt_bytes
  )
{
  uint32
    gt_bytes_exact,
    res = SOC_SAND_OK,
    gr_bytes_mnt = 0,
    gr_bytes_exp = 0;
  SOC_PETRA_REGS
    *regs;
  uint32
    fld_ndx,
    reg_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_COMMITTED_Q_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(grnt_bytes);

  regs = soc_petra_regs();

  /*
   *	Exact the exponent and the mantissa to get the exact value
   */
  reg_ndx = rt_cls4_ndx / SOC_PB_REG_IQM_GRNT_BYTES_SET_REG_NOF_FLDS;
  fld_ndx = rt_cls4_ndx % SOC_PB_REG_IQM_GRNT_BYTES_SET_REG_NOF_FLDS;
  SOC_PB_FLD_GET(regs->iqm.grnt_bytes_set_reg[reg_ndx].grnt_bytes_mnt[fld_ndx], gr_bytes_mnt, 10, exit);
  SOC_PB_FLD_GET(regs->iqm.grnt_bytes_set_reg[reg_ndx].grnt_bytes_exp[fld_ndx], gr_bytes_exp, 20, exit);

  gt_bytes_exact = gr_bytes_mnt * (1 << gr_bytes_exp);
  gt_bytes_exact = gt_bytes_exact * SOC_PB_ITM_COMMITTED_BYTES_RESOLUTION;
  *grnt_bytes = gt_bytes_exact;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_committed_q_size_get_unsafe()", 0, 0);
}

/* $Id: pb_ingress_traffic_mgmt.c,v 1.6 Broadcom SDK $
 *	Extension to the Soc_petra API
 */
uint32
  soc_pb_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_GLOB_RCS_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* Excess memory */

  for (index = 0 ; index < SOC_PETRA_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mem_excess[index].clear, SOC_PB_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX,
      SOC_PB_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mem_excess[index].set, SOC_PB_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX,
      SOC_PB_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_itm_glob_rcs_drop_verify()",0,0);
}

uint32
  soc_pb_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_THRESH_WITH_HYST_INFO mem_size[SOC_PETRA_NOF_DROP_PRECEDENCE],
    SOC_SAND_OUT SOC_PETRA_THRESH_WITH_HYST_INFO exact_mem_size[SOC_PETRA_NOF_DROP_PRECEDENCE]
  )
{
  uint32
    res,
    mnt_nof_bits,
    exp_nof_bits;
  uint32
    mnt_val,
    exp_val,
    indx = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_GLOB_RCS_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_size);
  SOC_SAND_CHECK_NULL_INPUT(exact_mem_size);

  regs = soc_petra_regs();

  /*
   *  Excess memory size
   */
  mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PB_REG_DB_ACC(regs->iqm.dram_dyn_size_th_cfg_reg[0].dram_dyn_size_rjct_set_th_mnt));
  exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PB_REG_DB_ACC(regs->iqm.dram_dyn_size_th_cfg_reg[0].dram_dyn_size_rjct_set_th_exp));

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              mem_size[indx].clear,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      exact_mem_size[indx].clear = (mnt_val) * (1<<(exp_val));

      SOC_PB_FLD_SET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_clr_th_mnt, (uint32)mnt_val, 20, exit);
      SOC_PB_FLD_SET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_clr_th_exp, (uint32)exp_val, 30, exit);

      res = soc_sand_break_to_mnt_exp_round_up(
              mem_size[indx].set,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      exact_mem_size[indx].set = (mnt_val) * (1<<(exp_val));

      SOC_PB_FLD_SET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_set_th_mnt, (uint32)mnt_val, 50, exit);
      SOC_PB_FLD_SET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_set_th_exp, (uint32)exp_val, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_itm_glob_rcs_drop_set_unsafe()", indx, 0);
}

uint32
  soc_pb_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PETRA_THRESH_WITH_HYST_INFO mem_size[SOC_PETRA_NOF_DROP_PRECEDENCE]
  )
{
  uint32
    res,
    mnt_val,
    exp_val;
  uint32
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_GLOB_RCS_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_size);

  regs = soc_petra_regs();

  /*
   * Excess memory
   */

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
    SOC_PB_FLD_GET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_clr_th_mnt, mnt_val, 10, exit);
    SOC_PB_FLD_GET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_clr_th_exp, exp_val, 20, exit);

    mem_size[indx].clear = (mnt_val) * (1<<(exp_val));

    SOC_PB_FLD_GET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_set_th_mnt, mnt_val, 30, exit);
    SOC_PB_FLD_GET(regs->iqm.dram_dyn_size_th_cfg_reg[indx].dram_dyn_size_rjct_set_th_exp, exp_val, 40, exit);

    mem_size[indx].set = (mnt_val) * (1<<(exp_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_itm_glob_rcs_drop_get_unsafe()",0,0);

}

uint32
  SOC_PB_ITM_STAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_ITM_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->enable_mode, SOC_PB_ITM_STAG_INFO_ENABLE_MODE_MAX, SOC_PB_ITM_ENABLE_MODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->offset_4bits, SOC_PB_ITM_STAG_INFO_OFFSETB__MAX, SOC_PB_ITM_OFFSET_4B_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_ITM_STAG_INFO_verify()",0,0);
}


uint32
  soc_pb_itm_dp_discard_set_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        discard_dp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_DISCARD_DP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(discard_dp, SOC_PB_ITM_DISCARD_DP_MAX, SOC_PB_ITM_DP_DISCARD_OUT_OF_RANGE_ERR, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_dp_discard_set_verify()", discard_dp, 0);
}

/*********************************************************************
*     Set the drop precedence value above which 
*     all packets will always be discarded.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_itm_dp_discard_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  )
{     
  uint32 res;
  SOC_PETRA_REGS *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_DISCARD_DP_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PB_FLD_SET(regs->iqm.iqm_enablers_reg.dscrd_dp, discard_dp, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_dp_discard_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the drop precedence value above which 
*     all packets will always be discarded.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_itm_dp_discard_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  )
{     
  uint32 res, fld_val;
  SOC_PETRA_REGS *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ITM_DISCARD_DP_GET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->iqm.iqm_enablers_reg.dscrd_dp, fld_val, 15, exit);
  *discard_dp = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_itm_dp_discard_get_unsafe()", 0, 0);
}
#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

