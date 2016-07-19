/* $Id: pb_pp_frwrd_trill.c,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_frwrd_trill.c
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_trill.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_MAX                     (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_TRILL_MC_ID_MAX                            (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_TRILL_TREE_NICK_MAX                        (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_TRILL_ING_NICK_MAX                         (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_TRILL_ADJACENT_EEP_MAX                     (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_MAX              (SOC_SAND_U32_MAX)
#define SOC_PB_PP_FRWRD_TRILL_CFG_TTL_MAX                          (SOC_SAND_U32_MAX)

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

static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_pp_procedure_desc_element_frwrd_trill[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_FRWRD_TRILL_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_pp_error_desc_element_frwrd_trill[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'nickname_key' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR",
    "The parameter 'mc_id' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR",
    "The parameter 'tree_nick' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR",
    "The parameter 'ing_nick' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'adjacent_eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'expect_adjacent_eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR",
    "The parameter 'cfg_ttl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */



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
STATIC
  uint32
    soc_pb_pp_frwrd_mc_id_to_dest(
      SOC_SAND_IN  uint32      mc_id,
      SOC_SAND_OUT uint32      *dest,
      SOC_SAND_OUT uint32      *add_info
    )
{
  SOC_PB_PP_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&em_dest);
  *add_info = 0;

  em_dest.type = SOC_PB_PP_FRWRD_DECISION_TYPE_MC;
  em_dest.dest_id = mc_id;
  em_dest.additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;

  res = soc_pb_pp_fwd_decision_in_buffer_build(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
          &em_dest,
          FALSE,
          dest,
          add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_fec_to_em_dest()",0,0);
}

STATIC
  uint32
    soc_pb_pp_frwrd_fec_to_dest(
      SOC_SAND_IN SOC_PB_PP_FEC_ID   fec_id,
      SOC_SAND_OUT uint32      *dest,
      SOC_SAND_OUT uint32      *add_info
    )
{
  SOC_PB_PP_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&em_dest);
  *add_info = 0;

  em_dest.type = SOC_PB_PP_FRWRD_DECISION_TYPE_FEC;
  em_dest.dest_id = fec_id;
  em_dest.additional_info.eei.type = SOC_PB_PP_EEI_TYPE_EMPTY;

  res = soc_pb_pp_fwd_decision_in_buffer_build(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
          &em_dest,
          FALSE,
          dest,
          add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_fec_to_em_dest()",0,0);
}

STATIC
  void
    soc_pb_pp_frwrd_trill_uc_lem_payload_build(
      SOC_SAND_IN  SOC_PB_PP_FEC_ID                 fec_id,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD     *payload
    )
{
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(payload);
   
  soc_pb_pp_frwrd_fec_to_dest(
    fec_id,
    &payload->dest,
    &payload->asd
  );
}



STATIC
  void
    soc_pb_pp_frwrd_trill_mc_lem_payload_build(
      SOC_SAND_IN  uint32                     mc_id,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_PAYLOAD     *payload
    )
{
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(payload);
   
  soc_pb_pp_frwrd_mc_id_to_dest(
    mc_id,
    &payload->dest,
    &payload->asd
  );
}

/* build lem access key for TRILL UC */
STATIC
  void
    soc_pb_pp_frwrd_trill_uc_lem_request_key_build(
      SOC_SAND_IN SOC_PB_PP_LEM_ACCESS_KEY_TYPE type,
      SOC_SAND_IN uint32 nickname,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  SOC_PB_PP_LEM_ACCESS_KEY_clear(key);
  
  key->type = type;

  key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC;

  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = nickname;

  key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC;
  key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC;
}

