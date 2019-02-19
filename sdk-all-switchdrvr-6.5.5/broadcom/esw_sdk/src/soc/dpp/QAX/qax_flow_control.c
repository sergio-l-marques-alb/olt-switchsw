#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
/* $Id: qax_flow_control.c,v 1.59 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FLOWCONTROL

/*************
 * INCLUDES  *
 *************/
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_flow_control.h>
#include <soc/dpp/JER/jer_flow_control.h>
#include <soc/dpp/QAX/qax_flow_control.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_pmf_pgm_mgmt.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/portmod/portmod.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/mcm/allenum.h>

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
static uint32
        pfc_map_reg[SOC_DPP_DEFS_MAX(NOF_FC_PFC_GENERIC_BITMAPS)] = {
        CFC_PFC_GENERIC_BITMAP_0r, CFC_PFC_GENERIC_BITMAP_1r, CFC_PFC_GENERIC_BITMAP_2r, CFC_PFC_GENERIC_BITMAP_3r,
        CFC_PFC_GENERIC_BITMAP_4r, CFC_PFC_GENERIC_BITMAP_5r, CFC_PFC_GENERIC_BITMAP_6r, CFC_PFC_GENERIC_BITMAP_7r,
        CFC_PFC_GENERIC_BITMAP_8r, CFC_PFC_GENERIC_BITMAP_9r, CFC_PFC_GENERIC_BITMAP_10r, CFC_PFC_GENERIC_BITMAP_11r,
        CFC_PFC_GENERIC_BITMAP_12r, CFC_PFC_GENERIC_BITMAP_13r, CFC_PFC_GENERIC_BITMAP_14r, CFC_PFC_GENERIC_BITMAP_15r,
        CFC_PFC_GENERIC_BITMAP_16r, CFC_PFC_GENERIC_BITMAP_17r, CFC_PFC_GENERIC_BITMAP_18r, CFC_PFC_GENERIC_BITMAP_19r,
        CFC_PFC_GENERIC_BITMAP_20r, CFC_PFC_GENERIC_BITMAP_21r, CFC_PFC_GENERIC_BITMAP_22r, CFC_PFC_GENERIC_BITMAP_23r,
        CFC_PFC_GENERIC_BITMAP_24r, CFC_PFC_GENERIC_BITMAP_25r, CFC_PFC_GENERIC_BITMAP_26r, CFC_PFC_GENERIC_BITMAP_27r,
        CFC_PFC_GENERIC_BITMAP_28r, CFC_PFC_GENERIC_BITMAP_29r, CFC_PFC_GENERIC_BITMAP_30r, CFC_PFC_GENERIC_BITMAP_31r,
        CFC_PFC_GENERIC_BITMAP_32r, CFC_PFC_GENERIC_BITMAP_33r, CFC_PFC_GENERIC_BITMAP_34r, CFC_PFC_GENERIC_BITMAP_35r,
        CFC_PFC_GENERIC_BITMAP_36r, CFC_PFC_GENERIC_BITMAP_37r, CFC_PFC_GENERIC_BITMAP_38r, CFC_PFC_GENERIC_BITMAP_39r,
        };
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

