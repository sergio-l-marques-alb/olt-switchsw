/* $Id: pcp_oam_general.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PCP/pcp_api_tbl_access.h>

#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_reg_access.h>

#include <soc/dpp/PCP/pcp_oam_api_eth.h>
#include <soc/dpp/PCP/pcp_oam_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_OAM_GENERAL_EXCEPTION_MAX                          (PCP_OAM_NOF_MSG_EXCEPTION_TYPES-1)
#define PCP_OAM_GENERAL_INFO1_MAX                              (SOC_SAND_U32_MAX)
#define PCP_OAM_GENERAL_INFO2_MAX                              (SOC_SAND_U32_MAX)
#define PCP_OAM_GENERAL_CPU_DST_SYS_PORT_MAX                   (8192 - 1)
#define PCP_OAM_GENERAL_CPU_TC_MAX                             (7)
#define PCP_OAM_GENERAL_CPU_DP_MAX                             (3)

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_oam_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_CALLBACK_FUNCTION_REGISTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_CALLBACK_FUNCTION_REGISTER_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_CALLBACK_FUNCTION_REGISTER_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_CALLBACK_FUNCTION_REGISTER_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_INTERRUPT_HANDLER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_INTERRUPT_HANDLER_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_INTERRUPT_HANDLER_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_INTERRUPT_HANDLER_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MSG_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MSG_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MSG_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_MSG_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_OAM_GENERAL_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_oam_general[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_OAM_GENERAL_EXCEPTION_OUT_OF_RANGE_ERR,
    "PCP_OAM_GENERAL_EXCEPTION_OUT_OF_RANGE_ERR",
    "The parameter 'exception' is out of range. \n\r "
    "The range is: 0 - PCP_OAM_NOF_MSG_EXCEPTION_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_GENERAL_INFO1_OUT_OF_RANGE_ERR,
    "PCP_OAM_GENERAL_INFO1_OUT_OF_RANGE_ERR",
    "The parameter 'info1' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_GENERAL_INFO2_OUT_OF_RANGE_ERR,
    "PCP_OAM_GENERAL_INFO2_OUT_OF_RANGE_ERR",
    "The parameter 'info2' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_GENERAL_CPU_DST_SYS_PORT_OUT_OF_RANGE_ERR,
    "PCP_OAM_GENERAL_CPU_DST_SYS_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_dst_sys_port' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_GENERAL_CPU_TC_OUT_OF_RANGE_ERR,
    "PCP_OAM_GENERAL_CPU_TC_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_tc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_OAM_GENERAL_CPU_DP_OUT_OF_RANGE_ERR,
    "PCP_OAM_GENERAL_CPU_DP_OUT_OF_RANGE_ERR",
    "The parameter 'cpu_dp' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_UINT_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    PCP_OAM_GENERAL_MSG_FIFO_EMPTY_ERR,
    "PCP_OAM_GENERAL_MSG_FIFO_EMPTY_ERR",
    "Calling msg info get while fifo is empty. \n\r "
    "'1'- Message fifo not empty - a message is wating for the CPU. '0'- Message fifo is empty.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
 {
    PCP_OAM_GENERAL_MSG_INFO_UNKNOWN_EXCEPTION_CODE_ERR,
    "PCP_OAM_GENERAL_MSG_INFO_UNKNOWN_EXCEPTION_CODE_ERR",
    "Exception code unknown \n\r "
    "No handling to Exception code recived from HW.\n\r ",
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

/*********************************************************************
*     This procedure applies general configuration of the
 *     OAMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_general_info_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_GENERAL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = pcp_regs();

  PCP_FLD_SET(regs->oam.cpuport_reg.cpu_sys_port , info->cpu_dst_sys_port, 10, exit);
  PCP_FLD_SET(regs->oam.cpuport_reg.cpu_port_tc  , info->cpu_tc, 20, exit);
  PCP_FLD_SET(regs->oam.cpuport_reg.cpu_port_dp  , info->cpu_dp, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_general_info_set_unsafe()", 0, 0);
}

uint32
  pcp_oam_general_info_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_GENERAL_INFO_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_OAM_GENERAL_INFO, info, 10, exit);

  /* IMPLEMENTED */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_general_info_set_verify()", 0, 0);
}

uint32
  pcp_oam_general_info_get_verify(
    SOC_SAND_IN  int                       unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_GENERAL_INFO_GET_VERIFY);

  /* IMPLEMENTED */

  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_general_info_get_verify()", 0, 0);
}

