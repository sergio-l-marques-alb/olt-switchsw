/* $Id: pb_pmf_low_level_pgm.c,v 1.8 Broadcom SDK $
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
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/petra_api_ports.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PORTS_FC_TYPE_NONE_FLD_VAL		(0)
#define SOC_PB_PORTS_FC_TYPE_LL_FLD_VAL     (1)
#define SOC_PB_PORTS_FC_TYPE_CB2_FLD_VAL    (2)
#define SOC_PB_PORTS_FC_TYPE_CB8_FLD_VAL    (3)

#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_FC_TYPE_LSB         (0)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_FC_TYPE_MSB         (1)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_1ST_SIZE_HDR_LSB    (2)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_1ST_SIZE_HDR_MSB    (7)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_STAG_LSB            (8)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_STAG_MSB            (8)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SSP_LSB             (9)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SSP_MSB             (9)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_HEADER_TYPE_LSB     (10)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_HEADER_TYPE_MSB     (13)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_MIRROR_PROFILE_LSB  (14)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_MIRROR_PROFILE_MSB  (17)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SNOOP_LSB           (18)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SNOOP_MSB           (18)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_IS_LSB              (19)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_IS_MSB              (19)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PPH_PRESENT_LSB     (20)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PPH_PRESENT_MSB     (20)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PFG_NDX_LSB         (21)
#define SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PFG_NDX_MSB         (23)


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
/* $Id: pb_pmf_low_level_pgm.c,v 1.8 Broadcom SDK $
 * PP port configuration functions
 */
uint32
  soc_pb_pmf_low_level_pgm_port_profile_encode(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          pfg_ndx,
    SOC_SAND_IN  uint8                          is_only_port_pmf_profile,
    SOC_SAND_OUT uint32                          *encoded_pp_info
  )
{
  uint32
    pp_port_conf_encoded;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_PORT_PROFILE_ENCODE);

  pp_port_conf_encoded = 0;
  pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->fc_type, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_FC_TYPE_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_FC_TYPE_LSB);
  pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->first_header_size, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_1ST_SIZE_HDR_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_1ST_SIZE_HDR_LSB);
  pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->is_stag_enabled, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_STAG_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_STAG_LSB);
  pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->is_tm_src_syst_port_ext_present, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SSP_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SSP_LSB);

  if (is_only_port_pmf_profile == FALSE)
  {
    pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->header_type, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_HEADER_TYPE_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_HEADER_TYPE_LSB);
    pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->mirror_profile, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_MIRROR_PROFILE_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_MIRROR_PROFILE_LSB);
    pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->is_snoop_enabled, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SNOOP_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SNOOP_LSB);
    pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->is_tm_ing_shaping_enabled, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_IS_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_IS_LSB);
    pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pp_port_info->is_tm_pph_present_enabled, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PPH_PRESENT_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PPH_PRESENT_LSB);
    /* Encode only for Header type Ethernet */
    if (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_ETH)
    {
      pp_port_conf_encoded |= SOC_SAND_SET_BITS_RANGE(pfg_ndx, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PFG_NDX_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PFG_NDX_LSB);
    }
  }

  *encoded_pp_info = pp_port_conf_encoded;

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_port_profile_encode()", 0, 0);
}

uint32
  soc_pb_pmf_low_level_pgm_port_profile_decode(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           encoded_pp_info,
    SOC_SAND_IN  uint8                          is_only_port_pmf_profile,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_OUT  uint32                        *pfg_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_PORT_PROFILE_DECODE);

  SOC_PB_PORT_PP_PORT_INFO_clear(pp_port_info);
  *pfg_ndx = 0;

  pp_port_info->fc_type =
    SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_FC_TYPE_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_FC_TYPE_LSB);
  pp_port_info->first_header_size = SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_1ST_SIZE_HDR_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_1ST_SIZE_HDR_LSB);
  pp_port_info->is_stag_enabled = SOC_SAND_NUM2BOOL(
    SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_STAG_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_STAG_LSB));
  pp_port_info->is_tm_src_syst_port_ext_present = SOC_SAND_NUM2BOOL(
    SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SSP_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SSP_LSB));

  if (is_only_port_pmf_profile == FALSE)
  {
    pp_port_info->header_type =
      SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_HEADER_TYPE_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_HEADER_TYPE_LSB);
    pp_port_info->mirror_profile =
      SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_MIRROR_PROFILE_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_MIRROR_PROFILE_LSB);
    pp_port_info->is_snoop_enabled = SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SNOOP_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_SNOOP_LSB));
    pp_port_info->is_tm_ing_shaping_enabled = SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_IS_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_IS_LSB));
    pp_port_info->is_tm_pph_present_enabled = SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PPH_PRESENT_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PPH_PRESENT_LSB));
    if (pp_port_info->header_type == SOC_PETRA_PORT_HEADER_TYPE_ETH)
    {
      *pfg_ndx = SOC_SAND_GET_BITS_RANGE(encoded_pp_info, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PFG_NDX_MSB, SOC_PB_PMF_PGM_PORT_PROFILE_ENCODED_PFG_NDX_LSB);
    }
    else
    {
      *pfg_ndx = 0;
    }
  }

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_port_profile_decode()", encoded_pp_info, 0);
}


