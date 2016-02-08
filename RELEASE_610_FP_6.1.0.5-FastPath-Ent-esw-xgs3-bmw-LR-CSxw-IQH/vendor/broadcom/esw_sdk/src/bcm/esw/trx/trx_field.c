/* $Id: trx_field.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        field.c
 * Purpose:     BCM56624 Field Processor installation functions.
 */
#include <soc/defs.h>
#if defined(BCM_TRX_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/mem.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/mirror.h>
#include <bcm/tunnel.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/scorpion.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw_dispatch.h>

static soc_field_t _trx_ing_f4_reg[16] = {
    SLICE_0_F4f, SLICE_1_F4f,
    SLICE_2_F4f, SLICE_3_F4f,
    SLICE_4_F4f, SLICE_5_F4f,
    SLICE_6_F4f, SLICE_7_F4f,
    SLICE_8_F4f, SLICE_9_F4f,
    SLICE_10_F4f, SLICE_11_F4f,
    SLICE_12_F4f, SLICE_13_F4f,
    SLICE_14_F4f, SLICE_15_F4f};
static soc_field_t _trx_slice_pairing_field[8] = {
    SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf,
    SLICE5_4_PAIRINGf,   SLICE7_6_PAIRINGf,
    SLICE9_8_PAIRINGf,   SLICE11_10_PAIRINGf,
    SLICE13_12_PAIRINGf, SLICE15_14_PAIRINGf};
static soc_field_t _trx_slice_wide_mode_field[16] = {
    SLICE0_DOUBLE_WIDE_MODEf,
    SLICE1_DOUBLE_WIDE_MODEf,
    SLICE2_DOUBLE_WIDE_MODEf,
    SLICE3_DOUBLE_WIDE_MODEf,
    SLICE4_DOUBLE_WIDE_MODEf,
    SLICE5_DOUBLE_WIDE_MODEf,
    SLICE6_DOUBLE_WIDE_MODEf,
    SLICE7_DOUBLE_WIDE_MODEf,
    SLICE8_DOUBLE_WIDE_MODEf,
    SLICE9_DOUBLE_WIDE_MODEf,
    SLICE10_DOUBLE_WIDE_MODEf,
    SLICE11_DOUBLE_WIDE_MODEf,
    SLICE12_DOUBLE_WIDE_MODEf,
    SLICE13_DOUBLE_WIDE_MODEf,
    SLICE14_DOUBLE_WIDE_MODEf,
    SLICE15_DOUBLE_WIDE_MODEf};
static soc_field_t _trx_efp_slice_mode[4][2] =  {
    {SLICE_0_MODEf, SLICE_0_IPV6_KEY_MODEf},          
    {SLICE_1_MODEf, SLICE_1_IPV6_KEY_MODEf},          
    {SLICE_2_MODEf, SLICE_2_IPV6_KEY_MODEf},          
    {SLICE_3_MODEf, SLICE_3_IPV6_KEY_MODEf}};
static soc_field_t _trx_vfp_field_sel[4][2] = {
    {SLICE_0_F2f, SLICE_0_F3f},
    {SLICE_1_F2f, SLICE_1_F3f},
    {SLICE_2_F2f, SLICE_2_F3f},
    {SLICE_3_F2f, SLICE_3_F3f}};
static soc_field_t _trx_vfp_double_wide_sel[]= {
    SLICE_0_DOUBLE_WIDE_KEY_SELECTf,
    SLICE_1_DOUBLE_WIDE_KEY_SELECTf,
    SLICE_2_DOUBLE_WIDE_KEY_SELECTf,
    SLICE_3_DOUBLE_WIDE_KEY_SELECTf};
static soc_field_t _trx_vfp_ip_header_sel[]= {
    SLICE_0_IP_FIELD_SELECTf, SLICE_1_IP_FIELD_SELECTf,
    SLICE_2_IP_FIELD_SELECTf, SLICE_3_IP_FIELD_SELECTf};
static soc_field_t _trx_field_sel[16][3] = {
    {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f},
    {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f},
    {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f},
    {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f},
    {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f},
    {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f},
    {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f},
    {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f},
    {SLICE8_F1f, SLICE8_F2f, SLICE8_F3f},
    {SLICE9_F1f, SLICE9_F2f, SLICE9_F3f},
    {SLICE10_F1f, SLICE10_F2f, SLICE10_F3f},
    {SLICE11_F1f, SLICE11_F2f, SLICE11_F3f},
    {SLICE12_F1f, SLICE12_F2f, SLICE12_F3f},
    {SLICE13_F1f, SLICE13_F2f, SLICE13_F3f},
    {SLICE14_F1f, SLICE14_F2f, SLICE14_F3f},
    {SLICE15_F1f, SLICE15_F2f, SLICE15_F3f}};
static soc_field_t d_type_sel[] =  {
    SLICE0_D_TYPE_SELf, SLICE1_D_TYPE_SELf,
    SLICE2_D_TYPE_SELf, SLICE3_D_TYPE_SELf,
    SLICE4_D_TYPE_SELf, SLICE5_D_TYPE_SELf,
    SLICE6_D_TYPE_SELf, SLICE7_D_TYPE_SELf,
    SLICE8_D_TYPE_SELf, SLICE9_D_TYPE_SELf,
    SLICE10_D_TYPE_SELf, SLICE11_D_TYPE_SELf,
    SLICE12_D_TYPE_SELf, SLICE13_D_TYPE_SELf,
    SLICE14_D_TYPE_SELf, SLICE15_D_TYPE_SELf};
static soc_field_t s_type_sel[] =  {
    SLICE0_S_TYPE_SELf, SLICE1_S_TYPE_SELf,
    SLICE2_S_TYPE_SELf, SLICE3_S_TYPE_SELf,
    SLICE4_S_TYPE_SELf, SLICE5_S_TYPE_SELf,
    SLICE6_S_TYPE_SELf, SLICE7_S_TYPE_SELf,
    SLICE8_S_TYPE_SELf, SLICE9_S_TYPE_SELf,
    SLICE10_S_TYPE_SELf, SLICE11_S_TYPE_SELf,
    SLICE12_S_TYPE_SELf, SLICE13_S_TYPE_SELf,
    SLICE14_S_TYPE_SELf, SLICE15_S_TYPE_SELf};
static soc_field_t f4_sel[] =  {
    SLICE_0_F4f, SLICE_1_F4f,
    SLICE_2_F4f, SLICE_3_F4f,
    SLICE_4_F4f, SLICE_5_F4f,
    SLICE_6_F4f, SLICE_7_F4f,
    SLICE_8_F4f, SLICE_9_F4f,
    SLICE_10_F4f, SLICE_11_F4f,
    SLICE_12_F4f, SLICE_13_F4f,
    SLICE_14_F4f, SLICE_15_F4f};
static soc_field_t _trx_ifp_double_wide_key[] = {
    SLICE0_DOUBLE_WIDE_KEY_SELECTf,
    SLICE1_DOUBLE_WIDE_KEY_SELECTf,
    SLICE2_DOUBLE_WIDE_KEY_SELECTf,
    SLICE3_DOUBLE_WIDE_KEY_SELECTf,
    SLICE4_DOUBLE_WIDE_KEY_SELECTf,
    SLICE5_DOUBLE_WIDE_KEY_SELECTf,
    SLICE6_DOUBLE_WIDE_KEY_SELECTf,
    SLICE7_DOUBLE_WIDE_KEY_SELECTf,
    SLICE8_DOUBLE_WIDE_KEY_SELECTf,
    SLICE9_DOUBLE_WIDE_KEY_SELECTf,
    SLICE10_DOUBLE_WIDE_KEY_SELECTf,
    SLICE11_DOUBLE_WIDE_KEY_SELECTf,
    SLICE12_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE13_DOUBLE_WIDE_KEY_SELECTf,
    SLICE14_DOUBLE_WIDE_KEY_SELECTf,
    SLICE15_DOUBLE_WIDE_KEY_SELECTf};
static soc_field_t _trx_src_class_id_sel[] = {
    SLICE_0_SRC_CLASS_ID_SELf,
    SLICE_1_SRC_CLASS_ID_SELf,
    SLICE_2_SRC_CLASS_ID_SELf,
    SLICE_3_SRC_CLASS_ID_SELf,
    SLICE_4_SRC_CLASS_ID_SELf,
    SLICE_5_SRC_CLASS_ID_SELf,
    SLICE_6_SRC_CLASS_ID_SELf,
    SLICE_7_SRC_CLASS_ID_SELf,
    SLICE_8_SRC_CLASS_ID_SELf,
    SLICE_9_SRC_CLASS_ID_SELf,
    SLICE_10_SRC_CLASS_ID_SELf,
    SLICE_11_SRC_CLASS_ID_SELf,
    SLICE_12_SRC_CLASS_ID_SELf,
    SLICE_13_SRC_CLASS_ID_SELf,
    SLICE_14_SRC_CLASS_ID_SELf,
    SLICE_15_SRC_CLASS_ID_SELf};
static soc_field_t _trx_dst_class_id_sel[] = {
    SLICE_0_DST_CLASS_ID_SELf,
    SLICE_1_DST_CLASS_ID_SELf,
    SLICE_2_DST_CLASS_ID_SELf,
    SLICE_3_DST_CLASS_ID_SELf,
    SLICE_4_DST_CLASS_ID_SELf,
    SLICE_5_DST_CLASS_ID_SELf,
    SLICE_6_DST_CLASS_ID_SELf,
    SLICE_7_DST_CLASS_ID_SELf,
    SLICE_8_DST_CLASS_ID_SELf,
    SLICE_9_DST_CLASS_ID_SELf,
    SLICE_10_DST_CLASS_ID_SELf,
    SLICE_11_DST_CLASS_ID_SELf,
    SLICE_12_DST_CLASS_ID_SELf,
    SLICE_13_DST_CLASS_ID_SELf,
    SLICE_14_DST_CLASS_ID_SELf,
    SLICE_15_DST_CLASS_ID_SELf};
static soc_field_t _trx_interface_class_id_sel[] = {
    SLICE_0_INTERFACE_CLASS_ID_SELf,
    SLICE_1_INTERFACE_CLASS_ID_SELf,
    SLICE_2_INTERFACE_CLASS_ID_SELf,
    SLICE_3_INTERFACE_CLASS_ID_SELf,
    SLICE_4_INTERFACE_CLASS_ID_SELf,
    SLICE_5_INTERFACE_CLASS_ID_SELf,
    SLICE_6_INTERFACE_CLASS_ID_SELf,
    SLICE_7_INTERFACE_CLASS_ID_SELf,
    SLICE_8_INTERFACE_CLASS_ID_SELf,
    SLICE_9_INTERFACE_CLASS_ID_SELf,
    SLICE_10_INTERFACE_CLASS_ID_SELf,
    SLICE_11_INTERFACE_CLASS_ID_SELf,
    SLICE_12_INTERFACE_CLASS_ID_SELf,
    SLICE_13_INTERFACE_CLASS_ID_SELf,
    SLICE_14_INTERFACE_CLASS_ID_SELf,
    SLICE_15_INTERFACE_CLASS_ID_SELf};

/*
 * Function:
 *     _bcm_field_trx_egress_selcode_get
 * Purpose:
 *     Finds a select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     fg        - (IN/OUT)Select code information filled into the group.  
 * 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_egress_selcode_get(int unit, _field_stage_t *stage_fc, 
                               bcm_field_qset_t *qset_req,
                               _field_group_t *fg)
{
    int             rv;          /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    if  (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        /* Attempt _BCM_FIELD_EFP_KEY4  (L2 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY4, 0, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY5  (L3 any key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY5, 0, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY1  (IPv4 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY1, 0, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY2  (IPv6 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY2, 0, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }
    }  else  {
        /* L2 + L3 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY5,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* L2 + L3 v4 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY1,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* L2 + L3 v6  double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY2,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* IPv6 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY3,
                                           _BCM_FIELD_EFP_KEY2, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }
    }
    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *     _field_egress_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_fb_mode_install that sets the mode of a
 *     slice in a register value that is to be used for FP_SLICE_CONFIGr.
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure. 
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_field_egress_mode_set(int unit, uint8 slice_numb, 
                          _field_group_t *fg, uint8 flags)
{
    uint32 mode_val[2];
    /* Input parameters check. */
    if ((NULL == fg) || (slice_numb >= COUNTOF(_trx_efp_slice_mode))) {
        return (BCM_E_PARAM);
    }

    mode_val[1]  = _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6;

    if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        /* IP + L2 Double wide key. */
        if ((_BCM_FIELD_EFP_KEY5 == fg->sel_codes[0].fpf3) && \
            (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[1].fpf3)) {
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY;
        } 

        /* DstIP6 + SrcIp6 or IPv4 + L2 Double wide key. */
        if (((_BCM_FIELD_EFP_KEY3 == fg->sel_codes[0].fpf3) && \
            (_BCM_FIELD_EFP_KEY2 == fg->sel_codes[1].fpf3)) ||
            BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
        } 

        /* IPv6 + L2 Double wide key. */
        else if ((_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) && \
                 (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[1].fpf3)) {
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6;
            if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
            }
        } else {
            /* IPv4/Don't care + L2 Double wide key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY;
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 2, 
                                     _trx_efp_slice_mode[slice_numb], mode_val));
    } else {
        if (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[0].fpf3) {
            /* L2 - Slice mode. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2;

        } else  if (_BCM_FIELD_EFP_KEY5 == fg->sel_codes[0].fpf3) {
            /* L3 - Any single wide key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY;

        } else  if ((_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3)  && 
                    BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
            /* L3 - IPv4 single wide key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3;

        } else if ((_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3)) {
            /* L3 - IPv6 single wide key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3;
            if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
            }
        } else {
            /* L3 common key. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY;
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 2,
                                     _trx_efp_slice_mode[slice_numb], mode_val));

    } 
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_egress_slice_clear
 *
 * Purpose:
 *     Reset slice configuraton on group deletion event. 
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_egress_slice_clear(int unit, uint8 slice_numb)
{
    uint32 mode_val[2] =  {0, 0};
    int rv; 

    /* Input parameters check. */
    if (slice_numb >= COUNTOF(_trx_efp_slice_mode)) {
        return (BCM_E_PARAM);
    }
    
    rv = soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                             2, _trx_efp_slice_mode[slice_numb], mode_val);

    return (rv);
}

/*
 * Function:
 *     _field_trx_mode_set
 *
 * Purpose:
 *    Auxiliary routine used to set group pairing mode.
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure. 
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int 
_field_trx_mode_set(int unit, uint8 slice_numb, _field_group_t *fg, uint8 flags) 
{
    int rv;     /* Operation return status. */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_fb_ingress_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv  = _bcm_field_fb_lookup_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_egress_mode_set(unit, slice_numb, fg, flags);
          break;
#if defined (BCM_TRIUMPH_SUPPORT)
      case _BCM_FIELD_STAGE_EXTERNAL:
          rv = _bcm_field_tr_external_mode_set(unit, slice_numb, fg, flags);
          break;
#endif /* BCM_TRIUMPH_SUPPORT */
      default: 
          rv = BCM_E_PARAM;
    }
    return (rv);
}


/*
 * Function:
 *      _bcm_trx_range_checker_selcodes_update
 *
 * Purpose:
 *     Update group select codes based on range checker id 
 *     used in field group entry.
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     f_ent         - (IN) Field entry.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_trx_range_checker_selcodes_update(int unit, _field_entry_t *f_ent)
                                       
{
    uint8            slice_number;          /* Slices iterator.         */
    uint8            entry_part;            /* Wide entry part number.  */
    uint32           buf[SOC_MAX_MEM_FIELD_WORDS];/* HW entry buffer.   */
    _field_sel_t     *sel;                  /* Group field selectors.   */
    _field_group_t   *fg;                   /* Field group structure.   */
    _field_slice_t   *fs;                   /* Field slice structure.   */
    int              rv;                    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == f_ent)  {
        return (BCM_E_PARAM);
    } 

    fg = f_ent->group;

    /* Get tcam part. */
    rv = _bcm_field_entry_flags_to_tcam_part (f_ent->flags, fg->flags,
                                              &entry_part);
    BCM_IF_ERROR_RETURN(rv);

    /* Per slice selectors installed in primary portion only. */
    if (fg->sel_codes[entry_part].intraslice) {
        entry_part--;
    }

    /* Get slice number for the entry part. */
    rv = _bcm_field_tcam_part_to_slice_number(entry_part, fg->flags,
                                              &slice_number);
    BCM_IF_ERROR_RETURN(rv);

    /* Update selector value in all group slices. */
    fs = fg->slices + slice_number;

    sel = fg->sel_codes + entry_part;
    if (3 == sel->intf_class_sel) {
        return (BCM_E_NONE);
    } else if (_FP_SELCODE_DONT_CARE != fs->intf_class_sel) {
        return (BCM_E_RESOURCE);
    } else {
        sel->intf_class_sel = 3;  /* Range checker selector. */
    }

    /* Update FP_SLICE_KEY_CONTROL memory entry. */
    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    rv = soc_mem_read(unit, FP_SLICE_KEY_CONTROLm, MEM_BLOCK_ANY, 0, buf);
    BCM_IF_ERROR_RETURN(rv);

    while (fs != NULL) {
        /* Set interface class select field. */ 
        soc_mem_field32_set(unit, FP_SLICE_KEY_CONTROLm, buf, 
                            _trx_interface_class_id_sel[fs->slice_number], 
                            sel->intf_class_sel);

        fs->intf_class_sel = sel->intf_class_sel;
        fs = fs->next;
    }
    rv = soc_mem_write(unit, FP_SLICE_KEY_CONTROLm, MEM_BLOCK_ALL, 0, buf);
    return (rv);
}

/*
 * Function:
 *     _field_trx_ingress_pfs_bmap_get
 *
 * Purpose:
 *     Fill in set of (PFS) indexes applicable for the specific group.
 *
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     fg            - (IN) Field group.
 *     pbmp          - (IN) Group  active port bit map.
 *     selcode_index - (IN) Index into select codes array.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_ingress_pfs_bmap_get(int unit, _field_group_t *fg, 
                                bcm_pbmp_t *pbmp, int selcode_index, 
                                SHR_BITDCL *pfs_bmp)
{
    bcm_port_t         idx;               /* Device pfs indexes iterator.*/
    int                rv;                /* Operation return status.    */
    _field_sel_t       *sel;              /* Group field selectors.      */
    bcm_port_config_t  port_config;       /* Device port configuration.  */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == pbmp) || (NULL == pfs_bmp)) {
        return (BCM_E_PARAM);
    }

    /* Get group select codes. */
    sel = &fg->sel_codes[selcode_index];

    /* Read device port bitmaps. */
    rv = bcm_esw_port_config_get(unit, &port_config);
    BCM_IF_ERROR_RETURN(rv);
    if (fg->flags & _FP_GROUP_WLAN) {
        SHR_BITSET(pfs_bmp, soc_mem_index_max(unit, FP_PORT_FIELD_SELm));
    } else if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        ((selcode_index == 1) || (selcode_index == 3)) &&
        (sel->fpf2 != _FP_SELCODE_DONT_CARE)) {
        sal_memcpy(pfs_bmp, &port_config.all, sizeof(bcm_pbmp_t));
    } else {
        sal_memcpy(pfs_bmp, pbmp, sizeof(bcm_pbmp_t));
    }

#if defined(BCM_ENDURO_SUPPORT)
    if (SOC_IS_ENDURO(unit)) {
        /* Have to set up CPU port for Enduro. */
        SHR_BITSET(pfs_bmp, 34);
    }
