#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_frwrd_ilm.c,v 1.28 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif
#include <soc/dpp/drv.h>
#include <soc/hwstate/hw_log.h>


#ifdef PLISIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#else
  #include <soc/dpp/ARAD/arad_sim_em.h>
#endif


/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* EXP nof bits */
#define ARAD_PP_FRWRD_ILM_EXP_NOF_BITS      (3)

#define ARAD_PP_IHB_MPLS_EXP_REG_NOF_FLDS   (8)


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

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_frwrd_ilm[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_BLOCK_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_BLOCK_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_BLOCK_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_TABLE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_TABLE_CLEAR_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_TABLE_CLEAR_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_TABLE_CLEAR_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_ILM_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_frwrd_ilm[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_ILM_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    ARAD_PP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR,
    "ARAD_PP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR",
    "If by the global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is masked, \n\r"
    "then the inport field of ILM key has to be zero \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR,
    "ARAD_PP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR",
    "If by the global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is masked, \n\r"
    "then the inrif field of ILM key has to be zero \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_ILM_EEI_NOT_MPLS_ERR,
    "ARAD_PP_FRWRD_ILM_EEI_NOT_MPLS_ERR",
    "EEI in ILM add command must be of type MPLS. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR,
    "ARAD_PP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR",
    "Mapped_exp ILM key field should be zero for labels not in the ELSP range . \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR,
    "ARAD_PP_FRWRD_ILM_KEY_MASK_NOT_SUPPORTED_ERR",
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
  arad_pp_frwrd_ilm_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  ARAD_PP_FRWRD_ILM_GLBL_INFO
    glbl_info;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);

 /*
  * default set lookup to be according to label only.
  */
  glbl_info.key_info.mask_inrif = TRUE;
  glbl_info.key_info.mask_port = TRUE;

  res = arad_pp_frwrd_ilm_glbl_info_set_unsafe(
          unit,
          &glbl_info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_init_unsafe()", 0, 0);
}

STATIC
  uint32
    arad_pp_frwrd_ilm_key_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key
    )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_FRWRD_ILM_GLBL_INFO
    glbl_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = ARAD_PP_FRWRD_ILM_KEY_verify(unit, ilm_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = arad_pp_frwrd_ilm_glbl_info_get(
          unit,
          &glbl_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if((glbl_info.key_info.mask_port) && (ilm_key->in_local_port != 0))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_ILM_KEY_INPORT_NOT_MASKED_ERR, 40, exit);
  }
  if((glbl_info.key_info.mask_inrif) && (ilm_key->inrif != 0))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_ILM_KEY_INRIF_NOT_MASKED_ERR, 50, exit);
  }
  
  if((ilm_key->in_label < glbl_info.elsp_info.labels_range.start) ||
     (ilm_key->in_label > glbl_info.elsp_info.labels_range.end))
  {
    if(ilm_key->mapped_exp != 0)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_ILM_KEY_MAPPED_EXP_NOT_ZERO_ERR, 50, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_key_verify()", 0, 0);
}

/* build lem access key for ILM host address */
void
    arad_pp_frwrd_ilm_lem_key_build(
       SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY  *ilm_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  ARAD_PP_LEM_ACCESS_KEY_clear(key);
  if (ilm_key->flags & ARAD_PP_FRWRD_ILM_KEY_COUPLING) 
  {
    key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_COUPLING_ILM;
    key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_COUPLING_ILM;
    num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_COUPLING_ILM;
    key->param[0].nof_bits = (uint8)num_bits;
    key->param[0].value[0] = ilm_key->in_label;
  
    num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_COUPLING_ILM;
    key->param[1].nof_bits = (uint8)num_bits;
    key->param[1].value[0] = ilm_key->in_label_second;
    key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_COUPLING_ILM;
    key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_COUPLING_ILM;
  } 
  else
  {   
  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM;
  key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM;

  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = ilm_key->in_label;
  
  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = ilm_key->mapped_exp;

  key->param[2].value[0] = ilm_key->in_local_port;
  if (SOC_IS_JERICHO(unit)) {
      /* in Jericho the port is 9 bits, 1bit Core-Bit and 8bits In-PP-Port */
      key->param[2].value[0] |= ilm_key->in_core << ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
      key->param[2].nof_bits = (uint8)(ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM_JERICHO);
  } else {
      
      key->param[2].nof_bits = (uint8)ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
  }

  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
  key->param[3].nof_bits = (uint8)num_bits;
  key->param[3].value[0] = ilm_key->inrif;

  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_ILM;
  }
}

