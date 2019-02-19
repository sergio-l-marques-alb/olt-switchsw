/* $Id: arad_pp_oam.c,v 1.111 Broadcom SDK $
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
 * $
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM
#include <shared/bsl.h>

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

#include <soc/mcm/memregs.h>
#include <soc/mcm/memacc.h>
#include <soc/mem.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>


#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */



/* MDL-MP-TYPE encoding*/
#define _JER_PP_OAM_MDL_MP_TYPE_NO_MP 0
#define _JER_PP_OAM_MDL_MP_TYPE_MIP 1
#define _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH 2
#define _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH 3


/* mp_type_entry is a concatination of the bits OAM-is-BFD, MDL-MP-Type(2), Above-MDL-MEP-Bitmap-OR*/
#define _JER_PP_OAM_MP_TYPE_MAP_IS_BFD(mp_type_entry) (mp_type_entry >>3)
#define _JER_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_entry) ((mp_type_entry>>1) &0x3)
#define _JER_PP_OAM_MP_TYPE_MAP_ABOVE_MDL_MEP_BITMAP_OR(mp_type_entry) (mp_type_entry & 0x1) 


/** 
 *  Mapping is as following:
 *  MIP (1) --> MIP-match (0) ,
 *  active-match(2) --> active_match(1)
 *  passive-match(3)-->passive-match(2) */
#define _JER_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(mdl_mp_type) (mdl_mp_type -1)




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

/**********************************************/ 
/*                   static functions declerations*/
/**********************************************/ 

STATIC soc_error_t _soc_jer_pp_oam_set_opcode_n_table(int unit);

/**********************************************/ 
/* *****************************************    */              
/**********************************************/ 


soc_error_t soc_jer_pp_oam_init(int unit)
{
    int rv;
    uint32 mp_type_index,reg32;
    uint64 reg_64;
    uint32 mp_type_reg[2];
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS; 

    /* MP-type static configuration*/

    /*following encoding is used: 
    0 - MIP-Match 
    1 - Active-Match 
    2 - Passive-Match 
    3 - Below-Highest-MEP 
    4 - Above-All 
    5 - BFD 
    table is accessed by:  
     OAM-is-BFD, MDL-MP-Type(2), Above-MDL-MEP-Bitmap-OR
    */

    for (mp_type_index=0; mp_type_index<0x10 /*all combinations of 4 bits*/; ++mp_type_index ) {
        uint32 mp_type;
        if (_JER_PP_OAM_MP_TYPE_MAP_IS_BFD(mp_type_index)) {
            mp_type = SOC_PPC_OAM_MP_TYPE_JERICHO_BFD;
        } else  if (_JER_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_index)==_JER_PP_OAM_MDL_MP_TYPE_NO_MP ) {
            mp_type = _JER_PP_OAM_MP_TYPE_MAP_ABOVE_MDL_MEP_BITMAP_OR(mp_type_index) ? SOC_PPC_OAM_MP_TYPE_JERICHO_BELLOW_HIGHEST_MEP :
                SOC_PPC_OAM_MP_TYPE_JERICHO_ABOVE_ALL;
        } else {
            mp_type = _JER_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(_JER_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_index));
        }
        SHR_BITCOPY_RANGE(mp_type_reg, (3 * mp_type_index), &mp_type, 0, 3);
    }

    COMPILER_64_SET(reg_64, mp_type_reg[1], mp_type_reg[0]);
    rv = WRITE_IHP_OAM_MP_TYPE_MAPr(unit,SOC_CORE_ALL,reg_64);
    SOCDNX_IF_ERR_EXIT(rv);


    /* BASE MAC SA FILL. Always zero.*/
    rv = READ_OAMP_CCM_MAC_SAr(unit,reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field32_set(unit,OAMP_CCM_MAC_SAr,reg_above_64,BASE_MAC_SA_FILLf,0);
    rv = WRITE_OAMP_CCM_MAC_SAr(unit,reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_OAMP_UP_PTCHr(unit,&reg32);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit,OAMP_UP_PTCHr, &reg32, UP_PTCH_OPAQUE_PT_ATTR_PROFILE_0f, 0x7);
    soc_reg_field_set(unit,OAMP_UP_PTCHr, &reg32, UP_PTCH_OPAQUE_PT_ATTR_PROFILE_1f, 0x7);
    soc_reg_field_set(unit,OAMP_UP_PTCHr, &reg32, UP_PTCH_OPAQUE_PT_ATTR_MODEf, 0);/*use the MAC SA in the sensible way.*/
    rv = WRITE_OAMP_UP_PTCHr(unit,reg32);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = _soc_jer_pp_oam_set_opcode_n_table(unit);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;
}

