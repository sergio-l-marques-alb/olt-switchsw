/* $Id: pb_tcam_key.c,v 1.9 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_sw_db_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_acl.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_TCAM_KEY_FORMAT_TYPE_MAX       (SOC_PB_TCAM_NOF_KEY_FORMAT_TYPES-1)
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_L2S_MAX       (SOC_PB_EGR_NOF_ACL_TCAM_FLD_L2S-1)
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_IPV4_MAX       (SOC_PB_EGR_NOF_ACL_TCAM_FLD_IPV4S-1)
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_MPLS_MAX       (SOC_PB_EGR_NOF_ACL_TCAM_FLD_MPLSS-1)
#define SOC_PB_EGR_NOF_ACL_TCAM_FLD_TM_MAX       (SOC_PB_EGR_NOF_ACL_TCAM_FLD_TMS-1)

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


uint32
  soc_pb_tcam_key_size_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT                   *key_format,
    SOC_SAND_OUT SOC_PB_TCAM_BANK_ENTRY_SIZE              *key_size,
    SOC_SAND_OUT uint32                             *key_size_in_bits
  )
{
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    fld_val = 0;
  uint32
    size_in_bits = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_SIZE_GET);

  if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF)
  {
    switch(key_format->pmf)
    {
    case SOC_PB_PMF_TCAM_KEY_SRC_L2:
    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
      fld_val = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
      size_in_bits = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS;
      break;
    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
      fld_val = SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS;
      size_in_bits = SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS_SIZE_IN_BITS;
      break;
    case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
      fld_val = SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS;
      size_in_bits = SOC_PB_TCAM_BANK_ENTRY_SIZE_288_BITS_SIZE_IN_BITS;
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
      fld_val = SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS;
      size_in_bits = SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS_SIZE_IN_BITS;
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
      fld_val = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
      size_in_bits = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS;
      break;

    default:
      break;
    }
  }
  else if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL)
  {
    switch(key_format->egr_acl)
    {
    case SOC_PB_EGR_ACL_DB_TYPE_ETH:
    case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
    case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
    case SOC_PB_EGR_ACL_DB_TYPE_TM:
      fld_val = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
      size_in_bits = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS_SIZE_IN_BITS;
      break;

    default:
      break;
    }
  }

  *key_size = fld_val;
  *key_size_in_bits = size_in_bits;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_size_get()", 0, 0);
}


/*********************************************************************
*     Set the key format and its size (according to the
 *     'key_format' parameter) and mask all the fields.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_key_clear_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT            *key_format,
    SOC_SAND_OUT SOC_PB_TCAM_KEY                   *tcam_key
  )
{
  uint32
    size_in_bits,
    res;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    fld_val;
  uint32
    long_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_CLEAR_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tcam_key);
  SOC_PB_TCAM_KEY_clear(tcam_key);

  res = soc_pb_tcam_key_size_get(
          unit,
          key_format,
          &fld_val,
          &size_in_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tcam_key->format.type = key_format->type;
  tcam_key->format.pmf = key_format->pmf;
  tcam_key->format.egr_acl = key_format->egr_acl;
  tcam_key->size = fld_val;

  for (long_ndx = 0; long_ndx < SOC_PB_TCAM_RULE_NOF_UINT32S_MAX; ++long_ndx)
  {
    tcam_key->data.val[long_ndx] = 0;
    
    /*
     *	Mask all the fields
     */
    tcam_key->data.mask[long_ndx] = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_clear_unsafe()", 0, 0);
}


uint32
  soc_pb_tcam_key_clear_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT               *key_format
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_CLEAR_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY_FORMAT, key_format, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_clear_verify()", 0, 0);
}

