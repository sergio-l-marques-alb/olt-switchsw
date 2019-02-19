/* $Id: pcp_lem_access.c,v 1.13 Broadcom SDK $
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

#include <soc/dpp/PCP/pcp_api_mgmt.h>

#include <soc/dpp/PCP/pcp_lem_access.h>
#include <soc/dpp/PCP/pcp_api_framework.h>
#include <soc/dpp/PCP/pcp_frwrd_mact.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_sw_db.h>

#ifdef USING_CHIP_SIM
  #include <sim/dpp/ChipSim/chip_sim_em.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define PCP_LEM_ACCESS_CMD_MAX                              (PCP_LEM_ACCESS_NOF_CMDS-1)
#define PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4    (2)
#define PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_ILM              (4)

#define PCP_LEM_STAMP_MIN                                   (10)
#define PCP_LEM_STAMP_MAX                                   (250)
#define PCP_LEM_ACCESS_KEY_TYPE_IN_BITS_4                   (4)


/* Payload destination parameter sizes in bits */

/* Polling before writing a CPU request */
#define PCP_LEM_ACCESS_BUSY_WAIT_ITERATIONS                 (50)
#define PCP_LEM_ACCESS_TIMER_ITERATIONS                     (10)
#define PCP_LEM_ACCESS_TIMER_DELAY_MSEC                     (50)

/* Field values */
#define PCP_LEM_ACCCESS_REQ_ORIGIN_CPU_FLD_VAL              (0)
#define PCP_LEM_ACCCESS_REQ_ORIGIN_OLP_FLD_VAL              (1)
#define PCP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS              (3)

#define PCP_LEM_ACCESS_FAIL_REASON_UNKNOWN_VSI_FLD_VAL      (0)
#define PCP_LEM_ACCESS_FAIL_REASON_FID_LIMIT_FLD_VAL        (1)
#define PCP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM_FLD_VAL      (3)

/* extend P2P */
#define PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_EXTEND_P2P       (1)


#define PCP_LEM_INSTRUCTION_CEK_IN_RIF                      { 11,  53,   0,   1,   1}
#define PCP_LEM_INSTRUCTION_CEK_IN_PORT                     {  5,   1,   0,   1,   1}
#define PCP_LEM_INSTRUCTION_CEK_0(n)                        {  n, 107,   0,   1,   1}

    /*
     * Extended
     */
#define PCP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS                  (34)