#endif /* BCM_ENDURO_SUPPORT */

    for (idx = 0; idx < BCM_PBMP_PORT_MAX; idx++) {
        if ((SHR_BITGET(pfs_bmp, idx)) && \
            (PBMP_MEMBER(port_config.hg, idx))) {
#if defined(BCM_SCORPION_SUPPORT)
            if (SOC_IS_SC_CQ(unit)) {
                SHR_BITSET(pfs_bmp, idx + 29);
                continue;
            }
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT)
            if (SOC_IS_ENDURO(unit)) {
                switch (idx) {
                  case 26:
                      SHR_BITSET(pfs_bmp, 30);
                      break;
                  case 27:
                      SHR_BITSET(pfs_bmp, 31);
                      break;
                  case 28:
                      SHR_BITSET(pfs_bmp, 32);
                      break;
                  case 29:
                      SHR_BITSET(pfs_bmp, 33);
                      break;
                  default:
                      return (BCM_E_PORT);
                }
                continue;
            }
#endif /* BCM_ENDURO_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
            if (soc_feature(unit, soc_feature_lport_tab_profile)) {
                 SHR_BITSET(pfs_bmp,
                            soc_mem_index_max(unit, FP_PORT_FIELD_SELm) - 1);
                 break;
            }

            if (SOC_IS_TR_VL(unit)) {
                switch (idx) {
                  case 2:
                      SHR_BITSET(pfs_bmp, 54);
                      break;
                  case 14:
                      SHR_BITSET(pfs_bmp, 55);
                      break;
                  case 26:
                      SHR_BITSET(pfs_bmp, 56);
                      break;
                  case 27:
                      SHR_BITSET(pfs_bmp, 57);
                      break;
                  case 28:
                      SHR_BITSET(pfs_bmp, 58);
                      break;
                  case 29:
                      SHR_BITSET(pfs_bmp, 59);
                      break;
                  case 30:
                      SHR_BITSET(pfs_bmp, 60);
                      break;
                  case 31:
                      SHR_BITSET(pfs_bmp, 61);
                      break;
                  case 0:
                      SHR_BITSET(pfs_bmp, 62);
                      break;
                  default:
                      return (BCM_E_PORT);
                }
            } 
#endif /* BCM_TRIUMPH_SUPPORT */
        }
    }
    return (rv);
}
/*
 * Function:
 *     _field_trx_ingress_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     fg            - (IN) Field group.
 *     slice_numb    - (IN) Slice number group installed in. 
 *     pbmp          - (IN) Group  active port bit map.
 *     selcode_index - (IN) Index into select codes array.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_ingress_selcodes_install(int unit, _field_group_t *fg, 
                                    uint8 slice_num, bcm_pbmp_t *pbmp, 
                                    int selcode_index)
{
    bcm_port_t       idx;                   /* Device pfs indexes iterator.*/
    uint32           buf[SOC_MAX_MEM_FIELD_WORDS];/* HW entry buffer.      */
    SHR_BITDCL       *pfs_bmp;              /* PFS bitmap.                 */
    int              pfs_idx_count;         /* PFS index count.            */
    _field_sel_t     *sel;                  /* Group field selectors.      */
    uint32           value;                 /* Per slice selector.         */ 
    int              rv;                    /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == fg) || (slice_num >= 16))  {
        return (BCM_E_PARAM);
    } 

    sel = &fg->sel_codes[selcode_index];

    /* Get port field select table size and allocated bitmap of indexes
     * applicable to the group. 
     */
    pfs_bmp = NULL;
    pfs_idx_count = soc_mem_index_count(unit, FP_PORT_FIELD_SELm);
    _FP_XGS3_ALLOC(pfs_bmp,
                   MAX(SHR_BITALLOCSIZE(pfs_idx_count), sizeof(bcm_pbmp_t)),
                   "PFS bmp");
    if (NULL == pfs_bmp) {
        return (BCM_E_MEMORY);
    }

    /* Populate pfs indexes applicable for the group. */
    rv = _field_trx_ingress_pfs_bmap_get(unit, fg, pbmp,
                                         selcode_index, pfs_bmp);
    if (BCM_FAILURE(rv)) {
        sal_free(pfs_bmp);
        return (rv);
    }

    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        ((selcode_index == 1) || (selcode_index == 3))) {

        /* Write appropriate values in FP_DOUBLE_WIDE_F4_SELECTr and */
        if ((sel->fpf4 != _FP_SELCODE_DONT_CARE) &&
            SOC_REG_FIELD_VALID(unit, FP_DOUBLE_WIDE_F4_SELECTr,
                                f4_sel[slice_num])) {
            rv = soc_reg_field32_modify(unit, FP_DOUBLE_WIDE_F4_SELECTr, 
                                        REG_PORT_ANY, f4_sel[slice_num],  
                                        sel->fpf4);
            if (BCM_FAILURE(rv)) {
                sal_free(pfs_bmp);
                return (rv);
            }
        }

        if ((sel->fpf2 != _FP_SELCODE_DONT_CARE) &&
            SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm,
                                _trx_ifp_double_wide_key[slice_num])) {
            /* Do the same thing for each entry in FP_PORT_FIELD_SEL table */
            for (idx = 0; idx < pfs_idx_count; idx++) { 
                if (0 == SHR_BITGET(pfs_bmp, idx)) {
                    continue;
                }
                sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
                rv = soc_mem_read(unit, FP_PORT_FIELD_SELm, MEM_BLOCK_ANY, 
                                  idx, buf);
                if (BCM_FAILURE(rv)) {
                    sal_free(pfs_bmp);
                    return (rv);
                }
                soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf, 
                                    _trx_ifp_double_wide_key[slice_num],
                                    sel->fpf2);

                rv = soc_mem_write(unit, FP_PORT_FIELD_SELm, MEM_BLOCK_ALL, 
                                   idx, buf);
                if (BCM_FAILURE(rv)) {
                    sal_free(pfs_bmp);
                    return (rv);
                }
            }
        }
    } else {
        /* Iterate over all ports */
        for (idx = 0; idx < pfs_idx_count; idx++) {
            if (0 == SHR_BITGET(pfs_bmp, idx)) {
                continue;
            }
            sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
            /* Read Port's current entry in FP_PORT_FIELD_SEL table */
            rv = soc_mem_read(unit, FP_PORT_FIELD_SELm, MEM_BLOCK_ANY, idx, buf);
            if (BCM_FAILURE(rv)) {
                sal_free(pfs_bmp);
                return (rv);
            }

            /* modify 0-3 fields depending on state of SELCODE_INVALID */
            if (sel->fpf1 != _FP_SELCODE_DONT_CARE) {
                soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf, 
                                    _trx_field_sel[slice_num][0], sel->fpf1);
            }

            if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
                soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf, 
                                    _trx_field_sel[slice_num][1], sel->fpf2);
            }
            if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
                soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf, 
                                    _trx_field_sel[slice_num][2], sel->fpf3);
            }

            /* Set destination forwarding type selection. */
            if (sel->dst_fwd_entity_sel != _FP_SELCODE_DONT_CARE) {
                switch (sel->dst_fwd_entity_sel) {
                  case _bcmFieldFwdEntityMplsGport:
                  case _bcmFieldFwdEntityMimGport:
                  case _bcmFieldFwdEntityWlanGport:
                      value = 3;
                      break;
                  case _bcmFieldFwdEntityL3Egress:
                      value = 1;
                      break;
                  default:
                      value = 0;
                      break;
                }
                if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, 
                                        d_type_sel[slice_num])) {
                    soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf, 
                                        d_type_sel[slice_num], value);
                }
            }
            /* Set ingress entity type selection. */
            if (sel->ingress_entity_sel != _FP_SELCODE_DONT_CARE) {
                switch (sel->dst_fwd_entity_sel) {
                  case _bcmFieldFwdEntityMplsGport:
                  case _bcmFieldFwdEntityMimGport:
                  case _bcmFieldFwdEntityWlanGport:
                      value = 3;
                      break;
                  case _bcmFieldFwdEntityModPortGport:
                      value = 2;
                      break;
                  case _bcmFieldFwdEntityGlp:
                      value = 1;
                      break;
                  default:
                      value = 0;
                      break;
                }
                if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, 
                                        s_type_sel[slice_num])) {
                    soc_mem_field32_set(unit, FP_PORT_FIELD_SELm, buf, 
                                        s_type_sel[slice_num], value);
                }
            }

            /* Write each port's new entry */
            rv = soc_mem_write(unit, FP_PORT_FIELD_SELm, 
                               MEM_BLOCK_ALL, idx, buf);
            if (BCM_FAILURE(rv)) {
                sal_free(pfs_bmp);
                return (rv);
            }
        }
    }
    sal_free(pfs_bmp);

    /* Update FP_SLICE_KEY_CONTROL memory entry. */
    sal_memset(buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    rv = soc_mem_read(unit, FP_SLICE_KEY_CONTROLm, MEM_BLOCK_ANY, 0, buf);
    BCM_IF_ERROR_RETURN(rv);


    /* Set source class select field. */ 
    if (sel->src_class_sel != _FP_SELCODE_DONT_CARE) {
        soc_mem_field32_set(unit, FP_SLICE_KEY_CONTROLm, buf, 
                            _trx_src_class_id_sel[slice_num], 
                            sel->src_class_sel);
    }
    /* Set destination class select field. */ 
    if (sel->dst_class_sel != _FP_SELCODE_DONT_CARE) {
        soc_mem_field32_set(unit, FP_SLICE_KEY_CONTROLm, buf, 
                            _trx_dst_class_id_sel[slice_num], 
                            sel->dst_class_sel);
    }

    /* Set interface class select field. */ 
    if (sel->intf_class_sel != _FP_SELCODE_DONT_CARE) {
        soc_mem_field32_set(unit, FP_SLICE_KEY_CONTROLm, buf, 
                            _trx_interface_class_id_sel[slice_num], 
                            sel->intf_class_sel);
    }
    rv = soc_mem_write(unit, FP_SLICE_KEY_CONTROLm, MEM_BLOCK_ALL, 0, buf);
    return (rv);
}

/*
 * Function:
 *     _field_trx_ingress_slice_clear
 *
 * Purpose:
 *     Resets the IFP field slice configuration.
 *
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Field slice number.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_ingress_slice_clear(int unit, uint8 slice_numb)
{
    bcm_port_config_t           port_config;
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
    soc_field_t                 dw_fld;
    int                         rv;

    /* Read device port configuration. */ 
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    /* Iterate over all ports */
    BCM_PBMP_ITER(port_config.all, port) {
        /* Read Port's current entry in FP_PORT_FIELD_SEL table */
        rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, 
                                     port, &pfs_entry);
        BCM_IF_ERROR_RETURN(rv);
        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           _trx_field_sel[slice_numb][0], 0);
        BCM_IF_ERROR_RETURN(rv);

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           _trx_field_sel[slice_numb][1], 0);
        BCM_IF_ERROR_RETURN(rv);
        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           _trx_field_sel[slice_numb][2], 0);
        BCM_IF_ERROR_RETURN(rv);

        dw_fld = _trx_ifp_double_wide_key[slice_numb]; 
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);
        }

        dw_fld = _trx_slice_wide_mode_field[slice_numb];
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);
        }

        dw_fld = _trx_slice_pairing_field[slice_numb / 2];
        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);

        rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port, &pfs_entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    dw_fld = _trx_ing_f4_reg[slice_numb];
    if (SOC_REG_FIELD_VALID(unit, FP_DOUBLE_WIDE_F4_SELECTr, dw_fld)) {
        rv = soc_reg_field32_modify(unit, FP_DOUBLE_WIDE_F4_SELECTr,
                                    REG_PORT_ANY, dw_fld,  0);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_trx_lookup_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *     for VFP (_BCM_FIELD_STAGE_LOOKUP) lookup stage.
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_lookup_selcodes_install(int unit, _field_group_t *fg, 
                                   uint8 slice_numb, int selcode_index)
{
    uint32        reg_val;
    _field_sel_t  *sel;
    int           rv;
    sel = &fg->sel_codes[selcode_index];

    rv = READ_VFP_KEY_CONTROLr(unit, &reg_val); 
    BCM_IF_ERROR_RETURN(rv);

    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        (selcode_index % 2)) {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _trx_vfp_double_wide_sel[slice_numb], sel->fpf2);
        }
    } else {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _trx_vfp_field_sel[slice_numb][0], sel->fpf2); 
        }
        if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _trx_vfp_field_sel[slice_numb][1], sel->fpf3); 
        }
    }
    rv = WRITE_VFP_KEY_CONTROLr(unit, reg_val);
    BCM_IF_ERROR_RETURN(rv);

    /* Set inner/outer ip header selection. */
    if (sel->ip_header_sel != _FP_SELCODE_DONT_CARE) {
        rv = soc_reg_field32_modify(unit, VFP_KEY_CONTROL_2r, REG_PORT_ANY, 
                                    _trx_vfp_ip_header_sel[slice_numb], 
                                    sel->ip_header_sel);
    }
    return (rv);
}

/*
 * Function:
 *     _field_trx_lookup_slice_clear
 *
 * Purpose:
 *     Reset slice configuraton on group deletion event. 
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_trx_lookup_slice_clear(int unit, uint8 slice_numb)
{
    uint32 reg_val;
    int    rv;

    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val)); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _trx_vfp_double_wide_sel[slice_numb], 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _trx_vfp_field_sel[slice_numb][0], 0); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _trx_vfp_field_sel[slice_numb][1], 0); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _trx_slice_pairing_field[slice_numb / 2], 0);
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));

    rv = soc_reg_field32_modify(unit, VFP_KEY_CONTROL_2r, REG_PORT_ANY, 
                                _trx_vfp_ip_header_sel[slice_numb], 0);
    return (rv);
}

/*
 * Function:
 *     _bcm_field_trx_slice_clear
 *
 * Purpose:
 *     Clear slice configuration on group removal
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - Field group slice belongs to
 *     fs    - Field slice structure.
 *
 * Returns:
 *     BCM_E_XXX
 *
 */
int
_bcm_field_trx_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs) 
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_trx_ingress_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_trx_lookup_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_trx_egress_slice_clear(unit, fs->slice_number);
          break;
#if defined(BCM_TRIUMPH_SUPPORT)
      case _BCM_FIELD_STAGE_EXTERNAL:
          rv = _bcm_field_tr_external_slice_clear(unit, fg);
          break;
#endif /* BCM_TRIUMPH_SUPPORT */
      default: 
          rv = BCM_E_INTERNAL;
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_trx_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_trx_selcodes_install(int unit, _field_group_t *fg, 
                                uint8 slice_numb, bcm_pbmp_t pbmp, 
                                int selcode_index)
{
    int rv;    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Set slice mode. Single/Double/Triple, Intraslice */
    rv = _field_trx_mode_set(unit, slice_numb, fg, fg->flags);
    BCM_IF_ERROR_RETURN(rv);

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS: 
          rv = _field_trx_ingress_selcodes_install(unit, fg, slice_numb,
                                                   &pbmp, selcode_index);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_trx_lookup_selcodes_install(unit, fg, slice_numb,
                                                  selcode_index);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
      case _BCM_FIELD_STAGE_EXTERNAL:
          rv = (BCM_E_NONE);
          break;
      default:
          rv = (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_trx_qual_lists_get
 * Purpose:
 *     Build a group's qualifiers array by assembling
 *     qualifiers from each select code.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     stage_fc  - (IN) Stage field control structure. 
 *     fg        - (IN) Group control structure.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_qual_lists_get(int unit, _field_stage_t *stage_fc,
                             _field_group_t *fg)
{
    return (BCM_E_NONE); 
}


/* 
 * Function:
 *     _bcm_field_trx_tcam_get
 * Purpose:
 *    Get the rules to be written into tcam.
 * Parameters:
 *     unit      -  (IN) BCM device number.
 *     tcam_mem  -  (IN) TCAM memory
 *     f_ent     -  (IN)  Field entry structure to get tcam info from.
 *     buf       -  (OUT) TCAM entry
 * Returns:
 *     BCM_E_NONE  - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_trx_tcam_get(int unit, soc_mem_t mem, 
                        _field_entry_t *f_ent, uint32 *buf)
{
    soc_field_t         mask_field;
    _field_tcam_t       *tcam; 
    _field_group_t      *fg;

    tcam = &f_ent->tcam;
    fg = f_ent->group;

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
        soc_mem_field32_set(unit, mem, buf, VALIDf, 
                            (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 2);
            /* Intra-slice double wide key */
            soc_mem_field_set(unit, mem, buf, DATAf, tcam->key);
            soc_mem_field_set(unit, mem, buf, DATA_MASKf, tcam->mask);
    } else {
        if (_BCM_FIELD_STAGE_LOOKUP == fg->stage_id) {
            mask_field = MASKf;
        } else if (_BCM_FIELD_STAGE_EGRESS == fg->stage_id){
            mask_field = KEY_MASKf;
        } else {
            return (BCM_E_PARAM);
        }
        soc_mem_field_set(unit, mem, buf, KEYf, tcam->key);
        soc_mem_field_set(unit, mem, buf, mask_field, tcam->mask);
        soc_mem_field32_set(unit, mem, buf, VALIDf, 
                            (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 2);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_tcp_ttl_tos_init
 * Purpose:
 *     Initialize the TCP_FN, TTL_FN and TOS_FN tables
 * Parameters:
 *     unit       - (IN) BCM device number.
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_trx_tcp_ttl_tos_init(int unit)
{
    int i;
    tcp_fn_entry_t tcp_entry;
    ttl_fn_entry_t ttl_entry;
    tos_fn_entry_t tos_entry;
    
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    sal_memset(&tcp_entry, 0, sizeof(tcp_fn_entry_t));
    sal_memset(&ttl_entry, 0, sizeof(ttl_fn_entry_t));
    sal_memset(&tos_entry, 0, sizeof(tos_fn_entry_t));

    /* TCP_FN table */
    for (i = soc_mem_index_min(unit, TCP_FNm); 
         i <= soc_mem_index_max(unit, TCP_FNm); i++) {

        soc_mem_field32_set(unit, TCP_FNm, &tcp_entry, FN0f, i);
        soc_mem_field32_set(unit, TCP_FNm, &tcp_entry, FN1f, i);

        soc_mem_write(unit, TCP_FNm, MEM_BLOCK_ALL, i, &tcp_entry);
    }

    /* TTL_FN table */
    for (i = soc_mem_index_min(unit, TTL_FNm);
         i <= soc_mem_index_max(unit, TTL_FNm); i++) {

        soc_mem_field32_set(unit, TTL_FNm, &ttl_entry, FN0f, i);
        soc_mem_field32_set(unit, TTL_FNm, &ttl_entry, FN1f, i);

        soc_mem_write(unit, TTL_FNm, MEM_BLOCK_ALL, i, &ttl_entry);
    }

    /* TOS_FN table */
    for (i = soc_mem_index_min(unit, TOS_FNm);
         i <= soc_mem_index_max(unit, TOS_FNm); i++) {

        soc_mem_field32_set(unit, TOS_FNm, &tos_entry, FN0f, i);
        soc_mem_field32_set(unit, TOS_FNm, &tos_entry, FN1f, i);

        soc_mem_write(unit, TOS_FNm, MEM_BLOCK_ALL, i, &tos_entry);
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_trx_write_slice_map_egress
 *
 * Purpose:
 *     Write the EFP_SLICE_MAP (EGRESS)
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *
 * Returns:
 *     BCM_E_XXX
 *     
 * Notes:
 */
int
_bcm_field_trx_write_slice_map_egress(int unit, _field_stage_t *stage_fc)
{
    soc_field_t field;               /* HW entry fields.         */
    uint32 map_entry;                /* HW entry buffer.         */
    int vmap_size;                   /* Virtual map index count. */
    uint32 value;                    /* Field entry value.       */
    int idx;                         /* Map fields iterator.     */     
    int rv;                          /* Operation return status. */

    uint32 virtual_to_physical_map[] = {
        VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf};
    uint32 virtual_to_group_map[] = { 
        VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf};

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_EFP_SLICE_MAPr(unit, &map_entry);
    BCM_IF_ERROR_RETURN(rv);
    for (idx = 0; idx < vmap_size; idx++) {
        value = (stage_fc->vmap[_FP_VMAP_DEFAULT][idx]).vmap_key;
        field = virtual_to_physical_map[idx];
        soc_reg_field_set(unit, EFP_SLICE_MAPr, &map_entry, field, value);

        value = (stage_fc->vmap[_FP_VMAP_DEFAULT][idx]).virtual_group;
        field = virtual_to_group_map[idx];
        soc_reg_field_set(unit, EFP_SLICE_MAPr, &map_entry, field, value);
    }

    rv = WRITE_EFP_SLICE_MAPr(unit, map_entry);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_write_slice_map_vfp
 *
 * Purpose:
 *     Write the VFP_SLICE_MAP (LOOKUP)
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *
 * Returns:
 *     BCM_E_XXX
 *     
 */
int
_bcm_field_trx_write_slice_map_vfp(int unit, _field_stage_t *stage_fc)
{
    soc_field_t field;               /* HW entry fields.         */
    uint32 map_entry;                /* HW entry buffer.         */
    int vmap_size;                   /* Virtual map index count. */
    uint32 value;                    /* Field entry value.       */
    int idx;                         /* Map fields iterator.     */     
    int rv;                          /* Operation return status. */

    uint32 virtual_to_physical_map[] = {
        VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf};
    uint32 virtual_to_group_map[] = {
        VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf};

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_VFP_SLICE_MAPr(unit, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        value = (stage_fc->vmap[_FP_VMAP_DEFAULT][idx]).vmap_key; 
        field = virtual_to_physical_map[idx];
        soc_reg_field_set(unit, VFP_SLICE_MAPr, &map_entry, field, value);

        value = (stage_fc->vmap[_FP_VMAP_DEFAULT][idx]).virtual_group;
        field = virtual_to_group_map[idx];
        soc_reg_field_set(unit, VFP_SLICE_MAPr, &map_entry, field, value);
    }

    rv = WRITE_VFP_SLICE_MAPr(unit, map_entry);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}


#ifdef INCLUDE_L3
/*
 * Function:
 *     _field_trx_policy_set_l3_info
 * Purpose:
 *     Install l3 forwarding policy entry.  
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mem       - (IN) Policy table memory. 
 *     value     - (IN) Egress object id or combined next hop information.
 *     buf       - (IN/OUT) Hw entry buffer to write.
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_field_trx_policy_set_l3_info(int unit, soc_mem_t mem, int value, uint32 *buf)
{
    uint32 flags;         /* L3 forwarding flags           */ 
    int nh_ecmp_id;       /* Next hop/Ecmp group id.       */
    int retval;           /* Operation return value.       */ 
    
    /* Resove next hop /ecmp group id. */
    retval = _bcm_field_policy_set_l3_nh_resolve(unit,  value,
                                                 &flags, &nh_ecmp_id);
    BCM_IF_ERROR_RETURN(retval);

    if (flags & BCM_L3_MULTIPATH) {              
        FP_VVERB(("FP(unit %d) vverb: Install mpath L3 policy (Ecmp_group: %d)))", 
                  unit, nh_ecmp_id));
        PolicySet(unit, mem, buf, ECMPf, 1);
        PolicySet(unit, mem, buf, ECMP_PTRf, nh_ecmp_id);
    } else {
        FP_VVERB(("FP(unit %d) vverb: Install unipath L3 policy(Next hop id: %d)))", 
                  unit, nh_ecmp_id));
        PolicySet(unit, mem, buf, ECMPf, 0);
        PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, nh_ecmp_id);
    }
    return (BCM_E_NONE);
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *     _field_trx_action_copy_to_cpu
 * Purpose:
 *     Install copy to cpu action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Field entry structure to get policy info from
 *     fa       - (IN  Field action 
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_action_copy_to_cpu(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                              _field_action_t *fa, uint32 *buf)
{
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionTimeStampToCpu:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x5);
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x5);
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x5);
          PolicySet(unit, mem, buf, R_DROPf, 0x1);
          PolicySet(unit, mem, buf, Y_DROPf, 0x1);
          PolicySet(unit, mem, buf, G_DROPf, 0x1);
          break;
      case bcmFieldActionRpTimeStampToCpu:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x5);
          PolicySet(unit, mem, buf, R_DROPf, 0x1);
          break;
      case bcmFieldActionYpTimeStampToCpu:
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x5);
          PolicySet(unit, mem, buf, Y_DROPf, 0x1);
          break;
      case bcmFieldActionGpTimeStampToCpu:
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x5);
          PolicySet(unit, mem, buf, G_DROPf, 0x1);
          break;
      case bcmFieldActionCopyToCpu:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          } else {
              PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x1);
              PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x1);
              PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x1);
          }
          break;
      case bcmFieldActionRpCopyToCpu:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x1);
          break;
      case bcmFieldActionYpCopyToCpu:
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x1);
          break;
      case bcmFieldActionGpCopyToCpu:
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x1);
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    if (fa->param[0] != 0) {
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, VFP_MATCHED_RULEf, fa->param[1]);
        } else {
            PolicySet(unit, mem, buf, MATCHED_RULEf, fa->param[1]);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_trx_action_copy_to_cpu_cancel
 * Purpose:
 *     Override copy to cpu action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Field entry structure to get policy info from
 *     fa       - (IN  Field action 
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_action_copy_to_cpu_cancel(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                                     _field_action_t *fa, uint32 *buf)
{
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionTimeStampToCpuCancel:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
          PolicySet(unit, mem, buf, R_DROPf, 0x2);
          PolicySet(unit, mem, buf, Y_DROPf, 0x2);
          PolicySet(unit, mem, buf, G_DROPf, 0x2);
          break;
      case bcmFieldActionRpTimeStampToCpuCancel:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
          PolicySet(unit, mem, buf, R_DROPf, 0x2);
          break;
      case bcmFieldActionYpTimeStampToCpuCancel:
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
          PolicySet(unit, mem, buf, Y_DROPf, 0x2);
          break;
      case bcmFieldActionGpTimeStampToCpuCancel:
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
          PolicySet(unit, mem, buf, G_DROPf, 0x2);
          break;
      case bcmFieldActionCopyToCpuCancel:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x2);
          } else {
              PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
              PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
              PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
          }
          break;
      case bcmFieldActionRpCopyToCpuCancel:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x2);
          break;
      case bcmFieldActionYpCopyToCpuCancel:
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x2);
          break;
      case bcmFieldActionGpCopyToCpuCancel:
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x2);
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_trx_action_ecn_update
 * Purpose:
 *     Install ECN bits int IP header TOS field update action. 
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory. 
 *     f_ent    - (IN) Entry structure.
 *     fa       - (IN) Field action. 
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_action_ecn_update (int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                                  _field_action_t *fa, uint32 *buf)
{
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    /* ECN value sanity check. */
    switch (fa->param[0]) {
      case 0x1:
      case 0x2:
      case 0x3:
          break;
      case 0:
          return (BCM_E_UNAVAIL);
      default:
          return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionEcnNew:
          PolicySet(unit, mem, buf, R_CHANGE_ECNf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_ECNf, fa->param[0]);
          PolicySet(unit, mem, buf, G_CHANGE_ECNf, fa->param[0]);
          break;
      case bcmFieldActionRpEcnNew:
          PolicySet(unit, mem, buf, R_CHANGE_ECNf, fa->param[0]);
          break;
      case bcmFieldActionYpEcnNew:
          PolicySet(unit, mem, buf, Y_CHANGE_ECNf, fa->param[0]);
          break;
      case bcmFieldActionGpEcnNew:
          PolicySet(unit, mem, buf, G_CHANGE_ECNf, fa->param[0]);
          break;
      default:
          return (BCM_E_PARAM);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_stat_action_set
 * Purpose:
 *     Install counter update action into policy table.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - entry structure to get policy info from
 *     mem      - Policy table memory
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_stat_action_set(int unit, _field_entry_t *f_ent, 
                               soc_mem_t mem, uint32 *buf)
{
    _field_stat_t  *f_st;  /* Field statistics descriptor. */  
    int mode;              /* Counter hw mode.             */
    int idx;               /* Counter index.               */
    int rv;                /* Opear return status.         */

    if (NULL == f_ent || NULL == buf) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    f_st = NULL;

    /* VFP doesn't have counters. */
    if ((_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) && 
        (0 == soc_feature(unit, soc_feature_field_vfp_flex_counter))) {
        return (BCM_E_NONE);
    }

    if ((0 == (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_STAT_IN_SECONDARY_SLICE))) {
        /* Disable counting if counter was not attached to the entry. */
        idx = 0;
        mode = 0;
    } else {
        /* Get statistics entity description structure. */
        rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st);
        BCM_IF_ERROR_RETURN(rv);
        idx = f_st->hw_index;
        mode = f_st->hw_mode;

        /* Adjust counter hw mode for COUNTER_MODE_YES_NO/NO_YES */
        if (f_ent->statistic.flags & _FP_ENTRY_STAT_USE_ODD) {
            mode++;
        }

    }

    /* Write policy table counter config. */
    if (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id) {
        if (NULL != f_st) {
            f_st->pool_index = f_ent->fs->slice_number;
            f_st->hw_index = f_ent->slice_idx;
        }
        PolicySet(unit, mem, buf, EXT_COUNTER_MODEf, mode);
    } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
        PolicySet(unit, mem, buf, PID_COUNTER_MODEf, mode);
        PolicySet(unit, mem, buf, PID_COUNTER_INDEXf, idx);
    } else if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
        PolicySet(unit, mem, buf, USE_VINTF_CTR_IDXf, mode);
        PolicySet(unit, mem, buf, VINTF_CTR_IDXf, idx);
    } else { 
        PolicySet(unit, mem, buf, COUNTER_MODEf, mode);
        PolicySet(unit, mem, buf, COUNTER_INDEXf, idx);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_trx_redirect_profile_get
 * Purpose:
 *     Get the redirect profile for the unit
 * Parameters:
 *     unit             - BCM device number
 *     redirect_profile - (OUT) redirect profile
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For triumph, External TCAM and IFP refer to same IFP_REDIRECTION_PROFILEm
 */
STATIC int
_field_trx_redirect_profile_get(int unit, soc_profile_mem_t **redirect_profile)
{
    _field_stage_t *stage_fc; 

    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc));

    *redirect_profile = &stage_fc->redirect_profile;

    return (BCM_E_NONE); 
}