STATIC
  uint32
    soc_pb_pmf_tcam_key_a_b_field_lsb_size_get(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SRC               key_format,
      SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B               *fld_type_ndx,
      SOC_SAND_OUT uint32                             *fld_lsb_val,
      SOC_SAND_OUT uint32                             *fld_size_val
    )
{
  uint32
    loc_u32,
    vector_lsb,
    vector_msb,
    size_in_bits,
    irpp_fld_size_in_b,
    fld_lsb,
    fld_size,
    curr_ce_vector_lsb,
    res = SOC_SAND_OK;
  uint8
    is_ce_not_valid,
    is_fld_found = FALSE;
  uint32
    vector_ndx,
    ce_ndx2,
    ce_ndx;
  SOC_PB_PMF_TCAM_KEY_TYPE
    key_type;
  SOC_PB_TCAM_BANK_ENTRY_SIZE
    key_size;
  SOC_PB_PMF_IRPP_INFO_FIELD
    irpp_field;
  SOC_PB_PMF_CE_PACKET_HEADER_INFO
    pkt_hdr_info;
  SOC_PB_TCAM_KEY_FORMAT
    format;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_TCAM_KEY_A_B_FIELD_LSB_SIZE_GET);

  /*
   *	Compute the key type
   */
  switch(key_format)
  {
  case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
    key_type = SOC_PB_PMF_TCAM_KEY_TYPE_A_B;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
  case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
  case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
  case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
    key_type = SOC_PB_PMF_TCAM_KEY_TYPE_A;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
  case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
  case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
  case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
    key_type = SOC_PB_PMF_TCAM_KEY_TYPE_B;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_L2:
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 10, exit);
  }

  /*
   *	Compute the vector size and lsb
   */
  SOC_PB_TCAM_KEY_FORMAT_clear(&format);
  format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
  format.pmf = key_format;
  res = soc_pb_tcam_key_size_get(
          unit,
          &format,
          &key_size,
          &size_in_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch(key_format)
  {
  case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
    vector_lsb = 0;
    vector_msb = 143;
    break;

 case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
 case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
    vector_lsb = 0;
    vector_msb = 71;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
  case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
    vector_lsb = 0;
    vector_msb = 143;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
  case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
    vector_lsb = 32;
    vector_msb = 103;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
  case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
    vector_lsb = 32;
    vector_msb = 175;
    break;

  case SOC_PB_PMF_TCAM_KEY_SRC_L2:
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
  case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
  /* must have default. otherwise, compilation error */
  /* coverity[dead_error_begin : FALSE] */
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 30, exit);
  }


  /*
   *	Find the field location and length
   */
  fld_lsb = 0;
  fld_size = 0;

  /*
   *	Traverse the Copy Engine instructions
   */
  for (vector_ndx = 0; vector_ndx < SOC_PB_NOF_PMF_TCAM_KEY_NDXS; ++vector_ndx)
  {
    /*
     *	Exit the loop if already found
     */
    if (is_fld_found == TRUE)
    {
      break;
    }

    /*
     *	Do not consider the wrong output
     */
    if (
        ((key_type == SOC_PB_PMF_TCAM_KEY_TYPE_A) && (vector_ndx == SOC_PB_PMF_TCAM_KEY_NDX_B))
        || ((key_type == SOC_PB_PMF_TCAM_KEY_TYPE_B) && (vector_ndx == SOC_PB_PMF_TCAM_KEY_NDX_A))
       )
    {
      break;
    }

    /*
     *	Check if the Copy Engine is valid, compute the cumulative size and verify if the field
     *  can be found in it
     */
    curr_ce_vector_lsb = 0;
    for (ce_ndx2 = 0; ce_ndx2 <= SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX; ++ce_ndx2)
    {
      ce_ndx = SOC_PB_PMF_LOW_LEVEL_CE_NDX_MAX - ce_ndx2;
      /*
       *	Exit the loop if already found
       */
      if (is_fld_found == TRUE)
      {
        break;
      }

      res = soc_pb_pmf_ce_nop_entry_get_unsafe(
              unit,
              fld_type_ndx->pmf_pgm_id,
              SOC_PB_PMF_TCAM_KEY_NDX_A,
              ce_ndx,
              &is_ce_not_valid,
              &loc_u32
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      /*
       *	Consider only valid copy engines
       */
      if (is_ce_not_valid == FALSE)
      {
        /*
         *	Get the Copy Engine configuration
         */
        switch(loc_u32)
        {
        case SOC_PB_PMF_TCAM_FLD_A_B_LOC_IRPP:
          res = soc_pb_pmf_ce_irpp_info_entry_get_unsafe(
                  unit,
                  fld_type_ndx->pmf_pgm_id,
                  vector_ndx,
                  ce_ndx,
                  &irpp_fld_size_in_b,
                  &irpp_field
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

          /*
           *	Verify if the field is the wanted one in a
           *  reachable vector location (from field lsb to field msb)
           */
          if (
              (fld_type_ndx->loc == SOC_PB_PMF_TCAM_FLD_A_B_LOC_IRPP)
              && (irpp_field == fld_type_ndx->irpp_fld)
              && (curr_ce_vector_lsb >= vector_lsb)
              && (curr_ce_vector_lsb + irpp_fld_size_in_b - 1 <= vector_msb)
             )
          {
            is_fld_found = TRUE;
            fld_size = irpp_fld_size_in_b;
            fld_lsb = curr_ce_vector_lsb - vector_lsb;
            /*
             *	Special case for the Vector B in the A-B format
             */
            if ((key_type == SOC_PB_PMF_TCAM_KEY_TYPE_A_B) && (vector_ndx == SOC_PB_PMF_TCAM_KEY_NDX_B))
            {
              fld_lsb = fld_lsb + SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS_SIZE_IN_BITS;
            }
          }
          else
          {
           curr_ce_vector_lsb += irpp_fld_size_in_b;
          }
          break;

        case SOC_PB_PMF_TCAM_FLD_A_B_LOC_HDR:
          res = soc_pb_pmf_ce_packet_header_entry_get_unsafe(
                  unit,
                  fld_type_ndx->pmf_pgm_id,
                  vector_ndx,
                  ce_ndx,
                  &pkt_hdr_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          if (fld_type_ndx->loc == SOC_PB_PMF_TCAM_FLD_A_B_LOC_HDR)
          {
            /*
             *	Evaluate if this Copy Engine has the searched instruction
             */
            if (
                (pkt_hdr_info.sub_header == fld_type_ndx->sub_header)
               )
            {
              /*
               *	Verify the offset is in the range of the header found
               */
              /*if (
                  (fld_type_ndx->sub_header_offset >= pkt_hdr_info.offset)
                  && (fld_type_ndx->sub_header_offset < (pkt_hdr_info.offset + pk_nof_bits))
                 )*/
              if (fld_type_ndx->sub_header_offset == pkt_hdr_info.offset)
              {
                is_fld_found = TRUE;
                fld_lsb = (curr_ce_vector_lsb - vector_lsb)
                          + (fld_type_ndx->sub_header_offset - pkt_hdr_info.offset);
                fld_size = pkt_hdr_info.offset + pkt_hdr_info.nof_bits - fld_type_ndx->sub_header_offset;
              }
            }
          }

          curr_ce_vector_lsb += pkt_hdr_info.nof_bits;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_LOC_OUT_OF_RANGE_ERR, 80, exit);
        }
      }
    }
  }

  /*
   *	Return an error if the field is not found
   */
  if (is_fld_found == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_CE_FLD_NOT_FOUND_ERR, 90, exit);
  }

  *fld_lsb_val = fld_lsb;
  *fld_size_val = fld_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_tcam_key_a_b_field_lsb_size_get()", 0, 0);
}


