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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
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

/* Initialize MP-Type configuration.
 * In QAX, some changes were made to support hierarchical LM.
 * So need to setup the MP-Type different from previous devices.
 */
STATIC soc_error_t soc_qax_pp_oam_mp_type_config_init(int unit);

/* Initialize the key selection for O-EM-1a/b for hierarchical LM */
STATIC soc_error_t soc_qax_pp_oam_oem1_key_select_init(int unit);

/* intialize default values in the TCAM validates the MEP-Type and opcode/channel
   against the FHEI.Trap-code */
STATIC soc_error_t soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(int unit);

/* Initialize taking the subtype in the egress from the OAM-TS
   subtype in case OAM-TS is present. */
STATIC soc_error_t soc_qax_pp_oam_egress_sub_type_from_opcode_init(int unit);
/************************************/

/* QAX specific classifier initializations */
soc_error_t soc_qax_pp_oam_classifier_init(int unit) {

    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_mp_type_config_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_qax_pp_oam_oem1_key_select_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_qax_pp_oam_egress_sub_type_from_opcode_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* QAX specific OAMP initializations */
soc_error_t soc_qax_pp_oam_oamp_init(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_qax_pp_oam_oamp_mep_type_v_trap_code_tcam_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = qax_pp_oam_bfd_flexible_verification_init(unit);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/* HW access function to setup a punt profile */
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

/* HW access function to read a punt profile. */
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

/* See static decleration for doc */
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


    key_sel_outer       = _QAX_PP_OAM_OEM1_KEY_SEL_NULL ; /* QAX - Bug prevents hierarchical LM so working in JER mode */
    your_disc_outer     = 0                             ; /* QAX - Bug prevents hierarchical LM so working in JER mode */

    for (key = 0; key < (1<<5) ; key++) {

        key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
        if ((!_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(key))
            && (_QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_OAM_LIF_OUTER_VALID(key))) {
            /* Outer Lif is valid but the OAM-Lif is different
               Choose the Outer-Lif as the inner key */
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER;
        }

        your_disc_inner = _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key);

        mp_profile_sel = _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_INNER(key) ||
            _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_LIF_EQ_TO_OAM_LIF_OUTER(key) ||
            _QAX_PP_OAM_OEM1_KEY_SEL_INGRESS_KEY_YOUR_DISC(key); /* Classifying based on your-discriminator Take MP-Profile from the result of OEM1 */

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
        if(SOC_IS_QAX(unit)) {
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_EGRESS_KEY_OAM_LIF_OUTER_VALID(key) ?
                            _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF_OUTER : _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
        } else {
            key_sel_inner = _QAX_PP_OAM_OEM1_KEY_SEL_OAM_LIF;
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
soc_error_t soc_qax_pp_oam_oamp_rx_trap_codes_delete(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) {
    uint32 res;
    int i;
    uint32 key=0 ;
    uint32 read_key=0 ;
    uint32 valid=0 ;
    soc_reg_above_64_val_t entbuf;
    uint32 internal_trap_code;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);

    res = arad_pp_oam_oamp_rx_trap_codes_delete_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    key = _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_type, internal_trap_code);

    /* Go over all the entries, find the given key (mep_type-trap_code pair), clear and get outta here.*/
    for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
        res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res); 
        valid  =     soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
        read_key =   soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, KEYf);
        if (valid && (read_key == key)) {
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
soc_error_t soc_qax_pp_oam_oamp_rx_trap_codes_set(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
    ) {
    uint32 res;
    int i;
    soc_reg_above_64_val_t entbuf; 
    uint32 valid = 0;
    uint32 key = 0;
    uint32 read_key = 0;
    uint32 internal_trap_code;
    uint8 found = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(entbuf);

    res = arad_pp_oam_oamp_rx_trap_codes_set_verify(unit, mep_type, trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    res = _arad_pp_oam_trap_code_to_internal(unit, trap_code, &internal_trap_code);
    SOCDNX_IF_ERR_EXIT(res); 

    key = _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_KEY(mep_type, internal_trap_code);

    found = 0;
    /* Go over all the entries, check if the key already exist.*/
    for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
        res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
        SOCDNX_IF_ERR_EXIT(res);

        valid = soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
        read_key = soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, KEYf);

        if (valid && (read_key == key)) {
            found = 1;
            break;
        }
    }

    if (!found) {
        /* Go over all the entries, find a free one, set the trap code and mep types, clear and get outta here.*/
        for (i=0; i<=soc_mem_index_max(unit, OAMP_CLS_TRAP_CODE_TCAMm) ; ++i) {
            res = READ_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
            SOCDNX_IF_ERR_EXIT(res);

            valid = soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_get(unit, entbuf, VALIDf);
            if (valid == 0) {
                /* found a free one.*/
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, KEYf, key);
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, MASKf, _QAX_PP_OAM_OAMP_MEP_TYPE_V_TRAP_CODE_TCAM_MASK);
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, DATf, 1);
                soc_OAMP_CLS_TRAP_CODE_TCAMm_field32_set(unit, entbuf, VALIDf, 1);
                res = WRITE_OAMP_CLS_TRAP_CODE_TCAMm(unit, MEM_BLOCK_ANY, i, entbuf);
                SOCDNX_IF_ERR_EXIT(res);
                break;
            }

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