/*
 * Function:
 *     _bcm_field_trx_redirect_profile_ref_count_get
 * Purpose:
 *     Get redirect profile entry use count.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index. 
 *     ref_count - (OUT) redirect profile use count. 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_redirect_profile_ref_count_get(int unit, int index, int *ref_count)
{
    soc_profile_mem_t *redirect_profile;

    if (NULL == ref_count) {
        return (BCM_E_PARAM);
    }

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN
        (_field_trx_redirect_profile_get(unit, &redirect_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit, redirect_profile,
                                                      index, ref_count));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_redirect_profile_delete
 * Purpose:
 *     Delete redirect profile entry.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     index     - (IN) Profile entry index. 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_redirect_profile_delete(int unit, int index)
{
    soc_profile_mem_t *redirect_profile;

    /* Get the redirect profile */
    BCM_IF_ERROR_RETURN
        (_field_trx_redirect_profile_get(unit, &redirect_profile));

    BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit, redirect_profile, index));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_redirect_profile_alloc
 * Purpose:
 *     Allocate redirect profile index
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_redirect_profile_alloc(int unit, _field_entry_t *f_ent, 
                                      _field_action_t *fa)
{
    ifp_redirection_profile_entry_t entry_arr[2];
    uint32            *entry_ptr[2];
    soc_profile_mem_t *redirect_profile;
    int               rv;
#ifdef INCLUDE_L3
    bcm_ipmc_addr_t  ipmc;
    int              ipmc_index;
    int              entry_count;
#endif /* INCLUDE_L3 */
    bcm_mcast_addr_t mcaddr;
    bcm_pbmp_t       pbmp;
    void             *entries[1];
    soc_mem_t       profile_mem = IFP_REDIRECTION_PROFILEm;

    entry_ptr[0] = (uint32 *)entry_arr;
    entry_ptr[1] =  entry_ptr[0] + soc_mem_entry_words(unit, profile_mem);
    entries[0] = (void *)&entry_arr;

    if ((NULL == f_ent) || (NULL == fa)) {
        return (BCM_E_PARAM);
    }

    /* Reset redirection profile entry. */
    sal_memcpy(entry_ptr[0], soc_mem_entry_null(unit, profile_mem), 
               soc_mem_entry_words(unit, profile_mem) * sizeof(uint32)); 
    sal_memcpy(entry_ptr[1], soc_mem_entry_null(unit, profile_mem), 
               soc_mem_entry_words(unit, profile_mem) * sizeof(uint32));

    /* Get the redirect profile */
    rv = _field_trx_redirect_profile_get(unit, &redirect_profile);
    BCM_IF_ERROR_RETURN(rv);

    switch (fa->action) {
      case bcmFieldActionRedirectPbmp:
          SOC_PBMP_WORD_SET(pbmp, 0, fa->param[0]);
          SOC_PBMP_WORD_SET(pbmp, 1, fa->param[1]);
          
          soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf, &pbmp);
          rv = soc_profile_mem_add(unit, redirect_profile, entries,
                                   1, (uint32*) &fa->hw_index);
          BCM_IF_ERROR_RETURN(rv);
          break;
      case bcmFieldActionEgressMask:
          SOC_PBMP_WORD_SET(pbmp, 0, fa->param[0]);
          SOC_PBMP_WORD_SET(pbmp, 1, fa->param[1]);
          
          soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf, &pbmp);
          rv = soc_profile_mem_add(unit, redirect_profile, entries,
                                   1, (uint32*)&fa->hw_index);
          BCM_IF_ERROR_RETURN(rv);
          break;
#ifdef INCLUDE_L3
      case bcmFieldActionRedirectIpmc:
          bcm_ipmc_addr_t_init(&ipmc);
          if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
              if (0 == _BCM_MULTICAST_IS_L3(fa->param[0])) {
                  return (BCM_E_PARAM);
              }
              ipmc_index = _BCM_MULTICAST_ID_GET(fa->param[0]); 
              rv = _bcm_trx_multicast_ipmc_read(unit, ipmc_index, 
                                                &ipmc.l2_pbmp, &ipmc.l3_pbmp);
              BCM_IF_ERROR_RETURN(rv);
          } else {
              ipmc_index = fa->param[0];
              if (SOC_IS_TR_VL(unit)) { 
                  rv = bcm_tr_ipmc_get(unit, fa->param[0], &ipmc);
                  BCM_IF_ERROR_RETURN(rv);
              } else {
                  rv = bcm_fb_er_ipmc_get(unit, fa->param[0], &ipmc);
                  BCM_IF_ERROR_RETURN(rv);
              }
          }
          if (SOC_IS_TR_VL(unit)) { 
              entry_count = 2;
              soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf,
                                     &ipmc.l3_pbmp);
              soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[1], BITMAPf,
                                     &ipmc.l2_pbmp);
          } else {
              entry_count = 1;
              soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], L3_BITMAPf,
                                     &ipmc.l3_pbmp);
              soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf,
                                     &ipmc.l2_pbmp);
          }
          soc_mem_field32_set(unit, profile_mem, entry_ptr[0], MC_INDEXf, ipmc_index);
          /* MTU profile index overlayed on MC_INDEXf. */
          rv = soc_profile_mem_add(unit, redirect_profile,
                                   entries, entry_count, (uint32*)&fa->hw_index);
          BCM_IF_ERROR_RETURN(rv);
          break;
#endif /* INCLUDE_L3 */
      case bcmFieldActionRedirectMcast:
          rv = _bcm_xgs3_mcast_index_port_get(unit, fa->param[0], &mcaddr);
          BCM_IF_ERROR_RETURN(rv);
          soc_mem_pbmp_field_set(unit, profile_mem, entry_ptr[0], BITMAPf,
                                 &mcaddr.pbmp);
          soc_mem_field32_set(unit, profile_mem, entry_ptr[0], MC_INDEXf, fa->param[0]);
          rv = soc_profile_mem_add(unit, redirect_profile, entries,
                                   1, (uint32*)&fa->hw_index);
          BCM_IF_ERROR_RETURN(rv);
          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_action_redirect
 * Purpose:
 *     Install redirect action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     mem      - (IN) Policy table memory.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     fa       - (IN) Field action.
 *     buf      - (OUT) Field Policy table entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_action_redirect(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                               _field_action_t *fa, uint32 *buf)
{
    uint32          redir_field = 0;
    int             shift_val;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionOffloadRedirect:
          PolicySet(unit, mem, buf, HI_PRI_ACTION_CONTROLf, 0x1);
          PolicySet(unit, mem, buf, HI_PRI_RESOLVEf, 0x1);
          PolicySet(unit, mem, buf, SUPPRESS_COLOR_SENSITIVE_ACTIONSf, 0x1);
          PolicySet(unit, mem, buf, DEFER_QOS_MARKINGSf, 0x1);
          PolicySet(unit, mem, buf, SUPPRESS_SW_ACTIONSf, 0x1);
          PolicySet(unit, mem, buf, SUPPRESS_VXLTf, 0x1);
          /* fall through to programm redirection port. */
      case bcmFieldActionRedirect: /* param0 = modid, param1 = port*/
          redir_field = ((fa->param[0] & 0x7f) << 6);
          redir_field |= (fa->param[1] & 0x3f);
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x1);
          PolicySet(unit, mem, buf, REDIRECTIONf, redir_field);
          break;
      case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
          shift_val = SOC_IS_TR_VL(unit) ?  7 : 8;
          redir_field |= (0x40 << shift_val);  /* Trunk indicator. */
          redir_field |= fa->param[0];
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x1);
          PolicySet(unit, mem, buf, REDIRECTIONf, redir_field);
          break;
      case bcmFieldActionRedirectCancel:
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 2);
          break;
      case bcmFieldActionRedirectPbmp:
          PolicySet(unit, mem, buf, REDIRECTIONf, fa->hw_index);
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
          break;
      case bcmFieldActionEgressMask:
          PolicySet(unit, mem, buf, REDIRECTIONf, fa->hw_index);
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x4);
          break;
#ifdef INCLUDE_L3
      case bcmFieldActionRedirectIpmc:
          if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
              if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
                  if (0 == _BCM_MULTICAST_IS_L3(fa->param[0])) {
                      return (BCM_E_PARAM);
                  }
                  redir_field = _BCM_MULTICAST_ID_GET(fa->param[0]);
              } else {
                  redir_field = fa->param[0];
              }
          } else {
              redir_field = fa->hw_index;
          }
          /* Assign IPMC action to redirect profile index. */
          shift_val = SOC_IS_TR_VL(unit) ?  12 : 13;
          PolicySet(unit, mem, buf, REDIRECTIONf, 
                         (redir_field | (3 << shift_val)));
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
          break;
#endif /* INCLUDE_L3 */
      case bcmFieldActionRedirectMcast:
          if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
              if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
                  if (0 == _BCM_MULTICAST_IS_L2(fa->param[0])) {
                      return (BCM_E_PARAM);
                  }
                  redir_field = _BCM_MULTICAST_ID_GET(fa->param[0]);
              } else {
                  redir_field = fa->param[0];
              }
          } else {
              redir_field = fa->hw_index;
          }
          /* Assign MCAST action to redirect profile index. */
          shift_val = SOC_IS_TR_VL(unit) ?  12 : 13;
          PolicySet(unit, mem, buf, REDIRECTIONf, 
                         (redir_field | (2 << shift_val)));
          PolicySet(unit, mem, buf, G_PACKET_REDIRECTIONf, 0x3);
          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_action_params_check(int unit,_field_entry_t *f_ent, 
                                   _field_action_t *fa)
{
    uint32   redir_field = 0;
    int      shift_val;
#if defined(INCLUDE_L3)
    uint32 flags;            /* L3 forwarding flags.    */ 
    int nh_ecmp_id;          /* Next hop/Ecmp group id. */
#endif /* INCLUDE_L3 */
    soc_mem_t mem;           /* Policy table memory id. */
    soc_mem_t tcam_mem;      /* Tcam memory id.         */
    int rv;                  /* Operation return value. */ 

    if (NULL == f_ent || NULL == fa) {
        return (BCM_E_PARAM);
    }

    /* Resolve policy memory id. */
    rv = _field_fb_tcam_policy_mem_get(unit, f_ent->group->stage_id, 
                                       &tcam_mem, &mem);
    BCM_IF_ERROR_RETURN(rv);


    switch (fa->action) {
      case bcmFieldActionMultipathHash:
          PolicyCheck(unit, mem, ECMP_HASH_SELf, fa->param[0]);
          break;
      case bcmFieldActionCopyToCpu:
      case bcmFieldActionRpCopyToCpu:
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionGpCopyToCpu:
      case bcmFieldActionTimeStampToCpu:
      case bcmFieldActionRpTimeStampToCpu:
      case bcmFieldActionYpTimeStampToCpu:
      case bcmFieldActionGpTimeStampToCpu:
          if (fa->param[0] != 0) {
              if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                  PolicyCheck(unit, mem, VFP_MATCHED_RULEf, fa->param[1]);
              } else {
                  PolicyCheck(unit, mem, MATCHED_RULEf, fa->param[1]);
              }
          }
          break;
      case bcmFieldActionClassDestSet:
          PolicyCheck(unit, mem, VFP_CLASS_ID_Lf, fa->param[0]);
          break;

      case bcmFieldActionClassSourceSet:
          PolicyCheck(unit, mem, VFP_CLASS_ID_Hf, fa->param[0]);
          break;

      case bcmFieldActionVrfSet:
          if (soc_feature(unit, soc_feature_mpls)) {
              PolicyCheck(unit, mem, VFP_VRF_IDf, fa->param[0]); 
          }  else {
              PolicyCheck(unit, mem, VFP_VRF_IDf, fa->param[0]); 

          }
          break;

      case bcmFieldActionDropPrecedence:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_CNGf, fa->param[0]);
          } else {
              PolicyCheck(unit, mem, R_DROP_PRECEDENCEf, fa->param[0]);
              PolicyCheck(unit, mem, Y_DROP_PRECEDENCEf, fa->param[0]);
              PolicyCheck(unit, mem, G_DROP_PRECEDENCEf, fa->param[0]);
          }
          break;
      case bcmFieldActionPrioPktNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_PKT_PRIORITYf, fa->param[0]);
          } else {
              PolicyCheck(unit, mem, R_NEW_PKT_PRIf, fa->param[0]);
              PolicyCheck(unit, mem, Y_NEW_PKT_PRIf, fa->param[0]);
              PolicyCheck(unit, mem, G_NEW_PKT_PRIf, fa->param[0]);
          }
          break;
      case bcmFieldActionEcnNew:
          if ((fa->param[0] <= 0) || (fa->param[0] > 3)) {
              return (BCM_E_PARAM);
          }
          break;

      case bcmFieldActionDscpNew:
          PolicyCheck(unit, mem, R_NEW_DSCPf, fa->param[0]);
          PolicyCheck(unit, mem, Y_NEW_DSCPf, fa->param[0]);
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, G_NEW_DSCPf, fa->param[0]);
          } else {
              PolicyCheck(unit, mem, G_NEW_DSCP_TOSf, fa->param[0]);
          }
          break;

      case bcmFieldActionCosQNew:
          PolicyCheck(unit, mem, R_COS_INT_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, Y_COS_INT_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, G_COS_INT_PRIf, fa->param[0]);
          break;

      case bcmFieldActionVlanCosQNew:
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicyCheck(unit, mem, R_COS_INT_PRIf, fa->param[0] + 8);
          PolicyCheck(unit, mem, Y_COS_INT_PRIf, fa->param[0] + 8);
          PolicyCheck(unit, mem, G_COS_INT_PRIf, fa->param[0] + 8);
          break;

      case bcmFieldActionPrioIntNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_INT_PRIORITYf,  fa->param[0]);
          } else {
              PolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & fa->param[0]));
          }
          break;

      case bcmFieldActionPrioPktAndIntNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_PKT_PRIORITYf, fa->param[0]);
              PolicyCheck(unit, mem, NEW_INT_PRIORITYf,  fa->param[0]);
          } else {
              PolicyCheck(unit, mem, R_NEW_PKT_PRIf, fa->param[0]);
              PolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicyCheck(unit, mem, Y_NEW_PKT_PRIf, fa->param[0]);
              PolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicyCheck(unit, mem, G_NEW_PKT_PRIf, fa->param[0]);
              PolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & fa->param[0]));
          }
          break;

      case bcmFieldActionCosQCpuNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, CPU_COSf, fa->param[0]);
          } else { 
              PolicyCheck(unit, mem, CPU_COSf, fa->param[0]);
          } 
          break;

      case bcmFieldActionOuterVlanPrioNew:
          PolicyCheck(unit, mem, R_NEW_DOT1Pf, fa->param[0]);
          PolicyCheck(unit, mem, Y_NEW_DOT1Pf, fa->param[0]);
          PolicyCheck(unit, mem, G_NEW_DOT1Pf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanPrioNew:
          PolicyCheck(unit, mem, R_NEW_INNER_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, Y_NEW_INNER_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, G_NEW_INNER_PRIf, fa->param[0]);
          break;

      case bcmFieldActionOuterVlanCfiNew:
          PolicyCheck(unit, mem, R_NEW_OUTER_CFIf, fa->param[0]);
          PolicyCheck(unit, mem, Y_NEW_OUTER_CFIf, fa->param[0]);
          PolicyCheck(unit, mem, G_NEW_OUTER_CFIf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanCfiNew:
          PolicyCheck(unit, mem, R_NEW_INNER_CFIf, fa->param[0]);
          PolicyCheck(unit, mem, Y_NEW_INNER_CFIf, fa->param[0]);
          PolicyCheck(unit, mem, G_NEW_INNER_CFIf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanAdd:
          PolicyCheck(unit, mem, NEW_INNER_VLANf, fa->param[0]);
          break;

      case bcmFieldActionOuterVlanAdd:
          PolicyCheck(unit, mem, NEW_OUTER_VLANf, fa->param[0]);
          break;

      case bcmFieldActionOuterVlanLookup:
          PolicyCheck(unit, mem, NEW_OUTER_VLANf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_INNER_VLANf, fa->param[0]);
          } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, PID_NEW_INNER_VIDf, fa->param[0]);
          }
          break;

      case bcmFieldActionOuterVlanNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_OUTER_VLANf, fa->param[0]);
          } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, PID_NEW_OUTER_VIDf, fa->param[0]);
          } 
          break;

      case bcmFieldActionMirrorIngress: 
      case bcmFieldActionMirrorEgress:  
          rv = _bcm_field_action_dest_check(unit, fa);
          BCM_IF_ERROR_RETURN(rv);
          break;
      case bcmFieldActionOffloadRedirect:
      case bcmFieldActionRedirect: 
          rv = _bcm_field_action_dest_check(unit, fa);
          BCM_IF_ERROR_RETURN(rv);
          redir_field = ((fa->param[0] & 0x7f) << 6);
          redir_field |= (fa->param[1] & 0x3f);
          PolicyCheck(unit, mem, REDIRECTIONf, redir_field);
          break;
      case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
          shift_val = SOC_IS_TR_VL(unit) ?  7 : 8;
          redir_field |= (0x40 << shift_val);  /* Trunk indicator. */
          redir_field |= fa->param[0];
          PolicyCheck(unit, mem, REDIRECTIONf, redir_field);
          break;
#ifdef INCLUDE_L3
      case bcmFieldActionRedirectIpmc:
          if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
              if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
                  if (0 == _BCM_MULTICAST_IS_L3(fa->param[0])) {
                      return (BCM_E_PARAM);
                  }
                  redir_field = _BCM_MULTICAST_ID_GET(fa->param[0]);
              } else {
                  redir_field = fa->param[0];
              }
          } else {
              redir_field = 0;
          }
          /* Assign IPMC action to redirect profile index. */
          shift_val = SOC_IS_TR_VL(unit) ?  12 : 13;
          PolicyCheck(unit, mem, REDIRECTIONf, 
                         (redir_field | (3 << shift_val)));
          break;
