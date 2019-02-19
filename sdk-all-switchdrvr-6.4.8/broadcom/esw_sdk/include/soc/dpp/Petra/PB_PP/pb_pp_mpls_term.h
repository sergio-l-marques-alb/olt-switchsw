/* $Id: pb_pp_mpls_term.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_mpls_term.h
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

#ifndef __SOC_PB_PP_MPLS_TERM_INCLUDED__
/* { */
#define __SOC_PB_PP_MPLS_TERM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_mpls_term.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_MPLS_TERM_FIRST,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_RANGE_TERMINATED_LABEL_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLOBAL_INFO_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK,
  SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_PRINT,
  SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_UNSAFE,
  SOC_PB_PP_MPLS_TERM_ENCOUNTERED_ENTRIES_GET_BLOCK_VERIFY,
  SOC_PB_PP_MPLS_TERM_COS_INFO_SET,
  SOC_PB_PP_MPLS_TERM_COS_INFO_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_COS_INFO_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_COS_INFO_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_COS_INFO_GET,
  SOC_PB_PP_MPLS_TERM_COS_INFO_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_COS_INFO_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_COS_INFO_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_PRINT,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_SET_VERIFY,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_PRINT,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_VERIFY,
  SOC_PB_PP_MPLS_TERM_LABEL_TO_COS_INFO_GET_UNSAFE,
  SOC_PB_PP_MPLS_TERM_GET_PROCS_PTR,
  SOC_PB_PP_MPLS_TERM_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_UPDATE_BASES,



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_MPLS_TERM_PROCEDURE_DESC_LAST
} SOC_PB_PP_MPLS_TERM_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_MPLS_TERM_RANGE_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_MPLS_TERM_FIRST,
  SOC_PB_PP_MPLS_TERM_IS_TERMINATED_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_KEY_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_PROCESSING_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_COS_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_NEXT_PRTCL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_MODEL_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_MPLS_TERM_RSRVD_ACTION_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_RSRVD_TRAP_CODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_MPLS_TERM_SIMPLE_RANGE_TERM_NEXT_NOT_MPLS_ERR,
  SOC_PB_PP_MPLS_TERM_RANGE_EXCEED_BITMAP_SIZE_ERR,
  SOC_PB_PP_MPLS_TERM_RANGE_LAST_SMALLER_THAN_FIRST_ERR,
  SOC_PB_PP_MPLS_TERM_RANGE_NOT_KEEP_ORDER_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_MPLS_TERM_ERR_LAST
} SOC_PB_PP_MPLS_TERM_ERR;

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
  soc_pb_pp_mpls_term_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );
/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_lkup_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the lookup to perfrom for MPLS tunnel termination
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info -
 *     Lookup type to perfrom for MPLS tunnel termination, may
 *     be tunnel, <tunnel,inRIF>
 * REMARKS:
 *   - Used for tunnel termination for both: MPLS label or
 *   PWE termination
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_lkup_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

uint32
  soc_pb_pp_mpls_term_lkup_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

uint32
  soc_pb_pp_mpls_term_lkup_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_lkup_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_mpls_term_lkup_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_lkup_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_label_range_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the range of MPLS labels that may be used as
 *   tunnels, and enable terminating those tables
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 range_ndx -
 *     There are 3 different ranges of labels. Range: 0-2.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info -
 *     MPLS label ranges, of terminated MPLS tunnels. Separated
 *     ranges for pipe and uniform handling
 * REMARKS:
 *   - T20E: Range '0' should be configured to PIPE Range '1'
 *   should be configured to UNIFORM Range '2' is currently
 *   invalid- Soc_petra-B: Tunnel outside the tunnel termination
 *   range may be terminated by calling to
 *   soc_ppd_rif_mpls_label_map_add()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

uint32
  soc_pb_pp_mpls_term_label_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