/*
 * PMF program management functions
 */
uint32
  soc_pb_pmf_low_level_pgm_profile_to_hw_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pp_port_ndx,
    SOC_SAND_IN  uint32                          pfg_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          internal_profile,
    SOC_SAND_IN  uint8                          first_appear
  )
{
  uint32
    encoded_pp_info,
    res = SOC_SAND_OK;
  uint32
    pmf_pgm_ndx;
  SOC_PB_SW_DB_PMF_PGM_PROFILE
    pgm_profile;
  uint8
    ing_shaping_enabled,
    pph_present_enabled;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_PROFILE_TO_HW_ADD);

  /*
   *  Save to the SW DB
   */
  pmf_pgm_ndx = internal_profile;
  for (ing_shaping_enabled = FALSE; ing_shaping_enabled <= TRUE; ing_shaping_enabled++)
  {
    if ((pp_port_info->is_tm_ing_shaping_enabled == TRUE) && (ing_shaping_enabled == FALSE))
    {
      continue;
    }
    for (pph_present_enabled = FALSE; pph_present_enabled <= TRUE; pph_present_enabled++)
    {
      if ((pp_port_info->is_tm_pph_present_enabled == TRUE) && (pph_present_enabled == FALSE))
      {
        continue;
      }

      soc_pb_sw_db_profile_ndx_pmf_pgm_set(
        unit,
        pp_port_ndx,
        pfg_ndx,
        SOC_PB_PMF_PGM_SEL_PARSER_PROFILE_TM(pph_present_enabled, ing_shaping_enabled),
        pmf_pgm_ndx
      );
    }
  }

  /*
   *  Write the PMF-Program to its tables (only if it is the first time)
   *  Assumption: the Header-Profile already defined.
   */
  if (first_appear == TRUE)
  {
    /*
     *  Save to the SW DB
     */
    res = soc_pb_pmf_low_level_pgm_port_profile_encode(
            unit,
            pp_port_info,
            pfg_ndx,
            FALSE,
            &encoded_pp_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    pgm_profile.pp_port_info_encoded = encoded_pp_info;
    pgm_profile.pfg_ndx = pfg_ndx;
    soc_pb_sw_db_pgm_profile_set(
      unit,
      internal_profile,
      &pgm_profile
    );

    /*
     * Set the PMF Program
     */
    res = soc_pb_pmf_pgm_mgmt_set(
            unit,
            pmf_pgm_ndx,
            pp_port_info,
            pfg_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_profile_to_hw_add()", pp_port_ndx, pfg_ndx);
}

/*
 *  PMF Port Profile management functions
 */
uint32
  soc_pb_pmf_low_level_pgm_port_profile_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_OUT uint32                          *internal_profile
  )
{
  uint32
    pp_port_conf_encoded,
    res = SOC_SAND_OK;
  SOC_PB_IHB_PINFO_PMF_TBL_DATA
    tbl_data;
  uint32
    pfg_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_PORT_PROFILE_GET);

  res = soc_pb_ihb_pinfo_pmf_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *internal_profile = tbl_data.port_pmf_profile;

  pp_port_conf_encoded = soc_pb_sw_db_pmf_port_index_profile_get(unit, *internal_profile);

  /*
   * Conversion from uint32 to PP port configuration
   */
  res = soc_pb_pmf_low_level_pgm_port_profile_decode(
          unit,
          pp_port_conf_encoded,
          TRUE,
          pp_port_info,
          &pfg_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_port_profile_get()", pp_port_ndx, 0);
}

uint32
  soc_pb_pmf_low_level_pgm_port_profile_to_hw_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO              *pp_port_info,
    SOC_SAND_IN  uint32                          internal_profile,
    SOC_SAND_IN  uint8                          first_appear
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHB_PINFO_PMF_TBL_DATA
    tbl_data;
  uint32
    port_conf_encoded;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_PORT_PROFILE_TO_HW_ADD);

  /*
   *  Write the PMF-Port profile
   */
  tbl_data.port_pmf_profile = internal_profile;
  res = soc_pb_ihb_pinfo_pmf_tbl_set_unsafe(
          unit,
          pp_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Save the Profile in the SW DB (only if it is the first time)
   */
  if (first_appear == TRUE)
  {
    /*
     * Conversion from the PP Port configuration to uint32
     */
    res = soc_pb_pmf_low_level_pgm_port_profile_encode(
            unit,
            pp_port_info,
            0,
            TRUE,
            &port_conf_encoded
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    soc_pb_sw_db_pmf_port_index_profile_set(
      unit,
      internal_profile,
      port_conf_encoded
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_port_profile_to_hw_add()", pp_port_ndx, 0);
}



/*
 *  System Header Profile management functions
 */
uint32
  soc_pb_pmf_low_level_pgm_header_profile_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pmf_pgm_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile,
    SOC_SAND_OUT uint32                          *internal_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_GET);

  res = soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *internal_profile = tbl_data.system_header_profile;

  res = soc_pb_pp_ihb_header_profile_tbl_get_unsafe(
          unit,
          *internal_profile,
          header_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_header_profile_get()", pmf_pgm_ndx, 0);
}

uint32
  soc_pb_pmf_low_level_pgm_header_profile_to_hw_add(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile,
    SOC_SAND_IN  uint32                          internal_profile,
    SOC_SAND_IN  uint8                          first_appear
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_TO_HW_ADD);

  /*
   *  Write the PMF-Program profile
   */
  res = soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.system_header_profile = internal_profile;

  res = soc_pb_pp_ihb_program_vars_table_tbl_set_unsafe(
          unit,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   * Write the Profile to its table (only if it is the first time)
   */
  if (first_appear == TRUE)
  {
    res = soc_pb_pp_ihb_header_profile_tbl_set_unsafe(
            unit,
            internal_profile,
            header_profile
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_header_profile_to_hw_add()", pmf_pgm_ndx, 0);
}

uint32
  soc_pb_pmf_low_level_pgm_header_profile_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PORTS_FC_TYPE                   fc_type,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE             header_type,
    SOC_SAND_IN  uint8                          tm_pph_present,
    SOC_SAND_OUT SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val = 0;
  uint32
    pph_present = FALSE;
  uint8
    is_tdm = FALSE,
    is_petra_rev_a_in_system,
    is_pp_profile = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_SET);

  SOC_PETRA_CLEAR(header_profile,SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA,1);

  switch (header_type)
  {
  case SOC_PETRA_PORT_HEADER_TYPE_NONE:
  case SOC_PETRA_PORT_HEADER_TYPE_RAW:
  case SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW:
  case SOC_PETRA_PORT_HEADER_TYPE_TM:
    pph_present = tm_pph_present;
    is_pp_profile = FALSE;
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_PROG:
    pph_present = TRUE;
    is_pp_profile = FALSE;
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_ETH:
    pph_present = TRUE;
    is_pp_profile = TRUE;
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_STACKING:
  case SOC_PETRA_PORT_HEADER_TYPE_TDM:
    is_tdm = TRUE;
    break;

  case SOC_PETRA_PORT_HEADER_TYPE_CPU:
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 15, exit);
  }

  if (is_tdm == TRUE)
  {
    /*
     *	All False for TDM - Stacking
     */
    header_profile->header_profile_build_ftmh = FALSE;
    header_profile->header_profile_learn_disable = FALSE;
    header_profile->header_profile_st_vsq_ptr_tc_mode = 0;
    header_profile->header_profile_build_pph = FALSE;
    header_profile->header_profile_ftmh_pph_present = FALSE;
    header_profile->header_profile_always_add_pph_learn_ext = FALSE;
  }
  else
  {
    header_profile->header_profile_build_ftmh = TRUE;
    header_profile->header_profile_learn_disable = FALSE;
    header_profile->header_profile_ftmh_pph_present = pph_present;
    if (is_pp_profile == TRUE)
    {
      is_petra_rev_a_in_system = soc_petra_sw_db_is_petra_rev_a_in_system_get(unit);
      /* if Soc_petra-A in system then no learn extension*/
      header_profile->header_profile_always_add_pph_learn_ext = !is_petra_rev_a_in_system;
      header_profile->header_profile_build_pph = TRUE;
    }
    else
    {
      header_profile->header_profile_learn_disable = TRUE;
      header_profile->header_profile_build_pph = FALSE;
      header_profile->header_profile_always_add_pph_learn_ext = FALSE;
    }
  }


  /*
   * Set the Flow control type
   */
  switch (fc_type)
  {
  case SOC_PB_PORTS_FC_TYPE_NONE:
    fld_val = SOC_PB_PORTS_FC_TYPE_NONE_FLD_VAL;
  	break;
  case SOC_PB_PORTS_FC_TYPE_LL:
    fld_val = SOC_PB_PORTS_FC_TYPE_LL_FLD_VAL;
    break;
  case SOC_PB_PORTS_FC_TYPE_CB2:
    fld_val = SOC_PB_PORTS_FC_TYPE_CB2_FLD_VAL;
    break;
  case SOC_PB_PORTS_FC_TYPE_CB8:
    fld_val = SOC_PB_PORTS_FC_TYPE_CB8_FLD_VAL;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  header_profile->header_profile_st_vsq_ptr_tc_mode = fld_val;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_header_profile_set()", 0, 0);
}

uint32
  soc_pb_pmf_low_level_pgm_header_type_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA *header_profile,
    SOC_SAND_OUT SOC_PB_PORTS_FC_TYPE                  *fc_type,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE            *header_type
  )
{
  uint32
    fld_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_HEADER_TYPE_GET);

  if (header_profile->header_profile_build_ftmh == FALSE)
  {
    *header_type = SOC_PETRA_PORT_HEADER_TYPE_TDM;
  }
  else if (header_profile->header_profile_build_pph == TRUE)
  {
    *header_type = SOC_PETRA_PORT_HEADER_TYPE_ETH;
  }
  else if (header_profile->header_profile_ftmh_pph_present == FALSE)
  {
    *header_type = SOC_PETRA_PORT_HEADER_TYPE_TM;
  }
  else
  {
    *header_type = SOC_PETRA_PORT_HEADER_TYPE_PROG;
  }

  /*
   * Set the Flow control type
   */
  switch (header_profile->header_profile_st_vsq_ptr_tc_mode)
  {
  case SOC_PB_PORTS_FC_TYPE_NONE_FLD_VAL:
    fld_val = SOC_PB_PORTS_FC_TYPE_NONE;
  	break;
  case SOC_PB_PORTS_FC_TYPE_LL_FLD_VAL:
    fld_val = SOC_PB_PORTS_FC_TYPE_LL;
    break;
  case SOC_PB_PORTS_FC_TYPE_CB2_FLD_VAL:
    fld_val = SOC_PB_PORTS_FC_TYPE_CB2;
    break;
  case SOC_PB_PORTS_FC_TYPE_CB8_FLD_VAL:
    fld_val = SOC_PB_PORTS_FC_TYPE_CB8;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  *fc_type = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_header_type_get()", 0, 0);
}


uint32
  soc_pb_pmf_low_level_pgm_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    pfg_ndx,
    pp_port_ndx,
    pmf_pgm_ndx;
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_LOW_LEVEL_PGM_INIT_UNSAFE);


  /*
   * Init the System-Header-Profile for all the PMF-Programs to 0
   */
  for (pmf_pgm_ndx = 0; pmf_pgm_ndx <= SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX; ++pmf_pgm_ndx)
  {
    SOC_PB_PROFILE_NDX_clear(&profile_ndx);
    SOC_PB_PROFILE_PARAMS_clear(&profile_params);
    SOC_PETRA_CLEAR(&(profile_params.header_profile), SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA, 1);
    profile_ndx.pmf_pgm_ndx = pmf_pgm_ndx;
    res = soc_pb_profile_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE,
            &profile_ndx,
            &profile_params,
            TRUE,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (success != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PGM_NOT_ENOUGH_ERR, 15, exit);
    }
  }

  /*
   * Init all the PP-Port to a PP-Port info 0
   */
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; ++pp_port_ndx)
  {
    SOC_PB_PROFILE_NDX_clear(&profile_ndx);
    SOC_PB_PROFILE_PARAMS_clear(&profile_params);
    profile_ndx.pp_port_ndx = pp_port_ndx;
    SOC_PB_PORT_PP_PORT_INFO_clear(&(profile_params.pp_port_info));

    res = soc_pb_profile_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE,
            &profile_ndx,
            &profile_params,
            TRUE,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (success != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PGM_NOT_ENOUGH_ERR, 25, exit);
    }
  }

  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; ++pp_port_ndx)
  {
    for (pfg_ndx = 0; pfg_ndx < SOC_PB_PMF_NOF_LKP_PROFILE_IDS; ++pfg_ndx)
    {
      profile_ndx.pmf_pgm_mgmt_ndx.pp_port_ndx = pp_port_ndx;
      profile_ndx.pmf_pgm_mgmt_ndx.pfg_ndx = pfg_ndx;
      res = soc_pb_profile_add(
              unit,
              SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE,
              &profile_ndx,
              &profile_params,
              TRUE,
              &success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (success != SOC_SAND_SUCCESS)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PGM_NOT_ENOUGH_ERR, 35, exit);
      }
    }

    soc_pb_sw_db_pp_port_header_type_set(
      unit,
      pp_port_ndx,
      SOC_PETRA_PORT_HEADER_TYPE_NONE
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_low_level_pgm_init_unsafe()", 0, 0);
}

