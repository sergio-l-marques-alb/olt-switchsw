/* $Id: pb_profile.c,v 1.6 Broadcom SDK $
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

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_profile.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>

#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

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


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_profile_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx,
    SOC_SAND_OUT SOC_PB_PROFILE_PARAMS            *profile_params,
    SOC_SAND_OUT uint32                     *internal_hw_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PROFILE_GET);

  SOC_PB_PROFILE_PARAMS_clear(profile_params);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch (profile_type)
  {
  case SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR:
    res = soc_pb_egr_prog_editor_profile_get(
            unit,
            profile_ndx->port_ndx,
            &(profile_params->eg_profile),
            internal_hw_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  	break;
    
  case SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE:
    res = soc_pb_pmf_low_level_pgm_header_profile_get(
            unit,
            profile_ndx->pmf_pgm_ndx,
            &(profile_params->header_profile),
            internal_hw_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  	break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE:
    res = soc_pb_pmf_low_level_pgm_port_profile_get(
            unit,
            profile_ndx->pp_port_ndx,
            &(profile_params->pp_port_info),
            internal_hw_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  	break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE:
    res = soc_pb_pmf_pgm_mgmt_profile_get(
            unit,
            &(profile_ndx->pmf_pgm_mgmt_ndx),
            profile_ndx->pmf_pgm_ndx,
            &(profile_params->pmf_pgm_mgmt_params),
            internal_hw_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  	break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_SW_DB_MULTI_SET_OUT_OF_RANGE_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_profile_get()", 0, 0);
}

uint32
  soc_pb_profile_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET           profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx,
    SOC_SAND_IN  SOC_PB_PROFILE_PARAMS            *profile_params,
    SOC_SAND_IN  uint8                    no_remove_previous_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK,
    internal_hw_ndx;
  uint8
    first_appear;
  SOC_SAND_SUCCESS_FAILURE
    multi_set_success;
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_HEADER_PROFILE_TBL
    *tbl;
  uint32
    data = 0;
  SOC_PB_PROFILE_NDX
    profile_ndx2,
    profile_ndx3;
  SOC_PB_PROFILE_PARAMS
    profile_params2,
    profile_params3;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PROFILE_ADD);

  SOC_SAND_CHECK_NULL_INPUT(profile_ndx);
  SOC_SAND_CHECK_NULL_INPUT(profile_params);
  SOC_SAND_CHECK_NULL_INPUT(success);

  *success = SOC_SAND_SUCCESS;
  /*
   * Remove the profile from this port (if not init)
   */
  if (no_remove_previous_profile == FALSE)
  {
    res = soc_pb_profile_remove(
            unit,
            profile_type,
            profile_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }


  /*
   *  Add the profile
   */
  switch (profile_type)
  {
  case SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR:
    res = soc_pb_sw_db_multiset_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR,
            profile_params->eg_profile,
            &internal_hw_ndx,
            &first_appear,
            &multi_set_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE:
    /*
     * Set the table to stream (identical to the HW)
     */
    res = soc_pb_pp_tbls_get(&(tables));
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    tbl = &(tables->ihb.header_profile_tbl);

    res = soc_pb_pp_ihb_header_profile_tbl2stream_set_unsafe(
            unit,
            &(profile_params->header_profile),
            tbl,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_pb_sw_db_multiset_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE,
            data,
            &internal_hw_ndx,
            &first_appear,
            &multi_set_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE:
    /*
     * Set the structure to stream
     */
    res = soc_pb_pmf_low_level_pgm_port_profile_encode(
            unit,
            &(profile_params->pp_port_info),
            profile_ndx->pmf_pgm_mgmt_ndx.pfg_ndx,
            TRUE,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_pb_sw_db_multiset_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE,
            data,
            &internal_hw_ndx,
            &first_appear,
            &multi_set_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE:
    /*
     * Set the structure to stream
     */
    res = soc_pb_pmf_low_level_pgm_port_profile_encode(
            unit,
            &(profile_params->pmf_pgm_mgmt_params.pp_port),
            profile_ndx->pmf_pgm_mgmt_ndx.pfg_ndx,
            FALSE,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_pb_sw_db_multiset_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE,
            data,
            &internal_hw_ndx,
            &first_appear,
            &multi_set_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    if (multi_set_success != SOC_SAND_SUCCESS)
    {
      *success = multi_set_success;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    /*
     *  Add the System-Header-Profile
     */
    SOC_PB_PROFILE_NDX_clear(&profile_ndx2);
    SOC_PB_PROFILE_PARAMS_clear(&profile_params2);
    res = soc_pb_pmf_low_level_pgm_header_profile_set(
            unit,
            profile_params->pmf_pgm_mgmt_params.pp_port.fc_type,
            profile_params->pmf_pgm_mgmt_params.pp_port.header_type,
            profile_params->pmf_pgm_mgmt_params.pp_port.is_tm_pph_present_enabled,
            &(profile_params2.header_profile)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

    profile_ndx2.pmf_pgm_ndx = internal_hw_ndx;
    res = soc_pb_profile_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE,
            &profile_ndx2,
            &profile_params2,
            no_remove_previous_profile,
            &multi_set_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);
    if (multi_set_success != SOC_SAND_SUCCESS)
    {
      *success = multi_set_success;
      SOC_PETRA_DO_NOTHING_AND_EXIT;
    }

    /*
     *  Add the PP-Port-PMF-Profile
     */
    SOC_PB_PROFILE_NDX_clear(&profile_ndx3);
    SOC_PB_PROFILE_PARAMS_clear(&profile_params3);
    profile_ndx3.pp_port_ndx = profile_ndx->pmf_pgm_mgmt_ndx.pp_port_ndx;
    SOC_PETRA_COPY(&(profile_params3.pp_port_info), &(profile_params->pmf_pgm_mgmt_params.pp_port), SOC_PB_PORT_PP_PORT_INFO, 1);
    res = soc_pb_profile_add(
            unit,
            SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE,
            &profile_ndx3,
            &profile_params3,
            no_remove_previous_profile,
            &multi_set_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_SW_DB_MULTI_SET_OUT_OF_RANGE_ERR, 100, exit);
  }
 
  if (multi_set_success != SOC_SAND_SUCCESS)
  {
    *success = multi_set_success;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /* Add to HW */
  res = soc_pb_profile_to_hw_add(
          unit,
          profile_type,
          profile_ndx,
          profile_params,
          internal_hw_ndx,
          first_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_profile_add()", 0, 0);
}

uint32
  soc_pb_profile_remove(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx
  )
{
  uint32
    internal_hw_ndx,
    res = SOC_SAND_OK;
  uint8
    last_appear;
  SOC_PB_PROFILE_PARAMS
    profile_params;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PROFILE_PROFILE_REMOVE);

  res = soc_pb_profile_get(
          unit,
          profile_type,
          profile_ndx,
          &profile_params,
          &internal_hw_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Remove the profile if single user (gives a place)
   */
  res = soc_pb_sw_db_multiset_remove_by_index(
          unit,
          profile_type,
          internal_hw_ndx,
          &last_appear
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (last_appear == TRUE)
  {
    if (profile_type == SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR)
    {
      if (profile_params.eg_profile == SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1)
      {
        res = soc_pb_sw_db_multiset_remove_by_index(
                unit,
                SOC_PB_SW_DB_MULTI_SET_STK_PRUNE_BMP,
                0,
                &last_appear
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }
      else if (profile_params.eg_profile == SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK2)
      {
        res = soc_pb_sw_db_multiset_remove_by_index(
                unit,
                SOC_PB_SW_DB_MULTI_SET_STK_PRUNE_BMP,
                1,
                &last_appear
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_profile_remove()", 0, 0);
}

uint32
  soc_pb_profile_to_hw_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_SW_DB_MULTI_SET              profile_type,
    SOC_SAND_IN  SOC_PB_PROFILE_NDX               *profile_ndx,
    SOC_SAND_IN  SOC_PB_PROFILE_PARAMS            *profile_params,
    SOC_SAND_IN  uint32                    internal_hw_ndx,
    SOC_SAND_IN  uint8                    first_appear
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PROFILE_TO_HW_ADD);

  switch (profile_type)
  {
  case SOC_PB_SW_DB_MULTI_SET_EGR_EDITOR:
    res = soc_pb_egr_prog_editor_port_profile_to_hw_add(
            unit,
            profile_ndx->port_ndx,
            profile_params->eg_profile,
            internal_hw_ndx,
            first_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_HDR_PROFILE:
    res = soc_pb_pmf_low_level_pgm_header_profile_to_hw_add(
            unit,
            profile_ndx->pmf_pgm_ndx,
            &(profile_params->header_profile),
            internal_hw_ndx,
            first_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_PORT_PROFILE:
    res = soc_pb_pmf_low_level_pgm_port_profile_to_hw_add(
            unit,
            profile_ndx->pp_port_ndx,
            &(profile_params->pp_port_info),
            internal_hw_ndx,
            first_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    break;

  case SOC_PB_SW_DB_MULTI_SET_PMF_PGM_PROFILE:
    res = soc_pb_pmf_low_level_pgm_profile_to_hw_add(
            unit,
            profile_ndx->pmf_pgm_mgmt_ndx.pp_port_ndx,
            profile_ndx->pmf_pgm_mgmt_ndx.pfg_ndx,
            &(profile_params->pmf_pgm_mgmt_params.pp_port),
            internal_hw_ndx,
            first_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_SW_DB_MULTI_SET_OUT_OF_RANGE_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_profile_to_hw_add()", 0, 0);
}

uint32
  soc_pb_profile_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PROFILE_UNSAFE);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_profile_unsafe()", 0, 0);
}


void
  SOC_PB_PROFILE_PARAMS_clear(
    SOC_SAND_OUT SOC_PB_PROFILE_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PROFILE_PARAMS));
  SOC_PB_PMF_PGM_MGMT_PARAMS_clear(&(info->pmf_pgm_mgmt_params));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PROFILE_NDX_clear(
    SOC_SAND_OUT SOC_PB_PROFILE_NDX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PROFILE_NDX));
  SOC_PB_PMF_PGM_MGMT_NDX_clear(&(info->pmf_pgm_mgmt_ndx));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