uint32
  soc_pb_pp_mpls_term_label_range_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_label_range_set_unsafe" API.
 *     Refer to "soc_pb_pp_mpls_term_label_range_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_range_terminated_label_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable / Disable termination of each label in the MPLS
 *   tunnels range
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx -
 *     MPLS Label ID. Range: 0-2^20-1
 *   SOC_SAND_IN  uint8                                 is_terminated_label -
 *     TRUE: Label is terminated as tunnelFALSE: Label is not
 *     terminated, although it is in the termination range
 * REMARKS:
 *   - Return error if label is not in the range, configured
 *   by soc_ppd_mpls_term_label_range_set().- Soc_petra-B: Tunnel
 *   outside the tunnel termination range may be terminated
 *   by calling to soc_ppd_rif_mpls_label_map_add().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_range_terminated_label_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  );

uint32
  soc_pb_pp_mpls_term_range_terminated_label_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  );

uint32
  soc_pb_pp_mpls_term_range_terminated_label_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_range_terminated_label_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_mpls_term_range_terminated_label_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_range_terminated_label_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT uint8                                 *is_terminated_label
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_reserved_labels_global_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Processing information for the MPLS reserved labels.
 *   MPLS Reserved labels are from 0 to 15.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info -
 *     SOC_SAND_IN SOC_PPD_MPLS_TERM_RESERVED_LABELS_GLBL_INFO
 *     *reserved_labels_info
 * REMARKS:
 *   - T20E only. The per reserved label processing
 *   information is configured by
 *   soc_ppd_mpls_term_reserved_label_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_reserved_labels_global_info_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_mpls_term_reserved_labels_global_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_reserved_label_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the per-reserved label processing information
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx -
 *     Reserved label IDRange: 0-15
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info -
 *     Termination methods for MPLS labels 0-15
 * REMARKS:
 *   T20E: The global reserved labels processing information
 *   is configured by
 *   soc_ppd_mpls_term_reserved_labels_global_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_label_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

uint32
  soc_pb_pp_mpls_term_reserved_label_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

uint32
  soc_pb_pp_mpls_term_reserved_label_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_reserved_label_info_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_mpls_term_reserved_label_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_label_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_encountered_entries_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Diagnostic tool: Indicates the terminated MPLS label
 *   Ids.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx -
 *     First MPLS label to scan
 *   SOC_SAND_INOUT uint32                                  *nof_encountered_labels -
 *     In: encountered_labels sizeOut: Number of encountered
 *     labels, stored in encountered_labels
 *   SOC_SAND_OUT uint32                                  *encountered_labels -
 *     Array of encountered labels. Each entry holds the label
 *     ID
 *   SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id -
 *     When (out nof_encountered_lifs == in
 *     nof_encountered_lifs), there may be more encounters
 *     labels, and the user may recall the function with
 *     first_label_ndx = next_label_id
 * REMARKS:
 *   Returns the label Ids which are terminated, starting
 *   from first label by traversing the terminated range and
 *   adding to the list only those for which the valid bit is
 *   set
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_encountered_entries_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels,
    SOC_SAND_OUT uint32                                  *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id
  );

uint32
  soc_pb_pp_mpls_term_encountered_entries_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_cos_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set information of resolving COS parameters whenever
 *   MPLS label is terminated.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info -
 *     How to use terminated tunnel in the calculation of the
 *     COS parameters.
 * REMARKS:
 *   - Relevant only for T20E. Error is returned if called
 *   for Soc_petra-B.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  );

uint32
  soc_pb_pp_mpls_term_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  );

uint32
  soc_pb_pp_mpls_term_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_cos_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_mpls_term_cos_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_label_to_cos_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from terminated label fields (EXP) to COS
 *   parameters TC and DP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key -
 *     SOC_SAND_IN SOC_PPD_MPLS_TERM_LABEL_COS_KEY *cos_key
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val -
 *     SOC_SAND_IN SOC_PPD_MPLS_TERM_LABEL_COS_VAL *cos_val
 * REMARKS:
 *   - Relevant only for T20E. Error is returned if called
 *   for Soc_petra-B.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_to_cos_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

uint32
  soc_pb_pp_mpls_term_label_to_cos_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

uint32
  soc_pb_pp_mpls_term_label_to_cos_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_label_to_cos_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_mpls_term_label_to_cos_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_to_cos_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_mpls_term module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_mpls_term_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_mpls_term module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_mpls_term_get_errs_ptr(void);

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_COS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY *info
  );

uint32
  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_MPLS_TERM_INCLUDED__*/
#endif