/*
 *	Get the encoded entry value
 */
STATIC
  uint32
    soc_pb_pmf_pgm_selection_entry_build(
      SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
      SOC_SAND_OUT uint32                             *tbl_ndx,
      SOC_SAND_OUT uint32                             *offset
    )
{
  uint32
    entry_offset,
    map_tbl_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SELECTION_ENTRY_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);

  /*
   *	Select the table and the entry value
   */
  entry_offset = 0;
  map_tbl_ndx = 0;
  switch(entry_ndx->type)
  {
  case SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8:
    entry_offset = entry_ndx->val.eei_outlif_15_8;
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0:
    entry_offset = entry_ndx->val.eei_outlif_7_0;
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_SEM_NDX_7_0:
    entry_offset = entry_ndx->val.sem_7_0_ndx;
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_SEM_NDX_7_0_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8:
    entry_offset = entry_ndx->val.pfq_sem.sem_13_8_ndx
      + (entry_ndx->val.pfq_sem.pfq << SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8_SIZE_IN_BITS);
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE:
    entry_offset = entry_ndx->val.fwd_ttc.ttc
      + (entry_ndx->val.fwd_ttc.fwd << SOC_PB_PMF_PGM_SEL_TYPE_TTC_CODE_SIZE_IN_BITS);
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_LOOKUPS:
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.lem_1st_found, 7);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.lem_2nd_found, 6);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.sem_1st_found, 5);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.sem_2nd_found, 4);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.lpm_1st_not_dflt, 3);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.lpm_2nd_not_dflt, 2);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.tcam_found, 1);
    SOC_SAND_SET_BIT(entry_offset, entry_ndx->val.lkp.elk_found, 0);
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_LOOKUPS_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO:
    entry_offset = entry_ndx->val.prsr_pmf.port_pmf
      + (entry_ndx->val.prsr_pmf.prsr << SOC_PB_PMF_PGM_SEL_PORT_PMF_SIZE_IN_BITS);
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO_TBL_NDX;
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_LLVP_PFC:
    entry_offset = entry_ndx->val.llvp_pfc.pmf_pro
      + (entry_ndx->val.llvp_pfc.llvp << SOC_PB_PMF_PGM_SEL_PMF_PROFILE_SIZE_IN_BITS);
    map_tbl_ndx = SOC_PB_PMF_PGM_SEL_TYPE_LLVP_PFC_TBL_NDX;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PGM_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  *tbl_ndx = map_tbl_ndx;
  *offset = entry_offset;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_selection_entry_build()", 0, 0);
}