/* nubmer of bits used for key content, ignore prefix */
#define  PCP_LEM_ACCESS_KEY_LEN  (PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS - 4)

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
  Pcp_procedure_desc_element_lem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_GET_ERRS_PTR),

  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_KEY_ENCODED_BUILD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_PAYLOAD_BUILD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_KEY_ENCODED_PARSE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_REQUEST_SEND),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_REQUEST_ANSWER_RECEIVE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_ENTRY_ADD_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_ENTRY_BY_INDEX_GET_UNSAFE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_AGE_FLD_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_AGE_FLD_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_SA_BASED_ASD_BUILD),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ACCESS_SA_BASED_ASD_PARSE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_LEM_ILM_KEY_BUILD_SET),

   /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Pcp_error_desc_element_lem_access[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    PCP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR,
    "PCP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR",
    "In the key construction, a parameter has a size out of range. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR,
    "PCP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR",
    "In the key construction, the sum of the parameter size \n\r "
    "with the prefix size is greater than the maximal key size. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR,
    "PCP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR",
    "The retrieved key has an unknown key prefix. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
    {
      PCP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR,
    "PCP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR",
    "'entry_ndx' is out of range. \n\r "
    "The range is: 0 to 64K+32-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR",
    "'command' is out of range. \n\r "
    "The range is: 0 to PCP_LEM_ACCESS_NOF_CMDS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_STAMP_OUT_OF_RANGE_ERR,
    "PCP_LEM_STAMP_OUT_OF_RANGE_ERR",
    "'stamp' is out of range. \n\r "
    "The stamp must be not set (set to zero).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR",
    "'type' is out of range. \n\r "
    "The range is: 0 to PCP_LEM_ACCESS_NOF_KEY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR",
    "'nof_params' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR",
    "'nof_bits' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR",
    "'prefix' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR",
    "'nof_bits' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR",
    "'value' is out of range. \n\r "
    "The range is set according to the type.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ASD_OUT_OF_RANGE_ERR,
      "PCP_LEM_ASD_OUT_OF_RANGE_ERR",
    "'asd' is out of range. \n\r "
    "The range is: 0 to 1<<24-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_AGE_OUT_OF_RANGE_ERR,
      "PCP_LEM_AGE_OUT_OF_RANGE_ERR",
    "'age' is out of range. \n\r "
    "The range is: 0 to 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_DEST_OUT_OF_RANGE_ERR,
      "PCP_LEM_DEST_OUT_OF_RANGE_ERR",
    "'dest' is out of range. \n\r "
    "The range is: 0 to 1<<16-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
    {
      PCP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR,
    "PCP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR",
    "The polling on the bit indicating \n\r "
    "the end of the lookup has failed.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR",
    "'req_origin' is out of range. \n\r "
    "The range is: 0 to PCP_LEM_ACCCESS_NOF_REQ_ORIGINS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR,
      "PCP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR",
    "'reason' is out of range. \n\r "
    "The range is: 0,1 and 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR,
    "PCP_LEM_ACCESS_SA_BASED_ILLEGAL_VID_ERR",
    " try to build ASD according to both SA-auth \n\r "
    " and VID-assign while VID value is not equal in both.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_LEM_ACCESS_PROGRAM_NOT_FOUND,
    "PCP_LEM_ACCESS_PROGRAM_NOT_FOUND",
    " the requested FLP program was not found\n\r "
    " in the FLP key program map.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },



  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

static uint32
  Pcp_lem_actual_stamp[SOC_SAND_MAX_DEVICE] = {PCP_LEM_STAMP_MIN};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  pcp_lem_access_age_fld_get(
      SOC_SAND_IN  int  unit,
      SOC_SAND_IN  uint32   age_fld,
      SOC_SAND_OUT uint32   *age
    )
{
  uint32
    age_lcl,
    fld_val,
    res = SOC_SAND_OK;
  PCP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_AGE_FLD_SET);

  SOC_SAND_CHECK_NULL_INPUT(age);

  regs = pcp_regs();

  /*
   *	Get the resolution (high / low)
   */
  PCP_FLD_GET(regs->elk.age_aging_resolution_reg.age_aging_resolution, fld_val, 10, exit);

  if (fld_val == 0)
  {
    /*
     *	Low resolution: Age mapped to (2,1,0,0)
     */
    if (age_fld != 0)
    {
      age_lcl = (age_fld % 4) - 1;
    }
    else
    {
      age_lcl = 0;
    }
  }
  else
  {
    /*
     *	High resolution: Age mapped to (6,5,4,2,6,1,0,0)
     */
    switch(age_fld)
    {
    case 0:
    case 1:
      age_lcl = 0;
      break;

    case 2:
      age_lcl = 1;
      break;

    case 3:
    case 7:
      age_lcl = 6;
      break;

    case 4:
      age_lcl = 2;
      break;

    case 5:
      age_lcl = 4;
      break;

    case 6:
      age_lcl = 5;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_LEM_AGE_OUT_OF_RANGE_ERR, 20, exit);
    }
  }

  *age = age_lcl;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_age_fld_set()", age_fld, 0);
}

STATIC uint32
  pcp_lem_access_actual_stamp_get(
      SOC_SAND_IN int  unit
    )
{
  return Pcp_lem_actual_stamp[unit];
}

STATIC uint32
  pcp_lem_access_next_stamp_get(
      SOC_SAND_IN int  unit
    )
{
  Pcp_lem_actual_stamp[unit] ++;
  if (Pcp_lem_actual_stamp[unit] == PCP_LEM_STAMP_MAX)
  {
    Pcp_lem_actual_stamp[unit] = PCP_LEM_STAMP_MIN;
  }
  return Pcp_lem_actual_stamp[unit];
}


uint32
  pcp_lem_access_payload_parse(
      SOC_SAND_IN   int                   unit,
      SOC_SAND_IN   uint32                    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S],
      SOC_SAND_OUT  PCP_LEM_ACCESS_PAYLOAD   *payload
    )
{
  uint32
    tmp;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_PAYLOAD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload);

  res = soc_sand_bitstream_get_any_field(
          payload_data,
          0,
          PCP_LEM_ACCESS_DEST_NOF_BITS,
          &(payload->dest)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          payload_data,
          PCP_LEM_ACCESS_DEST_NOF_BITS,
          PCP_LEM_ACCESS_ASD_NOF_BITS,
          &(payload->asd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = 0;
  res = soc_sand_bitstream_get_any_field(
          payload_data,
          PCP_LEM_ACCESS_DEST_NOF_BITS + PCP_LEM_ACCESS_ASD_NOF_BITS,
          1,
          &tmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  payload->is_dynamic = (uint8)tmp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_payload_build()", 0, 0);
}


uint32
  pcp_lem_access_payload_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD   *payload,
    SOC_SAND_OUT uint32                    payload_data[PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S]
  )
{
  uint32
    payload_ndx;
  uint32
    tmp;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_PAYLOAD_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(payload_data);

  for (payload_ndx = 0; payload_ndx < PCP_LEM_ACCESS_PAYLOAD_NOF_UINT32S; ++payload_ndx)
  {
    payload_data[payload_ndx] = 0;
  }

  res = soc_sand_bitstream_set_any_field(
          &(payload->dest),
          0,
          PCP_LEM_ACCESS_DEST_NOF_BITS,
          payload_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_set_any_field(
          &(payload->asd),
          PCP_LEM_ACCESS_DEST_NOF_BITS,
          PCP_LEM_ACCESS_ASD_NOF_BITS,
          payload_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tmp = payload->is_dynamic;
  res = soc_sand_bitstream_set_any_field(
          &tmp,
          PCP_LEM_ACCESS_ASD_NOF_BITS + PCP_LEM_ACCESS_DEST_NOF_BITS,
          1,
          payload_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_payload_build()", 0, 0);
}

uint32
  pcp_lem_key_extend(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  PCP_LEM_ACCESS_KEY             *key,
     SOC_SAND_OUT PCP_LEM_ACCESS_KEY             *extnd_key
  )
{
  uint32 
    bits_sum=0, param_ndx,
    new_param_indx=0;
  uint8 mask_inrif=0,mask_port=0;
  uint32
	  ilm_key_mask_bitmap = 0;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* calc number bits already in use */
  for (param_ndx = 0; param_ndx < key->nof_params; ++param_ndx)
  {
    bits_sum += key->param[param_ndx].nof_bits;
  }
  /* next param to use */
  new_param_indx = key->nof_params;

  /* copy key */
  sal_memcpy(extnd_key,key, sizeof(PCP_LEM_ACCESS_KEY));

  /* fix according to type */
  switch (key->type)
  {
    /* for TRILL UC duplicate ENICK x 2 times + 11 lsb bits */
  case PCP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
    extnd_key->nof_params += 3;
    /* fill keys till last */
    for (; new_param_indx < (uint8)(extnd_key->nof_params-1); ++new_param_indx)
    {
      extnd_key->param[new_param_indx].nof_bits = key->param[0].nof_bits;
      extnd_key->param[new_param_indx].value[0] = key->param[0].value[0];
      bits_sum += extnd_key->param[new_param_indx].nof_bits;
    }
    /* last param set with rest of bits*/
    extnd_key->param[new_param_indx].nof_bits = (uint8)(PCP_LEM_ACCESS_KEY_LEN - bits_sum);
    extnd_key->param[new_param_indx].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[new_param_indx].nof_bits-1,0);
  break;
  /* for IP host if no vrf in system duplicate 27 lsb of DIP */
  case PCP_LEM_ACCESS_KEY_TYPE_IP_HOST:
    /* depends on nof-vrfs, if nof-vrfs is zero, then key is DIP,DIP */
    /*res = soc_pb_pp_sw_db_ipv4_nof_vrfs_get(unit, &nof_vrfs);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);*/
    /* not new param but overwrite last parameter (VRF with DIP) */
    --new_param_indx;
    bits_sum -= key->param[new_param_indx].nof_bits;

    extnd_key->param[new_param_indx].nof_bits = (uint8) (PCP_LEM_ACCESS_KEY_LEN - bits_sum);
    /* duplicate lsb of param 0 */
    extnd_key->param[new_param_indx].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[new_param_indx].nof_bits-1,0);
  break;
  /* for extended lookup add new param with first param 25 lsb*/
  case PCP_LEM_ACCESS_KEY_TYPE_EXTENDED:
    extnd_key->nof_params += 1;
    extnd_key->param[new_param_indx].nof_bits = (uint8) (PCP_LEM_ACCESS_KEY_LEN - bits_sum);
    extnd_key->param[new_param_indx].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[new_param_indx].nof_bits-1,0);
    break;

  /* for ILM, depend on masking if port/rif masked then use bits from label instead */
  case PCP_LEM_ACCESS_KEY_TYPE_ILM:
    /* label and exp */
    ilm_key_mask_bitmap = soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_get(unit);
	mask_inrif  = SOC_SAND_GET_BIT(ilm_key_mask_bitmap,PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_INRIF);
	mask_port = SOC_SAND_GET_BIT(ilm_key_mask_bitmap,PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_PORT);
    /* instead RIF set part of label */
    if (mask_inrif)
    {
      extnd_key->param[3].value[0] = SOC_SAND_GET_BITS_RANGE(key->param[0].value[0],extnd_key->param[3].nof_bits-1,0);
    }
    /* if port is masked then take label instead*/
    if (mask_port)
    {
      extnd_key->param[2].value[0] = key->param[0].value[0];
      extnd_key->param[2].nof_bits = key->param[0].nof_bits;
    }
  break;
  default:
  break;
  }

 PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_key_extend()", 0, 0);
}



uint32
  pcp_lem_key_encoded_build(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  PCP_LEM_ACCESS_KEY             *key_in,
     SOC_SAND_OUT PCP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer
  )
{
  uint32
    current_param_lsb,
    res = SOC_SAND_OK,
    buffer_lcl[PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S];
  uint32
    param_ndx,
    buffer_ndx;
  PCP_LEM_ACCESS_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_KEY_ENCODED_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(key_in);
  SOC_SAND_CHECK_NULL_INPUT(key_in_buffer);

  res = pcp_lem_key_extend(unit,key_in,&key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Initialization
   */
  for (buffer_ndx = 0; buffer_ndx < PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S; ++buffer_ndx)
  {
    buffer_lcl[buffer_ndx] = 0;
  }

  /*
   *	Copy of the parameters
   */
  current_param_lsb = 0;
  for (param_ndx = 0; param_ndx < key.nof_params; ++param_ndx)
  {
    if (((key.param[param_ndx].nof_bits == 0) && (key.prefix.value != PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC))  || (key.param[param_ndx].nof_bits > PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS))
    {
      SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR, 10, exit);
    }
    res = soc_sand_bitstream_set_any_field(
            key.param[param_ndx].value,
            current_param_lsb,
            key.param[param_ndx].nof_bits,
            buffer_lcl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    current_param_lsb += key.param[param_ndx].nof_bits;
  }

  /*
   *	Copy of the prefix and verification of its length
   */
  if (current_param_lsb + key.prefix.nof_bits > PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_PREFIX_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          &(key.prefix.value),
          PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS - key.prefix.nof_bits,
          key.prefix.nof_bits,
          buffer_lcl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (buffer_ndx = 0; buffer_ndx < PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S; ++buffer_ndx)
  {
    key_in_buffer->buffer[buffer_ndx] = buffer_lcl[buffer_ndx];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_key_encoded_build()", 0, 0);
}

uint32
  pcp_lem_key_encoded_parse(
     SOC_SAND_IN  int                        unit,
     SOC_SAND_IN  PCP_LEM_ACCESS_KEY_ENCODED     *key_in_buffer,
     SOC_SAND_OUT PCP_LEM_ACCESS_KEY             *key
  )
{
  uint32
    curr_bit_lsb,
    key_type_4_msb = 0,
    ilm_key_mask_bitmap= 0x0,
    res = SOC_SAND_OK;
  uint32
    param_ndx;
  uint8 mask_inrif=0,mask_port=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_KEY_ENCODED_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(key_in_buffer);

  PCP_LEM_ACCESS_KEY_clear(key);

  /*
   *	Find the right type (encoded in the four msb of the buffer)
   */
  res = soc_sand_bitstream_get_any_field(
          key_in_buffer->buffer,
          PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS - PCP_LEM_ACCESS_KEY_TYPE_IN_BITS_4,
          PCP_LEM_ACCESS_KEY_TYPE_IN_BITS_4,
          &(key_type_4_msb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


  if (SOC_SAND_GET_BIT(key_type_4_msb, 3) == 0x1)
  {
    key_type_4_msb = PCP_LEM_ACCESS_KEY_TYPE_PREFIX_1;
  }

  switch(key_type_4_msb)
  {
    /*
     *	Backbone MAC
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0001:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_BACKBONE_MAC;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

    /*
     *	IPv4 Compatible Multicast
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0010:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IPV4_MC;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

    /*
     *	IPv4 Host
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0101:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_IP_HOST;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_IP_HOST;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;
    
    /*
     *	trill UC
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0100:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC;
    key->type = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC;
    key->param[1].nof_bits = 0;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

    /*
     *	trill MC - No support for trill in ELK.
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0000:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC;
    key->type = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC;
    key->param[2].nof_bits = PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC;
    key->param[3].nof_bits = PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC;
    key->param[4].nof_bits = PCP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC;
    break;
    
    /*
     *	SA AUTH
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0111:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_SA_AUTH;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SA_AUTH;
    key->param[1].nof_bits = 0;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

   /*
    *	ILM
    */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0110:
	ilm_key_mask_bitmap = soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_get(unit);
	mask_inrif  = SOC_SAND_GET_BIT(ilm_key_mask_bitmap,PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_INRIF);
	mask_port = SOC_SAND_GET_BIT(ilm_key_mask_bitmap,PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_PORT);
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_ILM;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_ILM;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_ILM;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM;
    /* if port is masked then there is 20 bits to skip */
    if (mask_port)
    {
      key->param[2].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM;
    }
    else
    {
      key->param[2].nof_bits = PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
    }
    /* this can be 12 bits of RIF or of label */
    key->param[3].nof_bits = PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
    key->param[4].nof_bits = 0;
    break;
    /*
     *	MAC
     */
  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_1:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_MAC;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_MAC;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_MAC;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC;
    key->param[0].nof_bits = PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC;
    key->param[1].nof_bits = PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC;
    key->param[2].nof_bits = 0;
    key->param[3].nof_bits = 0;
    key->param[4].nof_bits = 0;
    break;

  case PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0011:
    key->nof_params = PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_EXTENDED;
    key->type = PCP_LEM_ACCESS_KEY_TYPE_EXTENDED;
    key->prefix.value = PCP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED;
    key->prefix.nof_bits = PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED;
    key->param[0].nof_bits = PCP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS;
    break;
  

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_UNKNOWN_KEY_PREFIX_ERR, 20, exit);
  }

  /*
   *	Extract the fields from the buffer
   */
  curr_bit_lsb = 0;
  for (param_ndx = 0; param_ndx < key->nof_params; ++param_ndx)
  {
    if (
        ((key->param[param_ndx].nof_bits == 0) && key->prefix.value != PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC)
        || (key->param[param_ndx].nof_bits > PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS)
        || (curr_bit_lsb > PCP_LEM_ACCESS_KEY_MAX_SIZE_IN_BITS)
       )
    {
      SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR, 30, exit);
    }
    res = soc_sand_bitstream_get_any_field(
            key_in_buffer->buffer,
            curr_bit_lsb,
            key->param[param_ndx].nof_bits,
            key->param[param_ndx].value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    curr_bit_lsb += key->param[param_ndx].nof_bits;
  }

  /*
   *	Last verification of the key format
   */
  for (param_ndx = key->nof_params; param_ndx < PCP_LEM_KEY_MAX_NOF_PARAMS; ++param_ndx)
  {
    if (key->param[param_ndx].nof_bits != 0)
    {
      SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_PARAM_SIZE_OUT_OF_RANGE_ERR, 50, exit);
    }
  }

  /* fix content */
  if (key_type_4_msb == PCP_LEM_ACCESS_KEY_TYPE_PREFIX_0110)
  {
    if (mask_port)
    {
      key->param[2].nof_bits = PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM;
      key->param[2].value[0] = 0;
    }
    if (mask_inrif)
    {
      key->param[3].nof_bits = PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM;
      key->param[3].value[0] = 0;
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_key_encoded_parse()", key_type_4_msb, 0);
}

/*
 *	Write a request into the Request FIFO
 */
STATIC uint32
  pcp_lem_request_send(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_IN  PCP_LEM_ACCESS_OUTPUT         *request_all,
      SOC_SAND_OUT uint8                       *is_poll_success
    )
{
  uint32
    fld_val,
    reg_val,
    reg_val_key = 0,
    reg_val_key2 = 0,
    tmp,
    reg_vals[3],
    res = SOC_SAND_OK;
  PCP_POLL_INFO
    poll_info;
  uint8
    is_key,
    db_profile,
    poll_success;
  PCP_REGS
    *regs;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
#ifdef USING_CHIP_SIM
  uint32
    key[PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S],
    data[PCP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
  uint8
    success;
#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_REQUEST_SEND);

  SOC_SAND_CHECK_NULL_INPUT(request_all);

  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  regs = pcp_regs();

  /*
   *	Polling to verify no CPU request is waiting
   */
  PCP_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = PCP_LEM_ACCESS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = PCP_LEM_ACCESS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = PCP_LEM_ACCESS_TIMER_DELAY_MSEC;

  res = pcp_status_fld_poll_unsafe(
          unit,
          &(regs->elk.cpu_request_trigger_reg.cpu_request_trigger),
          PCP_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (poll_success == FALSE)
  {
    *is_poll_success = poll_success;
    PCP_DO_NOTHING_AND_EXIT;
  }
  else
  {
    *is_poll_success = poll_success;
  }

  /*
   *	Insert the request details
   */
  /*PCP_REG_GET(regs->elk.cpu_request_request_reg_3, reg_val, 20, exit);*/
  reg_val = 0;

  /*
   *	Request origin: CPU or OLP
   */
  switch(request_all->req_origin)
  {
  case PCP_LEM_ACCCESS_REQ_ORIGIN_CPU:
    fld_val = PCP_LEM_ACCCESS_REQ_ORIGIN_CPU_FLD_VAL;
    break;

  case PCP_LEM_ACCCESS_REQ_ORIGIN_OLP:
    fld_val = PCP_LEM_ACCCESS_REQ_ORIGIN_OLP_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR, 25, exit);
  }
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_3.mact_req_qualifier, fld_val, reg_val,  30, exit);

  /*
   *	Entry self-learnt
   */
  fld_val = request_all->is_learned_first_by_me;
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_3.mact_req_self, fld_val, reg_val, 35, exit);

  /*
   *	No need to set the age (overridden)
   */
  fld_val = 7;
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_3.mact_req_age_payload, fld_val, reg_val, 40, exit);
  fld_val = request_all->payload.is_dynamic;
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_3.mact_req_payload_is_dynamic, fld_val, reg_val, 45, exit);

  /*
   *	ASD
   */
  fld_val = request_all->payload.asd >> PCP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS;
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_3.mact_req_payload_eei , fld_val, reg_val, 50, exit);
  PCP_REG_SET(regs->elk.cpu_request_reg_3, reg_val, 55, exit);

  reg_val = 0;
  fld_val = request_all->payload.asd % (1 << PCP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS);
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_2.mact_req_payload_eei, fld_val, reg_val, 65, exit);

  /*
   *	Destination
   */
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_2.mact_req_payload_destination, request_all->payload.dest, reg_val, 70, exit);

  /*
   *	Stamp
   */
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_2.mact_req_stamp, request_all->request.stamp, reg_val, 75, exit);

  /*
   *	Command type
   */
  switch(request_all->request.command)
  {
  case PCP_LEM_ACCESS_CMD_DELETE:
    fld_val = PCP_LEM_ACCESS_CMD_DELETE_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_INSERT:
    fld_val = PCP_LEM_ACCESS_CMD_INSERT_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_REFRESH:
    fld_val = PCP_LEM_ACCESS_CMD_REFRESH_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_LEARN:
    fld_val = PCP_LEM_ACCESS_CMD_LEARN_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_DEFRAG:
    fld_val = PCP_LEM_ACCESS_CMD_DEFRAG_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_ACK:
    fld_val = PCP_LEM_ACCESS_CMD_ACK_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_TRANSPLANT:
    fld_val = PCP_LEM_ACCESS_CMD_TRANSPLANT_FLD_VAL;
    break;
  case PCP_LEM_ACCESS_CMD_ERROR:
    fld_val = PCP_LEM_ACCESS_CMD_ERROR_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 80, exit);
  }
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_2.mact_req_command, fld_val, reg_val, 85, exit);

  fld_val = 0x0;
  PCP_FLD_TO_REG(regs->elk.cpu_request_reg_2.mact_req_part_of_lag, fld_val, reg_val, 90, exit);

  /*
   *	Key insertion (or (System-VSI, MAC))
   */

  res = pcp_lem_key_encoded_build(
          unit,
          &(request_all->request.key),
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

  /*
   *	No insertion of System-VSI mode
   */

  PCP_CLEAR(reg_vals,uint32,2);

  /*if (request_all->request.key.type == PCP_LEM_ACCESS_KEY_TYPE_MAC)*/
  {
    is_key = TRUE;
    tmp = is_key;
    soc_sand_bitstream_set_any_field(&tmp,0,1,reg_vals);
  }

  soc_sand_bitstream_set_any_field(key_in_buffer.buffer,2,62,reg_vals);
  /* msb has to be 1, to match DB-ID */
  tmp = 1;
  /*soc_sand_bitstream_set_any_field(&tmp,63,1,reg_vals);*/

  reg_val_key = reg_vals[0];
  reg_val_key2 = reg_vals[1];

  db_profile = 1;

  if (request_all->request.key.type == PCP_LEM_ACCESS_KEY_TYPE_MAC)
  {
    fld_val = db_profile;
    PCP_FLD_TO_REG(regs->elk.cpu_request_reg_2.mact_req_key_db, fld_val, reg_val, 100, exit);
  }

  PCP_REG_SET(regs->elk.cpu_request_reg_2, reg_val, 105, exit);

  PCP_REG_SET(regs->elk.cpu_request_reg_1, reg_val_key2, 110, exit);

  /*
   *	Writing in this register is a trigger to send the request.
   */
  PCP_REG_SET(regs->elk.cpu_request_reg_0, reg_val_key, 115, exit);
  PCP_FLD_SET(regs->elk.cpu_request_trigger_reg.cpu_request_trigger, 0x1, 120, exit);
  

#ifdef USING_CHIP_SIM
  sal_memset(key, 0x0, CHIP_SIM_LEM_KEY_SIZE) ;
  sal_memset(data, 0x0, CHIP_SIM_LEM_DATA_SIZE) ;

  key[0] = key_in_buffer.buffer[0];
  key[1] = key_in_buffer.buffer[1];
  if (request_all->request.command == PCP_LEM_ACCESS_CMD_DELETE)
  {
    /*
     *	Deletion command
     */
    res = chip_sim_exact_match_entry_remove_unsafe(
            unit,
            CHIP_SIM_LEM_ADDR_BASE,
            key,
            CHIP_SIM_LEM_KEY_SIZE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    *is_poll_success = TRUE;
  }
  else
  {
    /*
     *	Insertion command
     */
    res = pcp_lem_access_payload_build(
            unit,
            &(request_all->payload),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    res = chip_sim_exact_match_entry_add_unsafe(
            unit,
            CHIP_SIM_LEM_ADDR_BASE,
            key,
            CHIP_SIM_LEM_KEY_SIZE,
            data,
            CHIP_SIM_LEM_DATA_SIZE,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    *is_poll_success = success;
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_request_send()", 0, 0);
}

/*
 *  Parse access output
 */
uint32
  pcp_lem_access_parse(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  PCP_LEM_ACCESS_BUFFER          *buffer,
      SOC_SAND_OUT PCP_LEM_ACCESS_OUTPUT          *request_all,
      SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS      *ack_status
    )
{
  uint32
    fld_val,
    reg_val2,
    reg_val,
    reg_val_key[2],
    res = SOC_SAND_OK;
  uint8
    is_key,
    db_profile;
  PCP_REGS
    *regs;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(buffer);
  SOC_SAND_CHECK_NULL_INPUT(request_all);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  regs = pcp_regs();

  reg_val = buffer->data[3];
  reg_val2 = buffer->data[2];
  reg_val_key[1] = buffer->data[1];
  reg_val_key[0] = buffer->data[0];

  /*
   *	Stamp
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_stamp , fld_val, reg_val2, 30, exit);
  request_all->stamp = fld_val;

  /*
   *	Request origin: CPU or OLP
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_qualifier, fld_val, reg_val,  30, exit);
  switch(fld_val)
  {
  case PCP_LEM_ACCCESS_REQ_ORIGIN_CPU_FLD_VAL:
    request_all->req_origin = PCP_LEM_ACCCESS_REQ_ORIGIN_CPU;
    break;

  case PCP_LEM_ACCCESS_REQ_ORIGIN_OLP_FLD_VAL:
    request_all->req_origin = PCP_LEM_ACCCESS_REQ_ORIGIN_OLP;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCCESS_REQ_ORIGIN_OUT_OF_RANGE_ERR, 25, exit);
  }

  /*
   *	Entry self-learnt
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_self, fld_val, reg_val, 35, exit);
  request_all->is_learned_first_by_me = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	Get the ACK status
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_success, fld_val, reg_val, 37, exit);
  if (fld_val == 0x1)
  {
    ack_status->is_success = TRUE;
    ack_status->reason = PCP_LEM_ACCESS_NOF_FAIL_REASONS;
  }
  else
  {
    ack_status->is_success = FALSE;
    PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_reason, fld_val, reg_val, 38, exit);
    switch(fld_val)
    {
    case PCP_LEM_ACCESS_FAIL_REASON_UNKNOWN_VSI_FLD_VAL:
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_FID_UNKNOWN;
      break;

    case PCP_LEM_ACCESS_FAIL_REASON_FID_LIMIT_FLD_VAL:
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_FID_LIMIT;
      break;

    case PCP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM_FLD_VAL:
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_FAIL_REASON_OUT_OF_RANGE_ERR, 43, exit);
    }
  }

  /*
   *	Computation of the good aging payload according to the system configuration
   */

  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_age_payload, fld_val, reg_val, 45, exit);
  res = pcp_lem_access_age_fld_get(
          unit,
          fld_val,
          &(request_all->payload.age)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_payload_is_dynamic, fld_val, reg_val, 46, exit);
  request_all->payload.is_dynamic = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	ASD
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_3.mact_req_payload_eei, fld_val, reg_val, 50, exit);
  request_all->payload.asd = fld_val << PCP_LEM_ACCESS_ASD_FIRST_PART_NOF_BITS;

  reg_val = reg_val2;
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_payload_eei, fld_val, reg_val, 65, exit);
  request_all->payload.asd +=  fld_val;

  /*
   *	Destination
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_payload_destination, request_all->payload.dest, reg_val, 70, exit);

  /*
   * Part of LAG
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_part_of_lag, fld_val, reg_val, 80, exit);
  request_all->is_part_of_lag = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *	Command type
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_command, fld_val, reg_val, 85, exit);
  switch(fld_val)
  {
  case PCP_LEM_ACCESS_CMD_DELETE_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_DELETE;
    break;
  case PCP_LEM_ACCESS_CMD_INSERT_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_INSERT;
    break;
  case PCP_LEM_ACCESS_CMD_REFRESH_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_REFRESH;
    break;
  case PCP_LEM_ACCESS_CMD_LEARN_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_LEARN;
    break;
  case PCP_LEM_ACCESS_CMD_DEFRAG_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_DEFRAG;
    break;
  case PCP_LEM_ACCESS_CMD_ACK_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_ACK;
    break;
  case PCP_LEM_ACCESS_CMD_TRANSPLANT_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_TRANSPLANT;
    break;
  case PCP_LEM_ACCESS_CMD_ERROR_FLD_VAL:
    request_all->request.command = PCP_LEM_ACCESS_CMD_ERROR;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 80, exit);
  }

  /*
   *	Key insertion (or (System-VSI, MAC))
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_key_db, fld_val, reg_val, 100, exit);
  db_profile = SOC_SAND_NUM2BOOL(fld_val);

  /* The PCP uses  64:2 from the register to key_in_buffer (LEM key) */
  is_key = TRUE;

  if (is_key == FALSE)
  {
    /*
     *  Copy bits 64:1 from the register to key_in_buffer (System-VSI & MAC)
     */
    key_in_buffer.buffer[0] = SOC_SAND_GET_BITS_RANGE(reg_val_key[0], 31, 1) + (SOC_SAND_GET_BIT(reg_val_key[1], 0) << 31);
    key_in_buffer.buffer[1] = SOC_SAND_GET_BITS_RANGE(reg_val_key[1], 31, 1) /*+ (db_profile << 31)*/;
  }
  else
  {
    /*
     *  Copy bits 64:2 from the register to key_in_buffer (LEM key)
     */
    res = soc_sand_bitstream_get_any_field(reg_val_key,2,62,key_in_buffer.buffer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
    key_in_buffer.buffer[1] |= (db_profile << 30);
  }
  
  res = pcp_lem_key_encoded_parse(
          unit,
          &key_in_buffer,
          &(request_all->request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

#ifdef USING_CHIP_SIM
  request_all->request.command = PCP_LEM_ACCESS_CMD_ACK;
  ack_status->is_success = TRUE;
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_parse()", 0, 0);
}


/*
 *	Read a request answer
 */
STATIC uint32
  pcp_lem_request_answer_receive(
      SOC_SAND_IN  int                        unit,
      SOC_SAND_OUT PCP_LEM_ACCESS_OUTPUT          *request_all,
      SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS      *ack_status
    )
{
  uint32
    fld_val,
    reg_interrupt,
    res = SOC_SAND_OK;
  PCP_REGS
    *regs;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
  PCP_LEM_ACCESS_BUFFER
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_REQUEST_ANSWER_RECEIVE);

  SOC_SAND_CHECK_NULL_INPUT(request_all);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
  PCP_LEM_ACCESS_BUFFER_clear(&buffer);

  regs = pcp_regs();

  /*
   *  Read registers into access buffer
   */
  PCP_REG_GET(regs->elk.crf_event_reg_3, buffer.data[3], 20, exit);
  PCP_REG_GET(regs->elk.crf_event_reg_2, buffer.data[2], 20, exit);
  PCP_REG_GET(regs->elk.crf_event_reg_1, buffer.data[1], 20, exit);
  PCP_REG_GET(regs->elk.crf_event_reg_0, buffer.data[0], 20, exit);

  /*
   *  Parse access buffer contents
   */
  res = pcp_lem_access_parse(
          unit,
          &buffer,
          request_all,
          ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Read the stamp to check it is the right request
   */
  PCP_FLD_FROM_REG(regs->elk.crf_event_reg_2.mact_req_stamp, fld_val, buffer.data[2], 20, exit);

  if (fld_val != pcp_lem_access_actual_stamp_get(unit))
  {
    ack_status->is_success = FALSE;
    ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_WRONG_STAMP;
  }

  /*
   *	Update the ACK status.
   */
  if (ack_status->reason == PCP_LEM_ACCESS_FAIL_REASON_EMC_PROBLEM)
  {
    /*
     *	Read the interrupts to know the error reason - an OLP interference is possible
     */
    PCP_REG_GET(regs->elk.interrupt_reg, reg_interrupt, 30, exit);

    PCP_FLD_FROM_REG(regs->elk.interrupt_reg.error_cam_table_full, fld_val, reg_interrupt, 40, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_CAM_FULL;
    }

    PCP_FLD_FROM_REG(regs->elk.interrupt_reg.error_delete_unknown_key, fld_val, reg_interrupt, 41, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN;
    }

    PCP_FLD_FROM_REG(regs->elk.interrupt_reg.error_reached_entry_limit, fld_val, reg_interrupt, 42, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_MACT_FULL;
    }

    PCP_FLD_FROM_REG(regs->elk.interrupt_reg.error_learn_over_static, fld_val, reg_interrupt, 43, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_LEARN_STATIC;
    }

    PCP_FLD_FROM_REG(regs->elk.interrupt_reg.error_refresh_over_static, fld_val, reg_interrupt, 44, exit);
    if (fld_val != 0x0)
    {
      ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_CHANGE_STATIC;
    }
  }

exit:
#ifdef USING_CHIP_SIM
  ack_status->is_success = TRUE;
#endif
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_request_answer_receive()", 0, 0);
}

/*********************************************************************
 *     Add an entry to the Exact match table.
 *********************************************************************/
uint32
  pcp_lem_access_entry_add_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS      *ack_status
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_OUTPUT
    request_hw,
    request_hw_rcv;
  uint8
    poll_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_ENTRY_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(request);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  PCP_LEM_ACCESS_OUTPUT_clear(&request_hw);
  PCP_LEM_ACCESS_OUTPUT_clear(&request_hw_rcv);

  /*
   *	Verify the inputs
   */
  PCP_STRUCT_VERIFY(PCP_LEM_ACCESS_REQUEST, request, 10, exit);
  PCP_STRUCT_VERIFY(PCP_LEM_ACCESS_PAYLOAD, payload, 20, exit);

  /*
   *	Build the HW request
   */
  PCP_COPY(&(request_hw.request), request, PCP_LEM_ACCESS_REQUEST, 1);
  PCP_COPY(&(request_hw.payload), payload, PCP_LEM_ACCESS_PAYLOAD, 1);
  request_hw.is_learned_first_by_me = TRUE;
  request_hw.req_origin = PCP_LEM_ACCCESS_REQ_ORIGIN_CPU;
  request_hw.request.stamp = pcp_lem_access_next_stamp_get(unit);

  if ((request_hw.request.command != PCP_LEM_ACCESS_CMD_INSERT)
    &&(request_hw.request.command != PCP_LEM_ACCESS_CMD_LEARN)
    &&(request_hw.request.command != PCP_LEM_ACCESS_CMD_REFRESH)
    &&(request_hw.request.command != PCP_LEM_ACCESS_CMD_TRANSPLANT))
  {
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 25, exit);
  }

  /*
   *	Send the request
   */
  res = pcp_lem_request_send(
          unit,
          &request_hw,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Case of an unsent request
   */
  if (poll_success == FALSE)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT;
    PCP_DO_NOTHING_AND_EXIT;
  }

  /*
   *	Receive the answer
   */
  res = pcp_lem_request_answer_receive(
          unit,
          &request_hw_rcv,
          ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (request_hw_rcv.request.command != PCP_LEM_ACCESS_CMD_ACK)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_UNKNOWN;
    PCP_DO_NOTHING_AND_EXIT;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_entry_add_unsafe()", 0, 0);
}

/*********************************************************************
 *     Remove an entry to the Exact match table.
 *********************************************************************/
uint32
  pcp_lem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_REQUEST         *request,
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS      *ack_status
  )
{
  uint32
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_OUTPUT
    request_hw,
    request_hw_rcv;
  uint8
    poll_success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_ENTRY_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(request);
  SOC_SAND_CHECK_NULL_INPUT(ack_status);

  PCP_LEM_ACCESS_OUTPUT_clear(&request_hw);
  PCP_LEM_ACCESS_OUTPUT_clear(&request_hw_rcv);

  /*
   *	Verify the inputs
   */
  PCP_STRUCT_VERIFY(PCP_LEM_ACCESS_REQUEST, request, 10, exit);

  /*
   *	Build the Hardware request
   */
  PCP_COPY(&(request_hw.request), request, PCP_LEM_ACCESS_REQUEST, 1);
  request_hw.req_origin = PCP_LEM_ACCCESS_REQ_ORIGIN_CPU;
  request_hw.request.stamp = pcp_lem_access_next_stamp_get(unit);

  if (request_hw.request.command != PCP_LEM_ACCESS_CMD_DELETE)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 25, exit);
  }

  /*
   *	Send the request
   */
  res = pcp_lem_request_send(
          unit,
          &request_hw,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Case of an unsent request
   */
  if (poll_success == FALSE)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_REQUEST_NOT_SENT;
    PCP_DO_NOTHING_AND_EXIT;
  }

  /*
   *	Receive the answer
   */
  res = pcp_lem_request_answer_receive(
          unit,
          &request_hw_rcv,
          ack_status
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (request_hw_rcv.request.command != PCP_LEM_ACCESS_CMD_ACK)
  {
    ack_status->is_success = FALSE;
    ack_status->reason = PCP_LEM_ACCESS_FAIL_REASON_UNKNOWN;
    PCP_DO_NOTHING_AND_EXIT;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_entry_remove_unsafe()", 0, 0);
}

/*********************************************************************
 *     Get an entry in the Exact match table according to its key
 *********************************************************************/
uint32
  pcp_lem_access_entry_by_key_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  PCP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD         *payload,
    SOC_SAND_OUT uint8                        *is_found
  )
{
  uint32
    fld_val,
    fld_val2,
    reg_val1,
    reg_val2,
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
  PCP_REGS
    *regs;
  uint32
    fld_ndx;
  PCP_POLL_INFO
    poll_info;
  uint8
    poll_success;
#ifdef USING_CHIP_SIM
  uint32
    key_in[PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S],
    data_out[PCP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
  uint8
    success;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_ENTRY_BY_KEY_GET_UNSAFE);

  regs = pcp_regs();

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(payload);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  /*
   *	Verify the inputs
   */
  PCP_STRUCT_VERIFY(PCP_LEM_ACCESS_KEY, key, 10, exit);

  /*
   *	Write the key
   */
  res = pcp_lem_key_encoded_build(
          unit,
          key,
          &key_in_buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (fld_ndx = 0; fld_ndx < PCP_ELK_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS; ++fld_ndx)
  {
    PCP_FLD_SET(regs->elk.diagnostics_key_reg[fld_ndx].diagnostics_key, key_in_buffer.buffer[fld_ndx], 20 + fld_ndx, exit);
  }

  /*
   *	Set the trigger
   */
  fld_val = 0x1;
  PCP_FLD_SET(regs->elk.diagnostics_reg.diagnostics_lookup, fld_val, 30, exit);

  /*
   *	Poll on the trigger bit before getting the result
   */
  PCP_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = PCP_LEM_ACCESS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = PCP_LEM_ACCESS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = PCP_LEM_ACCESS_TIMER_DELAY_MSEC;

  res = pcp_status_fld_poll_unsafe(
          unit,
          &(regs->elk.diagnostics_reg.diagnostics_lookup),
          PCP_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR, 50, exit);
  }

  /*
   *	Get the lookup result
   */
  PCP_REG_GET(regs->elk.diagnostics_lookup_result_reg_0, reg_val1, 60, exit);
  PCP_REG_GET(regs->elk.diagnostics_lookup_result_reg_1, reg_val2, 70, exit);

  PCP_FLD_FROM_REG(regs->elk.diagnostics_lookup_result_reg_0.entry_found, fld_val, reg_val1, 80, exit);
  *is_found = SOC_SAND_NUM2BOOL(fld_val);
#ifndef USING_CHIP_SIM
  if (!*is_found)
  {
    goto exit;
  }
#endif

  PCP_FLD_FROM_REG(regs->elk.diagnostics_lookup_result_reg_0.entry_payload, fld_val, reg_val1, 90, exit);
  payload->dest = fld_val;

  PCP_FLD_FROM_REG(regs->elk.diagnostics_lookup_result_reg_0.entry_payload, fld_val, reg_val1, 100, exit);
  PCP_FLD_FROM_REG(regs->elk.diagnostics_lookup_result_reg_1.entry_payload, fld_val2, reg_val2, 110, exit);
  payload->asd = fld_val + (fld_val2 << (regs->elk.diagnostics_lookup_result_reg_0.entry_payload.msb - regs->elk.diagnostics_lookup_result_reg_0.entry_payload.lsb + 1));

  PCP_FLD_FROM_REG(regs->elk.diagnostics_lookup_result_reg_1.entry_payload, fld_val, reg_val2, 115, exit);
  payload->is_dynamic = SOC_SAND_NUM2BOOL(fld_val);

  PCP_FLD_FROM_REG(regs->elk.diagnostics_lookup_result_reg_1.entry_age_stat, fld_val, reg_val2, 120, exit);
  res = pcp_lem_access_age_fld_get(
          unit,
          fld_val,
          &(payload->age)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

#ifdef USING_CHIP_SIM
  sal_memset(key_in, 0x0, CHIP_SIM_LEM_KEY_SIZE) ;
  sal_memset(data_out, 0x0, CHIP_SIM_LEM_DATA_SIZE) ;

  key_in[0] = key_in_buffer.buffer[0];
  key_in[1] = key_in_buffer.buffer[1];

  res = chip_sim_exact_match_entry_get_unsafe(
          unit,
          CHIP_SIM_LEM_ADDR_BASE,
          key_in,
          CHIP_SIM_LEM_KEY_SIZE,
          data_out,
          CHIP_SIM_LEM_DATA_SIZE,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  *is_found = success;

  res = pcp_lem_access_payload_parse(
          unit,
          data_out,
          payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_entry_by_key_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     Get an entry in the Exact match table according to its index
 *********************************************************************/
uint32
  pcp_lem_access_entry_by_index_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        entry_ndx,
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY             *key,
    SOC_SAND_OUT uint8                        *is_valid
  )
{
  uint32
    fld_val,
    reg_val1,
    reg_val2,
    res = SOC_SAND_OK;
  PCP_LEM_ACCESS_KEY_ENCODED
    key_in_buffer;
  PCP_REGS
    *regs;
  PCP_POLL_INFO
    poll_info;
  uint8
    poll_success;
#ifdef USING_CHIP_SIM
  uint32
    key_out[PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S],
    data_out[PCP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
#else
  uint32
    fld_val2;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_ENTRY_BY_INDEX_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(key);
  SOC_SAND_CHECK_NULL_INPUT(is_valid);

  regs = pcp_regs();

  PCP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);

  /*
   *	Verify the inputs
   */
  SOC_SAND_ERR_IF_ABOVE_MAX(entry_ndx, PCP_FRWRD_MACT_TABLE_ENTRY_MAX, PCP_LEM_ENTRY_INDEX_OUT_OF_RANGE_ERR, 10, exit);

  /*
   *	Write the index
   */
  PCP_FLD_SET(regs->elk.diagnostics_index_reg.diagnostics_index, entry_ndx, 20, exit);

  /*
   *	Set the trigger
   */
  fld_val = 0x1;
  PCP_FLD_SET(regs->elk.diagnostics_reg.diagnostics_read, fld_val, 30, exit);

  /*
   *	Poll on the trigger bit before getting the result
   */
  PCP_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = PCP_LEM_ACCESS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = PCP_LEM_ACCESS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = PCP_LEM_ACCESS_TIMER_DELAY_MSEC;

  res = pcp_status_fld_poll_unsafe(
          unit,
          &(regs->elk.diagnostics_reg.diagnostics_read),
          PCP_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_LOOKUP_POLL_TIMEOUT_ERR, 50, exit);
  }

  /*
   *	Get the lookup result
   */
  PCP_REG_GET(regs->elk.diagnostics_read_result_reg_0, reg_val1, 60, exit);
  PCP_REG_GET(regs->elk.diagnostics_read_result_reg_1, reg_val2, 70, exit);

  PCP_FLD_FROM_REG(regs->elk.diagnostics_read_result_reg_0.entry_valid, fld_val, reg_val1, 80, exit);
  *is_valid = SOC_SAND_NUM2BOOL(fld_val);

#ifdef USING_CHIP_SIM
  sal_memset(key_out, 0x0, CHIP_SIM_LEM_KEY_SIZE) ;
  sal_memset(data_out, 0x0, CHIP_SIM_LEM_DATA_SIZE) ;
 /* * COVERITY * 
   * overun was not found. */
 /* coverity[overrun-buffer-arg] */
  res = soc_sand_exact_match_entry_get_by_index_unsafe(
          unit,
          CHIP_SIM_LEM_ADDR_BASE,
          entry_ndx,
          key_out,
          CHIP_SIM_LEM_KEY_SIZE,
          data_out,
          CHIP_SIM_LEM_DATA_SIZE,
          is_valid
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  key_in_buffer.buffer[0] = key_out[0];
  key_in_buffer.buffer[1] = key_out[1];
#else
  PCP_FLD_FROM_REG(regs->elk.diagnostics_read_result_reg_0.entry_key, fld_val, reg_val1, 90, exit);
  PCP_FLD_FROM_REG(regs->elk.diagnostics_read_result_reg_1.entry_key, fld_val2, reg_val2, 100, exit);
  key_in_buffer.buffer[0] = fld_val + (SOC_SAND_GET_BIT(fld_val2, 0) << 31);
  key_in_buffer.buffer[1] = SOC_SAND_GET_BITS_RANGE(fld_val2, 31, 1);
#endif
  if (!*is_valid)
  {
    goto exit;
  }

  res = pcp_lem_key_encoded_parse(
          unit,
          &key_in_buffer,
          key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_access_entry_by_index_get_unsafe()", 0, 0);
}

/* support for ILM  code
uint32
  pcp_lem_ilm_key_build_set(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_FRWRD_ILM_GLBL_INFO       *glbl_info
  )
{
  PCP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
    ihb_flp_key_program_map_tbl_data;
  PCP_CE_INSTRUCTION
    inva_inst_6=PCP_LEM_INSTRUCTION_CEK_0(5),
    inva_inst_12=PCP_LEM_INSTRUCTION_CEK_0(11),
    port_inst=PCP_LEM_INSTRUCTION_CEK_IN_PORT,
    inrif_inst = PCP_LEM_INSTRUCTION_CEK_IN_RIF,
    *inst;

  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_LEM_ACCESS_SA_BASED_ASD_PARSE);

  res = pcp_ihb_flp_key_program_map_tbl_get_unsafe(
          unit,
          PCP_LEM_PROG_LSR_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (glbl_info->key_info.mask_inrif)
  {
    inst = &inva_inst_12;
  }
  else
  {
    inst = &inrif_inst;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction0 = pcp_ce_instruction_build(inst);

  if (glbl_info->key_info.mask_port)
  {
    inst = &inva_inst_6;
  }
  else
  {
    inst = &port_inst;
  }
  ihb_flp_key_program_map_tbl_data.key_b_instruction1 = pcp_ce_instruction_build(inst);

  res = pcp_ihb_flp_key_program_map_tbl_set_unsafe(
          unit,
          PCP_LEM_PROG_LSR_ID,
          &ihb_flp_key_program_map_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_lem_ilm_key_build_set()", 0, 0);
}
*/

/*
 *	Clear functions
 */
void
  PCP_LEM_ACCESS_KEY_ENCODED_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY_ENCODED *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_KEY_ENCODED));
  for (indx = 0; indx < PCP_LEM_ACCESS_KEY_ENCODING_IN_UINT32S; ++indx)
  {
    info->buffer[indx] = 0;
  }

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_KEY_PARAM_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY_PARAM *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_KEY_PARAM));
  for (indx = 0; indx < PCP_LEM_KEY_PARAM_MAX_IN_UINT32S; ++indx)
  {
    info->value[indx] = 0;
  }
  info->nof_bits = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_KEY_PREFIX_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_KEY_PREFIX));

  info->value = 0;
  info->nof_bits = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_KEY_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_KEY *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_KEY));
  info->type = PCP_LEM_ACCESS_NOF_KEY_TYPES;
  for (indx = 0; indx < PCP_LEM_KEY_MAX_NOF_PARAMS; ++indx)
  {
    PCP_LEM_ACCESS_KEY_PARAM_clear(&(info->param[indx]));
  }
  info->nof_params = 0;
  PCP_LEM_ACCESS_KEY_PREFIX_clear(&(info->prefix));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_REQUEST_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_REQUEST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_REQUEST));
  PCP_LEM_ACCESS_KEY_clear(&(info->key));
  info->command = PCP_LEM_ACCESS_NOF_CMDS;
  info->stamp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_PAYLOAD_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_PAYLOAD));
  info->dest = 0;
  info->age = 0;
  info->asd = 0;
  info->is_dynamic = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_OUTPUT_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_OUTPUT  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_OUTPUT));
  PCP_LEM_ACCESS_REQUEST_clear(&(info->request));
  PCP_LEM_ACCESS_PAYLOAD_clear(&(info->payload));
  info->is_learned_first_by_me = 0;
  info->req_origin = PCP_LEM_ACCCESS_NOF_REQ_ORIGINS;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_ACK_STATUS_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_ACK_STATUS  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_ACK_STATUS));
  info->is_success = 0;
  info->reason = PCP_LEM_ACCESS_NOF_FAIL_REASONS;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_LEM_ACCESS_BUFFER_clear(
    SOC_SAND_OUT PCP_LEM_ACCESS_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(PCP_LEM_ACCESS_BUFFER));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  PCP_LEM_ACCESS_KEY_PARAM_verify(
    SOC_SAND_IN PCP_LEM_ACCESS_KEY_TYPE   type,
    SOC_SAND_IN PCP_LEM_ACCESS_KEY_PARAM *info,
    SOC_SAND_IN uint32                   params_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(type, PCP_LEM_ACCESS_NOF_KEY_TYPES-1, PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);

  switch(type)
  {
  case PCP_LEM_ACCESS_KEY_TYPE_MAC:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC - 32))-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 12, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 13, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_MAC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 14, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 15, exit);
    }
    break;

  case PCP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 16, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_BACKBONE_MAC - 32))-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 17, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 18, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_BACKBONE_MAC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 19, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 20, exit);
    }
    break;

  case PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 21, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IPV4_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 22, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 23, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 24, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IPV4_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 25, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 26, exit);
    }
    break;

   case PCP_LEM_ACCESS_KEY_TYPE_IP_HOST:
    if (params_ndx == 0)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_IP_HOST, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 27, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[2], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 29, exit);
    }
    else  /* params_ndx == 1 */
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_IP_HOST, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 30, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->value[2], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 33, exit);
    }
    break;

   case PCP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_MAC - 32))-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 12, exit);
     }
   break;


   case PCP_LEM_ACCESS_KEY_TYPE_ILM:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 37, exit);
       
     }
     else  if (params_ndx == 1 )
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 41, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_ILM)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 42, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     else if ( params_ndx == 2 )
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 41, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_ILM)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 42, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     else  if ( params_ndx == 3 )
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 41, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_ILM)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 42, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     break;

   case PCP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 43, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 44, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
     }
     break;

   case PCP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 43, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 44, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 45, exit);
     }
     else if (params_ndx == 1)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 46, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 47, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 48, exit);
     }
     else if (params_ndx == 2)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 48, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 50, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 51, exit);
     }
     else if (params_ndx == 3)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 52, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 53, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 54, exit);
     }
     else if (params_ndx == 4)
     {       
       SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bits, PCP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 55, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[0], (1 << PCP_LEM_ACCESS_KEY_PARAM4_IN_BITS_FOR_TRILL_MC)-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 56, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], 0, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 57, exit);
     }
     break;

   case PCP_LEM_ACCESS_KEY_TYPE_EXTENDED:
     if (params_ndx == 0)
     {
       SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, PCP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS, PCP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS, PCP_LEM_ACCESS_KEY_PARAM_NOF_BITS_OUT_OF_RANGE_ERR, 45, exit);
       SOC_SAND_ERR_IF_ABOVE_MAX(info->value[1], (1 << (PCP_L2_LIF_KEY_TYPE_EXTENDED_PARAM0_NOF_BITS - 32))-1, PCP_LEM_ACCESS_KEY_PARAM_VALUE_OUT_OF_RANGE_ERR, 65, exit);
     }
     break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 60, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_LEM_ACCESS_KEY_PARAM_verify()",type,params_ndx);
}

