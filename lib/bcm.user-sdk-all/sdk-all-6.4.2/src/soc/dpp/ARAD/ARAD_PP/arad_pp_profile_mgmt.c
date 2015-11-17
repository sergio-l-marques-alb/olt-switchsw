#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_lif.c,v 1.72 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT
#include <soc/mem.h>


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_framework.h>

#include <soc/dpp/PPC/ppc_api_profile_mgmt.h>

#ifdef BCM_88660_A0
#include <soc/dpp/dpp_wb_engine.h>
#endif


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_PROFILE_MGMT_BITS_IN_BYTE                              (8)

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

/**
 * Function returns inlif profile mask according to the application given. 
 * The mask is stored in WB. 
 * For details see general explanation in .h file. 
 * 
 * @param (in) unit 
 * @param (in) inlif_profile_app - application that is using inlif profile 
 * @param (out) mask - the mask of the application in the profile 
 * 
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_inlif_profile_get_type_mask_from_wb(
   int                                                              unit,
   SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE                          inlif_profile_app,
   uint32                                                          *mask
   ) {
    uint32 res;
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE  inlif_profile_app_int;
    uint8 profile_bit;

    SOCDNX_INIT_FUNC_DEFS;

    for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS; profile_bit++) {
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_profile_app_int, profile_bit);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (inlif_profile_app_int == inlif_profile_app) {
            *mask |= (1 << profile_bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function returns inlif profile unused mask -
 * all the bits that are not used by any application, according 
 * to information stored in WB. For details see 
 * general explanation in .h file. 
 * 
 * @param (in) unit 
 * @param (out) mask - the mask of the application in the 
 *        profile
 * 
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_inlif_profile_get_type_unused_mask_from_wb(
   int                                                              unit,
   uint32                                                          *mask
   ) {
    uint32 res;
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE  inlif_profile_app_int;
    uint8 profile_bit;

    SOCDNX_INIT_FUNC_DEFS;

    *mask = 0;
    for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS; profile_bit++) {
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_profile_app_int, profile_bit);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (inlif_profile_app_int == (uint32)(-1)) {
            *mask |= (1 << profile_bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function returns outlif profile mask according to the 
 * application given. The mask is stored in WB. 
 * For details see general explanation in .h file. 
 * 
 * @param (in) unit 
 * @param (in) outlif_profile_app - application that is using 
 *                           outlif profile
 * @param (out) mask - the mask of the application in the 
 *        profile
 * 
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_outlif_profile_get_type_mask_from_wb(
   int                                                              unit,
   SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE                                     outlif_profile_app,
   uint32                                                          *mask
   ) {
    uint32 res;
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE  outlif_profile_app_int;
    uint8 profile_bit;

    SOCDNX_INIT_FUNC_DEFS;

    for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS; profile_bit++) {
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_OUTLIF_PROFILE_MASK, &outlif_profile_app_int, profile_bit);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (outlif_profile_app_int == outlif_profile_app) {
            *mask |= (1 << profile_bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function returns outlif profile unused mask -
 * all the bits that are not used by any application, according 
 * to information stored in WB. For details see 
 * general explanation in .h file. 
 * 
 * @param (in) unit 
 * @param (out) mask - the mask of the application in the 
 *        profile
 * 
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_outlif_profile_get_type_unused_mask_from_wb(
   int                                                              unit,
   uint32                                                          *mask
   ) {
    uint32 res;
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE  outlif_profile_app_int;
    uint8 profile_bit;

    SOCDNX_INIT_FUNC_DEFS;

    *mask = 0;
    for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS; profile_bit++) {
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &outlif_profile_app_int, profile_bit);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (outlif_profile_app_int == (uint32)(-1)) {
            *mask |= (1 << profile_bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function returns rif profile mask according to the 
 * application given. The mask is stored in WB. 
 * For details see general explanation in .h file. 
 * 
 * @param (in) unit 
 * @param (in) rif_profile_app - application that is using rif 
 *                           profile
 * @param (out) mask - the mask of the application in the 
 *        profile
 * 
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_rif_profile_get_type_mask_from_wb(
   int                                                              unit,
   SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE                                     rif_profile_app,
   uint32                                                          *mask
   ) {
    uint32 res;
    SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE  rif_profile_app_int;
    uint8 profile_bit;

    SOCDNX_INIT_FUNC_DEFS;

    for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_RIF_PROFILE_NOF_BITS; profile_bit++) {
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_RIF_PROFILE_MASK, &rif_profile_app_int, profile_bit);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (rif_profile_app_int == rif_profile_app) {
            *mask |= (1 << profile_bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function returns rif profile unused mask -
 * all the bits that are not used by any application, according 
 * to information stored in WB. For details see 
 * general explanation in .h file. 
 * 
 * @param (in) unit 
 * @param (out) mask - the mask of the application in the 
 *        profile
 * 
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_rif_profile_get_type_unused_mask_from_wb(
   int                                                              unit,
   uint32                                                          *mask
   ) {
    uint32 res;
    SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE  rif_profile_app_int;
    uint8 profile_bit;

    SOCDNX_INIT_FUNC_DEFS;

    *mask = 0;
    for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_RIF_PROFILE_NOF_BITS; profile_bit++) {
        res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_RIF_PROFILE_MASK, &rif_profile_app_int, profile_bit);
        SOCDNX_SAND_IF_ERR_EXIT(res);

        if (rif_profile_app_int == (uint32)(-1)) {
            *mask |= (1 << profile_bit);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/**
 * Function returns profile mask according to the interface type
 * and application given. 
 * 
 * @param (in) unit 
 * @param (in) interface_type - inlif/outlif/rif 
 * @param (in) applicaton_type - application that is using 
 *        profile
 * @param (out) mask - the mask of the application in the 
 *        profile
 *  
 * @return uint32 
 */