/* ****************************************************************************/
/*****************************************************************************/


soc_error_t soc_jer_pp_oam_oem1_mep_add(
        int unit,
        const SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
        const SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * prev_payload,
        SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * new_payload,
        uint8 is_active,
        uint8 update) {
    int is_mep;
    uint8 prev_mdl_mp_type, new_mdl_mp_type;
    uint32 new_mp_type_vector;
    SOCDNX_INIT_FUNC_DEFS;

    is_mep = ((classifier_mep_entry->flags & SOC_PPD_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT)!= 0);

    /* first: Get the relavent MDL-MP-Type, by level */
    prev_mdl_mp_type = JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(prev_payload->mp_type_vector, classifier_mep_entry->md_level);
    if (prev_mdl_mp_type && !update) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS,
                         (_BSL_SOCDNX_MSG("Only one MEP/MIP per LIF may be defined.")));
    }
    new_mdl_mp_type = (!is_mep)? _JER_PP_OAM_MDL_MP_TYPE_MIP : (is_active)? _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH : 
        _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH;

    new_mp_type_vector = prev_payload->mp_type_vector;

    JERICHO_PP_OAM_SET_MDL_MP_TYPE_VECTOR_BY_LEVEL(new_mp_type_vector,new_mdl_mp_type,classifier_mep_entry->md_level  );
    
    new_payload->mp_type_vector =new_mp_type_vector; 
    new_payload->mp_profile = classifier_mep_entry->non_acc_profile;
    new_payload->counter_ndx = classifier_mep_entry->counter;

exit:
    SOCDNX_FUNC_RETURN;
}


/* ****************************************************************************/
/*****************************************************************************/

soc_error_t soc_jer_pp_oam_oamp_eth1731_profile_set(
    int                                 unit,
    uint8                          profile_indx,
    const SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    int rv;
    soc_reg_above_64_val_t profile_entry;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(profile_entry);


    soc_OAMP_MEP_PROFILEm_field32_set(unit,profile_entry, DMM_RATEf, eth1731_profile->dmm_rate);
    soc_OAMP_MEP_PROFILEm_field32_set(unit,profile_entry, DMM_OFFSETf, eth1731_profile->dmm_offset);
    soc_OAMP_MEP_PROFILEm_field32_set(unit,profile_entry, DMR_OFFSETf, eth1731_profile->dmr_offset);

    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, LMM_RATEf, eth1731_profile->lmm_rate);
    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, LMM_OFFSETf, eth1731_profile->lmm_offset);
    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, LMR_OFFSETf, eth1731_profile->lmr_offset);
    
    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, PIGGYBACK_LMf, eth1731_profile->piggy_back_lm);

    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, RDI_GEN_METHODf, eth1731_profile->rdi_gen_method);

    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, LMM_DA_OUI_PROFILEf, eth1731_profile->lmm_da_oui_prof);

    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, OPCODE_0_RATEf, eth1731_profile->opcode_0_rate);
    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, OPCODE_1_RATEf, eth1731_profile->opcode_1_rate);

    soc_OAMP_MEP_PROFILEm_field32_set(unit, profile_entry, OPCODE_BMAPf, 0xff);/* We can do without this duplicity*/


    rv = WRITE_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY,profile_indx, profile_entry );
    SOCDNX_IF_ERR_EXIT(rv);

    /* set the phase as well*/
    SOC_REG_ABOVE_64_CLEAR(profile_entry);
    soc_OAMP_MEP_SCAN_PROFILEm_field32_set(unit,profile_entry,DMM_CNTf,eth1731_profile->dmm_cnt );
    soc_OAMP_MEP_SCAN_PROFILEm_field32_set(unit,profile_entry,LMM_CNTf,eth1731_profile->lmm_cnt );
    soc_OAMP_MEP_SCAN_PROFILEm_field32_set(unit,profile_entry,CCM_CNTf, eth1731_profile->ccm_cnt);
    soc_OAMP_MEP_SCAN_PROFILEm_field32_set(unit,profile_entry,OP_0_CNTf,eth1731_profile->op_0_cnt );
    soc_OAMP_MEP_SCAN_PROFILEm_field32_set(unit,profile_entry,OP_1_CNTf,eth1731_profile->op_1_cnt );
    rv = WRITE_OAMP_MEP_SCAN_PROFILEm(unit,MEM_BLOCK_ANY,profile_indx, profile_entry);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* ****************************************************************************/
/*****************************************************************************/

