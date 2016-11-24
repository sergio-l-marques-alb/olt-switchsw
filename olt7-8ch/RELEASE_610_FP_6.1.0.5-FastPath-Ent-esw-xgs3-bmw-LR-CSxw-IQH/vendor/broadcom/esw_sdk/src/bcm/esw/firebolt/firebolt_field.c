/* $Id: firebolt_field.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * Purpose:     BCM56504 Field Processor installation functions.
 */
#include <soc/mem.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/mirror.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/mirror.h>

#if (defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)) \
     && defined(BCM_FIELD_SUPPORT)

/* local/static function prototypes */
STATIC void _field_fb_functions_init(int unit, _field_funct_t *functions);
STATIC int _field_fb_detach(int unit, _field_control_t *fc);
STATIC int _field_fb_hw_clear(int unit, _field_stage_t *stage_fc);

STATIC int _field_fb_tcam_policy_clear(int unit, _field_stage_id_t stage_id,
                                       int tcam_idx);
STATIC int _field_fb_tcam_policy_install(int unit, _field_entry_t *f_ent,
                                         int tcam_idx);
STATIC int _field_fb_tcam_get(int unit, _field_entry_t *f_ent,
                               soc_mem_t mem, uint32 *buf);
STATIC int _field_fb_action_get(int unit, soc_mem_t mem, 
                                _field_entry_t *f_ent, int tcam_idx, 
                                _field_action_t *fa, uint32 *buf);
int _bcm_field_fb_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx);

#ifdef BROADCOM_DEBUG
STATIC char *_field_fb_action_name(bcm_field_action_t action);
#endif /* BROADCOM_DEBUG */

static soc_field_t _fb_ing_f4_reg[16] =   {
    SLICE_0_F4f, SLICE_1_F4f,
    SLICE_2_F4f, SLICE_3_F4f,
    SLICE_4_F4f, SLICE_5_F4f,
    SLICE_6_F4f, SLICE_7_F4f,
    SLICE_8_F4f, SLICE_9_F4f,
    SLICE_10_F4f, SLICE_11_F4f,
    SLICE_12_F4f, SLICE_13_F4f,
    SLICE_14_F4f, SLICE_15_F4f};

#if defined(BCM_FIREBOLT2_SUPPORT) 
static soc_field_t _fb2_ifp_double_wide_key[16] = {
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
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

STATIC soc_field_t _fb_field_tbl[16][3] = {
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
static soc_field_t _fb_ing_slice_mode_field[16] = {
    SLICE_0_MODEf,   SLICE_1_MODEf,
    SLICE_2_MODEf,   SLICE_3_MODEf,
    SLICE_4_MODEf,   SLICE_5_MODEf,
    SLICE_6_MODEf,   SLICE_7_MODEf,
    SLICE_8_MODEf,   SLICE_9_MODEf,
    SLICE_10_MODEf,  SLICE_11_MODEf,
    SLICE_12_MODEf,  SLICE_13_MODEf,
    SLICE_14_MODEf,  SLICE_15_MODEf};
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
static soc_field_t _fb2_slice_pairing_field[8] = {
    SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf,
    SLICE5_4_PAIRINGf,   SLICE7_6_PAIRINGf,
    SLICE9_8_PAIRINGf,   SLICE11_10_PAIRINGf,
    SLICE13_12_PAIRINGf, SLICE15_14_PAIRINGf};
static soc_field_t _fb2_slice_wide_mode_field[16] = {
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
static soc_field_t _vfp_slice_wide_mode_field[4] = {
    SLICE_0_DOUBLE_WIDE_MODEf,
    SLICE_1_DOUBLE_WIDE_MODEf,
    SLICE_2_DOUBLE_WIDE_MODEf,
    SLICE_3_DOUBLE_WIDE_MODEf};
static soc_field_t _vfp_slice_pairing_field[2] = {
    SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf};
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#if defined (BCM_FIREBOLT2_SUPPORT)
static soc_field_t _efp_slice_mode[] = {
    SLICE_0_MODEf, SLICE_1_MODEf,
    SLICE_2_MODEf, SLICE_3_MODEf};
static soc_field_t _vfp_field_tbl[4][2] = {
    {SLICE_0_F2f, SLICE_0_F3f},
    {SLICE_1_F2f, SLICE_1_F3f},
    {SLICE_2_F2f, SLICE_2_F3f},
    {SLICE_3_F2f, SLICE_3_F3f}};
#endif /* BCM_FIREBOLT2_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_field_fb_slice_reinit(int unit, fp_port_field_sel_entry_t *pfs_entry, 
		       int slice_numb, _field_sel_t *selcodes)
{
    

    soc_field_t f1_field, f2_field, f3_field;	
    f1_field = _fb_field_tbl[slice_numb][0];
    selcodes->fpf1 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f1_field);

    f2_field = _fb_field_tbl[slice_numb][1];
    selcodes->fpf2 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f2_field);

    f3_field = _fb_field_tbl[slice_numb][2];
    selcodes->fpf3 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f3_field);
    return BCM_E_NONE;    
}
#else
#define _field_fb_slice_reinit (NULL)
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     _field_fb_tcam_policy_mem_get
 *
 * Purpose:
 *     Get tcam & policy memories for a specific chip pipeline stage.
 *
 * Parameters:
 *     unit       -   (IN)BCM device number.
 *     stage_id   -   (IN)FP stage pipeline id. 
 *     tcam_mem   -   (IN)Tcam  memory id.
 *     policy_mem -   (IN)Policy memory id.
 *     
 * Returns:
 *     BCM_E_XXX
 */
int
_field_fb_tcam_policy_mem_get(int unit, _field_stage_id_t stage_id, 
                              soc_mem_t *tcam_mem, soc_mem_t *policy_mem)
{

    /* Input parameters check. */
    if ((NULL == tcam_mem) || (NULL == policy_mem)) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_id) {
        *tcam_mem = FP_TCAMm;
        *policy_mem= FP_POLICY_TABLEm;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    } else if (soc_feature(unit, soc_feature_field_multi_stage)) {
        if (_BCM_FIELD_STAGE_EGRESS == stage_id) {
            *tcam_mem = EFP_TCAMm;
            *policy_mem = EFP_POLICY_TABLEm;
        } else if (_BCM_FIELD_STAGE_LOOKUP== stage_id) {
            *tcam_mem = VFP_TCAMm;
            *policy_mem = VFP_POLICY_TABLEm;
        } else {
            /* Unknown stage specified */
            (*policy_mem) = (*tcam_mem) = INVALIDm; 
            return (BCM_E_PARAM);
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    } else {
        /* Not ingress stage specified while multi stage is not enabled */
        (*policy_mem) = (*tcam_mem) = INVALIDm; 
        return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

#ifdef BCM_FIREBOLT2_SUPPORT 
/*
 * Function:
 *     _field_fb2_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int           rv;              /* Operation return status.  */
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageLookup,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /*
     *  Enable the overlay of Sender Ethernet Address onto 
     *  MACSA on ARP/RARP packets.
     */
    rv = soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                ARP_VALIDATION_ENf, 1);
    BCM_IF_ERROR_RETURN(rv);

    /* FPF3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 0, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 0, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 0, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, 8, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 1, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 1, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 1, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 1, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 1, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, 1, 8, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 2, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 2, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 2, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 2, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 2, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, 2, 8, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 3, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 3, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 3, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 3, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 8, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, 12, 12);

    /* FPF2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, 32, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 0, 38, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 0, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, 64, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, 80, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, 88, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, 120, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 1, 24, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 2, 24, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, 56, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, 104, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 4, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 4, 40, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 4, 72, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, 104, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 5, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 5, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 5, 40, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 5, 72, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 104, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 6, 24, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 6, 88, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, 88, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, 128, 24);
    /* FPF1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                 _bcmFieldSliceSelFpf1, 0, 152, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, 155, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, 155, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, 156, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, 159, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, 171, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf1, 0, 173, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf1, 0, 178, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf1, 0, 182, 2);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     0, 8, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     8, 16, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     24, 8, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     32, 6, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     38, 2, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     64, 16, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     80, 8, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     88, 32, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     120, 32, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     152, 16, 0, 0, 0, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     0, 8, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     8, 16, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     24, 128, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     152, 16, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     168, 8, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     176, 8, 0, 0, 0, 0);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb2_egress_qualifiers_init
 * Purpose:
 *     Initialize device stage egress qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 2, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 61, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 69, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 101, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 133, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 134, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 146, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 152, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 153, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 153, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 166, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 172, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 176, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 8, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 152, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 153, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 153, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 166, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 172, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 176, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 2, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 48, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 2, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 66, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 114, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 114, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 126, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 127, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 146, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 152, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 153, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 153, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 172, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 176, 5);
    return (BCM_E_NONE);
}
#endif /*BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _field_fb_ingress_qualifiers_init
 * Purpose:
 *     Initialize device stage ingress qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t     tcam_mem;        /* TCAM memory id.           */
    soc_mem_t     policy_mem;      /* Policy table memory id .  */
    int           offset;          /* Tcam field offset.        */
    int           rv;              /* Operation return status.  */
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    rv = _field_fb_tcam_policy_mem_get(unit, stage_fc->stage_id, 
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    _key_fld_ = F4f;
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf4, 0, 0, 5);

    if (soc_feature(unit, soc_feature_field_qual_drop)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf4, 1, 0, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                     _bcmFieldSliceSelFpf4, 1, 2, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelFpf4, 1, 4, 1);
    }

    _key_fld_ = KEYf;
    offset = 0;

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelDisable, 0, 0, 0);

    }


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 0, 2, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, 0, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 0, 6, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 0, 9, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 0, 13, 3);

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf3, 1, 0, 14);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                     _bcmFieldSliceSelFpf3, 1, 6, 8);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 1, 14, 3);
        }
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf3, 1, 0, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                     _bcmFieldSliceSelFpf3, 1, 6, 6);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 1, 12, 3);
        }
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortTgid,
                 _bcmFieldSliceSelFpf3, 1, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 1, 0, 8);


#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf3, 2, 0, 14);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                     _bcmFieldSliceSelFpf3, 2, 6, 8);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 2, 14, 3);
        }
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf3, 2, 0, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                     _bcmFieldSliceSelFpf3, 2, 6, 6);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 2, 12, 3);
        }
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortTgid,
                 _bcmFieldSliceSelFpf3, 2, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 2, 0, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf3, 3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 3, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 3, 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 3, 3, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 3, 5, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 3, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 3, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, 3, 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 3, 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 3, 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 3, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 3, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 3, 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 3, 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 3, 15, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLookupStatus,
                 _bcmFieldSliceSelFpf3, 3, 0, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, 4, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, 4, 6, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 13, 3);

    if (soc_feature(unit, soc_feature_field_wide)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf3, 6, 0, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf3, 6, 8, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelFpf3, 7, 0, 16);
    } 

    /* FPF2 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F3f);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 0, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 1, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 1, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf2, 1, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 2, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 2, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 2, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 2, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 2, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf2, 2, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 2, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 2, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 2, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 3, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 4, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 5, offset, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 5, (offset + 6), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 5, (offset + 14), 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 5, (offset + 34), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 5, (offset + 42), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 5, (offset + 50), 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, (offset + 32), 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 6, (offset + 80), 48);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 7, (offset + 32), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 7, (offset + 64), 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 8, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 8, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 8, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 8, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 8, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 8, (offset + 32), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 8, (offset + 64), 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 9, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 0xA, offset, 128);

    if (soc_feature(unit, soc_feature_field_qual_Ip6High)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                     _bcmFieldSliceSelFpf2, 0xb, offset, 64);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                     _bcmFieldSliceSelFpf2, 0xb, offset, 64);
    }


#ifdef BCM_FIREBOLT2_SUPPORT
    if (SOC_IS_FIREBOLT2(unit)) { 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xc, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 96), 32);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xd, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 96), 32);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xe, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 96), 32);

    }

#endif /* BCM_FIREBOLT2_SUPPORT */

    /* FPF1 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F2f);

#if defined (BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                     _bcmFieldSliceSelFpf1, 0, offset, 21);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf1, 0, (offset + 21), 2);
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                     _bcmFieldSliceSelFpf1, 0, offset, 29);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                         _bcmFieldSliceSelFpf1, 0, (offset + 29), 2);

        }
#ifdef BCM_FIREBOLT2_SUPPORT
        if (SOC_IS_FIREBOLT2(unit)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                         _bcmFieldSliceSelFpf1, 0, (offset + 31), 1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
    }

#if defined (BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortTgid,
                     _bcmFieldSliceSelFpf1, 1, offset, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldSliceSelFpf1, 1, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf1, 1, offset, 14);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                     _bcmFieldSliceSelFpf1, 1, (offset + 6), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldSliceSelFpf1, 1, (offset + 14), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf1, 1, (offset + 14), 14);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortTgid,
                     _bcmFieldSliceSelFpf1, 1, (offset + 14), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                     _bcmFieldSliceSelFpf1, 1, (offset + 20), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                     _bcmFieldSliceSelFpf1, 1, (offset + 28), 3);
    } else 
#endif /* BCM_BRADLEY_SUPPORT */

    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortTgid,
                     _bcmFieldSliceSelFpf1, 1, offset, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldSliceSelFpf1, 1, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf1, 1, offset, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                     _bcmFieldSliceSelFpf1, 1, (offset + 6), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldSliceSelFpf1, 1, (offset + 12), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf1, 1, (offset + 12), 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortTgid,
                     _bcmFieldSliceSelFpf1, 1, (offset + 12), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                     _bcmFieldSliceSelFpf1, 1, (offset + 18), 6);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf1, 1, (offset + 24), 3);
        }
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf1, 2, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf1, 2, (offset + 16), 16);

#if defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 16, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 12, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 12), 1, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 13), 3, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 6, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyLookupClass0,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 6), 5, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMacGroup,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 11), 5, 0, 0, 0, 0);
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        if (soc_feature(unit, soc_feature_src_mac_group)) {
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             offset, 16, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             offset, 12, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 12), 1, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 13), 3, 0, 0, 0, 0);

            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMacGroup,
                             _bcmFieldDevSelInnerVlanOverlay, 1,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 12), 4, 0, 0, 0, 0);
        } else {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldSliceSelFpf1, 3, offset, 16);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldSliceSelFpf1, 3, offset, 12);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldSliceSelFpf1, 3, (offset + 12), 1);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldSliceSelFpf1, 3, (offset + 13), 3);

        }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, (offset + 16), 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, (offset + 28), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, (offset + 29), 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, (offset + 16), 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 5, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 5, (offset + 8), 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 6, offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 1), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 6, (offset + 2), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 6, (offset + 3), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 5), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 6, (offset + 6), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 7), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf1, 6, (offset + 8), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 9), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 10), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 11), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 13), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 6, (offset + 14), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 6, (offset + 15), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLookupStatus,
                 _bcmFieldSliceSelFpf1, 6, offset, 16);
#if defined(BCM_FIREBOLT2_SUPPORT) 
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 16, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 12, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 28), 1, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 29), 3, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 6, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyLookupClass0,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 22), 5, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMacGroup,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 27), 5, 0, 0, 0, 0);
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        if (soc_feature(unit, soc_feature_src_mac_group)) {
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 16), 16, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 16), 12, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 28), 1, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 29), 3, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMacGroup,
                             _bcmFieldDevSelInnerVlanOverlay, 1,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 28), 4, 0, 0, 0, 0);
        } else {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldSliceSelFpf1, 6, (offset + 16), 16);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldSliceSelFpf1, 6, (offset + 16), 12);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldSliceSelFpf1, 6, (offset + 28), 1);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldSliceSelFpf1, 6, (offset + 29), 3);
        }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 7, (offset + 16), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 7, (offset + 18), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf1, 7, (offset + 20), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 7, (offset + 22), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 7, (offset + 25), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 7, (offset + 29), 3);


#ifdef BCM_FIREBOLT2_SUPPORT
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelFpf1, 0xc, offset, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelFpf1, 0xc, offset, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 12), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 13), 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 16), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLookupClass0,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 22), 5);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 29), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 31), 1);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf1, 0xd, offset , 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 8) , 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLookupClass0,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 14) , 5);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 21) , 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 23) , 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 24) , 8);

        /* Intraslice. */
        _key_fld_ = DATAf;

        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 8, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 16, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 24, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 32, 6);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 38, 2);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyTunnelTerminated,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 40, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyVlanTranslationHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 41, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyForwardingVlanValid,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 42, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyIngressStpState,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 43, 2);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 45, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 46, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 47, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 48, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 49, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL3SrcHostHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 50, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL3DestHostHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 51, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyIpmcStarGroupHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 52, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL3DestRouteHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 53, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL2StationMove,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 54, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 55, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyLookupStatus,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 40, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 56, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 56, 12);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 68, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 69, 3);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 72, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 88, 32);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 120, 32);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 152, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 168, 16);

        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 6, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 8, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 16, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 24, 128);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 152, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 168, 16);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_qualifiers_init
 * Purpose:
 *     Initialize device qaualifiers select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_fb_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int rv;           /* Operation return status. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr, 
                   (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)), 
                   "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }
    
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_fb_ingress_qualifiers_init(unit, stage_fc);
          break;
#if defined (BCM_FIREBOLT2_SUPPORT)
      case _BCM_FIELD_STAGE_LOOKUP:
          if (SOC_IS_FIREBOLT2(unit)) {
              rv = _field_fb2_lookup_qualifiers_init(unit, stage_fc);
          } else {
              rv = (BCM_E_NONE);
          } 
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          if (SOC_IS_FIREBOLT2(unit)) {
              rv = _field_fb2_egress_qualifiers_init(unit, stage_fc);
          } else {
              rv = (BCM_E_NONE);
          } 
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _field_fb_init
 * Purpose:
 *     Perform initializations that are specific to BCM56504. This
 *     includes initializing the FP field select bit offset tables for FPF[1-3]
 *     for every stage. 
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fc         - (IN) Field Processor control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_fb_init(int unit, _field_control_t *fc) 
{
    _field_stage_t *stage_p; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_p = fc->stages;
    while (stage_p) {
        /* Clear hardware table */
        BCM_IF_ERROR_RETURN(_field_fb_hw_clear(unit, stage_p));

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_fb_qualifiers_init(unit, stage_p));

        /* Goto next stage */
        stage_p = stage_p->next;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Enable meter refresh */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
        if (SOC_IS_FIREBOLT(unit) && 
            !soc_feature(unit, soc_feature_field_wide)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_CAM_CONTROL_UPPERr,
                                                       REG_PORT_ANY, 
                                                       SECRET_CHAIN_MODEf,1));
        }
    }

    /* Initialize the function pointers */
    _field_fb_functions_init(unit, &fc->functions);;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_hw_clear
 * Purpose:
 *     Clear hardware memory of requested stage. 
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_fb_hw_clear(int unit, _field_stage_t *stage_fc)
{
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_UDF_OFFSETm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_RANGE_CHECKm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_TCAMm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_POLICY_TABLEm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_METER_TABLEm, COPYNO_ALL, TRUE));  
        if (SOC_MEM_IS_VALID(unit, FP_COUNTER_TABLEm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), FP_COUNTER_TABLEm, COPYNO_ALL, TRUE));
        }  
#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, FP_COUNTER_TABLE_Xm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), FP_COUNTER_TABLE_Xm, COPYNO_ALL, TRUE));
        }  
        if (SOC_MEM_IS_VALID(unit, FP_COUNTER_TABLE_Ym)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), FP_COUNTER_TABLE_Ym, COPYNO_ALL, TRUE));
        }
#endif  /* BCM_SCORPION_SUPPORT */
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));  
        if (SOC_MEM_IS_VALID(unit, IFP_PORT_FIELD_SELm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, IFP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));
        } 
#if defined(BCM_TRX_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_RAPTOR_SUPPORT) 
        if (SOC_MEM_IS_VALID(unit, FP_SLICE_MAPm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, FP_SLICE_MAPm, COPYNO_ALL, TRUE));
        }
#endif /* RAPTOR || TRX || FB2 */
#ifdef BCM_FIREBOLT2_SUPPORT 
    } else if (soc_feature(unit, soc_feature_field_multi_stage)) {

        if (_BCM_FIELD_STAGE_LOOKUP == stage_fc->stage_id) { 
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), VFP_TCAMm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), VFP_POLICY_TABLEm, COPYNO_ALL, TRUE));  
        
        } else if (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id) {

            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_TCAMm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_POLICY_TABLEm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_METER_TABLEm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_COUNTER_TABLEm, COPYNO_ALL, TRUE));  
        }
#endif /*  BCM_FIREBOLT2_SUPPORT  */
    } else {
        return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_detach
 *
 * Purpose:
 *     Deallocates BCM5650x's field tables.
 *
 * Parameters:
 *     unit - BCM device number
 *
 * Returns:
 *     BCM_E_NONE
 *
 */
STATIC int
_field_fb_detach(int unit, _field_control_t *fc)
{
    _field_stage_t   *stage_p;  /* Stage iteration pointer. */

    stage_p = fc->stages;

    while (stage_p) { 
       /* Clear the hardware tables */
        BCM_IF_ERROR_RETURN(_field_fb_hw_clear(unit, stage_p));
      
        /* Goto next stage */
        stage_p = stage_p->next; 
    }
    
    /* Clear udf match criteria registers. */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ipprotocol_delete_all(unit));
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ethertype_delete_all(unit));

    /* Clear the Filter Enable flags in the port table */
    BCM_IF_ERROR_RETURN (_field_port_filter_enable_set(unit, fc, FALSE));

    BCM_IF_ERROR_RETURN (_field_meter_refresh_enable_set(unit, fc, FALSE));
 
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_udf_spec_set
 * Purpose:
 *     Write user defined field configuration to udf_spec_t structure
 * Parameters:
 *     
 * Returns:
 *     BCM_E_PARAM - flags or offset out of range
 *     BCM_E_NONE
 */
