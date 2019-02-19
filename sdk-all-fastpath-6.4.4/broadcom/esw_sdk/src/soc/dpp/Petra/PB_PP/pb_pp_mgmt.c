/* $Id: pb_pp_mgmt.c,v 1.9 Broadcom SDK $
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

#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>

#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>

#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_MGMT_ELK_MODE_MAX                                (SOC_PB_PP_NOF_MGMT_ELK_MODES-1)
#define SOC_PB_PP_MGMT_LKP_TYPE_MAX                                (SOC_PB_PP_NOF_MGMT_LKP_TYPES-1)
#define SOC_PB_PP_MGMT_USE_ELK_MAX                                 (SOC_PB_PP_NOF_MGMT_LKP_TYPES-1)

#define SOC_PB_PP_MGMT_ELK_MODE_A1_BURST_SIZE                        (6)
#define SOC_PB_PP_MGMT_ELK_MODE_B0_BURST_SIZE                       (12)

#define SOC_PB_PP_MGMT_ELK_MODE_A1_RECORD_NUMBER                    (4)
#define SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT_RECORD_NUMBER          (8)
#define SOC_PB_PP_MGMT_ELK_MODE_B0_LONG_RECORD_NUMBER           (4)

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

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_DEVICE_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_DEVICE_CLOSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_OPERATION_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_OPERATION_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_OPERATION_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_OPERATION_MODE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_OPERATION_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_ELK_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_USE_ELK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MGMT_PROC_ERR_MECH_INIT),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'elk_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MGMT_ELK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MGMT_INGRESS_PKT_RATE_OUT_OF_RANGE_ERR",
    "The parameter 'ingress_pkt_rate' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MGMT_ELK_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'lkp_type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MGMT_LKP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR",
    "The parameter 'use_elk' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MGMT_LKP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_MGMT_ELK_MODE_CHIP_TYPE_MISS_MATCH_ERR,
    "SOC_PB_PP_MGMT_ELK_MODE_MISS_MATCH_ERR",
    "Trying to config Elk Mode on wrong Soc_petra-B chip type\n\r "
    "BO mode can not be configured on A0/A1 Soc_petra-B chips.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MGMT_ELK_MODE_DISABLED_ERR,
    "SOC_PB_PP_MGMT_ELK_MODE_DISABLED_ERR",
    "Trying to config Elk Mode but Elk is disabeled in HW_ADJUSTMENT\n\r "
    "Elk mode need to be enabled on HW_ADJUSTMENT.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  soc_pb_pp_mgmt_proc_err_mech_init(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_PROC_ERR_MECH_INIT);

  /* Add list of SOC_PB_PP errors to all-system errors pool                                                  */
  res = soc_pb_pp_errors_desc_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  /* Now add list of SOC_PB_PP procedure descriptors to all-system pool.                                     */
  res = soc_pb_pp_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_proc_err_mech_init()",0,0);
}


STATIC uint32
  soc_pb_pp_mgmt_device_init(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_DEVICE_INIT);

  res = soc_pb_pp_sw_db_device_init(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_device_init()",0,0);
}

uint32
  soc_pb_pp_mgmt_device_close_unsafe(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_DEVICE_CLOSE);

  res = soc_pb_pp_sw_db_device_close(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_device_close_unsafe()",0,0);
}