/* Set taking the subtype from the OAM-TS in the egress if the OAM-TS
   is present (counter pointer valid)*/
STATIC
soc_error_t soc_qax_pp_oam_egress_sub_type_from_opcode_init(int unit) {

    soc_error_t res = SOC_E_NONE;
    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = READ_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_DEFAULT, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPE_FROM_OPCODEf, 1);

    res = WRITE_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_ALL, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  )
{
    uint32 res = SOC_SAND_OK;
    uint32  entry[2];
    uint32 regv;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    ARAD_PP_CLEAR(entry, uint32, 2);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_PROFILEm(unit, MEM_BLOCK_ANY, profile_ndx, &entry));
    regv = sd_sf_profile_data->wnd_lngth;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, WND_LNGTHf,&regv);
    regv = sd_sf_profile_data->sd_set_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SD_SET_THRESHf,&regv);
    regv = sd_sf_profile_data->sf_set_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SF_SET_THRESHf,&regv);
    regv = sd_sf_profile_data->sd_clr_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SD_CLR_THRESHf,&regv);
    
    regv = sd_sf_profile_data->sf_clr_thresh;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SF_CLR_THRESHf,&regv);
    regv = sd_sf_profile_data->alert_method;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, ALERT_METHODf,&regv);
    
    regv = sd_sf_profile_data->supress_alerts;
    soc_OAMP_SD_SF_PROFILEm_field_set(unit, &entry, SUPRESS_ALERTSf,&regv);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_SD_SF_PROFILEm(unit, MEM_BLOCK_ANY, profile_ndx, &entry));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_profile_set()", profile_ndx, 0);
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  )
{
    uint32 res = SOC_SAND_OK;
    uint32  entry[2];
    uint32 regv;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    ARAD_PP_CLEAR(entry, uint32, 2);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_PROFILEm(unit, MEM_BLOCK_ANY, profile_ndx, &entry));

    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, WND_LNGTHf,&regv);
    sd_sf_profile_data->wnd_lngth = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SD_SET_THRESHf,&regv);
    sd_sf_profile_data->sd_set_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SF_SET_THRESHf,&regv);
    sd_sf_profile_data->sf_set_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SD_CLR_THRESHf,&regv);
    sd_sf_profile_data->sd_clr_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SF_CLR_THRESHf,&regv);
    sd_sf_profile_data->sf_clr_thresh = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, ALERT_METHODf,&regv);
    sd_sf_profile_data->alert_method = regv ; 
    
    soc_OAMP_SD_SF_PROFILEm_field_get(unit, &entry, SUPRESS_ALERTSf,&regv);
    sd_sf_profile_data->supress_alerts = regv ; 


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_profile_get()", profile_ndx, 0);
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_1711_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 d_excess_thresh,
    SOC_SAND_IN  uint8                                 clr_low_thresh,
    SOC_SAND_IN  uint8                                 clr_high_thresh,
    SOC_SAND_IN  uint8                                 num_entry
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg64_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    /* configure sd sf y1711 config reg */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_OAMP_SD_SF_Y_1711_CONFr(unit, &reg64_val));
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, D_EXCESS_THRESHf, (uint32)d_excess_thresh);
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, IND_CLR_LOW_THRESHf, (uint32)clr_low_thresh);
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, IND_CLR_HIGH_THRESHf, (uint32)clr_high_thresh);
    soc_reg_field_set(unit, OAMP_SD_SF_Y_1711_CONFr, &reg64_val, NUM_OF_1711_ENTRIESf, (uint32)num_entry);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,WRITE_OAMP_SD_SF_Y_1711_CONFr(unit, reg64_val));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_config_set()", 0, 0);
}

  soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_1711_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT  uint8                                 *d_excess_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_low_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_high_thresh,
    SOC_SAND_INOUT  uint8                                 *num_entry
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 reg_val;
    uint32 field;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_OAMP_SD_SF_Y_1711_CONFr(unit, &reg_val));
    /* get sd sf y1711 config value */
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, D_EXCESS_THRESHf);
    *d_excess_thresh = field;
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, IND_CLR_LOW_THRESHf);
    *clr_low_thresh = field;
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, IND_CLR_HIGH_THRESHf);
    *clr_high_thresh = field;
    field = soc_reg_field_get(unit, OAMP_SD_SF_Y_1711_CONFr, reg_val, NUM_OF_1711_ENTRIESf);
    *num_entry = field;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_config_get()", 0, 0);
}