int
_bcm_field_fb_udf_spec_set(int unit, bcm_field_udf_spec_t *udf_spec, 
                          uint32 flags, uint32 offset)
{
    uint8               pkt_fmt_idx = 0u;     /* index into udf_spec */

    if (offset > 31) {
        FP_ERR(("FP(unit %d) Error: offset=%d out-of-range.\n", unit, offset));
        return BCM_E_PARAM;
    }
    else if ((offset >= 29) && (flags & BCM_FIELD_USER_HIGIG) &&
             !soc_feature(unit, soc_feature_field_udf_higig)) {
        FP_ERR(("FP(unit %d) Error: device does not support UDF HiGig data.\n", unit));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_field_trans_flags_to_index(unit, flags, &pkt_fmt_idx));

    udf_spec->offset[pkt_fmt_idx] = offset;
    if (flags & BCM_FIELD_USER_OPTION_ADJUST) {
        udf_spec->offset[pkt_fmt_idx] |= BCM_FIELD_USER_OPTION_ADJUST;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_fb_udf_spec_get
 * Purpose:
 *     Get user defined field configuration entry from udf_spec_t structure
 * Parameters:
 *     
 * Returns:
 *     BCM_E_PARAM - flags or offset out of range
 *     BCM_E_NONE
 */
int
_bcm_field_fb_udf_spec_get(int unit, bcm_field_udf_spec_t *udf_spec, 
                          uint32 *flags, uint32 *offset)
{
    uint8               pkt_fmt_idx = 0u;     /* index into udf_spec */

    BCM_IF_ERROR_RETURN(_field_trans_flags_to_index(unit, *flags,
                                                    &pkt_fmt_idx));

    if ((*offset = udf_spec->offset[pkt_fmt_idx] & ~(BCM_FIELD_USER_OPTION_ADJUST)) > 31) {
        FP_ERR(("FP(unit %d) Error: offset=%d out-of-range.\n", unit, *offset));
        return BCM_E_PARAM;
    }
    if (udf_spec->offset[pkt_fmt_idx] & BCM_FIELD_USER_OPTION_ADJUST) {
        *flags |= BCM_FIELD_USER_OPTION_ADJUST;
    }
    else {
        *flags &= ~(BCM_FIELD_USER_OPTION_ADJUST);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_fb_udf_write
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
_bcm_field_fb_udf_write(int unit, bcm_field_udf_spec_t *udf_spec, 
                       uint32 udf_num, uint32 user_num)
{
    uint8               tbl_idx = 0u;         /* index into H/W table */
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
        offset = udf_spec->offset[tbl_idx] & ~(BCM_FIELD_USER_OPTION_ADJUST);
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

        SOC_IF_ERROR_RETURN
          (WRITE_FP_UDF_OFFSETm(unit, MEM_BLOCK_ALL, tbl_idx, &tbl_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_fb_udf_read
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
_bcm_field_fb_udf_read(int unit, bcm_field_udf_spec_t *udf_spec, 
                      uint32 udf_num, uint32 user_num)
{
    uint8               tbl_idx = 0u;         /* index into H/W table */
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
    }

    return BCM_E_NONE;
}

#ifdef BROADCOM_DEBUG
/*
 * Function:
 *     _field_fb_action_name
 * Purpose:
 *     Return text name of indicated action enum value.
 */
STATIC char *
_field_fb_action_name(bcm_field_action_t action)
{
    /* Text names of Actions. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_action_t enum order.
     */
    static char *action_text[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;

    return (action >= bcmFieldActionCount ? "??" : action_text[action]);
}

#endif /* BROADCOM_DEBUG*/

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function:
 *     _field_fb_egress_selcode_get
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
STATIC int
_field_fb_egress_selcode_get(int unit, _field_stage_t *stage_fc, 
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
        /* IPv6 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY3,
                                           _BCM_FIELD_EFP_KEY2, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* L2 + L3 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY1,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }
    }
    return (BCM_E_RESOURCE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _bcm_field_fb_group_inner_vlan_overlay_get
 * Purpose:
 *     Returns inner vlan overlay for a group.
 * Parameters:
 *     uint    - (IN) BCM unit number.
 *     value   - (OUT) Inner vlan overlay value.
 * 
 * Returns:
 *     BCM_E_XXX
 * Note:
 */
int
_bcm_field_fb_group_inner_vlan_overlay_get (int unit, int *value)
{
    _field_control_t *fc;    /* field control structure. */
    _field_group_t   *fg;    /* field group structure.   */

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    /* Get field control strucutre.*/
    BCM_IF_ERROR_RETURN(_field_control_get (unit, &fc));

    /* Iterate over all the groups to check for conflicts. */
    fg = fc->groups;

    while (NULL != fg) {
        if ((fg->stage_id == _BCM_FIELD_STAGE_INGRESS) && 
            (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].inner_vlan_overlay)) { 
            *value = fg->sel_codes[0].inner_vlan_overlay;
            break;
        }
        fg = fg->next;
    }

    if (NULL == fg) {
        *value = _FP_SELCODE_DONT_CARE;
    }
    return (BCM_E_NONE);
}
#if defined (BCM_FIREBOLT2_SUPPORT)
/*
 * Function:
 *     _bcm_field_vfp_doublewide_key_select_get
 * Purpose:
 *     Returns vfp doublewide key selection on FB2
 * Parameters:
 *     uint    - (IN) BCM unit number.
 *     value   - (OUT) Inner vlan overlay value.
 * 
 * Returns:
 *     BCM_E_RESOURCE - Conflict observed inside the group.
 *     BCM_E_NONE     - otherwise
 * Note:
 */
int
_bcm_field_vfp_doublewide_key_select_get(int unit, int *value)
{
    _field_control_t *fc;    /* field control structure. */
    _field_group_t   *fg;    /* field group structure.   */

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    if (0 == SOC_IS_FIREBOLT2(unit)) {
        *value = _FP_SELCODE_DONT_CARE; 
        return (BCM_E_NONE);
    }

    /* Get field control strucutre.*/
    BCM_IF_ERROR_RETURN(_field_control_get (unit, &fc));

    /* Iterate over all the groups to check for conflicts. */
    fg = fc->groups;

    while (NULL != fg) {
        if ((fg->stage_id == _BCM_FIELD_STAGE_LOOKUP) && 
            (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) &&
            (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].intraslice_vfp_sel)) { 
            *value = fg->sel_codes[0].intraslice_vfp_sel;
            break;
        }
        fg = fg->next;
    }

    if (NULL == fg) {
        *value = _FP_SELCODE_DONT_CARE;
    }
    return (BCM_E_NONE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _field_fb_tcam_policy_clear
 *
 * Purpose:
 *     Writes a null entry into the TCAM plus Policy table.
 *
 * Parameters:
 *     unit      -   (IN)BCM device number.
 *     stage_id  -   (IN)FP stage pipeline id. 
 *     tcam_idx  -   (IN)Entry tcam index.
 *     
 * Returns:
 *     BCM_E_XXX
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_tcam_policy_clear(int unit, _field_stage_id_t stage_id, int tcam_idx)
{
    soc_mem_t tcam_mem;                 /* TCAM memory id.                    */
    soc_mem_t policy_mem;               /* Policy table memory id .           */

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, stage_id, &tcam_mem, &policy_mem));
    
    /* Memory index sanity check. */
    if (tcam_idx > soc_mem_index_max(unit, tcam_mem)) {
        return (BCM_E_PARAM);
    }  

    /* Write the TCAM & Policy Tables */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx,
                       soc_mem_entry_null(unit, tcam_mem)));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx,
                       soc_mem_entry_null(unit, policy_mem)));
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
        if ((SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAMm)) && 
            (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            SOC_IF_ERROR_RETURN (soc_mem_write(unit, FP_GLOBAL_MASK_TCAMm,
                                               MEM_BLOCK_ALL, tcam_idx,
                                               soc_mem_entry_null(unit, FP_GLOBAL_MASK_TCAMm)));
        }
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
        if ((SOC_IS_SC_CQ(unit)) && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, FP_TCAM_Ym,
                               MEM_BLOCK_ALL, tcam_idx,
                               soc_mem_entry_null(unit, FP_TCAM_Ym)));
        }
#endif /* BCM_SCORPION_SUPPORT */

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_fb_counter_adjust_wide_mode
 * Purpose:
 *     For wide mode entries, 
 *     if there is movement across slices,
 *     handle the counter mode and index
 * Parameters:
 *     unit - BCM unit
 *     policy_mem - policy memory
 *     f_st - Statistics collection entity. 
 *     f_ent_pri - PRI entry
 *     f_ent_sec - SEC entry
 *     p_entry_pri - policy table for PRI entry
 *     p_entry_sec - policy table for SEC entry
 * Returns:
 *     none
 * Notes:
 *     FOUR cases are to be handled 
 *     1. previously counter in PRI slice, now in PRI slice
 *     2. previously counter in PRI slice, now in SEC slice
 *     3. previously counter in SEC slice, now in PRI slice
 *     4. previously counter in SEC slice, now in SEC slice
 *     
 */
void
_bcm_field_fb_counter_adjust_wide_mode(int unit, 
                                   soc_mem_t policy_mem,
                                   _field_stat_t *f_st,
                                   _field_entry_t *f_ent_pri, 
                                   _field_entry_t *f_ent_sec, 
                                   uint32 p_entry_pri[],
                                   uint32 p_entry_sec[])
{
    int mode;

    if ((mode = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, 
                                    COUNTER_MODEf)) != 0) {
        if (f_ent_pri->fs->slice_number == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_INDEXf, f_st->hw_index);
        } else if (f_ent_sec->fs->slice_number == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri, 
                                COUNTER_MODEf, 0);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec, 
                                COUNTER_MODEf, f_st->hw_mode);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_INDEXf, f_st->hw_index);
        }
    } else if ((mode = soc_mem_field32_get(unit, policy_mem, 
                (uint32 *) p_entry_sec, COUNTER_MODEf)) != 0) {
        if (f_ent_pri->fs->slice_number == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri, 
                                COUNTER_MODEf, f_st->hw_mode);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_INDEXf, f_st->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec, 
                                COUNTER_MODEf, 0);
        } else if (f_ent_sec->fs->slice_number == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_INDEXf, f_st->hw_index);
        }
    }
}

/*
 * Function:
 *     _bcm_field_fb_meter_adjust_wide_mode
 * Purpose:
 *     For wide mode entries, 
 *     if there is movement across slices,
 *     handle the meter mode, index, update & valid bits
 * Parameters:
 *     unit - BCM unit
 *     policy_mem - policy memory
 *     f_ent_pri - PRI entry
 *     f_ent_sec - SEC entry
 *     p_entry_pri - policy table for PRI entry
 *     p_entry_sec - policy table for SEC entry
 * Returns:
 *     none
 * Notes:
 *     FOUR cases are to be handled 
 *     1. previously meter in PRI slice, now in PRI slice
 *     2. previously meter in PRI slice, now in SEC slice
 *     3. previously meter in SEC slice, now in PRI slice
 *     4. previously meter in SEC slice, now in SEC slice
 *     MUST BE CALLED AFTER COUNTER ADJUST.
 *     
 */
void
_bcm_field_fb_meter_adjust_wide_mode(int unit, 
                                     soc_mem_t policy_mem,
                                     _field_policer_t *f_pl,
                                     _field_entry_t *f_ent_pri, 
                                     _field_entry_t *f_ent_sec, 
                                     uint32 p_entry_pri[],
                                     uint32 p_entry_sec[])
{
    uint32 counter_mode_pri;
    uint32 counter_mode_sec;
    uint32 counter_idx_pri;
    uint32 counter_idx_sec;
    int mode; 
    int restore_counter = FALSE; 


    /* Preserve counter mode. */
    counter_mode_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, COUNTER_MODEf);
    counter_idx_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, COUNTER_INDEXf);
    counter_mode_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, COUNTER_MODEf);
    counter_idx_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, COUNTER_INDEXf);


    if ((mode = soc_mem_field32_get(unit, policy_mem, 
         (uint32 *) p_entry_pri, METER_PAIR_MODEf)) != 0) {
        if (f_ent_pri->fs->slice_number == f_pl->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_EVENf, f_pl->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_ODDf, f_pl->hw_index);
            
        } else if (f_ent_sec->fs->slice_number == f_pl->pool_index) {
            sal_memcpy(p_entry_sec, p_entry_pri, sizeof(soc_mem_entry_null(unit, policy_mem)));
            sal_memset(p_entry_pri, 0, sizeof(soc_mem_entry_null(unit, policy_mem)));
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec, 
                                METER_PAIR_MODEf, mode);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_EVENf, f_pl->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_ODDf, f_pl->hw_index);
            restore_counter = TRUE; 
        }
    } else if ((mode = soc_mem_field32_get(unit, policy_mem, 
                (uint32 *) p_entry_sec, METER_PAIR_MODEf)) != 0) {
        if (f_ent_pri->fs->slice_number == f_pl->pool_index) {
            sal_memcpy(p_entry_pri, p_entry_sec, sizeof(soc_mem_entry_null(unit, policy_mem)));
            sal_memset(p_entry_sec, 0, sizeof(soc_mem_entry_null(unit, policy_mem)));
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri, 
                                METER_PAIR_MODEf, mode);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_EVENf, f_pl->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_ODDf, f_pl->hw_index);
            restore_counter = TRUE; 
        } else if (f_ent_sec->fs->slice_number == f_pl->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_EVENf, f_pl->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_ODDf, f_pl->hw_index);
        }
    }

    if (restore_counter) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_INDEXf, counter_idx_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_MODEf, counter_mode_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_INDEXf, counter_idx_pri);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_MODEf, counter_mode_pri);
    }
}

/*
 * Function:
 *     _bcm_field_fb_entry_move
 * Purpose:
 *     Copy an entry from one TCAM index to another. It copies the values in
 *     hardware from the old index to the new index. 
 *     IT IS ASSUMED THAT THE NEW INDEX IS EMPTY (VALIDf=00) IN HARDWARE.
 *     The old Hardware index is cleared at the end.
 * Parameters:
 *     unit           - (IN) BCM device number. 
 *     f_ent          - (IN) Entry to move
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 *                          
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to zero TCAM & Policy entry.*/
    uint32  p[_FP_MAX_ENTRY_WIDTH][SOC_MAX_MEM_FIELD_WORDS]; /* For policies  */
    int new_slice_numb = 0;             /* Entry new slice number.            */
    int new_slice_idx = 0;              /* Entry new offset in the slice      */ 
    soc_mem_t tcam_mem;                 /* TCAM memory id.                    */
    soc_mem_t policy_mem;               /* Policy table memory id .           */
    int tcam_idx_max;                   /* TCAM memory max index.             */
    int tcam_idx_min;                   /* TCAM memory min index.             */
    _field_stage_t *stage_fc;           /* Stage field control structure.     */
    _field_stage_id_t stage_id;         /* Field pipeline stage id.           */
    int idx;                            /* Iteration index.                   */
    _field_policer_t *f_pl = NULL;      /* Field policer descriptor.          */
    _field_stat_t    *f_st = NULL;      /* Field statistics descriptor.       */
    _field_group_t   *fg;               /* Field group structure.             */
    int              rv;                /* Operation return status.           */
#ifdef BCM_TRIUMPH_SUPPORT
    fp_global_mask_tcam_entry_t ipbm_entry; /* Always in PRI entry, 1st half */
