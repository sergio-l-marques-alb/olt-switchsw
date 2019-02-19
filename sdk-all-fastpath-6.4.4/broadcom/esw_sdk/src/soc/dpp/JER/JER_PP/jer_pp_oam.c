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
#include <soc/dpp/mbcm_pp.h>


#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/dpp_wb_engine.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */



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


/**
 * Write on the table EPNI_CFG_MAPPING_TO_OAM_PCP according to 
 * tc and outlif profile. 
 *  The index  used is  {TC, OAM-LIF-Profile(2)}.
 */
#define JER_PP_OAM_SET_EGRESS_OAM_PCP_BY_OUTLIF_PROFILE_AND_TC(outlif_prof, packet_tc, oam_pcp) \
    do {\
    uint32 reg32=0; \
        soc_EPNI_CFG_MAPPING_TO_OAM_PCPm_field32_set(unit,&reg32,CFG_MAPPING_TO_OAM_PCPf, (oam_pcp) );\
        rv = WRITE_EPNI_CFG_MAPPING_TO_OAM_PCPm(unit,SOC_CORE_ALL,(((outlif_prof) & 0x3) | (((packet_tc) & 0x7) <<2)),&reg32 );\
        SOCDNX_IF_ERR_EXIT(rv);\
} while (0)
    
  


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
STATIC soc_error_t soc_jer_pp_oam_classifier_oam1_passive_entries_add(int unit);

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
    int use_pcp_from_packet;
    int core;
    uint8 tc, outlif_profile;
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


    use_pcp_from_packet = soc_property_get(unit, spn_OAM_PCP_VALUE_EXTRACT_FROM_PACKET, 1); /* default: use PCP from packet*/
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
		/* PCP configurations.*/
        rv = READ_IHP_VTT_GENERAL_CONFIGS_1r(unit,core,&reg32);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit,IHP_VTT_GENERAL_CONFIGS_1r,&reg32,OAM_USE_PACKET_PCPf,use_pcp_from_packet );
        rv = WRITE_IHP_VTT_GENERAL_CONFIGS_1r(unit,core,reg32);
        SOCDNX_IF_ERR_EXIT(rv);

		/* Use Arad mode OAM-ID */
		rv = READ_IHP_FLP_GENERAL_CFGr(unit,core,&reg_64);
		SOCDNX_IF_ERR_EXIT(rv);
		soc_reg64_field32_set(unit,IHP_FLP_GENERAL_CFGr,&reg_64,OAM_ID_ARAD_MODEf,1 );
		rv = WRITE_IHP_FLP_GENERAL_CFGr(unit,core,reg_64);
		SOCDNX_IF_ERR_EXIT(rv);

    }

    /* IP TOS to PCP, DS to PCP used only by RFC-6374. Not implemented.*/

    /*Set the Egress PCP setting (configured statically).*/
    for (outlif_profile=0 ; outlif_profile < 0x40  /*all outlif profiles*/ ;++outlif_profile) {
        /* Mapping of outlif profile (6 bits) to oam-lif-profile (2).
           */
        uint32 oam_outlif_profile;
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get, 
                                 (unit,SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_OAM_PCP , ((uint32*)&outlif_profile), &oam_outlif_profile));
        SOCDNX_IF_ERR_EXIT(rv); 

        reg32=0;
        soc_EPNI_OUTLIF_TO_OAM_LIF_PROFILE_MAPm_field32_set(unit,&reg32,OUTLIF_TO_OAM_LIF_PROFILE_MAPf,oam_outlif_profile);
        rv = WRITE_EPNI_OUTLIF_TO_OAM_LIF_PROFILE_MAPm(unit, SOC_CORE_ALL, outlif_profile, &reg32);
        SOCDNX_IF_ERR_EXIT(rv); 
    }

    for (outlif_profile=0 ; outlif_profile < 4  /*2 bit value*/; ++outlif_profile) {
        for (tc=0 ; tc < 8 /*3 bit value */ ; ++tc) {
            /* The default mapping will be OAM_PCP = TC | OAM-LIF-Profile(2)*/
            JER_PP_OAM_SET_EGRESS_OAM_PCP_BY_OUTLIF_PROFILE_AND_TC(outlif_profile, tc, outlif_profile | tc);
        }
    }


    rv = soc_jer_pp_oam_classifier_oam1_passive_entries_add(unit);
    SOCDNX_IF_ERR_EXIT(rv); 


	if (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, 0)) {
		/* Enable Jericho ITMH */
		COMPILER_64_ZERO(reg_64);
		rv =  READ_OAMP_TX_PPHr(unit, &reg_64);
        SOCDNX_IF_ERR_EXIT(rv); 
		soc_reg64_field32_set(unit, OAMP_TX_PPHr, &reg_64, TX_USE_JER_ITMHf, 1);
		SOCDNX_IF_ERR_EXIT(WRITE_OAMP_TX_PPHr(unit, reg_64));
	}



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
    if ( (prev_mdl_mp_type==_JER_PP_OAM_MDL_MP_TYPE_MIP || prev_mdl_mp_type==_JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH) 
         && !update) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_EXISTS,
                         (_BSL_SOCDNX_MSG("Only one MEP/MIP per Level per LIF may be defined.")));
    }
    new_mdl_mp_type = (!is_mep)? _JER_PP_OAM_MDL_MP_TYPE_MIP : (is_active)? _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH : 
        _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH;

    new_mp_type_vector = prev_payload->mp_type_vector;

    JERICHO_PP_OAM_SET_MDL_MP_TYPE_VECTOR_BY_LEVEL(new_mp_type_vector,new_mdl_mp_type,classifier_mep_entry->md_level  );
    
    new_payload->mp_type_vector =new_mp_type_vector; 
    new_payload->mp_profile = is_active? classifier_mep_entry->non_acc_profile :prev_payload->mp_profile ; /* For passive entries the mp profile doesn't matter. 
																																								Assume the existing value (or zero if none exists)*/
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
    field_above_64[0] = (msbs[1] << 24) + (msbs[2] << 16) + (msbs[3] << 8) + msbs[4];
    field_above_64[1] = msbs[0];

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

    msbs[1] = field_above_64[0] >>24;
    msbs[2] = (field_above_64[0] >>16) & 0xff;
    msbs[3] = (field_above_64[0] >>8) & 0xff;
    msbs[4] = field_above_64[0] & 0xff;
    msbs[0] = field_above_64[0];


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
    uint32 write_val = 1;
    int table_entry, reg_field;
    SOCDNX_INIT_FUNC_DEFS;

    table_entry = endpoint_id /8;
    reg_field = endpoint_id %8;
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , write_val));

    soc_OAMP_DM_TRIGERm_field32_set(unit,&reg32, dm_trigger_field[reg_field], 1);

    rv = WRITE_OAMP_DM_TRIGERm(unit, MEM_BLOCK_ANY,table_entry, &reg32 );
    SOCDNX_IF_ERR_EXIT(rv); 
	write_val = 0;
	SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , write_val));


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