uint32
  PCP_LEM_ACCESS_KEY_PREFIX_verify(
    SOC_SAND_IN PCP_LEM_ACCESS_KEY_TYPE type,
    SOC_SAND_IN  PCP_LEM_ACCESS_KEY_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(type, PCP_LEM_ACCESS_NOF_KEY_TYPES-1, PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch(type)
  {
  case PCP_LEM_ACCESS_KEY_TYPE_MAC:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits,PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC,PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 11, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value,PCP_LEM_ACCESS_KEY_PREFIX_FOR_MAC,PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 11, exit);
	break;

  case PCP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_BACKBONE_MAC, PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 13, exit);
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_BACKBONE_MAC,PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 14, exit);
	break;

  case PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
    SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IPV4_MC,PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 15, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_IPV4_MC, PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 16, exit);
	break;

  case PCP_LEM_ACCESS_KEY_TYPE_IP_HOST:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_IP_HOST,PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 17, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_IP_HOST, PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 18, exit);
	break;

  case PCP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_SA_AUTH,PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 19, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_SA_AUTH,PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 20, exit);
    break;

  case PCP_LEM_ACCESS_KEY_TYPE_ILM:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_ILM,PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 17, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_ILM,PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 18, exit);
	break;
  
  case PCP_LEM_ACCESS_KEY_TYPE_EXTENDED:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_EXTENDED, PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 17, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_EXTENDED, PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 18, exit);
	break;

  case PCP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC, PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 19, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC, PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 20, exit);
	break;

  case PCP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->nof_bits, PCP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC,PCP_LEM_ACCESS_KEY_PREFIX_NOF_BITS_OUT_OF_RANGE_ERR, 19, exit);
	SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(info->value, PCP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC,PCP_LEM_ACCESS_KEY_PREFIX_PREFIX_OUT_OF_RANGE_ERR, 20, exit);
	break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 21, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_LEM_ACCESS_KEY_PREFIX_verify()",0,0);
}