#endif /* BCM_TRIUMPH_SUPPORT */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == tcam_idx_old) || (NULL == tcam_idx_new)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;

    /* Get field stage control . */
    stage_id = f_ent->group->stage_id;
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);      
    BCM_IF_ERROR_RETURN(rv);

    /* Get entry tcam and actions. */
    rv = _field_fb_tcam_policy_mem_get(unit, stage_id, &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    tcam_idx_max = soc_mem_index_max(unit, tcam_mem);
    tcam_idx_min = soc_mem_index_min(unit, tcam_mem);

    for (idx = 0; idx < parts_count; idx++) {
        /* Index sanity check. */
        if ((tcam_idx_old[idx] < tcam_idx_min) || (tcam_idx_old[idx] > tcam_idx_max) ||
            (tcam_idx_new[idx] < tcam_idx_min) || (tcam_idx_new[idx] > tcam_idx_max)) {
            FP_VVERB(("FP(unit %d) vverb: Invalid index range for _bcm_field_fb_entry_move \n \
                      from %d to %d", unit, tcam_idx_old[idx], tcam_idx_new[idx]));
            return (BCM_E_PARAM);
        }

        /* Read policy entry from current tcam index. */
        rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, tcam_idx_old[idx], p[idx]);
        BCM_IF_ERROR_RETURN(rv);

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) 
        if ((SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAMm)) && 
            (stage_id == _BCM_FIELD_STAGE_INGRESS) && (0 == idx)) {
            rv = READ_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ANY, 
                                           tcam_idx_old[0], &ipbm_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif /* BCM_TRIUMPH_SUPPORT || BCM_TRIUMPH2_SUPPORT */
    }

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, tcam_idx_new[0],
                                             &new_slice_numb, &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Update policy entry if moving across the slices. */
    if (f_ent->fs->slice_number != new_slice_numb) {
        /* Get policer associated with the entry. */
        if ((0 == (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS)) &&
            (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) {
            BCM_IF_ERROR_RETURN (_bcm_field_policer_get(unit,
                                                        f_ent->policer[0].pid, 
                                                        &f_pl));
        }
        /* Get statistics entity associated with the entry. */
        if ((0 == (stage_fc->flags & _FP_STAGE_GLOBAL_COUNTERS)) &&
            (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
            BCM_IF_ERROR_RETURN (_bcm_field_stat_get(unit,
                                                     f_ent->statistic.sid, 
                                                     &f_st));
        }
        if (fg->flags & (_FP_GROUP_SPAN_SINGLE_SLICE | 
                         _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
            /*
             * For _FP_GROUP_INTRASLICE_DOUBLEWIDE, *even* if it is  
             *     _FP_GROUP_SPAN_DOUBLE_SLICE, we do this.
             *     This is because in intraslice double-wide, the PRI 
             *     slice has tcam_slice_sz/2 entries, and same number
             *     of counter/meter pairs.
             *         Thus, counter/meter will always be allocated in the 
             *         PRI slice.
             */
            if (NULL != f_st) {
                /* 
                 * Set the index of the counter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p[0], 
                                    COUNTER_INDEXf, f_st->hw_index);
            }
            if (NULL != f_pl) {
                /* 
                 * Set the index of the meter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p[0],
                                    METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p[0],
                                    METER_INDEX_ODDf, f_pl->hw_index);
            }
        } else {
            if (NULL != f_st) {
                _bcm_field_fb_counter_adjust_wide_mode(unit, policy_mem, 
                                                       f_st, f_ent, 
                                                       f_ent + 1, p[0], p[1]);
            }
            if (NULL != f_pl) {
                _bcm_field_fb_meter_adjust_wide_mode(unit, policy_mem,
                                                     f_pl, f_ent, f_ent + 1, 
                                                     p[0], p[1]);
            }
        } 
    }

    /* 
     * Write entry to the destination
     * ORDER is important
     */ 
    for (idx = parts_count - 1; idx >= 0; idx--) {

        /* Write duplicate  policy entry to new tcam index. */
        rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx_new[idx], p[idx]);
        BCM_IF_ERROR_RETURN(rv);


#ifdef BCM_TRIUMPH_SUPPORT
        if ((SOC_IS_TR_VL(unit)) && 
            (stage_id == _BCM_FIELD_STAGE_INGRESS) && (idx == 0)) {
            rv = WRITE_FP_GLOBAL_MASK_TCAMm( unit, MEM_BLOCK_ALL, 
                                             tcam_idx_new[0], &ipbm_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif /* BCM_TRIUMPH_SUPPORT */

        /* Read tcam entry from current tcam index. */
        rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, tcam_idx_old[idx], e);
        BCM_IF_ERROR_RETURN(rv);

        /* Write duplicate  tcam entry to new tcam index. */
        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_new[idx], e);
        BCM_IF_ERROR_RETURN(rv);
#ifdef BCM_SCORPION_SUPPORT
        if ((SOC_IS_SC_CQ(unit)) && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            rv = soc_mem_read(unit, FP_TCAM_Ym, MEM_BLOCK_ANY, 
                              tcam_idx_old[idx], e);
            BCM_IF_ERROR_RETURN(rv);

            rv = soc_mem_write(unit, FP_TCAM_Ym, MEM_BLOCK_ALL, 
                               tcam_idx_new[idx], e);
            BCM_IF_ERROR_RETURN(rv);

        }
#endif /* BCM_SCORPION_SUPPORT */
    }

    /*
     * Clear old location 
     * ORDER is important
     */
    for (idx = 0; idx < parts_count; idx++) {
        rv = _field_fb_tcam_policy_clear(unit, stage_id, tcam_idx_old[idx]);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_stat_action_set
 *
 * Purpose:
 *     Get counter portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_stat_action_set(int unit, _field_entry_t *f_ent,
                       soc_mem_t mem, uint32 *buf)
{
    _field_stat_t    *f_st;    /* Field policer descriptor. */  
    int mode;              /* Counter hw mode.          */                    
    int idx;               /* Counter index.            */

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_FIREBOLT2_SUPPORT)
    /* VFP doesn't have counters. */
    if ((_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */


    /* Disable counting if counter was not attached to the entry. */
    if ((0 == (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_STAT_IN_SECONDARY_SLICE))) {
        idx = 0;
        mode = 0;
    } else {
        /* Get statistics entity description structure. */
        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));
        idx = f_st->hw_index;
        mode = f_st->hw_mode;
        /* Adjust counter hw mode for COUNTER_MODE_YES_NO/NO_YES */
        if (f_ent->statistic.flags & _FP_ENTRY_STAT_USE_ODD) {
            mode++;
        }
    }

#if defined(BCM_FIREBOLT2_SUPPORT)
    if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
        PolicySet(unit, mem, buf, PID_INCR_COUNTERf, (mode) ? 1 : 0);
        PolicySet(unit, mem, buf, PID_COUNTER_INDEXf, idx);
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
    { 
        PolicySet(unit, mem, buf, COUNTER_INDEXf, idx);
        PolicySet(unit, mem, buf, COUNTER_MODEf, mode);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_tcam_policy_install
 *
 * Purpose:
 *     Write entry into the chip's memory. 
 *
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 *
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_tcam_policy_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* tcAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    _field_action_t  *fa;               /* Actions iterator.                  */


    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, f_ent->fs->stage_id, 
                                       &tcam_mem, &policy_mem));

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Reset buffer. */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        BCM_IF_ERROR_RETURN(
            _field_fb_action_get(unit, policy_mem, f_ent, tcam_idx, fa, e));
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                        (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Install meter */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_meter_action_set(unit, f_ent, 
                                                       policy_mem, e));

    /* Install counter */
    BCM_IF_ERROR_RETURN(_field_fb_stat_action_set(unit, f_ent, policy_mem, e));

    /* Write policy entry. */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e));
 
    /* Reset buffer. */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the qualifier info from the entry structure. */
    BCM_IF_ERROR_RETURN(
        _field_fb_tcam_get(unit, f_ent, tcam_mem, e));

    /* Write tcam entry. */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e));
 
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_fb_tcam_get
 *
 * Purpose:
 *     Write entry into the chip's memory.
 *
 * Parameters:
 *     unit      -  (IN)BCM device number.  
 *     f_ent     -  (IN)Entry structure to get tcam info from.
 *     mem       -  (IN)Tcam memory.  
 *     buf       - (OUT) TCAM hardware entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_tcam_get(int unit, _field_entry_t *f_ent,
                   soc_mem_t mem, uint32 *buf)
{
    _field_tcam_t       *tcam; 
    _field_group_t      *fg;
    uint8               ent_part;
    int                 test_var;

    if ((NULL == f_ent) || (NULL == buf)) {
        return (BCM_E_PARAM);
    }

    tcam = &f_ent->tcam;
    fg = f_ent->group;

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
        soc_mem_field32_set(unit, mem, buf, VALIDf, 
                            (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 2);
        if (f_ent->flags & _FP_ENTRY_SECOND_HALF) {
            /* Intra-slice double wide key */
#ifdef BCM_FIREBOLT2_SUPPORT
            if (SOC_IS_FIREBOLT2(unit)) {
                soc_mem_field_set(unit, mem, buf, DATAf, tcam->key);
                soc_mem_field_set(unit, mem, buf, DATA_MASKf, tcam->mask);
            }
#endif /* BCM_FIREBOLT2_SUPPORT */
        } else {
            /* Single wide key */
            soc_mem_field_set(unit, mem, buf, KEYf, tcam->key);
            soc_mem_field_set(unit, mem, buf, MASKf, tcam->mask);

            /* 
             * Notes:
             *     1. SOURCE_PORT_NUMBER is same as F4.0
             *     2. DROP is part of F4.1
             *     3. PORT_FIELD_SEL_INDEX is same as F4.2 (not implemented
             *                                                  2006.01.18)
             */
            if ((BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPort)) &&
                (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_PORT_NUMBERf))) {
                soc_mem_field32_set(unit, mem, buf, SOURCE_PORT_NUMBERf, 
                                    tcam->f4);
                soc_mem_field32_set(unit, mem, buf, SOURCE_PORT_NUMBER_MASKf, 
                                    tcam->f4_mask);
            } else if (SOC_MEM_FIELD_VALID(unit, mem, F4f)) {
                soc_mem_field32_set(unit, mem, buf, F4f, tcam->f4);
                soc_mem_field32_set(unit, mem, buf, F4_MASKf, tcam->f4_mask);
            }

            /* 
             * Check if IPBM_SEL needs to be set
             *     Will be set if fpf1.0 is selected
             * First find out the part of the group, entry resides in
             *     Then check the corresponding sel_codes
             */ 
            BCM_IF_ERROR_RETURN
                (_bcm_field_entry_flags_to_tcam_part (f_ent->flags, fg->flags,
                                                      &ent_part));
            test_var = (fg->sel_codes[ent_part].fpf1 == 0) ? 1 : 0;

            soc_mem_field32_set(unit, mem, buf, IPBM_SELf, test_var);
            soc_mem_field32_set(unit, mem, buf, IPBM_SEL_MASKf, test_var);

            /*
             * Qualify on HiGig packets.
             */
            test_var =  
                (BCM_FIELD_QSET_TEST(fg->qset,
                                     bcmFieldQualifyHiGig) && tcam->higig) ? 1 : 0; 
            soc_mem_field32_set(unit, mem, buf, PKT_TYPEf, test_var);

            test_var =  
                (BCM_FIELD_QSET_TEST(fg->qset,
                                     bcmFieldQualifyHiGig) && tcam->higig_mask) ? 1 : 0;
            soc_mem_field32_set(unit, mem, buf, PKT_TYPE_MASKf, test_var);
#ifdef BCM_FIREBOLT2_SUPPORT 
            /*
             * Qualify on Drop.
             */
            if (SOC_IS_FIREBOLT2(unit)) {
                test_var =  
                    (BCM_FIELD_QSET_TEST(fg->qset,
                                         bcmFieldQualifyDrop) && tcam->drop) ? 1 : 0; 

                soc_mem_field32_set(unit, mem, buf, DROPf, test_var);

                test_var =  
                    (BCM_FIELD_QSET_TEST(fg->qset,
                                         bcmFieldQualifyDrop) && tcam->drop_mask) ? 1 : 0;
                soc_mem_field32_set(unit, mem, buf, DROP_MASKf, test_var);
            }
#endif
        }
#ifdef BCM_FIREBOLT2_SUPPORT
    } else if (soc_feature(unit, soc_feature_field_multi_stage)) {
        soc_field_t mask_field;
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
#endif
    } else {
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_meter_action_set
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
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_fb_meter_action_set(int unit, _field_entry_t *f_ent,
                               soc_mem_t mem, uint32 *buf)
{
    _field_policer_t *f_pl;             /* Field policer descriptor. */  
    uint32           meter_pair_mode;   /* Meter usage.              */ 

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_FIREBOLT2_SUPPORT) 
    /* VFP doesn't have meters. */
    if ((_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    

    if ((0 == (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_POLICER_IN_SECONDARY_SLICE))) {
        /* If no meter, install default meter pair mode.
         * Default meter pair mode doesn't need to burn a real meter.
         */
        soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);

#if defined(BCM_FIREBOLT2_SUPPORT)
        if ((SOC_IS_FIREBOLT2(unit) && 
             _BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id)) {
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 4);
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
        return (BCM_E_NONE);
    }

    /* Get policer description structure. */
    BCM_IF_ERROR_RETURN(_bcm_field_policer_get(unit, f_ent->policer[0].pid, 
                                               &f_pl));
    /* Get the even and odd indexes from the entry. The even and odd
     * meter indices are the same.
     */
    soc_mem_field32_set(unit, mem, buf, METER_INDEX_EVENf, 
                        f_pl->hw_index);
    soc_mem_field32_set(unit, mem, buf, METER_INDEX_ODDf, 
                        f_pl->hw_index);


    /* Get hw encoding for meter mode. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode));

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
        if (SOC_MEM_FIELD_VALID(unit, mem, METER_PAIR_MODE_MODIFIERf)) {
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODE_MODIFIERf, 1);
        } 
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
    }

    soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, meter_pair_mode);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_ingress_selcodes_install
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
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_ingress_selcodes_install(int unit, _field_group_t *fg, 
                                  uint8 slice_numb, bcm_pbmp_t pbmp, 
                                  int selcode_index)
{
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
    ifp_port_field_sel_entry_t  ipfs_entry;
    soc_field_t                 f1_field, f2_field, f3_field;
    uint32                      fpf4;
    int                         i_write_flag;
    _field_sel_t                *sel = NULL;
    int                         rv;
#if defined(BCM_FIREBOLT2_SUPPORT) 
    soc_field_t                 dw_fld;
    bcm_port_config_t           port_config;
#endif /* BCM_FIREBOLT2_SUPPORT */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    } 

    sel = &fg->sel_codes[selcode_index];

    if (slice_numb >= 16) {
        return (BCM_E_PARAM);
    }


    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        ((selcode_index == 1) || (selcode_index == 3))) {
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (SOC_IS_FIREBOLT2(unit)) {

            /* Write appropriate values in FP_DOUBLE_WIDE_F4_SELECTr and */
            /* FP_PORT_FIELD_SELm for an intra-slice double wide group */
            /* V4 key => F4 should be FP_PORT_FIELD_SEL_INDEX: sel->fpf2 = 0 */
            /* V6 key => F4 should be TCP flags: sel->fpf2 = 1*/
            dw_fld = _fb_ing_f4_reg[slice_numb];
            if ((sel->fpf2 != _FP_SELCODE_DONT_CARE) &&
                SOC_REG_FIELD_VALID(unit, FP_DOUBLE_WIDE_F4_SELECTr, dw_fld)) { 
                rv = soc_reg_field32_modify(unit, FP_DOUBLE_WIDE_F4_SELECTr, 
                                            REG_PORT_ANY, dw_fld,  sel->fpf2);
                BCM_IF_ERROR_RETURN(rv);
            }

            /* Read device port configuration. */ 
            rv = bcm_esw_port_config_get(unit, &port_config);
            BCM_IF_ERROR_RETURN(rv);

            dw_fld = _fb2_ifp_double_wide_key[slice_numb]; 
            if ((sel->fpf2 != _FP_SELCODE_DONT_CARE) &&
                SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
                /* Do the same thing for each entry in FP_PORT_FIELD_SEL table */
                BCM_PBMP_ITER(port_config.all, port) {
                    rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, port, &pfs_entry); 
                    BCM_IF_ERROR_RETURN(rv);

                    soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, 
                                                       dw_fld, sel->fpf2);
                    rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port, &pfs_entry);
                    BCM_IF_ERROR_RETURN(rv);

                    if (_FIELD_NEED_I_WRITE(unit, port, IFP_PORT_FIELD_SELm)) {
                        rv = READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, 
                                                      port, &pfs_entry);
                        BCM_IF_ERROR_RETURN(rv);

                        soc_IFP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, 
                                                            dw_fld, sel->fpf2);

                        rv = WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, 
                                                       port, &pfs_entry);
                        BCM_IF_ERROR_RETURN(rv);
                    }
                }
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
    } else {

        /* Determine which 3 fields will be modified */
        f1_field = _fb_field_tbl[slice_numb][0];
        f2_field = _fb_field_tbl[slice_numb][1];
        f3_field = _fb_field_tbl[slice_numb][2];

        /* Iterate over all ports */
        PBMP_ITER(pbmp, port) {
            i_write_flag = _FIELD_NEED_I_WRITE(unit, port,
                                               IFP_PORT_FIELD_SELm);
            /* Read Port's current entry in FP_PORT_FIELD_SEL table */
            rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                         port, &pfs_entry);
            BCM_IF_ERROR_RETURN(rv);
            if (i_write_flag) {
                rv = READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                              port, &ipfs_entry);
                BCM_IF_ERROR_RETURN(rv);
            }

            /* modify 0-3 fields depending on state of SELCODE_INVALID */
            if (sel->fpf1 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                                   f1_field, sel->fpf1);
                if (i_write_flag) {
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                        f1_field, sel->fpf1);
                }
            }

            if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                                   f2_field,
                                                   sel->fpf2);
                if (i_write_flag) {
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                        f2_field,
                                                        sel->fpf2);
                }
            }

            if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                                   f3_field,
                                                   sel->fpf3);
                if (i_write_flag) {
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                        f3_field,
                                                        sel->fpf3);
                }
            }

            /* Write each port's new entry */
            rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                          port, &pfs_entry);
            BCM_IF_ERROR_RETURN(rv);
            if (i_write_flag) {
                rv = WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                               port, &ipfs_entry);
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        /* If the device supports it, write the F4 select code. */
        if (soc_feature(unit, soc_feature_field_qual_drop)) {
            fpf4 = (sel->fpf4 == _FP_SELCODE_DONT_CARE) ? 0 : sel->fpf4;
            rv = soc_reg_field32_modify(unit, FP_F4_SELECTr, REG_PORT_ANY, 
                                        _fb_ing_f4_reg[slice_numb], fpf4);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Set inner vlan overlay config. */
        if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].inner_vlan_overlay) {
#if defined(BCM_FIREBOLT2_SUPPORT)  || defined(BCM_RAVEN_SUPPORT)
            if (SOC_REG_FIELD_VALID(unit, ING_MISC_CONFIG2r, 
                                    FP_INNER_VLAN_OVERLAY_ENABLEf)) {
                rv = soc_reg_field32_modify(unit, ING_MISC_CONFIG2r, REG_PORT_ANY,
                                            FP_INNER_VLAN_OVERLAY_ENABLEf,
                                            fg->sel_codes[0].inner_vlan_overlay);
                BCM_IF_ERROR_RETURN(rv);
            } else 
#endif  /* BCM_FIREBOLT2_SUPPORT  || BCM_RAVEN_SUPPORT */
                if (SOC_REG_FIELD_VALID(unit, ING_CONFIGr, MAC_BLOCK_INDEX_OVERLAYf)) {
                    rv = soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                                MAC_BLOCK_INDEX_OVERLAYf,
                                                fg->sel_codes[0].inner_vlan_overlay);
                    BCM_IF_ERROR_RETURN(rv);
                }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_ingress_slice_clear
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
_field_fb_ingress_slice_clear(int unit, uint8 slice_numb)
{
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
    ifp_port_field_sel_entry_t  ipfs_entry;
    bcm_port_config_t           port_config;
    soc_field_t                 f1_field, f2_field, f3_field;
    int                         i_write_flag;
    int                         rv;
#if defined (BCM_FIREBOLT2_SUPPORT)
    soc_field_t                 dw_fld;
#endif /* BCM_FIREBOLT2_SUPPORT */

    f1_field = _fb_field_tbl[slice_numb][0];
    f2_field = _fb_field_tbl[slice_numb][1];
    f3_field = _fb_field_tbl[slice_numb][2];

    /* Read device port configuration. */ 
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    /* Iterate over all ports */
    BCM_PBMP_ITER(port_config.all, port) {
        i_write_flag = _FIELD_NEED_I_WRITE(unit, port, IFP_PORT_FIELD_SELm);
        /* Read Port's current entry in FP_PORT_FIELD_SEL table */
        rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, 
                                     port, &pfs_entry);
        BCM_IF_ERROR_RETURN(rv);

        if (i_write_flag) {
            rv = READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                          port, &ipfs_entry);
            BCM_IF_ERROR_RETURN(rv);
        }

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           f1_field, 0);
        if (i_write_flag) {
            soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                f1_field, 0);
        }

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           f2_field, 0);
        if (i_write_flag) {
            soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                f2_field, 0);
        }

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           f3_field, 0);
        if (i_write_flag) {
            soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                f3_field, 0);
        }

#if defined (BCM_FIREBOLT2_SUPPORT)
        dw_fld = _fb2_ifp_double_wide_key[slice_numb]; 
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, 
                                               dw_fld, 0);
            if (i_write_flag) {
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                    dw_fld, 0);
            }
        }

        dw_fld = _fb2_slice_wide_mode_field[slice_numb];
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);
            if (i_write_flag) {
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                    dw_fld, 0);
            }
        }

        dw_fld = _fb2_slice_pairing_field[slice_numb / 2];
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);
            if (i_write_flag) {
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                    dw_fld, 0);
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */

        rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port,
                                      &pfs_entry);
        BCM_IF_ERROR_RETURN(rv);

        if (i_write_flag) {
            rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port,
                                          &ipfs_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (SOC_REG_FIELD_VALID(unit, FP_SLICE_CONFIGr, 
                            _fb_ing_slice_mode_field[slice_numb])) {
        rv = soc_reg_field32_modify(unit, FP_SLICE_CONFIGr, REG_PORT_ANY,
                                    _fb_ing_slice_mode_field[slice_numb], 0);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* If the device supports it, write the F4 select code. */
    if (SOC_REG_FIELD_VALID(unit, FP_F4_SELECTr, _fb_ing_f4_reg[slice_numb])) {
        rv = soc_reg_field32_modify(unit, FP_F4_SELECTr, REG_PORT_ANY,
                                    _fb_ing_f4_reg[slice_numb],0);
        BCM_IF_ERROR_RETURN(rv);
    }
#if defined (BCM_FIREBOLT2_SUPPORT)
    dw_fld = _fb_ing_f4_reg[slice_numb];
    if (SOC_REG_FIELD_VALID(unit, FP_DOUBLE_WIDE_F4_SELECTr, dw_fld)) {
        rv = soc_reg_field32_modify(unit, FP_DOUBLE_WIDE_F4_SELECTr,
                                    REG_PORT_ANY, dw_fld,  0);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) 

/*
 * Function:
 *     _field_fb_lookup_selcodes_install
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
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_lookup_selcodes_install(int unit, _field_group_t *fg, 
                                  uint8 slice_numb, int selcode_index)
{
    uint32        reg_val;
    _field_sel_t  *sel;

    sel = &fg->sel_codes[selcode_index];


    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val)); 
    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        (selcode_index % 2)) {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              DOUBLE_WIDE_KEY_SELECTf, sel->fpf2);
        }
    } else {
        /* FPF1 is fixed in Firebolt2 VFP */
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _vfp_field_tbl[slice_numb][0], sel->fpf2);
        }
        if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _vfp_field_tbl[slice_numb][1], sel->fpf3); 
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_lookup_slice_clear
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
_field_fb_lookup_slice_clear(int unit, uint8 slice_numb)
{
    uint32        reg_val;

    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val)); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      DOUBLE_WIDE_KEY_SELECTf, 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_field_tbl[slice_numb][0], 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_field_tbl[slice_numb][1], 0); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_pairing_field[slice_numb / 2], 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_wide_mode_field[slice_numb], 0);
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */

/*
 * Function:
 *     _bcm_field_fb_ingress_mode_set
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
int 
_bcm_field_fb_ingress_mode_set(int unit, uint8 slice_numb, 
                           _field_group_t *fg, uint8 flags)
{
    int     mode;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    bcm_port_config_t  port_config; /* Device port config. */
    int paired, dbl_wide;
    soc_field_t fld;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

    if (slice_numb >= COUNTOF(_fb_ing_slice_mode_field)) {
        return BCM_E_PARAM;
    }


#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_field_intraslice_double_wide)) {
        int port;
        fp_port_field_sel_entry_t pfs_entry;

        /* Read device port configuration. */ 
        BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

        /* Do the same thing for each port (index) in FP_PORT_FIELD_SEL table */
        BCM_PBMP_ITER(port_config.all, port) {

            BCM_IF_ERROR_RETURN
                (READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, port, &pfs_entry));

            fld = _fb2_slice_wide_mode_field[slice_numb];
            if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, fld)) {
                dbl_wide = (flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 1 : 0;

                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld,
                                                   dbl_wide);
            }

            paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0;
            fld = _fb2_slice_pairing_field[slice_numb / 2];
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld, paired);

            BCM_IF_ERROR_RETURN
                (WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port, &pfs_entry));

            if (_FIELD_NEED_I_WRITE(unit, port, IFP_PORT_FIELD_SELm)) {

                BCM_IF_ERROR_RETURN
                    (READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, port, &pfs_entry));

                fld = _fb2_slice_wide_mode_field[slice_numb];
                if (SOC_MEM_FIELD_VALID(unit, IFP_PORT_FIELD_SELm, fld)) {
                    dbl_wide = (flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 1 : 0;
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld,
                                                        dbl_wide);
                }

                paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0;
                fld = _fb2_slice_pairing_field[slice_numb / 2];
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld,
                                                    paired);

                BCM_IF_ERROR_RETURN
                    (WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port,
                                               &pfs_entry));
            }

        }
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    {
        if (flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
            mode = 0;
        } else if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            mode = 1;
        } else if (flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
            mode = 2; 
        } else {
            return (BCM_E_PARAM);
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, FP_SLICE_CONFIGr, REG_PORT_ANY,
                                    _fb_ing_slice_mode_field[slice_numb],
                                    mode));
    }
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
/*
 * Function:
 *     _bcm_field_fb_lookup_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_fb_mode_install that sets the mode of a
 *     slice in a register value that is to be used for VFP_KEY_CONTROLr.
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Filed group structure.
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int 
_bcm_field_fb_lookup_mode_set(int unit, uint8 slice_numb, 
                              _field_group_t *fg, uint8 flags) 
{
    int paired, dbl_wide;
    uint32  reg_val;
    if (slice_numb >= COUNTOF(_vfp_slice_wide_mode_field)) {
        return (BCM_E_PARAM);
    }
    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val));

    paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0; 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_pairing_field[slice_numb / 2], paired);

    dbl_wide = (flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 1 : 0;
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_wide_mode_field[slice_numb], dbl_wide);

    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT*/

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function:
 *     _field_fb_egress_mode_set
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
_field_fb_egress_mode_set(int unit, uint8 slice_numb, 
                          _field_group_t *fg, uint8 flags)
{
    int                mode_val;

    /* Input parameters check. */
    if ((NULL == fg) || (slice_numb >= COUNTOF(_efp_slice_mode))) {
        return (BCM_E_PARAM);
    }

    if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                                    _efp_slice_mode[slice_numb],
                                    _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE));
    } else {
        mode_val = (3 == fg->sel_codes[0].fpf3) ?
            _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2 :
            _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3;
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                                    _efp_slice_mode[slice_numb], mode_val));
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
_field_fb_egress_slice_clear(int unit, uint8 slice_numb)
{
    int rv; 

    /* Input parameters check. */
    if (slice_numb >= COUNTOF(_efp_slice_mode)) {
        return (BCM_E_PARAM);
    }

    rv = soc_reg_field32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                                _efp_slice_mode[slice_numb], 0);
    return (BCM_E_NONE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _bcm_field_fb_mode_set
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
int 
_bcm_field_fb_mode_set(int unit, uint8 slice_numb, _field_group_t *fg, uint8 flags) 
{
    int rv;     /* Operation return status. */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    if (!soc_feature(unit, soc_feature_field_wide)) {
        if (flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
            return (BCM_E_NONE);
        }
        return (BCM_E_UNAVAIL);
    }

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _bcm_field_fb_ingress_mode_set(unit, slice_numb, fg, flags);
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_LOOKUP:
          rv  = _bcm_field_fb_lookup_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_fb_egress_mode_set(unit, slice_numb, fg, flags);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default: 
          rv = BCM_E_PARAM;
    }
    return (rv);
}