soc_error_t soc_jer_pp_oam_oamp_eth1731_profile_get(
    int                                 unit,
    uint8                          profile_indx,
    SOC_PPD_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    int rv;
    soc_reg_above_64_val_t profile_entry;
    SOCDNX_INIT_FUNC_DEFS;

    rv = READ_OAMP_MEP_PROFILEm(unit, MEM_BLOCK_ANY,profile_indx, profile_entry );
    SOCDNX_IF_ERR_EXIT(rv);

    eth1731_profile->dmm_offset  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, DMM_OFFSETf);
    eth1731_profile->dmr_offset  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, DMR_OFFSETf);
    eth1731_profile->dmm_rate  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, DMM_RATEf);

    eth1731_profile->lmm_offset  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, LMM_OFFSETf);
    eth1731_profile->lmr_offset  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, LMR_OFFSETf);
    eth1731_profile->lmm_rate  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, LMM_RATEf); 
    eth1731_profile->piggy_back_lm  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, PIGGYBACK_LMf); 


    eth1731_profile->rdi_gen_method  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, RDI_GEN_METHODf); 

    eth1731_profile->lmm_da_oui_prof  = soc_OAMP_MEP_PROFILEm_field32_get(unit, profile_entry, LMM_DA_OUI_PROFILEf); 


    SOC_REG_ABOVE_64_CLEAR(profile_entry);

exit:
    SOCDNX_FUNC_RETURN;
}

/* ****************************************************************************/
/*****************************************************************************/


soc_error_t soc_jer_pp_oam_init_eci_tod(
   int                                 unit
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    /** NTP  */
    /* write 0 to control register */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_EVENT_MUX_CONTROLf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_COUNT_ENABLEf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_LOAD_ENABLEf,  0));
    /* write value to frac sec lower register */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_TIME_FRAC_SEC_LOWERf,  0x13576543));
    /* write value to frac sec upper register */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_TIME_FRAC_SEC_UPPERf,  0x1ffff00));
    /* write value to frequency register (4 nS in binary fraction) */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_TIME_FREQ_CONTROLf,  0x44b82fa1));
    /* write value to time sec register  */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_ONE_SECf, 1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_UPPER_SECf, 0x0804560));

    /* write to control register to load values */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_LOAD_ENABLEf,  1));
    /* write to control register to disable load values */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_6r, REG_PORT_ANY, 0, TOD_NTP_EVENT_MUX_CONTROLf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_COUNT_ENABLEf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_LOAD_ENABLEf,  0));
    /* write to control register to enable counter */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_NTP_COUNT_ENABLEf,  1));

    /** IEEE 1588 */
    /* write 0 to control register */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_EVENT_MUX_CONTROLf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_COUNT_ENABLEf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_LOAD_ENABLEf,  0));
    /* write value to frac sec lower register */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_FRAC_SEC_LOWERf,  0x13576543));
    /* write value to frac sec upper register */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_FRAC_SEC_UPPERf,  0x1ffff00));
    /* write value to frequency register (4 nS) */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_TIME_FREQ_CONTROLf,  0x10000000));
    /* write value to time sec register  */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_ONE_SECf, 1));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_UPPER_SECf, 0x0804560));
    /* write to control register to load values */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_LOAD_ENABLEf,  1));
    /* write to control register to disable load values */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_7r, REG_PORT_ANY, 0, TOD_IEEE_1588_EVENT_MUX_CONTROLf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_COUNT_ENABLEf,  0));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_LOAD_ENABLEf,  0));
    /* write to control register to enable counter */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GP_CONTROL_8r, REG_PORT_ANY, 0, TOD_IEEE_1588_COUNT_ENABLEf,  1));

exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t soc_jer_pp_oam_sa_addr_msbs_set(
    int unit,
    int profile,
   const uint8 * msbs)
{
    soc_reg_above_64_val_t reg_above_64, field_above_64 = {0};
    soc_field_t base_mac_sa_profiles[] = {BASE_MAC_SA_PROFILE_0f, BASE_MAC_SA_PROFILE_1f };
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (profile < 0 || profile >= 2) {
        SOCDNX_IF_ERR_EXIT_MSG(SOC_E_INTERNAL,  (_BSL_SOCDNX_MSG("Internal error: incorrect SA profile.")));
    }
    
    rv = READ_OAMP_CCM_MAC_SAr(unit, reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);
    field_above_64[0] = (msbs[0] << 24) + (msbs[1] << 16) + (msbs[2] << 8) + msbs[3];
    field_above_64[1] = msbs[4];

    soc_reg_above_64_field_set(unit, OAMP_CCM_MAC_SAr, reg_above_64, base_mac_sa_profiles[profile], field_above_64);

    rv = WRITE_OAMP_CCM_MAC_SAr(unit, reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv); 


exit:
    SOCDNX_FUNC_RETURN; 
}