/*********************************************************************
*     This procedure applies general configuration of the
 *     OAMP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_general_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_GENERAL_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg;
  PCP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_GENERAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_GENERAL_INFO_clear(info);

  regs = pcp_regs();

  PCP_FLD_GET(regs->oam.cpuport_reg.cpu_sys_port , reg, 10, exit);
  info->cpu_dst_sys_port = (uint32)reg;
  PCP_FLD_GET(regs->oam.cpuport_reg.cpu_port_tc  , reg, 20, exit);
  info->cpu_tc = (uint32)reg;
  PCP_FLD_GET(regs->oam.cpuport_reg.cpu_port_dp  , reg, 30, exit);
  info->cpu_dp = (uint32)reg;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_general_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This procedure registers user callback that will be
 *     called as a response to a certain interrupt.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_callback_function_register_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct,
    SOC_SAND_OUT uint32                        *callback_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_CALLBACK_FUNCTION_REGISTER_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(user_callback_struct);
  SOC_SAND_CHECK_NULL_INPUT(callback_id);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_callback_function_register_unsafe()", 0, 0);
}

uint32
  pcp_oam_callback_function_register_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK              *user_callback_struct
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_CALLBACK_FUNCTION_REGISTER_VERIFY);

  /* PCP_STRUCT_VERIFY(SOC_SAND_USER_CALLBACK, user_callback_struct, 10, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_callback_function_register_verify()", 0, 0);
}

/*********************************************************************
*     This procedure is called from the CPU ISR when interrupt
 *     handling for OAMP is required.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_interrupt_handler_unsafe(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_INTERRUPT_HANDLER_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_interrupt_handler_unsafe()", 0, 0);
}

uint32
  pcp_oam_interrupt_handler_verify(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_INTERRUPT_HANDLER_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_interrupt_handler_verify()", 0, 0);
}

/*********************************************************************
*     This function reads and parses the head of the OAM
 *     message FIFO. If no message exists an indication is
 *     returned.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_oam_msg_info_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_OAM_MSG_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK,
    fifo_not_empty_reg,
    interrupt_msg,
    int_msg_rmep_pointer,
    int_msg_exception_code;
  PCP_REGS
    *regs;
  PCP_OAM_RMEP_DB_TBL_DATA
    rmep_db_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MSG_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_OAM_MSG_INFO_clear(info);

  regs = pcp_regs();

  PCP_FLD_GET(regs->oam.interrupt_reg.msg_fifo_not_empty, fifo_not_empty_reg, 10, exit);

  if (fifo_not_empty_reg == 0)
  {
    info->exception = PCP_OAM_MSG_NONE;
    info->info1     = 0x0;
    info->info2     = 0x0;
  }
  else
  {

    PCP_FLD_GET(regs->oam.interrupt_message_reg.interrupt_message, interrupt_msg, 20, exit);
    int_msg_rmep_pointer   = SOC_SAND_GET_BITS_RANGE(interrupt_msg,15,0);
    int_msg_exception_code = SOC_SAND_GET_BITS_RANGE(interrupt_msg,23,16);

    info->exception = int_msg_exception_code;

    /* handling the exception code */
    if (int_msg_exception_code == PCP_OAM_MSG_CCM_TIMEOUT)
    {
      info->info1     = int_msg_rmep_pointer;
      info->info2     = 0x0;

      /* Set ccm_defect bit - Disable ccm timeout interrupts from PCP */
      res = pcp_oam_rmep_db_tbl_get(
              unit,
              int_msg_rmep_pointer,
              &rmep_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      rmep_db_tbl_data.ccm_defect = SOC_SAND_NUM2BOOL(TRUE);

      res = pcp_oam_rmep_db_tbl_set(
              unit,
              int_msg_rmep_pointer,
              &rmep_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    }
    else if (int_msg_exception_code == PCP_OAM_MSG_CCM_EXIT_TIMEOUT)
    {
      info->info1     = int_msg_rmep_pointer;
      info->info2     = 0x0;
          
      /* Clear ccm_defect bit - Enable ccm timeout interrupts from PCP */
      res = pcp_oam_rmep_db_tbl_get(
              unit,
              int_msg_rmep_pointer,
              &rmep_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      rmep_db_tbl_data.ccm_defect = SOC_SAND_NUM2BOOL(FALSE);

      res = pcp_oam_rmep_db_tbl_set(
              unit,
              int_msg_rmep_pointer,
              &rmep_db_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(PCP_OAM_GENERAL_MSG_INFO_UNKNOWN_EXCEPTION_CODE_ERR, 70, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_msg_info_get_unsafe()", 0, 0);
}

uint32
  pcp_oam_msg_info_get_verify(
    SOC_SAND_IN  int                       unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_OAM_MSG_INFO_GET_VERIFY);

  /* IMPLEMENTED */

  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_oam_msg_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_oam_api_general module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_oam_general_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_oam_general;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_oam_api_general module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_oam_general_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_oam_general;
}
uint32
  PCP_OAM_MSG_INFO_verify(
    SOC_SAND_IN  PCP_OAM_MSG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->exception, PCP_OAM_GENERAL_EXCEPTION_MAX, PCP_OAM_GENERAL_EXCEPTION_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->info1, PCP_OAM_GENERAL_INFO1_MAX, PCP_OAM_GENERAL_INFO1_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->info2, PCP_OAM_GENERAL_INFO2_MAX, PCP_OAM_GENERAL_INFO2_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_MSG_INFO_verify()",0,0);
}

uint32
  PCP_OAM_GENERAL_INFO_verify(
    SOC_SAND_IN  PCP_OAM_GENERAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_dst_sys_port, PCP_OAM_GENERAL_CPU_DST_SYS_PORT_MAX, PCP_OAM_GENERAL_CPU_DST_SYS_PORT_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_tc, PCP_OAM_GENERAL_CPU_TC_MAX, PCP_OAM_GENERAL_CPU_TC_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cpu_dp, PCP_OAM_GENERAL_CPU_DP_MAX, PCP_OAM_GENERAL_CPU_DP_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_OAM_GENERAL_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