/*
 * Function:
 *     _field_selcodes_install
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
STATIC int
_field_selcodes_install(int unit, _field_group_t *fg, 
                           uint8 slice_numb, bcm_pbmp_t pbmp, 
                           int selcode_index)
{
    int rv;    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }
    

    /* Set slice mode. Single/Double/Triple, Intraslice */
    rv = _bcm_field_fb_mode_set(unit, slice_numb, fg, fg->flags);
    BCM_IF_ERROR_RETURN(rv);

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS: 
          rv = _field_fb_ingress_selcodes_install(unit, fg, slice_numb,
                                                  pbmp, selcode_index);
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_fb_lookup_selcodes_install(unit, fg, slice_numb,
                                                 selcode_index);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = (BCM_E_NONE);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
      default:
          rv = (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_fb_group_install
 *
 * Purpose:
 *     Writes the Group's mode and field select codes into hardware. This
 *     should be called both at group creation time and any time the select
 *     codes change (i.e. bcm_field_group_set calls).
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - group to install
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_fb_group_install(int unit, _field_group_t *fg) 
{
    _field_slice_t *fs;        /* Slice pointer.           */
    uint8  slice_number;       /* Slices iterator.         */
    int    parts_count;        /* Number of entry parts.   */
    int    idx;                /* Iteration index.         */
    int    rv;                 /* Operation return status. */

    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    rv = _bcm_field_entry_tcam_parts_count (unit, fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < parts_count;  idx++) {

        /* Get slice id for entry part */
        rv = _bcm_field_tcam_part_to_slice_number(idx, fg->flags, &slice_number);
        BCM_IF_ERROR_RETURN(rv);

        /* Get slice pointer. */
        fs = fg->slices + slice_number;

#if defined (BCM_TRX_SUPPORT) 
        if (SOC_IS_TRX(unit)) {
            rv = _bcm_field_trx_selcodes_install (unit, fg, fs->slice_number, 
                                                  fg->pbmp, idx);
        } else 
#endif /* BCM_TRX_SUPPORT */
        {
            rv = _field_selcodes_install(unit, fg, fs->slice_number, fg->pbmp, idx);
        }
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_slice_clear
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
_bcm_field_fb_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs) 
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_fb_ingress_slice_clear(unit, fs->slice_number);
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_fb_lookup_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_fb_egress_slice_clear(unit, fs->slice_number);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default: 
          rv = BCM_E_INTERNAL;
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_field_fb_entry_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     f_ent  - Physical entry data
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_fb_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }


    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_clear(unit, f_ent->fs->stage_id, tcam_idx));

    f_ent->flags |= _FP_ENTRY_DIRTY; /* Mark entry as not installed */

    return (BCM_E_NONE);
}
#ifdef INCLUDE_L3
/*
 * Function:
 *     _bcm_field_policy_set_l3_info
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

int
_bcm_field_policy_set_l3_info(int unit, soc_mem_t mem, int value, uint32 *buf)
{
    uint32 flags;         /* L3 forwarding flags           */ 
    int nh_ecmp_id;       /* Next hop/Ecmp group id.       */
    int max_ecmp_paths;   /* Maximum number of ecmp paths. */
    int retval;           /* Operation return value.       */ 
    
    /* Resove next hop /ecmp group id. */
    retval = _bcm_field_policy_set_l3_nh_resolve(unit,  value,
                                                 &flags, &nh_ecmp_id);
    BCM_IF_ERROR_RETURN(retval);

    if (flags & BCM_L3_MULTIPATH) {              
        PolicySet(unit, mem, buf, ECMPf, 1);
        PolicySet(unit, mem, buf, ECMP_PTRf, nh_ecmp_id);
        BCM_IF_ERROR_RETURN(bcm_xgs3_max_ecmp_get(unit, &max_ecmp_paths));
        PolicySet(unit, mem, buf, ECMP_COUNTf, max_ecmp_paths - 1);
    } else {
        PolicySet(unit, mem, buf, ECMPf, 0);
        PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, nh_ecmp_id);
    }
    return (BCM_E_NONE);
}
#endif /* INCLUDE_L3 */
/*
 * Function:
 *     _field_fb_action_copy_to_cpu
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
_field_fb_action_copy_to_cpu(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                             _field_action_t *fa, uint32 *buf)
{
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if ((fa->param[0] != 0) && (fa->param[1] >= (1 << 8))) {
        FP_ERR(("FP(unit %d) Error: param1=%d out of range for CopyToCpu.\n",
                unit, fa->param[1]));
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionCopyToCpu:
#if defined(BCM_FIREBOLT2_SUPPORT)
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 0x1);
              PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 0x1);
              PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          }
          break;
      case bcmFieldActionRpCopyToCpu:
          PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 0x1);
          break;
      case bcmFieldActionYpCopyToCpu:
          PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 0x1);
          break;
      case bcmFieldActionGpCopyToCpu:
          PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          break;

      default:
          return (BCM_E_INTERNAL);
    }

    if (fa->param[0] != 0) {
        if (SOC_MEM_FIELD_VALID(unit, mem, MATCHED_RULEf)) {
            PolicySet(unit, mem, buf, MATCHED_RULEf, fa->param[1]);
        }
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_fb_action_get
 * Purpose:
 *     Install an action into the hardware tables.
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory.
 *     tcam_idx - index into TCAM
 *     fa       - field action 
 *     buf      - (OUT) Field TCAM plus Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
STATIC int
_field_fb_action_get(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                     int tcam_idx, _field_action_t *fa, uint32 *buf)
{
#if defined (BCM_FIREBOLT2_SUPPORT)
    uint32                          mode;
#endif /* BCM_FIREBOLT2_SUPPORT */
    uint32                          redir_field = 0;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    FP_VVERB(("FP(unit %d) vverb: BEGIN _field_fb_action_get(eid=%d, tcam_idx=0x%x, ",
              unit, f_ent->eid, tcam_idx));
    FP_VVERB(("action={%s,%d,%d})\n", _field_fb_action_name(fa->action),
              fa->param[0], fa->param[1]));

    switch (fa->action) {
    case bcmFieldActionCosQNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 1);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionCosQCpuNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 2);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioPktAndIntCopy:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 4);
        break;
    case bcmFieldActionPrioPktAndIntNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 5);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioPktAndIntTos:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 6);
        break;
    case bcmFieldActionPrioPktAndIntCancel:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 7);
        break;
    case bcmFieldActionPrioPktCopy:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 8);
        break;
    case bcmFieldActionPrioPktNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 9);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioPktTos:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 10);
        break;
    case bcmFieldActionPrioPktCancel:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 11);
        break;
    case bcmFieldActionPrioIntCopy:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 12);
        break;
    case bcmFieldActionPrioIntNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 13);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioIntTos:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 14);
        break;
    case bcmFieldActionPrioIntCancel:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 15);
        break;
    case bcmFieldActionTosNew:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 1);
        PolicySet(unit, mem, buf, NEWDSCP_TOSf, fa->param[0]);
        break;
    case bcmFieldActionTosCopy:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 2);
        break;
    case bcmFieldActionDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
			PolicySet(unit, mem, buf, RP_CHANGE_DSCPf, 1);
			PolicySet(unit, mem, buf, YP_CHANGE_DSCPf, 1);
			PolicySet(unit, mem, buf, GP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, RP_NEW_DSCPf, fa->param[0]);
            PolicySet(unit, mem, buf, YP_NEW_DSCPf, fa->param[0]);
            PolicySet(unit, mem, buf, GP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 3);
            PolicySet(unit, mem, buf, NEWDSCP_TOSf, fa->param[0]);
            PolicySet(unit, mem, buf, RP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, RP_DSCPf, fa->param[0]);
            PolicySet(unit, mem, buf, YP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, YP_DSCPf, fa->param[0]);
        }
        break;
    case bcmFieldActionTosCancel:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 4);
        break;
    case bcmFieldActionDscpCancel:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 4);
        break;
    case bcmFieldActionCopyToCpu:
    case bcmFieldActionRpCopyToCpu:
    case bcmFieldActionYpCopyToCpu:
    case bcmFieldActionGpCopyToCpu:
        BCM_IF_ERROR_RETURN
            (_field_fb_action_copy_to_cpu(unit, mem, f_ent, fa, buf));
        break;
    case bcmFieldActionGpCopyToCpuCancel:
        PolicySet(unit, mem, buf, COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionCopyToCpuCancel:
        PolicySet(unit, mem, buf, COPY_TO_CPUf, 2);
        PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 2);
        PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionSwitchToCpuCancel:
        PolicySet(unit, mem, buf, COPY_TO_CPUf, 3);
        break;
    case bcmFieldActionRedirect:    /* param0=modid, param1=port/tgid */
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 1);
        PolicySet(unit, mem, buf, REDIRECTIONf, 
                  (fa->param[0] << 6) | fa->param[1]);
        break;
    case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
        if (soc_property_get(unit, spn_TRUNK_EXTEND, 0)) {
            redir_field = ((fa->param[0] & 0x60) << 1) | (fa->param[0] & 0x1f);
        } else {
            redir_field = fa->param[0] & 0x1f;
        }
        redir_field |= 0x20; 
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 1);
        PolicySet(unit, mem, buf, REDIRECTIONf, redir_field);
        break;
    case bcmFieldActionRedirectCancel:
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 2);
        break;
    case bcmFieldActionRedirectPbmp:
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 3);
        PolicySet(unit, mem, buf, REDIRECTIONf, fa->param[0]);
        break;
    case bcmFieldActionEgressMask:
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 4);
        PolicySet(unit, mem, buf, REDIRECTIONf, fa->param[0]);
        break;
    case bcmFieldActionDrop:
#if defined (BCM_FIREBOLT2_SUPPORT)
		if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
			PolicySet(unit, mem, buf, RP_DROPf, 1);
			PolicySet(unit, mem, buf, YP_DROPf, 1);
			PolicySet(unit, mem, buf, GP_DROPf, 1);
		} else if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
			PolicySet(unit, mem, buf, DROPf, 1);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
		{
			PolicySet(unit, mem, buf, RP_DROPf, 1);
			PolicySet(unit, mem, buf, YP_DROPf, 1);
			PolicySet(unit, mem, buf, DROPf, 1);
		}
		break;
	case bcmFieldActionDropCancel:
#if defined (BCM_FIREBOLT2_SUPPORT)
		if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
			PolicySet(unit, mem, buf, RP_DROPf, 2);
			PolicySet(unit, mem, buf, YP_DROPf, 2);
			PolicySet(unit, mem, buf, GP_DROPf, 2);
		} else if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
			PolicySet(unit, mem, buf, DROPf, 2);
		} else 
#endif /* BCM_FIREBOLT2_SUPPORT */
		{
			PolicySet(unit, mem, buf, RP_DROPf, 2);
			PolicySet(unit, mem, buf, YP_DROPf, 2);
			PolicySet(unit, mem, buf, DROPf, 2);
		}
		break;
    case bcmFieldActionMirrorOverride:
        if (soc_feature(unit, soc_feature_field_mirror_ovr)) {
            PolicySet(unit, mem, buf, MIRROR_OVERRIDEf, 1);
        } else {
            return BCM_E_PARAM;
        }
        break;
    case bcmFieldActionMirrorIngress:    /* param0=modid, param1=port/tgid */
        PolicySet(unit, mem, buf, IM_MTP_INDEXf, fa->hw_index);
        PolicySet(unit, mem, buf, MIRRORf, 
                  PolicyGet(unit, mem, buf, MIRRORf) | 0x1);
        if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                        ENABLE_FP_FOR_MIRROR_PKTSf, 1));
        }
        break;
    case bcmFieldActionMirrorEgress:     /* param0=modid, param1=port/tgid */
        PolicySet(unit, mem, buf, EM_MTP_INDEXf, fa->hw_index);
        PolicySet(unit, mem, buf, MIRRORf, 
                  PolicyGet(unit, mem, buf, MIRRORf) | 2);
        if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                        ENABLE_FP_FOR_MIRROR_PKTSf, 1));
        }
        break;
#ifdef INCLUDE_L3 
    case bcmFieldActionL3ChangeVlan:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 1);
        BCM_IF_ERROR_RETURN
            (_bcm_field_policy_set_l3_info(unit, mem, fa->param[0], buf)); 
        break;
    case bcmFieldActionL3ChangeVlanCancel:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 2);
        break;
    case bcmFieldActionL3ChangeMacDa:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 4);
        BCM_IF_ERROR_RETURN
            (_bcm_field_policy_set_l3_info(unit, mem,  fa->param[0], buf)); 
        break;
    case bcmFieldActionL3ChangeMacDaCancel:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 5);
        break;
    case bcmFieldActionL3Switch:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 6);
        BCM_IF_ERROR_RETURN
            (_bcm_field_policy_set_l3_info(unit, mem, fa->param[0], buf)); 
        break;
    case bcmFieldActionL3SwitchCancel:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 7);
        break;
#endif /* INCLUDE_L3 */
    case bcmFieldActionAddClassTag:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 3);
        PolicySet(unit, mem, buf, CLASSIFICATION_TAGf, fa->param[0]);
        break;
    case bcmFieldActionEcnNew:
        /* Ecn in tos byte will be forced to a Drop Precedence value. */ 
        PolicySet(unit, mem, buf, ECN_CNGf, 0x1);
    case bcmFieldActionDropPrecedence:
        PolicySet(unit, mem, buf, RP_DROP_PRECEDENCEf, fa->param[0]);
        PolicySet(unit, mem, buf, YP_DROP_PRECEDENCEf, fa->param[0]);
        PolicySet(unit, mem, buf, DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionGpDropPrecedence:
        PolicySet(unit, mem, buf, DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionRpDrop:
        PolicySet(unit, mem, buf, RP_DROPf, 1);
        break;
    case bcmFieldActionRpDropCancel:
        PolicySet(unit, mem, buf, RP_DROPf, 2);
        break;
    case bcmFieldActionRpDropPrecedence:
        PolicySet(unit, mem, buf, RP_DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionRpCopyToCpuCancel:
        PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionRpDscpNew:
        PolicySet(unit, mem, buf, RP_CHANGE_DSCPf, 1);
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, RP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, RP_DSCPf, fa->param[0]);
        }
        break;
    case bcmFieldActionYpDrop:
        PolicySet(unit, mem, buf, YP_DROPf, 1);
        break;
    case bcmFieldActionYpDropCancel:
        PolicySet(unit, mem, buf, YP_DROPf, 2);
        break;
    case bcmFieldActionYpDropPrecedence:
        PolicySet(unit, mem, buf, YP_DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionYpCopyToCpuCancel:
        PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionYpDscpNew:
        PolicySet(unit, mem, buf, YP_CHANGE_DSCPf, 1);
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, YP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, YP_DSCPf, fa->param[0]);
        }
        break;
    case bcmFieldActionUpdateCounter:
    case bcmFieldActionMeterConfig:
        /*
         * This action is handled by the calling routine, not by this
         * routine.
         */
        return BCM_E_INTERNAL;
    case bcmFieldActionGpDrop:
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, GP_DROPf, 1);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, DROPf, 1);
        } 
        break;
    case bcmFieldActionGpDropCancel:
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, GP_DROPf, 2);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, DROPf, 2);
        } 
        break;
    case bcmFieldActionGpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
			PolicySet(unit, mem, buf, GP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, GP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 3);
            PolicySet(unit, mem, buf, NEWDSCP_TOSf, fa->param[0]);
        } 
        break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
    case bcmFieldActionRpOuterVlanPrioNew:
        PolicySet(unit, mem, buf, RP_CHANGE_DOT1Pf, 1);
        PolicySet(unit, mem, buf, RP_NEW_DOT1Pf, fa->param[0]);
        break;
    case bcmFieldActionYpOuterVlanPrioNew:
        PolicySet(unit, mem, buf, YP_CHANGE_DOT1Pf, 1);
        PolicySet(unit, mem, buf, YP_NEW_DOT1Pf, fa->param[0]);
        break;
    case bcmFieldActionGpOuterVlanPrioNew:
        PolicySet(unit, mem, buf, GP_CHANGE_DOT1Pf, 1);
        PolicySet(unit, mem, buf, GP_NEW_DOT1Pf, fa->param[0]);
        break;
    case bcmFieldActionClassSet:
        PolicySet(unit, mem, buf, USE_VFP_CLASS_IDf, 1);
        PolicySet(unit, mem, buf, VFP_CLASS_IDf, fa->param[0]); 
        break;
    case bcmFieldActionVrfSet:
        PolicySet(unit, mem, buf, USE_VFP_VRF_IDf, 1);
        PolicySet(unit, mem, buf, VFP_VRF_IDf, fa->param[0]); 
        break;
    case bcmFieldActionVlanAdd:
        PolicySet(unit, mem, buf, CHANGE_VLANf, 1);
        PolicySet(unit, mem, buf, NEW_VLANf, fa->param[0]);
        break;
    case bcmFieldActionInnerVlanNew:
        PolicySet(unit, mem, buf, PID_REPLACE_INNER_VIDf, 1);
        PolicySet(unit, mem, buf, PID_NEW_INNER_VIDf, fa->param[0]);
        break;
    case bcmFieldActionInnerVlanPrioNew:
        PolicySet(unit, mem, buf, PID_REPLACE_INNER_PRIf, 1);
        PolicySet(unit, mem, buf, PID_NEW_INNER_PRIf, fa->param[0]);
        break;
    case bcmFieldActionOuterVlanPrioNew:
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 1);
            PolicySet(unit, mem, buf, NEW_PRIORITYf, fa->param[0]);
        } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) { 
            PolicySet(unit, mem, buf, RP_CHANGE_DOT1Pf, 1);
            PolicySet(unit, mem, buf, RP_NEW_DOT1Pf, fa->param[0]);
            PolicySet(unit, mem, buf, YP_CHANGE_DOT1Pf, 1);
            PolicySet(unit, mem, buf, YP_NEW_DOT1Pf, fa->param[0]);
            PolicySet(unit, mem, buf, GP_CHANGE_DOT1Pf, 1);
            PolicySet(unit, mem, buf, GP_NEW_DOT1Pf, fa->param[0]);
        }
        break;
    case bcmFieldActionOuterVlanNew:
        if (_BCM_FIELD_STAGE_LOOKUP== f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, CHANGE_VLANf, 2);
            PolicySet(unit, mem, buf, NEW_VLANf, fa->param[0]);
        }
        else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, PID_REPLACE_OUTER_VIDf, 1);
            PolicySet(unit, mem, buf, PID_NEW_OUTER_VIDf, fa->param[0]);
        }
        break;
    case bcmFieldActionOuterTpidNew:
        BCM_IF_ERROR_RETURN(_bcm_field_tpid_hw_encode(unit, fa->param[0], &mode));
        PolicySet(unit, mem, buf, PID_REPLACE_OUTER_TPIDf, 1);
        PolicySet(unit, mem, buf, PID_TPID_INDEXf, mode);
        break;
#endif /* BCM_FIREBOLT2_SUPPORT */
    default:
        FP_ERR(("FP(unit %d) Error: Unknown action\n", unit, &fa->action));
        return BCM_E_PARAM;
    }

    fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */

    FP_VVERB(("FP(unit %d) vverb: END _field_fb_action_get()\n", unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_fb_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_action_params_check(int unit,_field_entry_t *f_ent, 
                             _field_action_t *fa)
{
    uint32  redir_field = 0; /* Redirection field value.*/
#if defined(INCLUDE_L3)
    uint32 flags;            /* L3 forwarding flags.    */ 
    int nh_ecmp_id;          /* Next hop/Ecmp group id. */
#endif /* INCLUDE_L3 */
    soc_mem_t mem;           /* Policy table memory id. */
    soc_mem_t tcam_mem;      /* Tcam memory id.         */
    int rv;                  /* Operation return value. */ 


    /* Input parameters check. */
    if ((NULL == f_ent) || (fa == NULL)){
        return (BCM_E_PARAM);
    }

    rv = _field_fb_tcam_policy_mem_get(unit, f_ent->group->stage_id, 
                                       &tcam_mem, &mem);
    BCM_IF_ERROR_RETURN(rv);

    switch (fa->action) {
      case bcmFieldActionCosQNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionCosQCpuNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionPrioPktAndIntNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionPrioPktNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionPrioIntNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionTosNew:
          PolicyCheck(unit, mem, NEWDSCP_TOSf, fa->param[0]);
          break;
      case bcmFieldActionDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, RP_NEW_DSCPf, fa->param[0]);
              PolicyCheck(unit, mem, YP_NEW_DSCPf, fa->param[0]);
              PolicyCheck(unit, mem, GP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, NEWDSCP_TOSf, fa->param[0]);
              PolicyCheck(unit, mem, RP_DSCPf, fa->param[0]);
              PolicyCheck(unit, mem, YP_DSCPf, fa->param[0]);
          }
          break;
      case bcmFieldActionCopyToCpu:
      case bcmFieldActionRpCopyToCpu:
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionGpCopyToCpu:
          if (fa->param[0]) {
              PolicyCheck(unit, mem, MATCHED_RULEf, fa->param[1]);
          }
          break;
      case bcmFieldActionRedirect:    /* param0 = modid, param1 = port/tgid */
          rv = _bcm_field_action_dest_check(unit, fa);
          BCM_IF_ERROR_RETURN(rv);

          if (fa->param[0] > 0x3f || fa->param[1] > 0x3f) {
              return BCM_E_PARAM;
          }
          PolicyCheck(unit, mem, REDIRECTIONf, 
                      (fa->param[0] << 6) | fa->param[1]);
          break;
      case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
          if (soc_property_get(unit, spn_TRUNK_EXTEND, 0)) {
              redir_field = ((fa->param[0] & 0x60) << 1) | (fa->param[0] & 0x1f);
          } else {
              redir_field = fa->param[0] & 0x1f;
          }
          redir_field |= 0x20; 
          PolicyCheck(unit, mem, REDIRECTIONf, redir_field);
          break;
      case bcmFieldActionRedirectPbmp:
          PolicyCheck(unit, mem, REDIRECTIONf, fa->param[0]);
          break;
      case bcmFieldActionEgressMask:
          PolicyCheck(unit, mem, REDIRECTIONf, fa->param[0]);
          break;
      case bcmFieldActionMirrorIngress:
      case bcmFieldActionMirrorEgress:
          rv = _bcm_field_action_dest_check(unit, fa);
          BCM_IF_ERROR_RETURN(rv);
          break;
#ifdef INCLUDE_L3 
      case bcmFieldActionL3ChangeVlan:
      case bcmFieldActionL3ChangeMacDa:
      case bcmFieldActionL3Switch:
          rv = _bcm_field_policy_set_l3_nh_resolve(unit,  fa->param[0],
                                                   &flags, &nh_ecmp_id);
          BCM_IF_ERROR_RETURN(rv);
          break;
#endif /* INCLUDE_L3 */
      case bcmFieldActionAddClassTag:
          PolicyCheck(unit, mem, CLASSIFICATION_TAGf, fa->param[0]);
          break;
      case bcmFieldActionDropPrecedence:
          PolicyCheck(unit, mem, RP_DROP_PRECEDENCEf, fa->param[0]);
          PolicyCheck(unit, mem, YP_DROP_PRECEDENCEf, fa->param[0]);
          PolicyCheck(unit, mem, DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionGpDropPrecedence:
          PolicyCheck(unit, mem, DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionRpDropPrecedence:
          PolicyCheck(unit, mem, RP_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionRpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, RP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, RP_DSCPf, fa->param[0]);
          }
          break;
      case bcmFieldActionYpDropPrecedence:
          PolicyCheck(unit, mem, YP_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionYpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, YP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, YP_DSCPf, fa->param[0]);
          }
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case bcmFieldActionRpOuterVlanPrioNew:
          PolicyCheck(unit, mem, RP_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionYpOuterVlanPrioNew:
          PolicyCheck(unit, mem, YP_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionGpOuterVlanPrioNew:
          PolicyCheck(unit, mem, GP_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionClassSet:
          PolicyCheck(unit, mem, VFP_CLASS_IDf, fa->param[0]); 
          break;
      case bcmFieldActionVrfSet:
          PolicyCheck(unit, mem, VFP_VRF_IDf, fa->param[0]); 
          break;
      case bcmFieldActionVlanAdd:
          PolicyCheck(unit, mem, NEW_VLANf, fa->param[0]);
          break;
      case bcmFieldActionGpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, GP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, NEWDSCP_TOSf, fa->param[0]);
          }
          break;
      case bcmFieldActionInnerVlanNew:
          PolicyCheck(unit, mem, PID_NEW_INNER_VIDf, fa->param[0]);
          break;
      case bcmFieldActionInnerVlanPrioNew:
          PolicyCheck(unit, mem, PID_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionOuterVlanPrioNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_PRIORITYf, fa->param[0]);
          } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) { 
              PolicyCheck(unit, mem, RP_NEW_DOT1Pf, fa->param[0]);
              PolicyCheck(unit, mem, YP_NEW_DOT1Pf, fa->param[0]);
              PolicyCheck(unit, mem, GP_NEW_DOT1Pf, fa->param[0]);
          }
          break;
      case bcmFieldActionOuterVlanNew:
          if (_BCM_FIELD_STAGE_LOOKUP== f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_VLANf, fa->param[0]);
          }
          else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, PID_NEW_OUTER_VIDf, fa->param[0]);
          }
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default:
          return BCM_E_NONE;
    }
    return BCM_E_NONE;
}

