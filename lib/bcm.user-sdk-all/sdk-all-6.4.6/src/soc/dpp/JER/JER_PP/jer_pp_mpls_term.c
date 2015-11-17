/* $Id: arad_pp_mpls_term.c,v 1.29 Broadcom SDK $
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
#include <soc/dcmn/utils.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>

#include <soc/dpp/PPC/ppc_api_mpls_term.h>
#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>
#include <shared/swstate/access/sw_state_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_NOF_BITS           (9)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_TO_TERMINATE_WIDTH         (4)

#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_IS_OAM_OFFSET (0)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_VALUE_OFFSET (1)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_OFFSET (5)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_HAS_CW_OFFSET (6)
#define _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_LABELS_TO_TERMINATE_OFFSET (7)

#define _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS          (5)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_SHIFT        (0)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_SHIFT          (1)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_SHIFT   (2)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_SHIFT   (3)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_SHIFT        (4)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_BIT          (1)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_BIT            (2)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_BIT     (4)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_BIT     (8)
#define _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_BIT          (16)

#define _SOC_JER_PP_MPLS_TERM_LABEL_MAX  ((1 << 20) - 1)

#define _SOC_JER_PP_MPLS_TERM_LABEL_IN_RANGE(_label_)   \
        (_label_ <= _SOC_JER_PP_MPLS_TERM_LABEL_MAX)



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

/* Index in the 'for' loop. Not used for anything else */
typedef enum {
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_ELI = 0,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_GAL,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV4_EXP,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV6_EXP,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_RA,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_OAM_ALERT,
    _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_NOF
} _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX;

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
 *     Init MPLS special labels termination mechanism
 *     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t soc_jer_pp_mpls_termination_spacial_labels_init(int unit) {
    uint32 reg, field_val;
    soc_reg_above_64_val_t  reg_above_64; 
    uint64 reg_64;
    uint32 label, expected_ttl, expected_bos, check_bos, check_ttl;
    uint32 force_is_oam = 0, force_fwd_code = 0, force_fwd_code_value = 0, force_has_cw = 0;
    uint32 labels_to_terminate;
    uint32 special_label_profile;
    uint32 label_index;
    uint32 type;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
	if (SOC_WARM_BOOT(unit)) {
		SOCDNX_FUNC_RETURN;
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/

    
    SOCDNX_SAND_IF_ERR_EXIT(READ_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, &reg));
    field_val = 0x1;
    soc_reg_field_set(unit, EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r, &reg, CFG_MPLS_PIPE_FIX_ENABLEf, field_val);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EPNI_CFG_BUG_FIX_CHICKEN_BITS_REG_2r(unit, REG_PORT_ANY, reg));

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_GENERAL_CONFIGS_1r(unit, REG_PORT_ANY, &reg));
    field_val = 0x1;
    soc_reg_field_set(unit, IHP_VTT_GENERAL_CONFIGS_1r, &reg, MPLS_ENH_TT_ENABLEf, field_val);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_GENERAL_CONFIGS_1r(unit, REG_PORT_ANY, reg));


    
    /* Special labels soc properties are not supported */
    if (soc_property_get(unit, spn_MPLS_ENTROPY_LABEL_INDICATOR_ENABLE, 0)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, 
                             (_BSL_SOCDNX_MSG("Error, Jericho does not support mpls_entropy_label_indicator_enable soc property. Entropy is supported by default.\n")));
    }

    /* Loop on all the special labels setting them according to pre-defined properties.
     *  Note: Explicit NULL label will not be inserted to the special labels TCAM, but will be set in the bitmap specifying the special labels.
     *  This will result in VttMplsIllegalLabelTrap.
     */ 
    for (label_index=0; label_index<_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_NOF; label_index++) {

        /* In case OAM is not enabled on the device, reserved label RouterAlert will not be configured */
        /* The reason is that this way user can add it to the lif table and configure a trap */
        if ((soc_property_get(unit, spn_NUM_OAMP_PORTS, 0) == 0) && 
            (label_index == _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_RA)) {
            continue;
        }

        switch (label_index) {
        case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_ELI:
            label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_ELI;
            expected_ttl = 0;
            expected_bos = 0;
            check_bos = 1;
            check_ttl = 0;
            labels_to_terminate = 2;
            force_is_oam = 0;
            force_fwd_code = 0;
            force_fwd_code_value = 0;
            force_has_cw = 0;
            break;
        case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_GAL:
            label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_GAL;
            expected_ttl = 0;
            expected_bos = 0;
            check_bos = 0;
            check_ttl = 0;
            labels_to_terminate = 1;
            force_is_oam = 0;
            force_fwd_code = 1;
            force_fwd_code_value = ARAD_PP_FWD_CODE_TM; 
            force_has_cw = 1;
            break;
        case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV4_EXP:
            label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV4_EXP;
            expected_ttl = 0;
            expected_bos = 1;
            check_bos = 1;
            check_ttl = 0;
            labels_to_terminate = 1;
            force_is_oam = 0;
            force_fwd_code = 1;
            force_fwd_code_value = 1;
            force_has_cw = 0;
            break;
        case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_IPV6_EXP:
            label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_IPV6_EXP;
            expected_ttl = 0;
            expected_bos = 1;
            check_bos = 1;
            check_ttl = 0;
            labels_to_terminate = 1;
            force_is_oam = 0;
            force_fwd_code = 1;
            force_fwd_code_value = 3;
            force_has_cw = 0;
            break;
        case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_RA: 
            label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_RA;
            expected_ttl = 0;
            expected_bos = 0;
            check_bos = 1;
            check_ttl = 0;
            labels_to_terminate = 1;
            force_is_oam = 1;
            force_fwd_code = 0;
            force_fwd_code_value = 0;
            force_has_cw = 0;
            break;
        case _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_INDEX_OAM_ALERT: 
            label = SOC_PPC_MPLS_TERM_RESERVED_LABEL_OAM_ALERT;
            expected_ttl = 0;
            expected_bos = 1;
            check_bos = 1;
            check_ttl = 0;
            labels_to_terminate = 1;
            force_is_oam = 0;
            force_fwd_code = 0;
            force_fwd_code_value = 0;
            force_has_cw = 0;
            break;
        }

        /* Init VttMplsLabelTcam - 6 entries tcam for special label match */
        SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_TCAMm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), label_index, reg_above_64));
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, KEYf, &label);
        field_val = 0;
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, KEY_MASKf, &field_val);
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, EXPECTED_TTLf, &expected_ttl);
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, EXPECTED_BOSf, &expected_bos);
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, CHECK_BOSf, &check_bos);
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, CHECK_TTLf, &check_ttl);
        field_val = 1;
        soc_IHP_VTT_MPLS_LABEL_TCAMm_field_set(unit, reg_above_64, VALIDf, &field_val);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_LABEL_TCAMm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), label_index, reg_above_64));

        /* Init MplsSpecialLabelProfileTable - Special profile per label */
        special_label_profile = (force_is_oam << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_IS_OAM_OFFSET) | 
                                (force_fwd_code_value << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_VALUE_OFFSET) | 
                                (force_fwd_code << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_FORCE_FWD_CODE_OFFSET) | 
                                (force_has_cw << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_HAS_CW_OFFSET) | 
                                (labels_to_terminate << _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_LABELS_TO_TERMINATE_OFFSET);
        SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_MPLS_SPECIAL_LABEL_PROFILE_TABLEr(unit, SOC_CORE_ALL, &reg_64));
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        reg_above_64[0] = COMPILER_64_LO(reg_64);
        reg_above_64[1] = COMPILER_64_HI(reg_64);
        SHR_BITCOPY_RANGE(reg_above_64, _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_NOF_BITS*label_index, &special_label_profile, 0, _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_PROFILE_NOF_BITS);
        COMPILER_64_SET(reg_64, reg_above_64[1], reg_above_64[0]);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_MPLS_SPECIAL_LABEL_PROFILE_TABLEr(unit, SOC_CORE_ALL, reg_64));

        /* Set number of labels to terminate per special label */
        SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_MPLS_SPECIAL_LABELS_TO_TERMINATEr(unit, SOC_CORE_ALL, &reg));
        SHR_BITCOPY_RANGE(&reg, label_index*_SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_TO_TERMINATE_WIDTH, &labels_to_terminate, 0, _SOC_JER_PP_MPLS_TERM_SPECIAL_LABELS_TO_TERMINATE_WIDTH);
        soc_reg_field_set(unit, IHP_MPLS_SPECIAL_LABELS_TO_TERMINATEr, &reg, MPLS_SPECIAL_LABELS_TO_TERMINATEf, reg);
        SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_MPLS_SPECIAL_LABELS_TO_TERMINATEr(unit, SOC_CORE_ALL, reg));
    }

    /* Init VttMplsDummyLif - similar to single entry in Lif table.
       Dummy lif is used for TSE_0 - termination of special label that appears in the beginning of the stack.
       Basically none of the fields of this entry is used beside Termination-Type field
       which is the index of VttMplsEnhDummyLifTerminationProfile and must be set to zero.
     */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    type = 2;
    soc_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm_field_set(unit, reg_above_64, TYPEf, &type);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_DUMMY_LIFr(unit, SOC_CORE_ALL, reg_above_64));

    /* Set maximum number of labels to terminate to 13 */
    reg = 0;
    soc_reg_field_set(unit, IHP_MPLS_LABEL_PROCEDURE_CONFIGr, &reg, MAX_NOF_LABELS_TO_TERMINATEf, 13);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_MPLS_LABEL_PROCEDURE_CONFIGr(unit, SOC_CORE_ALL, reg));

    /* Profile of the special label "lif entry". Set to 0. */
    reg = 0;
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_ENH_DUMMY_LIF_TERMINATION_PROFILEr(unit, SOC_CORE_ALL, reg));

    /* Labels 0-15 (special labels) shall be classified as tunnel label if the corresponding bit in the Bit Map is set.
       All the labels that are not in the TCAM are set here */
    reg = 0x9F70;
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_ENH_EXCLUDE_SPECIAL_LABEL_BIT_MAPr(unit, SOC_CORE_ALL, reg));  

	    if (SOC_DPP_CONFIG(unit)->pp.mldp_support) {
        ARAD_PP_ISEM_ACCESS_KEY                  isem_key;
        ARAD_PP_ISEM_ACCESS_ENTRY                isem_entry;
        SOC_SAND_SUCCESS_FAILURE                 success;
        uint32                                   res = SOC_SAND_OK;
        SOC_PPD_LIF_ID local_mldp_dummy_lif_id[2]; 
        JER_MPLS_TERM.local_mldp_dummy_lif_id.get(unit, 0, &(local_mldp_dummy_lif_id[0])); 
        JER_MPLS_TERM.local_mldp_dummy_lif_id.get(unit, 1, &(local_mldp_dummy_lif_id[1])); 


        /* Add entries to isemb for dummy mpls */
        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        SOCDNX_CLEAR(&isem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
        isem_key.key_info.mpls.flags = ARAD_PP_MPLS_TERM_FLAG_DUMMY_LABEL;
        isem_key.key_info.mpls.is_bos = 0;
        isem_entry.sem_result_ndx = local_mldp_dummy_lif_id[1];
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(success != SOC_SAND_SUCCESS)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                    (_BSL_SOCDNX_MSG("Error, isemb  addition fails dummy mpls lif1 on init, unexpected behavior.\n")));
        }

        SOCDNX_CLEAR(&isem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
        isem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_MPLS;
        isem_key.key_info.mpls.flags = ARAD_PP_MPLS_TERM_FLAG_DUMMY_LABEL;
        isem_key.key_info.mpls.is_bos = 1;
        isem_entry.sem_result_ndx = local_mldp_dummy_lif_id[0];
        res = arad_pp_isem_access_entry_add_unsafe(unit, &isem_key, &isem_entry, &success);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if(success != SOC_SAND_SUCCESS)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                    (_BSL_SOCDNX_MSG("Error, isemb  addition fails dummy mpls lif2 on init, unexpected behavior.\n")));
        }

    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


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
soc_error_t soc_jer_pp_mpls_termination_range_action_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO * range_action_info) {
    soc_reg_above_64_val_t  reg_above_64; 

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_action_info);

    if (!_SOC_JER_PP_MPLS_TERM_LABEL_IN_RANGE(range_action_info->label_high) || \
        !_SOC_JER_PP_MPLS_TERM_LABEL_IN_RANGE(range_action_info->label_low)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error, One of the given labels is out of (0,2^20-1)\n")));
    }

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_TABLEm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), entry_index, reg_above_64));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, RANGE_MINf, &(range_action_info->label_low));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, RANGE_MAXf, &(range_action_info->label_high));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, BOS_VALUEf, &(range_action_info->bos_value));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_set(unit, reg_above_64, BOS_VALUE_MASKf, &(range_action_info->bos_value_mask));
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_LABEL_RANGE_TABLEm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), entry_index, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t soc_jer_pp_mpls_termination_range_action_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO * range_action_info) {
    soc_reg_above_64_val_t  reg_above_64; 

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_action_info);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_TABLEm(unit, IHP_BLOCK(unit, SOC_BLOCK_ALL), entry_index, reg_above_64));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, RANGE_MINf, &(range_action_info->label_low));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, RANGE_MAXf, &(range_action_info->label_high));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, BOS_VALUEf, &(range_action_info->bos_value));
    soc_IHP_VTT_MPLS_LABEL_RANGE_TABLEm_field_get(unit, reg_above_64, BOS_VALUE_MASKf, &(range_action_info->bos_value_mask));
    