/* build lem access key for TRILL MC */
uint32
    soc_pb_pp_frwrd_trill_mc_lem_request_key_build(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  SOC_PB_PP_LEM_ACCESS_KEY_TYPE  type,
      SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY   *trill_mc_key,
      SOC_SAND_OUT SOC_PB_PP_LEM_ACCESS_KEY       *key
    )
{
  uint32
    num_bits;
  SOC_PB_PP_TRILL_MC_MASKED_FIELDS
    masked_fields;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_clear(&masked_fields);
  res = soc_pb_pp_frwrd_trill_multicast_key_mask_get_unsafe(
    unit,
    &masked_fields);
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);


  SOC_PB_PP_LEM_ACCESS_KEY_clear(key);

  
  key->type = type;

  key->nof_params = SOC_PB_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC;

   /* dist tree nickname */
  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC;
  key->param[4].nof_bits = (uint8)num_bits;
  key->param[4].value[0] = trill_mc_key->tree_nick;

  /* ingres nickname - should be fitted to mask cfg*/
  if (masked_fields.mask_ing_nickname == TRUE)
  {
    num_bits = 0;
  }
  else if (masked_fields.mask_ing_nickname == FALSE)
  {
    num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC;
  }
  key->param[3].nof_bits = (uint8)num_bits;
  key->param[3].value[0] = trill_mc_key->ing_nick;

  /* fid */
  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC;
  key->param[2].nof_bits = (uint8)num_bits;
  key->param[2].value[0] = trill_mc_key->fid;

  /* adj eep */
  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = trill_mc_key->adjacent_eep;

  /* esadi */
  num_bits = SOC_PB_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = trill_mc_key->esadi;


  key->prefix.nof_bits = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC;
  key->prefix.value = SOC_PB_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_mc_lem_request_key_build()", 0, 0);
}