/*********************************************************************
*     Set which programs are relevant per value of a program
 *     selection parameter.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_pgm_selection_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK,
    pgm_ndx,
    entry_offset,
    map_tbl_ndx;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SELECTION_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_pmf_pgm_selection_entry_set_verify(
          unit,
          entry_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


  /*
   *	Select the table and the entry value
   */
  entry_offset = 0;
  map_tbl_ndx = 0;
  res = soc_pb_pmf_pgm_selection_entry_build(
          unit,
          entry_ndx,
          &map_tbl_ndx,
          &entry_offset
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.program_selection_map = 0;
  for (pgm_ndx = 1; pgm_ndx < SOC_PB_PMF_NOF_PGMS; ++pgm_ndx)
  {
    if (info->is_pgm_valid[pgm_ndx] == TRUE)
    {
      SOC_SAND_SET_BIT(tbl_data.program_selection_map, 0x1, pgm_ndx-1);
    }
  }
  
  /*
   *	Set the corresponding table entry
   */
  res = soc_pb_pp_ihb_program_selection_map_tbl_set_unsafe(
          unit,
          map_tbl_ndx,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_selection_entry_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pmf_pgm_selection_entry_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SELECTION_ENTRY_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SELECTION_ENTRY, entry_ndx, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_VALIDITY_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_selection_entry_set_verify()", 0, 0);
}

uint32
  soc_pb_pmf_pgm_selection_entry_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SELECTION_ENTRY_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SELECTION_ENTRY, entry_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_selection_entry_get_verify()", 0, 0);
}