#endif /* INCLUDE_L3 */
      case bcmFieldActionRedirectMcast:
          if (soc_feature(unit, soc_feature_field_action_redirect_ipmc)) {
              if (_BCM_MULTICAST_IS_SET(fa->param[0])) {
                  if (0 == _BCM_MULTICAST_IS_L2(fa->param[0])) {
                      return (BCM_E_PARAM);
                  }
                  redir_field = _BCM_MULTICAST_ID_GET(fa->param[0]);
              } else {
                  redir_field = fa->param[0];
              }
          } else {
              redir_field = 0;
          }
          /* Assign MCAST action to redirect profile index. */
          shift_val = SOC_IS_TR_VL(unit) ?  12 : 13;
          PolicyCheck(unit, mem, REDIRECTIONf, 
                         (redir_field | (2 << shift_val)));
          break;
      case bcmFieldActionOffloadClassSet:
          PolicyCheck(unit, mem, PPD3_CLASS_TAGf, fa->param[0]);
          break;
      case bcmFieldActionRpDropPrecedence:
          PolicyCheck(unit, mem, R_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionRpPrioPktNew:
          PolicyCheck(unit, mem, R_NEW_PKT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionRpEcnNew:
          if ((fa->param[0] <= 0) || (fa->param[0] > 3)) {
              return (BCM_E_PARAM);
          }
          break;
      case bcmFieldActionRpDscpNew:
          PolicyCheck(unit, mem, R_NEW_DSCPf, fa->param[0]);
          break;
      case bcmFieldActionRpCosQNew:
          PolicyCheck(unit, mem, R_COS_INT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionRpVlanCosQNew:
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicyCheck(unit, mem, R_COS_INT_PRIf, fa->param[0] + 8);
          break;
      case bcmFieldActionRpPrioPktAndIntNew:
          PolicyCheck(unit, mem, R_NEW_PKT_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionRpOuterVlanPrioNew:
          PolicyCheck(unit, mem, R_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionRpInnerVlanPrioNew:
          PolicyCheck(unit, mem, R_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionRpOuterVlanCfiNew:
          PolicyCheck(unit, mem, R_NEW_OUTER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionRpInnerVlanCfiNew:
          PolicyCheck(unit, mem, R_NEW_INNER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionRpPrioIntNew:
          PolicyCheck(unit, mem, R_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionYpDropPrecedence:
          PolicyCheck(unit, mem, Y_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionYpPrioPktNew:
          PolicyCheck(unit, mem, Y_NEW_PKT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionYpEcnNew:
          if ((fa->param[0] <= 0) || (fa->param[0] > 3)) {
              return (BCM_E_PARAM);
          }
          break;
      case bcmFieldActionYpDscpNew:
          PolicyCheck(unit, mem, Y_NEW_DSCPf, fa->param[0]);
          break;
      case bcmFieldActionYpCosQNew:
          PolicyCheck(unit, mem, Y_COS_INT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionYpVlanCosQNew:
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicyCheck(unit, mem, Y_COS_INT_PRIf, fa->param[0] + 8);
          break;
      case bcmFieldActionYpPrioPktAndIntNew:
          PolicyCheck(unit, mem, Y_NEW_PKT_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionYpOuterVlanPrioNew:
          PolicyCheck(unit, mem, Y_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionYpInnerVlanPrioNew:
          PolicyCheck(unit, mem, Y_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionYpOuterVlanCfiNew:
          PolicyCheck(unit, mem, Y_NEW_OUTER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionYpInnerVlanCfiNew:
          PolicyCheck(unit, mem, Y_NEW_INNER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionYpPrioIntNew:
          PolicyCheck(unit, mem, Y_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionGpDropPrecedence:
          PolicyCheck(unit, mem, G_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionGpPrioPktNew:
          PolicyCheck(unit, mem, G_NEW_PKT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionGpEcnNew:
          if ((fa->param[0] <= 0) || (fa->param[0] > 3)) {
              return (BCM_E_PARAM);
          }
          break;
      case bcmFieldActionGpDscpNew:
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, G_NEW_DSCPf, fa->param[0]);
          } else {
              PolicyCheck(unit, mem, G_NEW_DSCP_TOSf, fa->param[0]);
          }
          break;
      case bcmFieldActionGpCosQNew:
          PolicyCheck(unit, mem, G_COS_INT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionGpVlanCosQNew:
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicyCheck(unit, mem, G_COS_INT_PRIf, fa->param[0] + 8);
          break;
      case bcmFieldActionGpPrioPktAndIntNew:
          PolicyCheck(unit, mem, G_NEW_PKT_PRIf, fa->param[0]);
          PolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionGpOuterVlanPrioNew:
          PolicyCheck(unit, mem, G_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionGpInnerVlanPrioNew:
          PolicyCheck(unit, mem, G_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionGpOuterVlanCfiNew:
          PolicyCheck(unit, mem, G_NEW_OUTER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionGpInnerVlanCfiNew:
          PolicyCheck(unit, mem, G_NEW_INNER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionGpPrioIntNew:
          PolicyCheck(unit, mem, G_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionAddClassTag:
          PolicyCheck(unit, mem, NEXT_HOP_INDEXf, fa->param[0]);
          break;


#ifdef INCLUDE_L3 
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
      case bcmFieldActionIncomingMplsPortSet:
          if (!BCM_GPORT_IS_MPLS_PORT(fa->param[0])) {
              return BCM_E_PARAM;
          }
#endif
          break;
      case bcmFieldActionL3ChangeVlan:
      case bcmFieldActionL3ChangeMacDa:
      case bcmFieldActionL3Switch:
          rv = _bcm_field_policy_set_l3_nh_resolve(unit,  fa->param[0],
                                                   &flags, &nh_ecmp_id);
          BCM_IF_ERROR_RETURN(rv);
          break;
#endif /* INCLUDE_L3 */

#if defined(BCM_ENDURO_SUPPORT)
      case bcmFieldActionOamUpMep:
          PolicyCheck(unit, mem, OAM_UP_MEPf, fa->param[0]);
          break;
      case bcmFieldActionOamTx:
          PolicyCheck(unit, mem, OAM_TXf, fa->param[0]);
          break;
      case bcmFieldActionOamLmepMdl:
          PolicyCheck(unit, mem, OAM_LMEP_MDLf, fa->param[0]);
          break;
      case bcmFieldActionOamServicePriMappingPtr:
          PolicyCheck(unit, mem, OAM_SERVICE_PRI_MAPPING_PTRf, fa->param[0]);
          break;
      case bcmFieldActionOamLmBasePtr:
          PolicyCheck(unit, mem, OAM_LM_BASE_PTRf, fa->param[0]);
          break;
      case bcmFieldActionOamDmEnable:
          PolicyCheck(unit, mem, OAM_DM_ENf, fa->param[0]);
          break;
      case bcmFieldActionOamLmEnable:
          PolicyCheck(unit, mem, OAM_LM_ENf, fa->param[0]);
          break;
      case bcmFieldActionOamLmepEnable:
          PolicyCheck(unit, mem, OAM_LMEP_ENf, fa->param[0]);
          break;
      case bcmFieldActionOamPbbteLookupEnable: 
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, OAM_PBBTE_LOOKUP_ENABLEf, fa->param[0]);
          }
          break;
#endif /* BCM_ENDURO_SUPPORT */
      default:
          return BCM_E_NONE;
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_trx_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory
 *     f_ent    - entry structure to get policy info from
 *     tcam_idx - index into TCAM
 *     fa       - field action 
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_trx_action_get(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                          int tcam_idx, _field_action_t *fa, uint32 *buf)
{
    uint32    mode;
    int       rv;
#if defined(INCLUDE_L3) && defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
    int        svp;
#endif

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionMultipathHash:
          PolicySet(unit, mem, buf, ECMP_HASH_SELf, fa->param[0]);
          break;
      case bcmFieldActionCopyToCpu:
      case bcmFieldActionRpCopyToCpu:
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionGpCopyToCpu:
      case bcmFieldActionTimeStampToCpu:
      case bcmFieldActionRpTimeStampToCpu:
      case bcmFieldActionYpTimeStampToCpu:
      case bcmFieldActionGpTimeStampToCpu:
          rv = _field_trx_action_copy_to_cpu(unit, mem, f_ent, fa, buf);
          BCM_IF_ERROR_RETURN(rv);
          break;

      case bcmFieldActionCopyToCpuCancel:
      case bcmFieldActionRpCopyToCpuCancel:
      case bcmFieldActionYpCopyToCpuCancel:
      case bcmFieldActionGpCopyToCpuCancel:
      case bcmFieldActionTimeStampToCpuCancel:
      case bcmFieldActionRpTimeStampToCpuCancel:
      case bcmFieldActionYpTimeStampToCpuCancel:
      case bcmFieldActionGpTimeStampToCpuCancel:
          rv = _field_trx_action_copy_to_cpu_cancel(unit, mem, f_ent, fa, buf);
          BCM_IF_ERROR_RETURN(rv);
          break;

      case bcmFieldActionDrop:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, DROPf, 0x1);
          } else {
              PolicySet(unit, mem, buf, R_DROPf, 0x1);
              PolicySet(unit, mem, buf, Y_DROPf, 0x1);
              PolicySet(unit, mem, buf, G_DROPf, 0x1);
          }
          break;

      case bcmFieldActionDropCancel:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, DROPf, 0x2);
          } else {
              PolicySet(unit, mem, buf, R_DROPf, 0x2);
              PolicySet(unit, mem, buf, Y_DROPf, 0x2);
              PolicySet(unit, mem, buf, G_DROPf, 0x2);
          }
          break;

      case bcmFieldActionClassDestSet:
          PolicySet(unit, mem, buf, USE_VFP_CLASS_ID_Lf, 0x1);
          PolicySet(unit, mem, buf, VFP_CLASS_ID_Lf, fa->param[0]);
          break;

      case bcmFieldActionClassSourceSet:
          PolicySet(unit, mem, buf, USE_VFP_CLASS_ID_Hf, 0x1);
          PolicySet(unit, mem, buf, VFP_CLASS_ID_Hf, fa->param[0]);
          break;

      case bcmFieldActionVrfSet:
          if (soc_feature(unit, soc_feature_mpls)) {
              PolicySet(unit, mem, buf, MPLS_ACTIONf, 0x3);
              PolicySet(unit, mem, buf, VFP_VRF_IDf, fa->param[0]); 
          }  else {
              PolicySet(unit, mem, buf, USE_VFP_VRF_IDf , 0x1);
              PolicySet(unit, mem, buf, VFP_VRF_IDf, fa->param[0]); 
              
          }
          break;

      case bcmFieldActionDropPrecedence:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, CHANGE_CNGf, 0x1);
              PolicySet(unit, mem, buf, NEW_CNGf, fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, R_DROP_PRECEDENCEf, fa->param[0]);
              PolicySet(unit, mem, buf, Y_DROP_PRECEDENCEf, fa->param[0]);
              PolicySet(unit, mem, buf, G_DROP_PRECEDENCEf, fa->param[0]);
          }
          break;

      case bcmFieldActionPrioPktCopy:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
          break;

      case bcmFieldActionPrioPktNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, CHANGE_PKT_PRIORITYf, 0x1);
              PolicySet(unit, mem, buf, NEW_PKT_PRIORITYf, fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
              PolicySet(unit, mem, buf, R_NEW_PKT_PRIf, fa->param[0]);
              PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
              PolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, fa->param[0]);
              PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
              PolicySet(unit, mem, buf, G_NEW_PKT_PRIf, fa->param[0]);
          }
          break;

      case bcmFieldActionPrioPktTos:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
          break;

      case bcmFieldActionPrioPktCancel:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
          break;

      case bcmFieldActionUpdateCounter:
      case bcmFieldActionMeterConfig:
          /*This action is handled by the calling routine, not by this routine.*/
          return (BCM_E_INTERNAL);
      case bcmFieldActionEcnNew:
          BCM_IF_ERROR_RETURN
              (_field_trx_action_ecn_update (unit, mem, f_ent, fa, buf));
          break;

      case bcmFieldActionDscpNew:
          PolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_DSCPf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_DSCPf, fa->param[0]);
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, G_CHANGE_DSCPf, 0x1);
              PolicySet(unit, mem, buf, G_NEW_DSCPf, fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x3);
              PolicySet(unit, mem, buf, G_NEW_DSCP_TOSf, fa->param[0]);
          }
          break;

      case bcmFieldActionDscpCancel:
          PolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x2);
          PolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x2);
          PolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x4);
          break;

      case bcmFieldActionCosQNew:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x1);
          PolicySet(unit, mem, buf, R_COS_INT_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x1);
          PolicySet(unit, mem, buf, Y_COS_INT_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x1);
          PolicySet(unit, mem, buf, G_COS_INT_PRIf, fa->param[0]);
          break;

      case bcmFieldActionVlanCosQNew:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x2);
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicySet(unit, mem, buf, R_COS_INT_PRIf, fa->param[0] + 8);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x2);
          PolicySet(unit, mem, buf, Y_COS_INT_PRIf, fa->param[0] + 8);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x2);
          PolicySet(unit, mem, buf, G_COS_INT_PRIf, fa->param[0] + 8);
          break;

      case bcmFieldActionPrioIntCopy:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;

      case bcmFieldActionPrioIntNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, CHANGE_INT_PRIORITYf, 0x1);
              PolicySet(unit, mem, buf, NEW_INT_PRIORITYf,  fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
              PolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
              PolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
              PolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & fa->param[0]));
          }
          break;

      case bcmFieldActionPrioIntTos:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;

      case bcmFieldActionPrioIntCancel:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;

      case bcmFieldActionPrioPktAndIntNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, CHANGE_PKT_PRIORITYf, 0x1);
              PolicySet(unit, mem, buf, NEW_PKT_PRIORITYf, fa->param[0]);
              PolicySet(unit, mem, buf, CHANGE_INT_PRIORITYf, 0x1);
              PolicySet(unit, mem, buf, NEW_INT_PRIORITYf,  fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
              PolicySet(unit, mem, buf, R_NEW_PKT_PRIf, fa->param[0]);
              PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
              PolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
              PolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, fa->param[0]);
              PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
              PolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & fa->param[0]));
              PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
              PolicySet(unit, mem, buf, G_NEW_PKT_PRIf, fa->param[0]);
              PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
              PolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & fa->param[0]));
          }
          break;

      case bcmFieldActionPrioPktAndIntCopy:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;

      case bcmFieldActionPrioPktAndIntTos:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;

      case bcmFieldActionPrioPktAndIntCancel:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;

      case bcmFieldActionCosQCpuNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, CPU_COSf, fa->param[0]);
          } else { 
              PolicySet(unit, mem, buf, CHANGE_CPU_COSf, 0x1);
              PolicySet(unit, mem, buf, CPU_COSf, fa->param[0]);
          } 
          break;

      case bcmFieldActionOuterVlanPrioNew:
          PolicySet(unit, mem, buf, R_CHANGE_DOT1Pf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_DOT1Pf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_DOT1Pf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_DOT1Pf, fa->param[0]);
          PolicySet(unit, mem, buf, G_CHANGE_DOT1Pf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_DOT1Pf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanPrioNew:
          PolicySet(unit, mem, buf, R_REPLACE_INNER_PRIf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_INNER_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_REPLACE_INNER_PRIf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_INNER_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, G_REPLACE_INNER_PRIf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_INNER_PRIf, fa->param[0]);
          break;

      case bcmFieldActionOuterVlanCfiNew:
          PolicySet(unit, mem, buf, R_CHANGE_OUTER_CFIf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_OUTER_CFIf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_OUTER_CFIf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_OUTER_CFIf, fa->param[0]);
          PolicySet(unit, mem, buf, G_CHANGE_OUTER_CFIf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_OUTER_CFIf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanCfiNew:
          PolicySet(unit, mem, buf, R_CHANGE_INNER_CFIf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_INNER_CFIf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_INNER_CFIf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_INNER_CFIf, fa->param[0]);
          PolicySet(unit, mem, buf, G_CHANGE_INNER_CFIf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_INNER_CFIf, fa->param[0]);
          break;

      case bcmFieldActionSwitchToCpuCancel:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x3);
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x3);
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x3);
          break;

      case bcmFieldActionSwitchToCpuReinstate:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x4);
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x4);
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x4);
          break;

      case bcmFieldActionInnerVlanAdd:
          PolicySet(unit, mem, buf, INNER_VLAN_ACTIONSf, 0x1);
          PolicySet(unit, mem, buf, NEW_INNER_VLANf, fa->param[0]);
          break;

      case bcmFieldActionOuterVlanAdd:
          PolicySet(unit, mem, buf, OUTER_VLAN_ACTIONSf, 0x1);
          PolicySet(unit, mem, buf, NEW_OUTER_VLANf, fa->param[0]);
          break;

      case bcmFieldActionInnerVlanDelete:
          PolicySet(unit, mem, buf, INNER_VLAN_ACTIONSf, 0x3);
          break;

      case bcmFieldActionOuterVlanLookup:
          PolicySet(unit, mem, buf, OUTER_VLAN_ACTIONSf, 0x3);
          PolicySet(unit, mem, buf, NEW_OUTER_VLANf, fa->param[0]);
          break;

      case bcmFieldActionIpFix:
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, PID_IPFIX_ACTIONSf, 0x1);
          } else {
              PolicySet(unit, mem, buf, IPFIX_CONTROLf, 0x1);
          }
          break;

      case bcmFieldActionIpFixCancel:
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, PID_IPFIX_ACTIONSf, 0x2);
          } else {
              PolicySet(unit, mem, buf, IPFIX_CONTROLf, 0x2);
          }
          break;

      case bcmFieldActionDoNotLearn:
          PolicySet(unit, mem, buf, DO_NOT_LEARNf, 0x1);
          break;

      case bcmFieldActionDoNotCheckVlan:
          PolicySet(unit, mem, buf, DISABLE_VLAN_CHECKSf, 0x1);
          break;

      case bcmFieldActionInnerVlanNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, INNER_VLAN_ACTIONSf, 0x2);
              PolicySet(unit, mem, buf, NEW_INNER_VLANf, fa->param[0]);
          } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, PID_REPLACE_INNER_VIDf, 0x1);
              PolicySet(unit, mem, buf, PID_NEW_INNER_VIDf, fa->param[0]);
          }
          break;
      case bcmFieldActionOuterVlanNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, OUTER_VLAN_ACTIONSf, 0x2);
              PolicySet(unit, mem, buf, NEW_OUTER_VLANf, fa->param[0]);
          } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, PID_REPLACE_OUTER_VIDf, 0x1);
              PolicySet(unit, mem, buf, PID_NEW_OUTER_VIDf, fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 0x1);
              PolicySet(unit, mem, buf, ECMPf, 0);
              PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, fa->hw_index);
          } 
          break;
      case bcmFieldActionSrcMacNew:
      case bcmFieldActionDstMacNew:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 0x1);
          PolicySet(unit, mem, buf, ECMPf, 0);
          PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, fa->hw_index);
          break;
      case bcmFieldActionOuterTpidNew:
          BCM_IF_ERROR_RETURN(_bcm_field_tpid_hw_encode(unit, fa->param[0], &mode));
          PolicySet(unit, mem, buf, PID_REPLACE_OUTER_TPIDf, 1);
          PolicySet(unit, mem, buf, PID_OUTER_TPID_INDEXf, mode);
          break;

      case bcmFieldActionMirrorOverride:
          PolicySet(unit, mem, buf, MIRROR_OVERRIDEf, 1);
          break;

      case bcmFieldActionDoNotChangeTtl:
          PolicySet(unit, mem, buf, DO_NOT_CHANGE_TTLf, 1);
          break;

      case bcmFieldActionDoNotCheckUrpf:
          PolicySet(unit, mem, buf, DO_NOT_URPFf, 1);
          break;

      case bcmFieldActionMirrorIngress:    /* param0=modid, param1=port/tgid */
          BCM_IF_ERROR_RETURN
              (_bcm_field_trx_mirror_ingress_add(unit, mem, f_ent, fa, buf));
          break;
      case bcmFieldActionMirrorEgress:     /* param0=modid, param1=port/tgid */
          BCM_IF_ERROR_RETURN
              (_bcm_field_trx_mirror_egress_add(unit, mem, f_ent, fa, buf));
          break;
      case bcmFieldActionOffloadRedirect:
      case bcmFieldActionRedirect:
      case bcmFieldActionRedirectTrunk:
      case bcmFieldActionRedirectCancel:
      case bcmFieldActionRedirectPbmp:
#ifdef INCLUDE_L3
      case bcmFieldActionRedirectIpmc:
