#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_frwrd_extend_p2p.c,v 1.11 Broadcom SDK $
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
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_extend_p2p.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vsi.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ilm.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define ARAD_PP_FRWRD_EXTEND_P2P_LEM_BUF_SIZE                    (2)
#define ARAD_PP_FRWRD_EXTEND_P2P_SEM_KEY_NOF_BITS                (41)


#define ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_MAX                (3)

#define ARAD_PP_FRWRD_EXTEND_P2P_MPLS_PUSH_PROFILE_MAX           (3)



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
  Arad_pp_procedure_desc_element_frwrd_extend_p2p[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_MIM_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_EXTEND_P2P_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_frwrd_extend_p2p[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_FRWRD_EXTEND_P2P_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_LEN_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'tpid_profile' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_IS_NOT_PUSH_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_IS_NOT_PUSH_ERR",
    "mpls command has to be push. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_PUSH_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_PUSH_PROFILE_OUT_OF_RANGE_ERR",
    "The push profile of the mpls command is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_EXTEND_P2P_INFO_FIELD_NOT_SUPPORTED_FOR_AC_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_INFO_FIELD_NOT_SUPPORTED_FOR_AC_ERR",
    "TPID-profile and has-cw should be clear (0) for AC. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_EXTEND_P2P_KEY_PWE_FIELD_NOT_SUPPORTED_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_KEY_PWE_FIELD_NOT_SUPPORTED_ERR",
    "not supported key for P2P PWE. \n\r "
    "port, RIF and EXP has to be cleared (0) . \n\r "    ,
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_EXTEND_P2P_ALWAYS_ENABLED_ERR,
    "ARAD_PP_FRWRD_EXTEND_P2P_ALWAYS_ENABLED_ERR",
    "extend p2p always enabled \n\r "
    "the enable/disable is per port, use soc_ppd_port_info_set. \n\r "    ,
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


/*
 * convert LIF key to buffer
 */
STATIC uint32
  arad_pp_frwrd_p2p_key_to_lem_key(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY         *ac_key,
      SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY  *pwe_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *lem_key
    )
{
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_FRWRD_ILM_KEY
    ilm_key;
  uint32
    sem_buffer[ARAD_PP_FRWRD_EXTEND_P2P_LEM_BUF_SIZE];
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(lem_key);

  ARAD_CLEAR(sem_buffer,uint32,ARAD_PP_FRWRD_EXTEND_P2P_LEM_BUF_SIZE);

  ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  ARAD_PP_LEM_ACCESS_KEY_clear(lem_key);

  if (pwe_key)
  {
    ARAD_PP_FRWRD_ILM_KEY_clear(&ilm_key);
    ilm_key.in_label = pwe_key->in_label;
    arad_pp_frwrd_ilm_lem_key_build(unit,&ilm_key,lem_key);
    goto exit;
  }


  /* map LIF-key to sem key */
  if (ac_key)
  {
    res = arad_pp_l2_lif_ac_key_to_sem_key_unsafe(unit,ac_key,&sem_key);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* convert sem-key to buffer */
    res = arad_pp_isem_access_key_to_buffer(unit,&sem_key, ARAD_PP_ISEM_ACCESS_ID_ISEM_A, 0, sem_buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    lem_key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_EXTENDED;
    lem_key->prefix.nof_bits = ARAD_PP_EXTENDED_KEY_PREFIX_NOF_BITS;

    lem_key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED;

    /* convert sem-key to lem key */
    lem_key->nof_params = 1;
    lem_key->param[0].value[0] = sem_buffer[0];
    lem_key->param[0].value[1] = sem_buffer[1];
    lem_key->param[0].nof_bits = ARAD_PP_FRWRD_EXTEND_P2P_SEM_KEY_NOF_BITS;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(" error in  arad_pp_frwrd_lif_ac_key_convert()",0,0);
}

/*
 * convert lem payload to p2p-info
 */
uint32
  arad_pp_frwrd_p2p_info_from_lem_buffer(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                                  key_type,
      SOC_SAND_OUT  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *p2p_info,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD                   *payload
    )
{
  uint32
    act_asd;
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(p2p_info);

  if (key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_AC || key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_MIM)
  {
    /* can be outlif or MPLS-command with push profile 0-3*/
    res = arad_pp_fwd_decision_in_buffer_parse(
            unit,
            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
            payload->dest,
            payload->asd,
            payload->flags,
            &p2p_info->forward_decision
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else if (key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE)
  {
    act_asd = payload->asd;
    p2p_info->tpid_profile = payload->tpid_profile;
    p2p_info->has_cw = payload->has_cw;

    /* can be outlif*/
    res = arad_pp_fwd_decision_in_buffer_parse(
            unit,
            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
            payload->dest,
            act_asd,
            payload->flags,
            &p2p_info->forward_decision
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(" error in  arad_pp_frwrd_p2p_info_from_lem_buffer()",0,0);
}
  
/*
 * convert p2p-info to lem payload
 */
STATIC uint32
  arad_pp_frwrd_p2p_info_to_lem_buffer(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                                  key_type,
      SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *p2p_info,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                 *payload
    )
{
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(p2p_info);

  if (key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_AC || key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_MIM)
  {
    /* p2p */
    payload->is_dynamic = TRUE;
    payload->age = 0;

    /* can be outlif or MPLS-command with push profile 0-3*/
    res = arad_pp_fwd_decision_in_buffer_build(
            unit,
            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
            &(p2p_info->forward_decision),
            &payload->dest,
            &payload->asd
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_OULTIF;
  }
  else if (key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE)
  {
    /* p2p */
    payload->is_dynamic = TRUE;
    payload->age = 0;
    /* can be outlif*/
    res = arad_pp_fwd_decision_in_buffer_build(
            unit,
            ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_EXTENDED,
            &(p2p_info->forward_decision), 
            &payload->dest,
            &payload->asd
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    payload->has_cw = p2p_info->has_cw;
    payload->tpid_profile = p2p_info->tpid_profile;
    payload->flags = ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_OULTIF;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(" error in  arad_pp_frwrd_p2p_info_to_lem_buffer()",0,0);
}

uint32
  arad_pp_frwrd_extend_p2p_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Setting global information of the Extended P2P services,
 *     including enabling this mode and the profiles to assign
 *     for these services
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_UNSAFE);
  
  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  fld_val = glbl_info->enable;
    

  fld_val = glbl_info->pwe_info.system_vsi;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_LSR_P2P_SERVICE_CFGr, REG_PORT_ANY, 0, TRANSPARENT_P2P_PWE_VSIf,  fld_val));
  /*
    res = arad_pp_vsi_sys_to_local_map_get_unsafe(
          unit,
          glbl_info->pwe_info.system_vsi,
          &local_vsi,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO, glbl_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_glbl_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_VERIFY);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Setting global information of the Extended P2P services,
 *     including enabling this mode and the profiles to assign
 *     for these services
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO          *glbl_info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_clear(glbl_info);


  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LSR_P2P_SERVICE_CFGr, REG_PORT_ANY, 0, TRANSPARENT_P2P_PWE_VSIf, &fld_val));
  glbl_info->pwe_info.system_vsi = fld_val;

  glbl_info->enable = TRUE;

  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Add P2P service for packets entering from the access
 *     side with AC key = (vlan-domain x VID x VID). Thus,
 *     these packets will have the same processing and
 *     forwarding regardless of the MAC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_ac_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *ac_p2p_info,
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
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

 /*
  * build key
  */
  res = arad_pp_frwrd_p2p_key_to_lem_key(
          unit,
          ac_key,
          NULL,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
  request.stamp = 0;
 /*
  * build payload
  */
  res = arad_pp_frwrd_p2p_info_to_lem_buffer(
          unit,
          ARAD_PP_FRWRD_P2P_KEY_TYPE_AC,
          ac_p2p_info,
          &payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /*
   *  add to lem
   */
  res = arad_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    switch(ack_status.reason)
    {
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_MACT_FULL:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_CAM_FULL:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      break;

    case ARAD_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
      break;

    case ARAD_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT:
   case ARAD_PP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_3;
      break;

   case ARAD_PP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC:
      *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
      break;

    case ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN:
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
    }
  
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_ac_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_ac_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *ac_p2p_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_ADD_VERIFY);

  res = ARAD_PP_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_verify(ac_p2p_info,ARAD_PP_FRWRD_P2P_KEY_TYPE_AC);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if (ac_p2p_info->has_cw || ac_p2p_info->tpid_profile)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_EXTEND_P2P_INFO_FIELD_NOT_SUPPORTED_FOR_AC_ERR, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_ac_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the P2P service for packets entering from the
 *     access side with AC key = (vlan-domain x VID x VID).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_ac_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *ac_p2p_info,
    SOC_SAND_OUT uint8                                *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY
    lem_key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    is_found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  SOC_SAND_CHECK_NULL_INPUT(ac_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(ac_p2p_info);
  ARAD_PP_LEM_ACCESS_KEY_clear(&lem_key);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  /*
   *  build key
   */
  res = arad_pp_frwrd_p2p_key_to_lem_key(
          unit,
          ac_key,
          NULL,
          &(lem_key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /*
   *	get from mact
   */
  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &lem_key,
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *found = is_found;
  if (*found)
  {
    /*
     *	parse payload
     */

    res = arad_pp_frwrd_p2p_info_from_lem_buffer(
            unit,
            ARAD_PP_FRWRD_P2P_KEY_TYPE_AC,
            ac_p2p_info,
            &(payload)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_ac_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_ac_get_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_GET_VERIFY);

  res = ARAD_PP_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_ac_get_verify()", 0, 0);
}

/*********************************************************************
*     Remove P2P service for packets entering from the access
 *     side with AC key = (vlan-domain x VID x VID).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_ac_remove_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key
  )
{

  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ac_key);


  SOC_SAND_CHECK_NULL_INPUT(ac_key);
  
 

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  /*
   *	build key
   */
  request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
  request.stamp = 0;

  res = arad_pp_frwrd_p2p_key_to_lem_key(
          unit,
          ac_key,
          NULL,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


 res = arad_pp_lem_access_entry_remove_unsafe(
         unit,
         &request,
         &ack_status
         );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!((ack_status.is_success == TRUE) || (ack_status.reason == ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_ac_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_ac_remove_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_L2_LIF_AC_KEY                      *ac_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_VERIFY);

  res = ARAD_PP_L2_LIF_AC_KEY_verify(unit, ac_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_ac_remove_verify()", 0, 0);
}

/*********************************************************************
*     Add P2P service for packets entering with PWE key =
 *     (in-RIF x VC label). Thus, these packets will have the
 *     same processing and forwarding regardless of the MAC.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_pwe_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY                *pwe_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *pwe_p2p_info,
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
    ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_key);
  SOC_SAND_CHECK_NULL_INPUT(pwe_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

   
  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);


 /*
  * build key
  */
  res = arad_pp_frwrd_p2p_key_to_lem_key(
          unit,
          NULL,
          pwe_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
  request.stamp = 0;
 /*
  * build payload
  */
  res = arad_pp_frwrd_p2p_info_to_lem_buffer(
          unit,
          ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE,
          pwe_p2p_info,
          &payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /*
   *  add to lem
   */
  res = arad_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    switch(ack_status.reason)
    {
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_MACT_FULL:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_CAM_FULL:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      break;

    case ARAD_PP_LEM_ACCESS_FAIL_REASON_FID_LIMIT:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
      break;

    case ARAD_PP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT:
   case ARAD_PP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_UNKNOWN:
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_3;
      break;

   case ARAD_PP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC:
    case ARAD_PP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC:
      *success = SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST;
      break;

    case ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN:
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
    }
  
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_pwe_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_pwe_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY                *pwe_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO         *pwe_p2p_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_PWE_ADD_VERIFY);

  res = ARAD_PP_FRWRD_EXTEND_PWE_KEY_verify(unit, pwe_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_verify(pwe_p2p_info,ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_pwe_add_verify()", 0, 0);
}

/*********************************************************************
*     Gets the P2P service for packets entering with PWE key =
 *     (in-RIF x VC label).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_pwe_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO        *pwe_p2p_info,
    SOC_SAND_OUT uint8                                *found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_KEY
    lem_key;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    is_found;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_key);
  SOC_SAND_CHECK_NULL_INPUT(pwe_p2p_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(pwe_p2p_info);
  /*
   * build key
   */
  res = arad_pp_frwrd_p2p_key_to_lem_key(
          unit,
          NULL,
          pwe_key,
          &(lem_key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  /*
   * get from mact
   */
  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &lem_key,
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *found = is_found;
  if (*found)
  {
    /*
     * parse payload
     */

    res = arad_pp_frwrd_p2p_info_from_lem_buffer(
            unit,
            ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE,
            pwe_p2p_info,
            &(payload)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_pwe_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_pwe_get_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_PWE_GET_VERIFY);

  res = ARAD_PP_FRWRD_EXTEND_PWE_KEY_verify(unit, pwe_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_pwe_get_verify()", 0, 0);
}

/*********************************************************************
*     Removes P2P service for packets entering with PWE key =
 *     (in-RIF x VC label).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_frwrd_extend_p2p_pwe_remove_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  )
{
 
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_AC_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pwe_key);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  /*
   *  build key
   */
  request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
  request.stamp = 0;

  res = arad_pp_frwrd_p2p_key_to_lem_key(
          unit,
          NULL,
          pwe_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_lem_access_entry_remove_unsafe(
    unit,
    &request,
    &ack_status
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!((ack_status.is_success == TRUE) || (ack_status.reason == ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 50, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_pwe_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_extend_p2p_pwe_remove_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY               *pwe_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_EXTEND_P2P_PWE_REMOVE_VERIFY);

  res = ARAD_PP_FRWRD_EXTEND_PWE_KEY_verify(unit, pwe_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_extend_p2p_pwe_remove_verify()", 0, 0);
}
/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_frwrd_extend_p2p module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_extend_p2p_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_extend_p2p;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_frwrd_extend_p2p module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_extend_p2p_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_extend_p2p;
}
uint32
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->system_vsi, ARAD_PP_SYS_VSI_ID_MAX, ARAD_PP_SYS_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_verify()",0,0);
}

uint32
  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO, &(info->ac_info), 11, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_EXTEND_P2P_GLBL_LIF_INFO, &(info->pwe_info), 12, exit);
  if (!info->enable)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_EXTEND_P2P_ALWAYS_ENABLED_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_EXTEND_P2P_GLBL_INFO_verify()",0,0);
}

uint32
  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_verify(
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO *info,
    SOC_SAND_IN  uint32                          key_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_FRWRD_DECISION_INFO, &(info->forward_decision), 10, exit);

  if (key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_AC )
  {
    if (info->forward_decision.additional_info.eei.type == ARAD_PP_EEI_TYPE_MPLS)
    {
      if (info->forward_decision.additional_info.eei.val.mpls_command.command != ARAD_PP_MPLS_COMMAND_TYPE_PUSH)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_IS_NOT_PUSH_ERR, 20, exit);
      }
      SOC_SAND_ERR_IF_ABOVE_MAX(info->forward_decision.additional_info.eei.val.mpls_command.push_profile,
        ARAD_PP_FRWRD_EXTEND_P2P_MPLS_PUSH_PROFILE_MAX,
        ARAD_PP_FRWRD_EXTEND_P2P_MPLS_CMD_PUSH_PROFILE_OUT_OF_RANGE_ERR,
        30, exit);
    }

  }
  if (key_type == ARAD_PP_FRWRD_P2P_KEY_TYPE_PWE )
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->tpid_profile, ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_MAX, ARAD_PP_FRWRD_EXTEND_P2P_TPID_PROFILE_OUT_OF_RANGE_ERR, 40, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_EXTEND_P2P_FRWRD_INFO_verify()",0,0);
}

uint32
  ARAD_PP_FRWRD_EXTEND_PWE_KEY_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_EXTEND_PWE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_label, SOC_SAND_PP_MPLS_LABEL_MAX, SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->internal_exp, SOC_SAND_PP_MPLS_EXP_MAX, SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->in_local_port, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->inrif_port, SOC_DPP_CONFIG(unit)->l3.nof_rifs, ARAD_PP_RIF_ID_OUT_OF_RANGE_ERR, 13, exit);
  if (info->internal_exp || info->in_local_port || info->inrif_port)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_EXTEND_P2P_KEY_PWE_FIELD_NOT_SUPPORTED_ERR, 50, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_FRWRD_EXTEND_PWE_KEY_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */
