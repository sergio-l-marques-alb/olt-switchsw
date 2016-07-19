/* $Id: pb_pp_frwrd_ilm.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ilm.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


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

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_frwrd_ilm[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_ILM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_frwrd_ilm[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    SOC_PB_PP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR,
    "SOC_PB_PP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR",
    "If by the global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is masked, \n\r"
    "then the inport field of ILM key has to be zero \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR,
    "SOC_PB_PP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR",
    "If by the global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is masked, \n\r"
    "then the inrif field of ILM key has to be zero \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_ILM_EEI_NOT_MPLS_ERR,
    "SOC_PB_PP_FRWRD_ILM_EEI_NOT_MPLS_ERR",
    "EEI in ILM add command must be of type MPLS. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR,
    "SOC_PB_PP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR",
    "Mapped_exp ILM key field should be zero for labels not in the ELSP range . \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR,
    "SOC_PB_PP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR",
    "In ILM key, Inrif and Port are always masked. This setting can not be changed in this SW release. \n\r ",
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

uint32
  soc_pb_pp_frwrd_ilm_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  SOC_PB_PP_FRWRD_ILM_GLBL_INFO
    glbl_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);

 /*
  * default set lookup to be according to label only.
  */
  glbl_info.key_info.mask_inrif = TRUE;
  glbl_info.key_info.mask_port = TRUE;

  res = soc_pb_pp_frwrd_ilm_glbl_info_set_unsafe(
          unit,
          &glbl_info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_init_unsafe()", 0, 0);
}