STATIC
  uint32
    soc_pb_tcam_key_field_lsb_size_get(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT                   *key_format,
      SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE                 *fld_type_ndx,
      SOC_SAND_OUT uint32                             *fld_lsb_val,
      SOC_SAND_OUT uint32                             *fld_size_val
    )
{
  uint32
    fld_lsb,
    fld_size,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_FIELD_LSB_SIZE_GET);

  SOC_SAND_CHECK_NULL_INPUT(fld_type_ndx);

  /*
   *	Find the field location and length
   */
  fld_lsb = 0;
  fld_size = 0;
  if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF)
  {
    switch(key_format->pmf)
    {
    case SOC_PB_PMF_TCAM_KEY_SRC_L2:
      switch(fld_type_ndx->l2)
      {
      case SOC_PB_PMF_TCAM_FLD_L2_LLVP:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_LLVP_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_LLVP_SIZE;
        break;
      case SOC_PB_PMF_TCAM_FLD_L2_STAG:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_STAG_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_STAG_SIZE;
        break;
      case SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_CTAG_IN_AC_SIZE;
        break;
      case SOC_PB_PMF_TCAM_FLD_L2_SA:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_SA_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_SA_SIZE;
        break;
      case SOC_PB_PMF_TCAM_FLD_L2_DA:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_DA_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_DA_SIZE;
        break;
      case SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_ETHERTYPE_SIZE;
        break;
      case SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT:
        fld_lsb = SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT_LSB;
        fld_size = SOC_PB_PMF_TCAM_FLD_L2_IN_PP_PORT_SIZE;
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_L2_OUT_OF_RANGE_ERR, 10, exit);
      }
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
      switch(fld_type_ndx->l3.mode)
      {
      case SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV4:
        switch(fld_type_ndx->l3.ipv4_fld)
        {
        case SOC_PB_PMF_FLD_IPV4_L4OPS:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_L4OPS_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_L4OPS_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_NEXT_PRTCL_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_DF:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_DF_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_DF_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_MF:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_MF_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_MF_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_FRAG_NON_0:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_FRAG_NON_0_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_FRAG_NON_0_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_L4OPS_FLAG:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_L4OPS_FLAG_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_L4OPS_FLAG_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_SIP:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_SIP_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_SIP_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_DIP:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_DIP_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_DIP_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_L4OPS_OPT:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_L4OPS_OPT_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_L4OPS_OPT_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_SRC_PORT:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_SRC_PORT_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_SRC_PORT_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_DEST_PORT:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_DEST_PORT_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_DEST_PORT_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_TOS:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_TOS_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_TOS_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_TCP_CTL:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_TCP_CTL_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_TCP_CTL_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_IN_AC_VRF:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_IN_AC_VRF_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_IN_AC_VRF_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_IN_PP_PORT:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_IN_PP_PORT_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_IN_PP_PORT_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV4_IN_VID:
          fld_lsb = SOC_PB_PMF_FLD_IPV4_IN_VID_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV4_IN_VID_SIZE;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_IPV4_FLD_OUT_OF_RANGE_ERR, 20, exit);
        }

        break;

      case SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV6:
        switch(fld_type_ndx->l3.ipv6_fld)
        {
        case SOC_PB_PMF_FLD_IPV6_L4OPS:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_L4OPS_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_L4OPS_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_SIP_HIGH:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_SIP_HIGH_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_SIP_HIGH_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_SIP_LOW:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_SIP_LOW_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_SIP_LOW_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_DIP_HIGH:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_DIP_HIGH_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_DIP_HIGH_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_DIP_LOW:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_DIP_LOW_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_DIP_LOW_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_NEXT_PRTCL_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_PP_PORT_TCP_CTL_SIZE;
          break;
        case SOC_PB_PMF_FLD_IPV6_IN_AC_VRF:
          fld_lsb = SOC_PB_PMF_FLD_IPV6_IN_AC_VRF_LSB;
          fld_size = SOC_PB_PMF_FLD_IPV6_IN_AC_VRF_SIZE;
          break;

        default:
          SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_IPV4_FLD_OUT_OF_RANGE_ERR, 30, exit);
        }
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_MODE_OUT_OF_RANGE_ERR, 40, exit);
      }
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
      res = soc_pb_pmf_tcam_key_a_b_field_lsb_size_get(
              unit,
              key_format->pmf,
              &(fld_type_ndx->a_b),
              &fld_lsb,
              &fld_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 100, exit);
    }
  }
  else if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL)
  {
    switch(key_format->egr_acl)
    {
    case SOC_PB_EGR_ACL_DB_TYPE_ETH:
      switch(fld_type_ndx->egr_l2)
      {
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_OUTER_TAG_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_INNER_TAG_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_SA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_SA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_SA_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_DA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_DA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_DA_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA_SIZE;
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_L2_OUT_OF_RANGE_ERR, 10, exit);
      }
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
      switch(fld_type_ndx->egr_ipv4)
      {
      case SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_TOS_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_SIP_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_DIP_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA_SIZE;
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_IPV4_OUT_OF_RANGE_ERR, 20, exit);
      }
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
      switch(fld_type_ndx->egr_mpls)
      {
      case SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_FTMH_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_LABEL_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_EXP_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_TTL_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA_SIZE;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_MPLS_OUT_OF_RANGE_ERR, 30, exit);
      }
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_TM:
      switch(fld_type_ndx->egr_tm)
      {
      case SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_TM_FTMH_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_TM_HDR_DATA_SIZE;
        break;
      case SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA:
        fld_lsb = SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA_LSB;
        fld_size = SOC_PB_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA_SIZE;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_TM_OUT_OF_RANGE_ERR, 40, exit);
      }
      break;


    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR, 100, exit);
    }
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_KEY_FORMAT_TYPE_OUT_OF_RANGE_ERR, 40, exit);
  }

  *fld_lsb_val = fld_lsb;
  *fld_size_val = fld_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_field_lsb_size_get()", 0, 0);
}