#endif /* INCLUDE_L3 */
      case bcmFieldActionRedirectMcast:
      case bcmFieldActionEgressMask:
          BCM_IF_ERROR_RETURN
              (_field_trx_action_redirect(unit, mem, f_ent, fa, buf));
          break;
      case bcmFieldActionOffloadDropIndication:
          PolicySet(unit, mem, buf, SPLIT_DROP_RESOLVEf, 0x1);
          break;
      case bcmFieldActionOffloadClassSet:
          PolicySet(unit, mem, buf, PPD3_CLASS_TAGf, fa->param[0]);
          break;
      case bcmFieldActionRpDrop:
          PolicySet(unit, mem, buf, R_DROPf, 0x1);
          break;
      case bcmFieldActionRpDropCancel:
          PolicySet(unit, mem, buf, R_DROPf, 0x2);
          break;
      case bcmFieldActionRpDropPrecedence:
          PolicySet(unit, mem, buf, R_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionRpSwitchToCpuCancel:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x3);
          break;
      case bcmFieldActionRpSwitchToCpuReinstate:
          PolicySet(unit, mem, buf, R_COPY_TO_CPUf, 0x4);
          break;
      case bcmFieldActionRpPrioPktCopy:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
          break;
      case bcmFieldActionRpPrioPktNew:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
          PolicySet(unit, mem, buf, R_NEW_PKT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionRpPrioPktTos:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
          break;
      case bcmFieldActionRpPrioPktCancel:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
          break;
      case bcmFieldActionRpEcnNew:
          BCM_IF_ERROR_RETURN
              (_field_trx_action_ecn_update (unit, mem, f_ent, fa, buf));
          break;
      case bcmFieldActionRpDscpNew:
          PolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_DSCPf, fa->param[0]);
          break;
      case bcmFieldActionRpDscpCancel:
          PolicySet(unit, mem, buf, R_CHANGE_DSCPf, 0x2);
          break;
      case bcmFieldActionRpCosQNew:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x1);
          PolicySet(unit, mem, buf, R_COS_INT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionRpVlanCosQNew:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x2);
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicySet(unit, mem, buf, R_COS_INT_PRIf, fa->param[0] + 8);
          break;
      case bcmFieldActionRpPrioPktAndIntCopy:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;
      case bcmFieldActionRpPrioPktAndIntNew:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x5);
          PolicySet(unit, mem, buf, R_NEW_PKT_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
          PolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionRpPrioPktAndIntTos:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;
      case bcmFieldActionRpPrioPktAndIntCancel:
          PolicySet(unit, mem, buf, R_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;
      case bcmFieldActionRpOuterVlanPrioNew:
          PolicySet(unit, mem, buf, R_CHANGE_DOT1Pf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionRpInnerVlanPrioNew:
          PolicySet(unit, mem, buf, R_REPLACE_INNER_PRIf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionRpOuterVlanCfiNew:
          PolicySet(unit, mem, buf, R_CHANGE_OUTER_CFIf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_OUTER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionRpInnerVlanCfiNew:
          PolicySet(unit, mem, buf, R_CHANGE_INNER_CFIf, 0x1);
          PolicySet(unit, mem, buf, R_NEW_INNER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionRpPrioIntCopy:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;
      case bcmFieldActionRpPrioIntNew:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x5);
          PolicySet(unit, mem, buf, R_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionRpPrioIntTos:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;
      case bcmFieldActionRpPrioIntCancel:
          PolicySet(unit, mem, buf, R_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;


      case bcmFieldActionYpDrop:
          PolicySet(unit, mem, buf, Y_DROPf, 0x1);
          break;
      case bcmFieldActionYpDropCancel:
          PolicySet(unit, mem, buf, Y_DROPf, 0x2);
          break;
      case bcmFieldActionYpDropPrecedence:
          PolicySet(unit, mem, buf, Y_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionYpSwitchToCpuCancel:
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x3);
          break;
      case bcmFieldActionYpSwitchToCpuReinstate:
          PolicySet(unit, mem, buf, Y_COPY_TO_CPUf, 0x4);
          break;
      case bcmFieldActionYpPrioPktCopy:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
          break;
      case bcmFieldActionYpPrioPktNew:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
          PolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionYpPrioPktTos:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
          break;
      case bcmFieldActionYpPrioPktCancel:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
          break;
      case bcmFieldActionYpEcnNew:
          BCM_IF_ERROR_RETURN
              (_field_trx_action_ecn_update (unit, mem, f_ent, fa, buf));
          break;
      case bcmFieldActionYpDscpNew:
          PolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_DSCPf, fa->param[0]);
          break;
      case bcmFieldActionYpDscpCancel:
          PolicySet(unit, mem, buf, Y_CHANGE_DSCPf, 0x2);
          break;
      case bcmFieldActionYpCosQNew:
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x1);
          PolicySet(unit, mem, buf, Y_COS_INT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionYpVlanCosQNew:
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x2);
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicySet(unit, mem, buf, Y_COS_INT_PRIf, fa->param[0] + 8);
          break;
      case bcmFieldActionYpPrioPktAndIntCopy:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;
      case bcmFieldActionYpPrioPktAndIntNew:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x5);
          PolicySet(unit, mem, buf, Y_NEW_PKT_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
          PolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionYpPrioPktAndIntTos:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;
      case bcmFieldActionYpPrioPktAndIntCancel:
          PolicySet(unit, mem, buf, Y_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;
      case bcmFieldActionYpOuterVlanPrioNew:
          PolicySet(unit, mem, buf, Y_CHANGE_DOT1Pf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionYpInnerVlanPrioNew:
          PolicySet(unit, mem, buf, Y_REPLACE_INNER_PRIf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionYpOuterVlanCfiNew:
          PolicySet(unit, mem, buf, Y_CHANGE_OUTER_CFIf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_OUTER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionYpInnerVlanCfiNew:
          PolicySet(unit, mem, buf, Y_CHANGE_INNER_CFIf, 0x1);
          PolicySet(unit, mem, buf, Y_NEW_INNER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionYpPrioIntCopy:
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;
      case bcmFieldActionYpPrioIntNew:
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x5);
          PolicySet(unit, mem, buf, Y_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionYpPrioIntTos:
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;
      case bcmFieldActionYpPrioIntCancel:
          PolicySet(unit, mem, buf, Y_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;



      case bcmFieldActionGpDrop:
          PolicySet(unit, mem, buf, G_DROPf, 0x1);
          break;
      case bcmFieldActionGpDropCancel:
          PolicySet(unit, mem, buf, G_DROPf, 0x2);
          break;
      case bcmFieldActionGpDropPrecedence:
          PolicySet(unit, mem, buf, G_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionGpSwitchToCpuCancel:
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x3);
          break;
      case bcmFieldActionGpSwitchToCpuReinstate:
          PolicySet(unit, mem, buf, G_COPY_TO_CPUf, 0x4);
          break;
      case bcmFieldActionGpPrioPktCopy:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
          break;
      case bcmFieldActionGpPrioPktNew:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
          PolicySet(unit, mem, buf, G_NEW_PKT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionGpPrioPktTos:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
          break;
      case bcmFieldActionGpPrioPktCancel:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
          break;
      case bcmFieldActionGpEcnNew:
          BCM_IF_ERROR_RETURN
              (_field_trx_action_ecn_update (unit, mem, f_ent, fa, buf));
          break;
      case bcmFieldActionGpDscpNew:
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, G_CHANGE_DSCPf, 1);
              PolicySet(unit, mem, buf, G_NEW_DSCPf, fa->param[0]);
          } else {
              PolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x3);
              PolicySet(unit, mem, buf, G_NEW_DSCP_TOSf, fa->param[0]);
          }
          break;
      case bcmFieldActionGpDscpCancel:
          PolicySet(unit, mem, buf, G_CHANGE_DSCP_TOSf, 0x4);
          break;
      case bcmFieldActionGpCosQNew:
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x1);
          PolicySet(unit, mem, buf, G_COS_INT_PRIf, fa->param[0]);
          break;
      case bcmFieldActionGpVlanCosQNew:
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x2);
          /* Add 8 to the value since VLAN shaping queues are 8..23 */
          PolicySet(unit, mem, buf, G_COS_INT_PRIf, fa->param[0] + 8);
          break;
      case bcmFieldActionGpPrioPktAndIntCopy:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x4);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;
      case bcmFieldActionGpPrioPktAndIntNew:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x5);
          PolicySet(unit, mem, buf, G_NEW_PKT_PRIf, fa->param[0]);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
          PolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionGpPrioPktAndIntTos:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x6);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;
      case bcmFieldActionGpPrioPktAndIntCancel:
          PolicySet(unit, mem, buf, G_CHANGE_PKT_PRIf, 0x7);
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;
      case bcmFieldActionGpOuterVlanPrioNew:
          PolicySet(unit, mem, buf, G_CHANGE_DOT1Pf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionGpInnerVlanPrioNew:
          PolicySet(unit, mem, buf, G_REPLACE_INNER_PRIf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionGpOuterVlanCfiNew:
          PolicySet(unit, mem, buf, G_CHANGE_OUTER_CFIf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_OUTER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionGpInnerVlanCfiNew:
          PolicySet(unit, mem, buf, G_CHANGE_INNER_CFIf, 0x1);
          PolicySet(unit, mem, buf, G_NEW_INNER_CFIf, fa->param[0]);
          break;
      case bcmFieldActionGpPrioIntCopy:
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x4);
          break;
      case bcmFieldActionGpPrioIntNew:
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x5);
          PolicySet(unit, mem, buf, G_COS_INT_PRIf, (0xf & fa->param[0]));
          break;
      case bcmFieldActionGpPrioIntTos:
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x6);
          break;
      case bcmFieldActionGpPrioIntCancel:
          PolicySet(unit, mem, buf, G_CHANGE_COS_OR_INT_PRIf, 0x7);
          break;
      case bcmFieldActionAddClassTag:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 
                    soc_feature(unit, soc_feature_field_action_l2_change) ? \
                    0x4: 0x3);
          PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, fa->param[0]);
          break;
#ifdef INCLUDE_L3 
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT)
      case bcmFieldActionIncomingMplsPortSet:
          if (!BCM_GPORT_IS_MPLS_PORT(fa->param[0])) {
              return BCM_E_PARAM;
          }
          PolicySet(unit, mem, buf, MPLS_ACTIONf, 0x1);
          svp = BCM_GPORT_MPLS_PORT_ID_GET((bcm_gport_t)fa->param[0]);
          PolicySet(unit, mem, buf, SVPf, svp); 
          PolicySet(unit, mem, buf, DISABLE_VLAN_CHECKSf, 1); 
          break;
#endif

      case bcmFieldActionL3ChangeVlan:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 0x1);
          BCM_IF_ERROR_RETURN
              (_field_trx_policy_set_l3_info(unit, mem, fa->param[0], buf)); 
          break;
      case bcmFieldActionL3ChangeVlanCancel:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 0x2);
          break;
      case bcmFieldActionL3ChangeMacDa:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 
                    soc_feature(unit, soc_feature_field_action_l2_change) ? \
                    0x1: 0x4);
          BCM_IF_ERROR_RETURN
              (_field_trx_policy_set_l3_info(unit, mem,  fa->param[0], buf)); 
          break;
      case bcmFieldActionL3ChangeMacDaCancel:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 
                    soc_feature(unit, soc_feature_field_action_l2_change) ? \
                    0x2: 0x5);
          break;
      case bcmFieldActionL3Switch:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 0x6);
          BCM_IF_ERROR_RETURN
              (_field_trx_policy_set_l3_info(unit, mem, fa->param[0], buf)); 
          break;
      case bcmFieldActionL3SwitchCancel:
          PolicySet(unit, mem, buf, G_L3SW_CHANGE_MACDA_OR_VLANf, 0x7);
          break;
#endif /* INCLUDE_L3 */

#if defined(BCM_ENDURO_SUPPORT)
      case bcmFieldActionOamUpMep:
          PolicySet(unit, mem, buf, OAM_UP_MEPf, fa->param[0]);
          break;
      case bcmFieldActionOamTx:
          PolicySet(unit, mem, buf, OAM_TXf, fa->param[0]);
          break;
      case bcmFieldActionOamLmepMdl:
          PolicySet(unit, mem, buf, OAM_LMEP_MDLf, fa->param[0]);
          break;
      case bcmFieldActionOamServicePriMappingPtr:
          PolicySet(unit, mem, buf, OAM_SERVICE_PRI_MAPPING_PTRf, fa->param[0]);
          break;
      case bcmFieldActionOamLmBasePtr:
          PolicySet(unit, mem, buf, OAM_LM_BASE_PTRf, fa->param[0]);
          break;
      case bcmFieldActionOamDmEnable:
          PolicySet(unit, mem, buf, OAM_DM_ENf, fa->param[0]);
          break;
      case bcmFieldActionOamLmEnable:
          PolicySet(unit, mem, buf, OAM_LM_ENf, fa->param[0]);
          break;
      case bcmFieldActionOamLmepEnable:
          PolicySet(unit, mem, buf, OAM_LMEP_ENf, fa->param[0]);
          break;
      case bcmFieldActionOamPbbteLookupEnable: 
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, OAM_PBBTE_LOOKUP_ENABLEf, fa->param[0]);
          }
          break;
#endif /* BCM_ENDURO_SUPPORT */
      default:
          return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_trx2_mirror_add
 * Purpose:
 *     Set mirroring destination & enable mirroring for rule matching 
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Entry structure to get policy info from
 *     fa       - (IN) field action 
 *     buf      - (IN/OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int
_field_trx2_mirror_add(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                       _field_action_t *fa, uint32 *buf)
{
    int mtp_index;
    uint32 enable;
    int index; 

    soc_field_t mtp_field[] = {
        MTP_INDEX0f, MTP_INDEX1f, 
        MTP_INDEX2f, MTP_INDEX3f};

    /* Input parameters check. */
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }
    mtp_index = fa->hw_index & ((1 << BCM_MIRROR_MTP_FLEX_SLOT_SHIFT) - 1);
    index = fa->hw_index >> BCM_MIRROR_MTP_FLEX_SLOT_SHIFT;
    if (index >= COUNTOF(mtp_field)) {
        return (BCM_E_INTERNAL);
    }

    enable = PolicyGet(unit, mem, buf, MIRRORf);

    PolicySet(unit, mem, buf, mtp_field[index], mtp_index);
    PolicySet(unit, mem, buf, MIRRORf, (enable | (0x1 << index)));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_mirror_ingress_add
 * Purpose:
 *     Set ingress mirroring destination & enable mirroring for rule matching 
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Entry structure to get policy info from
 *     fa       - (IN) field action 
 *     buf      - (IN/OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_mirror_ingress_add(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                                  _field_action_t *fa, uint32 *buf)
{
    int mtp_index;
    uint32 enable;

    /* Input parameters check. */
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, MIRRORf)) {
        return _field_trx2_mirror_add(unit, mem, f_ent, fa, buf);
    }
        
    /* Allocate mirror to index. */
    mtp_index = fa->hw_index;
    enable = PolicyGet(unit, mem, buf, INGRESS_MIRRORf);
    /* If destination 0 not in use install new mtp to destination 0. */
     if(0 == (enable & 0x1)) {
        PolicySet(unit, mem, buf, IM0_MTP_INDEXf, mtp_index);
        PolicySet(unit, mem, buf, INGRESS_MIRRORf, (enable | 0x1));

        return (BCM_E_NONE);
    } 
    
    /* If destination 1 not in use install new mtp to destination 1. */
     if(0 == (enable & 0x2)) {
        PolicySet(unit, mem, buf, IM1_MTP_INDEXf, mtp_index);
        PolicySet(unit, mem, buf, INGRESS_MIRRORf, (enable | 0x2));
        return (BCM_E_NONE);
    }
    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *     _bcm_field_trx_mirror_egress_add
 * Purpose:
 *     Set ingress mirroring destination & enable mirroring for rule matching 
 *     packets.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Entry structure to get policy info from
 *     fa       - (IN) field action 
 *     buf      - (IN/OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_mirror_egress_add(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                                 _field_action_t *fa, uint32 *buf)
{
    int mtp_index;
    int enable;

    /* Input parameters check. */
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, MIRRORf)) {
        return _field_trx2_mirror_add(unit, mem, f_ent, fa, buf);
    }

    /* Allocate mirror to index. */
    mtp_index = fa->hw_index;
    enable = PolicyGet(unit, mem, buf, EGRESS_MIRRORf);
    /* If destination 0 not in use install new mtp to destination 0. */
     if(0 == (enable & 0x1)) {
        PolicySet(unit, mem, buf, EM0_MTP_INDEXf, mtp_index);
        PolicySet(unit, mem, buf, EGRESS_MIRRORf, (enable | 0x1));

        return (BCM_E_NONE);
    } 
    
    /* If destination 1 not in use install new mtp to destination 1. */
     if(0 == (enable & 0x2)) {
        PolicySet(unit, mem, buf, EM1_MTP_INDEXf, mtp_index);
        PolicySet(unit, mem, buf, EGRESS_MIRRORf, (enable | 0x2));
        return (BCM_E_NONE);
    }
    return (BCM_E_RESOURCE);
}

/*
 * Function:
 *     _bcm_field_trx_action_support_check
 *
 * Purpose:
 *     Check if action is supported by device.
 *
 * Parameters:
 *     unit   -(IN) BCM device number
 *     f_ent  -(IN) Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     result -(OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX   
 */