uint32
  soc_pb_pp_frwrd_trill_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    old_reg_val;
  SOC_PB_PP_REGS
    *regs = NULL;
  SOC_PB_REGS
    *tm_regs = NULL;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  tm_regs = soc_pb_regs();
  regs = soc_pb_pp_regs();
  SOC_PB_PP_FLD_SET(regs->eci.ethernet_type_trill_configuration_reg_reg.ethertype_trill, DEFAULT_TRILL_ETHER_TYPE, 20, exit);

  reg_val = DEFAULT_TRILL_ETHER_TYPE;
  SOC_PB_IMPLICIT_REG_GET(tm_regs->eci.egress_pp_configuration_reg[1], old_reg_val, 60, exit);
  old_reg_val = old_reg_val & 0xffff0000;
  reg_val = reg_val | old_reg_val;
  SOC_PB_IMPLICIT_REG_SET(tm_regs->eci.egress_pp_configuration_reg[1], reg_val, 60, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_init_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_l2_lif_trill_uc_get_internal_unsafe(
    SOC_SAND_IN   int                            unit,
    SOC_SAND_IN   uint32                         nickname,
    SOC_SAND_IN   uint8                          ignore_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID              *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT  uint8                         *is_found
  )
{
  uint32
    res = SOC_SAND_OK,
    add_info = 0;
  uint8
    is_sa_drop = FALSE;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;
  SOC_PB_PP_FRWRD_DECISION_INFO
    em_dest;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&em_dest);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  if (!ignore_key)
  {
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL;
    sem_key.key_info.trill.nick_name = nickname;
    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    res = soc_pb_pp_isem_access_entry_get_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (found)
  {
    *lif_index = sem_entry.sem_result_ndx;

    do
    {
      SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA
        data;

      res = soc_pb_pp_ihp_sem_result_table_trill_tbl_get_unsafe(
            unit,
            *lif_index,
            &data
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      
      res = soc_pb_pp_fwd_decision_in_buffer_parse(
              SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
              data.learn_destination,
              add_info,
              &em_dest,
              &is_sa_drop
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

      trill_info->fec_id = em_dest.dest_id;
      *is_found = TRUE;
      goto exit;
      
    } while(0);
  }
  else
  {
    *is_found = FALSE;
    goto exit;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_trill_uc_get_internal_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_trill_mc_get_internal_unsafe(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   SOC_PB_PP_TRILL_MC_ROUTE_KEY                  *trill_mc_key,
    SOC_SAND_IN   uint8                                 ignore_key,
    SOC_SAND_OUT  uint32                                  *mc_id,
    SOC_SAND_OUT  uint8                                 *is_found
  )
{
  uint32
    res = SOC_SAND_OK,
    add_info = 0;
  uint8
    is_sa_drop = FALSE;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    found;
  SOC_PB_PP_FRWRD_DECISION_INFO
    em_dest;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PB_PP_FRWRD_DECISION_INFO_clear(&em_dest);
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  soc_pb_pp_frwrd_trill_mc_lem_request_key_build(
    unit,
    SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC,
    trill_mc_key,
    &request.key
  );

  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &request.key,
          &payload,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!found)
  {
    *is_found = FALSE;
    goto exit;
  }
  
  res = soc_pb_pp_fwd_decision_in_buffer_parse(
          SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
          payload.dest,
          add_info,
          &em_dest,
          &is_sa_drop
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  *mc_id = em_dest.dest_id;
  *is_found = TRUE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_trill_mc_get_internal_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_l2_lif_trill_add_internal_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       nickname,
    SOC_SAND_IN  uint8                        ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID             lif_index,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  )
{
  uint32
    res = SOC_SAND_OK,
    additional_info = 0x0;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL;
    sem_key.key_info.trill.nick_name = nickname;
    SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
    sem_entry.sem_result_ndx = lif_index;
    sem_entry.sem_op_code = SOC_PB_PP_LIF_OPCODE_NO_COS;
    res = soc_pb_pp_isem_access_entry_add_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

 /*
  * success or overwriting exist entry
  */
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA
      data;

    res = soc_pb_pp_ihp_sem_result_table_trill_tbl_get_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    data.vsi = 0x3fff; /* max */
    data.tt_learn_enable = trill_info->learn_enable;
    data.tpid_profile = 0;
    data.service_type = SOC_PB_PP_RIF_ISEM_RES_SERVICE_TYPE;
    
    soc_pb_pp_frwrd_fec_to_dest(
      trill_info->fec_id,
      &data.learn_destination,
      &additional_info
    );
   
    res = soc_pb_pp_ihp_sem_result_table_trill_tbl_set_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }
  res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_TRILL_NICK
          );

   SOC_SAND_CHECK_FUNC_RESULT(res, 3001, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_l2_lif_trill_add_internal_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the key type of TRILL multicast routes lookup. The
 *     following fields are optional: Ing-Nick-key;
 *     Adjacent-EEP-key; FID-key
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  res = soc_pb_pp_lem_trill_mc_key_build_set(
          unit,
          masked_fields->mask_adjacent_nickname,
          masked_fields->mask_fid,
          masked_fields->mask_ing_nickname
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_key_mask_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRILL_MC_MASKED_FIELDS, masked_fields, 10, exit);
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_key_mask_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_get_verify(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_VERIFY);


  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_key_mask_get_verify()", 0, 0);
}

/*********************************************************************
*     Set the key type of TRILL multicast routes lookup. The
 *     following fields are optional: Ing-Nick-key;
 *     Adjacent-EEP-key; FID-key
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_clear(masked_fields);

  res = soc_pb_pp_lem_trill_mc_key_build_get(
          unit,
          &(masked_fields->mask_adjacent_nickname),
          &(masked_fields->mask_fid),
          &(masked_fields->mask_ing_nickname)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_key_mask_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Map nick-name to a FEC entry ID. Used for forwarding
 *     packets with the nick name as destination to the FEC,
 *     and to associate the FEC as learning information, upon
 *     receiving packets with the Nick-Name as the source
 *     address
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_add_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID            lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);


  /******************************/
  /*       add LEM entry        */
  /******************************/
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;
  soc_pb_pp_frwrd_trill_uc_lem_request_key_build(
    SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC,
    nickname_key,
    &request.key
  );

  soc_pb_pp_frwrd_trill_uc_lem_payload_build(
    trill_info->fec_id,
    &payload
  );

  res = soc_pb_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 
  if(ack.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

  /********************************/
  /*        add ISEM entry        */
  /********************************/
  res = soc_pb_pp_l2_lif_trill_add_internal_unsafe(
          unit,
          nickname_key,
          FALSE,
          lif_index,
          trill_info,
          success
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /*
  * success or overwriting exist entry
  */
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_TRILL_NICK
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_add_unsafe()", nickname_key, 0);
}

uint32
  soc_pb_pp_frwrd_trill_unicast_route_add_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID            lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO  *trill_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(lif_index, SOC_PB_PP_LIF_ID_MAX, SOC_PB_PP_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(trill_info->fec_id, SOC_PB_PP_FEC_ID_MAX, SOC_PB_PP_FEC_ID_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_add_verify()", nickname_key, 0);
}

/*********************************************************************
*     Get mapping of TRILL nickname to FEC ID and LIF index
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        nickname_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID             *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT uint8                        *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  res = soc_pb_pp_l2_lif_trill_uc_get_internal_unsafe(
          unit,
          nickname_key,
          FALSE,
          lif_index,
          trill_info,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_get_unsafe()", nickname_key, 0);
}

uint32
  soc_pb_pp_frwrd_trill_unicast_route_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_get_verify()", nickname_key, 0);
}

/*********************************************************************
*     Remove TRILL nick-name
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack;
  SOC_PB_PP_ISEM_ACCESS_KEY
    sem_key;
  SOC_PB_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNSAFE);

   /***************************************/
  /*         remove LEM entry            */
  /***************************************/
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;
  soc_pb_pp_frwrd_trill_uc_lem_request_key_build(
    SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC,
    nickname_key,
    &request.key
  );
  
  res = soc_pb_pp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  /***************************************/
  /*         remove ISEM entry           */
  /***************************************/
  SOC_PB_PP_CLEAR(&sem_key, SOC_PB_PP_ISEM_ACCESS_KEY, 1);
  sem_key.key_type = SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL;
  sem_key.key_info.trill.nick_name = nickname_key;

  SOC_PB_PP_CLEAR(&sem_entry, SOC_PB_PP_ISEM_ACCESS_ENTRY, 1);
  res = soc_pb_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  *lif_index = (SOC_PB_PP_LIF_ID) sem_entry.sem_result_ndx;

  if (found)
  {
    res = soc_pb_pp_isem_access_entry_remove_unsafe(
            unit,
            &sem_key,
            &found
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
   /*
    * set entry type
    */
    res = soc_pb_pp_sw_db_lif_table_entry_use_set(
            unit,
            *lif_index,
            SOC_PB_PP_LIF_ENTRY_TYPE_EMPTY
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_remove_unsafe()", nickname_key, 0);
}

uint32
  soc_pb_pp_frwrd_trill_unicast_route_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, SOC_PB_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_unicast_route_remove_verify()", nickname_key, 0);
}

/*********************************************************************
*     Map a TRILL distribution tree to a FEC
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_add_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

   /******************************/
  /*       add LEM entry        */
  /******************************/
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;
  soc_pb_pp_frwrd_trill_mc_lem_request_key_build(
    unit,
    SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC,
    trill_mc_key,
    &request.key
  );

  soc_pb_pp_frwrd_trill_mc_lem_payload_build(
    mc_id,
    &payload
  );

  res = soc_pb_pp_lem_access_entry_add_unsafe(
          unit,
          &request,
          &payload,
          &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 
  if(ack.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }
  exit:
  	SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_add_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_multicast_route_add_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRILL_MC_ROUTE_KEY, trill_mc_key, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mc_id, SOC_PB_PP_FRWRD_TRILL_MC_ID_MAX, SOC_PB_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_add_verify()", 0, 0);
}

/*********************************************************************
*     Get Mapping of TRILL distribution tree to a FEC
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                      *mc_id,
    SOC_SAND_OUT uint8                     *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  res = soc_pb_pp_l2_lif_trill_mc_get_internal_unsafe(
          unit,
          trill_mc_key,
          FALSE,
          mc_id,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_multicast_route_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRILL_MC_ROUTE_KEY, trill_mc_key, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_get_verify()", 0, 0);
}

/*********************************************************************
*     Remove a TRILL distribution tree mapping
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);

  /***************************************/
  /*         remove LEM entry            */
  /***************************************/
  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

  request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;
  soc_pb_pp_frwrd_trill_mc_lem_request_key_build(
    unit,
    SOC_PB_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC,
    trill_mc_key,
    &request.key
  );

  res = soc_pb_pp_lem_access_entry_remove_unsafe(
          unit,
          &request,
          &ack
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_remove_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_multicast_route_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRILL_MC_ROUTE_KEY, trill_mc_key, 10, exit);
    
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_multicast_route_remove_verify()", 0, 0);
}

/*********************************************************************
*     Map SA MAC adress to expected adjacent EEP and expected
 *     system port in SA-Based_adj db. Used for authenticating
 *     incoming trill packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_adj_info_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  SOC_PB_PP_FRWRD_DECISION_INFO
    fwd_decision;
  uint8
    is_found;
  uint32
    new_asd;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  request.stamp = 0;
  /* convert to LEM key */
  res = soc_pb_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* set payload */

  /* get previous payload. ASD , as part of the ASD used for VID-assignment */
  /* if key is found and destination is not relevant then keep exist destination payload.dest*/
  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* if remove and not found then done */
  if (!is_found && !enable)
  {
    *success = SOC_SAND_SUCCESS;
    goto exit;
  }

  /* set defaults */
  payload.age = SOC_PB_PP_TRILL_SA_AUTH_ENTRY_AGE;
  payload.is_dynamic = SOC_PB_PP_TRILL_SA_AUTH_ENTRY_IS_DYNAMIC;

  /* set payload for add operation */
  if (enable)
  {
   /*
    * update destination.
    */
    SOC_PB_PP_FRWRD_DECISION_INFO_clear(&fwd_decision);
    if (mac_auth_info->expect_system_port.sys_port_type == SOC_SAND_PP_SYS_PORT_TYPE_LAG)
    {
      fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG;
    }
    else
    {
      fwd_decision.type = SOC_PB_PP_FRWRD_DECISION_TYPE_UC_PORT;
    }

    fwd_decision.dest_id = mac_auth_info->expect_system_port.sys_id;
    fwd_decision.additional_info.outlif.type = SOC_PB_PP_OUTLIF_ENCODE_TYPE_NONE;

    res = soc_pb_pp_fwd_decision_in_buffer_build(
            SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
            &(fwd_decision),
            FALSE, /* not matter, affects ASD, will be overwritten*/
            &(payload.dest),
            &(payload.asd)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    
   /*
    * new ASD
    */
    res = soc_pb_pp_lem_access_trill_sa_based_asd_build(
            unit,
            mac_auth_info,
            &new_asd
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    payload.asd = new_asd;
    /* add entry */
    request.command = SOC_PB_PP_LEM_ACCESS_CMD_INSERT;
    res = soc_pb_pp_lem_access_entry_add_unsafe(
            unit,
            &request,
            &payload,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  else /* found and remove operation*/
  {
    request.command = SOC_PB_PP_LEM_ACCESS_CMD_DELETE;
    res = soc_pb_pp_lem_access_entry_remove_unsafe(
            unit,
            &request,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    if (ack_status.reason == SOC_PB_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
    }
    else
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_adj_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_adj_info_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_SET_VERIFY);

  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, mac_address_key, 10, exit); */
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_TRILL_ADJ_INFO, mac_auth_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_adj_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_adj_info_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET_VERIFY);

  /* SOC_PB_PP_STRUCT_VERIFY(SOC_SAND_PP_MAC_ADDRESS, mac_address_key, 10, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_adj_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Map SA MAC adress to expected adjacent EEP and expected
 *     system port in SA-Based_adj db. Used for authenticating
 *     incoming trill packets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_adj_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                     *enable
  )
{
  SOC_PB_PP_LEM_ACCESS_REQUEST
    request;
  SOC_PB_PP_LEM_ACCESS_PAYLOAD
    payload;
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  SOC_PB_PP_FRWRD_DECISION_INFO
    fwd_decision;
  uint8
    is_found,
    sa_drop;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_ADJ_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(enable);
  
  SOC_PB_PP_TRILL_ADJ_INFO_clear(mac_auth_info);
  /*sal_memset(mac_auth_info, 0x0, sizeof(SOC_PB_PP_TRILL_ADJ_INFO));*/
  mac_auth_info->expect_adjacent_eep = 0;
  soc_sand_SAND_PP_SYS_PORT_ID_clear(&(mac_auth_info->expect_system_port));
  mac_auth_info->expect_system_port.sys_id = 0xffffffff;

  SOC_PB_PP_LEM_ACCESS_REQUEST_clear(&request);
  SOC_PB_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  SOC_PB_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);


  request.stamp = 0;
  /* convert to LEM key */
  res = soc_pb_pp_sa_based_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* get previous payload. ASD , as part of the ASD used for VID-assignment */
  /* if key is found and destination is not relevant then keep exist destination payload.dest*/
  res = soc_pb_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* if not found then done */
  if (!is_found)
  {
    *enable = FALSE;
    goto exit;
  }

  /*found, check if valid*/
  res = soc_pb_pp_lem_access_trill_sa_based_asd_parse(
          unit,
          payload.asd,
          mac_auth_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
 /*
  * get the specific port if needed
  */
  if (mac_auth_info->expect_system_port.sys_id != SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
  {
      res = soc_pb_pp_fwd_decision_in_buffer_parse(
              SOC_PB_PP_FRWRD_DECISION_APPLICATION_TYPE_SA_AUTH,
              payload.dest,
              0, /* Don't matter */
              &(fwd_decision),
              &sa_drop /* not matter, affects ASD, will be overwritten*/
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (fwd_decision.type == SOC_PB_PP_FRWRD_DECISION_TYPE_UC_LAG)
      {
        mac_auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_LAG;
        mac_auth_info->expect_system_port.sys_id = fwd_decision.dest_id;
      }
      else
      {
        mac_auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
        mac_auth_info->expect_system_port.sys_id = fwd_decision.dest_id;
      }
  }

  *enable = TRUE;/*(uint8)!SOC_PB_PP_LLP_SA_AUTH_ACCEPT_ALL(mac_auth_info);*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_adj_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set TRILL global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_global_info_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    reg_val,
    old_reg_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;
   SOC_PB_REGS
    *tm_regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  regs = soc_pb_pp_regs();
  tm_regs = soc_pb_regs();

  SOC_PB_PP_FLD_SET(regs->epni.trill_config_reg.trill_op_cnt, glbl_info->cfg_ttl, 10, exit);

  regs = soc_pb_pp_regs();
  SOC_PB_PP_FLD_SET(regs->eci.ethernet_type_trill_configuration_reg_reg.ethertype_trill, glbl_info->ethertype, 20, exit);

  reg_val = glbl_info->ethertype;
  SOC_PB_IMPLICIT_REG_GET(tm_regs->eci.egress_pp_configuration_reg[1], old_reg_val, 60, exit);
  old_reg_val = old_reg_val & 0xffff0000;
  reg_val = reg_val | old_reg_val;
  SOC_PB_IMPLICIT_REG_SET(tm_regs->eci.egress_pp_configuration_reg[1], reg_val, 60, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_global_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_global_info_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO, glbl_info, 10, exit);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_global_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_trill_global_info_get_verify(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET_VERIFY);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_global_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Set TRILL global attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_global_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_clear(glbl_info);

  regs = soc_pb_pp_regs();

  SOC_PB_PP_FLD_GET(regs->epni.trill_config_reg.trill_op_cnt, glbl_info->cfg_ttl, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_trill_global_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_frwrd_trill module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_trill_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_frwrd_trill;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_frwrd_trill module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_trill_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_frwrd_trill;
}
uint32
  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_verify(
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRILL_MC_MASKED_FIELDS_verify()",0,0);
}

uint32
  SOC_PB_PP_TRILL_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tree_nick, SOC_PB_PP_FRWRD_TRILL_TREE_NICK_MAX, SOC_PB_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid, SOC_PB_PP_FID_MAX, SOC_PB_PP_FID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ing_nick, SOC_PB_PP_FRWRD_TRILL_ING_NICK_MAX, SOC_PB_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->adjacent_eep, SOC_PB_PP_FRWRD_TRILL_ADJACENT_EEP_MAX, SOC_PB_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRILL_MC_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PB_PP_TRILL_ADJ_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->expect_adjacent_eep, SOC_PB_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_MAX, SOC_PB_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR, 10, exit);
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_TRILL_ADJ_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cfg_ttl, SOC_PB_PP_FRWRD_TRILL_CFG_TTL_MAX, SOC_PB_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