/*********************************************************************
*     Write inside the key the field value in the place set
 *     according to the field type and the key format. Unmask
 *     the corresponding bits in the key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_key_val_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                 *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val
  )
{
  uint32
    fld_lsb,
    fld_size,
    buffer_val,
    res = SOC_SAND_OK;
  uint32
    bit_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_VAL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tcam_key);
  SOC_SAND_CHECK_NULL_INPUT(fld_type_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fld_val);

  res = soc_pb_tcam_key_val_set_verify(
          unit,
          tcam_key,
          fld_type_ndx,
          fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Find the field location and length
   */
  fld_lsb = 0;
  fld_size = 0;

  res = soc_pb_tcam_key_field_lsb_size_get(
          unit,
          &(tcam_key->format),
          fld_type_ndx,
          &fld_lsb,
          &fld_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Set the field value in the key and unmask the fields
   */
  for (bit_ndx = 0; bit_ndx < fld_size; ++bit_ndx)
  {
    if (bit_ndx < fld_size)
    {
      buffer_val = 0x1;
    }
    else
    {
      buffer_val = 0x0;
    }
    res = soc_sand_bitstream_set_any_field(
            &(buffer_val),
            bit_ndx + fld_lsb,
            1,
            tcam_key->data.mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          fld_val->val,
          fld_lsb,
          fld_size,
          tcam_key->data.val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_val_set_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_key_val_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                 *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_VAL_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY, tcam_key, 10, exit);

  res = SOC_PB_TCAM_KEY_FLD_TYPE_verify(&(tcam_key->format), fld_type_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = SOC_PB_TCAM_KEY_FLD_VAL_verify(unit, &(tcam_key->format), fld_type_ndx, fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_val_set_verify()", 0, 0);
}

uint32
  soc_pb_tcam_key_val_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_VAL_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY, tcam_key, 10, exit);

  res = SOC_PB_TCAM_KEY_FLD_TYPE_verify(&(tcam_key->format), fld_type_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_val_get_verify()", 0, 0);
}

/*********************************************************************
*     Write inside the key the field value in the place set
 *     according to the field type and the key format. Unmask
 *     the corresponding bits in the key.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_key_val_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL           *fld_val
  )
{
  uint32
    fld_lsb,
    fld_size,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_VAL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tcam_key);
  SOC_SAND_CHECK_NULL_INPUT(fld_type_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fld_val);

  SOC_PB_TCAM_KEY_FLD_VAL_clear(fld_val);

  res = soc_pb_tcam_key_val_get_verify(
          unit,
          tcam_key,
          fld_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Find the field location and length
   */
  fld_lsb = 0;
  fld_size = 0;

  res = soc_pb_tcam_key_field_lsb_size_get(
          unit,
          &(tcam_key->format),
          fld_type_ndx,
          &fld_lsb,
          &fld_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the field value
   */
  res = soc_sand_bitstream_get_any_field(
          tcam_key->data.val,
          fld_lsb,
          fld_size,
          fld_val->val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_val_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Write inside the key the field value in the place set
 *     according to the field type and the key format. Unmask
 *     in the key the bits according to the mask value.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_key_masked_val_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *mask_val
  )
{
  uint32
    fld_lsb,
    fld_size,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_MASKED_VAL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tcam_key);
  SOC_SAND_CHECK_NULL_INPUT(fld_type_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fld_val);
  SOC_SAND_CHECK_NULL_INPUT(mask_val);

  res = soc_pb_tcam_key_masked_val_set_verify(
          unit,
          tcam_key,
          fld_type_ndx,
          fld_val,
          mask_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Called the unmasked helper function (which unmasks all the bits)
   *  and then mask the bits according to the mask value
   */
  res = soc_pb_tcam_key_val_set_unsafe(
          unit,
          tcam_key,
          fld_type_ndx,
          fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Find the field location and length
   */
  fld_lsb = 0;
  fld_size = 0;

  res = soc_pb_tcam_key_field_lsb_size_get(
          unit,
          &(tcam_key->format),
          fld_type_ndx,
          &fld_lsb,
          &fld_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_set_any_field(
          mask_val->val,
          fld_lsb,
          fld_size,
          tcam_key->data.mask
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_masked_val_set_unsafe()", 0, 0);
}

uint32
  soc_pb_tcam_key_masked_val_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL           *mask_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_MASKED_VAL_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY, tcam_key, 10, exit);

  res = SOC_PB_TCAM_KEY_FLD_TYPE_verify(&(tcam_key->format), fld_type_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = SOC_PB_TCAM_KEY_FLD_VAL_verify(unit, &(tcam_key->format), fld_type_ndx, fld_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = SOC_PB_TCAM_KEY_FLD_VAL_verify(unit, &(tcam_key->format), fld_type_ndx, mask_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_masked_val_set_verify()", 0, 0);
}

uint32
  soc_pb_tcam_key_masked_val_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_MASKED_VAL_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY, tcam_key, 10, exit);

  res = SOC_PB_TCAM_KEY_FLD_TYPE_verify(&(tcam_key->format), fld_type_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_masked_val_get_verify()", 0, 0);
}

/*********************************************************************
*     Write inside the key the field value in the place set
 *     according to the field type and the key format. Unmask
 *     in the key the bits according to the mask value.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_tcam_key_masked_val_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL           *mask_val
  )
{
  uint32
    fld_lsb,
    fld_size,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_TCAM_KEY_MASKED_VAL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(tcam_key);
  SOC_SAND_CHECK_NULL_INPUT(fld_type_ndx);
  SOC_SAND_CHECK_NULL_INPUT(fld_val);
  SOC_SAND_CHECK_NULL_INPUT(mask_val);

  SOC_PB_TCAM_KEY_FLD_VAL_clear(fld_val);
  SOC_PB_TCAM_KEY_FLD_VAL_clear(mask_val);

  res = soc_pb_tcam_key_masked_val_get_verify(
          unit,
          tcam_key,
          fld_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Called the unmasked helper function to get the field value
   */
  res = soc_pb_tcam_key_val_get_unsafe(
          unit,
          tcam_key,
          fld_type_ndx,
          fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Find the field location and length
   */
  fld_lsb = 0;
  fld_size = 0;

  res = soc_pb_tcam_key_field_lsb_size_get(
          unit,
          &(tcam_key->format),
          fld_type_ndx,
          &fld_lsb,
          &fld_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	Get the mask value
   */
  res = soc_sand_bitstream_get_any_field(
          tcam_key->data.mask,
          fld_lsb,
          fld_size,
          mask_val->val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_tcam_key_masked_val_get_unsafe()", 0, 0);
}

uint32
  SOC_PB_TCAM_RULE_INFO_verify(
    SOC_SAND_IN  SOC_PB_TCAM_RULE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_RULE_INFO_verify()",0,0);
}

uint32
  SOC_PB_TCAM_KEY_FORMAT_verify(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_TCAM_KEY_FORMAT_TYPE_MAX, SOC_PB_TCAM_KEY_FORMAT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch (info->type)
  {
  case SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF:
    /*SOC_SAND_ERR_IF_ABOVE_MAX(info->pmf, SOC_PB_PMF_LOW_LEVEL_KEY_FORMAT_MAX, SOC_PB_PMF_LOW_LEVEL_KEY_FORMAT_OUT_OF_RANGE_ERR, 11, exit);*/
    break;
  case SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->egr_acl, SOC_PB_EGR_ACL_DB_TYPE_MAX, SOC_PB_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR, 12, exit);
    break;
  default:
    break;
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_KEY_FORMAT_verify()",0,0);
}

uint32
  SOC_PB_TCAM_KEY_verify(
    SOC_SAND_IN  SOC_PB_TCAM_KEY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->size, SOC_PB_PMF_LOW_LEVEL_SIZE_MAX, SOC_PB_PMF_LOW_LEVEL_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_RULE_INFO, &(info->data), 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_TCAM_KEY_FORMAT, &(info->format), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pmf_pgm_id, SOC_PB_PMF_LOW_LEVEL_PGM_ID_MAX, SOC_PB_PMF_LOW_LEVEL_PGM_ID_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_KEY_verify()",0,0);
}

uint32
  SOC_PB_TCAM_OUTPUT_verify(
    SOC_SAND_IN  SOC_PB_TCAM_OUTPUT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* No Limit: encoded in 32b */
/*  SOC_SAND_ERR_IF_ABOVE_MAX(info->val, 0, SOC_PB_PMF_LOW_LEVEL_VAL_OUT_OF_RANGE_ERR, 10, exit);*/

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_OUTPUT_verify()",0,0);
}
uint32
  SOC_PB_TCAM_KEY_FLD_TYPE_verify(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF)
  {
    switch(key_format->pmf)
    {
    case SOC_PB_PMF_TCAM_KEY_SRC_L2:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->l2, SOC_PB_PMF_LOW_LEVEL_L2_MAX, SOC_PB_PMF_LOW_LEVEL_L2_OUT_OF_RANGE_ERR, 10, exit);
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV4:
    case SOC_PB_PMF_TCAM_KEY_SRC_L3_IPV6:
      SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TCAM_FLD_L3, &(info->l3), 11, exit);
      break;

    case SOC_PB_PMF_TCAM_KEY_SRC_B_A:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_71_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_103_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_71_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_103_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_143_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_A_175_32:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_143_0:
    case SOC_PB_PMF_TCAM_KEY_SRC_B_175_32:
      SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_TCAM_FLD_A_B, &(info->a_b), 12, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR, 20, exit);
    }
  }
  else if (key_format->type == SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL)
  {
    switch(key_format->egr_acl)
    {
    case SOC_PB_EGR_ACL_DB_TYPE_ETH:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->egr_l2, SOC_PB_EGR_NOF_ACL_TCAM_FLD_L2S_MAX, SOC_PB_EGR_ACL_L2_OUT_OF_RANGE_ERR, 21, exit);
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_IPV4:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->egr_ipv4, SOC_PB_EGR_NOF_ACL_TCAM_FLD_IPV4_MAX, SOC_PB_EGR_ACL_IPV4_OUT_OF_RANGE_ERR, 22, exit);
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_MPLS:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->egr_mpls, SOC_PB_EGR_NOF_ACL_TCAM_FLD_MPLS_MAX, SOC_PB_EGR_ACL_MPLS_OUT_OF_RANGE_ERR, 23, exit);
      break;

    case SOC_PB_EGR_ACL_DB_TYPE_TM:
      SOC_SAND_ERR_IF_ABOVE_MAX(info->egr_tm, SOC_PB_EGR_NOF_ACL_TCAM_FLD_TM_MAX, SOC_PB_EGR_ACL_TM_OUT_OF_RANGE_ERR, 24, exit);
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR, 30, exit);
    }
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_KEY_FORMAT_TYPE_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_KEY_FLD_TYPE_verify()",0,0);
}

uint32
  SOC_PB_PMF_TCAM_FLD_L3_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L3 *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_PMF_LOW_LEVEL_MODE_MAX, SOC_PB_PMF_LOW_LEVEL_MODE_OUT_OF_RANGE_ERR, 10, exit);
  switch(info->mode)
  {
  case SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV4:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->ipv4_fld, SOC_PB_PMF_LOW_LEVEL_IPV4_FLD_MAX, SOC_PB_PMF_LOW_LEVEL_IPV4_FLD_OUT_OF_RANGE_ERR, 11, exit);
    break;
  
  case SOC_PB_PMF_TCAM_FLD_L3_MODE_IPV6:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->ipv6_fld, SOC_PB_PMF_LOW_LEVEL_IPV6_FLD_MAX, SOC_PB_PMF_LOW_LEVEL_IPV6_FLD_OUT_OF_RANGE_ERR, 12, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_FLD_L3_verify()",0,0);
}