/* parse lem access key for IpV4 host address
*/
  void
    arad_pp_frwrd_ilm_lem_key_parse(
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY *key,
      SOC_SAND_OUT ARAD_PP_FRWRD_ILM_KEY                       *ilm_key
    )
{
  ilm_key->in_label = key->param[0].value[0];
  ilm_key->mapped_exp = (SOC_SAND_PP_MPLS_EXP)key->param[1].value[0];
  ilm_key->in_local_port = key->param[2].value[0] & ((1 << ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM) -1);
  ilm_key->in_core = (key->param[2].value[0]  >> ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM);
  ilm_key->inrif = key->param[3].value[0];
}

/* build lem access payload for IpV4 host address */
STATIC
  uint32
    arad_pp_frwrd_ilm_lem_payload_build(
      SOC_SAND_IN int                                      unit,
      SOC_SAND_IN ARAD_PP_FRWRD_DECISION_INFO                 *ilm_val,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                 *payload
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);

  /*
  *	Get the encoded destination and ASD
  */
  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
          ilm_val,
          &payload->dest,
          &payload->asd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  payload->age = 0;
  payload->is_dynamic = FALSE;

  payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST;

  if(ilm_val->additional_info.eei.type != ARAD_PP_EEI_TYPE_EMPTY) {
    payload->flags |= ARAD_PP_FWD_DECISION_PARSE_EEI;
  }
  else if(ilm_val->additional_info.outlif.type != ARAD_PP_OUTLIF_ENCODE_TYPE_NONE) {
    payload->flags |= ARAD_PP_FWD_DECISION_PARSE_OULTIF;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_lem_payload_build()", 0, 0);
}

/* parse lem access payload for IpV4 host address
*/
  void
    arad_pp_frwrd_ilm_lem_payload_parse(
      SOC_SAND_IN int                                      unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD                  *payload,
      SOC_SAND_OUT  ARAD_PP_FRWRD_DECISION_INFO               *ilm_val
    )
{
  arad_pp_fwd_decision_in_buffer_parse(
    unit,
    ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_ILM,
    payload->dest,
    payload->asd,
    payload->flags,
    ilm_val    
  );
}

/*********************************************************************
*     Setting global information of the ILM (ingress label
 *     mapping) (including ELSP and key building information)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val = 0,
    fld_ndx,
    data32;
  uint64
    data64;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  COMPILER_64_ZERO(data64);

  /* ELSP range { */
  fld_val = glbl_info->elsp_info.labels_range.start;
  soc_reg64_field32_set(unit, IHP_LSR_ELSP_RANGEr, &data64, ELSP_RANGE_MINf, fld_val);

  fld_val = glbl_info->elsp_info.labels_range.end;
  soc_reg64_field32_set(unit, IHP_LSR_ELSP_RANGEr, &data64, ELSP_RANGE_MAXf, fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_LSR_ELSP_RANGEr(unit, SOC_CORE_ALL, data64));
  /* ELSP range } */

  /* ELSP EXP MAP { */
  data32 = 0;
  for(fld_ndx = 0; fld_ndx < ARAD_PP_IHB_MPLS_EXP_REG_NOF_FLDS; ++fld_ndx) 
  {
    fld_val = glbl_info->elsp_info.exp_map_tbl[fld_ndx];
    SHR_BITCOPY_RANGE(&data32,fld_ndx*ARAD_PP_FRWRD_ILM_EXP_NOF_BITS,&fld_val,0,ARAD_PP_FRWRD_ILM_EXP_NOF_BITS);
  }  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_MPLS_EXP_MAPr(unit, SOC_CORE_ALL, data32));
  /* ELSP EXP MAP } */

  res = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.mask_inrif.set(unit, glbl_info->key_info.mask_inrif);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.mask_port.set(unit, glbl_info->key_info.mask_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

#ifdef BCM_88660
  if (SOC_IS_ARADPLUS(unit)) {
     
    /* Global pipe mode */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_VTT_GENERAL_CONFIGS_1r, SOC_CORE_ALL, 0, ENABLE_MPLS_PIPEf,  glbl_info->short_pipe_enable));
  }
 