/*********************************************************************
*     Set which programs are relevant per value of a program
 *     selection parameter.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_pgm_selection_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY        *entry_ndx,
    SOC_SAND_OUT SOC_PB_PMF_PGM_VALIDITY_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK,
    pgm_ndx,
    entry_offset,
    map_tbl_ndx;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SELECTION_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(entry_ndx);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_PGM_VALIDITY_INFO_clear(info);

  res = soc_pb_pmf_pgm_selection_entry_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Select the table and the entry value
   */
  entry_offset = 0;
  map_tbl_ndx = 0;
  res = soc_pb_pmf_pgm_selection_entry_build(
          unit,
          entry_ndx,
          &map_tbl_ndx,
          &entry_offset
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the corresponding table entry
   */
  res = soc_pb_pp_ihb_program_selection_map_tbl_get_unsafe(
          unit,
          map_tbl_ndx,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  info->is_pgm_valid[0] = TRUE;
  for (pgm_ndx = 1; pgm_ndx < SOC_PB_PMF_NOF_PGMS; ++pgm_ndx)
  {
    if (SOC_SAND_GET_BIT(tbl_data.program_selection_map, pgm_ndx-1) == TRUE)
    {
      info->is_pgm_valid[pgm_ndx] = TRUE;
    }
    else
    {
      info->is_pgm_valid[pgm_ndx] = FALSE;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_selection_entry_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Program properties, e.g. Lookup-Profile-IDs. The
 *     Copy Engine instructions are set via the PMF Copy Engine
 *     APIs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_pgm_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO          *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  *success = SOC_SAND_SUCCESS;

  res = soc_pb_pmf_pgm_set_verify(
          unit,
          pmf_pgm_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the entry data
   */
  res = soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  tbl_data.first_pass_key_profile_index = info->lkp_profile_id[0];
  tbl_data.second_pass_key_profile_index = info->lkp_profile_id[1];
  tbl_data.tag_selection_profile_index = info->tag_profile_id;

  switch(info->bytes_to_rmv.header_type)
  {
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START_FLD_VAL;
    break;
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_1ST:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_1ST_FLD_VAL;
    break;
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_FWDING:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_FWDING_FLD_VAL;
    break;
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  tbl_data.bytes_to_remove_header = fld_val;
  tbl_data.bytes_to_remove_offset = info->bytes_to_rmv.nof_bytes;
  tbl_data.copy_program_variable = info->copy_pgm_var;

  /*
   *	Set the table
   */
  res = soc_pb_pp_ihb_program_vars_table_tbl_set_unsafe(
          unit,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_set_unsafe()", pmf_pgm_ndx, 0);
}

uint32
  soc_pb_pmf_pgm_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO                   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_set_verify()", pmf_pgm_ndx, 0);
}

uint32
  soc_pb_pmf_pgm_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_get_verify()", pmf_pgm_ndx, 0);
}

/*********************************************************************
*     Set the Program properties, e.g. Lookup-Profile-IDs. The
 *     Copy Engine instructions are set via the PMF Copy Engine
 *     APIs.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_pgm_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_OUT SOC_PB_PMF_PGM_INFO                   *info
  )
{
  uint32
    fld_val,
    internal_hw_ndx,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA
    tbl_data;
  SOC_PB_PROFILE_NDX
    profile_ndx;
  SOC_PB_PROFILE_PARAMS
    *profile_params = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_PGM_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_PMF_PGM_INFO_clear(info);

  res = soc_pb_pmf_pgm_get_verify(
          unit,
          pmf_pgm_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *	Get the table entry
   */
  res = soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe(
          unit,
          pmf_pgm_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the entry data
   */
  info->lkp_profile_id[0] = tbl_data.first_pass_key_profile_index;
  info->lkp_profile_id[1] = tbl_data.second_pass_key_profile_index;
  info->tag_profile_id = tbl_data.tag_selection_profile_index;

  switch(tbl_data.bytes_to_remove_header)
  {
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START_FLD_VAL:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_START;
    break;
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_1ST_FLD_VAL:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_1ST;
    break;
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_FWDING_FLD_VAL:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_FWDING;
    break;
  case SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING_FLD_VAL:
    fld_val = SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }
  info->bytes_to_rmv.header_type = fld_val;
  info->bytes_to_rmv.nof_bytes = tbl_data.bytes_to_remove_offset;

  SOC_PB_PROFILE_NDX_clear(&profile_ndx);
  SOC_PETRA_ALLOC(profile_params, SOC_PB_PROFILE_PARAMS, 1);
  SOC_PB_PROFILE_PARAMS_clear(profile_params);
  profile_ndx.pmf_pgm_ndx = pmf_pgm_ndx;
  res = soc_pb_profile_get(
          unit,
          SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE,
          &profile_ndx,
          profile_params,
          &internal_hw_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pmf_low_level_pgm_header_type_get(
          unit,
          &(profile_params->header_profile),
          &(info->fc_type),
          &(info->header_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  info->copy_pgm_var = tbl_data.copy_program_variable;

exit:
  SOC_PETRA_FREE(profile_params);
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_pgm_get_unsafe()", pmf_pgm_ndx, 0);
}


uint32
  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->pfq, SOC_PB_PMF_LOW_LEVEL_PFQ_MAX, SOC_PB_PMF_LOW_LEVEL_PFQ_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->sem_13_8_ndx, SOC_PB_PMF_LOW_LEVEL_SEM_13_8_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_SEM_13_8_NDX_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fwd, SOC_PB_PMF_LOW_LEVEL_FWD_MAX, SOC_PB_PMF_LOW_LEVEL_FWD_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ttc, SOC_PB_PMF_LOW_LEVEL_TTC_MAX, SOC_PB_PMF_LOW_LEVEL_TTC_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->prsr, SOC_PB_PMF_LOW_LEVEL_PRSR_MAX, SOC_PB_PMF_LOW_LEVEL_PRSR_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_pmf, SOC_PB_PMF_LOW_LEVEL_PORT_PMF_MAX, SOC_PB_PMF_LOW_LEVEL_PORT_PMF_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_SEL_VAL_LKP_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LKP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_SEL_VAL_LKP_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->llvp, SOC_PB_PMF_LOW_LEVEL_LLVP_MAX, SOC_PB_PMF_LOW_LEVEL_LLVP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pmf_pro, SOC_PB_PMF_LOW_LEVEL_PMF_PRO_MAX, SOC_PB_PMF_LOW_LEVEL_PMF_PRO_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_SELECTION_ENTRY_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_PMF_LOW_LEVEL_PGM_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_PGM_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  switch(info->type)
  {
  case SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val.eei_outlif_15_8, SOC_PB_PMF_LOW_LEVEL_EEI_OUTLIF_15_8_MAX, SOC_PB_PMF_LOW_LEVEL_EEI_OUTLIF_15_8_OUT_OF_RANGE_ERR, 9, exit);
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val.eei_outlif_7_0, SOC_PB_PMF_LOW_LEVEL_EEI_OUTLIF_7_0_MAX, SOC_PB_PMF_LOW_LEVEL_EEI_OUTLIF_7_0_OUT_OF_RANGE_ERR, 10, exit);
    break;

  case SOC_PB_PMF_PGM_SEL_TYPE_SEM_NDX_7_0:
    SOC_SAND_ERR_IF_ABOVE_MAX(info->val.sem_7_0_ndx, SOC_PB_PMF_LOW_LEVEL_SEM_7_0_NDX_MAX, SOC_PB_PMF_LOW_LEVEL_SEM_7_0_NDX_OUT_OF_RANGE_ERR, 11, exit);
   break;

  case SOC_PB_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8:
   SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM, &(info->val.pfq_sem), 12, exit);
   break;
  
  case SOC_PB_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE:
   SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC, &(info->val.fwd_ttc), 13, exit);
   break;
 
  case SOC_PB_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO:
   SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF, &(info->val.prsr_pmf), 14, exit);
   break;
 
  case SOC_PB_PMF_PGM_SEL_TYPE_LOOKUPS:
   SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SEL_VAL_LKP, &(info->val.lkp), 15, exit);
   break;
 
  case SOC_PB_PMF_PGM_SEL_TYPE_LLVP_PFC:
    SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC, &(info->val.llvp_pfc), 16, exit);
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PGM_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_SELECTION_ENTRY_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_VALIDITY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_VALIDITY_INFO_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_BYTES_TO_RMV_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, SOC_PB_PMF_LOW_LEVEL_HEADER_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_bytes, SOC_PB_PMF_LOW_LEVEL_NOF_BYTES_MAX, SOC_PB_PMF_LOW_LEVEL_NOF_BYTES_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_BYTES_TO_RMV_verify()",0,0);
}