soc_error_t
 soc_qax_pp_oam_oamp_sd_sf_scanner_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                            scan_index
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 num_clocks_per_mep_scan;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t scan_fields[] = {SD_SF_NUM_SCANS_1f,SD_SF_NUM_SCANS_2f,SD_SF_NUM_SCANS_3f,SD_SF_NUM_SCANS_4f,SD_SF_NUM_SCANS_5f,SD_SF_NUM_SCANS_6f,SD_SF_NUM_SCANS_7f};
    uint32 scan_value[] = {1,3,30,300,3000,18000,180000}; /* for 3.33ms, 10ms, 100ms ... 10min (ccm period *3)*/
    uint32  reg_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    
    /*scan_index: ccm interval*/
    if((scan_index >7) || (scan_index<1)){
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" scan index error \n")));
    }
    
     /*num_clocks_per_mep_scan = ARAD_PP_OAM_NUM_CLOCKS_IN_MEP_SCAN;*/
    num_clocks_per_mep_scan = ( 333*arad_chip_kilo_ticks_per_sec_get(unit))/100;
        
    /* configure sd sf y1711 config reg */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));

    /* Should enable it for qax OAM init*/
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val, SD_SF_SCAN_ENf, 1);
    reg_val = scan_value[scan_index-1];
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val, scan_fields[scan_index-1], reg_val);
    
    /* Number of system clock cycles in one MEP scan (3.33 ms)*/
    soc_reg_above_64_field32_set(unit, OAMP_SD_SF_SCANNER_CFGr, reg_above_64_val,  NUM_CLOCKS_SD_SF_DB_SCANf,num_clocks_per_mep_scan);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,WRITE_OAMP_SD_SF_SCANNER_CFGr(unit, reg_above_64_val));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_scanner_set()", 0, 0);
}


soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_1711_db_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_IN  uint32                   sd_sf_1711_db_format,
    SOC_SAND_IN  uint8                   ccm_tx_rate,
    SOC_SAND_IN  uint8                   alert_method
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    uint32  reg_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_DB_Y_1711m(unit, MEM_BLOCK_ANY, y1711_sd_sf_id, &entry));
    
    reg_val = rmep_index; 
    soc_OAMP_SD_SF_DB_Y_1711m_field_set(unit, entry, RMEP_DB_PTRf, &reg_val);
    
    reg_val = ccm_tx_rate; 
    soc_OAMP_SD_SF_DB_Y_1711m_field_set(unit, entry, CCM_TX_RATEf, &reg_val);
    
    reg_val = alert_method; 
    soc_OAMP_SD_SF_DB_Y_1711m_field_set(unit, entry, ALLERT_METHODf, &reg_val);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_OAMP_SD_SF_DB_Y_1711m(unit, MEM_BLOCK_ANY, y1711_sd_sf_id, entry));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_db_set()", 0, 0);

}

soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_1711_db_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY  *sd_sf_1711_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    uint32  reg_val;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, READ_OAMP_SD_SF_DB_Y_1711m(unit, MEM_BLOCK_ANY, y1711_sd_sf_id, &entry));
    
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, D_EXCESSf, &reg_val);
    sd_sf_1711_entry->d_excess = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, D_MISMERGEf, &reg_val);
    sd_sf_1711_entry->d_mismatch = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, D_MISMATCHf, &reg_val);
    sd_sf_1711_entry->d_mismerge = reg_val;
    
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_PKT_CNTR_0f, &reg_val);
    sd_sf_1711_entry->prd_pkt_cnt_0 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_PKT_CNTR_1f, &reg_val);
    sd_sf_1711_entry->prd_pkt_cnt_1 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_PKT_CNTR_2f, &reg_val);
    sd_sf_1711_entry->prd_pkt_cnt_2 = reg_val;
    
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_ERR_IND_0f, &reg_val);
    sd_sf_1711_entry->prd_err_ind_0 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_ERR_IND_1f, &reg_val);
    sd_sf_1711_entry->prd_err_ind_1 = reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, PRD_ERR_IND_2f, &reg_val);
    sd_sf_1711_entry->prd_err_ind_2 = reg_val;

    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, CCM_TX_RATEf, &reg_val);
    sd_sf_1711_entry->ccm_tx_rate= reg_val;
    soc_OAMP_SD_SF_DB_Y_1711m_field_get(unit, entry, ALLERT_METHODf, &reg_val);
    sd_sf_1711_entry->allert_method= reg_val;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_1711_db_get()", 0, 0);

}



soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_db_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   sd_sf_db_index,
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY       *sd_sf_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    uint8 dbflag=0;/*1:db1 0:db2*/
    uint32 num_sdsfdb1 = 4096;
    uint32  reg_val = 0;
    uint32 sd_sf_entry_index = 0;
	
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);

   if((sd_sf_entry->ccm_tx_rate> 0x7)||(sd_sf_entry->thresh_profile> 15)){
       SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG(" ccm rate or thresh_profile error \n")));
   }
   
    /* need to allocate sd_sf index since it just has 4.5k */
    if(sd_sf_db_index < num_sdsfdb1){
      dbflag=1;/*db1*/
      sd_sf_entry_index =sd_sf_db_index;
    }
    else{
      dbflag=0;/*db2*/
      sd_sf_entry_index = sd_sf_db_index - 4095;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, dbflag? READ_OAMP_SD_SF_DB_1m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, &entry) : READ_OAMP_SD_SF_DB_2m(unit, MEM_BLOCK_ANY, (sd_sf_entry_index-4096), &entry));
    
    if(dbflag == 1){
        reg_val = sd_sf_entry->entry_format;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, ENTRY_FORMATf, &reg_val);
        reg_val = rmep_index;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, RMEP_DB_PTRf, &reg_val) ;
        reg_val = sd_sf_entry->ccm_tx_rate;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, CCM_TX_RATEf, &reg_val) ;
        reg_val = sd_sf_entry->thresh_profile;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, THRESH_PROFILEf, &reg_val) ;
        reg_val = sd_sf_entry->sd;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, SD_INDICATIONf, &reg_val) ;
        reg_val = sd_sf_entry->sf;
        soc_OAMP_SD_SF_DB_1m_field_set(unit, &entry, SF_INDICATIONf, &reg_val) ;
    }
    else{
        reg_val = sd_sf_entry->entry_format;
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, ENTRY_FORMATf, &reg_val);
        reg_val = rmep_index;
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, RMEP_DB_PTRf, &reg_val);
        reg_val = sd_sf_entry->ccm_tx_rate;
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, CCM_TX_RATEf, &reg_val) ;
        reg_val = sd_sf_entry->thresh_profile;
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, THRESH_PROFILEf, &reg_val) ;
        reg_val = sd_sf_entry->sd;
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, SD_INDICATIONf, &reg_val) ;
        reg_val = sd_sf_entry->sf;
        soc_OAMP_SD_SF_DB_2m_field_set(unit, &entry, SF_INDICATIONf, &reg_val) ;
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, dbflag ? WRITE_OAMP_SD_SF_DB_1m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, entry) :  WRITE_OAMP_SD_SF_DB_2m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, entry));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_db_set()", 0, 0);
}


