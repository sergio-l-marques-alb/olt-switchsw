/* $Id: pb_pp_api_mpls_term.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_mpls_term.h
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

#ifndef __SOC_PB_PP_API_MPLS_TERM_INCLUDED__
/* { */
#define __SOC_PB_PP_API_MPLS_TERM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_mpls_term.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     MPLS Labels 0-15 are reserved labels                    */

/*     Maximum number of terminated                    */


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

#define SOC_PB_PP_MPLS_TERM_MODEL_PIPE                         SOC_PPC_MPLS_TERM_MODEL_PIPE
#define SOC_PB_PP_MPLS_TERM_MODEL_UNIFORM                      SOC_PPC_MPLS_TERM_MODEL_UNIFORM
#define SOC_PB_PP_NOF_MPLS_TERM_MODEL_TYPES                    SOC_PPC_NOF_MPLS_TERM_MODEL_TYPES
typedef SOC_PPC_MPLS_TERM_MODEL_TYPE                           SOC_PB_PP_MPLS_TERM_MODEL_TYPE;

#define SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL                     SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL
#define SOC_PB_PP_MPLS_TERM_KEY_TYPE_LABEL_RIF                 SOC_PPC_MPLS_TERM_KEY_TYPE_LABEL_RIF
#define SOC_PB_PP_NOF_MPLS_TERM_KEY_TYPES                      SOC_PPC_NOF_MPLS_TERM_KEY_TYPES
typedef SOC_PPC_MPLS_TERM_KEY_TYPE                             SOC_PB_PP_MPLS_TERM_KEY_TYPE;

typedef SOC_PPC_MPLS_TERM_LABEL_RANGE                          SOC_PB_PP_MPLS_TERM_LABEL_RANGE;
typedef SOC_PPC_MPLS_TERM_LKUP_INFO                            SOC_PB_PP_MPLS_TERM_LKUP_INFO;
typedef SOC_PPC_MPLS_TERM_INFO                                 SOC_PB_PP_MPLS_TERM_INFO;
typedef SOC_PPC_MPLS_TERM_LABEL_RANGE_INFO                     SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO;
typedef SOC_PPC_MPLS_TERM_RESERVED_LABEL_INFO                  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO;
typedef SOC_PPC_MPLS_TERM_RESERVED_LABELS_GLBL_INFO            SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO;
typedef SOC_PPC_MPLS_TERM_COS_INFO                             SOC_PB_PP_MPLS_TERM_COS_INFO;
typedef SOC_PPC_MPLS_TERM_LABEL_COS_KEY                        SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY;
typedef SOC_PPC_MPLS_TERM_LABEL_COS_VAL                        SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL;

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
* NAME:
 *   soc_pb_pp_mpls_term_lkup_info_set
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
  soc_pb_pp_mpls_term_lkup_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_lkup_info_set" API.
 *     Refer to "soc_pb_pp_mpls_term_lkup_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LKUP_INFO                 *lkup_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_label_range_set
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
  soc_pb_pp_mpls_term_label_range_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_label_range_set" API.
 *     Refer to "soc_pb_pp_mpls_term_label_range_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_range_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 range_ndx,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO          *label_range_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_range_terminated_label_set
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
  soc_pb_pp_mpls_term_range_terminated_label_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  uint8                                 is_terminated_label
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_range_terminated_label_set" API.
 *     Refer to "soc_pb_pp_mpls_term_range_terminated_label_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_range_terminated_label_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT uint8                                 *is_terminated_label
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_reserved_labels_global_info_set
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
  soc_pb_pp_mpls_term_reserved_labels_global_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_reserved_labels_global_info_set" API.
 *     Refer to
 *     "soc_pb_pp_mpls_term_reserved_labels_global_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_labels_global_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *reserved_labels_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_reserved_label_info_set
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
  soc_pb_pp_mpls_term_reserved_label_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_reserved_label_info_set" API.
 *     Refer to "soc_pb_pp_mpls_term_reserved_label_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_reserved_label_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        label_ndx,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO       *label_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_encountered_entries_get_block
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
  soc_pb_pp_mpls_term_encountered_entries_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        first_label_ndx,
    SOC_SAND_INOUT uint32                                  *nof_encountered_labels,
    SOC_SAND_OUT uint32                                  *encountered_labels,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_LABEL                        *next_label_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_cos_info_set
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
  soc_pb_pp_mpls_term_cos_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_cos_info_set" API.
 *     Refer to "soc_pb_pp_mpls_term_cos_info_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_cos_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_COS_INFO                  *term_cos_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_mpls_term_label_to_cos_info_set
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
  soc_pb_pp_mpls_term_label_to_cos_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_mpls_term_label_to_cos_info_set" API.
 *     Refer to "soc_pb_pp_mpls_term_label_to_cos_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_mpls_term_label_to_cos_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY             *cos_key,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL             *cos_val
  );

void
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_RANGE *info
  );

void
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LKUP_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_INFO *info
  );
void
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_COS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_COS_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY *info
  );

void
  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_MPLS_TERM_MODEL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_MODEL_TYPE enum_val
  );

const char*
  SOC_PB_PP_MPLS_TERM_KEY_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_KEY_TYPE enum_val
  );

void
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE *info
  );

void
  SOC_PB_PP_MPLS_TERM_LKUP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LKUP_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO *info
  );
void
  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_RANGE_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABEL_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_RESERVED_LABELS_GLBL_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_COS_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_COS_INFO *info
  );

void
  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_KEY *info
  );

void
  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_LABEL_COS_VAL *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_MPLS_TERM_INCLUDED__*/
#endif