int
_bcm_field_trx_action_support_check(int unit, _field_entry_t *f_ent,
                                    bcm_field_action_t action, int *result)
{
    /* Input parameters check */
    if ((NULL == f_ent) || (NULL == result)) {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
        switch (action) {
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
          case bcmFieldActionUpdateCounter:
          case bcmFieldActionInnerVlanNew:
          case bcmFieldActionOuterVlanNew:
          case bcmFieldActionOuterTpidNew:
          case bcmFieldActionOuterVlanPrioNew:
          case bcmFieldActionInnerVlanPrioNew:
          case bcmFieldActionOuterVlanCfiNew:
          case bcmFieldActionInnerVlanCfiNew:
          case bcmFieldActionIpFix:
          case bcmFieldActionIpFixCancel:
          case bcmFieldActionMeterConfig:
          case bcmFieldActionDscpNew:

          case bcmFieldActionRpDrop:
          case bcmFieldActionRpDropCancel:
          case bcmFieldActionRpDscpNew:
          case bcmFieldActionRpOuterVlanPrioNew:
          case bcmFieldActionRpInnerVlanPrioNew:
          case bcmFieldActionRpOuterVlanCfiNew:
          case bcmFieldActionRpInnerVlanCfiNew:

          case bcmFieldActionYpDrop:
          case bcmFieldActionYpDropCancel:
          case bcmFieldActionYpDscpNew:
          case bcmFieldActionYpOuterVlanPrioNew:
          case bcmFieldActionYpInnerVlanPrioNew:
          case bcmFieldActionYpOuterVlanCfiNew:
          case bcmFieldActionYpInnerVlanCfiNew:

          case bcmFieldActionGpDrop:
          case bcmFieldActionGpDropCancel:
          case bcmFieldActionGpDscpNew:
          case bcmFieldActionGpOuterVlanPrioNew:
          case bcmFieldActionGpInnerVlanPrioNew:
          case bcmFieldActionGpOuterVlanCfiNew:
          case bcmFieldActionGpInnerVlanCfiNew:
              *result = TRUE;
              break;
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
          case bcmFieldActionRpCopyToCpu:
          case bcmFieldActionRpCopyToCpuCancel:
          case bcmFieldActionYpCopyToCpu:
          case bcmFieldActionYpCopyToCpuCancel:
          case bcmFieldActionGpCopyToCpu:
          case bcmFieldActionGpCopyToCpuCancel:
              *result = SOC_MEM_FIELD_VALID(unit, EFP_POLICY_TABLEm,
                                            G_COPY_TO_CPUf) ? TRUE : FALSE;
              break;
          default:
              *result = FALSE;
        }
        return (BCM_E_NONE);
    }
    if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
        switch (action) {
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
          case bcmFieldActionPrioPktNew:
          case bcmFieldActionInnerVlanAdd:
          case bcmFieldActionInnerVlanNew:
          case bcmFieldActionInnerVlanDelete:
          case bcmFieldActionOuterVlanAdd:
          case bcmFieldActionOuterVlanNew:
          case bcmFieldActionOuterVlanLookup:
          case bcmFieldActionCosQCpuNew:
          case bcmFieldActionPrioPktAndIntNew:
          case bcmFieldActionPrioIntNew:
          case bcmFieldActionDropPrecedence:
          case bcmFieldActionVrfSet:
          case bcmFieldActionDoNotLearn:
          case bcmFieldActionDoNotCheckVlan:
          case bcmFieldActionClassDestSet:
          case bcmFieldActionClassSourceSet:
              *result = TRUE;
              break;
          case bcmFieldActionIncomingMplsPortSet:
              *result = soc_feature(unit, soc_feature_mpls) ? TRUE : FALSE;
              break;
          case bcmFieldActionOamPbbteLookupEnable:
              *result = SOC_IS_ENDURO(unit) ? TRUE : FALSE;
              break;
          case bcmFieldActionUpdateCounter:
              *result = soc_feature(unit, soc_feature_field_vfp_flex_counter) ? 
                  TRUE : FALSE;
              break;
          default:
              *result = FALSE;
        }
        return (BCM_E_NONE);
    }

    if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) ||
        (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id)) {
        switch (action) {
          case bcmFieldActionAddClassTag:
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
          case bcmFieldActionDropPrecedence:
          case bcmFieldActionUpdateCounter:
          case bcmFieldActionColorIndependent:
          case bcmFieldActionL3ChangeVlan:
          case bcmFieldActionL3ChangeVlanCancel:
          case bcmFieldActionL3ChangeMacDa:
          case bcmFieldActionL3ChangeMacDaCancel:
          case bcmFieldActionL3Switch:
          case bcmFieldActionL3SwitchCancel:
          case bcmFieldActionPrioPktCopy:
          case bcmFieldActionPrioPktNew:
          case bcmFieldActionPrioPktTos:
          case bcmFieldActionPrioPktCancel:
          case bcmFieldActionEcnNew:
          case bcmFieldActionDscpNew:
          case bcmFieldActionDscpCancel:
          case bcmFieldActionCosQNew:
          case bcmFieldActionCosQCpuNew:
          case bcmFieldActionVlanCosQNew:
          case bcmFieldActionMirrorOverride:
          case bcmFieldActionSwitchToCpuCancel:
          case bcmFieldActionSwitchToCpuReinstate:
          case bcmFieldActionRedirect:
          case bcmFieldActionRedirectTrunk:
          case bcmFieldActionRedirectCancel:
          case bcmFieldActionRedirectPbmp:
          case bcmFieldActionRedirectIpmc:
          case bcmFieldActionRedirectMcast:
          case bcmFieldActionEgressMask:
          case bcmFieldActionDoNotChangeTtl:
          case bcmFieldActionDoNotCheckUrpf:
          case bcmFieldActionMeterConfig:
          case bcmFieldActionPrioPktAndIntCopy:
          case bcmFieldActionPrioPktAndIntNew:
          case bcmFieldActionPrioPktAndIntTos:
          case bcmFieldActionPrioPktAndIntCancel:
          case bcmFieldActionPrioIntCopy:
          case bcmFieldActionPrioIntNew:
          case bcmFieldActionPrioIntTos:
          case bcmFieldActionPrioIntCancel:
          case bcmFieldActionMirrorIngress:
          case bcmFieldActionMirrorEgress:

          case bcmFieldActionRpDrop:
          case bcmFieldActionRpDropCancel:
          case bcmFieldActionRpDropPrecedence:
          case bcmFieldActionRpCopyToCpu:
          case bcmFieldActionRpCopyToCpuCancel:
          case bcmFieldActionRpPrioPktCopy:
          case bcmFieldActionRpPrioPktNew:
          case bcmFieldActionRpPrioPktTos:
          case bcmFieldActionRpPrioPktCancel:
          case bcmFieldActionRpDscpNew:
          case bcmFieldActionRpDscpCancel:
          case bcmFieldActionRpEcnNew:
          case bcmFieldActionRpCosQNew:
          case bcmFieldActionRpVlanCosQNew:
          case bcmFieldActionRpPrioPktAndIntCopy:
          case bcmFieldActionRpPrioPktAndIntNew:
          case bcmFieldActionRpPrioPktAndIntTos:
          case bcmFieldActionRpPrioPktAndIntCancel:
          case bcmFieldActionRpSwitchToCpuCancel:
          case bcmFieldActionRpSwitchToCpuReinstate:
          case bcmFieldActionRpPrioIntCopy:
          case bcmFieldActionRpPrioIntNew:
          case bcmFieldActionRpPrioIntTos:
          case bcmFieldActionRpPrioIntCancel:

          case bcmFieldActionYpDrop:
          case bcmFieldActionYpDropCancel:
          case bcmFieldActionYpDropPrecedence:
          case bcmFieldActionYpCopyToCpu:
          case bcmFieldActionYpCopyToCpuCancel:
          case bcmFieldActionYpPrioPktCopy:
          case bcmFieldActionYpPrioPktNew:
          case bcmFieldActionYpPrioPktTos:
          case bcmFieldActionYpPrioPktCancel:
          case bcmFieldActionYpDscpNew:
          case bcmFieldActionYpDscpCancel:
          case bcmFieldActionYpEcnNew:
          case bcmFieldActionYpCosQNew:
          case bcmFieldActionYpVlanCosQNew:
          case bcmFieldActionYpPrioPktAndIntCopy:
          case bcmFieldActionYpPrioPktAndIntNew:
          case bcmFieldActionYpPrioPktAndIntTos:
          case bcmFieldActionYpPrioPktAndIntCancel:
          case bcmFieldActionYpSwitchToCpuCancel:
          case bcmFieldActionYpSwitchToCpuReinstate:
          case bcmFieldActionYpPrioIntCopy:
          case bcmFieldActionYpPrioIntNew:
          case bcmFieldActionYpPrioIntTos:
          case bcmFieldActionYpPrioIntCancel:

          case bcmFieldActionGpDrop:
          case bcmFieldActionGpDropCancel:
          case bcmFieldActionGpDropPrecedence:
          case bcmFieldActionGpCopyToCpu:
          case bcmFieldActionGpCopyToCpuCancel:
          case bcmFieldActionGpPrioPktCopy:
          case bcmFieldActionGpPrioPktNew:
          case bcmFieldActionGpPrioPktTos:
          case bcmFieldActionGpPrioPktCancel:
          case bcmFieldActionGpDscpNew:
          case bcmFieldActionGpDscpCancel:
          case bcmFieldActionGpEcnNew:
          case bcmFieldActionGpCosQNew:
          case bcmFieldActionGpVlanCosQNew:
          case bcmFieldActionGpPrioPktAndIntCopy:
          case bcmFieldActionGpPrioPktAndIntNew:
          case bcmFieldActionGpPrioPktAndIntTos:
          case bcmFieldActionGpPrioPktAndIntCancel:
          case bcmFieldActionGpSwitchToCpuCancel:
          case bcmFieldActionGpSwitchToCpuReinstate:
          case bcmFieldActionGpPrioIntCopy:
          case bcmFieldActionGpPrioIntNew:
          case bcmFieldActionGpPrioIntTos:
          case bcmFieldActionGpPrioIntCancel:
              *result = TRUE;
              break;
          case bcmFieldActionMultipathHash:
              *result = SOC_MEM_FIELD_VALID(unit, FP_POLICY_TABLEm, ECMP_HASH_SELf);
              break;
          case bcmFieldActionIpFix:
          case bcmFieldActionIpFixCancel:
              *result = soc_feature(unit, soc_feature_ipfix) ? TRUE : FALSE;
              break;
          case bcmFieldActionOamUpMep:
          case bcmFieldActionOamTx:
          case bcmFieldActionOamLmepMdl:
          case bcmFieldActionOamServicePriMappingPtr:
          case bcmFieldActionOamLmBasePtr:
          case bcmFieldActionOamDmEnable:
          case bcmFieldActionOamLmEnable:
          case bcmFieldActionOamLmepEnable:
              *result = SOC_IS_ENDURO(unit) ? TRUE : FALSE;
              break;
          case bcmFieldActionTimeStampToCpu:
          case bcmFieldActionRpTimeStampToCpu:
          case bcmFieldActionYpTimeStampToCpu:
          case bcmFieldActionGpTimeStampToCpu:
          case bcmFieldActionTimeStampToCpuCancel:
          case bcmFieldActionRpTimeStampToCpuCancel:
          case bcmFieldActionYpTimeStampToCpuCancel:
          case bcmFieldActionGpTimeStampToCpuCancel:
              *result = soc_feature(unit, soc_feature_field_action_timestamp) ? \
                        TRUE : FALSE; 
          case bcmFieldActionOffloadRedirect:
          case bcmFieldActionOffloadClassSet:
          case bcmFieldActionOffloadDropIndication:
              *result = (SOC_MEM_FIELD_VALID(unit, FP_POLICY_TABLEm, 
                                             HI_PRI_ACTION_CONTROLf) &&
                         (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id));
              break;
          case bcmFieldActionSrcMacNew:
          case bcmFieldActionDstMacNew:
          case bcmFieldActionOuterVlanNew:
#if defined(INCLUDE_L3) 
              *result = (SOC_MEM_FIELD_VALID(unit, FP_POLICY_TABLEm, 
                                             G_L3SW_CHANGE_L2_FIELDSf) &&
                         (_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id));
#else /* INCLUDE_L3 */
              *result = FALSE;
#endif /* INCLUDE_L3 */
              break;
          default:
              *result = FALSE;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_action_conflict_check
 *
 * Purpose:
 *     Check if two action conflict (occupy the same field in FP policy table)
 *
 * Parameters:
 *     unit    -(IN)BCM device number
 *     f_ent   -(IN)Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     action1 -(IN) Action to check(bcmFieldActionXXX)
 *
 * Returns:
 *     BCM_E_CONFIG - if actions do conflict
 *     BCM_E_NONE   - if there is no conflict
 */
int
_bcm_field_trx_action_conflict_check(int unit, _field_entry_t *f_ent,
                                 bcm_field_action_t action1, 
                                 bcm_field_action_t action)
{
    /* Input parameters check */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    /* Two identical actions are forbidden. */
    if (action1 == action) {
        if ((action != bcmFieldActionMirrorIngress) &&
            (action != bcmFieldActionMirrorEgress)) {
            return (BCM_E_CONFIG);
        }
    }

    if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
        switch (action1) {
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
              break; 
          case bcmFieldActionRpDrop:
          case bcmFieldActionRpDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
              break; 
          case bcmFieldActionYpDrop:
          case bcmFieldActionYpDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
              break; 
          case bcmFieldActionGpDrop:
          case bcmFieldActionGpDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
              break; 

          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
              break;
          case bcmFieldActionRpCopyToCpu:
          case bcmFieldActionRpCopyToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
              break;
          case bcmFieldActionYpCopyToCpu:
          case bcmFieldActionYpCopyToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
              break;
          case bcmFieldActionGpCopyToCpu:
          case bcmFieldActionGpCopyToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
              break;
          case bcmFieldActionOuterVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpOuterVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpOuterVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpOuterVlanPrioNew);
              break;
          case bcmFieldActionRpOuterVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpOuterVlanPrioNew);
              break;
          case bcmFieldActionYpOuterVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpOuterVlanPrioNew);
              break;
          case bcmFieldActionGpOuterVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpOuterVlanPrioNew);
              break;
          case bcmFieldActionInnerVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpInnerVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpInnerVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpInnerVlanPrioNew);
              break;
          case bcmFieldActionRpInnerVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpInnerVlanPrioNew);
              break;
          case bcmFieldActionYpInnerVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpInnerVlanPrioNew);
              break;
          case bcmFieldActionGpInnerVlanPrioNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanPrioNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpInnerVlanPrioNew);
              break;
          case bcmFieldActionIpFix:
          case bcmFieldActionIpFixCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionIpFix);
              _FP_ACTIONS_CONFLICT(bcmFieldActionIpFixCancel);
              break;
          case bcmFieldActionOuterVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpOuterVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpOuterVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpOuterVlanCfiNew);
              break;
          case bcmFieldActionRpOuterVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpOuterVlanCfiNew);
              break;
          case bcmFieldActionYpOuterVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpOuterVlanCfiNew);
              break;
          case bcmFieldActionGpOuterVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpOuterVlanCfiNew);
              break;
          case bcmFieldActionInnerVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpInnerVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpInnerVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpInnerVlanCfiNew);
              break;
          case bcmFieldActionRpInnerVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpInnerVlanCfiNew);
              break;
          case bcmFieldActionYpInnerVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpInnerVlanCfiNew);
              break;
          case bcmFieldActionGpInnerVlanCfiNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanCfiNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpInnerVlanCfiNew);
              break;
          case bcmFieldActionUpdateCounter:
          case bcmFieldActionMeterConfig:
          case bcmFieldActionInnerVlanNew:
          case bcmFieldActionOuterVlanNew:
          case bcmFieldActionOuterTpidNew:
          case bcmFieldActionRpDscpNew:
          case bcmFieldActionYpDscpNew:
          case bcmFieldActionGpDscpNew:
              break;
          default:
              break;
        }
        return (BCM_E_NONE);
    }
    if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
        switch (action1) {
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              break;
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              break;
          case bcmFieldActionInnerVlanAdd:
          case bcmFieldActionInnerVlanNew:
          case bcmFieldActionInnerVlanDelete:
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanAdd);
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionInnerVlanDelete);
              break;
          case bcmFieldActionOuterVlanAdd:
          case bcmFieldActionOuterVlanNew:
          case bcmFieldActionOuterVlanLookup:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanAdd);
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanLookup);
              break;
          case bcmFieldActionPrioPktNew:
          case bcmFieldActionCosQCpuNew:
          case bcmFieldActionPrioPktAndIntNew:
          case bcmFieldActionPrioIntNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
              break;
          case bcmFieldActionDropPrecedence:
          case bcmFieldActionVrfSet:
          case bcmFieldActionDoNotLearn:
          case bcmFieldActionDoNotCheckVlan:
          case bcmFieldActionClassDestSet:
          case bcmFieldActionClassSourceSet:
          case bcmFieldActionIncomingMplsPortSet:
          case bcmFieldActionOamPbbteLookupEnable:
              break;
          default:
              break;
        }
        return (BCM_E_NONE);
    }

    if ((_BCM_FIELD_STAGE_INGRESS == f_ent->group->stage_id) ||
        (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id)) {
        switch (action1) {
          case bcmFieldActionCopyToCpu:
          case bcmFieldActionCopyToCpuCancel:
          case bcmFieldActionSwitchToCpuCancel:
          case bcmFieldActionSwitchToCpuReinstate:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuReinstate);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuReinstate);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuReinstate);
              break;
          case bcmFieldActionRpCopyToCpu:
          case bcmFieldActionRpCopyToCpuCancel:
          case bcmFieldActionRpSwitchToCpuCancel:
          case bcmFieldActionRpSwitchToCpuReinstate:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuReinstate);
              break;
          case bcmFieldActionYpCopyToCpu:
          case bcmFieldActionYpCopyToCpuCancel:
          case bcmFieldActionYpSwitchToCpuCancel:
          case bcmFieldActionYpSwitchToCpuReinstate:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpSwitchToCpuReinstate);
              break;
          case bcmFieldActionGpCopyToCpu:
          case bcmFieldActionGpCopyToCpuCancel:
          case bcmFieldActionGpSwitchToCpuCancel:
          case bcmFieldActionGpSwitchToCpuReinstate:
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuReinstate);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpSwitchToCpuReinstate);
              break;
          case bcmFieldActionDrop:
          case bcmFieldActionDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
              break;
          case bcmFieldActionRpDrop:
          case bcmFieldActionRpDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
              break;
          case bcmFieldActionYpDrop:
          case bcmFieldActionYpDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
              break;
          case bcmFieldActionGpDrop:
          case bcmFieldActionGpDropCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
              break;
          case bcmFieldActionL3ChangeVlan:
          case bcmFieldActionL3ChangeVlanCancel:
          case bcmFieldActionL3ChangeMacDa:
          case bcmFieldActionL3ChangeMacDaCancel:
          case bcmFieldActionL3Switch:
          case bcmFieldActionL3SwitchCancel:
          case bcmFieldActionAddClassTag:
              _FP_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionSrcMacNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDstMacNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
              break;
          case bcmFieldActionOuterVlanNew:
          case bcmFieldActionSrcMacNew:
          case bcmFieldActionDstMacNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
              _FP_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
              break;
          case bcmFieldActionRedirect:
          case bcmFieldActionRedirectTrunk:
          case bcmFieldActionRedirectCancel:
          case bcmFieldActionRedirectPbmp:
          case bcmFieldActionRedirectIpmc:
          case bcmFieldActionRedirectMcast:
          case bcmFieldActionOffloadRedirect:
          case bcmFieldActionEgressMask:
              _FP_ACTIONS_CONFLICT(bcmFieldActionOffloadRedirect);
              _FP_ACTIONS_CONFLICT(bcmFieldActionEgressMask);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRedirect);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectTrunk);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectPbmp);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectIpmc);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectMcast);
              break;
          case bcmFieldActionPrioPktCopy:
          case bcmFieldActionPrioPktNew:
          case bcmFieldActionPrioPktTos:
          case bcmFieldActionPrioPktCancel:
          case bcmFieldActionPrioPktAndIntCopy:
          case bcmFieldActionPrioPktAndIntNew:
          case bcmFieldActionPrioPktAndIntTos:
          case bcmFieldActionPrioPktAndIntCancel:
          case bcmFieldActionPrioIntCopy:
          case bcmFieldActionPrioIntNew:
          case bcmFieldActionPrioIntTos:
          case bcmFieldActionPrioIntCancel:
          case bcmFieldActionVlanCosQNew:
          case bcmFieldActionCosQNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
              break;
          case bcmFieldActionRpPrioPktCopy:
          case bcmFieldActionRpPrioPktNew:
          case bcmFieldActionRpPrioPktTos:
          case bcmFieldActionRpPrioPktCancel:
          case bcmFieldActionRpPrioPktAndIntCopy:
          case bcmFieldActionRpPrioPktAndIntNew:
          case bcmFieldActionRpPrioPktAndIntTos:
          case bcmFieldActionRpPrioPktAndIntCancel:
          case bcmFieldActionRpPrioIntCopy:
          case bcmFieldActionRpPrioIntNew:
          case bcmFieldActionRpPrioIntTos:
          case bcmFieldActionRpPrioIntCancel:
          case bcmFieldActionRpCosQNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
              break;
          case bcmFieldActionYpPrioPktCopy:
          case bcmFieldActionYpPrioPktNew:
          case bcmFieldActionYpPrioPktTos:
          case bcmFieldActionYpPrioPktCancel:
          case bcmFieldActionYpPrioPktAndIntCopy:
          case bcmFieldActionYpPrioPktAndIntNew:
          case bcmFieldActionYpPrioPktAndIntTos:
          case bcmFieldActionYpPrioPktAndIntCancel:
          case bcmFieldActionYpPrioIntCopy:
          case bcmFieldActionYpPrioIntNew:
          case bcmFieldActionYpPrioIntTos:
          case bcmFieldActionYpPrioIntCancel:
          case bcmFieldActionYpCosQNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
              break;
          case bcmFieldActionGpPrioPktCopy:
          case bcmFieldActionGpPrioPktNew:
          case bcmFieldActionGpPrioPktTos:
          case bcmFieldActionGpPrioPktCancel:
          case bcmFieldActionGpPrioPktAndIntCopy:
          case bcmFieldActionGpPrioPktAndIntNew:
          case bcmFieldActionGpPrioPktAndIntTos:
          case bcmFieldActionGpPrioPktAndIntCancel:
          case bcmFieldActionGpPrioIntCopy:
          case bcmFieldActionGpPrioIntNew:
          case bcmFieldActionGpPrioIntTos:
          case bcmFieldActionGpPrioIntCancel:
          case bcmFieldActionGpCosQNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpVlanCosQNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
              break;
          case bcmFieldActionDropPrecedence:
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
              break;
          case bcmFieldActionRpDropPrecedence:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
              break;
          case bcmFieldActionYpDropPrecedence:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
              break;
          case bcmFieldActionGpDropPrecedence:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
              break;
          case bcmFieldActionTimeStampToCpu:
          case bcmFieldActionTimeStampToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpuCancel);
              break;
          case bcmFieldActionRpTimeStampToCpu:
          case bcmFieldActionRpTimeStampToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpTimeStampToCpuCancel);
              break;
          case bcmFieldActionYpTimeStampToCpu:
          case bcmFieldActionYpTimeStampToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpTimeStampToCpuCancel);
              break;
          case bcmFieldActionGpTimeStampToCpu:
          case bcmFieldActionGpTimeStampToCpuCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionTimeStampToCpuCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpu);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpTimeStampToCpuCancel);
              break;
          case bcmFieldActionIpFix:
          case bcmFieldActionIpFixCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionIpFix);
              _FP_ACTIONS_CONFLICT(bcmFieldActionIpFixCancel);
              break;
          case bcmFieldActionDscpNew:
          case bcmFieldActionDscpCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpCancel);
              break; 
          case bcmFieldActionRpDscpNew:
          case bcmFieldActionRpDscpCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpCancel);
              break; 
          case bcmFieldActionYpDscpNew:
          case bcmFieldActionYpDscpCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpCancel);
              break; 
          case bcmFieldActionGpDscpNew:
          case bcmFieldActionGpDscpCancel:
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpCancel);
              break; 
          case bcmFieldActionEcnNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpEcnNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpEcnNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpEcnNew);
              break;
          case bcmFieldActionRpEcnNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionRpEcnNew);
              break;
          case bcmFieldActionYpEcnNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionYpEcnNew);
              break;
          case bcmFieldActionGpEcnNew:
              _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
              _FP_ACTIONS_CONFLICT(bcmFieldActionGpEcnNew);
              break;
          case bcmFieldActionMirrorIngress:
              _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
              break;
          case bcmFieldActionMirrorEgress:
              _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
              break;
          case bcmFieldActionMirrorOverride:
              _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
              _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
              break;
          case bcmFieldActionDoNotChangeTtl:
          case bcmFieldActionDoNotCheckUrpf:
          case bcmFieldActionMeterConfig:
          case bcmFieldActionUpdateCounter:
          case bcmFieldActionColorIndependent:
          case bcmFieldActionMultipathHash:
          case bcmFieldActionOamUpMep:
          case bcmFieldActionOamTx:
          case bcmFieldActionOamLmepMdl:
          case bcmFieldActionOamServicePriMappingPtr:
          case bcmFieldActionOamLmBasePtr:
          case bcmFieldActionOamDmEnable:
          case bcmFieldActionOamLmEnable:
          case bcmFieldActionOamLmepEnable:
          case bcmFieldActionOffloadClassSet:
          case bcmFieldActionOffloadDropIndication:
              break;
          default:
              break;
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_egress_key_match_type_set
 *
 * Purpose:
 *     Set key match type based on entry group.
 *     NOTE: For double wide entries key type must be the same for 
 *           both parts of the entry.   
 * Parameters:
 *     unit   - (IN) BCM device number
 *     f_ent  - (IN) Slice number to enable
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_egress_key_match_type_set (int unit, _field_entry_t *f_ent)
{
    _field_group_t           *fg;   /* Field group entry belongs to. */
    uint32                   data;  /* Key match type.               */         
    uint32                   mask;  /* Key match type mask.          */         
    /* Key match type offset.        */
    _bcm_field_qual_offset_t q_offset = {KEYf, 207, 3, 0, 0, 0, 0}; 

    /* Input parameters check. */ 
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    } 
    if (NULL == (fg = f_ent->group)) {
        return (BCM_E_PARAM);
    }

    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        switch (fg->sel_codes[0].fpf3) {
          case _BCM_FIELD_EFP_KEY1:
              data = _BCM_FIELD_EFP_KEY1_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              data = _BCM_FIELD_EFP_KEY4_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY5:
              data = _BCM_FIELD_EFP_KEY1_MATCH_TYPE;
              break;
          default:
              return (BCM_E_INTERNAL);
        } 
    } else {
        switch (fg->sel_codes[1].fpf3) {
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              if ((_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3) ||
                  (_BCM_FIELD_EFP_KEY5 == fg->sel_codes[0].fpf3)) {
                  data = _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE;
              } else {
                  data = _BCM_FIELD_EFP_KEY2_KEY4_MATCH_TYPE;
              }
              break;
          default:
              return (BCM_E_INTERNAL);
        }
    }

    if (SOC_IS_TR_VL(unit)) {
        q_offset.offset += 4;
    }
    mask = 0x7;
   return _bcm_field_qual_value_set(unit, &q_offset, f_ent, &data, &mask);
}