#define BCM_FB_BITS_IN_4_KBYTES   (1 << 15) /* (4 * 1024 * 8) 4K bytes */
/*
 * Function: 
 *     _field_fb_bucket_calc
 * Purpose:
 *     Lookup the proper meter bucket size encoding.
 */

STATIC int 
_field_fb_bucket_calc(int unit, uint32 burst, 
                      uint32 *bucket_size, uint32 *bucket_count)
{
    uint32 b_size;

    /* Input  parameters check. */  
    if ((NULL == bucket_size) || (NULL == bucket_count)) {
        return (BCM_E_PARAM);
    }

    /* If requested rate is 0 -> set size & count to 0 */
    if (0 == burst) {
        *bucket_size = *bucket_count = 0;
        return (BCM_E_NONE);
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined (BCM_TRX_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit)) {
        int linear_mode = TRUE;
        uint32 max_bucket_size;

        /* Bucket size is in 4 KBytes granularity */
        b_size = _bcm_fb2_kbits_to_bucket_encoding(burst, linear_mode);

        /* Verify that value fits into hw field. */
        max_bucket_size =  
            (1 << soc_mem_field_length(unit, FP_METER_TABLEm, BUCKETSIZEf)) - 1;
        if (b_size > max_bucket_size) {
            return (BCM_E_PARAM);
        }

        *bucket_size = b_size;
        /* Bucket count is in 1/2 bit granularity and bucket size is
         * in 4 KByte granularity.
         */ 
        *bucket_count = 2 * b_size * BCM_FB_BITS_IN_4_KBYTES;
    } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    {
        uint8               b_code;
       
        /* Bucket size is encoded as follow 
         *            Bucket Size
         * 4'd0     : 4 K - 1
         * 4'd1     : 8 K - 1
         * 4'd2     : 16 K - 1
         * 4'd3     : 32 K - 1
         * 4'd4     : 64 K - 1
         * 4'd5     : 128 K - 1
         * 4'd6     : 256 K - 1
         * 4'd7     : 512 K - 1
         * 4'd8     : 1 M - 1
         * 4'd9     : 2 M - 1
         * 4'd10    : 4 M - 1
         * 4'd11    : 8 M - 1
         * 4'd12    : 16 M - 1
         */
#define BCM_FB_BUCKET_SIZE_MAX (12)
        b_size = BCM_FB_BITS_IN_4_KBYTES;
        burst = burst * 1000; /* Convert KBits to bits */
        for (b_code = 0; b_code <= BCM_FB_BUCKET_SIZE_MAX; b_code++) {
            if (burst < b_size) {
                break;
            }
            b_size *= 2;
        }
        *bucket_size  = b_code;
        if (*bucket_size >  BCM_FB_BUCKET_SIZE_MAX) {
            return (BCM_E_PARAM);
        }
#undef BCM_FB_BUCKET_SIZE_MAX 

        /* Bucket count is in 1/2 of a bit granularity */
        *bucket_count = 2 * b_size;
    }
    FP_VVERB(("FP(unit %d) vverb: _field_fb_bucket_calc  rate=%d, bucket_count=%d, "
             "bucket_size=%d\n", unit, burst, *bucket_count, *bucket_size));
    return (BCM_E_NONE);
}
#undef BCM_FB_BITS_IN_4_KBYTES

STATIC int
_field_fb_policer_mem_get(int unit, _field_entry_t *f_ent,
                          soc_mem_t *mem)
{
    _field_group_t  *fg;              /* Field group structure. */
    soc_mem_t meter_table = INVALIDm; /* Hw meter table.        */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == mem)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HB_GW(unit)) {
            meter_table = FP_METER_TABLE_Xm; 
        } else
#endif /* BCM_BRADLEY_SUPPORT */
        { 
            meter_table = FP_METER_TABLEm;
        }
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    } if (soc_feature(unit, soc_feature_field_multi_stage)) {
        if (_BCM_FIELD_STAGE_EGRESS == fg->stage_id) {
            meter_table = EFP_METER_TABLEm;
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    }
    if (INVALIDm == meter_table) {
        return (BCM_E_INTERNAL);
    } 
    *mem = meter_table;
    return (BCM_E_NONE);
}

STATIC int
_field_fb_policer_hw_update(int unit, _field_entry_t  *f_ent,
                            _field_policer_t *f_pl, uint8 index_mtr,
                            uint32 bucket_size, uint32 bucket_count, 
                            uint32 refresh_count)
{
    _field_stage_t    *stage_fc;    /* Stage field control.       */
    int               meter_offset; /* HW buffer for meter entry. */
    uint32            meter_entry[SOC_MAX_MEM_FIELD_WORDS];
    int               mem_field_width;
    soc_mem_t         mem;          /* Meter table memory.        */
    int               meter_hw_idx; /* Meter memory entry index.  */

    if ((NULL == f_pl) || (NULL == f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Get meter table memory. */
    BCM_IF_ERROR_RETURN(_field_fb_policer_mem_get(unit, f_ent, &mem));
    
    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc));

    mem_field_width = soc_mem_field_length(unit, mem, REFRESHCOUNTf); 
    if (refresh_count > (uint32)((1 << mem_field_width) - 1)) {
        return (BCM_E_PARAM);
    }
 
    meter_offset = (BCM_FIELD_METER_PEAK == index_mtr) ? 0 : 1;

    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        meter_hw_idx = (2 * f_pl->pool_index *
                        stage_fc->meter_pool[f_pl->pool_index]->size)  +
                        (2 * f_pl->hw_index) + meter_offset;
    } else {
        meter_hw_idx = 
            stage_fc->slices[f_pl->pool_index].start_tcam_idx + \
            (2 * f_pl->hw_index) + meter_offset;
    }

    if (meter_hw_idx < soc_mem_index_min(unit, mem) ||
        meter_hw_idx > soc_mem_index_max(unit, mem)) {
        return (BCM_E_INTERNAL);
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      meter_hw_idx, &meter_entry));
 
    soc_mem_field32_set(unit, mem, &meter_entry, REFRESHCOUNTf,
                        refresh_count);
    soc_mem_field32_set(unit, mem, &meter_entry, BUCKETSIZEf,
                        bucket_size);
    soc_mem_field32_set(unit, mem, &meter_entry, BUCKETCOUNTf,
                        bucket_count);

    /* Refresh mode is only set to 1 for Single Rate. Other modes get 0 */
     
    if (f_pl->cfg.mode  == bcmPolicerModeSrTcm) {
        soc_mem_field32_set(unit, mem, &meter_entry, REFRESH_MODEf, 1);
    } else {
        soc_mem_field32_set(unit, mem, &meter_entry, REFRESH_MODEf, 0);
    }

#if defined(BCM_TRX_SUPPORT) 
    if (((stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) ||
        (stage_fc->stage_id == _BCM_FIELD_STAGE_EXTERNAL)) &&
        (SOC_MEM_FIELD_VALID(unit, mem, METER_GRANf))) {
        soc_mem_field32_set(unit, mem, &meter_entry, METER_GRANf, 3);
    }
#endif /* BCM_TRX_SUPPORT) */

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                       meter_hw_idx, meter_entry));

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HB_GW(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, FP_METER_TABLE_Ym, MEM_BLOCK_ALL,
                           meter_hw_idx, meter_entry));
    }
#endif /* BCM_BRADLEY_SUPPORT */ 

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_policer_install
 * Purpose:
 *     Install a policer pair into the hardware tables.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     f_ent  - (IN) Field entry structure.
 *     f_pl   - (IN) Field policer descriptor.
 * Returns:
 *     BCM_E_XXX
 */
/* This reflects the 64,000bps granularity */
#define _FP_POLICER_REFRESH_RATE 64
int
_bcm_field_fb_policer_install(int unit, _field_entry_t *f_ent, 
                              _field_policer_t *f_pl)
{
    uint32                       bucket_size = 0;
    uint32                       refresh_count = 0;
    uint32                       bucket_count = 0;

    if ((NULL == f_ent) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->group || NULL == f_ent->fs) {
        return (BCM_E_INTERNAL);
    }

    if (0 == (f_pl->hw_flags & _FP_POLICER_DIRTY)) {
        return (BCM_E_NONE);
    }

    /* lookup bucket size from tables */
    if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
        BCM_IF_ERROR_RETURN
            (_field_fb_bucket_calc(unit, f_pl->cfg.ckbits_burst,
                                   &bucket_size, &bucket_count));

        /* A value of one in the REFRESHCOUNT is 64 kbits/sec */
        refresh_count = f_pl->cfg.ckbits_sec / _FP_POLICER_REFRESH_RATE;

        BCM_IF_ERROR_RETURN
            (_field_fb_policer_hw_update(unit, f_ent, f_pl,   
                                         BCM_FIELD_METER_COMMITTED,
                                         bucket_size, bucket_count,
                                         refresh_count));

        FP_VVERB(("FP(unit %d) vverb: commited-> bucket_size=%u, bucket_count=%u, "
                 "refresh_count=%u\n", unit, bucket_size, bucket_count, 
                 refresh_count)); 
        f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;
    }

    if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
        BCM_IF_ERROR_RETURN
            (_field_fb_bucket_calc(unit, f_pl->cfg.pkbits_burst, 
                                   &bucket_size, &bucket_count));

        refresh_count  = f_pl->cfg.pkbits_sec / _FP_POLICER_REFRESH_RATE;

        BCM_IF_ERROR_RETURN
            (_field_fb_policer_hw_update(unit, f_ent, f_pl,
                                         BCM_FIELD_METER_PEAK,
                                         bucket_size, bucket_count,
                                         refresh_count));

        FP_VVERB(("FP(unit %d) vverb: commited-> bucket_size=%u, bucket_count=%u, "
                 "refresh_count=%u\n", unit, bucket_size, bucket_count, 
                 refresh_count)); 
        f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;
    }
    return BCM_E_NONE;
}
#undef _FP_POLICER_REFRESH_RATE

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function:
 *     _field_fb2_key_match_type_set
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
STATIC int
_field_fb2_key_match_type_set(int unit, _field_entry_t *f_ent)
{
    _field_group_t           *fg;   /* Field group entry belongs to. */
    uint32                   data;  /* Key match type.               */         
    uint32                   mask;  /* Key match type mask.          */         
                                    /* Key match type offset.        */
    _bcm_field_qual_offset_t q_offset = {KEYf, 181, 3, 0, 0, 0, 0}; 

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
          default:
              return (BCM_E_INTERNAL);
        } 
    } else {
        switch (fg->sel_codes[1].fpf3) {
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              data = _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE;
              break;
          default:
              return (BCM_E_INTERNAL);
        }
    }
   mask = 0x7;
   return _bcm_field_qual_value_set(unit, &q_offset, f_ent, &data, &mask);
}
#endif /* BCM_FIREBOLT2_SUPPORT */

/*
 * Function:
 *     _bcm_field_fb_write_slice_map
 *
 * Purpose:
 *     Write the FP_SLICE_MAP
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
int
_bcm_field_fb_write_slice_map(int unit, _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
    int idx;
    fp_slice_map_entry_t map_entry;
    soc_field_t field;
    int vmap_size;
    uint32 value;

    uint32 virtual_to_physical_map[] = {
        VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBERf
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
            ,VIRTUAL_SLICE_8_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_9_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_10_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBERf
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT */
    };
    uint32 virtual_to_group_map[] = {
        VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUPf
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
            ,VIRTUAL_SLICE_8_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_9_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_10_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUPf
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT */
    };

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        value = (stage_fc->vmap[0][idx]).vmap_key;
        field = virtual_to_physical_map[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);

        value = (stage_fc->vmap[0][idx]).virtual_group;
        field = virtual_to_group_map[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);
    }

    rv = WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &map_entry);
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT */
    return (rv);
}


/*
 * Function:
 *     _field_fb_qualify_ip_type
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
STATIC int
_field_fb_qualify_ip_type(int unit, _field_entry_t *f_ent, 
                              bcm_field_IpType_t type) 
{
    _field_group_t      *fg;
    uint32              data = BCM_FIELD_IPTYPE_BAD,
                        mask = BCM_FIELD_IPTYPE_BAD;

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    /*
     * Devices with a separate PacketFormat and IpType use different hardware
     * encodings for IpType. 
     */
    if (soc_feature(unit, soc_feature_field_qual_IpType)) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            switch (type) {
              case bcmFieldIpTypeAny:
                  data = 0x0;
                  mask = 0x0;
                  break;
              case bcmFieldIpTypeNonIp:
                  data = 0x0;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv4Not:
                  data = 0x0;
                  mask = 0x1;
                  break;
              case bcmFieldIpTypeIpv4NoOpts:
                  data = 0x1;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv4WithOpts:
                  data = 0x3;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv4Any:
                  data = 0x1;
                  mask = 0x1;
                  break;
              case bcmFieldIpTypeIpv6:
                  data = 0x2;
                  mask = 0x3;
                  break;
              default:
                  break;
            }
        }