uint32
  SOC_PB_PMF_PGM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_PMF_NOF_CYCLES; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->lkp_profile_id[ind], SOC_PB_PMF_LOW_LEVEL_LKP_PROFILE_ID_MAX, SOC_PB_PMF_LOW_LEVEL_LKP_PROFILE_ID_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tag_profile_id, SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_ID_MAX, SOC_PB_PMF_LOW_LEVEL_TAG_PROFILE_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, SOC_PETRA_PORT_NOF_HEADER_TYPES-1, SOC_PB_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 12, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PMF_PGM_BYTES_TO_RMV, &(info->bytes_to_rmv), 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->copy_pgm_var, SOC_PB_PMF_LOW_LEVEL_COPY_PGM_VAR_MAX, SOC_PB_PMF_LOW_LEVEL_COPY_PGM_VAR_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fc_type, SOC_PB_PMF_LOW_LEVEL_FC_TYPE_MAX, SOC_PB_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR, 15, exit);

  if (info->header_type == SOC_PETRA_PORT_HEADER_TYPE_CPU)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PMF_PGM_INFO_verify()",0,0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_LKP_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_LKP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_LKP_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_clear(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_TYPE sel_type,
    SOC_SAND_OUT SOC_PB_PMF_PGM_SEL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_clear(sel_type, info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SELECTION_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_SELECTION_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SELECTION_ENTRY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_VALIDITY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_VALIDITY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_VALIDITY_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_BYTES_TO_RMV_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_BYTES_TO_RMV_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_PGM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1
const char*
  SOC_PB_PMF_PGM_SEL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_TYPE enum_val
  )
{
  return SOC_TMC_PMF_PGM_SEL_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR_to_string(
    SOC_SAND_IN  SOC_PB_PMF_PGM_BYTES_TO_RMV_HDR enum_val
  )
{
  return SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_to_string(enum_val);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PFQ_SEM *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_FWD_TTC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_PRSR_PMF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_LKP_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LKP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_LKP_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL_LLVP_PFC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SEL_VAL_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SEL_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SEL_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_SELECTION_ENTRY_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_SELECTION_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_SELECTION_ENTRY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_VALIDITY_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_VALIDITY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_VALIDITY_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_BYTES_TO_RMV_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_BYTES_TO_RMV *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_BYTES_TO_RMV_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_PGM_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_PGM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_PGM_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* SOC_PB_DEBUG_IS_LVL1 */


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