soc_error_t arad_pp_profile_mgmt_if_profile_get_type_mask(
   int                                                              unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                          applicaton_type,
   uint32                                                          *mask
   ) {
    soc_error_t res;
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE inlif_profile_app;
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE outlif_profile_app;
    SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE rif_profile_app;

    SOCDNX_INIT_FUNC_DEFS;

    *mask = 0;
    switch (interface_type) {
    case SOC_PPC_PROFILE_MGMT_TYPE_INLIF:
        inlif_profile_app = applicaton_type;
        if (inlif_profile_app == SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_USER) {
            res = arad_pp_profile_mgmt_inlif_profile_get_type_unused_mask_from_wb(unit, mask);
            SOCDNX_IF_ERR_EXIT(res);
        } else {
            res = arad_pp_profile_mgmt_inlif_profile_get_type_mask_from_wb(unit, inlif_profile_app, mask);
            SOCDNX_IF_ERR_EXIT(res);
        }
        break;
    case SOC_PPC_PROFILE_MGMT_TYPE_OUTLIF:
        outlif_profile_app = applicaton_type;
        if (outlif_profile_app == SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE_USER) {
            res = arad_pp_profile_mgmt_outlif_profile_get_type_unused_mask_from_wb(unit, mask);
            SOCDNX_IF_ERR_EXIT(res);
        } else {
            res = arad_pp_profile_mgmt_outlif_profile_get_type_mask_from_wb(unit, outlif_profile_app, mask);
            SOCDNX_IF_ERR_EXIT(res);
        }
        break;
    case SOC_PPC_PROFILE_MGMT_TYPE_RIF:
        rif_profile_app = applicaton_type;
        if (rif_profile_app == SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE_USER) {
            res = arad_pp_profile_mgmt_rif_profile_get_type_unused_mask_from_wb(unit, mask);
            SOCDNX_IF_ERR_EXIT(res);
        } else {
            res = arad_pp_profile_mgmt_rif_profile_get_type_mask_from_wb(unit, rif_profile_app, mask);
            SOCDNX_IF_ERR_EXIT(res);
        }
        break;
    default:
        break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Set the given value in the reserved bits of the given interface, according to application type.
*     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_val_set(
   int                                                          unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                          applicaton_type,
   uint32                                                          val,
   uint32                                                          *full_profile) {
    soc_error_t res;
    uint32 mask;
    uint32 mask_idx = 0;
    uint32 val_tmp = val;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_profile_mgmt_if_profile_get_type_mask(unit, interface_type, applicaton_type, &mask);
    SOCDNX_IF_ERR_EXIT(res);

    *full_profile &= ~mask;

    for (mask_idx = 0; mask_idx < sizeof(mask) * ARAD_PP_PROFILE_MGMT_BITS_IN_BYTE; mask_idx++) {
        if (mask & (1 << mask_idx)) {
            uint32 bitval = val_tmp & 1;
            *full_profile += (bitval << mask_idx);
            val_tmp >>= 1;
        }
    }
    if (val_tmp > 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Error, in arad_pp_profile_mgmt_if_profile_val_set\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Extract value from the given profile according to application type bits of the given interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_get_val_from_map(
   int                                                            unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                          applicaton_type,
   uint32                                                            full_profile,
   uint32                                                            *val
   ) {
    uint32 mask_idx = 0;
    uint32 unmapped_idx = 0;
    soc_error_t res;
    uint32 mask;

    SOCDNX_INIT_FUNC_DEFS;

    *val = 0;

    res = arad_pp_profile_mgmt_if_profile_get_type_mask(unit, interface_type, applicaton_type, &mask);
    SOCDNX_IF_ERR_EXIT(res);

    for (mask_idx = 0; mask_idx < sizeof(mask) * ARAD_PP_PROFILE_MGMT_BITS_IN_BYTE; mask_idx++) {
        if (mask & (1 << mask_idx)) {
            uint32 bitval = (full_profile & (1 << mask_idx)) >> mask_idx;
            *val |= (bitval << unmapped_idx);
            unmapped_idx++;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Set the given value in the free bits of the given interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_advanced_val_set(
   int                                                          unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                          val,
   uint32                                                          *full_profile) {
    soc_error_t res;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_profile_mgmt_if_profile_val_set(unit, interface_type, SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_USER, val, full_profile);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Extract value from the given profile according to the free bits of the given interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_advanced_get_val_from_map(
   int                                                            unit,
   SOC_PPC_PROFILE_MGMT_TYPE                                         interface_type,
   uint32                                                            full_profile,
   uint32                                                            *val
   ) {
    soc_error_t res;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_profile_mgmt_if_profile_get_val_from_map(unit, interface_type, SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_USER, full_profile, val);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Init profile management mechanism, allocate lif&rif resources
*     Details: in the H file. (search for prototype)
*********************************************************************/
soc_error_t arad_pp_profile_mgmt_if_profile_init(
   int                                                            unit
   ) 
{
    soc_error_t res;
    uint32 soc_sand_rv;

    SOC_SAND_OCC_BM_INIT_INFO
       inlif_btmp_init_info, outlif_btmp_init_info;
    SOC_SAND_OCC_BM_PTR
       inlif_bit_map, outlif_bit_map;
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE 
        inlif_profile_types_pri[SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_NOF];
    SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE inlif_type = (uint32)(-1);
    SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE outlif_type = (uint32)(-1);
    SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE rif_type = (uint32)(-1);
    uint32 bit_index;
    uint32 inlif_type_ndx;
    uint8 found, occupied;
    uint32 nof_profile_bits;

    SOCDNX_INIT_FUNC_DEFS;

    /* Set priorities to the different applications according to the restrictions */
    inlif_profile_types_pri[0]=SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_SAME_INTERFACE; /*restricted to 2 lsbs*/
    inlif_profile_types_pri[1]=SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_DSCP_MARKING; /*restricted to 2 lsbs*/
    /* other types are not restricted */
    inlif_profile_types_pri[2]=SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_OAM;
    inlif_profile_types_pri[3]=SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_TAGGED_MODE;
    inlif_profile_types_pri[4]=SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_STRICT_URPF;
    inlif_profile_types_pri[5]=SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_USER;

    /* Init WB buffer */
    soc_sand_rv = soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_ARAD_SW_DB_LIF_RIF_PROFILE);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Init occupation bitmaps */
    inlif_btmp_init_info.size = SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS;
    inlif_btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_NONE;
    inlif_btmp_init_info.unit = unit;
    inlif_btmp_init_info.init_val = 0;
    inlif_btmp_init_info.support_cache = 0;

    soc_sand_rv = soc_sand_occ_bm_create(
       &inlif_btmp_init_info,
       &inlif_bit_map
       );
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    outlif_btmp_init_info.size = SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_NOF_BITS(unit);
    outlif_btmp_init_info.wb_var_index  = SOC_DPP_WB_ENGINE_VAR_NONE;
    outlif_btmp_init_info.unit = unit;
    outlif_btmp_init_info.init_val = 0;
    outlif_btmp_init_info.support_cache = 0;

    soc_sand_rv = soc_sand_occ_bm_create(
       &outlif_btmp_init_info,
       &outlif_bit_map
       );
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Fixe: Add one for RIF */

    /* Init all applications to -1 */
    for (bit_index = 0; bit_index < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS; bit_index++) {
        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_type, bit_index);
        SOCDNX_IF_ERR_EXIT(res);
    }
    for (bit_index = 0; bit_index < SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_NOF_BITS(unit); bit_index++) {
        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_OUTLIF_PROFILE_MASK, &outlif_type, bit_index);
        SOCDNX_IF_ERR_EXIT(res);
    }
    for (bit_index = 0; bit_index < SOC_PPC_PROFILE_MGMT_RIF_PROFILE_NOF_BITS; bit_index++) {
        res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_RIF_PROFILE_MASK, &rif_type, bit_index);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* Go over all applications and allocate bits according to requirements. */
    /* The applications that have restrictions on the bits they can use will be allocated first. */

    /* Inlif Profile Allocation: */
    for (inlif_type_ndx = 0; inlif_type_ndx < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_NOF; inlif_type_ndx++) {
        switch (inlif_profile_types_pri[inlif_type_ndx]) {
        case SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_SAME_INTERFACE:
            if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0)) {
                /* For same interface we need one bit of the inlif profile, should be tranfered to the egress */
                for (bit_index = 0; bit_index < SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; bit_index++) {
                    soc_sand_rv = soc_sand_occ_bm_is_occupied(inlif_bit_map, bit_index, &occupied);
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (!occupied) {
                        break;
                    }
                }

                if (occupied) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Error, not enough resources in lif profile\n")));
                }

                soc_sand_rv = soc_sand_occ_bm_occup_status_set(inlif_bit_map, bit_index, 1/*occupied*/);
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

                res = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_profile_types_pri[inlif_type_ndx], bit_index);
                SOCDNX_IF_ERR_EXIT(res);
            }
            break;
#ifdef BCM_88660_A0
        case SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_SIMPLE_DSCP_MARKING:
            if (SOC_IS_ARADPLUS(unit)) {
                uint32 dscp_bit;

                if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0)) {
                    /* For DSCP marking we need two bits of the inlif profile, should be LSBs */

                    for (dscp_bit = 0; dscp_bit < SOC_PPC_PROFILE_MGMT_NOF_DSCP_BITS/*nof bits to allocate*/; dscp_bit++) {
                        for (bit_index = 0; bit_index < SOC_PPC_PROFILE_MGMT_NOF_INLIF_PROFILES_TRANSFERED_TO_EGRESS; bit_index++) {
                            soc_sand_rv = soc_sand_occ_bm_is_occupied(inlif_bit_map, bit_index, &occupied);
                            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

                            if (!occupied) {
                                break;
                            }
                        }

                        if (occupied) {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Error, not enough resources in lif profile\n")));
                        }

                        soc_sand_rv = soc_sand_occ_bm_occup_status_set(inlif_bit_map, bit_index, 1/*occupied*/);
                        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

                        soc_sand_rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_profile_types_pri[inlif_type_ndx], bit_index);
                        SOCDNX_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
            break;
        case SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_STRICT_URPF:
            if (SOC_IS_ARADPLUS(unit)) {
                if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
                    /* For URPF we need one bit of the inlif profile */

                    /* allocate 1 bit without id */
                    soc_sand_rv = soc_sand_occ_bm_alloc_next(inlif_bit_map, &bit_index, &found);
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

                    if (!found) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Error, not enough resources in lif profile\n")));
                    }

                    soc_sand_rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_profile_types_pri[inlif_type_ndx], bit_index);
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
                }
            }
            break;
        case SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE_OAM:
            if (SOC_IS_ARADPLUS(unit)) {
                nof_profile_bits = soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0);
                while (nof_profile_bits--) {
                    /* allocate 1 bit without id */
                    soc_sand_rv = soc_sand_occ_bm_alloc_next(inlif_bit_map, &bit_index, &found);
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);
                    if (!found) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Error, not enough resources in lif profile\n")));
                    }
                    /* Register this bit as OAM trap profile */
                    soc_sand_rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_profile_types_pri[inlif_type_ndx], bit_index);
                    SOCDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            }
            break;
