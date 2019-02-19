#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_mpls_term.c,v 1.8 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MPLS

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
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mpls_term.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Set the lookup to perfrom for MPLS tunnel termination
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_lkup_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LKUP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  res = arad_pp_mpls_term_lkup_info_set_verify(
          unit,
          lkup_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_lkup_info_set_unsafe(
          unit,
          lkup_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_lkup_info_set()", 0, 0);
}

/*********************************************************************
*     Set the lookup to perfrom for MPLS tunnel termination
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LKUP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lkup_info);

  res = arad_pp_mpls_term_lkup_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_lkup_info_get_unsafe(
          unit,
          lkup_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_lkup_info_get()", 0, 0);
}

/*********************************************************************
*     Set the range of MPLS labels that may be used as
 *     tunnels, and enable terminating those tables
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_label_range_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(label_range_info);

  res = arad_pp_mpls_term_label_range_set_verify(
          unit,
          range_ndx,
          label_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_label_range_set_unsafe(
          unit,
          range_ndx,
          label_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_range_set()", range_ndx, 0);
}

/*********************************************************************
*     Set the range of MPLS labels that may be used as
 *     tunnels, and enable terminating those tables
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_label_range_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(label_range_info);

  res = arad_pp_mpls_term_label_range_get_verify(
          unit,
          range_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_label_range_get_unsafe(
          unit,
          range_ndx,
          label_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_range_get()", range_ndx, 0);
}

/*********************************************************************
*     Enable / Disable termination of each label in the MPLS
 *     tunnels range
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_range_terminated_label_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_mpls_term_range_terminated_label_set_verify(
          unit,
          label_ndx,
          is_terminated_label
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_range_terminated_label_set_unsafe(
          unit,
          label_ndx,
          is_terminated_label
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_terminated_label_set()", label_ndx, 0);
}

/*********************************************************************
*     Enable / Disable termination of each label in the MPLS
 *     tunnels range
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_range_terminated_label_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT uint8                                 *is_terminated_label
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_terminated_label);

  res = arad_pp_mpls_term_range_terminated_label_get_verify(
          unit,
          label_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_range_terminated_label_get_unsafe(
          unit,
          label_ndx,
          is_terminated_label
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_range_terminated_label_get()", label_ndx, 0);
}

/*********************************************************************
*     Processing information for the MPLS reserved labels.
 *     MPLS Reserved labels are from 0 to 15.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_reserved_labels_global_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(reserved_labels_info);

  res = arad_pp_mpls_term_reserved_labels_global_info_set_verify(
          unit,
          reserved_labels_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_reserved_labels_global_info_set_unsafe(
          unit,
          reserved_labels_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_labels_global_info_set()", 0, 0);
}

/*********************************************************************
*     Processing information for the MPLS reserved labels.
 *     MPLS Reserved labels are from 0 to 15.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_reserved_labels_global_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(reserved_labels_info);

  res = arad_pp_mpls_term_reserved_labels_global_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_reserved_labels_global_info_get_unsafe(
          unit,
          reserved_labels_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_labels_global_info_get()", 0, 0);
}

/*********************************************************************
*     Set the per-reserved label processing information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_reserved_label_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(label_info);

  res = arad_pp_mpls_term_reserved_label_info_set_verify(
          unit,
          label_ndx,
          label_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_reserved_label_info_set_unsafe(
          unit,
          label_ndx,
          label_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_label_info_set()", label_ndx, 0);
}

/*********************************************************************
*     Set the per-reserved label processing information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_reserved_label_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(label_info);

  res = arad_pp_mpls_term_reserved_label_info_get_verify(
          unit,
          label_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_reserved_label_info_get_unsafe(
          unit,
          label_ndx,
          label_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_reserved_label_info_get()", label_ndx, 0);
}

/*********************************************************************
*     Diagnostic tool: Indicates the terminated MPLS label
 *     Ids.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_encountered_entries_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels,
    SOC_SAND_OUT uint32                                  *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(nof_encountered_labels);
  SOC_SAND_CHECK_NULL_INPUT(encountered_labels);
  SOC_SAND_CHECK_NULL_INPUT(next_label_id);

  res = arad_pp_mpls_term_encountered_entries_get_block_verify(
          unit,
          first_label_ndx,
          nof_encountered_labels
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_encountered_entries_get_block_unsafe(
          unit,
          first_label_ndx,
          nof_encountered_labels,
          encountered_labels,
          next_label_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_encountered_entries_get_block()", first_label_ndx, 0);
}

/*********************************************************************
*     Set information of resolving COS parameters whenever
 *     MPLS label is terminated.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_cos_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_COS_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_cos_info);

  res = arad_pp_mpls_term_cos_info_set_verify(
          unit,
          term_cos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_cos_info_set_unsafe(
          unit,
          term_cos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_cos_info_set()", 0, 0);
}

/*********************************************************************
*     Set information of resolving COS parameters whenever
 *     MPLS label is terminated.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_cos_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_COS_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_cos_info);

  res = arad_pp_mpls_term_cos_info_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_cos_info_get_unsafe(
          unit,
          term_cos_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_cos_info_get()", 0, 0);
}

/*********************************************************************
*     Set mapping from terminated label fields (EXP) to COS
 *     parameters TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_label_to_cos_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cos_key);
  SOC_SAND_CHECK_NULL_INPUT(cos_val);

  res = arad_pp_mpls_term_label_to_cos_info_set_verify(
          unit,
          cos_key,
          cos_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_label_to_cos_info_set_unsafe(
          unit,
          cos_key,
          cos_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_to_cos_info_set()", 0, 0);
}

/*********************************************************************
*     Set mapping from terminated label fields (EXP) to COS
 *     parameters TC and DP.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_label_to_cos_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cos_key);
  SOC_SAND_CHECK_NULL_INPUT(cos_val);

  res = arad_pp_mpls_term_label_to_cos_info_get_verify(
          unit,
          cos_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_label_to_cos_info_get_unsafe(
          unit,
          cos_key,
          cos_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_label_to_cos_info_get()", 0, 0);
}

/*********************************************************************
*     Set termination profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_profile_info_set(
	SOC_SAND_IN  int                        		unit,
	SOC_SAND_IN  uint32 								term_profile_ndx, 
	SOC_SAND_IN  ARAD_PP_MPLS_TERM_PROFILE_INFO 		*term_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_profile_info);

  res = arad_pp_mpls_term_profile_info_set_verify(
          unit,
          term_profile_ndx, 
 		  term_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_profile_info_set_unsafe(
          unit,
          term_profile_ndx, 
 		  term_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_profile_info_set()", 0, 0);
}

/*********************************************************************
*     Get termination profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_profile_info_get(
	SOC_SAND_IN  int                      	  unit,
	SOC_SAND_IN  uint32 						      term_profile_ndx, 
	SOC_SAND_OUT ARAD_PP_MPLS_TERM_PROFILE_INFO 	  *term_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(term_profile_info);

  res = arad_pp_mpls_term_profile_info_get_verify(
          unit,
          term_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_profile_info_get_unsafe(
          unit,
          term_profile_ndx,
          term_profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_profile_info_get()", 0, 0);
}

/*********************************************************************
*     Set termination action profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mpls_term_action_set(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_IN  ARAD_PP_ACTION_PROFILE         			*action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ACTION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  res = arad_pp_mpls_term_action_set_verify(
          unit,
          action_profile_ndx,
		  action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_action_set_unsafe(
          unit,
          action_profile_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_action_set()", 0, 0);
}

/*********************************************************************
*     Get termination action profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_pp_mpls_term_action_get(
	  SOC_SAND_IN  int                             	unit,
	  SOC_SAND_IN  uint32                             	action_profile_ndx,
	  SOC_SAND_OUT ARAD_PP_ACTION_PROFILE         			*action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MPLS_TERM_ACTION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  res = arad_pp_mpls_term_action_get_verify(
          unit,
          action_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_mpls_term_action_get_unsafe(
          unit,
          action_profile_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mpls_term_action_get()", 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_RANGE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LKUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LKUP_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_RANGE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LABEL_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_COS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_COS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_COS_KEY_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LABEL_COS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_COS_VAL_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_LABEL_COS_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_MPLS_TERM_MODEL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_MODEL_TYPE enum_val
  )
{
  return SOC_PPC_MPLS_TERM_MODEL_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_MPLS_TERM_KEY_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_KEY_TYPE enum_val
  )
{
  return SOC_PPC_MPLS_TERM_KEY_TYPE_to_string(enum_val);
}

void
  ARAD_PP_MPLS_TERM_LABEL_RANGE_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_RANGE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LKUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LKUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LKUP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_INFO_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_RANGE_INFO_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_RANGE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_RESERVED_LABEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_COS_INFO_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_COS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_COS_KEY_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_COS_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_COS_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_MPLS_TERM_LABEL_COS_VAL_print(
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_LABEL_COS_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_MPLS_TERM_LABEL_COS_VAL_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