#endif /* BCM_88660 */

  /* this requires to change FLP program that builds ILM key*/  
  res = arad_pp_lem_ilm_key_build_set(
          unit,
          glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GLBL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_ILM_GLBL_INFO, glbl_info, 10, exit);  
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GLBL_INFO_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the ILM (ingress label
 *     mapping) (including ELSP and key building information)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_ILM_GLBL_INFO                 *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val = 0,
    fld_ndx,
    data32;
  uint64
    data64;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  ARAD_PP_FRWRD_ILM_GLBL_INFO_clear(glbl_info);

  COMPILER_64_ZERO(data64);

  /* ELSP range { */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_LSR_ELSP_RANGEr(unit, 0, &data64));

  fld_val = soc_reg64_field32_get(unit, IHP_LSR_ELSP_RANGEr, data64, ELSP_RANGE_MINf);
  glbl_info->elsp_info.labels_range.start = fld_val;

  fld_val = soc_reg64_field32_get(unit, IHP_LSR_ELSP_RANGEr, data64, ELSP_RANGE_MAXf);
  glbl_info->elsp_info.labels_range.end = fld_val;
  /* ELSP range } */

  /* ELSP EXP MAP { */
  data32 = 0;  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_MPLS_EXP_MAPr(unit, 0, &data32));
  for(fld_ndx = 0; fld_ndx < ARAD_PP_IHB_MPLS_EXP_REG_NOF_FLDS; ++fld_ndx) 
  {
    fld_val = 0;
    SHR_BITCOPY_RANGE(&fld_val,0,&data32,fld_ndx*ARAD_PP_FRWRD_ILM_EXP_NOF_BITS,ARAD_PP_FRWRD_ILM_EXP_NOF_BITS);
    glbl_info->elsp_info.exp_map_tbl[fld_ndx] = fld_val;    
  }    
  /* ELSP EXP MAP } */

  res = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.mask_inrif.get(unit, &glbl_info->key_info.mask_inrif);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.ilm_info.mask_port.get(unit, &glbl_info->key_info.mask_port);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_glbl_info_get_unsafe()", 0, 0);
}

#if defined(INCLUDE_KBP) && !defined(BCM_88030)

STATIC
  uint32
    arad_pp_frwrd_ilm_kbp_key_mask_encode(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY        *ilm_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *mask
    )
{
    uint32
        res,
        mask_full = SOC_SAND_U32_MAX,
        mask_void = 0;
    ARAD_PP_FRWRD_ILM_GLBL_INFO
        glbl_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_clear(data);
    ARAD_PP_LEM_ACCESS_KEY_clear(mask);

    ARAD_PP_FRWRD_ILM_GLBL_INFO_clear(&glbl_info);
    res = arad_pp_frwrd_ilm_glbl_info_get(unit, &glbl_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
    
    /* Encode Key into buffer according to the following format:
     *  
     * +-------------------------------------------------------------------------------+
     * | 32b 0   | 5b - Reserved | 12b In-RIF  | 8b In-Port  | 3b EXP  | 20b In-Label  |
     * +-------------------------------------------------------------------------------+ 
     *  79     48 47           43 42         31 30         23 22     20 19            0
     *  
     */

    /* Mask is calculated according to mpls soc-properties */
    /* not necessarily the same encoding than inside */
    /* Param 0  - in label */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, ilm_key->in_label, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, mask_full, mask);
    /* Param 1  - exp */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, ilm_key->mapped_exp, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, mask_full, mask);
    /* Param 2  - in-port */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, ilm_key->in_local_port, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, ((glbl_info.key_info.mask_port)? mask_void: mask_full), mask);
    /* Param 3  - in-rif */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 3, ilm_key->inrif, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 3, ((glbl_info.key_info.mask_inrif)? mask_void: mask_full), mask);


    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ilm_kbp_key_mask_encode()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ilm_kbp_key_mask_decode(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY       *mask,
      SOC_SAND_OUT ARAD_PP_FRWRD_ILM_KEY        *ilm_key
    )
{
    uint32
        fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_FRWRD_ILM_KEY_clear(ilm_key);
    SOC_SAND_CHECK_NULL_INPUT(data);
    SOC_SAND_CHECK_NULL_INPUT(mask);

    /* Encode Key into buffer according to the following format:
     *  
     * +-------------------------------------------------------------------------------+
     * | 32b 0   | 5b - Reserved | 12b In-RIF  | 8b In-Port  | 3b EXP  | 20b In-Label  |
     * +-------------------------------------------------------------------------------+ 
     *  79     48 47           43 42         31 30         23 22     20 19            0
     *  
     */

    /* Mask is calculated according to mpls soc-properties */
    /* not necessarily the same encoding than inside */
    /* Param 0  - in label */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 0, &ilm_key->in_label, data, mask);
    /* Param 1  - exp */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 1, &fld_val, data, mask);
    ilm_key->mapped_exp = (uint8) (fld_val & 0xFF);
    /* Param 2  - in-port */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 2, &ilm_key->in_local_port, data, mask);
    /* Param 3  - in-rif */
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_DECODE(unit, frwrd_table_id, 3, &ilm_key->inrif, data, mask);

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ilm_kbp_key_mask_decode()",0,0);
}


