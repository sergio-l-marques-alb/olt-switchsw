/*
 * $Id: $
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
 *
 * File: qax_pp_oam.c
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


#include <soc/dpp/QAX/QAX_PP/qax_pp_oam.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_oam_mep_db.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/dpp_wb_engine.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* Default Mask for MEP-Type vs. Trap-code TCAM (masked bits are set, unmasked - cleared)
   TCAM Key: {MEP-Type(4),channel_or_opcode_sel(1),channel/opcode(16),trap-code(8)} */
#define _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK \
            ((~(_QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(0xf,0xff)))&((1<<(4+1+16+8))-1))

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* mp_type_entry is a concatination of the bits OAM-is-BFD, Not-Below-MDL-MEP-Bitmap-OR, MDL-MP-Type(2), Not-Above-MDL-MEP-Bitmap-OR*/
#define _QAX_PP_OAM_MP_TYPE_MAP_IS_BFD(mp_type_entry) ((mp_type_entry)>>4)
#define _QAX_PP_OAM_MP_TYPE_MAP_NOT_BELLOW_MDL_MEP_BITMAP_OR(mp_type_entry) (((mp_type_entry)>>3) & 0x1)
#define _QAX_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_entry) (((mp_type_entry)>>1) &0x3)
#define _QAX_PP_OAM_MP_TYPE_MAP_NOT_ABOVE_MDL_MEP_BITMAP_OR(mp_type_entry) ((mp_type_entry) & 0x1)

#define _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(_mep_type, _trap_code) \
            (((_mep_type)<<(8+16+1)) + (_trap_code))

#define _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY_GET_TRAP_CODE(entry_key) (entry_key & 0xff)


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

/************************************
 * Static declerations
 ************************************/
/* QAX specific classifier initializations */
STATIC soc_error_t soc_qax_pp_oam_classifier_init(int unit);

/* QAX specific OAMP initializations */
STATIC soc_error_t soc_qax_pp_oam_oamp_init(int unit);

/* Initialize MP-Type configuration for hierarchical LM */
STATIC soc_error_t soc_qax_pp_oam_mp_type_config_init(int unit);

/* Initialize the key selection for O-EM-1a/b for hierarchical LM */
STATIC soc_error_t soc_qax_pp_oam_oem1_key_select_init(int unit);

/* intialize default values in the TCAM validates the MEP-Type and opcode/channel
   against the FHEI.Trap-code */
STATIC soc_error_t soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(int unit);
/************************************/

/* QAX specific OAM initialization */
soc_error_t soc_qax_pp_oam_init(int unit){

    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    /* QAX OAM-classifier */
    rv = soc_qax_pp_oam_classifier_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    /* QAX OAMP */
    rv = soc_qax_pp_oam_oamp_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX specific classifier initializations */
STATIC
soc_error_t soc_qax_pp_oam_classifier_init(int unit) {

    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_mp_type_config_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_qax_pp_oam_oem1_key_select_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX specific OAMP initializations */
STATIC
soc_error_t soc_qax_pp_oam_oamp_init(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_qax_pp_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN int                                    unit,
    SOC_SAND_IN uint32                                 profile_ndx,
    SOC_SAND_IN SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  )
{

    uint64 reg, field64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    COMPILER_64_ZERO(reg);

    SOC_SAND_IF_ERR_EXIT(
       soc_reg_get(unit, OAMP_PUNT_EVENT_HENDLINGr, REG_PORT_ANY, profile_ndx, &reg));
    COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_PUNT_RATEf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->punt_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_PUNT_ENABLEf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->rx_state_update_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_RX_STATE_UPDATE_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->scan_state_update_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_SCAN_STATE_UPDATE_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_MEP_RDI_UPDATE_LOC_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_loc_clear_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_MEP_RDI_UPDATE_LOC_CLEAR_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->mep_rdi_update_rx_enable);
    soc_reg64_field_set(unit, OAMP_PUNT_EVENT_HENDLINGr, &reg, PROFILE_N_MEP_RDI_UPDATE_RX_ENf, field64);
    COMPILER_64_SET(field64, 0, punt_profile_data->punt_rate);
    SOC_SAND_IF_ERR_EXIT(
       soc_reg_set(unit, OAMP_PUNT_EVENT_HENDLINGr, REG_PORT_ANY, profile_ndx, reg));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in qax_pp_oam_oamp_punt_event_hendling_profile_set()", profile_ndx, 0);
}

uint32
  soc_qax_pp_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA    *punt_profile_data
  ) {
    uint64 reg, field64;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    COMPILER_64_ZERO(reg);

    SOC_SAND_IF_ERR_EXIT(
       soc_reg_get(unit, OAMP_PUNT_EVENT_HENDLINGr, REG_PORT_ANY, profile_ndx, &reg));
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_PUNT_RATEf);
    punt_profile_data->punt_rate = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_PUNT_ENABLEf);
    punt_profile_data->punt_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_RX_STATE_UPDATE_ENf);
    punt_profile_data->rx_state_update_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_SCAN_STATE_UPDATE_ENf);
    punt_profile_data->scan_state_update_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_MEP_RDI_UPDATE_LOC_ENf);
    punt_profile_data->mep_rdi_update_loc_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_MEP_RDI_UPDATE_LOC_CLEAR_ENf);
    punt_profile_data->mep_rdi_update_loc_clear_enable = COMPILER_64_LO(field64);
    field64 = soc_reg64_field_get(unit, OAMP_PUNT_EVENT_HENDLINGr, reg, PROFILE_N_MEP_RDI_UPDATE_RX_ENf);
    punt_profile_data->mep_rdi_update_rx_enable = COMPILER_64_LO(field64);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_oamp_punt_event_hendling_profile_get()", profile_ndx, 0);
}