/*********************************************************************
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_OPERATION_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  /*
   *    Init the error mechanism
   */
  res = soc_pb_pp_mgmt_proc_err_mech_init();
  SOC_SAND_CHECK_FUNC_RESULT(res,10, exit);

  /*
   *    Init the SW DB
   */
  res = soc_pb_pp_sw_db_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * Init the SW DB and Save the operation mode in
  */
  res = soc_pb_pp_mgmt_device_init(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_operation_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_OPERATION_MODE      *op_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_OPERATION_MODE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  SOC_SAND_MAGIC_NUM_VERIFY(op_mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_operation_mode_verify()",0,0);
}

/*********************************************************************
*     Set soc_petra_pp device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT SOC_PB_PP_MGMT_OPERATION_MODE     *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_OPERATION_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  res = soc_pb_pp_sw_db_oper_mode_get(unit,op_mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_operation_mode_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the ELK interface mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_elk_mode_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
    uint32
        fld_val,
        reg_val,
        serdes_rate,
        rate_in_gbps,
        burst_size,
        record_number = 0x0,
        core_freq_in_mbps,
        shaper_value,
        spr_value,
        frc_value,
        res = SOC_SAND_OK;
    SOC_PB_PP_REGS
        *pp_regs;
    SOC_PB_NIF_ELK_INFO                
        nif_elk_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ELK_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);

  pp_regs = soc_pb_pp_regs();

  if (elk_mode == SOC_PB_PP_MGMT_ELK_MODE_NONE) {
      fld_val = 0x0;
  } else {
      fld_val = 0x1;
  }
  SOC_PB_PP_FLD_SET(pp_regs->ihb.lookup_control_reg.enable_elk_lookup,    fld_val, 10, exit);

  /* configure ELK learn lookup according to ELK mode*/
  SOC_PB_PP_FLD_SET(pp_regs->ihp.large_em_learn_lookup_general_configuration_reg.large_em_learn_lookup_elk , fld_val, 35, exit);

  switch(elk_mode) {
  
  case SOC_PB_PP_MGMT_ELK_MODE_NONE:
  case SOC_PB_PP_MGMT_ELK_MODE_NORMAL:
       if (SOC_PB_REV_ABOVE_A1 == TRUE) {
          fld_val = 0x0;
          SOC_PB_PP_FLD_SET(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_mode, fld_val, 20, exit);

          fld_val = 0x1;
          SOC_PB_PP_FLD_SET(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_nif_credits, fld_val, 30, exit);
      }

       if (elk_mode == SOC_PB_PP_MGMT_ELK_MODE_NONE) {
           burst_size = 0x0;
           record_number = 0x0;
       } else {
           burst_size = SOC_PB_PP_MGMT_ELK_MODE_A1_BURST_SIZE;
           record_number = SOC_PB_PP_MGMT_ELK_MODE_A1_RECORD_NUMBER;
       }
       break;
    
  case SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT:
  case SOC_PB_PP_MGMT_ELK_MODE_B0_LONG:
  case SOC_PB_PP_MGMT_ELK_MODE_B0_BOTH:
       if (SOC_PB_REV_A1_OR_BELOW == TRUE) {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MGMT_ELK_MODE_CHIP_TYPE_MISS_MATCH_ERR, 40, exit);
      }

       /* Disable soc_petra c mode before configure */
       reg_val = 0x0;
       SOC_PB_PP_REG_SET(pp_regs->ihb.soc_petra_c_reg, reg_val, 50, exit);

       fld_val = burst_size = SOC_PB_PP_MGMT_ELK_MODE_B0_BURST_SIZE;
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_burst_size,            fld_val, reg_val, 60, exit);

       fld_val = 0x1;
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_mode,                     fld_val, reg_val, 70, exit);
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_lkp_record_size,     fld_val, reg_val, 80, exit);
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_nop_record_size,  fld_val, reg_val, 90, exit);
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_lkp_packing,           fld_val, reg_val, 100, exit);
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_nif_credits,              fld_val, reg_val, 110, exit);
       SOC_PB_PP_FLD_TO_REG(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_lrn_record_size, fld_val, reg_val, 120, exit);

       if (elk_mode == SOC_PB_PP_MGMT_ELK_MODE_B0_LONG) {
           fld_val = 0x0;
           record_number = SOC_PB_PP_MGMT_ELK_MODE_B0_LONG_RECORD_NUMBER;
       } else {
           fld_val = 0xffff;
           record_number = SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT_RECORD_NUMBER;
       }
       SOC_PB_PP_FLD_SET(pp_regs->ihb.soc_petra_c_elk_db_id_reg.soc_petra_c_elk_db_id, fld_val, 120, exit);

       /* Set soc_petra c mode register */
       SOC_PB_PP_REG_SET(pp_regs->ihb.soc_petra_c_reg, reg_val, 130, exit);
       break;
  default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR, 140, exit);
  }

  res = soc_pb_nif_elk_get(
                unit,
                &nif_elk_info
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  if (nif_elk_info.enable == FALSE) {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MGMT_ELK_MODE_DISABLED_ERR, 160, exit);
  }

  serdes_rate = soc_petra_srd_rate_calc_kbps(
                unit,
                SOC_PB_NIF_NOF_MALGS * nif_elk_info.mal_id
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  rate_in_gbps = ((serdes_rate * 4 * 8) / 10) / 1000000;

  core_freq_in_mbps = soc_petra_chip_mega_ticks_per_sec_get(unit);

  if (rate_in_gbps != 0) {
      shaper_value = ((burst_size * 64 * core_freq_in_mbps) / (1000 * rate_in_gbps) );
  } else {
      shaper_value = 0x0;
  }

  spr_value = (shaper_value * 256) / 256;
  SOC_PB_PP_FLD_SET(pp_regs->ihb.spr_dly_reg.spr_dly, spr_value, 180, exit);

  frc_value = (shaper_value *256) % 256;
  SOC_PB_PP_FLD_SET(pp_regs->ihb.spr_dly_fraction_reg.spr_dly_fraction, frc_value, 190, exit);

  if (burst_size != 0x0) {
      *ingress_pkt_rate = (((serdes_rate * 4 * 1000 * 8) / 10 ) * record_number) / ((burst_size * 8) * 8);
  } else {
      *ingress_pkt_rate = 0x0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_elk_mode_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mgmt_elk_mode_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE elk_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ELK_MODE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(elk_mode, SOC_PB_PP_MGMT_ELK_MODE_MAX, SOC_PB_PP_MGMT_ELK_MODE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_elk_mode_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mgmt_elk_mode_get_verify(
    SOC_SAND_IN  int           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ELK_MODE_GET_VERIFY);


  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_elk_mode_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the ELK interface mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_elk_mode_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_OUT SOC_PB_PP_MGMT_ELK_MODE *elk_mode,
    SOC_SAND_OUT uint32           *ingress_pkt_rate
  )
{
    uint32
        enable_elk_lookup_fld,
        soc_petra_c_mode_fld, 
        db_id_fld,
        burst_size = 0,
        serdes_rate,
        record_number = 0x0,
        res = SOC_SAND_OK;
    SOC_PB_PP_REGS
        *pp_regs;
    SOC_PB_NIF_ELK_INFO                
        nif_elk_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ELK_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(elk_mode);
  SOC_SAND_CHECK_NULL_INPUT(ingress_pkt_rate);

  pp_regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(pp_regs->ihb.lookup_control_reg.enable_elk_lookup,         enable_elk_lookup_fld, 10, exit);
  SOC_PB_PP_FLD_GET(pp_regs->ihb.soc_petra_c_reg.soc_petra_c_mode,                           soc_petra_c_mode_fld,  20, exit);
  SOC_PB_PP_FLD_GET(pp_regs->ihb.soc_petra_c_elk_db_id_reg.soc_petra_c_elk_db_id,    db_id_fld, 30, exit);

  if (enable_elk_lookup_fld == 0x0) {
      *elk_mode             = SOC_PB_PP_MGMT_ELK_MODE_NONE;
      *ingress_pkt_rate   = 0X0;
  } else if ((SOC_PB_REV_A1_OR_BELOW == TRUE) || (soc_petra_c_mode_fld == 0x0)) {
      *elk_mode         = SOC_PB_PP_MGMT_ELK_MODE_NORMAL;
      burst_size            = SOC_PB_PP_MGMT_ELK_MODE_A1_BURST_SIZE;
      record_number  = SOC_PB_PP_MGMT_ELK_MODE_A1_RECORD_NUMBER;
  } else if (db_id_fld != 0x0) {
      *elk_mode         = SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT;
       burst_size           = SOC_PB_PP_MGMT_ELK_MODE_B0_BURST_SIZE;
       record_number = SOC_PB_PP_MGMT_ELK_MODE_B0_SHORT_RECORD_NUMBER;
  } else {
      *elk_mode         = SOC_PB_PP_MGMT_ELK_MODE_B0_LONG;
       burst_size           = SOC_PB_PP_MGMT_ELK_MODE_B0_BURST_SIZE;
       record_number = SOC_PB_PP_MGMT_ELK_MODE_B0_LONG_RECORD_NUMBER;
  }

  res = soc_pb_nif_elk_get(
                unit,
                &nif_elk_info
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (nif_elk_info.enable == FALSE) {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MGMT_ELK_MODE_DISABLED_ERR, 50, exit);
  }

  serdes_rate = soc_petra_srd_rate_calc_kbps(
                unit,
                SOC_PB_NIF_NOF_MALGS * nif_elk_info.mal_id
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  if (burst_size != 0x0) {
      *ingress_pkt_rate = (((serdes_rate * 4 * 1000 * 8) / 10) * record_number) / ((burst_size * 8) * 8);
  } else {
      *ingress_pkt_rate = 0x0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_elk_mode_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Determine whether the specified lookup is externalized
 *     or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_use_elk_set_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
      reg_val,
      res = SOC_SAND_OK;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;
  SOC_PB_PP_REGS
        *pp_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_USE_ELK_SET_UNSAFE);

  pp_regs = soc_pb_pp_regs();

  switch(lkp_type) {
  case SOC_PB_PP_MGMT_LKP_TYPE_ETH:
      /* configure DSP Engine - soc_petra mact learning according to use_elk */
      reg_val = SOC_SAND_BOOL2NUM_INVERSE(use_elk);
      SOC_PB_PP_REG_SET(pp_regs->olp.dsp_engine_configuration_reg[0].dsp_generation_en , reg_val, 5, exit);
      SOC_PB_PP_REG_SET(pp_regs->olp.dsp_engine_configuration_reg[1].dsp_generation_en , reg_val, 6, exit);

      res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
              unit,
              0x1,
              &ihb_flp_key_program_map_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      ihb_flp_key_program_map_tbl_data.elk_lkp_valid         = SOC_SAND_BOOL2NUM(use_elk);
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_valid = SOC_SAND_BOOL2NUM_INVERSE(use_elk);
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_valid = SOC_SAND_BOOL2NUM_INVERSE(use_elk);

       res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
              unit,
              0x1,
              &ihb_flp_key_program_map_tbl_data
            );
       SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

       res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
              unit,
              0x2,
              &ihb_flp_key_program_map_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      ihb_flp_key_program_map_tbl_data.elk_lkp_valid         = SOC_SAND_BOOL2NUM(use_elk);
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_valid = SOC_SAND_BOOL2NUM_INVERSE(use_elk);
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_valid = SOC_SAND_BOOL2NUM_INVERSE(use_elk);

       res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
              unit,
              0x2,
              &ihb_flp_key_program_map_tbl_data
            );
       SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      break;
  case SOC_PB_PP_MGMT_LKP_TYPE_LSR:
      res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
              unit,
              0xd,
              &ihb_flp_key_program_map_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      ihb_flp_key_program_map_tbl_data.elk_lkp_valid         = SOC_SAND_BOOL2NUM(use_elk);
      ihb_flp_key_program_map_tbl_data.lem_1st_lkp_valid = SOC_SAND_BOOL2NUM_INVERSE(use_elk);
      ihb_flp_key_program_map_tbl_data.lem_2nd_lkp_valid = SOC_SAND_BOOL2NUM_INVERSE(use_elk);

       res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
              unit,
              0xd,
              &ihb_flp_key_program_map_tbl_data
            );
       SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      break;
  default:
      break;
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_use_elk_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mgmt_use_elk_set_verify(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_IN  uint8           use_elk
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_USE_ELK_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lkp_type, SOC_PB_PP_MGMT_LKP_TYPE_MAX, SOC_PB_PP_MGMT_LKP_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(use_elk, SOC_PB_PP_MGMT_USE_ELK_MAX, SOC_PB_PP_MGMT_USE_ELK_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_use_elk_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mgmt_use_elk_get_verify(
    SOC_SAND_IN  int           unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_USE_ELK_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_use_elk_get_verify()", 0, 0);
}

/*********************************************************************
*     Determine whether the specified lookup is externalized
 *     or not.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mgmt_use_elk_get_unsafe(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PB_PP_MGMT_LKP_TYPE lkp_type,
    SOC_SAND_OUT uint8           *use_elk
  )
{
  uint32
    prog_line_number = 0x0,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_USE_ELK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lkp_type);
  SOC_SAND_CHECK_NULL_INPUT(use_elk);

  switch(lkp_type) {
  case SOC_PB_PP_MGMT_LKP_TYPE_ETH:
        prog_line_number = 0x1;
      break;
  case SOC_PB_PP_MGMT_LKP_TYPE_LSR:
      prog_line_number = 0xd;
      break;
  default:
      *use_elk = 0x0;
      SOC_PB_PP_DO_NOTHING_AND_EXIT;
      break;
  }

   res = soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
                unit,
                prog_line_number,
                &ihb_flp_key_program_map_tbl_data
            );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   *use_elk = SOC_SAND_NUM2BOOL(ihb_flp_key_program_map_tbl_data.elk_lkp_valid);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mgmt_use_elk_get_unsafe()", 0, 0);
}

/* $Id: pb_pp_mgmt.c,v 1.9 Broadcom SDK $
 * Print Functions 
 */

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_mgmt_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_mgmt;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_mgmt_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_mgmt;
}

#if SOC_PB_PP_DEBUG_IS_LVL1
void
  SOC_PB_PP_MGMT_ELK_MODE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MGMT_ELK_MODE info
  )
{

  soc_sand_os_printf("Elk Mode:\n\r");
  soc_sand_os_printf(
    "Type %s, ",
    SOC_PB_PP_MGMT_ELK_MODE_to_string(info)
  );

  return;
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

