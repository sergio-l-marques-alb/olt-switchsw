/* $Id: arad_pp_frwrd_fcf.c,v 1.7 Broadcom SDK $
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
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_extend_p2p.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>

#include <soc/dpp/PPC/ppc_api_fp.h>

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

void
  arad_pp_frwrd_extend_p2p_lem_entry_qals_fill(int unit, SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info, SOC_PPC_FP_QUAL_VAL * qual_vals) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[0]);
    SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[1]);
    SOC_PPC_FP_QUAL_VAL_clear(&qual_vals[2]);

    qual_vals[0].val.arr[0] = fwd_match_info->in_lif;
    qual_vals[0].val.arr[1] = 0;
    qual_vals[0].type = SOC_PPC_FP_QUAL_IRPP_IN_LIF;

    qual_vals[1].val.arr[0] = fwd_match_info->outer_tag;
    qual_vals[1].val.arr[1] = 0;
    qual_vals[1].type = SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID;

    if (fwd_match_info->nof_tags == 2/*double*/) {
        qual_vals[2].val.arr[0] = fwd_match_info->inner_tag;
        qual_vals[2].val.arr[1] = 0;
        qual_vals[2].type = SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID;
    }

    SOCDNX_FUNC_RETURN_VOID;
}

soc_error_t
  arad_pp_frwrd_extend_p2p_lem_entry_add(int unit, 
                                         SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info,
                                         SOC_PPC_FRWRD_DECISION_INFO *frwrd_info) 
{
    int rv;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE success;
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;

    SOCDNX_INIT_FUNC_DEFS;

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    SOC_PPC_FP_QUAL_VAL_clear(qual_vals);

    rv = arad_pp_fwd_decision_in_buffer_build(
       unit,
       ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
       frwrd_info,
       &payload.dest,
       &payload.asd
       );
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    payload.flags = ARAD_PP_LEM_ACCESS_ASD_OUTLIF;

    arad_pp_frwrd_extend_p2p_lem_entry_qals_fill(unit, fwd_match_info, qual_vals);

    if (fwd_match_info->nof_tags == 1/*single_tag*/) {
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_SINGLE_TAG, qual_vals, 0, &payload, &success);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    } else { /* double tag */
        rv = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_DOUBLE_TAG, qual_vals, 0, &payload, &success);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    }

    if (SOC_SAND_SUCCESS2BOOL(success)) {
        LOG_CLI((BSL_META("Entry updated successfuly\n")));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Failed updating entry\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_frwrd_extend_p2p_lem_entry_remove(int unit, 
                                         SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info) 
{
    int rv;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    SOC_SAND_SUCCESS_FAILURE success;

    SOCDNX_INIT_FUNC_DEFS;

    arad_pp_frwrd_extend_p2p_lem_entry_qals_fill(unit, fwd_match_info, qual_vals);

    if (fwd_match_info->nof_tags == 1/*single_tag*/) {
        rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_SINGLE_TAG, qual_vals, &success);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    } else { /* double tag */
        rv = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_DOUBLE_TAG, qual_vals, &success);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    }

    if (SOC_SAND_SUCCESS2BOOL(success)) {
        LOG_CLI((BSL_META("Entry deleted successfuly\n")));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Failed deleting entry\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_frwrd_extend_p2p_lem_entry_get(int unit, 
                                         SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info,
                                         SOC_PPC_FRWRD_DECISION_INFO *frwrd_info,
                                         uint8 * found) 

{
    int rv;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_DECISION_INFO_clear(frwrd_info);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    arad_pp_frwrd_extend_p2p_lem_entry_qals_fill(unit, fwd_match_info, qual_vals);

    if (fwd_match_info->nof_tags == 1/*single_tag*/) {
        rv = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_SINGLE_TAG, qual_vals, &payload, 0/*priority*/, 0/*hit_bit*/, found);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    } else { /* double tag */
        rv = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_VPWS_TAGGED_DOUBLE_TAG, qual_vals, &payload, 0/*priority*/, 0/*hit_bit*/, found);
        SOCDNX_SAND_IF_ERR_EXIT(rv);
    }

    rv = arad_pp_fwd_decision_in_buffer_parse(unit, payload.dest, payload.asd, ARAD_PP_FWD_DECISION_PARSE_DEST|ARAD_PP_FWD_DECISION_PARSE_OULTIF/*flags*/, frwrd_info);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>