STATIC
soc_error_t soc_qax_pp_oam_mp_type_config_init(int unit)
{
    int rv;
    uint32 mp_type_index;
    soc_reg_above_64_val_t reg_above_64;
    uint32 mp_type;

    SOCDNX_INIT_FUNC_DEFS;

    rv = READ_IHP_OAM_MP_TYPE_MAPr(unit, SOC_CORE_ALL, reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

    for (mp_type_index=0; mp_type_index<(1<<5) /*all combinations of 5 bits*/; ++mp_type_index ) {
        if (_QAX_PP_OAM_MP_TYPE_MAP_IS_BFD(mp_type_index)) {
            mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BFD;
        } else if (_QAX_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_index)==_QAX_PP_OAM_MDL_MP_TYPE_NO_MP ) {
            if (_QAX_PP_OAM_MP_TYPE_MAP_NOT_ABOVE_MDL_MEP_BITMAP_OR(mp_type_index) &&
                _QAX_PP_OAM_MP_TYPE_MAP_NOT_BELLOW_MDL_MEP_BITMAP_OR(mp_type_index)) {
                if (SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm) {
                    mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BETWEEN_MEPS;
                }
                else {
                    mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BELOW_ALL;
                }
            }
            else if (_QAX_PP_OAM_MP_TYPE_MAP_NOT_ABOVE_MDL_MEP_BITMAP_OR(mp_type_index)) {
                mp_type = SOC_PPC_OAM_MP_TYPE_QAX_BELOW_ALL;
            }
            else {
                mp_type = SOC_PPC_OAM_MP_TYPE_QAX_ABOVE_ALL;
            }
        } else {
            mp_type = _QAX_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(
               _QAX_PP_OAM_MP_TYPE_MAP_MDL_MP_TYPE(mp_type_index));
        }
        SHR_BITCOPY_RANGE(reg_above_64, (3 * mp_type_index), &mp_type, 0, 3);
    }

    rv = WRITE_IHP_OAM_MP_TYPE_MAPr(unit,SOC_CORE_ALL,reg_above_64);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


/* Classifier key selection in ingress (see soc_qax_pp_oam_oem1_key_select_init) */
STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_ingress_init(int unit)
{
    int rv;

    uint32 key, payload;
    soc_reg_above_64_val_t reg_val = {0};

    uint8
        key_sel_inner,
        key_sel_outer,
        mp_profile_sel,
        your_disc_inner,
        your_disc_outer;


    SOCDNX_INIT_FUNC_DEFS;


    mp_profile_sel = 1; /* always take OAM1's mp_profile */

    key_sel_outer       = _QAX_PP_OAM_OEM1_KEY_SEL_NULL ; /* QAX - Bug prevents hierarchical LM so working in JER mode */
    your_disc_outer     = 0                             ; /* QAX - Bug prevents hierarchical LM so working in JER mode */

    for (key = 0; key < (1<<5) ; key++) {

        key_sel_inner  = _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_OUTER_VALID(key) ?
            _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER : _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF ;

        your_disc_inner  = _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key);

        payload = _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(key_sel_inner,  key_sel_outer,
                                                   mp_profile_sel,
                                                   your_disc_inner, your_disc_outer);
        SHR_BITCOPY_RANGE(reg_val, key*7, &payload, 0, 7);
    }

    rv = WRITE_IHP_OAM_INGRESS_KEY_SELECTr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* Classifier key selection in egress (see soc_qax_pp_oam_oem1_key_select_init) */
STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_egress_init(int unit)
{
    int rv;

    uint32 key, payload;
    soc_reg_above_64_val_t reg_val = {0};

    uint8
        key_sel_inner,
        key_sel_outer,
        mp_profile_sel,
        your_disc_inner,
        your_disc_outer;


    SOCDNX_INIT_FUNC_DEFS;


    mp_profile_sel  = 1; /* always take OAM1's mp_profile */
    your_disc_inner = 0; /* Unused in egress */
    your_disc_outer = 0; /* Unused in egress */

    key_sel_outer   = _QAX_PP_OAM_OEM1_KEY_SEL_NULL ; /* QAX - Bug prevents hierarchical LM so working in JER mode */

    for (key = 0; key < (1<<5) ; key++) {

        key_sel_inner  = _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_OUTER_VALID(key) ?
            _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER : _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF ;


        payload = _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(key_sel_inner,  key_sel_outer,
                                                   mp_profile_sel,
                                                   your_disc_inner, your_disc_outer);
        SHR_BITCOPY_RANGE(reg_val, key*7, &payload, 0, 7);
    }

    rv = WRITE_IHP_OAM_EGRESS_KEY_SELECTr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * This function initializes the key selection for OEM1, when accessed
 * from ingress, in order to achieve heirarchical loss measurement.
 * Note: When 2 level hierarchy is looked up, the indication whether the
 * key is a LIF or a Your-Discriminator value is used to differentiate
 * between the 2 lookups and loses it's original function. The user is
 * required to make sure LIFs and Your-Disc-s do not overlap.
 */
STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_h_lm_ingress_init(int unit)
{
    int rv;

    uint32 key, payload;
    soc_reg_above_64_val_t reg_val = {0};

    uint8
        key_sel_inner,
        key_sel_outer,
        mp_profile_sel,
        your_disc_inner,
        your_disc_outer;


    SOCDNX_INIT_FUNC_DEFS;


    for (key = 0; key < (1<<5) ; key++) {

        if (_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_OUTER_VALID(key) &&
            _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_INNER_VALID(key)) {
            /* 2 Valid LIFs - Hierarchy by LIF   */
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_INNER;
            key_sel_outer = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
            your_disc_inner =
                your_disc_outer =
                 _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key);
            mp_profile_sel = (_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(key) ||
                              _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_INNER(key)    );

        }
        else {
            /* Hierarchy by MD-Level or single endpoint */
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
            key_sel_outer = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
            your_disc_inner = _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key);
            your_disc_outer = 1 - your_disc_inner;
            mp_profile_sel = 1;
        }

        payload = _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(key_sel_inner,  key_sel_outer,
                                                   mp_profile_sel,
                                                   your_disc_inner, your_disc_outer);
        SHR_BITCOPY_RANGE(reg_val, key*7, &payload, 0, 7);
    }

    rv = WRITE_IHP_OAM_INGRESS_KEY_SELECTr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * This function initializes the key selection for OEM1, when accessed
 * from egress, in order to achieve heirarchical loss measurement.
 */
STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_h_lm_egress_init(int unit)
{
    int rv;

    uint32 key, payload;
    soc_reg_above_64_val_t reg_val = {0};

    uint8
        key_sel_inner,
        key_sel_outer,
        mp_profile_sel,
        your_disc_inner,
        your_disc_outer;


    SOCDNX_INIT_FUNC_DEFS;


    mp_profile_sel  = 1; /* always take OAM1's mp_profile */

    for (key = 0; key < (1<<5) ; key++) {
        if (_QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_OUTER_VALID(key) &&
            _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_INNER_VALID(key)    ) {
            /* 2 Valid LIFs - Hierarchy by LIF   */
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_INNER;
            key_sel_outer = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
            your_disc_inner =
                your_disc_outer = 0;
        }
        else {
            /* Hierarchy by MD-Level or single endpoint */
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
            key_sel_outer = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
            your_disc_inner = 0;
            your_disc_outer = 1;
        }

        payload = _QAX_PP_OAM_OEM1_KEY_SEL_PAYLOAD(key_sel_inner,  key_sel_outer,
                                                   mp_profile_sel,
                                                   your_disc_inner, your_disc_outer);
        SHR_BITCOPY_RANGE(reg_val, key*7, &payload, 0, 7);
    }

    rv = WRITE_IHP_OAM_EGRESS_KEY_SELECTr(unit, reg_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * The key for lookup in the OAM classifier is configurable.
 * This function initializes the key selection for O-EM-1a/b
 */
STATIC
soc_error_t soc_qax_pp_oam_oem1_key_select_init(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_CONFIG(unit)->pp.oam_hierarchical_lm) {
        /* Key selection for hierarchical LM   */
        rv = soc_qax_pp_oam_oem1_key_select_h_lm_ingress_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_qax_pp_oam_oem1_key_select_h_lm_egress_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
    else { /* No Hierarchical LM */
        rv = soc_qax_pp_oam_oem1_key_select_ingress_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = soc_qax_pp_oam_oem1_key_select_egress_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


/**
 * Find the entry with the given trap code and delete it. 
 * Logic is identical to the arad_pp_oam version. 
 *  
 * 
 * @author sinai (27/12/2015)
 * 
 * @param unit 
 * @param mep_type 
 * @param trap_code 
 * 
 * @return soc_error_t 
 */
soc_error_t qax_pp_oam_oamp_rx_trap_codes_delete(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) {
    uint32 res;
    int i;
    uint32 field=0 ;
    uint32 valid=0 ;
    soc_reg_above_64_val_t entbuf;
    uint32 internal_trap_code;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);

    res = arad_pp_oam_oamp_rx_trap_codes_delete_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    /* Go over all the entries, find the given trap code, clear and get outta here.*/
    /* Assume if Datf is != 0 then so is the TCAM*/
    for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
        res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res); 
        valid  =     soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
        field =     soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, DATf);
        if (_QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY_GET_TRAP_CODE(field) == internal_trap_code && valid) {
            /* Found him.*/
            SOC_REG_ABOVE_64_CLEAR(entbuf);
            res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
            SOCDNX_IF_ERR_EXIT(res); 
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Find a free entry in the TCAM and set the given trap on it. 
 * Logic is identical to the arad_pp_oam version.
 * 
 * @author sinai (27/12/2015)
 * 
 * @param unit 
 * @param mep_type 
 * @param trap_code 
 * 
 * @return soc_error_t 
 */
soc_error_t qax_pp_oam_oamp_rx_trap_codes_set(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) {
    uint32 res;
    int i;
    soc_reg_above_64_val_t entbuf; 
    uint32 field=0;
    uint32 internal_trap_code;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);

    res = arad_pp_oam_oamp_rx_trap_codes_set_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    /* Go over all the entries, find a free one, set the trap code and mep types, clear and get outta here.*/
    for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
        res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res); 

        field  =     soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
        if (field==0) {
            /* found a free one.*/
            soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, KEYf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_type, internal_trap_code));
            soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, MASKf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK);
            soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, VALIDf, 1);
            res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
            SOCDNX_IF_ERR_EXIT(res); 
            break;
        }

    }



exit:
    SOCDNX_FUNC_RETURN;

}


/* intialize default values in the TCAM validates the MEP-Type and opcode/channel
   against the FHEI.Trap-code */
STATIC
soc_error_t soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(int unit) {
    uint32 res;


    uint8 mep_types[] = { SOC_PPC_OAM_MEP_TYPE_ETH_OAM,
        SOC_PPC_OAM_MEP_TYPE_Y1731_O_MPLSTP,
        SOC_PPC_OAM_MEP_TYPE_Y1731_O_PWE,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_1_HOP,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV4_M_HOP,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_MPLS,
        SOC_PPC_OAM_MEP_TYPE_BFD_O_PWE };

    uint8 trap_codes[] = {0xe0, 0xe1, 0xe2, 0xe3, 0xe3, 0xe4, 0xe5};

    int i;
    soc_reg_above_64_val_t entbuf;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);
    soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, MASKf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK);
    soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, VALIDf, 1);
    soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, DATf, 1);


    for (i = 0; i < sizeof(mep_types); ++i) {
        soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, KEYf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_types[i], trap_codes[i]));
        res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
  SOCDNX_FUNC_RETURN;

}


/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