#ifdef BCM_FIREBOLT2_SUPPORT 
        else if (fg->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
            switch (type) {
              case bcmFieldIpTypeNonIp:
                  data = 0x2;
                  mask = 0x2;
                  break; 
              case bcmFieldIpTypeIpv4Any:
                  data = 0x0;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv6:
                  data = 0x1;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIp:
                  data = 0x0;
                  mask = 0x2;
                  break;
              case bcmFieldIpTypeArp:
                  data = 0x2;
                  mask = 0x3;
                  break; 
              default:
                  break;
            }
        } else if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            switch (type) {
              case bcmFieldIpTypeIpv4WithOpts:
                  data = 0x1;
                  mask = 0x1;
                  break; 
              case bcmFieldIpTypeIpv4NoOpts:
                  data = 0x0;
                  mask = 0x1;
                  break;
              case bcmFieldIpTypeIpv4Any:
                  if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
                      return (BCM_E_NONE);
                  } 
                  return (BCM_E_UNAVAIL);
              case bcmFieldIpTypeIpv6:
                  if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
                      return (BCM_E_NONE);
                  } 
                  return (BCM_E_UNAVAIL);
              default:
                  return (BCM_E_UNAVAIL);
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */ 
    } else { /* Devices without separate IpType field. */
        switch (type) {
          case bcmFieldIpTypeAny:
              data = 0x0;
              mask = 0x0;
              break;
          case bcmFieldIpTypeNonIp:
              data = 0x0;
              mask = 0x3;
              break;
          case bcmFieldIpTypeIpv4Not:
              data = 0x0;
              mask = 0x1;
              break;
          case bcmFieldIpTypeIpv4Any:
              data = 0x1;
              mask = 0x3;
              break;
          case bcmFieldIpTypeIpv6:
              data = 0x2;
              mask = 0x3;
              break;
          case bcmFieldIpTypeIpv6Not:
              data = 0x0;
              mask = 0x2;
              break;
          default:
              break;
        }
    }

    FP_VVERB(("FP(unit %d) vverb: entry=%d qualifying on Iptype, data=%#x, mask=%#x\n",
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
 *     _field_fb_qualify_ip_type_get
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
STATIC int
_field_fb_qualify_ip_type_get(int unit, _field_entry_t *f_ent, 
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


    /*
     * Devices with a separate PacketFormat and IpType use different hardware
     * encodings for IpType. 
     */
    if (soc_feature(unit, soc_feature_field_qual_IpType)) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            if ((hw_data == 0) && (hw_mask == 0)) {
                *type = bcmFieldIpTypeAny;
            } else if ((hw_data == 0) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeNonIp;
            } else if ((hw_data == 0) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4Not;
            } else if ((hw_data == 1) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv4NoOpts;
            } else if ((hw_data == 3) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv4WithOpts;
            } else if ((hw_data == 1) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4Any;
            } else if ((hw_data == 2) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv6;
            } else {
                return (BCM_E_INTERNAL);
            }
        }
#ifdef BCM_FIREBOLT2_SUPPORT 
        else if (fg->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
            if ((hw_data == 2) && (hw_mask == 2)) {
                *type = bcmFieldIpTypeNonIp;
            } else if ((hw_data == 0) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv4Any;
            } else if ((hw_data == 1) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv6;
            } else if ((hw_data == 0) && (hw_mask == 2)) {
                *type = bcmFieldIpTypeIp;
            } else if ((hw_data == 2) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeArp;
            } else {
                return (BCM_E_INTERNAL);
            }
        } else if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            if ((hw_data == 1) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4WithOpts;
            } else if ((hw_data == 0) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4NoOpts;
            } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
                *type = bcmFieldIpTypeIpv4Any;
            } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
                *type = bcmFieldIpTypeIpv6;
            } else {
                return (BCM_E_INTERNAL);
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */ 
    } else { /* Devices without separate IpType field. */
        if ((hw_data == 0) && (hw_mask == 0)) {
            *type = bcmFieldIpTypeAny;
        } else if ((hw_data == 0) && (hw_mask == 3)) {
            *type = bcmFieldIpTypeNonIp;
        } else if ((hw_data == 0) && (hw_mask == 1)) {
            *type = bcmFieldIpTypeIpv4Not;
        } else if ((hw_data == 1) && (hw_mask == 3)) {
            *type = bcmFieldIpTypeIpv4Any;
        } else if ((hw_data == 2) && (hw_mask == 3)) {
            *type = bcmFieldIpTypeIpv6;
        } else if ((hw_data == 0) && (hw_mask == 2)) {
            *type = bcmFieldIpTypeIpv6Not;
        } else {
            return (BCM_E_INTERNAL);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_action_conflict_check
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
STATIC int
_field_fb_action_conflict_check(int unit, _field_entry_t *f_ent,
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
    _FP_ACTIONS_CONFLICT(action1);

#ifdef BCM_FIREBOLT2_SUPPORT 
    if (SOC_IS_FIREBOLT2(unit)) {
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            switch (action1) {
              case bcmFieldActionDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
                  break;
              case bcmFieldActionDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
                  break;
              case bcmFieldActionRpDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
                  break;
              case bcmFieldActionRpDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
                  break;
              case bcmFieldActionYpDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
                  break;
              case bcmFieldActionYpDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
                  break;
              case bcmFieldActionGpDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
                  break;
              case bcmFieldActionGpDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
                  break;
              case bcmFieldActionDscpNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
                  break;
              case bcmFieldActionRpDscpNew:
              case bcmFieldActionYpDscpNew:
              case bcmFieldActionGpDscpNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
                  break;
              case bcmFieldActionRpOuterVlanPrioNew:
              case bcmFieldActionYpOuterVlanPrioNew:
              case bcmFieldActionGpOuterVlanPrioNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanPrioNew);
                  break;
              case bcmFieldActionOuterVlanPrioNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpOuterVlanPrioNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpOuterVlanPrioNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpOuterVlanPrioNew);
                  break;
              case bcmFieldActionInnerVlanPrioNew:
              case bcmFieldActionInnerVlanNew:
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionOuterTpidNew:
              case bcmFieldActionUpdateCounter:
              case bcmFieldActionMeterConfig:
                  break;
              default:
                  break;
            }
            return (BCM_E_NONE);
        }
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            switch (action1) {
              case bcmFieldActionDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  break;
              case bcmFieldActionDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  break;
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionVlanAdd:
              case bcmFieldActionOuterVlanPrioNew:
              case bcmFieldActionCopyToCpu:
              case bcmFieldActionVrfSet:
              case bcmFieldActionClassSet:
                  break;
              default:
                  break;
            }
            return (BCM_E_NONE);
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    switch (action1) {
      case bcmFieldActionCosQNew:
      case bcmFieldActionCosQCpuNew:
      case bcmFieldActionPrioPktAndIntCopy:
      case bcmFieldActionPrioPktAndIntNew:
      case bcmFieldActionPrioPktAndIntTos:
      case bcmFieldActionPrioPktAndIntCancel:
      case bcmFieldActionPrioPktCopy:
      case bcmFieldActionPrioPktNew:
      case bcmFieldActionPrioPktTos:
      case bcmFieldActionPrioPktCancel:
      case bcmFieldActionPrioIntCopy:
      case bcmFieldActionPrioIntNew:
      case bcmFieldActionPrioIntTos:
      case bcmFieldActionPrioIntCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
          break;
      case bcmFieldActionTosNew:
      case bcmFieldActionTosCopy:
      case bcmFieldActionTosCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionDscpNew:
      case bcmFieldActionDscpCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionRpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;
      case bcmFieldActionYpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;
      case bcmFieldActionGpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;
      case bcmFieldActionEcnNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;

      case bcmFieldActionCopyToCpu:
      case bcmFieldActionCopyToCpuCancel:
      case bcmFieldActionSwitchToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
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
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
          break;
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionYpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
          break;
      case bcmFieldActionGpCopyToCpu:
      case bcmFieldActionGpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
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
      case bcmFieldActionRedirect:
      case bcmFieldActionRedirectTrunk:
      case bcmFieldActionRedirectCancel:
      case bcmFieldActionRedirectPbmp:
      case bcmFieldActionEgressMask:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirect);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectTrunk);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectPbmp);
          _FP_ACTIONS_CONFLICT(bcmFieldActionEgressMask);
          break;
      case bcmFieldActionMirrorIngress:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
          break;
      case bcmFieldActionMirrorEgress:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
          break;
      case bcmFieldActionMirrorOverride:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
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
      case bcmFieldActionUpdateCounter:
      case bcmFieldActionMeterConfig:
      case bcmFieldActionColorIndependent:
          break;
      default:
          break;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_fb_action_support_check
 *
 * Purpose:
 *     Check if action is supported by device.
 *
 * Parameters:
 *     unit   -(IN)BCM device number
 *     f_ent  -(IN)Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     result -(OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 *
 * Returns:
 *     BCM_E_XXX   
 */
STATIC int
_field_fb_action_support_check(int unit, _field_entry_t *f_ent,
                               bcm_field_action_t action, int *result)
{
    /* Input parameters check */
    if ((NULL == f_ent) || (NULL == result)) {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_FIREBOLT2_SUPPORT 
    if (SOC_IS_FIREBOLT2(unit)) {
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            switch (action) {
              case bcmFieldActionDrop:
              case bcmFieldActionDropCancel:
              case bcmFieldActionDscpNew:
              case bcmFieldActionRpDrop:
              case bcmFieldActionYpDrop:
              case bcmFieldActionGpDrop:
              case bcmFieldActionRpDropCancel:
              case bcmFieldActionYpDropCancel:
              case bcmFieldActionGpDropCancel:
              case bcmFieldActionRpDscpNew:
              case bcmFieldActionYpDscpNew:
              case bcmFieldActionGpDscpNew:
              case bcmFieldActionRpOuterVlanPrioNew:
              case bcmFieldActionYpOuterVlanPrioNew:
              case bcmFieldActionGpOuterVlanPrioNew:
              case bcmFieldActionInnerVlanPrioNew:
              case bcmFieldActionOuterVlanPrioNew:
              case bcmFieldActionInnerVlanNew:
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionOuterTpidNew:
              case bcmFieldActionUpdateCounter:
              case bcmFieldActionMeterConfig:
                  *result = TRUE;
                  break;
              default:
                  *result = FALSE;
            }
            return (BCM_E_NONE);
        }
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            switch (action) {
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionVlanAdd:
              case bcmFieldActionOuterVlanPrioNew:
              case bcmFieldActionDrop:
              case bcmFieldActionDropCancel:
              case bcmFieldActionCopyToCpu:
              case bcmFieldActionVrfSet:
              case bcmFieldActionClassSet:
                  *result = TRUE;
                  break;
              default:
                  *result = FALSE;
            }
            return (BCM_E_NONE);
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    switch (action) {
        case bcmFieldActionCosQNew:
        case bcmFieldActionCosQCpuNew:
        case bcmFieldActionColorIndependent:
            *result = 
                (soc_feature(unit, soc_feature_field_wide)) ? TRUE : FALSE;
            break;
        case bcmFieldActionMirrorOverride:
            *result = (soc_feature(unit, soc_feature_field_mirror_ovr)) ? \
                                                TRUE : FALSE;
            break;
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionTosNew:
        case bcmFieldActionTosCopy:
        case bcmFieldActionTosCancel:
        case bcmFieldActionDscpNew:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionSwitchToCpuCancel:
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionEgressMask:
        case bcmFieldActionDrop:
        case bcmFieldActionDropCancel:
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeVlanCancel:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3ChangeMacDaCancel:
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchCancel:
        case bcmFieldActionAddClassTag:
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionRpDrop:
        case bcmFieldActionRpDropCancel:
        case bcmFieldActionRpDropPrecedence:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionYpDrop:
        case bcmFieldActionYpDropCancel:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionUpdateCounter:
        case bcmFieldActionMeterConfig:
        case bcmFieldActionGpDrop:
        case bcmFieldActionGpDropCancel:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionGpCopyToCpu:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionGpDscpNew:
            *result = TRUE;
            break;
        case bcmFieldActionEcnNew:
            *result = (SOC_MEM_FIELD_VALID(unit, FP_POLICY_TABLEm, ECN_CNGf)) ?
                TRUE : FALSE;
            break;
        default:
            *result = FALSE;
    }

    return (BCM_E_NONE);
}

/*
 * Function: 
 *   _bcm_field_fb_stat_hw_mode_to_bmap 
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
_bcm_field_fb_stat_hw_mode_to_bmap(int unit, uint8 mode, 
                                   _field_stage_id_t stage_id, 
                                   uint32 *hw_bmap, uint8 *hw_entry_count)
{
    uint32 value = 0;
    /* Input parameters check. */
    if ((NULL == hw_bmap) || (NULL == hw_entry_count)) {
        return (BCM_E_PARAM);
    }

#if defined (BCM_FIREBOLT2_SUPPORT)
    if (_BCM_FIELD_STAGE_EGRESS == stage_id) {
        if (1 == mode) {
            value |= (1 << (int)bcmFieldStatPackets);
            *hw_entry_count = 1;
        }
        *hw_bmap = value;
        return (BCM_E_NONE);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    switch (mode) {
      case (0x1):
      case (0x2):
          value |= (1 << (int)bcmFieldStatPackets);
          *hw_entry_count = 1;
          break;
      case (0x9):
      case (0xa):
          value |= (1 << (int)bcmFieldStatBytes);
          *hw_entry_count = 1;
      case (0x3):
          value |= (1 << (int)bcmFieldStatRedPackets);
          value |= (1 << (int)bcmFieldStatNotRedPackets);
          *hw_entry_count = 2;
          break;
      case (0xb):
          value |= (1 << (int)bcmFieldStatRedBytes);
          value |= (1 << (int)bcmFieldStatNotRedBytes);
          *hw_entry_count = 2;
          break;
      case (0x4):
          value |= (1 << (int)bcmFieldStatGreenPackets);
          value |= (1 << (int)bcmFieldStatNotGreenPackets);
          *hw_entry_count = 2;
          break;
      case (0xc):
          value |= (1 << (int)bcmFieldStatGreenBytes);
          value |= (1 << (int)bcmFieldStatNotGreenBytes);
          *hw_entry_count = 2;
          break;
      case (0x5):
          value |= (1 << (int)bcmFieldStatGreenPackets);
          value |= (1 << (int)bcmFieldStatRedPackets);
          value |= (1 << (int)bcmFieldStatNotYellowPackets);
          *hw_entry_count = 2;
          break;
      case (0xd):
          value |= (1 << (int)bcmFieldStatGreenBytes);
          value |= (1 << (int)bcmFieldStatRedBytes);
          value |= (1 << (int)bcmFieldStatNotYellowBytes);
          *hw_entry_count = 2;
          break;
      case (0x6):
          value |= (1 << (int)bcmFieldStatGreenPackets);
          value |= (1 << (int)bcmFieldStatYellowPackets);
          value |= (1 << (int)bcmFieldStatNotRedPackets);
          *hw_entry_count = 2;
          break;
      case (0xe):
          value |= (1 << (int)bcmFieldStatGreenBytes);
          value |= (1 << (int)bcmFieldStatYellowBytes);
          value |= (1 << (int)bcmFieldStatNotRedBytes);
          *hw_entry_count = 2;
          break;
      case (0x7):
          value |= (1 << (int)bcmFieldStatRedPackets);
          value |= (1 << (int)bcmFieldStatYellowPackets);
          value |= (1 << (int)bcmFieldStatNotGreenPackets);
          *hw_entry_count = 2;
          break;
      case (0xf):
          value |= (1 << (int)bcmFieldStatRedBytes);
          value |= (1 << (int)bcmFieldStatYellowBytes);
          value |= (1 << (int)bcmFieldStatNotGreenBytes);
          *hw_entry_count = 2;
          break;
      default:
          *hw_entry_count = 0;
    }
    *hw_bmap = value;
    return (BCM_E_NONE);
}

#if defined(BCM_BRADLEY_SUPPORT)
/*
 * Function:
 *     _field_br_counter_write
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_write(int unit, soc_mem_t mem, int idx, uint32 *buf,
                      _field_counter64_collect_t *cntrs64_buf,
                      uint64 *value)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Set accumulated SW counter. */
    COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
    COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*value));
    COMPILER_64_SET(cntrs64_buf->last_hw_value, 
                    COMPILER_64_HI(*value) & 0xf, COMPILER_64_LO(*value));

    /* 36 LSB set as a last hw value. */
    hw_val[0] = COMPILER_64_LO(cntrs64_buf->last_hw_value);
    hw_val[1] = COMPILER_64_HI(cntrs64_buf->last_hw_value);
    soc_mem_field_set(unit, mem, buf, COUNTERf, hw_val);
    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, buf);
}