uint32
  PCP_LEM_ACCESS_KEY_verify(
    SOC_SAND_IN  PCP_LEM_ACCESS_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    params_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, PCP_LEM_ACCESS_NOF_KEY_TYPES-1, PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch(info->type)
  {
  case PCP_LEM_ACCESS_KEY_TYPE_MAC:
  case PCP_LEM_ACCESS_KEY_TYPE_BACKBONE_MAC:
  case PCP_LEM_ACCESS_KEY_TYPE_IPV4_MC:
  case PCP_LEM_ACCESS_KEY_TYPE_IP_HOST:
  
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4, PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_MAC_BMAC_IPV4, PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case PCP_LEM_ACCESS_KEY_TYPE_ILM:

    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_ILM, PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_ILM, PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case PCP_LEM_ACCESS_KEY_TYPE_EXTENDED:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_params, PCP_LEM_ACCESS_KEY_TYPE_NOF_PARAMS_EXTEND_P2P, PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 11, exit);
    break;
  case PCP_LEM_ACCESS_KEY_TYPE_SA_AUTH:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH, PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SA_AUTH, PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 12, exit);
  break;
  case PCP_LEM_ACCESS_KEY_TYPE_TRILL_UC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC, PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC, PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 13, exit);
  break;
  case PCP_LEM_ACCESS_KEY_TYPE_TRILL_MC:
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_params, PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC, PCP_LEM_ACCESS_KEY_NOF_PARAMS_OUT_OF_RANGE_ERR, 14, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(PCP_LEM_ACCESS_KEY_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }
  for (params_ndx = 0; params_ndx < info->nof_params; ++params_ndx)
  {
    res = PCP_LEM_ACCESS_KEY_PARAM_verify(info->type, &(info->param[params_ndx]), params_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20 + params_ndx, exit);
  }

  res = PCP_LEM_ACCESS_KEY_PREFIX_verify(info->type, &(info->prefix));
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_LEM_ACCESS_KEY_verify()",0,0);
}

uint32
  PCP_LEM_ACCESS_REQUEST_verify(
    SOC_SAND_IN  PCP_LEM_ACCESS_REQUEST *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_LEM_ACCESS_KEY, &(info->key), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->command, PCP_LEM_ACCESS_CMD_MAX, PCP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stamp, 0, PCP_LEM_STAMP_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_LEM_ACCESS_REQUEST_verify()",0,0);
}

uint32
  PCP_LEM_ACCESS_PAYLOAD_verify(
    SOC_SAND_IN  PCP_LEM_ACCESS_PAYLOAD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest, (1<<PCP_LEM_ACCESS_DEST_NOF_BITS)-1, PCP_LEM_DEST_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->asd, (1<<PCP_LEM_ACCESS_ASD_NOF_BITS)-1, PCP_LEM_ASD_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->age, PCP_FRWRD_MACT_ENTRY_KEY_AGE_STATUS_MAX+1, PCP_LEM_AGE_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_LEM_ACCESS_PAYLOAD_verify()",0,0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_lem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_lem_access_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_lem_access;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_lem_access module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_lem_access_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_lem_access;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