exit:
    SOCDNX_FUNC_RETURN;
}


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
soc_error_t soc_jer_pp_mpls_termination_range_profile_set(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO * range_profile_info) {
    soc_reg_above_64_val_t  reg_above_64; 
    uint64 reg_64;
    uint32 range_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_profile_info);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_PROFILEr(unit, SOC_CORE_ALL, &reg_64));

    range_profile = (range_profile_info->mpls_label_range_tag_mode << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_SHIFT) |
                    (range_profile_info->mpls_label_range_has_cw << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_SHIFT) |
                    (range_profile_info->mpls_label_range_set_outer_vid << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_SHIFT) |
                    (range_profile_info->mpls_label_range_set_inner_vid << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_SHIFT) |
                    (range_profile_info->mpls_label_range_use_base << _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_SHIFT);    
    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    reg_above_64[0] = COMPILER_64_LO(reg_64);
    reg_above_64[1] = COMPILER_64_HI(reg_64);
    SHR_BITCOPY_RANGE(reg_above_64, _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS*entry_index, &range_profile, 0, _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS);
    COMPILER_64_SET(reg_64, reg_above_64[1], reg_above_64[0]);
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_VTT_MPLS_LABEL_RANGE_PROFILEr(unit, SOC_CORE_ALL, reg_64));

 

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t soc_jer_pp_mpls_termination_range_profile_get(int unit, uint32 entry_index, SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info) {
    uint64 reg_64;
    soc_reg_above_64_val_t  reg_above_64; 
    uint32 range_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Error, Only Jericho device supports this function\n")));
    }

    SOCDNX_NULL_CHECK(range_profile_info);

    SOCDNX_SAND_IF_ERR_EXIT(READ_IHP_VTT_MPLS_LABEL_RANGE_PROFILEr(unit, SOC_CORE_ALL, &reg_64));    
    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    reg_above_64[0] = COMPILER_64_LO(reg_64);
    reg_above_64[1] = COMPILER_64_HI(reg_64);
    SHR_BITCOPY_RANGE(&range_profile, 0,reg_above_64 , _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS*entry_index, _SOC_JER_PP_MPLS_TERM_RANGE_PROFILE_NOF_FIELDS);
    
    range_profile_info->mpls_label_range_tag_mode = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_TAG_MODE_SHIFT; 
    range_profile_info->mpls_label_range_has_cw = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_HAS_CW_SHIFT;
    range_profile_info->mpls_label_range_set_outer_vid = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_OUTER_VID_SHIFT;
    range_profile_info->mpls_label_range_set_inner_vid = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_SET_INNER_VID_SHIFT ;
    range_profile_info->mpls_label_range_use_base = (range_profile & _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_BIT) \
                                                    >> _SOC_JER_PP_MPLS_TERM_LABEL_RANGE_USE_BASE_SHIFT;

exit:
    SOCDNX_FUNC_RETURN;
}