#endif
        default:
            break;
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        /* TBD: Outlif Profile Allocation:*/

        /* TBD: Rif Profile Allocation:*/
    }


    /* Destroy occupation bitmaps */
    soc_sand_rv = soc_sand_occ_bm_destroy(inlif_bit_map);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_sand_occ_bm_destroy(outlif_bit_map);
    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    SOCDNX_FUNC_RETURN;
}


/*********************************************************************
*     LIF/RIF profile for diagnostics
*     Details: in the H file. (search for prototype)
*********************************************************************/

soc_error_t arad_pp_profile_mgmt_if_profile_diag_info_get(
   int                                                              unit,
   SOC_PPC_DIAG_LIF_PROFILE_INFO                                   *info
   ) {
    uint32 res;
	uint8 profile_bit;
	SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_TYPE inlif_read_profile_type;
	SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_TYPE outlif_read_profile_type;
	SOC_PPC_PROFILE_MGMT_RIF_PROFILE_TYPE rif_read_profile_type;

	SOCDNX_INIT_FUNC_DEFS;

	/* InLIF bit reading */
	for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_INLIF_PROFILE_NOF_BITS; profile_bit++) {
		res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_INLIF_PROFILE_MASK, &inlif_read_profile_type, profile_bit);
		SOCDNX_SAND_IF_ERR_EXIT(res);
		info->inlif_bits[profile_bit] = inlif_read_profile_type;
	}

	/* OutLIF bit reading */
	for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_OUTLIF_PROFILE_NOF_BITS(unit); profile_bit++) {
		res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_OUTLIF_PROFILE_MASK, &outlif_read_profile_type, profile_bit);
		SOCDNX_SAND_IF_ERR_EXIT(res);
		info->outlif_bits[profile_bit] = outlif_read_profile_type;
	}

	/* InLIF bit reading */
	for (profile_bit = 0; profile_bit < SOC_PPC_PROFILE_MGMT_RIF_PROFILE_NOF_BITS; profile_bit++) {
		res = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_SW_DB_RIF_PROFILE_MASK, &rif_read_profile_type, profile_bit);
		SOCDNX_SAND_IF_ERR_EXIT(res);
		info->rif_bits[profile_bit] = rif_read_profile_type;
	}

exit:
    SOCDNX_FUNC_RETURN;
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