soc_error_t
  qax_fc_pfc_generic_bitmap_verify(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if ((priority < 0) || (priority > SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if(pfc_bitmap_index >= SOC_DPP_DEFS_GET(unit, nof_fc_pfc_generic_bitmaps)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_fc_pfc_generic_bitmap_set(
    SOC_SAND_IN   int                              unit,
    SOC_SAND_IN   int                              priority,
    SOC_SAND_IN   uint32                           pfc_bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        reg_offset;
    soc_reg_above_64_val_t  
        pfc_map_data;

    SOCDNX_INIT_FUNC_DEFS;

    rv = qax_fc_pfc_generic_bitmap_verify(unit, priority, pfc_bitmap_index);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(pfc_map_data);

    /* Read the selected Generic Bitmap */
    rv = soc_reg_above_64_get(unit, pfc_map_reg[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);
    
    /* Update the Generic Bitmap */
    for(reg_offset = 0; reg_offset < SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE / 32; reg_offset++)
    {
        pfc_map_data[reg_offset] = pfc_bitmap->bitmap[reg_offset];
    }

    /* Write the updated Generic Bitmap */
    rv = soc_reg_above_64_set(unit, pfc_map_reg[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_fc_pfc_generic_bitmap_get(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   int       priority,
    SOC_SAND_IN   uint32    pfc_bitmap_index,
    SOC_SAND_OUT   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        reg_offset;
    soc_reg_above_64_val_t  
        pfc_map_data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pfc_bitmap);
    rv = qax_fc_pfc_generic_bitmap_verify(unit, priority, pfc_bitmap_index);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(pfc_map_data);

    /* Read the selected Generic Bitmap */
    rv = soc_reg_above_64_get(unit, pfc_map_reg[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_data);
    SOCDNX_IF_ERR_EXIT(rv);
  
    /* Get the requested bitmap */
    pfc_bitmap->core = 0;
    for(reg_offset = 0; reg_offset < SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE / 32; reg_offset++)
    {
        pfc_bitmap->bitmap[reg_offset] = pfc_map_data[reg_offset];
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_fc_glb_rcs_mask_verify(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    /* Place your code here */
    if (core > SOC_DPP_DEFS_GET(unit, nof_cores)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("core is out of range")));
    }
    
    if (glb_res_dst < SOC_TMC_FC_GLB_RES_TYPE_OCB || glb_res_dst > SOC_TMC_FC_GLB_RES_TYPE_OCB_HEADROOM) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("global resouce dest is error")));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_fc_glb_rcs_mask_set(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_IN uint32                        glb_res_src_bitmap
    )
{
    soc_error_t res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    soc_reg_t reg = CFC_GLB_RSC_CGM_MASKr;
    soc_field_t field = INVALIDf;
    uint32 field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = qax_fc_glb_rcs_mask_verify(unit, core, is_high_prio, glb_res_dst);
    SOCDNX_IF_ERR_EXIT(res);
    if (is_high_prio) {
        field_value = 2;   /* Bit 1 for HP mask */
    }
    else {
        field_value = 1;   /* Bit 0 for LP mask */
    }   

    switch (glb_res_dst) {
    case SOC_TMC_FC_GLB_RES_TYPE_OCB:
        field = GLB_RSC_SRAM_MASKf;  
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_DRAM:
        field = GLB_RSC_MIX_MASKf;  
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_POOL0:
        field = GLB_RSC_POOL_0_MASKf;
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_POOL1:
        field = GLB_RSC_POOL_1_MASKf;
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB_HEADROOM:
        field = GLB_RSC_HDRM_MASKf;
        break;
    default:
        break;
    }

    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
        
    soc_reg_above_64_field32_set(unit, reg, reg_data, field, field_value);    
        
    res = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_fc_glb_rcs_mask_get(
      SOC_SAND_IN int                           unit,
      SOC_SAND_IN int                           core,
      SOC_SAND_IN int                           is_high_prio,
      SOC_SAND_IN SOC_TMC_FC_GLB_RES_TYPE       glb_res_dst,
      SOC_SAND_OUT uint32                       *glb_res_src_bitmap
    )
{
    soc_error_t res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_data;
    soc_reg_t reg = CFC_GLB_RSC_CGM_MASKr;
    soc_field_t field = INVALIDf;
    uint32 field_value = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = qax_fc_glb_rcs_mask_verify(unit, core, is_high_prio, glb_res_dst);
    SOCDNX_IF_ERR_EXIT(res);
    
    SOCDNX_NULL_CHECK(glb_res_src_bitmap);

    switch (glb_res_dst) {
    case SOC_TMC_FC_GLB_RES_TYPE_DRAM:
        field = GLB_RSC_MIX_MASKf;
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB:
        field = GLB_RSC_SRAM_MASKf;
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_POOL0:
        field = GLB_RSC_POOL_0_MASKf;
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_POOL1:
        field = GLB_RSC_POOL_1_MASKf;
        break;
    case SOC_TMC_FC_GLB_RES_TYPE_OCB_HEADROOM:
        field = GLB_RSC_HDRM_MASKf;
        break;
    default:
        break;
    }

    res = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_data);
    SOCDNX_IF_ERR_EXIT(res);
        
    field_value = soc_reg_above_64_field32_get(unit, reg, reg_data, field);

    if (field_value && (glb_res_dst == SOC_TMC_FC_GLB_RES_TYPE_DRAM)) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_DRAM;
    }
    if (field_value && (glb_res_dst == SOC_TMC_FC_GLB_RES_TYPE_OCB)) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_OCB;
    }
    if (field_value && (glb_res_dst == SOC_TMC_FC_GLB_RES_TYPE_POOL0)) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_POOL0;
    }
    if (field_value && (glb_res_dst == SOC_TMC_FC_GLB_RES_TYPE_POOL1)) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_POOL1;
    } 
    if (field_value && (glb_res_dst == SOC_TMC_FC_GLB_RES_TYPE_OCB_HEADROOM)) {
        (*glb_res_src_bitmap) |= SOC_TMC_FC_GLB_RES_TYPE_OCB_HEADROOM;
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