soc_error_t
  soc_qax_pp_oam_oamp_sd_sf_db_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                sd_sf_db_index,    
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY    *sd_sf_entry
  )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t entry;
    soc_reg_above_64_val_t  slinding_count;
    uint32 num_sdsfdb1 = 4096;
    uint8 dbflag=0;/*1:db1 0:db2*/
    uint32  reg_val;
    uint32 sd_sf_entry_index = 0;
    int index=0; 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_REG_ABOVE_64_CLEAR(entry);

    if(sd_sf_db_index < num_sdsfdb1){
      dbflag=1;/*db1*/
      sd_sf_entry_index =sd_sf_db_index;
    }
    else{
      dbflag=0;/*db2*/
      sd_sf_entry_index = sd_sf_db_index - 4095;
    }
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 15, exit, dbflag? READ_OAMP_SD_SF_DB_1m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, &entry) : READ_OAMP_SD_SF_DB_2m(unit, MEM_BLOCK_ANY, sd_sf_entry_index, &entry));

    if(dbflag){
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SD_INDICATIONf, &reg_val);
         sd_sf_entry->sd=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SF_INDICATIONf, &reg_val);
         sd_sf_entry->sf=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SUM_OF_CNTRSf, &reg_val);
         sd_sf_entry->sum_cnt=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SLIDING_WND_CNTRf, slinding_count);
         for( index=0; index<256; index++){
             SHR_BITCOPY_RANGE(&(sd_sf_entry->sliding_wnd_cntr[index]),0,slinding_count,index*2,2);
         }
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, CCM_TX_RATEf,&reg_val) ;
         sd_sf_entry->ccm_tx_rate=reg_val;
         soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, THRESH_PROFILEf, &reg_val) ;
         sd_sf_entry->thresh_profile=reg_val;
    }
    else{
        soc_OAMP_SD_SF_DB_2m_field_get(unit, &entry, SD_INDICATIONf, &reg_val);
        sd_sf_entry->sd=reg_val;
        soc_OAMP_SD_SF_DB_2m_field_get(unit, &entry, SF_INDICATIONf, &reg_val);
        sd_sf_entry->sf=reg_val;
        soc_OAMP_SD_SF_DB_2m_field_get(unit, &entry, SUM_OF_CNTRSf, &reg_val);
        sd_sf_entry->sum_cnt=reg_val;
        soc_OAMP_SD_SF_DB_1m_field_get(unit, &entry, SLIDING_WND_CNTRf, slinding_count);
        for( index=0; index<256; index++){
            SHR_BITCOPY_RANGE(&(sd_sf_entry->sliding_wnd_cntr[index]),0,slinding_count,index*2,2);
        }
        soc_OAMP_SD_SF_DB_2m_field_get(unit, &entry, CCM_TX_RATEf,&reg_val) ;
        sd_sf_entry->ccm_tx_rate=reg_val;
        soc_OAMP_SD_SF_DB_2m_field_get(unit, &entry, THRESH_PROFILEf, &reg_val) ;
        sd_sf_entry->thresh_profile=reg_val;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_sd_sf_db_get()", 0, 0);
}


soc_error_t
  soc_qax_pp_oam_oamp_rmep_db_ext_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY  *rmep_db_ext_entry
  )
{
    uint32  res;
    uint32  rmep_db_ext_index;
    uint32  entry_index;
    uint64  entry_64 ,val64;
    uint32  val32 = 0;
    
    soc_field_t db_entrys[] = {ENTRY_0f,ENTRY_1f,ENTRY_2f,ENTRY_3f,ENTRY_4f,ENTRY_5f,ENTRY_6f,ENTRY_7f,ENTRY_8f,ENTRY_9f,
        ENTRY_10f,ENTRY_11f,ENTRY_12f,ENTRY_13f,ENTRY_14f,ENTRY_15f};
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* get the DB index and entry index */
    rmep_db_ext_index = (rmep_index/8) + 1;
    entry_index = rmep_index%8;
    
    /*enter RMEP DB entry*/  
    COMPILER_64_ZERO(entry_64);
    COMPILER_64_ZERO(val64);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_OAMP_RMEP_DB_EXTm(unit, MEM_BLOCK_ANY, rmep_db_ext_index, &entry_64));

    soc_mem_field64_get(unit, OAMP_RMEP_DB_EXTm, &entry_64, db_entrys[entry_index], &val64);
    val32 = COMPILER_64_LO(val64);
    rmep_db_ext_entry->last_prd_pkt_cnt_1731 = val32&0x3; /*[1:0]*/
    rmep_db_ext_entry->last_prd_pkt_cnt_1711 = (val32&0x1c)>>2; /*[4:2]*/
    rmep_db_ext_entry->rx_err = (val32&0x20)>>5; /*[5:5]*/
    rmep_db_ext_entry->loc= (val32&0x40)>>6; /*[6:6]*/

exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_qax_pp_oam_oamp_rmep_db_ext_get()", 0, 0);
}

/*
 * Setup SLM mode for the device. 
 * This can setup the subtype resolution in the egress to be compatible 
 * with previous devices (meaning one subtype for every non-OAM packet 
 * egressing the device). This is used to port the legacy SLM feature 
 * and should be replaced by a per-LIF implementation. 
 */
soc_error_t soc_qax_pp_oam_slm_set(int unit, int is_slm) {

    
    soc_error_t res = SOC_E_NONE;
    soc_reg_above_64_val_t reg_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_data);

    res = READ_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_DEFAULT, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

    if (is_slm) {
        /* Set the device wide SLM mode - default egress subtype that isn't taken from the OAM-TS */
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPE_FROM_OPCODEf, 0);
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPEf, _ARAD_PP_OAM_SUBTYPE_LM);
    }
    else {
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPE_FROM_OPCODEf, 1);
        soc_reg_above_64_field32_set(unit, IHP_FLP_GENERAL_CFGr, reg_data, EGRESS_NON_OAM_PACKET_SUB_TYPEf, 0);
    }

    res = WRITE_IHP_FLP_GENERAL_CFGr(unit, SOC_CORE_ALL, reg_data);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