soc_error_t soc_jer_pp_oam_egress_pcp_set_by_profile_and_tc(int unit, uint8 tc, uint8 outlif_profile,uint8 oam_pcp){
    int rv;
    SOCDNX_INIT_FUNC_DEFS;


    JER_PP_OAM_SET_EGRESS_OAM_PCP_BY_OUTLIF_PROFILE_AND_TC(outlif_profile, tc, oam_pcp);


exit:
    SOCDNX_FUNC_RETURN; 
}



/**
 * All Passive entries are configured statically. 
 * When MP-Type = Passive match, for all other values the output 
 * of the LIF-Action table is the same.
 * 
 * @author sinai (25/11/2014)
 * 
 * @param unit 
 * @param tc 
 * @param outlif_profile 
 * @param oam_pcp 
 * 
 * @return soc_error_t 
 */
STATIC
soc_error_t soc_jer_pp_oam_classifier_oam1_passive_entries_add(int unit){
    int rv;
    int opcode_profile, ingress,my_cfm_mac, mp_profile;
    SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY       oam1_key;
    SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD   oam_payload;
    uint8 oam_mirror_profile[4];  /* Stored as 8 uint8's on uint32's*/
    uint32 oam_trap_code;
    uint32 soc_sand_rv;
    _oam_oam_a_b_table_buffer_t oama_buffer;

    SOCDNX_INIT_FUNC_DEFS;

    soc_sand_rv = arad_pp_oam_classifier_oam1_allocate_sw_buffer(unit,&oama_buffer);
    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

    SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRY_KEY_clear(&oam1_key);
    SOC_PPD_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD_clear(&oam_payload);

    oam1_key.inject = 0;
    oam1_key.mp_type_jr = SOC_PPC_OAM_MP_TYPE_JERICHO_PASSIVE_MATCH; 

    /* payload is the same*/
    oam_payload.snoop_strength = 0;
    oam_payload.counter_disable = 1;
    oam_payload.sub_type = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                       "oam_additional_FTMH_on_error_packets", 0) ?
                                _ARAD_PP_OAM_SUBTYPE_CCM : _ARAD_PP_OAM_SUBTYPE_DEFAULT; 

    /* Egress*/
    rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_MIRROR_PROFILE_ERR_PASSIVE, &oam_mirror_profile);
    SOCDNX_IF_ERR_EXIT(rv); 
    oam_payload.mirror_profile = oam_mirror_profile[0];

    /* Ingress*/
    rv = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_PP_OAM_TRAP_CODE_TRAP_TO_CPU_PASSIVE, &oam_trap_code);
    SOCDNX_IF_ERR_EXIT(rv); 
    oam_payload.cpu_trap_code = oam_trap_code;


    oam_payload.meter_disable = 1;
    oam_payload.forwarding_strength = 7;
    oam_payload.mirror_profile = 0;
    oam_payload.forward_disable = 1; /* default */
    oam_payload.snoop_strength = 0;
    oam_payload.counter_disable = 1; /* default */

    for (ingress=0; ingress<=1 ;++ingress ) {
        for (my_cfm_mac=0; my_cfm_mac<=1 ; ++my_cfm_mac) {
            for (opcode_profile=0; opcode_profile<SOC_PPD_OAM_OPCODE_MAP_COUNT ; ++opcode_profile) {
                for (mp_profile=0 ; mp_profile < SOC_PPD_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM ; ++mp_profile) {
                    if (ingress) {
                        oam_payload.up_map =1; /* Passive trap: Up at the ingress, Down at the egress.*/
                        oam_payload.forwarding_strength = _ARAD_PP_OAM_TRAP_STRENGTH; 
                    } else {
                        oam_payload.up_map =0; 
                    }
                    oam_payload.mirror_enable = !ingress; 
                    soc_sand_rv = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
                    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);
                }
            }
        }
    }
    
    soc_sand_rv = arad_pp_oam_classifier_oam1_entry_set_on_buffer(unit, &oam1_key, &oam_payload, &oama_buffer);
    SOCDNX_SAND_IF_ERR_RETURN(soc_sand_rv);

exit:
    arad_pp_oam_classifier_oam1_2_buffer_free(unit,&oama_buffer);
    SOCDNX_FUNC_RETURN; 
}