uint32
  SOC_PB_PMF_TCAM_FLD_A_B_verify(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->loc, SOC_PB_PMF_LOW_LEVEL_LOC_MAX, SOC_PB_PMF_LOW_LEVEL_LOC_OUT_OF_RANGE_ERR, 10, exit);

  switch(info->loc)
  {
  case SOC_PB_PMF_TCAM_FLD_A_B_LOC_IRPP:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->irpp_fld, SOC_PB_PMF_LOW_LEVEL_IRPP_FLD_MAX, SOC_PB_PMF_LOW_LEVEL_IRPP_FLD_OUT_OF_RANGE_ERR, 11, exit);
    break;

  case SOC_PB_PMF_TCAM_FLD_A_B_LOC_HDR:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->sub_header, SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_MAX, SOC_PB_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 12, exit);
    if ((info->sub_header_offset > SOC_PB_PMF_LOW_LEVEL_OFFSET_MAX) || (info->sub_header_offset < SOC_PB_PMF_LOW_LEVEL_OFFSET_MIN))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_OFFSET_OUT_OF_RANGE_ERR, 13, exit);
    }
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_LOC_OUT_OF_RANGE_ERR, 20, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pmf_pgm_id, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_ID_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_ID_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_TCAM_FLD_A_B_verify()",0,0);
}