/*
 * Function:
 *     _bcm_field_trx_qualify_ip_type
 * Purpose:
 *     Install ip type qualifier into TCAM
 * Parameters:
 *     unit  - (IN) BCM device number
 *     f_ent - (IN) Field entry qualifier
 *     type  - (IN) Ip Type. 
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
#define BCM_FIELD_IPTYPE_BAD 0xff
int
_bcm_field_trx_qualify_ip_type(int unit, _field_entry_t *f_ent, 
                               bcm_field_IpType_t type) 
{

    _field_group_t      *fg;
    uint32              data = BCM_FIELD_IPTYPE_BAD,
                        mask = BCM_FIELD_IPTYPE_BAD;

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    switch (type) {
      case bcmFieldIpTypeAny:
          data = 0x0;
          mask = 0x0;
          break;
      case bcmFieldIpTypeNonIp:
          data = 0xf;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIp:
          data = 0x0;
          mask = 0x8;
          break;
          break;
      case bcmFieldIpTypeIpv4NoOpts:
          data = 0x0;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv4WithOpts:
          data = 0x1;
          mask = 0xf;
          break;
      case bcmFieldIpTypeIpv4Any:
          data = 0x0;
          mask = 0xe;
          break;
      case bcmFieldIpTypeIpv6:
          data = 0x4;
          mask = 0xc;
          break;
      case bcmFieldIpTypeArp:
          data = 0x8;
          mask = 0xe;
          break;
      case bcmFieldIpTypeArpRequest:
          data = 0x8;
          mask = 0xf;
          break;
      case bcmFieldIpTypeArpReply:
          data = 0x9;
          mask = 0xf;
          break;
      case bcmFieldIpTypeMplsUnicast:
          data = 0xc;
          mask = 0xf;
          break;
      case bcmFieldIpTypeMplsMulticast:
          data = 0xd;
          mask = 0xf;
          break;
      default:
          break;
    }
    FP_VVERB(("FP(unit %d) vverb: entry=%d qualifying on Iptype, data=%#x, mask=%#x\n))", 
             unit, f_ent->eid, data, mask));

    if ((data == BCM_FIELD_IPTYPE_BAD) ||
        (mask == BCM_FIELD_IPTYPE_BAD)) {
        return (BCM_E_UNAVAIL);
    }

    return _field_qualify32(unit, f_ent->eid, bcmFieldQualifyIpType,
                            data, mask);
}
#undef BCM_FIELD_IPTYPE_BAD
/*
 * Function:
 *     _bcm_field_trx_qualify_ip_type_get
 * Purpose:
 *     Read ip type qualifier match criteria from the HW.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     f_ent - (IN) Field entry qualifier
 *     type  - (OUT) Ip Type. 
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_trx_qualify_ip_type_get(int unit, _field_entry_t *f_ent, 
                              bcm_field_IpType_t *type) 
{
    _field_group_t *fg;      /* Field group structure.      */
    uint32 hw_data;          /* HW encoded qualifier data.  */
    uint32 hw_mask;          /* HW encoding qualifier mask. */
    int rv;                  /* Operation return status.    */

    /* Input parameters checks. */
    if ((NULL == f_ent) || (NULL == type)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    /* Read qualifier match value and mask. */
    rv = _bcm_field_entry_qualifier_uint32_get(unit, f_ent->eid,
                                               bcmFieldQualifyIpType,
                                               &hw_data, &hw_mask);
    BCM_IF_ERROR_RETURN(rv);

    if ((0 == hw_data) && (0 == hw_mask)) {
        *type = bcmFieldIpTypeAny;
    } else if ((0xf == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeNonIp;
    } else if ((0x0 == hw_data) && (0x8 == hw_mask)) {
        *type = bcmFieldIpTypeIp;
    } else if ((0x0 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv4NoOpts;
    } else if ((0x1 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeIpv4WithOpts;
    } else if ((0x0 == hw_data) && (0xe == hw_mask)) {
        *type = bcmFieldIpTypeIpv4Any;
    } else if ((0x4 == hw_data) && (0xc == hw_mask)) {
        *type = bcmFieldIpTypeIpv6;
    } else if ((0x8 == hw_data) && (0xe == hw_mask)) {
        *type = bcmFieldIpTypeArp;
    } else if ((0x8 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeArpRequest;
    } else if ((0x9 == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeArpReply;
    } else if ((0xc == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeMplsUnicast;
    } else if ((0xd == hw_data) && (0xf == hw_mask)) {
        *type = bcmFieldIpTypeMplsMulticast;
    } else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_trx_range_check_set
 * Purpose:
 *     Write the group's range checking parameters into the 
 *     FP_RANGE_CHECK  memory.
 * Parameters:
 *     unit   - (IN) BCM device number. 
 *     range  - (IN) Range HW index
 *     flags  - (IN) One of more of the BCM_FIELD_RANGE_* flags
 *     enable - (IN) TRUE or FALSE
 *     min    - (IN) Lower bounds of port range to be checked
 *     max    - (IN) Upper bounds of port range to be checked
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_range_check_set(int unit, int range, uint32 flags, int enable,
                               bcm_l4_port_t min, bcm_l4_port_t max)
{
                            /* Range table entry buffer. */ 
    uint32                  tbl_entry[SOC_MAX_MEM_FIELD_WORDS]; 
                                                                   
    soc_mem_t               fp_range_check_mem;     /* Range table memory. */
    uint32                  range_type_encoded = 0; /* Range type encoded. */
    uint32                  range_type = 0;         /* Range type.         */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (flags & BCM_FIELD_RANGE_EXTERNAL) {
        fp_range_check_mem = ESM_RANGE_CHECKm;
    } else 
#endif /* BCM_TRIUMPH_SUPPORT */
    {
        fp_range_check_mem = FP_RANGE_CHECKm;
    }

    /* Index sanity check. */
    if (!soc_mem_index_valid(unit, fp_range_check_mem, range)) { 
        return (BCM_E_PARAM);
    }

    if (enable) {
        range_type = flags & (BCM_FIELD_RANGE_SRCPORT |    \
                              BCM_FIELD_RANGE_DSTPORT |    \
                              BCM_FIELD_RANGE_OUTER_VLAN | \
                              BCM_FIELD_RANGE_PACKET_LENGTH);
        switch (range_type) {
          case BCM_FIELD_RANGE_SRCPORT:
              range_type_encoded = 0x0;
              break;
          case BCM_FIELD_RANGE_DSTPORT:
              range_type_encoded = 0x1;
              break;
          case BCM_FIELD_RANGE_OUTER_VLAN:
              range_type_encoded = 0x2;
              break;
          case BCM_FIELD_RANGE_PACKET_LENGTH:
              range_type_encoded = 0x3;
              break;
          default:
              FP_ERR(("FP(unit %d) Error: unsupported flags %#x\n", unit, flags));
              return (BCM_E_PARAM);
        }
    }

    /* read/modify/write range check memory */
    sal_memset(tbl_entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set source/destination port selection. */
    soc_mem_field_set(unit, fp_range_check_mem, tbl_entry, 
                      FIELD_SELECTf, &range_type_encoded);

    if (SOC_MEM_FIELD_VALID(unit, fp_range_check_mem, ENABLEf)) {
        soc_mem_field_set(unit, fp_range_check_mem, tbl_entry, 
                          ENABLEf, (uint32 *) &enable);
    }

    /* Set range min value. */
    soc_mem_field_set(unit, fp_range_check_mem, tbl_entry, 
                      LOWER_BOUNDSf, (uint32 *) &min);

    /* Set range max value. */
    soc_mem_field_set(unit, fp_range_check_mem, tbl_entry, 
                      UPPER_BOUNDSf, (uint32 *) &max);

    /* Write entry back to hw. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, fp_range_check_mem, 
                                      MEM_BLOCK_ALL, range, tbl_entry));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_udf_index_to_flags
 * Purpose:
 *     Translate the FP_UDF_OFFSETm  table index to API flags.
 * Parameters:
 *     tbl_idx - (IN) Index into FP_UDF_OFFSET table
 *     flags   - (OUT) Client specified flags.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcm_field_trx_udf_index_to_flags(int unit, uint32 tbl_idx, int *flags)
{
    int                 index;

    /* Input parameters check. */
    if (NULL == flags) {
        return (BCM_E_PARAM);
    }

    index = tbl_idx & 0xf;
    switch (index) {
      case 0x0:
          *flags |= BCM_FIELD_USER_IP4_HDR_ONLY;
          break;
      case 0x1:
          *flags |= BCM_FIELD_USER_IP6_HDR_ONLY;
          break;
      case 0x2:
          *flags |= BCM_FIELD_USER_IP4_OVER_IP4;
          break;
      case 0x3:
          *flags |= BCM_FIELD_USER_IP6_OVER_IP4;
          break;
      case 0x4:
          *flags |= BCM_FIELD_USER_IP4_OVER_IP6;
          break;
      case 0x5:
          *flags |= BCM_FIELD_USER_IP6_OVER_IP6;
          break;
      case 0x6:
          *flags |= BCM_FIELD_USER_GRE_IP4_OVER_IP4;
          break;
      case 0x7:
          *flags |= BCM_FIELD_USER_GRE_IP6_OVER_IP4;
          break;
      case 0x8:
          *flags |= BCM_FIELD_USER_GRE_IP4_OVER_IP6;
          break;
      case 0x9:
          *flags |= BCM_FIELD_USER_GRE_IP6_OVER_IP6;
          break;
      case 0xa:
          if (SOC_IS_SC_CQ(unit)) {
              *flags |= BCM_FIELD_USER_IP_NOTUSED;
          } else {
              *flags |= BCM_FIELD_USER_ONE_MPLS_LABEL;
          }
          break;
      case 0xb:
          *flags |= BCM_FIELD_USER_TWO_MPLS_LABELS;
          break;
      case 0xc:
          *flags |= BCM_FIELD_USER_IP_NOTUSED;
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    index = (tbl_idx >> (SOC_IS_SC_CQ(unit) ?  4 : 5)) & 0x3;
    switch (index) {
      case 0x0:
          *flags |= BCM_FIELD_USER_L2_ETHERNET2;
          break;
      case 0x1:
          *flags |= BCM_FIELD_USER_L2_SNAP;
          break;
      case 0x2:
          *flags |= BCM_FIELD_USER_L2_LLC;
          break;
      default:        
          return (BCM_E_INTERNAL);
    }

    index =  (tbl_idx >> (SOC_IS_SC_CQ(unit) ?  6 : 7));
    switch (index) {
      case 0x0:
          *flags |= BCM_FIELD_USER_VLAN_NOTAG;
          break;
      case 0x1:
          *flags |= BCM_FIELD_USER_VLAN_ONETAG;
          break;
      case 0x2:
          *flags |= BCM_FIELD_USER_VLAN_TWOTAG;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_trx_udf_flags_to_index
 * Purpose:
 *     Translate the client flags into a  FP_UDF_OFFSETm table index.
 * Parameters:
 *     flags   - Client specified flags.
 *     tbl_idx - (OUT) index into FP_UDF_OFFSET table
 * Returns:
 *     BCM_E_RESOURCE Client's flags won't work on this unit.
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_trx_udf_flags_to_index (int unit, int flags, uint32 *tbl_idx)
{
    uint32              flag_mask;
    int                 index;

    flag_mask = (BCM_FIELD_USER_VLAN_MASK | BCM_FIELD_USER_L2_MASK | \
      BCM_FIELD_USER_IP_MASK | BCM_FIELD_USER_OPTION_ADJUST |        \
      BCM_FIELD_USER_GRE_OPTION_ADJUST | BCM_FIELD_USER_HIGIG |      \
      BCM_FIELD_USER_HIGIG2);


    if (flags & ~(flag_mask)) {
        FP_ERR(("FP(unit %d) Error: Invalid Flags=0x%x, flag_mask=0x%x\n", 
                unit, flags, flag_mask));
        return (BCM_E_PARAM);
    }

    /* No mpls support on Scorpion / Conqueror */
    if (SOC_IS_SC_CQ(unit)) {
        switch (flags & BCM_FIELD_USER_IP_MASK) { 
          case BCM_FIELD_USER_ONE_MPLS_LABEL:     
          case BCM_FIELD_USER_TWO_MPLS_LABELS:     
              FP_ERR(("FP Error %d: No mpls support.\n", unit));
              return (BCM_E_RESOURCE);
              break;
          default:
              break;
        }
    }

    *tbl_idx = 0u; /* clear table index bits */
    /* Translate VLAN flag bits to index */
    switch (flags & BCM_FIELD_USER_VLAN_MASK) {
      case  BCM_FIELD_USER_VLAN_NOTAG:
          index =  0x0;
          break;
      case BCM_FIELD_USER_VLAN_ONETAG:
          index =  0x1;
          break;
      case BCM_FIELD_USER_VLAN_TWOTAG:
          index =  0x2;
          break;
      default:
          FP_ERR(("FP(unit %d) Error: Flags 0x%x needs a Tag flag.\n", unit, flags));
          return (BCM_E_RESOURCE);
    }
    *tbl_idx |=  (index << (SOC_IS_SC_CQ(unit) ?  6 : 7));


    /* Translate L2 flag bits to index */
    switch (flags & BCM_FIELD_USER_L2_MASK) { 
      case BCM_FIELD_USER_L2_ETHERNET2:
          index =  0x0; 
          break;
      case BCM_FIELD_USER_L2_SNAP:
          index =  0x1; 
          break;
      case BCM_FIELD_USER_L2_LLC:
          index =  0x2; 
          break;
      default:        
          FP_ERR(("FP(unit %d) Error: Flags 0x%x needs L2 Format flag.\n", unit, flags));
          return (BCM_E_RESOURCE);
    }
    *tbl_idx |=  (index << (SOC_IS_SC_CQ(unit) ?  4 : 5));

    /* Translate IP flag bits to index bits */
    switch (flags & BCM_FIELD_USER_IP_MASK) { 
      case BCM_FIELD_USER_IP4_HDR_ONLY:
          index =  0x0; 
          break;
      case BCM_FIELD_USER_IP6_HDR_ONLY:
          index =  0x1; 
          break;
      case BCM_FIELD_USER_IP4_OVER_IP4:
          index =  0x2; 
          break;
      case BCM_FIELD_USER_IP6_OVER_IP4:
          index =  0x3; 
          break;
      case BCM_FIELD_USER_IP4_OVER_IP6:
          index =  0x4; 
          break;
      case BCM_FIELD_USER_IP6_OVER_IP6:
          index =  0x5; 
          break;
      case BCM_FIELD_USER_GRE_IP4_OVER_IP4:
          index =  0x6; 
          break;
      case BCM_FIELD_USER_GRE_IP6_OVER_IP4:
          index =  0x7; 
          break;
      case BCM_FIELD_USER_GRE_IP4_OVER_IP6:
          index =  0x8; 
          break;
      case BCM_FIELD_USER_GRE_IP6_OVER_IP6:
          index =  0x9; 
          break;
      case BCM_FIELD_USER_ONE_MPLS_LABEL:     
          index =  0xa; 
          break;
      case BCM_FIELD_USER_TWO_MPLS_LABELS:     
          index =  0xb; 
          break;
      case BCM_FIELD_USER_IP_NOTUSED:     
          if (SOC_IS_SC_CQ(unit)) {
              index = 0xa;
          } else { 
              index =  0xc; 
          }
          break;
      default:        
          FP_ERR(("FP(unit %d) Error: Flags 0x%x needs IP Header Format flag.\n", unit, flags));
          return (BCM_E_RESOURCE);
    }
    *tbl_idx |= index;

    if ((*tbl_idx >  soc_mem_index_max(unit, FP_UDF_OFFSETm)) && 
        (0 == SOC_MEM_IS_VALID(unit, FP_UDF_TCAMm))) {
        return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_trx_udf_spec_set
 * Purpose:
 *     Write user defined field configuration to udf_spec_t structure
 * Parameters:
 *     
 * Returns:
 *     BCM_E_PARAM - flags or offset out of range
 *     BCM_E_NONE
 */
int
_bcm_field_trx_udf_spec_set(int unit, bcm_field_udf_spec_t *udf_spec, 
                            uint32 flags, uint32 offset)
{
    uint32   pkt_fmt_idx = 0u;     /* index into udf_spec */

    if (offset > 31) {
        FP_ERR(("FP(unit %d) Error: offset=%d out-of-range.\n", unit, offset));
        return BCM_E_PARAM;
    }
    if (0 == SOC_MEM_FIELD_VALID(unit, FP_UDF_OFFSETm, UDF1_ADD_IPV4_OPTIONS0f)) {
       if (flags & BCM_FIELD_USER_OPTION_ADJUST) {
           return (BCM_E_UNAVAIL);
       }
    }
    if (0 == SOC_MEM_FIELD_VALID(unit, FP_UDF_OFFSETm, UDF1_ADD_GRE_OPTIONS0f)) {
       if (flags & BCM_FIELD_USER_GRE_OPTION_ADJUST) {
           return (BCM_E_UNAVAIL);
       } 
    }

    BCM_IF_ERROR_RETURN(_bcm_field_trx_udf_flags_to_index(unit, flags, 
                                                          &pkt_fmt_idx));

    udf_spec->offset[pkt_fmt_idx] = offset;
    if (flags & BCM_FIELD_USER_OPTION_ADJUST) {
        udf_spec->offset[pkt_fmt_idx] |= BCM_FIELD_USER_OPTION_ADJUST;
    }

    if (flags & BCM_FIELD_USER_GRE_OPTION_ADJUST) {
        udf_spec->offset[pkt_fmt_idx] |= BCM_FIELD_USER_GRE_OPTION_ADJUST;
    }
    /* Mark offset as valid. */
    udf_spec->offset[pkt_fmt_idx] |= _BCM_FIELD_USER_OFFSET_VALID;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_trx_udf_spec_get
 * Purpose:
 *     Get user defined field configuration entry from udf_spec_t structure
 * Parameters:
 *     
 * Returns:
 *     BCM_E_PARAM - flags or offset out of range
 *     BCM_E_NONE
 */
int
_bcm_field_trx_udf_spec_get(int unit, bcm_field_udf_spec_t *udf_spec, 
                            uint32 *flags, uint32 *offset)
{
    uint32               pkt_fmt_idx = 0u;     /* index into udf_spec */

    BCM_IF_ERROR_RETURN(_bcm_field_trx_udf_flags_to_index(unit, *flags,
                                                        &pkt_fmt_idx));

    if ((*offset = udf_spec->offset[pkt_fmt_idx] &
         ~(_BCM_FIELD_USER_OFFSET_FLAGS)) > 31) {
        FP_ERR(("FP(unit %d) Error: offset=%d out-of-range.\n", unit, *offset));
        return BCM_E_PARAM;
    }

    if (udf_spec->offset[pkt_fmt_idx] & BCM_FIELD_USER_OPTION_ADJUST) {
        *flags |= BCM_FIELD_USER_OPTION_ADJUST;
    }
    else {
        *flags &= ~(BCM_FIELD_USER_OPTION_ADJUST);
    }

    if (udf_spec->offset[pkt_fmt_idx] & BCM_FIELD_USER_GRE_OPTION_ADJUST) {
        *flags |= BCM_FIELD_USER_GRE_OPTION_ADJUST;
    }
    else {
        *flags &= ~(BCM_FIELD_USER_GRE_OPTION_ADJUST);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_trx_udf_write
 * Purpose:
 *     Write the info in udf_spec to hardware
 * Parameters:
 *     
 * Returns:
 *     BCM_E_PARAM - UDF number or User number out of range
 *     BCM_E_NONE
 * Notes:
 *     Calling function should have unit's lock.
 */
int
_bcm_field_trx_udf_write(int unit, bcm_field_udf_spec_t *udf_spec, 
                       uint32 udf_num, uint32 user_num)
{
    uint16               tbl_idx = 0u;        /* index into H/W table */
    fp_udf_offset_entry_t  tbl_entry;
    uint8               f_idx;                /* field selector index */ 
    uint32              offset;            

    static soc_field_t  off_field[] = {UDF1_OFFSET0f, UDF1_OFFSET1f, 
                                       UDF1_OFFSET2f, UDF1_OFFSET3f,
                                       UDF2_OFFSET0f, UDF2_OFFSET1f,
                                       UDF2_OFFSET2f, UDF2_OFFSET3f};

    static soc_field_t  option_field[] = {UDF1_ADD_IPV4_OPTIONS0f,
                                          UDF1_ADD_IPV4_OPTIONS1f,
                                          UDF1_ADD_IPV4_OPTIONS2f,
                                          UDF1_ADD_IPV4_OPTIONS3f,
                                          UDF2_ADD_IPV4_OPTIONS0f,
                                          UDF2_ADD_IPV4_OPTIONS1f,
                                          UDF2_ADD_IPV4_OPTIONS2f,
                                          UDF2_ADD_IPV4_OPTIONS3f};

    static soc_field_t  gre_option_field[] = {UDF1_ADD_GRE_OPTIONS0f,
                                           UDF1_ADD_GRE_OPTIONS1f,
                                           UDF1_ADD_GRE_OPTIONS2f,
                                           UDF1_ADD_GRE_OPTIONS3f,
                                           UDF2_ADD_GRE_OPTIONS0f,
                                           UDF2_ADD_GRE_OPTIONS1f,
                                           UDF2_ADD_GRE_OPTIONS2f,
                                           UDF2_ADD_GRE_OPTIONS3f};


    if ((udf_num > 1) || (user_num > 3)) {
        FP_ERR(("FP(unit %d) Error: udf_num=%d or user_num=%d out-of-range.\n", 
                unit, udf_num, user_num));
        return BCM_E_PARAM;
    }

    /* Formulate the desired FP_UDF_OFFSET fields */
    f_idx = (udf_num << 2) | user_num;

    for (tbl_idx = soc_mem_index_min(unit, FP_UDF_OFFSETm);
         tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm);
         tbl_idx++) { 
        offset = udf_spec->offset[tbl_idx] & ~(_BCM_FIELD_USER_OFFSET_FLAGS);
        if (offset > 31) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN
          (READ_FP_UDF_OFFSETm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_entry));
        soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                       off_field[f_idx], 
                                       offset);

        /* Set the UDFn_ADD_IPV4_OPTIONSm bit accordingly */ 
        if (udf_spec->offset[tbl_idx] & BCM_FIELD_USER_OPTION_ADJUST) {
            soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                           option_field[f_idx], 1);
        }
        else {
            soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                           option_field[f_idx], 0);
        }

        /* Set the UDFn_ADD_GRE_OPTIONSm bit accordingly */ 
        if (udf_spec->offset[tbl_idx] & BCM_FIELD_USER_GRE_OPTION_ADJUST) {
            soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                           gre_option_field[f_idx], 1);
        }
        else {
            soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                           gre_option_field[f_idx], 0);
        }

        SOC_IF_ERROR_RETURN
          (WRITE_FP_UDF_OFFSETm(unit, MEM_BLOCK_ALL, tbl_idx, &tbl_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_trx_udf_read
 * Purpose:
 *     Read info into udf_spec from hardware
 * Parameters:
 *     
 * Returns:
 *     BCM_E_PARAM - UDF number or User number out of range
 *     BCM_E_NONE
 * Notes:
 *     Calling function should have unit's lock.
 */
int
_bcm_field_trx_udf_read(int unit, bcm_field_udf_spec_t *udf_spec, 
                      uint32 udf_num, uint32 user_num)
{
    uint16               tbl_idx = 0u;         /* index into H/W table */
    fp_udf_offset_entry_t  tbl_entry;
    uint8               f_idx;                /* field selector index */ 

    static soc_field_t  off_field[] = {UDF1_OFFSET0f, UDF1_OFFSET1f, 
                                       UDF1_OFFSET2f, UDF1_OFFSET3f,
                                       UDF2_OFFSET0f, UDF2_OFFSET1f,
                                       UDF2_OFFSET2f, UDF2_OFFSET3f};

    static soc_field_t  option_field[] = {UDF1_ADD_IPV4_OPTIONS0f,
                                          UDF1_ADD_IPV4_OPTIONS1f,
                                          UDF1_ADD_IPV4_OPTIONS2f,
                                          UDF1_ADD_IPV4_OPTIONS3f,
                                          UDF2_ADD_IPV4_OPTIONS0f,
                                          UDF2_ADD_IPV4_OPTIONS1f,
                                          UDF2_ADD_IPV4_OPTIONS2f,
                                          UDF2_ADD_IPV4_OPTIONS3f};

    static soc_field_t  gre_option_field[] = {UDF1_ADD_GRE_OPTIONS0f,
                                           UDF1_ADD_GRE_OPTIONS1f,
                                           UDF1_ADD_GRE_OPTIONS2f,
                                           UDF1_ADD_GRE_OPTIONS3f,
                                           UDF2_ADD_GRE_OPTIONS0f,
                                           UDF2_ADD_GRE_OPTIONS1f,
                                           UDF2_ADD_GRE_OPTIONS2f,
                                           UDF2_ADD_GRE_OPTIONS3f};

    if ((udf_num > 1) || (user_num > 3)) {
        FP_ERR(("FP(unit %d) Error: udf_num=%d or user_num=%d out-of-range.\n", 
                unit, udf_num, user_num));
        return BCM_E_PARAM;
    }

    /* Formulate the desired FP_UDF_OFFSET fields */
    f_idx = (udf_num << 2) | user_num;

    for (tbl_idx =  soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); 
         tbl_idx++) { 

        SOC_IF_ERROR_RETURN
          (READ_FP_UDF_OFFSETm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_entry));

        udf_spec->offset[tbl_idx] =
          soc_FP_UDF_OFFSETm_field32_get(unit, &tbl_entry, off_field[f_idx]); 

        /* Get the UDFn_ADD_IPV4_OPTIONSm bit */ 
        if (soc_FP_UDF_OFFSETm_field32_get(unit, &tbl_entry, 
                                           option_field[f_idx])) { 
            udf_spec->offset[tbl_idx] |= BCM_FIELD_USER_OPTION_ADJUST;
        }

        /* Get the UDFn_ADD_GRE_OPTIONSm bit */ 
        if (soc_FP_UDF_OFFSETm_field32_get(unit, &tbl_entry, 
                                           gre_option_field[f_idx])) { 
            udf_spec->offset[tbl_idx] |= BCM_FIELD_USER_GRE_OPTION_ADJUST;
        }
        udf_spec->offset[tbl_idx] |= _BCM_FIELD_USER_OFFSET_VALID;
    }

    return BCM_E_NONE;
}

int
_bcm_field_trx_control_arp_set(int unit,  bcm_field_control_t control, uint32 state)
{
    uint64 regval;
    uint32 enable;

    BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &regval));

    enable = soc_reg64_field32_get(unit, ING_CONFIG_64r, 
                                   regval, ARP_RARP_TO_FPf);

    switch (control) {
      case bcmFieldControlArpAsIp:
          if (state) {
              enable |= (0x1); 
          } else {
              enable &= ~(0x1);
          }
          break;
      case bcmFieldControlRarpAsIp:
          if (state) {
              enable |= (0x2);
          } else {
              enable &= ~(0x2);
          }
          break;
      default:
          return (BCM_E_UNAVAIL);
    }

    soc_reg64_field32_set(unit, ING_CONFIG_64r, &regval, ARP_RARP_TO_FPf, enable);
    BCM_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, regval));
    return (BCM_E_NONE);
}

/*
 * Function: 
 *   _bcm_field_trx_stat_hw_mode_to_bmap
 *
 * Description:
 *      HW counter mode to statistics bitmap. 
 * Parameters:
 *      unit           - (IN) BCM device number.
 *      mode           - (IN) HW counter mode.
 *      stage_id       - (IN) Stage id.
 *      hw_bmap        - (OUT) Statistics bitmap. 
 *      hw_entry_count - (OUT) Number of counters required.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_field_trx_stat_hw_mode_to_bmap(int unit, uint8 mode, 
                                    _field_stage_id_t stage_id, 
                                    uint32 *hw_bmap, uint8 *hw_entry_count)
{
    uint32 value = 0;
    /* Input parameters check. */
    if ((NULL == hw_bmap) || (NULL == hw_entry_count)) {
        return (BCM_E_PARAM);
    }
    switch (mode) {
      case 1:
          value |= (1 << (int)bcmFieldStatPackets);
          value |= (1 << (int)bcmFieldStatBytes);
          *hw_entry_count = 1;
          break;
      case 2:
          if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
              value |= (1 << (int)bcmFieldStatGreenBytes);
              value |= (1 << (int)bcmFieldStatGreenPackets);
          } else {
              value |= (1 << (int)bcmFieldStatPackets);
              value |= (1 << (int)bcmFieldStatBytes);
          } 
          *hw_entry_count = 1;
          break;
      case 3:
          if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
              value |= (1 << (int)bcmFieldStatYellowBytes);
              value |= (1 << (int)bcmFieldStatYellowPackets);
              *hw_entry_count = 1;
          } else {
              value |= (1 << (int)bcmFieldStatRedBytes);
              value |= (1 << (int)bcmFieldStatRedPackets);
              value |= (1 << (int)bcmFieldStatNotRedBytes);
              value |= (1 << (int)bcmFieldStatNotRedPackets);
              *hw_entry_count = 2;
          }
          break;
      case 4:
          if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
              value |= (1 << (int)bcmFieldStatRedBytes);
              value |= (1 << (int)bcmFieldStatRedPackets);
              *hw_entry_count = 1;
          } else {
              value |= (1 << (int)bcmFieldStatGreenBytes);
              value |= (1 << (int)bcmFieldStatGreenPackets);
              value |= (1 << (int)bcmFieldStatNotGreenBytes);
              value |= (1 << (int)bcmFieldStatNotGreenPackets);
              *hw_entry_count = 2;
          }
          break;
      case 5:
          if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
              value |= (1 << (int)bcmFieldStatNotGreenBytes);
              value |= (1 << (int)bcmFieldStatNotGreenPackets);
              *hw_entry_count = 1;
          } else {
              value |= (1 << (int)bcmFieldStatGreenBytes);
              value |= (1 << (int)bcmFieldStatGreenPackets);
              value |= (1 << (int)bcmFieldStatRedBytes);
              value |= (1 << (int)bcmFieldStatRedPackets);
              value |= (1 << (int)bcmFieldStatNotYellowBytes);
              value |= (1 << (int)bcmFieldStatNotYellowPackets);
              *hw_entry_count = 2;
          }
          break;
      case 6:
          if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
              value |= (1 << (int)bcmFieldStatNotYellowBytes);
              value |= (1 << (int)bcmFieldStatNotYellowPackets);
              *hw_entry_count = 1;
          } else {
              value |= (1 << (int)bcmFieldStatGreenBytes);
              value |= (1 << (int)bcmFieldStatGreenPackets);
              value |= (1 << (int)bcmFieldStatYellowBytes);
              value |= (1 << (int)bcmFieldStatYellowPackets);
              value |= (1 << (int)bcmFieldStatNotRedBytes);
              value |= (1 << (int)bcmFieldStatNotRedPackets);
              *hw_entry_count = 2;
          }
          break;
      case 7:
          if (_BCM_FIELD_STAGE_EXTERNAL == stage_id) {
              value |= (1 << (int)bcmFieldStatNotRedBytes);
              value |= (1 << (int)bcmFieldStatNotRedPackets);
              *hw_entry_count = 1;
          } else {
              value |= (1 << (int)bcmFieldStatRedBytes);
              value |= (1 << (int)bcmFieldStatRedPackets);
              value |= (1 << (int)bcmFieldStatYellowBytes);
              value |= (1 << (int)bcmFieldStatYellowPackets);
              value |= (1 << (int)bcmFieldStatNotGreenBytes);
              value |= (1 << (int)bcmFieldStatNotGreenPackets);
              *hw_entry_count = 2;
          }
          break;
      default:
              *hw_entry_count = 0;
    }
    *hw_bmap = value;
    return (BCM_E_NONE);
}

/*
 * Function: 
 *     _field_trx_meter_table_get
 * Purpose:
 *     Get policer table name.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     stage_id  - (IN) Field entry pipeline stage.
 *     mem_x     - (OUT) Policer table name X pipeline.
 *     mem_y     - (OUT) Policer table name Y pipeline.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_meter_table_get(int unit, _field_stage_id_t stage_id,
                           soc_mem_t *mem_x, soc_mem_t *mem_y)
{
    if ((NULL == mem_x) || (NULL == mem_y)) {
        return (BCM_E_PARAM);
    }

    *mem_x = *mem_y = INVALIDm;

    /* Resolve meter table name. */
    switch (stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
      case _BCM_FIELD_STAGE_EXTERNAL:
          *mem_x = FP_METER_TABLEm;
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          if (SOC_IS_SC_CQ(unit)) { 
              *mem_x = EFP_METER_TABLE_Xm;
              *mem_y = EFP_METER_TABLE_Ym;
          } else {
              *mem_x = EFP_METER_TABLEm;
          }
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function: 
 *     _field_trx_policer_hw_update
 * Purpose:
 *     Update (Install) the policer parameters
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     f_ent         - (IN) Field entry policer attached to.
 *     index_mtr     - (IN) Peak/Committed
 *     bucket_size   - (IN) Encoded bucket size.
 *     refresh_rate  - (IN) Tokens refresh rate.
 *     granularity   - (IN) Tokens granularity.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_trx_policer_hw_update(int unit, _field_entry_t *f_ent,
                             _field_policer_t *f_pl, uint8 index_mtr, 
                             uint32 bucket_size, uint32 refresh_rate, 
                             uint32 granularity, soc_mem_t meter_table)
{
    uint32          meter_entry[SOC_MAX_MEM_FIELD_WORDS]; /* HW entry buffer.*/
    int             bucket_cnt_bitsize;      /* Bucket count field bit size. */
    int             bucket_max_bitsize;      /* Bucket size field bit size.  */
    int             meter_hw_idx;            /* Meter table index.           */
    uint32          bucketcount;             /* Bucket count initial value.  */
    int             meter_offset;            /* Peak/Comm meter selection.   */
    _field_stage_t  *stage_fc;               /* Field stage control.         */
    int             rv;                      /* Operation return status.     */

    if ((NULL == f_pl) || (NULL == f_ent) || (INVALIDm == meter_table)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, f_ent->fs->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);


    /* Calculate initial backet count. 
     * bucket size << (bit count diff - 1(sign) -1 (overflow))  - 1
     */
    if (bucket_size) {
        bucket_cnt_bitsize = soc_mem_field_length(unit, meter_table, 
                                                  BUCKETCOUNTf);
        bucket_max_bitsize = soc_mem_field_length(unit, meter_table, 
                                                  BUCKETSIZEf);
        bucketcount =  
            (bucket_size << (bucket_cnt_bitsize - bucket_max_bitsize - 2))  - 1;
        bucketcount &= ((1 << bucket_cnt_bitsize) - 1);
    } else {
        bucketcount = 0;
    }
    
    meter_offset = (BCM_FIELD_METER_PEAK == index_mtr) ? 0 : 1;

    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        /* Hw index is (Pool number * 2 * Pairs in Pool + 2 * Pair number) */
        meter_hw_idx = (2 * f_pl->pool_index *
                        stage_fc->meter_pool[f_pl->pool_index]->size)  +
                        (2 * f_pl->hw_index) + meter_offset;
    } else {
        /* Hw index is (Slice number + 2 * Pair number) */
        meter_hw_idx = stage_fc->slices[f_pl->pool_index].start_tcam_idx + \
                       (2 * f_pl->hw_index) + meter_offset;
    }

    if (meter_hw_idx < soc_mem_index_min(unit, meter_table) ||
        meter_hw_idx > soc_mem_index_max(unit, meter_table)) {
        return (BCM_E_INTERNAL);
    }

    /* Update meter config in hw. */
    rv = soc_mem_read(unit, meter_table, MEM_BLOCK_ANY,
                      meter_hw_idx, &meter_entry);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESHCOUNTf,
                        refresh_rate);
    soc_mem_field32_set(unit, meter_table, &meter_entry, BUCKETSIZEf,
                        bucket_size);
    soc_mem_field32_set(unit, meter_table, &meter_entry, METER_GRANf,
                        granularity);
    soc_mem_field32_set(unit, meter_table, &meter_entry, BUCKETCOUNTf,
                        bucketcount);

    /* Refresh mode is only set to 1 for Single Rate. Other modes get 0 */
    if (f_pl->cfg.mode  == bcmPolicerModeSrTcm) {
        soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESH_MODEf, 1);
    } else if (f_pl->cfg.mode  == bcmPolicerModeCoupledTrTcmDs) {
        soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESH_MODEf, 2);
    } else {
        soc_mem_field32_set(unit, meter_table, &meter_entry, REFRESH_MODEf, 0);
    }

    rv = soc_mem_write(unit, meter_table, MEM_BLOCK_ALL,
                       meter_hw_idx, meter_entry);

    return (rv);
}