/**
 * Get MSBs of global SA address of outgoing PDUs.
 * 
 * @author sinai (26/06/2014)
 * 
 * @param unit 
 * @param profile - 0 or 1 
 * @param msbs - Assumed to be an array of 5 bytes
 * 
 * @return soc_error_t 
 */
soc_error_t soc_jer_pp_oam_sa_addr_msbs_get(
    int unit,
    int profile,
    uint8 * msbs)
{
    soc_reg_above_64_val_t reg_above_64= {0}, field_above_64 = {0};
    soc_field_t base_mac_sa_profiles[] = { BASE_MAC_SA_PROFILE_0f, BASE_MAC_SA_PROFILE_1f }; 
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (profile < 0 || profile >= 2) {
        SOCDNX_IF_ERR_EXIT_MSG(SOC_E_INTERNAL,  (_BSL_SOCDNX_MSG("Internal error: incorrect SA profile."))); 
    }

    rv = READ_OAMP_CCM_MAC_SAr(unit, reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_reg_above_64_field_set(unit, OAMP_CCM_MAC_SAr, reg_above_64, base_mac_sa_profiles[profile], field_above_64);

    msbs[0] = field_above_64[0] >>24;
    msbs[1] = (field_above_64[0] >>16) & 0xff;
    msbs[2] = (field_above_64[0] >>8) & 0xff;
    msbs[3] = field_above_64[0] & 0xff;
    msbs[4] = field_above_64[1];


exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t soc_jer_pp_oam_dm_trigger_set(
   int unit,
   int endpoint_id)
{
    uint32 reg32;
    soc_field_t dm_trigger_field[] = { DM_TRIGER_0f, DM_TRIGER_1f, DM_TRIGER_2f, DM_TRIGER_3f,
    DM_TRIGER_4f, DM_TRIGER_5f,DM_TRIGER_6f, DM_TRIGER_7f}; 
    int rv;
    int table_entry, reg_field;
    SOCDNX_INIT_FUNC_DEFS;

    table_entry = endpoint_id /8;
    reg_field = endpoint_id %8;

    soc_OAMP_DM_TRIGERm_field32_set(unit,&reg32, dm_trigger_field[reg_field], 1);

    rv = WRITE_OAMP_DM_TRIGERm(unit, MEM_BLOCK_ANY,table_entry, &reg32 );
    SOCDNX_IF_ERR_EXIT(rv); 


exit:
    SOCDNX_FUNC_RETURN; 

}


/**
 * Set the opcode info register. 
 * At this stage these are only configured to transmit AIS 
 * frames, one at period one second and another at period  one 
 * minute. 
 * 
 * @author sinai (13/07/2014)
 * 
 * @param unit 
 * 
 * @return soc_error_t 
 */
STATIC soc_error_t _soc_jer_pp_oam_set_opcode_n_table(int unit)
{
    int rv;
    soc_reg_above_64_val_t reg_above_64;
    SOCDNX_INIT_FUNC_DEFS;

    /* AIS frame with period one second*/
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    soc_reg_above_64_field32_set(unit, OAMP_OPCODE_INFOr, reg_above_64, OPCODE_N_OPCODEf, SOC_PPC_OAM_ETHERNET_PDU_OPCODE_AIS);
    soc_reg_above_64_field32_set(unit, OAMP_OPCODE_INFOr, reg_above_64, OPCODE_N_VERSIONf, 0);
    soc_reg_above_64_field32_set(unit, OAMP_OPCODE_INFOr, reg_above_64, OPCODE_N_FLAGSf, 0x4); /* flag indicating 1 frame per second*/
    soc_reg_above_64_field32_set(unit, OAMP_OPCODE_INFOr, reg_above_64, OPCODE_N_TLV_OFFSETf, 0x0);
    soc_reg_above_64_field32_set(unit, OAMP_OPCODE_INFOr, reg_above_64, OPCODE_N_TLV_SIZEf, 0x0);

    rv = WRITE_OAMP_OPCODE_INFOr(unit,SOC_PPC_OAM_AIS_PERIOD_ONE_SECOND_OPCODE_ENTRY,reg_above_64 );
    SOCDNX_IF_ERR_EXIT(rv); 

    /* AIS frame with period one minute. All values except Flags identical to those above.*/
    soc_reg_above_64_field32_set(unit, OAMP_OPCODE_INFOr, reg_above_64, OPCODE_N_FLAGSf, 0x6); /* flag indicating 1 frame per second*/
    rv = WRITE_OAMP_OPCODE_INFOr(unit,SOC_PPC_OAM_AIS_PERIOD_ONE_MINUTE_OPCODE_ENTRY,reg_above_64 );
    SOCDNX_IF_ERR_EXIT(rv); 




exit:
    SOCDNX_FUNC_RETURN; 
}