uint32
  SOC_PB_TCAM_KEY_FLD_VAL_verify(
   SOC_SAND_IN  int             unit,
   SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT   *key_format,
   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE *fld_type,
   SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL  *info
  )
{
  uint32
    fld_lsb,
    fld_size = 0,
    zero_size,
    res = SOC_SAND_OK;
  SOC_PB_TCAM_KEY_FLD_VAL
    zero_val;
  uint32
    val_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_TCAM_KEY_FLD_VAL_clear(&zero_val);

  /*
   *	Get the field value size in bits, and check
   *  the remaining value is zero
   */
  res = soc_pb_tcam_key_field_lsb_size_get(
          unit,
          key_format,
          fld_type,
          &fld_lsb,
          &fld_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  zero_size = SOC_PB_TCAM_KEY_FLD_NOF_UINT32S_MAX * SOC_SAND_REG_SIZE_BITS - fld_size;

  res = soc_sand_bitstream_get_any_field(
          info->val,
          fld_size,
          zero_size,
          zero_val.val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (val_ndx = 0; val_ndx < SOC_PB_TCAM_KEY_FLD_NOF_UINT32S_MAX; ++val_ndx)
  {
    if (zero_val.val[val_ndx] != 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_TCAM_KEY_FLD_VAL_OUT_OF_RANGE_ERR, 40, exit);
    }
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_TCAM_KEY_FLD_VAL_verify()",0,0);
}

void
  SOC_PB_TCAM_RULE_INFO_clear(
    SOC_SAND_OUT SOC_PB_TCAM_RULE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_RULE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_FORMAT_clear(
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_FORMAT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_clear(
    SOC_SAND_OUT SOC_PB_TCAM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_OUTPUT_clear(
    SOC_SAND_OUT SOC_PB_TCAM_OUTPUT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_OUTPUT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_TCAM_KEY_FLD_TYPE_clear(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_FLD_TYPE_clear(key_format, info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_PMF_TCAM_FLD_L3_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_FLD_L3 *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_FLD_L3_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_FLD_A_B_clear(
    SOC_SAND_OUT SOC_PB_PMF_TCAM_FLD_A_B *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_FLD_A_B_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_FLD_VAL_clear(
    SOC_SAND_OUT SOC_PB_TCAM_KEY_FLD_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_FLD_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1
const char*
  SOC_PB_EGR_ACL_DB_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_DB_TYPE enum_val
  )
{
  return SOC_TMC_EGR_ACL_DB_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PMF_TCAM_KEY_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_KEY_SRC enum_val
  )
{
  return SOC_TMC_PMF_TCAM_KEY_SRC_to_string(enum_val);
}

const char*
  SOC_PB_TCAM_KEY_FORMAT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT_TYPE enum_val
  )
{
  return SOC_TMC_TCAM_KEY_FORMAT_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PMF_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L2 enum_val
  )
{
  return SOC_TMC_PMF_TCAM_FLD_L2_to_string(enum_val);
}

const char*
  SOC_PB_PMF_TCAM_FLD_L3_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L3_MODE enum_val
  )
{
  return SOC_TMC_PMF_TCAM_FLD_L3_MODE_to_string(enum_val);
}

const char*
  SOC_PB_PMF_FLD_IPV4_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FLD_IPV4 enum_val
  )
{
  return SOC_TMC_PMF_FLD_IPV4_to_string(enum_val);
}

const char*
  SOC_PB_PMF_FLD_IPV6_to_string(
    SOC_SAND_IN  SOC_PB_PMF_FLD_IPV6 enum_val
  )
{
  return SOC_TMC_PMF_FLD_IPV6_to_string(enum_val);
}

const char*
  SOC_PB_PMF_TCAM_FLD_A_B_LOC_to_string(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B_LOC enum_val
  )
{
  return SOC_TMC_PMF_TCAM_FLD_A_B_LOC_to_string(enum_val);
}

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_L2 enum_val
  )
{
  return SOC_TMC_EGR_ACL_TCAM_FLD_L2_to_string(enum_val);
}

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_IPV4_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_IPV4 enum_val
  )
{
  return SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_to_string(enum_val);
}

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_MPLS_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_MPLS enum_val
  )
{
  return SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_to_string(enum_val);
}

const char*
  SOC_PB_EGR_ACL_TCAM_FLD_TM_to_string(
    SOC_SAND_IN  SOC_PB_EGR_ACL_TCAM_FLD_TM enum_val
  )
{
  return SOC_TMC_EGR_ACL_TCAM_FLD_TM_to_string(enum_val);
}

void
  SOC_PB_TCAM_RULE_INFO_print(
    SOC_SAND_IN  SOC_PB_TCAM_RULE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_RULE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_FORMAT_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FORMAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_FORMAT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_OUTPUT_print(
    SOC_SAND_IN  SOC_PB_TCAM_OUTPUT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_OUTPUT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_FLD_L3_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_L3 *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_FLD_L3_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_TCAM_FLD_A_B_print(
    SOC_SAND_IN  SOC_PB_PMF_TCAM_FLD_A_B *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_TCAM_FLD_A_B_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_FLD_TYPE_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_FLD_TYPE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_TCAM_KEY_FLD_VAL_print(
    SOC_SAND_IN  SOC_PB_TCAM_KEY_FLD_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_TCAM_KEY_FLD_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

