/* $Id: arad_pp_mpls_term.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_JER_PP_MPLS_TERM_INCLUDED__
/* { */
#define __SOC_JER_PP_MPLS_TERM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <shared/swstate/sw_state.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define JER_MPLS_TERM  sw_state_access[unit].dpp.soc.jericho.pp.mpls_term

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
typedef struct jer_pp_mpls_term_s {
    SOC_PPD_LIF_ID local_mldp_dummy_lif_id[2]; /* In-LIF-ID of IN-LIF-dummy for temination of mLDP,must be set if mLDP is support */
} jer_pp_mpls_term_t; 

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
 *   soc_jer_pp_mpls_termination_spacial_labels_init
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Init MPLS special labels termination mechanism
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
soc_error_t soc_jer_pp_mpls_termination_spacial_labels_init(int unit);

/********************************************************************* 
This function receives information regarding a certain label range (defined by the user) and places it in a given entry.
@params: 
        1) entry_index : a free entry in which the range will be allocated. Range: 0-7.
        2) range_action_info: info regarding all the relvant fields to be inserted into the entry. Members of this struct:
           label_low: lower limit of the label range.
           label_high: higher limit of the label range.
           bos_value: label expected bos value.
           bos_value_mask: when it equals 0, we check the bos value.
@returns: 
        No return value. 
*********************************************************************/
soc_error_t soc_jer_pp_mpls_termination_range_action_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info);
soc_error_t soc_jer_pp_mpls_termination_range_action_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info);

/********************************************************************* 
This function receives information regarding a profile of a certain label range and places it in a 
calculated offset in the register, according to the given entry in the label range tabel 
(which is set in soc_jer_pp_mpls_termination_range_action_set).
@params: 
        1) entry_index : the entry to which this profile is attached. Range: 0-7.
        2) range_profile_info: info regarding the profile. Members of this struct:
           mpls_label_range_tag_mode: tag mode indication.
           mpls_label_range_has_cw: has cw above label indication.
           mpls_label_range_set_outer_vid: outer vid valid indicaiton.
           mpls_label_range_set_inner_vid: inner vid valid indication.
           mpls_label_range_use_base: If set, replace label with lower limit of range.
@returns: 
        No return value. 
*********************************************************************/
soc_error_t soc_jer_pp_mpls_termination_range_profile_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info);
soc_error_t soc_jer_pp_mpls_termination_range_profile_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_MPLS_TERM_INCLUDED__*/
#endif