/*
 * Function:
 *     _field_br_counter_set
 * Purpose:
 *     Set accumulated sw & hw counters. 
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_set(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter64_collect_t *cntrs64_buf;  /* Sw packet counter value    */
    uint64                     *value;        /* Counter accumulated value. */
    int                        rv;            /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    value = (NULL == packet_count) ? byte_count : packet_count;

    /* Write X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        rv = _field_br_counter_write(unit, counter_x_mem, idx, mem_x_buf,
                                     cntrs64_buf, value);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Extract Y pipeline counter value. */
    if (NULL != mem_y_buf) {
        COMPILER_64_ZERO(*value);
        cntrs64_buf = &stage_fc->_field_y64_counters[idx];
        rv = _field_br_counter_write(unit, counter_y_mem, idx, mem_y_buf,
                                     cntrs64_buf, value);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_br_counter_read
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_read(int unit, soc_mem_t mem, uint32 *buf,
                      _field_counter64_collect_t *cntrs64_buf,
                      uint64 *count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Byte counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_mem_field_get(unit, mem, buf, COUNTERf, hw_val);

    _bcm_field_36bit_counter_update(unit, hw_val, cntrs64_buf);
    if (NULL != count) {
        COMPILER_64_ADD_64(*count, cntrs64_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_br_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_get(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter64_collect_t *cntrs64_buf;  /* Sw packet counter value    */
    uint64                     value;         /* Counter accumulated value. */
    int                        rv;            /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    sal_memset(&value, 0, sizeof(uint64));

    /* Extract X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        rv = _field_br_counter_read(unit, counter_x_mem, mem_x_buf,
                                   cntrs64_buf, &value);
    }

    /* Extract Y pipeline counter value. */
    if (NULL != mem_y_buf) {
        cntrs64_buf = &stage_fc->_field_y64_counters[idx];
        rv = _field_br_counter_read(unit, counter_y_mem, mem_y_buf,
                                   cntrs64_buf, &value);
    }

    /* Return accumulated value to the caller routine. */
    if (NULL != packet_count) {
        COMPILER_64_OR(*packet_count, value);
    }
    if (NULL != byte_count) {
        COMPILER_64_OR(*byte_count, value);
    }
    return (BCM_E_NONE);
}
#endif /* BCM_BRADLEY_SUPPORT */


/*
 * Function:
 *     _bcm_field_fb_counter_set
 * Purpose:
 *     Set accumulated sw & hw counters.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb_counter_set(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    uint32                     hw_val[2];     /* Parsed field counter value.*/
    uint64                     *value;        /* Programmed value.          */

    /* Input parameters check. */
    if ((NULL == mem_x_buf) || (INVALIDm == counter_x_mem) || \
        (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    hw_val[0] = hw_val[1] = 0;
    /* Set accumulated sw counter. */
    value = (NULL == packet_count) ? byte_count : packet_count;
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    COMPILER_64_ZERO(cntrs32_buf->accumulated_counter);
    COMPILER_64_OR(cntrs32_buf->accumulated_counter, *value);
    COMPILER_64_TO_32_LO(cntrs32_buf->last_hw_value, *value);
    cntrs32_buf->last_hw_value &= 0x1fffffff; /* 29 bits */

    /* Set lsb hw counter. */
    hw_val[0] = cntrs32_buf->last_hw_value;
    hw_val[1] = 0;
    soc_mem_field_set(unit, counter_x_mem, mem_x_buf, COUNTERf, hw_val);
    return soc_mem_write(unit, counter_x_mem, MEM_BLOCK_ALL, idx, mem_x_buf);
}


/*
 * Function:
 *     _bcm_field_fb_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb_counter_get(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    uint32                     hw_val[2];     /* Parsed field counter value.*/

    /* Input parameters check. */
    if ((NULL == mem_x_buf) || (INVALIDm == counter_x_mem) || \
        (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    hw_val[0] = hw_val[1] = 0;
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    soc_mem_field_get(unit, counter_x_mem, mem_x_buf, COUNTERf, hw_val);
    _bcm_field_32bit_counter_update(unit, hw_val, cntrs32_buf);
    /* Return counter value to caller if (out) pointer was provided. */ 
    if (NULL != byte_count) {
        COMPILER_64_OR(*byte_count, cntrs32_buf->accumulated_counter);
    }
    if (NULL != packet_count) {
        COMPILER_64_OR(*packet_count, cntrs32_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_counter_get
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
_bcm_field_fb_stat_index_get(int unit, _field_stat_t *f_st, 
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

    /* Stat entity indexes are adjusted for policy table counter pairs. */
#if defined (BCM_FIREBOLT2_SUPPORT) 
    if (SOC_IS_FIREBOLT2(unit) && (_BCM_FIELD_STAGE_EGRESS == f_st->stage_id)) {
        /* 5651x doesn't have counter pairs on stage egress. */
        counter_tbl_idx = f_st->hw_index;
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
    {
        counter_tbl_idx = f_st->hw_index << 1;
    }

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
            case (0xa):
                index1 = counter_tbl_idx + 1;
                break;
            case (0x1):
            case (0x9):
                index1 = counter_tbl_idx; 
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatGreenBytes:
          switch (f_st->hw_mode) {
            case (0xc):
            case (0xd):
            case (0xe):
                index1 = counter_tbl_idx + 1;   /* Green bytes. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatGreenPackets:
          switch (f_st->hw_mode) {
            case (0x4):
            case (0x5):
            case (0x6):
                index1 = counter_tbl_idx + 1;   /* Green packets. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatYellowBytes:
          switch (f_st->hw_mode) {
            case (0xe):
            case (0xf):
                index1 = counter_tbl_idx;   /* Yellow bytes. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatYellowPackets:
          switch (f_st->hw_mode) {
            case (0x6):
            case (0x7):
                index1 = counter_tbl_idx;   /* Yellow packets. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatRedBytes:
          switch (f_st->hw_mode) {
            case (0xb):
            case (0xf):
                index1 = counter_tbl_idx + 1;     /* Red bytes. */
                break;
            case (0xd):
                index1 = counter_tbl_idx;         /* Red bytes. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatRedPackets:
          switch (f_st->hw_mode) {
            case (0x3):
            case (0x7):
                index1 = counter_tbl_idx + 1;     /* Red packets. */
                break;
            case (0x5):
                index1 = counter_tbl_idx;         /* Red packets. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatNotGreenBytes:
          switch (f_st->hw_mode) {
            case (0xc):
                index1 = counter_tbl_idx;   /* Not Green bytes. */
                break;
            case (0xf):
                index1 = counter_tbl_idx + 1;    /* Red bytes.    */
                index2 = counter_tbl_idx;        /* Yellow bytes. */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatNotGreenPackets:
          switch (f_st->hw_mode) {
            case (0x4):
                index1 = counter_tbl_idx;        /* Green packets. */
                break;
            case (0x7):
                index1 = counter_tbl_idx + 1;   /*  Red packets.    */
                index2 = counter_tbl_idx;       /*  Yellow packets. */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatNotYellowBytes:
          switch (f_st->hw_mode) {
            case (0xd):
                index1 = counter_tbl_idx;           /* Green bytes. */
                index2 = counter_tbl_idx + 1;       /* Red bytes.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatNotYellowPackets:
          switch (f_st->hw_mode) {
            case (0x5):
                index1 = counter_tbl_idx + 1;      /* Green packets. */
                index2 = counter_tbl_idx;          /* Red packets.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatNotRedBytes:
          switch (f_st->hw_mode) {
            case (0xb):
                index1 = counter_tbl_idx;         /* Not red bytes. */
                break;
            case (0xe):
                index1 = counter_tbl_idx + 1;     /* Yellow bytes.  */
                index2 = counter_tbl_idx;         /* Green bytes.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatNotRedPackets:
          switch (f_st->hw_mode) {
            case (0x3):
                index1 = counter_tbl_idx;         /* Not red packets. */
                break;
            case (0x6):
                index1 = counter_tbl_idx;         /* Yellow packets.  */
                index2 = counter_tbl_idx + 1;     /* Green packets.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate  counter table entry index. */
    if (_FP_INVALID_INDEX != index1) {
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) && (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id)) {
            *idx1 = index1;
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            *idx1 = stage_fc->slices[f_st->pool_index].start_tcam_idx + index1;
        }
    } else {
        *idx1 = _FP_INVALID_INDEX;
    }

    if (_FP_INVALID_INDEX != index2) {
        *idx2 = stage_fc->slices[f_st->pool_index].start_tcam_idx + index2;
    } else {
        *idx2 = _FP_INVALID_INDEX;
    }
    *out_flags = flags;
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function: _bcm_field_fb2_qualify_ttl
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Field entry array.
 *     data - Data to qualify with 
 *     mask - Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_fb2_qualify_ttl(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    _field_entry_t *f_ent;
    _field_group_t *fg = NULL;
    uint8 result;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    fg = f_ent->group;   
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (result) {
        return _field_qualify32(unit, entry, bcmFieldQualifyTtl, data, mask);
    }

    /* Only intraslice groups support encoded TTL values. */
    if (0 == (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        return BCM_E_PARAM;
    }

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch (data & mask) {
      case 0:
          data = 0x0;
          mask &= 0x3;
          break;
      case 1:
          data = 0x1;
          mask &= 0x3;
          break;
      case 255:
          data = 0x3;
          mask &= 0x3;
          break;
      default:
          data = 0x2;
          mask &= 0x3;    
    }
    /* Write the TTL_ENCODE data and mask - only for FB 2 */
    rv = _bcm_field_qual_tcam_key_mask_get(unit, f_ent + 1);
    BCM_IF_ERROR_RETURN(rv);

    f_ent[1].tcam.key[0] &= ~(mask << 6);
    f_ent[1].tcam.key[0] |= data << 6;

    f_ent[1].tcam.mask[0] &= ~(mask << 6);
    f_ent[1].tcam.mask[0] |= mask << 6;
    f_ent[1].flags |= _FP_ENTRY_DIRTY;
    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_fb2_qualify_ttl_get
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Field entry array.
 *     data - Data to qualify with 
 *     mask - Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_fb2_qualify_ttl_get(int unit, bcm_field_entry_t entry,
                           uint8 *data, uint8 *mask)
{
    _field_entry_t *f_ent;
    _field_group_t *fg = NULL;
    uint8 free_tcam = FALSE;
    uint8 result;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    fg = f_ent->group;   
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (result) {
        return _bcm_field_entry_qualifier_uint8_get(unit, entry, 
                                                    bcmFieldQualifyTtl, 
                                                    data, mask);
    }

    /* Only intraslice groups support encoded TTL values. */
    if (0 == (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        return BCM_E_PARAM;
    }

    if (NULL == f_ent[1].tcam.key) {
        free_tcam = TRUE;
    }

    /* Read  the TTL_ENCODE data and mask - only for FB 2 */
    rv = _bcm_field_qual_tcam_key_mask_get(unit, f_ent + 1);
    BCM_IF_ERROR_RETURN(rv);

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch ((f_ent[1].tcam.key[0] >> 6) & 0x3) {
      case 0:
          *data = 0x0;
          *mask = 0xff;
          break;
      case 1:
          *data = 0x1;
          *mask &= 0xff;
          break;
      case 3:
          *data = 0xff;
          *mask &= 0xff;
          break;
      default:
          return (BCM_E_UNAVAIL);
    }
    if (free_tcam) {
        _bcm_field_qual_tcam_key_mask_free(unit, f_ent + 1);
    }
    return BCM_E_NONE;
}
#endif /* BCM_FIREBOLT2_SUPPORT */

/*
 * Function:
 *     _bcm_field_fb_selcode_get
 * Purpose:
 *     Finds a 4-tuple of select encodings that will satisfy the
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
_bcm_field_fb_selcode_get(int unit, _field_stage_t *stage_fc, 
                      bcm_field_qset_t *qset_req,
                      _field_group_t *fg)
{
    int  rv;                     /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Egress qualifiers are selected based on Key. */
    switch (stage_fc->stage_id) {
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_EGRESS:
          rv =  _field_fb_egress_selcode_get(unit, stage_fc, qset_req, fg);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default:
          rv =  _bcm_field_selcode_get(unit, stage_fc, qset_req, fg);
    }
    return (rv);
}

/*
 * Function: _bcm_field_fb_udf_ipprotocol_delete_all
 *
 * Purpose:
 *     Delete all the UDF ip protocol match values.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ipprotocol_delete_all(int unit)
{
    int index;

    if (0 == SOC_REG_IS_VALID(unit, UDF_IPPROTO_MATCHr)) {
        return (BCM_E_NONE);
    }

    for (index = 0; index < _FP_DATA_IP_PROTOCOL_MAX; index++) {
        SOC_IF_ERROR_RETURN(WRITE_UDF_IPPROTO_MATCHr(unit, index, 0));
    }
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ip_protocol_set
 *
 * Purpose:
 *     Set the UDF Ip Protocol match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     flags     - (IN) IP version match is enabled
 *     proto     - (IN) IP protocol id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ipprotocol_set(int unit, int index, uint32 flags, uint8 proto)
{
    uint32 reg_val = 0;
    uint32 ip4_enable = 0;
    uint32 ip6_enable = 0;
    uint32 protocol = 0;

    if (0 == SOC_REG_IS_VALID(unit, UDF_IPPROTO_MATCHr)) {
        return (BCM_E_UNAVAIL); 
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_IP_PROTOCOL_MAX <= index)) {
        return (BCM_E_PARAM);
    }

    protocol = proto;
    switch (flags) {
      case 0:
          break;
      case BCM_FIELD_DATA_FORMAT_IP4:
          ip4_enable = TRUE;
          break;
      case BCM_FIELD_DATA_FORMAT_IP6:
          ip6_enable = TRUE;
          break;
      case (BCM_FIELD_DATA_FORMAT_IP4 | BCM_FIELD_DATA_FORMAT_IP6):
          ip4_enable = ip6_enable = TRUE;
          break;
      default: 
          return (BCM_E_PARAM);
    }

    /*  Update UDF_IPPROTO_MATCHr register. */
    SOC_IF_ERROR_RETURN(READ_UDF_IPPROTO_MATCHr(unit, index, &reg_val));
    soc_reg_field_set(unit, UDF_IPPROTO_MATCHr, &reg_val, IPV4ENABLEf, ip4_enable);
    soc_reg_field_set(unit, UDF_IPPROTO_MATCHr, &reg_val, IPV6ENABLEf, ip6_enable);
    soc_reg_field_set(unit, UDF_IPPROTO_MATCHr, &reg_val, PROTOCOLf, protocol); 
    SOC_IF_ERROR_RETURN(WRITE_UDF_IPPROTO_MATCHr(unit, index, reg_val));

    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ip_protocol_get
 *
 * Purpose:
 *     Get the UDF Ip Protocol match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     flags     - (OUT) IP version match is enabled
 *     proto     - (OUT) IP protocol id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ipprotocol_get(int unit, int index, uint32 *flags, uint8 *proto)
{
    uint32              reg_val = 0;

    /* Input parameters check. */
    if ((NULL == flags) || (NULL == proto)) {
        return (BCM_E_PARAM);
    }

    if (0 == SOC_REG_IS_VALID(unit, UDF_IPPROTO_MATCHr)) {
        return (BCM_E_UNAVAIL); 
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_IP_PROTOCOL_MAX <= index)) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_UDF_IPPROTO_MATCHr(unit, index, &reg_val));

    *flags = 0;
    if (soc_reg_field_get(unit, UDF_IPPROTO_MATCHr, reg_val, IPV4ENABLEf)) {
        *flags |= BCM_FIELD_DATA_FORMAT_IP4;
    }
    if (soc_reg_field_get(unit, UDF_IPPROTO_MATCHr, reg_val, IPV6ENABLEf)) {
        *flags |= BCM_FIELD_DATA_FORMAT_IP6;
    }

    *proto = soc_reg_field_get(unit, UDF_IPPROTO_MATCHr, reg_val, PROTOCOLf);

    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ethertype_delete_all
 *
 * Purpose:
 *     Delete all the UDF ethertype match values.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ethertype_delete_all(int unit)
{
    int index;

    if (0 == SOC_REG_IS_VALID(unit, UDF_ETHERTYPE_MATCHr)) {
        return (BCM_E_NONE);
    }

    for (index = 0; index < _FP_DATA_ETHERTYPE_MAX; index++) {
        SOC_IF_ERROR_RETURN(WRITE_UDF_ETHERTYPE_MATCHr(unit, index, 0));
    }
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ethertype_set
 *
 * Purpose:
 *     Set the UDF Ethertype match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     frametype - (IN) BCM_FIELD_DATA_FORMAT_L2_XXX
 *     ethertype - (IN) Ethertype value to match
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ethertype_set(int unit, int index,
                         bcm_port_frametype_t frametype,
                         bcm_port_ethertype_t ethertype) 
{
    uint32 reg_val;
    uint32 enable;
    uint32 frame_type;

    if (0 == SOC_REG_IS_VALID(unit, UDF_ETHERTYPE_MATCHr)) {
        return (BCM_E_UNAVAIL);
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_ETHERTYPE_MAX  <= index)) {
        return (BCM_E_PARAM);
    }

    switch (frametype) {
      case 0:
          enable = FALSE;
          ethertype = 0;
          frame_type = 0;
          break;
      case BCM_PORT_FRAMETYPE_ETHER2:
          frame_type = 0;
          enable = TRUE;
          break;
      case BCM_PORT_FRAMETYPE_SNAP:
          frame_type = 1;
          enable = TRUE;
          break;
      case BCM_PORT_FRAMETYPE_LLC:
          frame_type = 2;
          enable = TRUE;
          break;
      default:
          return (BCM_E_PARAM);
    }

    /*  Update UDF_ETHERTYPE_MATCHr register. */
    SOC_IF_ERROR_RETURN(READ_UDF_ETHERTYPE_MATCHr(unit, index, &reg_val));
    soc_reg_field_set(unit, UDF_ETHERTYPE_MATCHr, &reg_val, ENABLEf, enable);
    soc_reg_field_set(unit, UDF_ETHERTYPE_MATCHr, &reg_val, L2_PACKET_FORMATf, 
                      frame_type);
    soc_reg_field_set(unit, UDF_ETHERTYPE_MATCHr, &reg_val, ETHERTYPEf, 
                      (uint32)ethertype);
    SOC_IF_ERROR_RETURN(WRITE_UDF_ETHERTYPE_MATCHr(unit, index, reg_val));
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ethertype_get
 *
 * Purpose:
 *     Set the UDF Ethertype match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     frametype - (OUT) BCM_FIELD_DATA_FORMAT_L2_XXX
 *     ethertype - (OUT) Ethertype value to match
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ethertype_get(int unit, int index,
                         bcm_port_frametype_t *frametype,
                         bcm_port_ethertype_t *ethertype) 
{
    uint32  reg_val = 0;
    uint32  enable;
    uint32  frame_type;     

    if (0 == SOC_REG_IS_VALID(unit, UDF_ETHERTYPE_MATCHr)) {
        return (BCM_E_UNAVAIL);
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_ETHERTYPE_MAX  <= index)) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check */
    if ((NULL == frametype) || (NULL == ethertype)) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_UDF_ETHERTYPE_MATCHr(unit, index, &reg_val));

    enable = soc_reg_field_get(unit, UDF_ETHERTYPE_MATCHr, reg_val,
                               ENABLEf);
    if (enable) {
        frame_type = soc_reg_field_get(unit, UDF_ETHERTYPE_MATCHr, reg_val,
                                       L2_PACKET_FORMATf);

        switch (frame_type) {
          case 0:
              *frametype = BCM_PORT_FRAMETYPE_ETHER2;
              break;
          case 1:
              *frametype = BCM_PORT_FRAMETYPE_SNAP;
              break;
          case 2:
              *frametype = BCM_PORT_FRAMETYPE_LLC;
              break;
          default:
              return (BCM_E_PARAM);
        }

        *ethertype = soc_reg_field_get(unit, UDF_ETHERTYPE_MATCHr, reg_val,
                                       ETHERTYPEf);
    } else {
        *frametype = 0;
        *ethertype = 0;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_udf_idx_to_l2_format
 * Purpose:
 *      Translate udf offset index l2 portion to packet l2 format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      l2       - (OUT)Packet L2 format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_idx_to_l2_format(int unit, int idx, uint16 *l2)
{
    int l2_idx;    /* L2 portion of udf offset table index. */

    /* Input parameters check. */
    if (NULL == l2) {
        return (BCM_E_PARAM);
    }

    if (SOC_IS_TR_VL(unit)) {
        l2_idx = idx >> 5; 
    } else if  (SOC_IS_SC_CQ(unit)) {
        l2_idx = idx >> 4; 
    } else {
        l2_idx = idx >> 3; 
    }

    switch (l2_idx & 0x3) {
      case 0:
          *l2 = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
          break;
      case 1:
          *l2 = BCM_FIELD_DATA_FORMAT_L2_SNAP;
          break;
      case 2:
          *l2 = BCM_FIELD_DATA_FORMAT_L2_LLC;
          break;
      case 3:
          *l2 = _BCM_FIELD_DATA_FORMAT_ETHERTYPE;
          break;
      default:        
          return (BCM_E_EMPTY);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _field_fb_udf_idx_to_vlan_format
 * Purpose:
 *      Translate udf offset index vlan portion to packet vlan tag format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      vlan     - (OUT)Packet vlan format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_idx_to_vlan_format(int unit, int idx, uint16 *vlan)
{
    int vlan_idx;    /* Vlan portion of udf offset table index. */

    /* Input parameters check. */
    if (NULL == vlan) {
        return (BCM_E_PARAM);
    }

    if (SOC_IS_TR_VL(unit)) {
        vlan_idx = idx >> 7; 
    } else if  (SOC_IS_SC_CQ(unit)) {
        vlan_idx = idx >> 6; 
    } else {
        vlan_idx = idx >> 5; 
    }

    switch (vlan_idx & 0x3) {
      case 0:
          *vlan = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
          break;
      case 1:
          *vlan = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
          break;
      case 2:
          *vlan = BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED;
          break;
      default:        
          return (BCM_E_EMPTY);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_udf_idx_to_ip_format
 * Purpose:
 *      Translate udf offset index ip portion to packet ip format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      pkt_fmt  - (OUT)Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_idx_to_ip_format(int unit, int idx, 
                            bcm_field_data_packet_format_t *pkt_fmt)
{
    int ip_idx;    /* Ip portion of udf offset table index. */

    /* Input parameters check. */
    if (NULL == pkt_fmt) {
        return (BCM_E_PARAM);
    }

    if (SOC_IS_TR_VL(unit)) {
        ip_idx = idx & 0x1f;  
    } else if  (SOC_IS_SC_CQ(unit)) {
        ip_idx = idx & 0xf; 
    } else {
        ip_idx = idx & 0x7; 
    }

    /* Init tunnel/ip header version to none. */
    pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
    pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;

    switch (ip_idx) {
      case 0:
          pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
          break;
      case 1:
          pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
          break;
      case 2:
          if (SOC_IS_EASYRIDER(unit)) {
              return (BCM_E_EMPTY);
          } else {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          }
          break;
      case 3:
          if (SOC_IS_EASYRIDER(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          } else {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          }
          break;
      case 4:
          if (SOC_IS_EASYRIDER(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          }
          break;
      case 5:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else if (SOC_IS_EASYRIDER(unit)) {
              return (BCM_E_EMPTY);
          } else {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0;
          }
          break;
      case 6:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          } else if (SOC_IS_EASYRIDER(unit)) {
              return (BCM_E_EMPTY);
          } else {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1;
          }
          break;
      case 7:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 8:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 9:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xa:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
              pkt_fmt->mpls = BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xb:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
              pkt_fmt->mpls = BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xc:
          break;
      case 0xd:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xe:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xf:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0x10:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      default:
          return (BCM_E_EMPTY);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_udf_offset_idx_to_packet_format
 * Purpose:
 *      Translate udf offset table index to packet format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      pkt_fmt  - (OUT)Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_offset_idx_to_packet_format(int unit, int idx,
                                bcm_field_data_packet_format_t *pkt_fmt)
{
    int rv;               /* Operation return status. */

    /* Input parameters check. */
    if (NULL == pkt_fmt) {
        return (BCM_E_PARAM);
    }

    /* parse L2 format */
    rv = _field_fb_udf_idx_to_l2_format(unit, idx, &pkt_fmt->l2);
    BCM_IF_ERROR_RETURN(rv);

    /* Ignore ethertype based indexes. */
    if (pkt_fmt->l2 == _BCM_FIELD_DATA_FORMAT_ETHERTYPE) {
        return (BCM_E_EMPTY);
    }

    /* parse vlan tag format */
    rv = _field_fb_udf_idx_to_vlan_format(unit, idx, &pkt_fmt->vlan_tag);
    BCM_IF_ERROR_RETURN(rv);

    /* parse L3 tunnel type/ Ip headers  format */
    rv = _field_fb_udf_idx_to_ip_format(unit, idx, pkt_fmt);
    BCM_IF_ERROR_RETURN(rv);

    /* Ignore ip protcol based indexes. */
    if ((pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0) ||
        (pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1) ||
        (pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0) ||
        (pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1)) {
        return (BCM_E_EMPTY);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_data_offset_install
 * Purpose:
 *     Write the info in udf_spec to hardware
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     f_dq     - (IN) Data qualifier structure.
 *     tbl_idx  - (IN) FP_UDF_OFFSETm table index.
 *     offset   - (IN) Word offset value.
 * Returns:
 *     BCM_E_PARAM - UDF number or User number out of range
 *     BCM_E_NONE
 * Notes:
 *     Calling function should have unit's lock.
 */
STATIC int
_field_fb_data_offset_install(int unit, _field_data_qualifier_t *f_dq, 
                           int tbl_idx, int offset)
{
    fp_udf_offset_entry_t  tbl_entry;   /* HW table buffer.               */ 
    uint32                 word_offset; /* Offset iterator.               */      
    int                    idx;         /* Data qualifier words iterator. */
    uint32                 value;       /* HW buffer value.               */


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

    /* Input parameters check. */
    if (NULL == f_dq) {
        return (BCM_E_PARAM);
    }

    /* index sanity check. */
    if ((tbl_idx < soc_mem_index_min(unit, FP_UDF_OFFSETm)) || 
        (tbl_idx > soc_mem_index_max(unit, FP_UDF_OFFSETm))) {
        return (BCM_E_PARAM);
    }

    /* Read udf offset table enty.*/
    BCM_IF_ERROR_RETURN
        (READ_FP_UDF_OFFSETm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_entry));

    word_offset = (offset < 0) ? 0 : (uint32)offset;

    /* Find a proper word to insert the data. */
    for (idx = 0; idx <= _FP_DATA_DATA1_WORD_MAX; idx++) {
        if (0 == (f_dq->hw_bmap & (1 << idx))) {
            continue;
        }

        if (word_offset > 31) {
            return BCM_E_PARAM;
        }

        soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, off_field[idx], 
                                       word_offset);

        /* Set the UDFn_ADD_IPV4_OPTIONSm bit accordingly */ 
        value = (f_dq->flags &
            BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST) ? TRUE : FALSE;
        soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                       option_field[idx], value);

        /* Set the UDFn_ADD_IPV4_OPTIONSm bit accordingly */ 
        if (SOC_MEM_FIELD_VALID(unit, FP_UDF_OFFSETm, UDF1_ADD_GRE_OPTIONS0f)) {
            value = (f_dq->flags &
                     BCM_FIELD_DATA_QUALIFIER_OFFSET_GRE_OPTIONS_ADJUST) ? TRUE : FALSE;
            soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                           gre_option_field[idx], value);
        }

        if (offset >= 0) {
            word_offset++;
        }
    }

    /* Write back udf offset table enty.*/
    BCM_IF_ERROR_RETURN
        (WRITE_FP_UDF_OFFSETm(unit, MEM_BLOCK_ALL, tbl_idx, &tbl_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _field_fb_data_qualifier_ethertype_install
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      f_dq       - (IN) Data qualifier descriptor.
 *      etype_idx  - (IN) Ethertype index. 
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_data_qualifier_ethertype_install (int unit,
                                         _field_data_qualifier_t *f_dq,
                                         uint8 idx,
                                         _field_data_ethertype_t *etype)
{
    bcm_port_frametype_t       frametype; /* HW register encoding.       */ 
    int                        idx_mask;  /* Ethertype id index mask.    */ 
    int                        offset;    /* Adjusted word offset.       */ 
    int                        tbl_idx;   /* Udf offset table iterator.  */
    uint16                     l2;        /* L2 packet format.           */
    uint16                     vlan_tag;  /* Vlan packet format.         */
    
    int                        rv;        /* Operation return status.    */


    /* Input parameters check. */
    if ((NULL == f_dq) || (NULL == etype)) {
        return (BCM_E_PARAM);
    }
    /* Verify UDF offset memory is valid. */
    if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
        return (BCM_E_UNAVAIL);
    }

    switch (etype->l2) {
      case BCM_FIELD_DATA_FORMAT_L2_ETH_II:
          frametype = BCM_PORT_FRAMETYPE_ETHER2;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_SNAP:
          frametype = BCM_PORT_FRAMETYPE_SNAP;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_LLC:
          frametype = BCM_PORT_FRAMETYPE_LLC;
          break;
      default: 
          frametype = 0;
    } 


    /* Get ethertype index mask. */
    if (SOC_IS_TR_VL(unit)) {
        idx_mask = 0x1f;  /* Ethertype is 5 lower bits of udf index. */
    } else {
        idx_mask = 0x7;   /* Ethertype is 3 lower bits of udf index. */
    }

    /* Update ethertype match register. */
    rv = _bcm_field_fb_udf_ethertype_set(unit, idx, frametype, etype->ethertype);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate offset */
    offset = f_dq->offset + etype->relative_offset;
    if ((offset < 0) || (offset  + f_dq->length) >=  _FP_DATA_OFFSET_MAX) {
        return (BCM_E_PARAM);
    }
    offset = ((offset + 2) % 128) / 4; 

    for (tbl_idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); tbl_idx++) {

        if ((tbl_idx & idx_mask) != idx) {
            continue;
        }

        /* parse L2 format */
        rv = _field_fb_udf_idx_to_l2_format(unit, tbl_idx, &l2);
        BCM_IF_ERROR_RETURN(rv);

        /* Ignore not ethertype based indexes. */
        if (l2 != _BCM_FIELD_DATA_FORMAT_ETHERTYPE) {
            continue;
        }

        /* parse vlan tag format */
        rv = _field_fb_udf_idx_to_vlan_format(unit, tbl_idx, &vlan_tag);
        BCM_IF_ERROR_RETURN(rv);
        /* Compare packet vlan tag format. */
        if ((etype->vlan_tag != BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY) &&
            (etype->vlan_tag != vlan_tag)) {
            continue;
        }

        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, tbl_idx, offset);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_data_qualifier_ip_protocol_install
 * Purpose:
 *      Add ip_protocol based offset to data qualifier object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      f_dq        - (IN) Data qualifier descriptor.
 *      idx         - (IN) Ip protocol index. 
 *      proto       - (IN) Ip protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_data_qualifier_ip_protocol_install(int unit,
                                         _field_data_qualifier_t *f_dq,
                                         uint8 idx,
                                         _field_data_protocol_t *proto)
{
    bcm_field_data_packet_format_t pkt_fmt;
    int                        offset;    /* Adjusted word offset.       */ 
    int                        tbl_idx;   /* Udf offset table iterator.  */
    uint16                     l2;        /* L2 packet format.           */
    uint16                     vlan_tag;  /* Vlan packet format.         */
    int                        rv;        /* Operation return status.    */

     /* Input parameters check. */
     if ((NULL == f_dq) || (NULL == proto)) {
         return (BCM_E_PARAM);
     }

     /* Verify UDF offset memory is valid. */
     if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
         return (BCM_E_UNAVAIL);
     }

     /* Update ip_protocol match register. */
     rv = _bcm_field_fb_udf_ipprotocol_set(unit, idx, proto->flags, proto->ip);
     BCM_IF_ERROR_RETURN(rv);

     /* Calculate offset */
     offset = f_dq->offset + proto->relative_offset;
     if ((offset < 0) || (offset  + f_dq->length) >=  _FP_DATA_OFFSET_MAX) {
         return (BCM_E_PARAM);
     }
     offset = ((offset + 2) % 128) / 4; 

     for (tbl_idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
          tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); tbl_idx++) {
         /* parse L3 tunnel type/ Ip headers  format */
         rv = _field_fb_udf_idx_to_ip_format(unit, tbl_idx, &pkt_fmt);
         if (rv == BCM_E_EMPTY) {
             continue;
         }
         BCM_IF_ERROR_RETURN(rv);

         /* Ignore not ip protcol based indexes. */
         if (idx == 0) {
             if (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0) ||
                  (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP4))) &&  
                 (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0) ||
                   (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP6))))) {
                 continue;
             }
         } else {
             if (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1) ||
                  (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP4))) &&  
                 (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1) ||
                   (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP6))))) {
                 continue;
             }
         }

        /* parse l2 format */
        rv = _field_fb_udf_idx_to_l2_format(unit, tbl_idx, &l2);
        if (rv == BCM_E_EMPTY) {
            continue;
        }
        BCM_IF_ERROR_RETURN(rv);
        /* Compare packet l2 format. */
        if ((proto->l2  != BCM_FIELD_DATA_FORMAT_L2_ANY) &&
            (proto->l2  != l2)) {
            continue;
        }

        /* parse vlan tag format */
        rv = _field_fb_udf_idx_to_vlan_format(unit, tbl_idx, &vlan_tag);
        if (rv == BCM_E_EMPTY) {
            continue;
        }
        BCM_IF_ERROR_RETURN(rv);
        /* Compare packet vlan tag format. */
        if ((proto->vlan_tag != BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY) &&
            (proto->vlan_tag != vlan_tag)) {
            continue;
        }

        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, tbl_idx, offset);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _field_fb_packet_format_is_subset
 * Purpose:
 *      Check if master packet format is superset of 
 *      subset packet format.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      master     - (IN) Superset packet format.
 *      subset     - (IN) Subset packet format.
 *      cmp_result - (OUT)Comparison result.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_is_subset(int unit, 
                                  bcm_field_data_packet_format_t *master, 
                                  bcm_field_data_packet_format_t *subset, 
                                  uint8 *cmp_result) 
{
    /* Input parameters check. */
    if ((NULL == master) || (NULL == subset) || (NULL == cmp_result)) {
        return (BCM_E_PARAM);
    }

    /* Compare packet l2 format. */
    if ((master->l2 != BCM_FIELD_DATA_FORMAT_L2_ANY) &&
        (master->l2 != subset->l2)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare packet vlan tag format. */
    if ((master->vlan_tag  != BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY) &&
        (master->vlan_tag != subset->vlan_tag)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare packet tunnel type. */
    if ((master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_ANY) &&
        (master->tunnel != subset->tunnel)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare outer ip header. */
    if ((master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) &&  
        (master->outer_ip != BCM_FIELD_DATA_FORMAT_IP_ANY) && 
        (master->outer_ip != subset->outer_ip)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare inner ip header. */
    if ((master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_NONE) && 
        (master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) &&
        (master->inner_ip != BCM_FIELD_DATA_FORMAT_IP_ANY) &&
        (master->inner_ip != subset->inner_ip)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare number of mpls labels. */
    if ((master->tunnel == BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) && 
        (master->mpls != BCM_FIELD_DATA_FORMAT_MPLS_ANY) &&
        (master->mpls != subset->mpls)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }
    *cmp_result = TRUE;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_tunnel_shim_size_get
 * Purpose:
 *      Get  tunnel shim size  based on packet
 *      format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      pkt_fmt    - (IN) Packet format.
 *      *size      - (OUT)Outer L3 + tunnel shim size.
 * Returns:
 *      BCM_E_XXX
 * NOTE: 
 *      This api calculates basic header size. 
 *      Gre Options... are not handled here.
 *      Please set proper FIELD_DATA_QUALIFIER_XXX options adjust flag.
 */
STATIC int
_field_fb_packet_format_tunnel_shim_size_get(int unit, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               uint8 *size) 
{
    /* Input parameters check. */
    if ((NULL == pkt_fmt) || (NULL == size)) {
        return (BCM_E_PARAM);
    }

    if (BCM_FIELD_DATA_FORMAT_TUNNEL_GRE == pkt_fmt->tunnel) { 
        *size = 4;  /* Basic (no key/no checksum/no options). */
    } else {
        *size = 0;  /* IPv4 header size. */
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_ip_size_get
 * Purpose:
 *      Get outer l3 header + tunnel shim size  based on packet
 *      format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      ip         - (IN) BCM_FIELD_DATA_FORMAT_IP_XXX.
 *      *size      - (OUT)Outer L3 + tunnel shim size.
 * Returns:
 *      BCM_E_XXX
 * NOTE: 
 *      This api calculates basic header size. 
 *      IP options/Extension headers  ... are not handled here.
 *      Please set proper FIELD_DATA_QUALIFIER_XXX options adjust flag.
 */
STATIC int
_field_fb_packet_format_ip_size_get(int unit, uint16 ip, uint8 *size) 
{
    /* Input parameters check. */
    if (NULL == size) {
        return (BCM_E_PARAM);
    }

    if (BCM_FIELD_DATA_FORMAT_IP6 == ip) {
        *size = 40;  /* Ipv6 header size */
    } else if (BCM_FIELD_DATA_FORMAT_IP4 == ip) {
        *size = 20;  /* IPv4 header size. */
    } else if (BCM_FIELD_DATA_FORMAT_IP_NONE == ip) {
        *size = 0;
    } else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_mpls_labels_size_get
 * Purpose:
 *      Get mpls labels size based on packet format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      pkt_fmt    - (IN) Packet format.
 *      *size      - (OUT)mpls labels size.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_mpls_labels_size_get(int unit, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               uint8 *size) 
{
    /* Input parameters check. */
    if ((NULL == pkt_fmt) || (NULL == size)) {
        return (BCM_E_PARAM);
    }

    if (BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS != pkt_fmt->tunnel) {
        *size = 0;
    } else if (BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL == pkt_fmt->mpls) {
        *size = 4;
    } else if (BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS == pkt_fmt->mpls) {
        *size = 8;
    }  else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_l2_header_size_get
 * Purpose:
 *      Get l2 header size based on packet format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      pkt_fmt    - (IN) Packet format.
 *      *size      - (OUT)L2 header size.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_l2_header_size_get(int unit, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               uint8 *size) 
{
    uint8 tmp;

    /* Input parameters check. */
    if ((NULL == pkt_fmt) || (NULL == size)) {
        return (BCM_E_PARAM);
    }

    switch(pkt_fmt->l2) {
      case BCM_FIELD_DATA_FORMAT_L2_ETH_II:
      case BCM_FIELD_DATA_FORMAT_L2_LLC:
          tmp = 14;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_SNAP:
          tmp = 22;
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    switch(pkt_fmt->vlan_tag) {
      case BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG:
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED:
          tmp += 4;  
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED:
          tmp += 8;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    *size = tmp;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_offset_adjust
 * Purpose:
 *      Calculate word offset for data qualifier 
 *      based on offset based, relative offset & master offset.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      f_dq       - (IN) Data qualifier descriptor.
 *      pkt_fmt    - (IN) Installed packet format.
 *      offset     - (OUT)Final word offset.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_offset_adjust(int unit, 
                               _field_data_qualifier_t *f_dq, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               int *offset) 
{
    int tmp_offset;         /* Local calculation variable. */
    uint8 size;             /* Various headers sizes.      */
    int rv;                 /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == f_dq) || (NULL == pkt_fmt) || (NULL == offset)) {
        return (BCM_E_PARAM);
    }

    /* Calculate offset */
    tmp_offset = f_dq->offset + pkt_fmt->relative_offset;
    if ((tmp_offset < 0) || 
        (tmp_offset  + f_dq->length) >=  _FP_DATA_OFFSET_MAX) {
        return (BCM_E_PARAM);
    }

    switch (f_dq->offset_base) {
      case bcmFieldDataOffsetBasePacketStart:
          break;

      case bcmFieldDataOffsetBaseOuterL3Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* b) Add size of mpls labels. */  
          rv = _field_fb_packet_format_mpls_labels_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseInnerL3Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;


          /* b) Add size of outer l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->outer_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* c) Add size of GRE shim. */  
          rv =  _field_fb_packet_format_tunnel_shim_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseOuterL4Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* b) Add size of outer l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->outer_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseInnerL4Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;


          /* b) Add size of outer l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->outer_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* c) Add size of GRE shim. */  
          rv =  _field_fb_packet_format_tunnel_shim_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* d) Add size of inner l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->inner_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseHigigHeader:
          /* Hardcoded Higig header length is 12 bytes. */  
          tmp_offset += 0xc;
          break;

          /* Hardcoded Higig2 header length is 16 bytes. */  
      case bcmFieldDataOffsetBaseHigig2Header:
          tmp_offset += 0x10;
          break;

      default:
          return (BCM_E_PARAM);
    }

    *offset = ((tmp_offset + 2) % 128) / 4; 
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_field_fb_data_qualifier_packet_format_add
 * Purpose:
 *      Add packet format based offset to data qualifier object.
 * Parameters:
 *      unit          - (IN) Bcm device number.
 *      qual_id       - (IN) Data qualifier id.
 *      packet_format - (IN) Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_packet_format_add(int unit,  int qual_id,
                                bcm_field_data_packet_format_t *packet_format)
{
    bcm_field_data_packet_format_t pkt_fmt;   /* Packet format iterator.     */
    _field_data_qualifier_t        *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t                 *stage_fc; /* Stage field control.        */
    uint8                          cmp_result;/* Packet format compare result*/ 
    int                            offset;    /* Adjusted word offset.       */ 
    int                            idx;       /* Udf offset table iterator.  */
    int                            rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == packet_format)) {
        return (BCM_E_PARAM);
    }

    /* Verify UDF offset memory is valid. */
    if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
        return (BCM_E_UNAVAIL);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);


    for (idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); idx++) {
        sal_memset(&pkt_fmt, 0, sizeof (bcm_field_data_packet_format_t));

        /* Get packet format associated with an index. */
        rv = _field_fb_udf_offset_idx_to_packet_format(unit, idx, &pkt_fmt);
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_EMPTY) {
                /* Unused/ethertype/ip protocol  table index. */
                continue;
            }
            return (rv);
        }

        /* Check that idx packet format is subset of installed packet format. */
        rv = _field_fb_packet_format_is_subset(unit, packet_format, 
                                               &pkt_fmt, &cmp_result);
        BCM_IF_ERROR_RETURN(rv);
        if (FALSE == cmp_result) {
            continue;
        }

        /* Calculate base offset adjusted offset. */
        rv = _field_fb_packet_format_offset_adjust(unit, f_dq, 
                                                   packet_format, &offset);
        BCM_IF_ERROR_RETURN(rv);


        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, idx, offset);
        BCM_IF_ERROR_RETURN(rv);

    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_packet_format_delete
 * Purpose:
 *      Remove packet format based offset to data qualifier object.
 * Parameters:
 *      unit          - (IN) Bcm device number.
 *      qual_id       - (IN) Data qualifier id.
 *      packet_format - (IN) Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_packet_format_delete(int unit,  int qual_id,
                                bcm_field_data_packet_format_t *packet_format)
{
    bcm_field_data_packet_format_t pkt_fmt;   /* Packet format iterator.     */
    _field_data_qualifier_t        *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t                 *stage_fc; /* Stage field control.        */
    uint8                          cmp_result;/* Packet format compare result*/ 
    int                            idx;       /* Udf offset table iterator.  */
    int                            rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == packet_format)) {
        return (BCM_E_PARAM);
    }

    /* Verify UDF offset memory is valid. */
    if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
        return (BCM_E_UNAVAIL);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);


    for (idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); idx++) {
        sal_memset(&pkt_fmt, 0, sizeof (bcm_field_data_packet_format_t));

        /* Get packet format associated with an index. */
        rv = _field_fb_udf_offset_idx_to_packet_format(unit, idx, &pkt_fmt);
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_EMPTY) {
                /* Unused/ethertype/ip protocol  table index. */
                continue;
            }
            return (rv);
        }

        /* Check that idx packet format is subset of installed packet format. */
        rv = _field_fb_packet_format_is_subset(unit, packet_format, &pkt_fmt,
                                            &cmp_result);
        BCM_IF_ERROR_RETURN(rv);
        if (FALSE == cmp_result) {
            continue;
        }

        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, idx, -1);
        BCM_IF_ERROR_RETURN(rv);

    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_data_qualifier_ethertype_add
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_field_fb_data_qualifier_ethertype_add(int unit,  int qual_id,
                                           bcm_field_data_ethertype_t *etype)
{
    _field_data_ethertype_t    *etype_ptr;/* Ether type pointer.         */
    _field_stage_t             *stage_fc; /* Stage field control.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    int                        idx;       /* Ethertype allocation index. */
    int                        unused_idx;/* Unused ethertype index.     */
    int                        rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == etype)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /*
     *  Ethertype based qualifier offset must be 
     *  from the beginning of the packet. 
     */
    if (f_dq->offset_base != bcmFieldDataOffsetBasePacketStart) {
        return (BCM_E_UNAVAIL);
    }

    unused_idx = -1;
    for (idx = 0; idx < _FP_DATA_ETHERTYPE_MAX; idx++) {
        etype_ptr = stage_fc->data_ctrl->etype + idx;
        /* Find an entry with identical ethertype/l2format. */
        if ((etype->l2 == etype_ptr->l2) &&
            (etype->vlan_tag == etype_ptr->vlan_tag) &&
            (etype->ethertype == etype_ptr->ethertype) &&
            (etype_ptr->ref_count > 0)) {
            etype_ptr->ref_count++;
            rv = BCM_E_NONE;
            break;
        }

        /* Find unused entry. */
        if ((-1 == unused_idx) && (0 == etype_ptr->ref_count)) {
            unused_idx = idx;
        }
    }
    if (_FP_DATA_ETHERTYPE_MAX == idx) {
        /* No match found -> check for unused entry. */
        if (-1 == unused_idx) {
            return (BCM_E_RESOURCE);
        }
        idx = unused_idx;
        etype_ptr = stage_fc->data_ctrl->etype + idx;
        etype_ptr->ref_count = 1;
        etype_ptr->ethertype = etype->ethertype;
        etype_ptr->l2 = etype->l2;
        etype_ptr->vlan_tag  = etype->vlan_tag;
        etype_ptr->relative_offset = etype->relative_offset;

        rv = _field_fb_data_qualifier_ethertype_install(unit, f_dq, 
                                                        idx, etype_ptr);
    }
    return (rv);
}

/*
 * Function:
 *      _field_fb_data_qualifier_ethertype_delete
 * Purpose:
 *      Remove ethertype based offset from data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification. 
 * Returns:
 *      BCM_E_XXX
 */
int
_field_fb_data_qualifier_ethertype_delete(int unit,  int qual_id,
                                          bcm_field_data_ethertype_t *etype)
{
    _field_stage_t             *stage_fc; /* Stage field control.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    int                        idx;       /* Ethertype allocation index. */
    int                        rv;        /* Operation return status.    */
    _field_data_ethertype_t    *etype_ptr = NULL;/* Ether type pointer.  */

    /* Input parameters check. */
    if ((NULL == etype)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < _FP_DATA_ETHERTYPE_MAX; idx++) {
        etype_ptr = stage_fc->data_ctrl->etype + idx;

        if (etype_ptr->ref_count <= 0) {
            continue;
        }

        /* Find an entry with identical ethertype/l2format. */
        if ((etype->l2 == etype_ptr->l2) &&
            (etype->vlan_tag == etype_ptr->vlan_tag) &&
            (etype->ethertype == etype_ptr->ethertype)) {
            etype_ptr->ref_count--;
            break;
        }
    }

    if (_FP_DATA_ETHERTYPE_MAX == idx) {
        return (BCM_E_NOT_FOUND);
    }

    if (0 == etype_ptr->ref_count) {
        etype_ptr->l2  = 0;
        etype_ptr->ethertype = 0;
        etype_ptr->vlan_tag = BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY;
        rv = _field_fb_data_qualifier_ethertype_install(unit, f_dq, idx, etype_ptr);
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ip_protocol_add
 * Purpose:
 *      Add ip_protocol based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) Bcm device number. 
 *      qual_id    - (IN) Data qualifier id.
 *      ip_protocol- (IN) Ip protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ip_protocol_add(int unit,  int qual_id,
                                bcm_field_data_ip_protocol_t *ip_protocol)
{
    _field_data_protocol_t     *proto_ptr;/* Ip protocol pointer.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t             *stage_fc; /* Stage field control.        */
    int                        unused_idx;/* Unused ip_protocol index.   */
    int                        idx;       /* Ethertype allocation index. */
    int                        flags;     /* Ip protocol version flags.  */
    int                        rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == ip_protocol)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    unused_idx = -1;
    flags = ip_protocol->flags & 
        (BCM_FIELD_DATA_FORMAT_IP4 | BCM_FIELD_DATA_FORMAT_IP6);
    for (idx = 0; idx < _FP_DATA_IP_PROTOCOL_MAX; idx++) {
        proto_ptr = stage_fc->data_ctrl->ip + idx;

        /* Find an entry with identical ip_protocol. */
        if ((ip_protocol->ip == proto_ptr->ip) &&
            ((proto_ptr->ip4_ref_count > 0) || 
             ((proto_ptr->ip6_ref_count > 0)))) {
            break; 
        }
        /* Find unused entry. */
        if ((-1 == unused_idx) && 
            (0 == proto_ptr->ip4_ref_count) && 
            (0 == proto_ptr->ip6_ref_count)) {
            unused_idx = idx;
        }
    }
    if (_FP_DATA_IP_PROTOCOL_MAX == idx) {
        /* No match found -> check for unused entry. */
        if (-1 == unused_idx) {
            return (BCM_E_RESOURCE);
        }
        idx = unused_idx;
    }

    proto_ptr = stage_fc->data_ctrl->ip + idx;
    /* Increment reference count. */
    if (flags & BCM_FIELD_DATA_FORMAT_IP4) {
        proto_ptr->ip4_ref_count++;
    } 
    if (flags & BCM_FIELD_DATA_FORMAT_IP6) {
        proto_ptr->ip6_ref_count++;
    } 

    /* Check if reinstall is required. */
    proto_ptr->ip = ip_protocol->ip;
    proto_ptr->l2 = ip_protocol->l2;
    proto_ptr->vlan_tag = ip_protocol->vlan_tag;
    proto_ptr->flags |= flags;
    proto_ptr->relative_offset |= ip_protocol->relative_offset;

    rv =  _field_fb_data_qualifier_ip_protocol_install(unit, f_dq,
                                                       idx, proto_ptr);
    return (rv);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ip_protocol_delete
 * Purpose:
 *      Remove ip protocol based offset from data qualifier object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      qual_id     - (IN) Data qualifier id.
 *      ip_protocol - (IN) Ip Protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ip_protocol_delete(int unit,  int qual_id,
                          bcm_field_data_ip_protocol_t *ip_protocol)
{
    _field_data_protocol_t     *proto_ptr;/* Ip protocol pointer.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t             *stage_fc; /* Stage field control.        */
    int                        idx;       /* Ethertype allocation index. */
    int                        flags;     /* Ip protocol version flags.  */
    int                        rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == ip_protocol)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    flags = ip_protocol->flags & 
        (BCM_FIELD_DATA_FORMAT_IP4 | BCM_FIELD_DATA_FORMAT_IP6);
    for (idx = 0; idx < _FP_DATA_IP_PROTOCOL_MAX; idx++) {
        proto_ptr = stage_fc->data_ctrl->ip + idx;

        /* Find an entry with identical ip_protocol. */
        if ((ip_protocol->ip == proto_ptr->ip) &&
            ((proto_ptr->ip4_ref_count > 0) || 
             ((proto_ptr->ip6_ref_count > 0)))) {
            break; 
        }
    }
    if (_FP_DATA_IP_PROTOCOL_MAX == idx) {
        /* No match found. */
        return (BCM_E_NOT_FOUND);
    }

    proto_ptr = stage_fc->data_ctrl->ip + idx;
    /* Increment reference count. */
    if ((flags & BCM_FIELD_DATA_FORMAT_IP4) &&
        (proto_ptr->ip4_ref_count > 0)) {
        proto_ptr->ip4_ref_count--;
        if (0 == proto_ptr->ip4_ref_count) {
            proto_ptr->flags &= ~BCM_FIELD_DATA_FORMAT_IP4;
        } 
    } 
    if ((flags & BCM_FIELD_DATA_FORMAT_IP6) &&
        (proto_ptr->ip6_ref_count > 0)) {
        proto_ptr->ip6_ref_count--;
        if (0 == proto_ptr->ip4_ref_count) {
            proto_ptr->flags &= ~BCM_FIELD_DATA_FORMAT_IP6;
        }
    } 

    /* Mark entry as unused. */
    if ((0 == proto_ptr->ip4_ref_count) && (0 == proto_ptr->ip6_ref_count)) {
        proto_ptr->ip = 0;
        proto_ptr->flags = 0;
    } 

    /* Install protocol match register. */
    rv = _bcm_field_fb_udf_ipprotocol_set(unit, idx, proto_ptr->flags, 
                                          proto_ptr->ip);
    return (rv);
}


#define _FP_L2_FORMAT_MIN   (0)
#define _FP_L2_FORMAT_MAX   (2)
/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ethertype_add
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ethertype_add(int unit,  int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    int               idx;             /* L2 format iteration index.*/
    uint16            l2;              /* Installed L2 format.      */
    int               rv = BCM_E_NONE; /* Operation return status.  */

    /* Input parameters check. */
    if (NULL == etype) {
        return (BCM_E_PARAM);
    }

    l2 = etype->l2;
    for (idx = _FP_L2_FORMAT_MIN; idx <= _FP_L2_FORMAT_MAX; idx++) {
        if (0 == (l2 & (1 << idx))) {
            continue;
        }
        etype->l2 = (1 << idx);
        rv = _field_fb_data_qualifier_ethertype_add(unit, qual_id, etype);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ethertype_delete
 * Purpose:
 *      Remove ethertype based offset from data qualifier object. 
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ethertype_delete(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    int               idx;             /* L2 format iteration index.*/
    uint16            l2;              /* Installed L2 format.      */
    int               rv = BCM_E_NONE; /* Operation return status.  */

    /* Input parameters check. */
    if (NULL == etype) {
        return (BCM_E_PARAM);
    }

    l2 = etype->l2;
    for (idx = _FP_L2_FORMAT_MIN; idx <= _FP_L2_FORMAT_MAX; idx++) {
        if (0 == (l2 & (1 << idx))) {
            continue;
        }
        etype->l2 = (1 << idx);
        rv = _field_fb_data_qualifier_ethertype_delete(unit, qual_id, etype);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return (rv);
}
#undef _FP_L2_FORMAT_MIN
#undef _FP_L2_FORMAT_MAX


/*
 * Function:
 *     _field_fb_functions_init
 *
 * Purpose:
 *     Set up functions pointers 
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN/OUT) pointers to stage control block whe the device 
 *                         and stage specific functions will be registered.
 *
 * Returns:
 *     nothing
 * Notes:
 */
STATIC void
_field_fb_functions_init(int unit, _field_funct_t *functions)
{
    functions->fp_detach               = _field_fb_detach;
    functions->fp_udf_spec_set         = _bcm_field_fb_udf_spec_set;
    functions->fp_udf_spec_get         = _bcm_field_fb_udf_spec_get;
    functions->fp_udf_write            = _bcm_field_fb_udf_write;
    functions->fp_udf_read             = _bcm_field_fb_udf_read;
    functions->fp_group_install        = _bcm_field_fb_group_install;
    functions->fp_selcodes_install     = _field_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_fb_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _bcm_field_fb_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = _field_fb_tcam_policy_clear;
    functions->fp_tcam_policy_install  = _field_fb_tcam_policy_install;
    functions->fp_policer_install      = _bcm_field_fb_policer_install;
    functions->fp_slice_reinit	       = _field_fb_slice_reinit;    
    functions->fp_write_slice_map      = _bcm_field_fb_write_slice_map;
    functions->fp_qualify_ip_type      = _field_fb_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _field_fb_qualify_ip_type_get;
    functions->fp_action_support_check = _field_fb_action_support_check;
    functions->fp_action_conflict_check = _field_fb_action_conflict_check;
    functions->fp_stat_index_get       = _bcm_field_fb_stat_index_get;
    functions->fp_action_params_check  = _field_fb_action_params_check;

#if defined(BCM_BRADLEY_SUPPORT) 
    if (soc_feature(unit, soc_feature_two_ingress_pipes)) { 
        functions->fp_counter_get    = _field_br_counter_get;
        functions->fp_counter_set    = _field_br_counter_set;
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        functions->fp_counter_get    = _bcm_field_fb_counter_get;
        functions->fp_counter_set    = _bcm_field_fb_counter_set;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) 
    functions->fp_egress_key_match_type_set = _field_fb2_key_match_type_set;
#else  /* BCM_FIREBOLT2_SUPPORT */
    functions->fp_egress_key_match_type_set = NULL;
#endif  /* BCM_FIREBOLT2_SUPPORT */
    functions->fp_external_entry_install = NULL;
    functions->fp_external_entry_remove = NULL;
    functions->fp_external_entry_prio_set = NULL;
}
#else /*BCM_FIREBOLT_SUPPORT && BCM_FIELD_SUPPORT */ 
int _firebolt_field_not_empty;
#endif  /* BCM_FIREBOLT_SUPPORT && BCM_FIELD_SUPPORT */