STATIC
  uint32
    arad_pp_frwrd_ilm_kbp_record_add(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY        *ilm_key,
      SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO  *ilm_val,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
    )
{
    uint32
        priority,
        res;
    ARAD_TCAM_ACTION                                
        action;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_PP_LEM_ACCESS_PAYLOAD
      payload;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&action, 0x0, sizeof(action));

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_LSR;

    /* The priority is irrelevant for MPLS records */
    priority = 0; 

    /* Encode Key */
    res = arad_pp_frwrd_ilm_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            ilm_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    /* Encode Result */
    /* Convert ILM result value to conventional structure */
    res = arad_pp_frwrd_ilm_lem_payload_build(
            unit,
            ilm_val,
            &payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Add record */
    res = arad_pp_frwrd_ip_tcam_route_kbp_add_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask,
            priority,
            &payload,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ilm_kbp_record_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ilm_kbp_record_remove(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY        *ilm_key
    )
{
    uint32
        res;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_LSR;

    /* Encode Key */
    res = arad_pp_frwrd_ilm_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            ilm_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    /* Remove Record */
    res = arad_pp_frwrd_ip_tcam_route_kbp_remove_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ilm_kbp_record_remove()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ilm_kbp_record_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY        *ilm_key,
      SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO  *ilm_val,
      SOC_SAND_OUT uint8                        *found
    )
{
    uint32
        priority,
        res;
    ARAD_PP_LEM_ACCESS_PAYLOAD
        payload;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_LSR;

    /* Encode Key */
    res = arad_pp_frwrd_ilm_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            ilm_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    /* get record */
    res = arad_pp_frwrd_ip_tcam_route_kbp_get_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask,
            &priority,
            &payload,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

    if (*found) 
    {
        arad_pp_frwrd_ilm_lem_payload_parse(
           unit, 
           &payload, 
           ilm_val
        );
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ilm_kbp_record_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_ilm_kbp_table_clear(
      SOC_SAND_IN  int   unit
    )
{
    uint32
        res;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_LSR;

    /* Clear Table */
    res = arad_pp_frwrd_ip_tcam_kbp_table_clear(
            unit,
            frwrd_table_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_ilm_kbp_table_clear()",0,0);
}

#endif

/*********************************************************************
*     Add mapping from incoming label to destination and MPLS
 *     command.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);
  SOC_SAND_CHECK_NULL_INPUT(ilm_val);
  SOC_SAND_CHECK_NULL_INPUT(success);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  /* Check if Exteral Lookup device is used */
  if(ARAD_KBP_ENABLE_MPLS) 
  {
      res = arad_pp_frwrd_ilm_kbp_record_add(
                unit,
                ilm_key,
                ilm_val,
                success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
      ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
      ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

      request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;

      arad_pp_frwrd_ilm_lem_key_build(
         unit,
          ilm_key,
          &request.key
        );

      arad_pp_frwrd_ilm_lem_payload_build(
        unit,
        ilm_val,
        &payload
      );

      res = arad_pp_lem_access_entry_add_unsafe(
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
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_DECISION_INFO                 *ilm_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_ADD_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_DECISION_INFO, ilm_val, 20, exit);

  res = arad_pp_frwrd_ilm_key_verify(  
          unit,        
          ilm_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the value (destination and MPLS command) the
 *     incoming label key is associated with.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO                 *ilm_val,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY
    key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);
  SOC_SAND_CHECK_NULL_INPUT(ilm_val);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_PP_FRWRD_DECISION_INFO_clear(ilm_val);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  /* Check if Exteral Lookup device is used */
  if(ARAD_KBP_ENABLE_MPLS) 
  {
      res = arad_pp_frwrd_ilm_kbp_record_get(
                unit,
                ilm_key,
                ilm_val,
                found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
      ARAD_PP_LEM_ACCESS_KEY_clear(&key);
      ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

      arad_pp_frwrd_ilm_lem_key_build(
         unit,
        ilm_key,
        &key
      );

      res = arad_pp_lem_access_entry_by_key_get_unsafe(
              unit,
              &key,
              &payload,
              found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      if(*found)
      {
        arad_pp_frwrd_ilm_lem_payload_parse(
          unit,
          &payload,
          ilm_val
        );
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GET_VERIFY);

  res = arad_pp_frwrd_ilm_key_verify(
          unit,
          ilm_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_get_verify()", 0, 0);
}

/*********************************************************************
*     Gets the block of entries from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT ARAD_PP_FRWRD_ILM_KEY                       *ilm_keys,
    SOC_SAND_OUT ARAD_PP_FRWRD_DECISION_INFO                 *ilm_vals,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    access_only = FALSE,
    read_index = 0,
    tmp_nof_entries = 0;
  ARAD_PP_FRWRD_ILM_KEY
    ilm_key;
  ARAD_PP_LEM_ACCESS_KEY
    key;
  ARAD_PP_LEM_ACCESS_KEY            
    key_mask;
  ARAD_PP_LEM_ACCESS_KEY
    *read_keys = NULL;
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  ARAD_PP_LEM_ACCESS_KEY
    *read_keys_mask = NULL;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  ARAD_PP_LEM_ACCESS_PAYLOAD
    *read_vals = NULL;
#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
    int i;
    uint32                   nof_total_entries;
#endif
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GET_BLOCK_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);
  SOC_SAND_CHECK_NULL_INPUT(ilm_keys);
  SOC_SAND_CHECK_NULL_INPUT(ilm_vals);  

  *nof_entries = 0;
  
  ARAD_PP_FRWRD_ILM_KEY_clear(&ilm_key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key);
  ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
  

#if defined (BCM_ARAD_SUPPORT) || (PLISIM)
  if ((SAL_BOOT_PLISIM ) || ((SOC_IS_DONE_INIT(unit)) && BCM_UNIT_DO_HW_READ_WRITE(unit)))
  {

      read_vals = (ARAD_PP_LEM_ACCESS_PAYLOAD*)sal_alloc(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * (block_range->entries_to_act), "arad_pp_lem_block_get");
      if (read_vals == NULL)
      {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
      }
      read_keys = (ARAD_PP_LEM_ACCESS_KEY*)sal_alloc(sizeof(ARAD_PP_LEM_ACCESS_KEY) * (block_range->entries_to_act), "arad_pp_lem_block_get");
      if (read_keys == NULL)
      {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 15, exit);
      }

      arad_pp_lem_block_get(unit, NULL, block_range, read_keys, read_vals, &nof_total_entries);
      SOC_SAND_TBL_ITER_SET_END(&(block_range->iter));

	  *nof_entries =0;
      for (i=0; i < nof_total_entries; i++) {

        if ((read_keys[i].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM) 
            || 
             (read_keys[i].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_COUPLING_ILM))
        {
          arad_pp_frwrd_ilm_lem_key_parse(
            &read_keys[i],
            &ilm_keys[*nof_entries]
          );

          ARAD_PP_FRWRD_DECISION_INFO_clear(&ilm_vals[*nof_entries]);
          arad_pp_frwrd_ilm_lem_payload_parse(
            unit,
            &read_vals[i],
            &ilm_vals[*nof_entries]
          );   
          (*nof_entries)++;   
        }


      }
      SOC_SAND_EXIT_NO_ERROR;
  }
#endif

  /* Prepare the keys */
  /* malloc place for keys and vals */
  read_keys = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act,"read_keys");
  read_vals = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act,"read_vals");

  if (!read_keys)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 40, exit);
  }
  if (!read_vals)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 45, exit);
  }

  /* initialize block range */
  res = soc_sand_os_memset(
          read_keys,
          0x0,
          sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_os_memset(
          read_vals,
          0x0,
          sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);


#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if(ARAD_KBP_ENABLE_MPLS) 
  {
      ARAD_KBP_FRWRD_IP_TBL_ID
          frwrd_table_id;
      ARAD_PP_IP_ROUTING_TABLE_RANGE          
          block_range_key;

      frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_LSR;

      /* Initialize read_keys_mask */
      read_keys_mask = soc_sand_os_malloc_any_size(sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act,"read_keys_mask");
      if (!read_keys_mask)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 42, exit);
      }
      res = soc_sand_os_memset(
              read_keys_mask,
              0x0,
              sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      /* Convert the block range */
      ARAD_PP_IP_ROUTING_TABLE_RANGE_clear(&block_range_key);
      block_range_key.entries_to_act = block_range->entries_to_act;
      block_range_key.entries_to_scan = block_range->entries_to_scan;
      block_range_key.start.payload.arr[0] = block_range->iter;

      /* Get the entries */
      res = arad_pp_frwrd_ip_tcam_route_kbp_get_block_unsafe(
                unit,
                frwrd_table_id,
                &block_range_key,
                read_keys,
                read_keys_mask,
                read_vals,
                &tmp_nof_entries
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      block_range->iter = block_range_key.start.payload.arr[0];

      /* Decode the entries: content and payload */
      for (read_index = 0; read_index < tmp_nof_entries; ++read_index)
      {
          /* Decode the key */
          res = arad_pp_frwrd_ilm_kbp_key_mask_decode(
                    unit,
                    frwrd_table_id,
                    &read_keys[read_index],
                    &read_keys_mask[read_index],
                    &ilm_keys[read_index]
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

          ARAD_PP_FRWRD_DECISION_INFO_clear(&ilm_vals[read_index]);
          arad_pp_frwrd_ilm_lem_payload_parse(
            unit,
            &read_vals[read_index],
            &ilm_vals[read_index]
          );      
      }
  } else 
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  {
      /* build key to include ILM, ILM coupling */
      arad_pp_frwrd_ilm_lem_key_build(
         unit,
        &ilm_key,
        &key
      );
     
      /* ILM { */
      /* consider all lem entries for ILM */
      key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM;
      key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
      key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;

      res = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only ,block_range,read_keys,read_vals,&tmp_nof_entries);
      SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

      /*
       *  Loop on the entry index
       */
      for ( read_index = 0; read_index < tmp_nof_entries; ++read_index)
      {
        if ((read_keys[read_index].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM))
        {
          arad_pp_frwrd_ilm_lem_key_parse(
            &read_keys[read_index],
            &ilm_keys[read_index]
          );
          ARAD_PP_FRWRD_DECISION_INFO_clear(&ilm_vals[read_index]);

          arad_pp_frwrd_ilm_lem_payload_parse(
            unit,
            &read_vals[read_index],
            &ilm_vals[read_index]
          );      
        }
      }
  }

  *nof_entries += tmp_nof_entries;
  /* ILM } */

  /* ILM Coupling { */
  tmp_nof_entries = 0;

  /* consider all lem entries for ILM Coupling */
  key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_COUPLING_ILM;
  key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
  key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_COUPLING_ILM;


  /* initialize block range */
  res = soc_sand_os_memset(
          read_keys,
          0x0,
          sizeof(ARAD_PP_LEM_ACCESS_KEY) * block_range->entries_to_act
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_os_memset(
          read_vals,
          0x0,
          sizeof(ARAD_PP_LEM_ACCESS_PAYLOAD) * block_range->entries_to_act
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = arad_pp_frwrd_lem_get_block_unsafe(unit,&key,&key_mask,NULL,access_only ,block_range,read_keys,read_vals,&tmp_nof_entries);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
  /*
   *  Loop on the entry index
   */
  for ( read_index = 0; read_index < tmp_nof_entries; ++read_index)
  {
    if ((read_keys[read_index].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_COUPLING_ILM))
    {
      arad_pp_frwrd_ilm_lem_key_parse(
        &read_keys[read_index],
        &ilm_keys[read_index]
      );
      ARAD_PP_FRWRD_DECISION_INFO_clear(&ilm_vals[read_index]);

      arad_pp_frwrd_ilm_lem_payload_parse(
        unit,
        &read_vals[read_index],
        &ilm_vals[read_index]
      );      
    }
  }

  *nof_entries += tmp_nof_entries;


exit:
  if (read_keys)
  {
    soc_sand_os_free_any_size(read_keys);
  }
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  if (read_keys_mask)
  {
    soc_sand_os_free_any_size(read_keys_mask);
  }
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
  if (read_vals)
  {
    soc_sand_os_free_any_size(read_vals);
  }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_get_block_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_GET_BLOCK_VERIFY);

  

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_get_block_verify()", 0, 0);
}

/*********************************************************************
*     Remove incoming label key from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ilm_key);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  /* Check if Exteral Lookup device is used */
  if(ARAD_KBP_ENABLE_MPLS) 
  {
      res = arad_pp_frwrd_ilm_kbp_record_remove(
                unit,
                ilm_key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
      ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

      request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;

      arad_pp_frwrd_ilm_lem_key_build(
         unit,
        ilm_key,
        &request.key
        );

      res = arad_pp_lem_access_entry_remove_unsafe(
             unit,
             &request,
             &ack
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY                       *ilm_key
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_REMOVE_VERIFY);

  /* remove it if exists*/

  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_remove_verify()", 0, 0);
}

/*********************************************************************
*     Remove all keys from the ILM DB.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_ilm_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY            key;
  ARAD_PP_LEM_ACCESS_KEY            key_mask;
  SOC_SAND_TABLE_BLOCK_RANGE        block_range;
  uint32                            nof_entries;
  ARAD_PP_FRWRD_ILM_KEY             ilm_key;
  ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION        action;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_TABLE_CLEAR_UNSAFE);

#if defined(INCLUDE_KBP) && !defined(BCM_88030)
  /* Check if External Lookup device is used */
  if(ARAD_KBP_ENABLE_MPLS) 
  {
      res = arad_pp_frwrd_ilm_kbp_table_clear(
                unit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
    ARAD_PP_FRWRD_ILM_KEY_clear(&ilm_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key_mask);
    ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    /* build key to include ILM */
    arad_pp_frwrd_ilm_lem_key_build(
       unit,
      &ilm_key,
      &key
    );

    /* consider all ILM entries*/
    key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_ILM;
    key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
    key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;

     /* set action */
    action.type = ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

    res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&key,&key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    /* consider all ILM Coupling entries*/
    key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_COUPLING_ILM;
    key_mask.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED;
    key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;

     /* set action */
    action.type = ARAD_PP_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;

    res = arad_pp_frwrd_lem_traverse_internal_unsafe(unit,&key,&key_mask,NULL,&block_range,&action, TRUE,&nof_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_table_clear_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_ilm_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_ILM_TABLE_CLEAR_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_ilm_table_clear_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_ilm_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_ilm;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_frwrd_ilm module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_ilm_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_ilm;
}

uint32
  ARAD_PP_FRWRD_ILM_GLBL_KEY_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_GLBL_KEY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_ILM_GLBL_KEY_INFO_verify()",0,0);
}

uint32
  ARAD_PP_FRWRD_ILM_GLBL_ELSP_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_GLBL_ELSP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  for (ind = 0; ind < SOC_SAND_PP_NOF_BITS_IN_EXP; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->exp_map_tbl[ind], SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 11, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_ILM_GLBL_ELSP_INFO_verify()",0,0);
}

uint32
  ARAD_PP_FRWRD_ILM_KEY_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mapped_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_local_port, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->inrif, SOC_DPP_CONFIG(unit)->l3.nof_rifs, ARAD_PP_RIF_ID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_ILM_KEY_verify()",0,0);
}

uint32
  ARAD_PP_FRWRD_ILM_GLBL_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_ILM_GLBL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_ILM_GLBL_KEY_INFO, &(info->key_info), 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_ILM_GLBL_ELSP_INFO, &(info->elsp_info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_ILM_GLBL_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