/*
 * Function:
 *     _field_trx_default_policer_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     stage_fc  - (IN)Stage control structure. 
 *     level     - (IN)Policer level.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */
STATIC int
_field_trx_default_policer_set(int unit, _field_stage_t *stage_fc,
                               int level, soc_mem_t mem, uint32 *buf)
{
    soc_field_t             pair_mode_field;/* Hw field for pair mode.   */
    soc_field_t             modifier_field; /* Hw field for meter update.*/ 

    /* Input parameter check. */
    if ((NULL == stage_fc) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }

    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        if (level) {
            pair_mode_field = METER_SHARING_MODEf;
            modifier_field = METER_SHARING_MODE_MODIFIERf; 
        } else {
            pair_mode_field = METER_PAIR_MODEf;
            modifier_field = METER_PAIR_MODE_MODIFIERf; 
        }

        /* Set the meter mode to default. */
        soc_mem_field32_set(unit, mem, buf, pair_mode_field, 0);
        /* Preserve packet color. */
        soc_mem_field32_set(unit, mem, buf, modifier_field, 1);
    } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        if (0 == level) {
            /* Set the meter mode to default. */
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 0);
            /* Preserve packet color. */
            soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
        }
    } else {
        return BCM_E_PARAM;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_trx_policer_action_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 */
int
_bcm_field_trx_policer_action_set(int unit, _field_entry_t *f_ent,
                                  soc_mem_t mem, uint32 *buf)
{
    _field_entry_policer_t  *f_ent_pl; /* Field entry policer descriptor.*/
    _field_stage_t          *stage_fc; /* Stage field control structure. */
    bcm_field_stage_t       stage_id;  /* Stage that contains meter.     */
    _field_policer_t        *f_pl;     /* Field policer descriptor.      */
    int                     idx;       /* Policers levels iterator.      */
    uint32                  meter_pair_mode;/* Hw meter usage bits.      */
    soc_field_t             pair_mode_field;/* Hw field for pair mode.   */
    soc_field_t             index_field;    /* Hw field for meter index. */
    soc_field_t             modifier_field; /* Hw field for meter update.*/ 
    int                     meter_pair_idx; /* Meter pair index.         */
    uint32                  meter_merge = 0;/* Meter merge mode.         */
    int                     rv;             /* Operation return status.  */ 


    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }
    
    stage_id = (f_ent->group->stage_id == _BCM_FIELD_STAGE_EXTERNAL) ? 
         _BCM_FIELD_STAGE_INGRESS : f_ent->group->stage_id;

    /* No policers at lookup stage. */
    if (_BCM_FIELD_STAGE_LOOKUP == stage_id) { 
        return (BCM_E_NONE);
    }

    /* Get stage control structure. */
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);
 
    for (idx = 0; idx < _FP_POLICER_LEVEL_COUNT; idx++) {
        f_ent_pl = f_ent->policer + idx;

        if (0 == (f_ent_pl->flags & _FP_POLICER_INSTALLED)) {
            /* Install preserve the color policer. */
            rv = _field_trx_default_policer_set(unit, stage_fc, idx, 
                                                mem, buf);
            BCM_IF_ERROR_RETURN(rv);
            continue;
        }

        /* Get policer config. */
        rv = _bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl);
        BCM_IF_ERROR_RETURN(rv);

        if (0 == f_pl->level) {
            /* Get hw encoding for meter mode. */
            rv = _bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode);
            BCM_IF_ERROR_RETURN(rv);
            /* If level 0 policer is Modified trTcm -> 
             * meter sharing mode is dual. 
             */
            if ((f_pl->cfg.mode == bcmPolicerModeTrTcmDs) ||
                (f_pl->cfg.mode == bcmPolicerModeCoupledTrTcmDs)) {
                meter_merge = 3;
            }
        }

 
       if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
           /* Pair index is (Pool number * Pairs in Pool + Pair number) */
            meter_pair_idx = (f_pl->pool_index * 
                              stage_fc->meter_pool[f_pl->pool_index]->size) + 
                              f_pl->hw_index;
            /* 
             * Get the meter index in the global meter pool 
             * IMP: This is the meter pair index, not the individual meter's
             *     which will be 2x (+1) (see _field_triumph_meter_install)
             */
            if (f_pl->level) {
                pair_mode_field = METER_SHARING_MODEf;
                index_field = SHARED_METER_PAIR_INDEXf;    
                modifier_field = METER_SHARING_MODE_MODIFIERf; 
                meter_pair_mode = (meter_merge) ? meter_merge : 
                    ((f_pl->cfg.flags & BCM_POLICER_COLOR_MERGE_OR) ? 1 : 2);
            } else {
                pair_mode_field = METER_PAIR_MODEf;
                index_field = METER_PAIR_INDEXf;    
                modifier_field = METER_PAIR_MODE_MODIFIERf; 
            }
            soc_mem_field32_set(unit, mem, buf, index_field, meter_pair_idx);

            /* Get the meter mode */
            soc_mem_field32_set(unit, mem, buf, pair_mode_field,
                                meter_pair_mode);
            /*
             * Flow mode cares about the MODIFIER field 
             *     PEAK (Even): 0
             *     COMMITTED/PASS THROUGH (Odd): 1
             */
            if ((f_pl->cfg.mode == bcmPolicerModeCommitted) ||
                (f_pl->cfg.mode == bcmPolicerModePassThrough) ||
                (f_pl->cfg.mode == bcmPolicerModeSrTcmModified)) {
                soc_mem_field32_set(unit, mem, buf, modifier_field, 1);
            } else if (f_pl->cfg.mode == bcmPolicerModePeak){
                soc_mem_field32_set(unit, mem, buf, modifier_field, 0);
            }
        } else if (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            /* Get the even and odd indexes from the entry. The even and odd
             * meter indices are the same.
             */
            soc_mem_field32_set(unit, mem, buf, METER_INDEX_EVENf, 
                                f_pl->hw_index);
            soc_mem_field32_set(unit, mem, buf, METER_INDEX_ODDf, 
                                f_pl->hw_index);

            /* Get the meter mode */
            
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf,
                                meter_pair_mode);

            /* 
             * Flow mode is the only one that cares about the test and update bits.
             * Even = BCM_FIELD_METER_PEAK
             * Odd = BCM_FIELD_METER_COMMITTED
             */
            if (f_pl->cfg.mode == bcmPolicerModePeak) {
                soc_mem_field32_set(unit, mem, buf, METER_TEST_ODDf, 0);
                soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
                soc_mem_field32_set(unit, mem, buf, METER_UPDATE_ODDf, 0);
                soc_mem_field32_set(unit, mem, buf, METER_UPDATE_EVENf, 1);
            } else if (f_pl->cfg.mode == bcmPolicerModeCommitted) {
                soc_mem_field32_set(unit, mem, buf, METER_TEST_ODDf, 1);
                soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 0);
                soc_mem_field32_set(unit, mem, buf, METER_UPDATE_ODDf, 1);
                soc_mem_field32_set(unit, mem, buf, METER_UPDATE_EVENf, 0);
            } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
                soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
            }
        } else {
            return BCM_E_PARAM;
        }
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_trx_policer_install
 * Purpose:
 *     Install a meter pair into the hardware tables.
 * Parameters:
 *     unit   - (IN) BCM device number.
 *     f_ent  - (IN) Field entry.
 *     f_pl   - (IN) Field policer descriptor.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_trx_policer_install(int unit, _field_entry_t *f_ent, 
                               _field_policer_t *f_pl)
{
    uint32    bucketsize = 0;      /* Bucket size.             */
    uint32    refresh_rate = 0;    /* Policer refresh rate.    */
    uint32    granularity = 0;     /* Policer granularity.     */
    int       refresh_bitsize;     /* Number of bits for the 
                                      refresh rate field.      */
    int       bucket_max_bitsize;  /* Number of bits for the 
                                      bucket max field.         */
    int       rv;                  /* Operation return status. */
    soc_mem_t meter_table_x;       /* Meter table name.        */
    soc_mem_t meter_table_y;       /* Meter table name.        */
    uint32    flags;               /* Policer flags.           */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->group || NULL == f_ent->fs) {
        return (BCM_E_INTERNAL);
    }

    if (0 == (f_pl->hw_flags & _FP_POLICER_DIRTY)) {
        return (BCM_E_NONE);
    }

    /* Resolve meter table name. */
    rv = _field_trx_meter_table_get(unit, f_ent->fs->stage_id,
                                    &meter_table_x, &meter_table_y);
    BCM_IF_ERROR_RETURN(rv);

    refresh_bitsize = soc_mem_field_length(unit, meter_table_x, REFRESHCOUNTf);
    bucket_max_bitsize = soc_mem_field_length(unit, meter_table_x, BUCKETSIZEf);

    /* lookup bucket size from tables */
    flags = _BCM_XGS_METER_FLAG_GRANULARITY | _BCM_XGS_METER_FLAG_FP_POLICER;
    if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
        /* Calculate policer bucket size/refresh_rate/granularity. */
        rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec, 
                                               f_pl->cfg.ckbits_burst,
                                               flags, refresh_bitsize,
                                               bucket_max_bitsize,
                                               &refresh_rate, &bucketsize,
                                               &granularity);
        /* Programm policer parameters into hw. */
        rv =  _field_trx_policer_hw_update(unit, f_ent, f_pl,
                                           BCM_FIELD_METER_COMMITTED,
                                           bucketsize, refresh_rate,
                                           granularity, meter_table_x);

        if (BCM_SUCCESS(rv) && (INVALIDm != meter_table_y)) {
            rv =  _field_trx_policer_hw_update(unit, f_ent, f_pl,
                                               BCM_FIELD_METER_COMMITTED,
                                               bucketsize, refresh_rate,
                                               granularity, meter_table_y);
        }
        f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;
    }

    if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
        /* Calculate policer bucket size/refresh_rate/granularity. */
        rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec, 
                                               f_pl->cfg.pkbits_burst,
                                               flags, refresh_bitsize,
                                               bucket_max_bitsize,
                                               &refresh_rate, &bucketsize,
                                               &granularity);
        /* Programm policer parameters into hw. */
        rv =  _field_trx_policer_hw_update(unit, f_ent, f_pl,
                                           BCM_FIELD_METER_PEAK,
                                           bucketsize, refresh_rate,
                                           granularity, meter_table_x);

        if (BCM_SUCCESS(rv) && (INVALIDm != meter_table_y)) {
            rv =  _field_trx_policer_hw_update(unit, f_ent, f_pl,
                                               BCM_FIELD_METER_PEAK,
                                               bucketsize, refresh_rate,
                                               granularity, meter_table_y);
        }
        f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY; 
    }
    return rv;
}

/*
 * Function:
 *     _bcm_field_trx_stat_index_get
 * Purpose:
 *      Get hw indexes and flags needed to compose requested statistic.
 *     
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   f_st          - (IN)  Field statistics entity.
 *   stat          - (IN)  Counter type.
 *   idx1          - (OUT)  Primary counter index.
 *   idx2          - (OUT)  Secondary counter index.
 *   out_flags     - (OUT)  Counter flags.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_trx_stat_index_get(int unit, _field_stat_t *f_st, 
                             bcm_field_stat_t stat,
                             int *idx1, int *idx2, uint32 *out_flags)
{
    _field_stage_t *stage_fc;        /* Stage field control structure. */
    int            rv;               /* Operation return status.       */
    uint32         flags = 0;        /* _FP_STAT_XXX flags.            */
    uint32         counter_tbl_idx;  /* Counter table index.           */
    int            index1  = _FP_INVALID_INDEX;
    int            index2  = _FP_INVALID_INDEX;

    /* Input parameters check. */
    if ((NULL == idx1) || (NULL == idx2) || 
        (NULL == out_flags) || (NULL == f_st)) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    *idx1 = *idx2 = _FP_INVALID_INDEX;

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_TRIUMPH_SUPPORT)
    /* External stage counter index translation. */
    if (_BCM_FIELD_STAGE_EXTERNAL == f_st->stage_id) {
        rv = _bcm_field_tr_external_counter_idx_get(unit, f_st->pool_index,
                                                    f_st->hw_index, idx1);
        switch (stat) {
          case bcmFieldStatBytes:
          case bcmFieldStatGreenBytes:
          case bcmFieldStatYellowBytes:
          case bcmFieldStatRedBytes:
          case bcmFieldStatNotGreenBytes:
          case bcmFieldStatNotYellowBytes:
          case bcmFieldStatNotRedBytes:
              flags |= _FP_STAT_BYTES;
              break;
          default:
              break;
        }
        *out_flags = flags;
        return (rv);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    /* Stat entity indexes are adjusted for policy table counter pairs. */
    counter_tbl_idx = f_st->hw_index << 1;

    switch (stat) {
      case _bcmFieldStatBytesEven:
          flags |= _FP_STAT_BYTES;
      case _bcmFieldStatPacketsEven:
          index1 = counter_tbl_idx; 
          break;
      case _bcmFieldStatBytesOdd:
          flags |= _FP_STAT_BYTES;
      case _bcmFieldStatPacketsOdd:
          index1 = counter_tbl_idx + 1; 
          break;
      case bcmFieldStatBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatPackets:
          switch (f_st->hw_mode) {
            case (0x2):
                index1 = counter_tbl_idx + 1;
                break;
            case (0x1):
                index1 = counter_tbl_idx; 
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatGreenBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatGreenPackets:
          index1 = counter_tbl_idx + 1; 
          break;

      case bcmFieldStatYellowBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatYellowPackets:
          index1 = counter_tbl_idx;  
          break;

      case bcmFieldStatRedBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatRedPackets:
          switch (f_st->hw_mode) {
            case 0x3:
            case 0x7:
                index1 = counter_tbl_idx + 1;  
                break;
            case 0x5:
                index1 = counter_tbl_idx;  
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;

      case bcmFieldStatNotGreenBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatNotGreenPackets:
          switch (f_st->hw_mode) {
            case 0x4:
                index1 = counter_tbl_idx;  
                break;
            case 0x7:
                index1 = counter_tbl_idx;  
                index2 = counter_tbl_idx + 1;  
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;

      case bcmFieldStatNotYellowBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatNotYellowPackets:
          switch (f_st->hw_mode) {
            case 0x5:
                index1 = counter_tbl_idx;  
                index2 = counter_tbl_idx + 1;  
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;

      case bcmFieldStatNotRedBytes:
          flags |= _FP_STAT_BYTES;
      case bcmFieldStatNotRedPackets:
          switch (f_st->hw_mode) {
            case (0x3):
                index1 = counter_tbl_idx;
                break;
            case (0x6):
                index1 = counter_tbl_idx;
                index2 = counter_tbl_idx + 1;
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      default:
          return (BCM_E_INTERNAL);
    }


    /* Calculate  counter table entry index. */
    if (_FP_INVALID_INDEX != index1) {
        if (stage_fc->flags & _FP_STAGE_GLOBAL_COUNTERS) {
            *idx1 = index1;
        } else {
            *idx1 = stage_fc->slices[f_st->pool_index].start_tcam_idx + index1;
        }
    } else {
        *idx1 = _FP_INVALID_INDEX;
    }

    if (_FP_INVALID_INDEX != index2) {
        if (stage_fc->flags & _FP_STAGE_GLOBAL_COUNTERS) {
            *idx2 = index2;
        } else {
            *idx2 = stage_fc->slices[f_st->pool_index].start_tcam_idx + index2;
        }
    } else {
        *idx2 = _FP_INVALID_INDEX;
    }
    *out_flags = flags;
    return (BCM_E_NONE);
}
#else /* BCM_TRX_SUPPORT && BCM_FIELD_SUPPORT */
int _trx_field_not_empty;
#endif  /* BCM_TRX_SUPPORT && BCM_FIELD_SUPPORT */