STATIC
  uint32
    soc_pb_pp_frwrd_ilm_key_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_FRWRD_ILM_GLBL_INFO
    glbl_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_ILM_KEY, ilm_key, 10, exit);
  
  res = soc_pb_pp_frwrd_ilm_glbl_info_get(
          unit,
          &glbl_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if((glbl_info.key_info.mask_port) && (ilm_key->in_local_port != 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR, 40, exit);
  }
  if((glbl_info.key_info.mask_inrif) && (ilm_key->inrif != 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR, 50, exit);
  }
  
  if((ilm_key->in_label < glbl_info.elsp_info.labels_range.start) ||
     (ilm_key->in_label > glbl_info.elsp_info.labels_range.end))
  {
    if(ilm_key->mapped_exp != 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_key_verify()", 0, 0);
}

/* build lem access key for ILM host address */
void
    soc_pb_pp_frwrd_ilm_lem_key_build(
      SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY  *ilm_key,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  SOC_PB_PP_LEM_ACCESS_KEY_clear(key);
  
  key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM;
  key->type = SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM;

  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = ilm_key->in_label;
  
  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = ilm_key->mapped_exp;

  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
  key->param[2].nof_bits = (uint8)num_bits;
  key->param[2].value[0] = ilm_key->in_local_port;

  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
  key->param[3].nof_bits = (uint8)num_bits;
  key->param[3].value[0] = ilm_key->inrif;

  key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;
  key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_ILM;
}

/* parse lem access key for IpV4 host address
*/
  void
    soc_pb_pp_frwrd_ilm_lem_key_parse(
      SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key
    )
{
  ilm_key->in_label = key->param[0].value[0];
  ilm_key->mapped_exp = (SOC_SAND_PP_MPLS_EXP)key->param[1].value[0];
  ilm_key->in_local_port = key->param[2].value[0];
  ilm_key->inrif = key->param[3].value[0];
}

/* build lem access payload for IpV4 host address */
STATIC
  uint32
    soc_pb_pp_frwrd_ilm_lem_payload_build(
      SOC_SAND_IN SOC_PB_PP_FRWRD_DECISION_INFO                 *ilm_val,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD *payload
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  /*
  *	Get the encoded destination and ASD
  */
  res = soc_pb_pp_fwd_decision_in_buffer_build(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
          ilm_val,
          FALSE,
          &payload->dest,
          &payload->asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  payload->age = 0;
  payload->is_dynamic = FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_lem_request_payload_build()", 0, 0);
}

/* parse lem access payload for IpV4 host address
*/
  void
    soc_pb_pp_frwrd_ilm_lem_payload_parse(
      SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_PAYLOAD *payload,
      SOC_SAND_OUT  SOC_PB_PP_FRWRD_DECISION_INFO                 *ilm_val
    )
{
  uint8 tmp;

  soc_pb_pp_fwd_decision_in_buffer_parse(
    SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
    payload->dest,
    payload->asd,
    ilm_val,
    &tmp
  );
}
/*********************************************************************
*     Setting global information of the ILM (ingress label
 *     mapping) (including ELSP and key building information)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val = 0,
    fld_ndx;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  regs = soc_pb_pp_regs();

  fld_val = glbl_info->elsp_info.labels_range.start;
  SOC_PB_PP_FLD_SET(regs->ihb.lsr_elsp_range_min_reg.elsp_range_min, fld_val, 10, exit);

  fld_val = glbl_info->elsp_info.labels_range.end;
  SOC_PB_PP_FLD_SET(regs->ihb.lsr_elsp_range_max_reg.elsp_range_max, fld_val, 20, exit);

  for(fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_MPLS_EXP_REG_NOF_FLDS; ++fld_ndx)
  {
    fld_val = glbl_info->elsp_info.exp_map_tbl[fld_ndx];
    SOC_PB_PP_FLD_SET(regs->ihb.mpls_exp_map_reg.mpls_exp[fld_ndx], fld_val, 30 + fld_ndx, exit);
  }

  res = soc_pb_pp_sw_db_ilm_key_mask_inrif_set(unit, glbl_info->key_info.mask_inrif);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_ilm_key_mask_port_set(unit, glbl_info->key_info.mask_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* this requires to change FLP program that builds ILM key*/
  
  res = soc_pb_pp_lem_ilm_key_build_set(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  SOC_SAND_TODO_IMPLEMENT_WARNING;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    mask_inrif,
    mask_port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_ILM_GLBL_INFO, glbl_info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  
  /* changing mask_inrif and mask_port requires changes in FLP program.
   * currently unsupported in SW
   */
  res = soc_pb_pp_sw_db_ilm_key_mask_inrif_get(unit, &mask_inrif);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_ilm_key_mask_port_get(unit, &mask_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

/* $Id: pb_pp_frwrd_ilm.c,v 1.7 Broadcom SDK $
  if((glbl_info->key_info.mask_inrif != mask_inrif) ||
     (glbl_info->key_info.mask_port != mask_port))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR, 40, exit);
  }*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_glbl_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the ILM (ingress label
 *     mapping) (including ELSP and key building information)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_ILM_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val = 0,
    fld_ndx;

  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PB_PP_FRWRD_ILM_GLBL_INFO_clear(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->ihb.lsr_elsp_range_min_reg.elsp_range_min, fld_val, 10, exit);
  glbl_info->elsp_info.labels_range.start = fld_val;

  SOC_PB_PP_FLD_GET(regs->ihb.lsr_elsp_range_max_reg.elsp_range_max, fld_val, 20, exit);
  glbl_info->elsp_info.labels_range.end = fld_val;;

  for(fld_ndx = 0; fld_ndx < SOC_PB_PP_IHP_MPLS_EXP_REG_NOF_FLDS; ++fld_ndx)
  {
    SOC_PB_PP_FLD_GET(regs->ihb.mpls_exp_map_reg.mpls_exp[fld_ndx], fld_val, 30 + fld_ndx, exit);
    glbl_info->elsp_info.exp_map_tbl[fld_ndx] = (SOC_SAND_PP_MPLS_EXP)fld_val;;
  }

  res = soc_pb_pp_sw_db_ilm_key_mask_inrif_get(unit, &glbl_info->key_info.mask_inrif);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_sw_db_ilm_key_mask_port_get(unit, &glbl_info->key_info.mask_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add mapping from incoming label to destination and MPLS
 *     command.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);
  SOC_SAND_CHECK_NULL_INPUT(ilm_val);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;

  soc_pb_pp_frwrd_ilm_lem_key_build(
      ilm_key,
      &request.key
    );

  soc_pb_pp_frwrd_ilm_lem_payload_build(
    ilm_val,
    &payload
  );

  res = soc_pb_pp_lem_access_entry_add_unsafe(
         unit,
         &request,
         &payload,
         &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(ack.is_success)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO                 *ilm_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_DECISION_INFO, ilm_val, 20, exit);

  res = soc_pb_pp_frwrd_ilm_key_verify(
          unit,
          ilm_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(ilm_val->additional_info.eei.type != SOC_PB_PP_EEI_TYPE_MPLS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_ILM_EEI_NOT_MPLS_ERR, 60, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the value (destination and MPLS command) the
 *     incoming label key is associated with.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_KEY
    key;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);
  SOC_SAND_CHECK_NULL_INPUT(ilm_val);
  SOC_SAND_CHECK_NULL_INPUT(found);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(ilm_val);

  SOC_PB_PP_LEM_ACCESS_KEY_clear(&key);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  soc_pb_pp_frwrd_ilm_lem_key_build(
    ilm_key,
    &key
  );

  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &key,
          &payload,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if(*found)
  {
    soc_pb_pp_frwrd_ilm_lem_payload_parse(
      &payload,
      ilm_val
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GET_VERIFY);

  res = soc_pb_pp_frwrd_ilm_key_verify(
          unit,
          ilm_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the block of entries from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_keys,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_DECISION_INFO                 *ilm_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    num_scanned = 0;
  uint8
    is_found,
    is_valid;
  SOC_PB_PP_LEM_ACCESS_KEY
    key;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
  SOC_SAND_CHECK_NULL_INPUT(ilm_keys);
  SOC_SAND_CHECK_NULL_INPUT(ilm_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  *nof_entries = 0;

  SOC_PB_PP_LEM_ACCESS_KEY_clear(&key);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  /*
   *	Loop on the entry index
   */
  for ( num_scanned = 0;
        (num_scanned < block_range->entries_to_scan) &&
         (*nof_entries < block_range->entries_to_act) &&
         (block_range->iter < SOC_PB_PP_LEM_NOF_ENTRIES);
        ++block_range->iter, ++num_scanned)
  {
    SOC_PB_PP_LEM_ACCESS_KEY_clear(&key);

    res = soc_pb_pp_lem_access_entry_by_index_get_unsafe(
            unit,
            block_range->iter,
            &key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((is_valid == TRUE) && (key.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM))
    {
      soc_pb_pp_frwrd_ilm_lem_key_parse(
        &key,
        &ilm_keys[*nof_entries]
      );

      res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
              unit,
              &key,
              &payload,
              &is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      soc_pb_pp_frwrd_ilm_lem_payload_parse(
        &payload,
        &ilm_vals[*nof_entries]
      );

      (*nof_entries)++;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_get_block_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_GET_BLOCK_VERIFY);

  

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove incoming label key from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;

  soc_pb_pp_frwrd_ilm_lem_key_build(
    ilm_key,
    &request.key
    );

  res = soc_pb_pp_lem_access_entry_remove_unsafe(
         unit,
         &request,
         &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY                       *ilm_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_REMOVE_VERIFY);

  /* remove it if exists*/

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_remove_verify()", 0, 0);
}

/*********************************************************************
*     Remove all keys from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ilm_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    i;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack;
  uint8
    is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_TABLE_CLEAR_UNSAFE);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;

  /*
   *	Loop on the entry index
   */
  for ( i = 0; i < SOC_PB_PP_LEM_NOF_ENTRIES; ++i)
  {
    res = soc_pb_pp_lem_access_entry_by_index_get_unsafe(
            unit,
            i,
            &request.key,
            &is_valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((is_valid == TRUE) && (request.key.type == SOC_PB_PP_LEM_ACCESS_KEY_TYPE_ILM))
    {
      res = soc_pb_pp_lem_access_entry_remove_unsafe(
              unit,
              &request,
              &ack
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_table_clear_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ilm_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_ILM_TABLE_CLEAR_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ilm_table_clear_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_ilm_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_frwrd_ilm;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_ilm_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_frwrd_ilm;
}

uint32
  SOC_PB_PP_FRWRD_ILM_GLBL_KEY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_ILM_GLBL_KEY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_ILM_GLBL_ELSP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_ELSP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  for (ind = 0; ind < SOC_SAND_PP_NOF_BITS_IN_EXP; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_map_tbl[ind], SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_ILM_GLBL_ELSP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_ILM_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mapped_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_local_port, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->inrif, SOC_PB_PP_RIF_ID_MAX, SOC_PB_PP_RIF_ID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_ILM_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_ILM_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_ILM_GLBL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_ILM_GLBL_KEY_INFO, &(info->key_info), 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_ILM_GLBL_ELSP_INFO, &(info->elsp_info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_ILM_GLBL_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

