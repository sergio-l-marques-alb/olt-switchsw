/* $Id: triumph2_field.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <soc/mem.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>


/* local/static function prototypes */
STATIC void _field_tr2_functions_init(_field_funct_t *functions) ;

/* Static variables. */
static soc_field_t  offset_field[] = {
    UDF1_OFFSET0f, UDF1_OFFSET1f, 
    UDF1_OFFSET2f, UDF1_OFFSET3f, 
    UDF2_OFFSET0f, UDF2_OFFSET1f, 
    UDF2_OFFSET2f, UDF2_OFFSET3f};
static soc_field_t  offset_base_field[] = {
    UDF1_BASE_OFFSET_0f, UDF1_BASE_OFFSET_1f,
    UDF1_BASE_OFFSET_2f, UDF1_BASE_OFFSET_3f,
    UDF2_BASE_OFFSET_0f, UDF2_BASE_OFFSET_1f,
    UDF2_BASE_OFFSET_2f, UDF2_BASE_OFFSET_3f};


/*
 * Function:
 *     _field_tr2_ingress_qualifiers_init
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
_field_tr2_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelDisable, 0, 8, 3);
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, 12, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelDisable, 0, 16, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelDisable, 0, 17, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopback,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0, 21, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyLoopbackType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 0, 18, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTunnelType,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceLoopbackTypeSelect, 1, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelDisable, 0, 22, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelDisable, 0, 23, 1);

    /* FPF3 primary slice single wide. */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 0, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 1, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 2, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceDstClassSelect, 3, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect,
                               _bcmFieldFwdFieldVpn, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 48, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 48, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 0,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 48, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 0, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 1, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 2, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 2, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 0, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 1, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceSrcClassSelect, 3, 30, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceDstClassSelect, 3, 24, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, 36, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, 36, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 1, 48, 12);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 30, 11);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 24, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp, 47, 9);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf3, 2,
                                _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 41, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 41, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, 56, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 3, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 3, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 37, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 40, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, 42, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelFpf3, 3, 40, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 3, 44, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 3, 46, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 3, 49, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 3, 54, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 4, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 4, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 37, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 4, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 4, 40, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 4, 52, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 4, 53, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 24, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 37, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 5, 40, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf3, 6, 24, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 6, 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 6, 26, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 6, 27, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 6, 29, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 6, 30, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 6, 31, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 6, 33, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 6, 34, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 6, 35, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 6, 36, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 6, 37, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 6, 38, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 6, 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 6, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 6, 40, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 6, 52, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 6, 53, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, 24, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 0, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 1, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, 7,
                               _bcmFieldSliceIntfClassSelect, 2, 48, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf3, 8, 24, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 44, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 8, 46, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelFpf3, 8, 44, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf3, 8, 48, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 8, 50, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 8, 52, 2);

    /* FPF2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, 61, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, 69, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, 75, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 0, 77, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, 85, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, 101, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, 117, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, 125, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, 157, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1, 61, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1, 69, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 1, 75, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 1, 77, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, 85, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 1, 101, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, 117, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, 125, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, 157, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 2, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 3, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 4, 61, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 4, 69, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 4, 75, 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf2, 4, 95, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, 103, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 4, 125, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 5, 61, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 5, 61, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 5, 73, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 5, 74, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 5, 77, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 5, 93, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 141, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, 61, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, 61, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, 73, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, 74, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, 77, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, 93, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 6, 141, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, 61, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, 61, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, 73, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, 74, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, 77, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 7, 93, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 7, 101, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 7, 109, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 7, 141, 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, 61, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 10, 61, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 10, 125, 64);


    /* FPF1 */ 
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 0, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 1, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 2, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 2, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 0, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 1, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceSrcClassSelect, 3, 195, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstClassSelect, 3, 189, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, 201, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, 201, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, 201, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 213, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 213, 13);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 213, 13);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 195, 11);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp, 212, 9);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityGlp, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityMimGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 206, 15);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                               _bcmFieldSliceSelFpf1, 1,
                               _bcmFieldSliceIngressEntitySelect,
                               _bcmFieldFwdEntityModPortGport, 206, 15);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, 221, 3);


    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 195, 11);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 189, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 189, 17);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 2, 206, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 0, 209, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 1, 209, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 2, 209, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 2, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 0, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 1, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceSrcClassSelect, 3, 215, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 2,
                               _bcmFieldSliceDstClassSelect, 3, 209, 6);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, 202, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 3, 205, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 3, 205, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, 217, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 3, 218, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 3, 221, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, 202, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, 205, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelFpf1, 5, 189, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 5, 190, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 5, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 5, 192, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 5, 194, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 5, 195, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 5, 196, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 5, 198, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 5, 199, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 5, 200, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 5, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 5, 202, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 5, 203, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 5, 204, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf1, 5, 205, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf1, 5, 205, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf1, 5, 217, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf1, 5, 218, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 5, 221, 2);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 6, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 6, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 6, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 6, 202, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, 205, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 6, 207, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelFpf1, 6, 205, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTranslatedVlanFormat,
                 _bcmFieldSliceSelFpf1, 6, 209, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 6, 211, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 6, 214, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 6, 219, 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, 189, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, 189, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, 201, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, 202, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceDstClassSelect, 0, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceDstClassSelect, 1, 205, 6);
#ifdef LVL7_FIXUP
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 2, 205, 6);
#else
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceDstClassSelect, 3, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceSrcClassSelect, 3, 205, 6);
#endif
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 7,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, 211, 12);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 8, 189, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf1, 8, 197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceDstClassSelect, 0, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceDstClassSelect, 1, 205, 6);
#ifdef LVL7_FIXUP
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 2, 205, 6);
#else
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceDstClassSelect, 3, 205, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceSrcClassSelect, 3, 205, 6);
#endif
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVrf, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVpn, 211, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyForwardingVlanId,
                               _bcmFieldSliceSelFpf1, 8,
                               _bcmFieldSliceFwdFieldSelect, 
                               _bcmFieldFwdFieldVlan, 211, 12);

    /* DWF4 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf4, 1,
                            _bcmFieldSliceSelDisable, 0, 0, 6);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 7, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 15, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 23, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 31, 8);

    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 55, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 56, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 57, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 58, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 60, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 61, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 62, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 64, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 65, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 66, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 67, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 68, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 69, 1);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 70, 1);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 0x1,
                     71, 24, 95, 8, 0, 0);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 0, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 1, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIntfClassSelect, 2, 95, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 103, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceSelDisable, 0, 135, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceSelDisable, 0, 39, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0, 39, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 3,
                            _bcmFieldSliceSelDisable, 0, 39, 128);
    /* DWF1 */
#ifdef LVL7_FIXUP
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 167, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 175, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 177, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 193, 16);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0, 
                     _bcmFieldSliceDstFwdEntitySelect, 
                     _bcmFieldFwdEntityGlp, 0x1,
                     209, 17, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0, 
                     _bcmFieldSliceDstFwdEntitySelect, 
                     _bcmFieldFwdEntityGlp, 0x1,
                     215, 11, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityGlp, 0x1,
                     209, 17, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMplsGport, 0x1,
                     209, 17, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMimGport, 0x1,
                     209, 17, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityWlanGport, 0x1,
                     209, 17, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityL3Egress, 0x1,
                     209, 17, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelDisable, 0,
                     _bcmFieldSliceDstFwdEntitySelect,
                     _bcmFieldFwdEntityMulticastGroup, 0x1,
                     209, 17, 0, 0, 0, 0);
#else
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 167, 7);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 174, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 176, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelDisable, 0,
                            _bcmFieldSliceSelDisable, 0, 192, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 208, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 214, 11);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityGlp, 208, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMplsGport, 208, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMimGport, 208, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstWlanGport,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityWlanGport, 208, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstL3Egress,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityL3Egress, 208, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMulticastGroup,
                               _bcmFieldSliceSelDisable, 0,
                               _bcmFieldSliceDstFwdEntitySelect,
                               _bcmFieldFwdEntityMulticastGroup, 208, 17);
#endif
    /* DONE DWF */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_lookup_qualifiers_init
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
_field_tr2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Enable the overlay of Sender Ethernet Address onto MACSA
     * on ARP/RARP packets.
     */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                                ARP_VALIDATION_ENf, 1));

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageLookup,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketFormat,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* FPF3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, 16, 16);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, 0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, 0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, 16, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, 0, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 0, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, 8, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 8, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 21, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                 _bcmFieldSliceSelFpf3, 3, 24, 7);

    /* FPF2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, 40, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 46, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 46, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 48, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 56, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 56, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 72, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 72, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 88, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 88, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 96, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 128, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 128, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, 64, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, 112, 48);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, 80, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, 80, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, 112, 48);

    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 40, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 48, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, 80, 32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, 80, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 112, 48);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, 32, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, 32, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, 96, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, 96, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, 96, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, 136, 24);

    /* FPF1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 160, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 160, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, 163, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 0, 165, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 0, 167, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf1, 0, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf1, 0, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort, 
                 _bcmFieldSliceSelFpf1, 0, 191, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass, 
                 _bcmFieldSliceSelFpf1, 0, 197, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, 205, 4);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, 205, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports, 
                 _bcmFieldSliceSelFpf1, 0, 209, 1);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 16, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 22, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 24, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 24, 8);
    /* DWF2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 32, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 40, 6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 46, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 46, 2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 48, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 48, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 56, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 56, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 72, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 72, 16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 88, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 88, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 96, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 96, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, 128, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 128, 32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, 32, 128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, 32, 128);

#ifdef LVL7_FIXUP
    /* FPF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 160, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 168, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 176, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, 184, 16);
#else
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf1, 0, 160, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf1, 0, 168, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf1, 0, 176, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 0, 184, 16);
#endif

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_external_qualifiers_init
 * Purpose:
 *     Initialize device stage external qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_tr2_external_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    uint32 rval;
    uint32 keygen_type;
    _FP_QUAL_DECL;

    BCM_IF_ERROR_RETURN(READ_ESM_KEYGEN_CTLr(unit, &rval));
    keygen_type = soc_reg_field_get(unit, ESM_KEYGEN_CTLr, rval, TCAM_TYPEf);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageExternal,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* _FP_EXT_ACL_144_L2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 0, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 48, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 60, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 61, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 64, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 70, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 72, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 120, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_L2, 136, 8);

    if (keygen_type != 3) {
    /* _FP_EXT_ACL_L2 */
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 12, 48);
    
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 62, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 64, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 68, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 69, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 71, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 72, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 120, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 136, 2);
    
    /* Unused: 2 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 144, 6);
    
    /* Unused: 1 bit */
    /* L2 or IPV4: 1 bit: the tcam_write will handle this */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 216, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 217, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceDstClassSelect, 0, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceDstClassSelect, 2, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceSrcClassSelect, 2, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceSrcClassSelect, 0, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceSrcClassSelect, 3, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceDstClassSelect, 3, 218, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 230, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceIntfClassSelect, 0, 238, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceIntfClassSelect, 1, 238, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                               _bcmFieldSliceIntfClassSelect, 2, 238, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     246, 8, 138, 3, 0, 0);
    /* Unused: 2 bits */
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 268, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 269, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 272, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 272, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 284, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 285, 3);
    } else {
        /* _FP_EXT_ACL_L2 (for 7K 350MHz) */
/* not for 7K
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 272,16);
*/
        /* Unused: 12 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 12, 48);
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 62, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 64, 4);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 68, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 69, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 71, 1);
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 120, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 136, 2);
        
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 144, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 156, 48);
        
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 206, 6);
        
        /* Unused: 1 bit */
        /* L2 or IPV4: 1 bit: the tcam_write will handle this */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 216, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 217, 1);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 0, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 2, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 2, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 0, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceSrcClassSelect, 3, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceDstClassSelect, 3, 218, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 230, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassPort,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 0, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 1, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2,
                                   _bcmFieldSliceIntfClassSelect, 2, 238, 8);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelDisable, 0,
                         _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         246, 8, 138, 3, 0, 0);
        
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 256, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 268, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2, 269, 3);
        
    }

    /* _FP_EXT_ACL_144_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 32, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 48, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 60, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 61, 3);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     64, 8, 138, 3, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 72, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 73, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 105, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 121, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 129, 8);
    /* ESM_IP_PROTOCOL_OVERLAY_EN 137, 1: Useless: 
                 See ESM_KEYGEN_CTLr.IP_PROT_OVERLAY_EN */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 141, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV4, 143, 1);

    if (keygen_type != 3) {
    /* _FP_EXT_ACL_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 32);
    /* Unused: 18 + 3 + 8 bits */
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 62, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 63, 4);
    /* Unused: 1 bit */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 68, 1);
    /* Unused: 2 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 71, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 72, 32);
    
        
    /* Unused: 29 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 144, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 150, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 166, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 182, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 196, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 204, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 212, 2);
    /* ACL_Type: 1 bit: tcam_write will handle this */
    /* L2 or IPV4: 1 bit: the tcam_write will handle this */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 216, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 217, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceDstClassSelect, 0, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceDstClassSelect, 1, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceDstClassSelect, 2, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceSrcClassSelect, 2, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceSrcClassSelect, 0, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceSrcClassSelect, 1, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceSrcClassSelect, 3, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceDstClassSelect, 3, 218, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 230, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceIntfClassSelect, 0, 238, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceIntfClassSelect, 1, 238, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                               _bcmFieldSliceIntfClassSelect, 2, 238, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 246, 11);
    /* Unused: 15 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 284, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 285, 3);
    } else {
        /* _FP_EXT_ACL_IPV4 (for 7K 350 MHz) */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 0, 32);
        /* Unused: 18 + 3 + 8 bits */
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 62, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 63, 4);
        /* Unused: 1 bit */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 68, 1);
        /* Unused: 2 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 71, 1);
        /* Unused: 17 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 89, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 95, 6);
        
        
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 111, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 127, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 143, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 144, 0);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 144, 32);
        /* Unused: 12 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 188, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 196, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 204, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 212, 2);
        /* ACL_Type: 1 bit: tcam_write will handle this */
        /* L2 or IPV4: 1 bit: the tcam_write will handle this */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 216, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 217, 1);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 0, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 1, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 2, 218, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 2, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 0, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 1, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceSrcClassSelect, 3, 224, 6);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceDstClassSelect, 3, 218, 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 230, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassPort,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 0, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL3,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 1, 238, 8);
        _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                                   bcmFieldQualifyInterfaceClassL2,
                                   _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4,
                                   _bcmFieldSliceIntfClassSelect, 2, 238, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 246, 11);
        /* Unused: 15 bits */
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 272, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 284, 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV4, 285, 3);
    }

    /* _FP_EXT_ACL_L2_IPV4 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 0, 32);
    /* Unused: 18 + 3 + 8 bits */
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 62, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 63, 4);
    /* Unused: 1 bit */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 68, 1);
    /* Unused: 2 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 71, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 72, 32);
    
    
    /* Unused: 29 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 143, 1);
    /* Unused: 12 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 156, 48);
    /* Unused: 11 + 1 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 216, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 222, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 238, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 254, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 260, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 268, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 276, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 284, 2);
    /* ACL_Type: 1 bit: tcam_write will handle this */
    /* L2 or IPV4: 1 bit: the tcam_write will handle this */
    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 300, 48);
    
    /* Unused: 11 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 360, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 361, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 0, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 1, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 2, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 2, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 0, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 1, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceSrcClassSelect, 3, 368, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 3, 362, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceDstClassSelect, 3, 374, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceIntfClassSelect, 0, 382, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceIntfClassSelect, 1, 382, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4,
                               _bcmFieldSliceIntfClassSelect, 2, 382, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 390, 11);
    /* Unused: 15 bits */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 416, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 416, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 428, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV4, 429, 3);

    /* _FP_EXT_ACL_144_IPV6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 0, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 72, 64);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     136, 8, 64, 8, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     136, 8, 64, 4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 68, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_144_IPV6, 69, 3);

    /* _FP_EXT_ACL_IPV6_SHORT */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     80, 64, 0, 64, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 64, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 72, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 73, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 77, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 78, 2);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     216, 72, 144, 56, 0, 0);
     
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 200, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 200, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 200, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 212, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 213, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 288, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 289, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceSrcClassSelect, 0, 290, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceSrcClassSelect, 1, 290, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceSrcClassSelect, 3, 290, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT,
                               _bcmFieldSliceDstClassSelect, 3, 290, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 296, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 312, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 328, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6HopLimit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 334, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 342, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_SHORT, 350, 8);


    /* _FP_EXT_ACL_IPV6_FULL */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     80, 64, 0, 64, 0, 0);
    /* Unused: 8 bits */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     213, 3, 72, 5, 0, 0);
    /* Unused: 3 bits */
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     284, 4, 144, 4, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 148, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 156, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 156, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 169, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 182, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 188, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 190, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 191, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 195, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 196, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMplsTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 204, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 205, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 216, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 217, 1);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 0, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 1, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 2, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 2, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 0, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 1, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceSrcClassSelect, 3, 224, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceDstClassSelect, 3, 218, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassPort,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceIntfClassSelect, 0, 230, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceIntfClassSelect, 1, 230, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL,
                               _bcmFieldSliceIntfClassSelect, 2, 230, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 238, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 254, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 270, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6HopLimit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 276, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6HopLimit,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 276, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     368, 64, 288, 64, 0, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     352, 8, 365, 3, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_IPV6_FULL, 363, 8);

    /* _FP_EXT_ACL_L2_IPV6 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 0, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     423, 9, 288, 72, 216, 47);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     263, 25, 144, 72, 97, 31);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 
                     _bcmFieldSliceSelDisable, 0, 0,
                     115, 29, 0, 19, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 19, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 68, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 70, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 71, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 103, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 103, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 360, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 361, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 372, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 388, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6NextHeader,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 404, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6TrafficClass,
                 _bcmFieldSliceSelExternal, _FP_EXT_ACL_L2_IPV6, 415, 8);

    
    /* ESM_IP_PROTOCOL_OVERLAY_EN 412, 1: Useless: 
                                        See ESM_KEYGEN_CTLr.IP_PROT_OVERLAY_EN */
    /* bcmFieldQualifyTcpControl 404, 8: OVERLAY: not used: 
                                       see ESM_KEYGEN_CTLr.IP_PROT_OVERLAY_EN */
    
    
    /* bcmFieldQualifyInPort 362, 6: OVERLAY: not used: see ESM_KEYGEN_CTL */
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr2_egress_qualifiers_init
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
_field_tr2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 8, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 12, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 16, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 22, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 38, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 62, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 70, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 102, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 134, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 142, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 6, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect, 
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6, 
                               14, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect, 
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               14, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect, 
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               14, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               78, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 142, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 150, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 8, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 12, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 14, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 16, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 22, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 38, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 62, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 190, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 198, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 210, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 19, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 23, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 25, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 27, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 43, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 91, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 147, 1);
#ifdef LVL7_FIXUP
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 148, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 149, 3);
#endif
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 210, 1);

    /* KEY1 without v4 specific fields. */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 8, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 12, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 16, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 22, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 38, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 62, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 134, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 142, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 143, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 152, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 165, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 172, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 173, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 173, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 186, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 189, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 191, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPortClass,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 192, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 200, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 206, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY5, 210, 1);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_qualifiers_init
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
_field_tr2_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_tr2_ingress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_tr2_lookup_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_tr2_egress_qualifiers_init(unit, stage_fc);
          break;
      case _BCM_FIELD_STAGE_EXTERNAL:  
          rv = _field_tr2_external_qualifiers_init(unit, stage_fc);
          break;
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_tr2_init
 * Purpose:
 *     Perform initializations that are specific to BCM56624. This
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
_bcm_field_tr2_init(int unit, _field_control_t *fc) 
{
    _field_stage_t *stage_fc; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_fc = fc->stages;
    while (stage_fc) {
        /* Clear hardware table */
        BCM_IF_ERROR_RETURN(_bcm_field_tr_hw_clear(unit, stage_fc));

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_tr2_qualifiers_init(unit, stage_fc));

        if (_BCM_FIELD_STAGE_EXTERNAL == stage_fc->stage_id) {
            _bcm_field_tr_external_init(unit, stage_fc);
            stage_fc = stage_fc->next;
            continue;
        }

        /* Goto next stage */
        stage_fc = stage_fc->next;
    }

    /* Initialize the TOS_FN, TTL_FN, TCP_FN tables */
    BCM_IF_ERROR_RETURN(_bcm_field_trx_tcp_ttl_tos_init(unit));
    
    if (0 == SOC_WARM_BOOT(unit)) {
        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Enable meter refresh */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
    }

    /* Initialize the function pointers */
    _field_tr2_functions_init(&fc->functions);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr2_udf_tcam_entry_move
 * Purpose:
 *     Move a single entry in  udf tcam and offset tables.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     data_ctrl - (IN) Data control structure.
 *     dest      - (IN) Insertion target index. 
 *     free_slot - (IN) Free slot. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_move(int unit, 
                               _field_data_control_t *data_ctrl, 
                               int src, int dst)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry.   */
    int rv;                                 /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == data_ctrl) {
        return (BCM_E_PARAM);
    }

    rv = soc_mem_read(unit, FP_UDF_OFFSETm, MEM_BLOCK_ANY, src, hw_buf);
    BCM_IF_ERROR_RETURN(rv);
    rv = soc_mem_write(unit, FP_UDF_OFFSETm, MEM_BLOCK_ALL, dst, hw_buf);
    BCM_IF_ERROR_RETURN(rv);
    rv = soc_mem_read(unit, FP_UDF_TCAMm, MEM_BLOCK_ANY, src, hw_buf);
    BCM_IF_ERROR_RETURN(rv);
    rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, dst, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Update sw structure tracking entry use. */
    sal_memcpy(data_ctrl->tcam_entry_arr + dst, 
               data_ctrl->tcam_entry_arr + src,
               sizeof(_field_data_tcam_entry_t));
    sal_memset(data_ctrl->tcam_entry_arr + src, 0,
               sizeof(_field_data_tcam_entry_t));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_move_down
 * Purpose:
 *     Moved udf tcam entries down 
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     data_ctrl - (IN) Data control structure.
 *     dest      - (IN) Insertion target index. 
 *     free_slot - (IN) Free slot. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_move_down(int unit, 
                              _field_data_control_t *data_ctrl, 
                              int dest, int free_slot)
{
    int idx;          /* Entries iterator.        */ 
    int rv;           /* Operation return status. */

    /* Input parameters check. */
    if (NULL == data_ctrl) {
        return (BCM_E_PARAM);
    }

    for (idx = free_slot; idx < dest; idx ++) {
        rv = _field_tr2_udf_tcam_entry_move(unit, data_ctrl, idx + 1, idx);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_move_up
 * Purpose:
 *     Moved udf tcam entries up.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     data_ctrl - (IN) Data control structure.
 *     dest      - (IN) Insertion target index. 
 *     free_slot - (IN) Free slot. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_move_up(int unit, 
                            _field_data_control_t *data_ctrl, 
                            int dest, int free_slot)
{
    int idx;          /* Entries iterator.        */ 
    int rv;           /* Operation return status. */

    /* Input parameters check. */
    if (NULL == data_ctrl) {
        return (BCM_E_PARAM);
    }

    for (idx = free_slot; idx > dest; idx --) {
        rv = _field_tr2_udf_tcam_entry_move(unit, data_ctrl, idx - 1, idx);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_match
 * Purpose:
 *     Match tcam entry against currently instllaed ones
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     stage_fc - (IN) Stage field control structure. 
 *     hw_buf   - (IN) Hw buffer.
 *     tcam_idx - (IN) Allocated tcam index.  
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_match(int unit, _field_stage_t *stage_fc,
                                uint32 *hw_buf, int *tcam_idx)
{
    uint32 *buffer;      /* Hw buffer to dma udf tcam. */
    uint32 *entry_ptr;   /* Tcam entry pointer.        */
    int alloc_size;      /* Memory allocation size.    */
    int entry_size;      /* Single tcam entry size.    */ 
    soc_mem_t mem;       /* Udf tcam memory id.        */
    int idx_count;       /* Tcam entry count.          */
    int idx;             /* Tcam entries iterator.     */
    int rv;              /* Operation return status.   */

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == stage_fc) || (NULL == tcam_idx)) {
        return (BCM_E_PARAM);
    }

    mem = FP_UDF_TCAMm;
    idx_count = soc_mem_index_count(unit, mem);
    entry_size = sizeof(fp_udf_tcam_entry_t);
    alloc_size = entry_size * idx_count;

    /* Allocate memory buffer. */
    buffer = soc_cm_salloc(unit, alloc_size, "Udf tcam");
    if (buffer == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Read table to the buffer. */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            soc_mem_index_min(unit, mem),
                            soc_mem_index_max(unit, mem), buffer);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, buffer);
        return (BCM_E_INTERNAL);
    }

    for (idx = 0; idx < idx_count; idx++) {
        if (stage_fc->data_ctrl->tcam_entry_arr[idx].ref_count) {
            entry_ptr = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                                     buffer, idx);

            if (0 == sal_memcmp(entry_ptr, hw_buf, entry_size)) {
                *tcam_idx = idx;
                break;
            }
        }
    }
    soc_cm_sfree(unit, buffer);
    return (idx < idx_count) ? BCM_E_NONE : BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_insert
 * Purpose:
 *     Insert udf tcam entry into the tcam. 
 *     No actual write happens in this routine
 *     only tcam reorganization and index allocation.
 *     SW must write FP_UDF_OFFSET table before inserting 
 *     valid tcam entry. 
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     hw_buf   - (IN) Hw buffer.
 *     priority - (IN) Inserted rule priority.    
 *     tcam_idx - (IN) Allocated tcam index.  
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_insert(int unit, uint32 *hw_buf, 
                                 uint8 priority, int *tcam_idx)
{
    _field_data_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.  */
    _field_stage_t *stage_fc;      /* Stage field contral structure.  */
    int unused_entry_min;          /* Unused entry before the range.  */
    int unused_entry_max;          /* Unused entry after the range.   */
    int range_min;                 /* Index min for entry insertion.  */
    int range_max;                 /* Index max for entry insertion.  */
    int idx_max;                   /* FP UDF tcam table index max.    */
    int idx;                       /* FP UDF tcam table interator.    */
    int rv;                        /* Operation return status.        */

    /* Input parameters check. */
    if (NULL == tcam_idx) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    idx_max = soc_mem_index_max(unit, FP_UDF_TCAMm);
    range_min = soc_mem_index_min(unit, FP_UDF_TCAMm);
    range_max = idx_max;
    unused_entry_min = unused_entry_max = -1;


    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);
    tcam_entry_arr = stage_fc->data_ctrl->tcam_entry_arr;


    /* Check if identical entry already exists. */
    rv = _field_tr2_udf_tcam_entry_match(unit, stage_fc, hw_buf, tcam_idx);
    if (BCM_SUCCESS(rv)) {
        if (tcam_entry_arr[*tcam_idx].priority != priority) {
            return (BCM_E_RESOURCE);
        }
        tcam_entry_arr[*tcam_idx].ref_count++;
        return (BCM_E_NONE);
    } else if (rv != BCM_E_NOT_FOUND) {
        return (rv);
    }

    /* No identical entry found try to allocate an unused entry and 
     * reshuffle tcam if necessary to organize entries by priority. 
     */
    for (idx = 0; idx <= idx_max; idx++) {
        if (0 == stage_fc->data_ctrl->tcam_entry_arr[idx].ref_count) {
            if (idx <= range_max) {
                /* Any free index below range max can be used for insertion. */
                unused_entry_min = idx;
            } else {
                /* There is no point to continue after first 
                 * free index above range max.
                 */ 
                unused_entry_max = idx;
                break;
            }
            continue;
        }
        /* Identify insertion range. */
        if (tcam_entry_arr[idx].priority > priority) {
            range_min = idx;
        } else if (tcam_entry_arr[idx].priority < priority) {
            if (idx < range_max) {
                range_max = idx;
            }
        }
    }

    /* Check if tcam is completely full. */
    if ((unused_entry_min == -1) && (unused_entry_max == -1)) {
        return (BCM_E_FULL);
    }

    /*  Tcam entries shuffling. */
    if (unused_entry_min > range_min) {
        *tcam_idx = unused_entry_min;
    } else if (unused_entry_min == -1) {
        rv = _field_tr2_udf_tcam_move_up(unit, stage_fc->data_ctrl,
                                         range_max, unused_entry_max);

        BCM_IF_ERROR_RETURN(rv);
        *tcam_idx = range_max;
    } else if (unused_entry_max == -1) {
        rv = _field_tr2_udf_tcam_move_down(unit, stage_fc->data_ctrl,
                                           range_min, unused_entry_min);
        BCM_IF_ERROR_RETURN(rv);
        *tcam_idx = range_min;
    } else if ((range_min - unused_entry_min) > 
               (unused_entry_max - range_max)) {
        rv = _field_tr2_udf_tcam_move_up(unit, stage_fc->data_ctrl,
                                         range_max, unused_entry_max);
        BCM_IF_ERROR_RETURN(rv);
        *tcam_idx = range_max;
    } else {
        rv = _field_tr2_udf_tcam_move_down(unit, stage_fc->data_ctrl,
                                           range_min, unused_entry_min);
        BCM_IF_ERROR_RETURN(rv);
        *tcam_idx = range_min;
    }

    /* Index was successfully allocated. */
    tcam_entry_arr[*tcam_idx].ref_count = 1;
    tcam_entry_arr[*tcam_idx].priority = priority;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_offset_entry_read
 * Purpose:
 *     Read the udf offset from hardware 
 * Parameters:
 *     unit         - (IN) BCM device number. 
 *     entry_idx    - (IN) Entry index in FP_UDF_OFFSETm.
 *     offset_idx   - (IN) (0-8) Offset number to fill.
 *     offset_base  - (IN) Offset number base point.  
 *     offset_value - (IN) Offset word number.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_offset_entry_read(int unit, 
                                 int entry_idx, 
                                 int offset_idx, 
                                 bcm_field_data_offset_base_t *offset_base,
                                 uint32 *offset_value)

{
    fp_udf_offset_entry_t offset_buf;    /* Udf offset entry.             */
    int offset_base_encoding;            /* Hw udf offset base encodings. */
    int rv;                              /* Operation return status.      */


    rv = soc_mem_read(unit, FP_UDF_OFFSETm, MEM_BLOCK_ANY, 
                      entry_idx, (uint32 *)&offset_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Get udf offset base. */
    offset_base_encoding = soc_mem_field32_get(unit, FP_UDF_OFFSETm, 
                                               (uint32 *)&offset_buf,
                                               offset_base_field[offset_idx]);
    switch (offset_base_encoding) {
      case 0x0:
          *offset_base = bcmFieldDataOffsetBaseHigigHeader;
          break;
      case 0x1: 
          *offset_base = bcmFieldDataOffsetBasePacketStart;
          break;
      case 0x2:
          *offset_base = bcmFieldDataOffsetBaseOuterL3Header;
          break;
      case 0x3:
          *offset_base = bcmFieldDataOffsetBaseOuterL4Header;
          break;
      case 0x4:
          *offset_base = bcmFieldDataOffsetBaseInnerL4Header;
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    /* Get udf offset. */
    *offset_value = soc_mem_field32_get(unit, FP_UDF_OFFSETm, 
                                        (uint32 *)&offset_buf,
                                        offset_field[offset_idx]);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_offset_entry_write
 * Purpose:
 *     Write the udf offsets to FP_UDF_OFFSETm 
 * Parameters:
 *     unit         - (IN) BCM device number. 
 *     entry_idx    - (IN) Entry index in FP_UDF_OFFSETm.
 *     offset_idx   - (IN) (0-8) Offset number to fill.
 *     offset_base  - (IN) Offset number base point.  
 *     offset_value - (IN) Offset word number.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_offset_entry_write(int unit, int entry_idx, int offset_idx,
                                  bcm_field_data_offset_base_t offset_base,
                                  uint32 offset_value)
{
    fp_udf_offset_entry_t offset_buf;    /* Udf offset entry.             */
    int offset_base_encoding;            /* Hw udf offset base encodings. */
    int rv;                              /* Operation return status.      */

    offset_value &= ~_BCM_FIELD_USER_OFFSET_FLAGS;

    rv = soc_mem_read(unit, FP_UDF_OFFSETm, MEM_BLOCK_ANY, 
                      entry_idx, (uint32 *)&offset_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set udf offset base. */
    switch (offset_base) {
      case bcmFieldDataOffsetBaseHigigHeader:
      case bcmFieldDataOffsetBaseHigig2Header:
          offset_base_encoding = 0;
          break;
      case bcmFieldDataOffsetBasePacketStart:
          offset_base_encoding = 1;
          break;
      case bcmFieldDataOffsetBaseOuterL3Header:
          offset_base_encoding = 2;
          break;
      case bcmFieldDataOffsetBaseInnerL3Header:
      case bcmFieldDataOffsetBaseOuterL4Header:
          offset_base_encoding = 3;
          break;
      case bcmFieldDataOffsetBaseInnerL4Header:
          offset_base_encoding = 4;
          break;
      default:
          return (BCM_E_PARAM);
    }

    /* Set udf offset. */
    soc_mem_field32_set(unit, FP_UDF_OFFSETm, (uint32 *)&offset_buf,
                        offset_field[offset_idx], offset_value);
    soc_mem_field32_set(unit, FP_UDF_OFFSETm, (uint32 *)&offset_buf,
                        offset_base_field[offset_idx], 
                        offset_base_encoding);
    /* Write udf offset entry back to HW. */
    rv = soc_mem_write(unit, FP_UDF_OFFSETm, MEM_BLOCK_ALL, 
                       entry_idx, (uint32 *)&offset_buf);
    return (rv);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_l3_parse
 * Purpose:
 *     Parse udf tcam entry l3 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     hw_buf   - (IN) Hw buffer.
 *     pkt_fmt  - (OUT) Packet format structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_l3_parse(int unit, uint32 *hw_buf,
                                   bcm_field_data_packet_format_t *pkt_fmt)
{
    uint32 ethertype = -1;
    uint32 inner_iptype = -1;
    uint32 l3fields = -1;
    soc_mem_t mem = FP_UDF_TCAMm;

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == pkt_fmt)) {
        return (BCM_E_PARAM);
    }

    if (soc_mem_field32_get(unit, mem, hw_buf, L2_ETHER_TYPE_MASKf)) {
        ethertype = soc_mem_field32_get(unit, mem, hw_buf, L2_ETHER_TYPEf);
    }

    if (soc_mem_field32_get(unit, mem, hw_buf, INNER_IP_TYPE_MASKf)) {
        inner_iptype = soc_mem_field32_get(unit, mem, hw_buf, INNER_IP_TYPEf);
    }

    if (soc_mem_field32_get(unit, mem, hw_buf, L3_FIELDS_MASKf)) {
        l3fields = soc_mem_field32_get(unit, mem, hw_buf, L3_FIELDSf);
    }

    if ((ethertype == 0x800) && (inner_iptype ==  0)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
    } else if ((ethertype == 0x86dd) && (inner_iptype ==  0)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
    } else if ((ethertype == 0x800) && (l3fields ==  0x40000)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
    } else if ((ethertype == 0x800) && (l3fields ==  0x290000)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x40000)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x290000)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
    } else if ((ethertype == 0x800) && (l3fields ==  0x2f0800)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
    } else if ((ethertype == 0x800) && (l3fields ==  0x2f86dd)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x2f0800)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x2f86dd)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
    } else if ((ethertype == 0x8847) && (l3fields ==  0x1)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
        pkt_fmt->mpls = BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL;
    } else if ((ethertype == 0x8847) && (l3fields ==  0x2)) {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
        pkt_fmt->mpls = BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS;
    } else {
        pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
        pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_l3_init
 * Purpose:
 *     Initialize udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     pkt_fmt  - (IN) Packet format structure.
 *     hw_buf   - (IN/OUT) Hw buffer.
 *     priority - (IN/OUT) udf tcam entry priority. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_l3_init(int unit,  
                                  bcm_field_data_packet_format_t *pkt_fmt,
                                  uint32 *hw_buf, uint8 *priority)
{
    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == priority)) {
        return (BCM_E_PARAM);
    }

    if (pkt_fmt->tunnel == BCM_FIELD_DATA_FORMAT_TUNNEL_NONE) {
        /* inner ip type (none). */
        soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                            INNER_IP_TYPEf, 0);
        soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                            INNER_IP_TYPE_MASKf, 0x7);
        if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP4) {
            /* L2 ether type 0x800 */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPEf, 0x800);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPE_MASKf, 0xffff);
        } else if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP6) {
            /* L2 ether type 0x86dd */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPEf, 0x86dd);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPE_MASKf, 0xffff);
#ifdef LVL7_FIXUP
        } 
        else if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP_NONE) {
          /* Outer ip type 0 */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              OUTER_IP_TYPEf, 0);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              OUTER_IP_TYPE_MASKf, 0x7);
        }
        else
        {
          /* Outer ip type don't care */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              OUTER_IP_TYPE_MASKf, 0x0);
#else
        } else {
            /* Outer ip type 0 */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                OUTER_IP_TYPEf, 0);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                OUTER_IP_TYPE_MASKf, 0x7);
#endif
        }
    } else if (pkt_fmt->tunnel == BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP) {
        if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP4) {
            /* L2 ether type 0x800 */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPEf, 0x800);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPE_MASKf, 0xffff);

            if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP4) {
                /* Inner ip protocol v4. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x40000);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xff0000);
            } else if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP6) {
                /* Inner ip protocol v6. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x290000);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xff0000);
            } else {
                return (BCM_E_UNAVAIL);
            }
        } else if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP6) {
            /* L2 ether type 0x86dd */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPEf, 0x86dd);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPE_MASKf, 0xffff);

            if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP4) {
                /* Inner ip protocol v4. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x40000);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xff0000);
            } else if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP6) {
                /* Inner ip protocol v6. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x290000);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xff0000);
            } else {
                return (BCM_E_UNAVAIL);
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else if (pkt_fmt->tunnel == BCM_FIELD_DATA_FORMAT_TUNNEL_GRE) {
        if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP4) {
            /* L2 ether type 0x800 */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPEf, 0x800);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPE_MASKf, 0xffff);

            if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP4) {
                /* Inner ip protocol gre, gre ethertype 0x800. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x02f0800);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xffffff);
            } else if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP6) {
                /* Inner ip protocol gre, gre ethertype 0x86dd. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x02f86dd);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xffffff);
            } else {
                return (BCM_E_UNAVAIL);
            }
        } else if (pkt_fmt->outer_ip == BCM_FIELD_DATA_FORMAT_IP6) {
            /* L2 ether type 0x86dd */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPEf, 0x86dd);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L2_ETHER_TYPE_MASKf, 0xffff);

            if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP4) {
                /* Inner ip protocol gre, gre ethertype 0x800. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x02f0800);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xffffff);
            } else if (pkt_fmt->inner_ip == BCM_FIELD_DATA_FORMAT_IP6) {
                /* Inner ip protocol gre, gre ethertype 0x86dd. */
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDSf, 0x02f86dd);
                soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                    L3_FIELDS_MASKf, 0xffffff);
            } else {
                return (BCM_E_UNAVAIL);
            }
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else if (pkt_fmt->tunnel == BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) {
        soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                            L2_ETHER_TYPEf, 0x8847);
        soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                            L2_ETHER_TYPE_MASKf, 0xffff);
        if (pkt_fmt->mpls == BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL) {
            /* L2 ether type 0x8847, outer label1 bos == 1 */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L3_FIELDSf, 0x1);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L3_FIELDS_MASKf, 0xffffff);
        } else if (pkt_fmt->mpls == BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS) {
            /* L2 ether type 0x8847, outer label1 bos == 0 */
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L3_FIELDSf, 0x2);
            soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                                L3_FIELDS_MASKf, 0xffffff);
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else {
        return (BCM_E_UNAVAIL);
    }

    *priority += _FP_DATA_QUALIFIER_PRIO_L3_FORMAT;
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_vlanformat_parse
 * Purpose:
 *     Parse udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     hw_buf   - (IN) Hw buffer.
 *     vlanformat - (OUT) BCM_FIELD_DATA_FORMAT_L2_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_vlanformat_parse(int unit, uint32 *hw_buf,
                                           uint16 *vlanformat)
{
    uint32 tag_status;

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == vlanformat)) {
        return (BCM_E_PARAM);
    }

    if (soc_mem_field32_get(unit, FP_UDF_TCAMm, hw_buf, L2_TAG_STATUS_MASKf)) {
        tag_status = soc_mem_field32_get(unit, FP_UDF_TCAMm, hw_buf,
                                         L2_TAG_STATUSf);
        switch (tag_status) {
          case 0:
              *vlanformat = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
              break;
          case 1:
              *vlanformat = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
              break;
          case 2:
              *vlanformat = BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED;
              break;
          default:
              break;
        }
    } else {
        *vlanformat = BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_vlanformat_init
 * Purpose:
 *     Initialize udf tcam entry vlan tag format match key.
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     vlanformat - (IN) BCM_FIELD_DATA_FORMAT_L2_XXX
 *     hw_buf     - (IN/OUT) Hw buffer.
 *     priority   - (OUT) tcam entry priority. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_vlanformat_init(int unit, uint16 vlanformat, 
                                          uint32 *hw_buf, uint8 *priority)
{
    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == priority)) {
        return (BCM_E_PARAM);
    }

    /* Translate L2 flag bits to index */
    switch (vlanformat) { 
      case BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUSf, 0);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUS_MASKf, 0x3);
          *priority += _FP_DATA_QUALIFIER_PRIO_VLAN_FORMAT;
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUSf, 1);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUS_MASKf, 0x3);
          *priority += _FP_DATA_QUALIFIER_PRIO_VLAN_FORMAT;
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUSf, 2);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUS_MASKf, 0x3);
          *priority += _FP_DATA_QUALIFIER_PRIO_VLAN_FORMAT;
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUSf, 0);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TAG_STATUS_MASKf, 0);
          break;
      default:        
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_l2format_parse
 * Purpose:
 *     Parse udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     hw_buf   - (IN) Hw buffer.
 *     l2format - (OUT) BCM_FIELD_DATA_FORMAT_L2_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_l2format_parse(int unit, uint32 *hw_buf,
                                         uint16 *l2format)
{
    uint32 l2type;

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == l2format)) {
        return (BCM_E_PARAM);
    }

    if (soc_mem_field32_get(unit, FP_UDF_TCAMm, hw_buf, L2_TYPE_MASKf)) {
        l2type = soc_mem_field32_get(unit, FP_UDF_TCAMm, hw_buf, L2_TYPEf);
        switch (l2type) {
          case 0:
              *l2format = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
              break;
          case 1:
              *l2format = BCM_FIELD_DATA_FORMAT_L2_SNAP;
              break;
          case 2:
              *l2format = BCM_FIELD_DATA_FORMAT_L2_LLC;
              break;
          default:
              break;
        }
    } else {
        *l2format = BCM_FIELD_DATA_FORMAT_L2_ANY;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_l2format_init
 * Purpose:
 *     Initialize udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     l2format - (IN) BCM_FIELD_DATA_FORMAT_L2_XXX
 *     hw_buf   - (OUT) Hw buffer.
 *     priority - (OUT) Tcam entry priority
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_l2format_init(int unit, uint16 l2format, 
                                        uint32 *hw_buf, uint8 *priority)
{
    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == priority)) {
        return (BCM_E_PARAM);
    }

    /* Translate L2 flag bits to index */
    switch (l2format) { 
      case BCM_FIELD_DATA_FORMAT_L2_ETH_II:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPEf, 0);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPE_MASKf, 0x3);
          *priority += _FP_DATA_QUALIFIER_PRIO_L2_FORMAT;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_SNAP:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPEf, 1);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPE_MASKf, 0x3);
          *priority += _FP_DATA_QUALIFIER_PRIO_L2_FORMAT;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_LLC:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPEf, 2);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPE_MASKf, 0x3);
          *priority += _FP_DATA_QUALIFIER_PRIO_L2_FORMAT;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_ANY:
          /* L2 Format . */
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPEf, 0);
          soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                              L2_TYPE_MASKf, 0);
          break;
      default:        
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_parse
 * Purpose:
 *     Parse udf tcam entry key 
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     hw_buf   - (IN) Hw buffer.
 *     flags    - (OUT) Udf spec index (encoded flags). 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_parse(int unit, uint32 *hw_buf, uint32 *flags)
{
    bcm_field_data_packet_format_t pkt_fmt;
    int rv;

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == flags)) {
        return (BCM_E_PARAM);
    }
    bcm_field_data_packet_format_t_init(&pkt_fmt);

    /* Get valid bit. */
    if (soc_mem_field32_get(unit, FP_UDF_TCAMm, hw_buf, VALIDf)) {
        *flags = _BCM_FIELD_USER_OFFSET_VALID;
    } else {
        *flags = 0;
        return (BCM_E_NONE);
    }

    /* Parse vlan_tag format.*/
    rv = _field_tr2_udf_tcam_entry_vlanformat_parse(unit, hw_buf,
                                                    &pkt_fmt.vlan_tag);
    BCM_IF_ERROR_RETURN(rv);

    /* Parse l2 format.*/
    rv = _field_tr2_udf_tcam_entry_l2format_parse(unit, hw_buf, &pkt_fmt.l2);
    BCM_IF_ERROR_RETURN(rv);

    /* Parse l3 fields.*/
    rv = _field_tr2_udf_tcam_entry_l3_parse(unit, hw_buf, &pkt_fmt);
    BCM_IF_ERROR_RETURN(rv);

    if (BCM_FIELD_DATA_FORMAT_TUNNEL_NONE == pkt_fmt.tunnel) { 
        if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.outer_ip) {
            *flags |= BCM_FIELD_USER_IP4_HDR_ONLY; 
        } else  if (BCM_FIELD_DATA_FORMAT_IP6 ==  pkt_fmt.outer_ip) {
            *flags |= BCM_FIELD_USER_IP6_HDR_ONLY; 
        } else {
            *flags |= BCM_FIELD_USER_IP_NOTUSED; 
        }
    } else if (BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP == pkt_fmt.tunnel) {
        if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.outer_ip) {
            if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_IP4_OVER_IP4; 
            } else if (BCM_FIELD_DATA_FORMAT_IP6 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_IP6_OVER_IP4; 
            }
        } else {
            if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_IP4_OVER_IP6; 
            } else if (BCM_FIELD_DATA_FORMAT_IP6 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_IP6_OVER_IP6; 
            }
        }
    } else if (BCM_FIELD_DATA_FORMAT_TUNNEL_GRE == pkt_fmt.tunnel) {
        if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.outer_ip) {
            if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_GRE_IP4_OVER_IP4; 
            } else if (BCM_FIELD_DATA_FORMAT_IP6 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_GRE_IP6_OVER_IP4; 
            }
        } else {
            if (BCM_FIELD_DATA_FORMAT_IP4 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_GRE_IP4_OVER_IP6; 
            } else if (BCM_FIELD_DATA_FORMAT_IP6 ==  pkt_fmt.inner_ip) {
                *flags |= BCM_FIELD_USER_GRE_IP6_OVER_IP6; 
            }
        }
    } else if (BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS == pkt_fmt.tunnel) {
        if (BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL ==  pkt_fmt.mpls) {
            *flags |= BCM_FIELD_USER_ONE_MPLS_LABEL; 
        } else if (BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS == pkt_fmt.mpls) {
            *flags |= BCM_FIELD_USER_TWO_MPLS_LABELS; 
        }
    }


    /* Get VLAN tag format flags. */
    switch (pkt_fmt.vlan_tag) {
      case BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG:
          *flags |= BCM_FIELD_USER_VLAN_NOTAG;
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED:
          *flags |= BCM_FIELD_USER_VLAN_ONETAG;
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED:
          *flags |= BCM_FIELD_USER_VLAN_TWOTAG;
          break;
      default:
          break;
    }

    /* Get L2 format flags. */
    switch (pkt_fmt.l2) {
      case BCM_FIELD_DATA_FORMAT_L2_ETH_II:
          *flags |= BCM_FIELD_USER_L2_ETHERNET2;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_SNAP:
          *flags |= BCM_FIELD_USER_L2_SNAP;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_LLC:
          *flags |= BCM_FIELD_USER_L2_LLC;
          break;
      default:
          break;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_tcam_entry_init
 * Purpose:
 *     Write the info in udf_spec to hardware
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     flags    - (IN) Udf spec index (encoded flags). 
 *     hw_buf   - (OUT) Hw buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_tcam_entry_init(int unit, int flags, uint32 *hw_buf)
{
    bcm_field_data_packet_format_t pkt_fmt; /* Packet data format structure. */
    uint8 priority = 0;                     /* Unused for udf api calls.     */
    int rv;                                 /* Operation return status.      */

    /* Input parameters check. */
    if (NULL == hw_buf) {
        return (BCM_E_PARAM);
    }

    bcm_field_data_packet_format_t_init(&pkt_fmt);

    /* Map udf flags to tcam entry. */
    /* Set valid bit. */
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf, VALIDf, 1);

    /* Translate IP flag bits to index bits */
    switch (flags & BCM_FIELD_USER_IP_MASK) {
      case BCM_FIELD_USER_IP4_HDR_ONLY:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          break;
      case BCM_FIELD_USER_IP6_HDR_ONLY:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          break;
      case BCM_FIELD_USER_IP4_OVER_IP4:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          break;
      case BCM_FIELD_USER_IP6_OVER_IP4:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          break;
      case BCM_FIELD_USER_IP4_OVER_IP6:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          break;
      case BCM_FIELD_USER_IP6_OVER_IP6:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          break;
      case BCM_FIELD_USER_GRE_IP4_OVER_IP4:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          break;
      case BCM_FIELD_USER_GRE_IP6_OVER_IP4:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          break;
      case BCM_FIELD_USER_GRE_IP4_OVER_IP6:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          break;
      case BCM_FIELD_USER_GRE_IP6_OVER_IP6:
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          break;
      case BCM_FIELD_USER_ONE_MPLS_LABEL:     
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
          pkt_fmt.mpls = BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL;
          break;
      case BCM_FIELD_USER_TWO_MPLS_LABELS:     
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
          pkt_fmt.mpls = BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS;
          break;
      case BCM_FIELD_USER_IP_NOTUSED:     
          pkt_fmt.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt.outer_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
          pkt_fmt.inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
          break;
      default:     
          return (BCM_E_INTERNAL);
    }

    /* Translate VLAN flag bits to index */
    switch (flags & BCM_FIELD_USER_VLAN_MASK) {
      case  BCM_FIELD_USER_VLAN_NOTAG:
          pkt_fmt.vlan_tag = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
          break;
      case BCM_FIELD_USER_VLAN_ONETAG:
          pkt_fmt.vlan_tag =
              BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
          break;
      case BCM_FIELD_USER_VLAN_TWOTAG:
          pkt_fmt.vlan_tag =
              BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED;
          break;
      default:     
          return (BCM_E_INTERNAL);
    }

    /* Translate L2 flag bits to index */
    switch (flags & BCM_FIELD_USER_L2_MASK) { 
      case BCM_FIELD_USER_L2_ETHERNET2:
          pkt_fmt.l2 = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
          break;
      case BCM_FIELD_USER_L2_SNAP:
          pkt_fmt.l2 = BCM_FIELD_DATA_FORMAT_L2_SNAP;
          break;
      case BCM_FIELD_USER_L2_LLC:
          pkt_fmt.l2 = BCM_FIELD_DATA_FORMAT_L2_LLC;
          break;
      default:        
          return (BCM_E_INTERNAL);
    }
    /* Set vlan_tag format.*/
    rv = _field_tr2_udf_tcam_entry_vlanformat_init(unit, pkt_fmt.vlan_tag, 
                                                   hw_buf, &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set l2 format.*/
    rv = _field_tr2_udf_tcam_entry_l2format_init(unit, pkt_fmt.l2, 
                                                 hw_buf, &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set l3 fields.*/
    rv = _field_tr2_udf_tcam_entry_l3_init(unit, &pkt_fmt, 
                                           hw_buf, &priority);
    BCM_IF_ERROR_RETURN(rv);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_spec_clear
 * Purpose:
 *     Uninstall udf spec from the udf tcam.
 * Parameters:
 *     unit        - (IN) BCM device number. 
 *     stage_fc    - (IN) Field stage control structure. 
 *     udf_spec    - (IN) Udf offsets spec.
 *     offset_idx  - (IN) Udf spec offset index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_udf_spec_clear(int unit, 
                          _field_stage_t *stage_fc,
                          bcm_field_udf_spec_t *udf_spec, 
                          int offset_idx)
{
    _field_data_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.        */
    fp_udf_tcam_entry_t tcam_buf;          /* Udf tcam entry.               */
    int tcam_idx;                          /* Tcam insertion index.         */
    int idx_max;                           /* Udf spec index max.           */
    int flags;                             /* Udf API flags.                */
    int idx;                               /* Udf spec iteration index.     */
    int rv;                                /* Operation return status.      */

    /* Input parameters check. */
    if (NULL == udf_spec) {
        return (BCM_E_PARAM);
    }

    /* Iterate over valid offsets in udf spec. */
    idx_max =  COUNTOF(udf_spec->offset);
    for (idx = 0; idx < COUNTOF(udf_spec->offset); idx++) {
        if (0 == (udf_spec->offset[idx] & _BCM_FIELD_USER_OFFSET_VALID)) {
            continue;
        }

        /* Map table index to packet type flags. */
        flags = 0;
        rv = _bcm_field_trx_udf_index_to_flags(unit, idx, &flags);
        BCM_IF_ERROR_RETURN(rv);

        /* Initialize udf tcam entry. */
        sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));
        rv = _field_tr2_udf_tcam_entry_init(unit, flags, (uint32*)&tcam_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Reorganize the tcam and reserve an index for the entry. */
         rv = _field_tr2_udf_tcam_entry_match(unit, stage_fc,
                               (uint32*)&tcam_buf, &tcam_idx);
        BCM_IF_ERROR_RETURN(rv);

        tcam_entry_arr = stage_fc->data_ctrl->tcam_entry_arr;
        if (tcam_entry_arr[tcam_idx].ref_count > 0) {
            tcam_entry_arr[tcam_idx].ref_count--;
        }
        if (0 == tcam_entry_arr[tcam_idx].ref_count) {
            /* Reset udf offset entry. */
            rv = _field_tr2_udf_offset_entry_write(unit, tcam_idx, offset_idx,
                                         bcmFieldDataOffsetBaseHigigHeader, 0);
            BCM_IF_ERROR_RETURN(rv);

            /* Reset udf tcam entry. */
            rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx,
                               soc_mem_entry_null(unit, FP_UDF_TCAMm));
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_spec_remove
 * Purpose:
 *     Deallocate udf tcam entries used by udf qualifier.
 * Parameters:
 *     unit        - (IN) BCM device number. 
 *     stage_fc    - (IN) Stage field control structure. 
 *     offset_idx  - (IN) Udf offset index.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr2_udf_spec_remove(int unit, _field_stage_t *stage_fc, 
                           int offset_idx)
{
    _field_data_qualifier_p f_dq;      /* Field data qualifier pointer.  */
    _field_data_qualifier_p f_dq_prev; /* Field data qualifier pointer.  */
    int rv;                            /* Operation return status.       */

    f_dq_prev = f_dq = stage_fc->data_ctrl->data_qual;

    while (NULL != f_dq) {
        if (offset_idx == f_dq->qid) {
            rv = _field_tr2_udf_spec_clear(unit, stage_fc, 
                                           f_dq->spec, offset_idx);
            BCM_IF_ERROR_RETURN(rv);
            /* Remove qualifier from qualifiers linked list. */
            if (f_dq == f_dq_prev) {
                stage_fc->data_ctrl->data_qual = f_dq->next;
            } else {
                f_dq_prev->next = f_dq->next;
            }
            rv = _bcm_field_data_qualifier_free(unit, f_dq);
            return rv;
        }
        f_dq_prev = f_dq;
        f_dq = f_dq->next;

    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr2_udf_spec_insert
 * Purpose:
 *     Insert udf_spec describing tcam entries used by udf qualifier.
 * Parameters:
 *     unit        - (IN) BCM device number. 
 *     stage_fc    - (IN) Stage field control structure. 
 *     offset_idx  - (IN) Udf offset index. 
 *     udf_spec    - (IN) Udf qualifier spec.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr2_udf_spec_insert(int unit, _field_stage_t *stage_fc, 
                           int offset_idx, bcm_field_udf_spec_t *spec)
{
    _field_data_qualifier_p f_dq;      /* Field data qualifier pointer.  */
    int rv;                            /* Operation return status.       */

    /* Allocated internal data qualifier descriptor. */
    rv = _bcm_field_data_qualifier_alloc(unit, &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize data qualifier config. */
    f_dq->qid = offset_idx;
    sal_memcpy(f_dq->spec, spec, sizeof(bcm_field_udf_spec_t));
    f_dq->next = stage_fc->data_ctrl->data_qual;
    stage_fc->data_ctrl->data_qual = f_dq;
    f_dq = stage_fc->data_ctrl->data_qual;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_udf_delete
 * Purpose:
 *     Write the info in udf_spec to hardware
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     udf_num  - (IN) Udf 0/1 
 *     user_num - (IN) 0,1,2,3 Udf id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_tr2_udf_delete(int unit, uint32 udf_num, uint32 user_num)
{
    _field_stage_t *stage_fc;        /* Field stage control.      */
    int offset_idx;                  /* Offset index.             */
    int rv;                          /* Operation return status.  */

    /* Initialization. */
    offset_idx = (udf_num << 2) | user_num;

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Remove udf tcam entries if spec was previously installed. */
    rv = _field_tr2_udf_spec_remove(unit, stage_fc, offset_idx);
    BCM_IF_ERROR_RETURN(rv);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr2_udf_write
 * Purpose:
 *     Write the info in udf_spec to hardware
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     udf_spec - (IN) Udf offsets spec.
 *     udf_num  - (IN) Udf 0/1 
 *     user_num - (IN) 0,1,2,3 Udf id.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr2_udf_write(int unit, bcm_field_udf_spec_t *udf_spec, 
                     uint32 udf_num, uint32 user_num)
{
    fp_udf_tcam_entry_t tcam_buf;              /* Udf tcam entry.           */
    _field_stage_t *stage_fc;                  /* Field stage control.      */
    int offset_idx;                            /* Offset index.             */
    int tcam_idx;                              /* Tcam insertion index.     */
    int idx_max;                               /* Udf spec index max.       */
    int flags;                                 /* Udf API flags.            */
    int idx;                                   /* Udf spec iteration index. */
    int rv;                                    /* Operation return status.  */

    /* Input parameters check. */
    if (NULL == udf_spec) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    offset_idx = (udf_num << 2) | user_num;

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Remove udf tcam entries if spec was previously installed. */
    rv = _field_tr2_udf_spec_remove(unit, stage_fc, offset_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Iterate over valid offsets in udf spec. */
    idx_max =  COUNTOF(udf_spec->offset);
    for (idx = 0; idx < COUNTOF(udf_spec->offset); idx++) {
        if (0 == (udf_spec->offset[idx] & _BCM_FIELD_USER_OFFSET_VALID)) {
            continue;
        }

        /* Map table index to packet type flags. */
        flags = 0;
        rv = _bcm_field_trx_udf_index_to_flags(unit, idx, &flags);
        BCM_IF_ERROR_RETURN(rv);

        /* Initialize udf tcam entry. */
        sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));
        rv = _field_tr2_udf_tcam_entry_init(unit, flags, (uint32*)&tcam_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Reorganize the tcam and reserve an index for the entry. */
        rv = _field_tr2_udf_tcam_entry_insert(unit, (uint32*)&tcam_buf,
                                              _FP_DATA_QUALIFIER_PRIO_HIGHEST,
                                              &tcam_idx);
        BCM_IF_ERROR_RETURN(rv);

        /* Initialize udf offset entry. */
        rv =  _field_tr2_udf_offset_entry_write(unit, tcam_idx, offset_idx,
                                                bcmFieldDataOffsetBaseHigigHeader,
                                                udf_spec->offset[idx]);
        BCM_IF_ERROR_RETURN(rv);

        /* Insert udf tcam entry. */
        rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, 
                           tcam_idx, &tcam_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Insert udf spec to the data control structure. */
    rv = _field_tr2_udf_spec_insert(unit, stage_fc, offset_idx, udf_spec);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_tr2_udf_read
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
_field_tr2_udf_read(int unit, bcm_field_udf_spec_t *udf_spec, 
                      uint32 udf_num, uint32 user_num)
{
    bcm_field_data_offset_base_t offset_base;
    /* Udf offset base encoding.     */
    _field_stage_t *stage_fc;      /* Stage field contral structure.*/
    uint32 *entry_ptr;             /* Tcam entry pointer.           */
    uint32 *buffer;                /* Hw buffer to dma udf tcam.    */
    int alloc_size;                /* Memory allocation size.       */
    int entry_size;                /* Single tcam entry size.       */
    int offset_idx;                /* Offset index.                 */
    uint32 tbl_idx;                /* Udf spec array index.         */
    int idx_count;                 /* Udf tcam entry count.         */
    soc_mem_t mem;                 /* Udf tcam memory id.           */
    uint32 flags;                  /* Parsed entry flags.           */
    int idx;                       /* Tcam entries iterator.        */
    int rv;                        /* Operation return status.      */

    /* Input parameters check. */
    if (NULL == udf_spec) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    mem = FP_UDF_TCAMm;
    entry_size = sizeof(fp_udf_tcam_entry_t);
    idx_count  = soc_mem_index_count(unit, mem);
    alloc_size = entry_size * idx_count; 
    offset_idx = (udf_num << 2) | user_num;

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Allocate memory buffer. */
    buffer = soc_cm_salloc(unit, alloc_size, "Udf tcam");
    if (buffer == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Read table to the buffer. */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            soc_mem_index_min(unit, mem),
                            soc_mem_index_max(unit, mem), buffer);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, buffer);
        return (BCM_E_INTERNAL);
    }

    for (idx = 0; idx < idx_count; idx++) {
        if (stage_fc->data_ctrl->tcam_entry_arr[idx].ref_count) {
            /* Get used entry pointer. */
            entry_ptr = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                                     buffer, idx);

            /* Parse entry key to udf flags. */
            rv = _field_tr2_udf_tcam_entry_parse(unit, entry_ptr, &flags);
            if (BCM_FAILURE(rv)) {
                break;
            }

            /* Translate flags to udf spec index. */
            rv = _bcm_field_trx_udf_flags_to_index (unit, flags, &tbl_idx);
            if (BCM_FAILURE(rv)) {
                break;
            }

            /* Get offset value. */
            rv = _field_tr2_udf_offset_entry_read(unit,
                                                  idx, offset_idx, &offset_base,
                                                  &udf_spec->offset[tbl_idx]);
            if (BCM_FAILURE(rv)) {
                break;
            }
            udf_spec->offset[tbl_idx] |= _BCM_FIELD_USER_OFFSET_VALID;
        }
    }
    soc_cm_sfree(unit, buffer);
    return (rv);
}

/*
 * Function:
 *     _field_tr2_data_offset_install
 * Purpose:
 *     Write the info in udf_tcam/udf_offset to the hardware
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     tbl_idx  - (IN) FP_UDF_OFFSET table index.
 *     f_dq     - (IN) Data qualifier structure.
 *     offset   - (IN) Word offset value FP_UDF_OFFSETm.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_data_tcam_offset_install(int unit, 
                                    _field_data_qualifier_t *f_dq, 
                                    int tbl_idx, int offset)
{
    uint32  word_offset; /* Offset iterator.              */
    int     idx;         /* Data qualifier words iterator.*/
    int     rv;          /* Operation return status.      */   

    /* Input parameters check. */
    if (NULL == f_dq) {
        return (BCM_E_PARAM);
    }

    word_offset = (offset < 0) ? 0 : (uint32)offset;

    /* Find a proper word to insert the data. */
    for (idx = 0; idx <= _FP_DATA_DATA1_WORD_MAX; idx++) {
        if (0 == (f_dq->hw_bmap & (1 << idx))) {
            continue;
        }

        if (word_offset > 31) {
            return BCM_E_PARAM;
        }

        rv = _field_tr2_udf_offset_entry_write(unit, tbl_idx, idx,
                                               f_dq->offset_base, 
                                               word_offset);
        BCM_IF_ERROR_RETURN(rv);

        if (offset >= 0) {
            word_offset++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_tr2_data_word_offset_calc
 * Purpose:
 *     Calculate word offset based on data qualifier
 *     relative offset, base offset and common offset.
 * Parameters:
 *     unit             - (IN) BCM device number.
 *     f_dq             - (IN) Data qualifier structure.
 *     reltive_offset   - (IN) Word offset value FP_UDF_OFFSETm.
 *     word_offset      - (OUT) Calculated offset value. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_tr2_data_word_offset_calc(int unit, _field_data_qualifier_t *f_dq,
                                 int relative_offset, int *word_offset)
{
    int offset;

    /* Input parameters check. */
    if ((NULL == f_dq) || (NULL == word_offset)) {
        return (BCM_E_PARAM);
    }

    offset = f_dq->offset + relative_offset;

    switch (f_dq->offset_base) {
      case bcmFieldDataOffsetBaseHigigHeader:
      case bcmFieldDataOffsetBaseHigig2Header:
          offset = ((offset + 2) % 128) / 4;
          break;
      default:
          offset = (offset % 128) / 4;
          break;
    }

    *word_offset = offset;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_tr2_data_qualifier_etype_tcam_key_init
 * Purpose:
 *      Initialize ethertype based udf tcam entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      etype      - (IN) Ethertype based offset specification.
 *      hw_buf     - (OUT) Hardware buffer. 
 *      priority   - (OUT) Tcam entry priority
 * Returns:
 *      BCM_E_XXX
 */
int
_field_tr2_data_qualifier_etype_tcam_key_init(int unit, 
                                              bcm_field_data_ethertype_t *etype,
                                              uint32 *hw_buf, uint8 *priority)
{
    int rv;               /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == priority)) {
        return (BCM_E_PARAM);
    }

    *priority = 0;

    /* Set valid bit. */
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf, VALIDf, 1);

    /* Set l2 format. */
    rv = _field_tr2_udf_tcam_entry_l2format_init(unit, etype->l2, 
                                                 hw_buf, priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set vlan tag format. */
    rv = _field_tr2_udf_tcam_entry_vlanformat_init(unit, etype->vlan_tag, 
                                                   hw_buf, priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set ethertype value. */
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                        L2_ETHER_TYPEf, etype->ethertype);
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                        L2_ETHER_TYPE_MASKf, 0xffff);
    *priority += _FP_DATA_QUALIFIER_PRIO_MISC;

    return (rv);
}

/*
 * Function:
 *      _field_tr2_data_qualifier_ip_proto_tcam_key_init
 * Purpose:
 *      Initialize ethertype based udf tcam entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ip_proto   - (IN) Ip Protocol based offset specification.
 *      hw_buf     - (OUT) Hardware buffer. 
 *      priority   - (OUT) Tcam entry priority
 * Returns:
 *      BCM_E_XXX
 */
int
_field_tr2_data_qualifier_ip_proto_tcam_key_init(int unit, 
                                 bcm_field_data_ip_protocol_t *ip_proto,
                                 uint32 *hw_buf, uint8 *priority)
{
    int rv;               /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == priority)) {
        return (BCM_E_PARAM);
    }

    *priority = 0;

    /* Set valid bit. */
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf, VALIDf, 1);

    /* Set l2 format. */
    rv = _field_tr2_udf_tcam_entry_l2format_init(unit, ip_proto->l2, 
                                                 hw_buf, priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set vlan tag format. */
    rv = _field_tr2_udf_tcam_entry_vlanformat_init(unit, ip_proto->vlan_tag, 
                                                   hw_buf, priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set ethertype value. */
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                        L3_FIELDSf, ip_proto->ip);
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf,
                        L3_FIELDS_MASKf, 0xff0000);
    *priority += _FP_DATA_QUALIFIER_PRIO_MISC;

    return (rv);
}

/*
 * Function:
 *      _field_tr2_data_qualifier_pkt_format_tcam_key_init
 * Purpose:
 *      Initialize ethertype based udf tcam entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      pkt_format - (IN) Packet format based offset specification.
 *      hw_buf     - (OUT) Hardware buffer. 
 *      priority   - (OUT) Tcam entry priority
 * Returns:
 *      BCM_E_XXX
 */
int
_field_tr2_data_qualifier_pkt_format_tcam_key_init(int unit, 
                                 bcm_field_data_packet_format_t *pkt_format,
                                 uint32 *hw_buf, uint8 *priority)
{
    int rv;               /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == hw_buf) || (NULL == priority)) {
        return (BCM_E_PARAM);
    }

    *priority = 0;

    /* Set valid bit. */
    soc_mem_field32_set(unit, FP_UDF_TCAMm, hw_buf, VALIDf, 1);

    /* Set l2 format. */
    rv = _field_tr2_udf_tcam_entry_l2format_init(unit, pkt_format->l2, 
                                                 hw_buf, priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set vlan tag format. */
    rv = _field_tr2_udf_tcam_entry_vlanformat_init(unit, pkt_format->vlan_tag, 
                                                   hw_buf, priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Set l3 packet format. */
    rv = _field_tr2_udf_tcam_entry_l3_init(unit, pkt_format, hw_buf, priority);

    return (rv);
}

/*
 * Function:
 *      _bcm_field_tr2_data_qualifier_ethertype_add
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
_bcm_field_tr2_data_qualifier_ethertype_add(int unit,  int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    fp_udf_tcam_entry_t     tcam_buf;     /* Udf tcam entry.            */
    _field_stage_t          *stage_fc;    /* Stage field control.       */
    _field_data_qualifier_t *f_dq;        /* Data qualifier descriptor. */
    uint8                   priority;     /* Udf tcam entry priority.   */
    int                     tcam_idx;     /* Tcam insertion index.      */
    int                     offset;       /* Qualifier tcam offset.     */
    int                     rv;           /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == etype) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize ethertype data qualifier key. */
    rv = _field_tr2_data_qualifier_etype_tcam_key_init(unit, etype, 
                                                       (uint32 *)&tcam_buf, 
                                                       &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Reorganize the tcam and reserve an index for the entry. */
    rv = _field_tr2_udf_tcam_entry_insert(unit, (uint32*)&tcam_buf,
                                          priority, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate word offset. */
    rv = _field_tr2_data_word_offset_calc(unit, f_dq, 
                                          etype->relative_offset, &offset);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize udf offset entry. */
    rv = _field_tr2_data_tcam_offset_install(unit, f_dq, tcam_idx, offset);
    BCM_IF_ERROR_RETURN(rv);

    /* Insert udf tcam entry. */
    rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx, &tcam_buf);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_tr2_data_qualifier_ethertype_delete
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
_bcm_field_tr2_data_qualifier_ethertype_delete(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    _field_data_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.     */
    fp_udf_tcam_entry_t      tcam_buf;     /* Udf tcam entry.            */
    _field_stage_t           *stage_fc;    /* Stage field control.       */
    _field_data_qualifier_t  *f_dq;        /* Data qualifier descriptor. */
    uint8                    priority;     /* Udf tcam entry priority.   */
    int                      tcam_idx;     /* Tcam insertion index.      */
    int                      rv;           /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == etype) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);


    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize ethertype data qualifier key. */
    rv = _field_tr2_data_qualifier_etype_tcam_key_init(unit, etype,
                                                       (uint32 *)&tcam_buf,
                                                       &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Match the tcam entry. */
    rv = _field_tr2_udf_tcam_entry_match(unit, stage_fc,
                               (uint32*)&tcam_buf, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    tcam_entry_arr = stage_fc->data_ctrl->tcam_entry_arr;
    if (tcam_entry_arr[tcam_idx].ref_count > 0) {
        tcam_entry_arr[tcam_idx].ref_count--;
    }
    if (0 == tcam_entry_arr[tcam_idx].ref_count) {
        /* Initialize udf offset entry. */
        rv = _field_tr2_data_tcam_offset_install(unit, f_dq, tcam_idx, -1);
        BCM_IF_ERROR_RETURN(rv);
        /* Reset udf tcam entry. */
        rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx,
                           &soc_mem_entry_null(unit, FP_UDF_TCAMm));
        BCM_IF_ERROR_RETURN(rv);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_tr2_data_qualifier_ip_protocol_add
 * Purpose:
 *      Add ipprotocol based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      ip_proto   - (IN) Ip protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_tr2_data_qualifier_ip_protocol_add(int unit,  int qual_id,
                                 bcm_field_data_ip_protocol_t *ip_proto)
{
    fp_udf_tcam_entry_t     tcam_buf;     /* Udf tcam entry.            */
    _field_stage_t          *stage_fc;    /* Stage field control.       */
    _field_data_qualifier_t *f_dq;        /* Data qualifier descriptor. */
    uint8                   priority;     /* Udf tcam entry priority.   */
    int                     tcam_idx;     /* Tcam insertion index.      */
    int                     offset;       /* Qualifier tcam offset.     */
    int                     rv;           /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == ip_proto) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize ipprotocol data qualifier key. */
    rv = _field_tr2_data_qualifier_ip_proto_tcam_key_init(unit, ip_proto, 
                                                          (uint32 *)&tcam_buf, 
                                                          &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Reorganize the tcam and reserve an index for the entry. */
    rv = _field_tr2_udf_tcam_entry_insert(unit, (uint32*)&tcam_buf,
                                          priority, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate word offset. */
    rv = _field_tr2_data_word_offset_calc(unit, f_dq, 
                                          ip_proto->relative_offset, &offset);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize udf offset entry. */
    rv = _field_tr2_data_tcam_offset_install(unit, f_dq, tcam_idx, offset);
    BCM_IF_ERROR_RETURN(rv);

    /* Insert udf tcam entry. */
    rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx, &tcam_buf);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_tr2_data_qualifier_ip_protocol_delete
 * Purpose:
 *      Remove ipprotocol based offset from data qualifier object. 
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      ip_proto   - (IN) Ip protocol based offset specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_tr2_data_qualifier_ip_protocol_delete(int unit, int qual_id,
                                 bcm_field_data_ip_protocol_t *ip_proto)
{
    _field_data_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.     */
    fp_udf_tcam_entry_t      tcam_buf;     /* Udf tcam entry.            */
    _field_stage_t           *stage_fc;    /* Stage field control.       */
    _field_data_qualifier_t  *f_dq;        /* Data qualifier descriptor. */
    uint8                    priority;     /* Udf tcam entry priority.   */
    int                      tcam_idx;     /* Tcam insertion index.      */
    int                      rv;           /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == ip_proto) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize ipprotocol data qualifier key. */
    rv = _field_tr2_data_qualifier_ip_proto_tcam_key_init(unit, ip_proto,
                                                          (uint32 *)&tcam_buf,
                                                          &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Match the tcam entry. */
    rv = _field_tr2_udf_tcam_entry_match(unit, stage_fc,
                                         (uint32*)&tcam_buf, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    tcam_entry_arr = stage_fc->data_ctrl->tcam_entry_arr;
    if (tcam_entry_arr[tcam_idx].ref_count > 0) {
        tcam_entry_arr[tcam_idx].ref_count--;
    }
    if (0 == tcam_entry_arr[tcam_idx].ref_count) {
        /* Initialize udf offset entry. */
        rv = _field_tr2_data_tcam_offset_install(unit, f_dq, tcam_idx, -1);
        BCM_IF_ERROR_RETURN(rv);
        /* Reset udf tcam entry. */
        rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx,
                           &soc_mem_entry_null(unit, FP_UDF_TCAMm));
        BCM_IF_ERROR_RETURN(rv);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_tr2_data_qualifier_packet_format_add
 * Purpose:
 *      Add ipprotocol based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      pkt_format - (IN) Packet format based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_tr2_data_qualifier_packet_format_add(int unit,  int qual_id,
                                 bcm_field_data_packet_format_t *pkt_format)
{
    fp_udf_tcam_entry_t     tcam_buf;     /* Udf tcam entry.            */
    _field_stage_t          *stage_fc;    /* Stage field control.       */
    _field_data_qualifier_t *f_dq;        /* Data qualifier descriptor. */
    uint8                   priority;     /* Udf tcam entry priority.   */
    int                     tcam_idx;     /* Tcam insertion index.      */
    int                     offset;       /* Qualifier tcam offset.     */
    int                     rv;           /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == pkt_format) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize packet format data qualifier key. */
    rv = _field_tr2_data_qualifier_pkt_format_tcam_key_init(unit, pkt_format, 
                                                          (uint32 *)&tcam_buf, 
                                                          &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Reorganize the tcam and reserve an index for the entry. */
    rv = _field_tr2_udf_tcam_entry_insert(unit, (uint32*)&tcam_buf,
                                          priority, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate word offset. */
    rv = _field_tr2_data_word_offset_calc(unit, f_dq, 
                                          pkt_format->relative_offset, &offset);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize udf offset entry. */
    rv = _field_tr2_data_tcam_offset_install(unit, f_dq, tcam_idx, offset);
    BCM_IF_ERROR_RETURN(rv);

    /* Insert udf tcam entry. */
    rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx, &tcam_buf);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_field_tr2_data_qualifier_packet_format_delete
 * Purpose:
 *      Remove ipprotocol based offset from data qualifier object. 
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      pkt_format - (IN) Packet format based udf offset specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_tr2_data_qualifier_packet_format_delete(int unit, int qual_id,
                                 bcm_field_data_packet_format_t *pkt_format)
{
    _field_data_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.     */
    fp_udf_tcam_entry_t      tcam_buf;     /* Udf tcam entry.            */
    _field_stage_t           *stage_fc;    /* Stage field control.       */
    _field_data_qualifier_t  *f_dq;        /* Data qualifier descriptor. */
    uint8                    priority;     /* Udf tcam entry priority.   */
    int                      tcam_idx;     /* Tcam insertion index.      */
    int                      rv;           /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == pkt_format) {
        return (BCM_E_PARAM);
    }

    /* Initialization. */
    sal_memset(&tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize packet_format data qualifier key. */
    rv = _field_tr2_data_qualifier_pkt_format_tcam_key_init(unit, pkt_format,
                                                          (uint32 *)&tcam_buf,
                                                          &priority);
    BCM_IF_ERROR_RETURN(rv);

    /* Match the tcam entry. */
    rv = _field_tr2_udf_tcam_entry_match(unit, stage_fc,
                                         (uint32*)&tcam_buf, &tcam_idx);
    BCM_IF_ERROR_RETURN(rv);

    tcam_entry_arr = stage_fc->data_ctrl->tcam_entry_arr;
    if (tcam_entry_arr[tcam_idx].ref_count > 0) {
        tcam_entry_arr[tcam_idx].ref_count--;
    }
    if (0 == tcam_entry_arr[tcam_idx].ref_count) {
        /* Initialize udf offset entry. */
        rv = _field_tr2_data_tcam_offset_install(unit, f_dq, tcam_idx, -1);
        BCM_IF_ERROR_RETURN(rv);
        /* Reset udf tcam entry. */
        rv = soc_mem_write(unit, FP_UDF_TCAMm, MEM_BLOCK_ALL, tcam_idx,
                           &soc_mem_entry_null(unit, FP_UDF_TCAMm));
        BCM_IF_ERROR_RETURN(rv);
    }

    return (BCM_E_NONE);
}


#if defined(INCLUDE_L3)

/*
 * Function:
 *     _field_tr2_l2_actions_nh_desroy
 *
 * Purpose:
 *     Free l3 next hop and egress interface for  
 *     l2 fields update action. 
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     nh_index - (IN) Next hop index.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr2_l2_actions_nh_destroy(int unit, int nh_index)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* hw entry  buffer.            */
    bcm_l3_egress_t egr;                    /* Egress forwarding object.    */
    uint32 intf;                            /* Egress L3 interface id.      */
    int rv = BCM_E_NONE;                    /* Operation return status.     */

    /* Initialization. */
    bcm_l3_egress_t_init(&egr);

    /* Read next hop. */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm, SOC_BLOCK_ANY, 
                      nh_index, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Get interface id. */
    intf = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                               hw_buf, INTF_NUMf);

    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, SOC_BLOCK_ALL, nh_index, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_xgs3_nh_del(unit, 0, nh_index);
    BCM_IF_ERROR_RETURN(rv);

    /* Reset interface entry in hw. */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    rv = soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL, intf, hw_buf);
    BCM_IF_ERROR_RETURN(rv);
    /* Free interface id. */
    rv = _bcm_xgs3_egress_l3_intf_id_free(unit, intf);
    BCM_IF_ERROR_RETURN(rv);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_tr2_l2_actions_nh_create
 *
 * Purpose:
 *     Free l3 next hop and egress interface for  
 *     l2 fields update action. 
 * Parameters:
 *     unit    - (IN) BCM device number.
 *     da_fa   - (IN) Update destination mac action.
 *     sa_fa   - (IN) Update source mac action.
 *     vid_fa  - (IN) Update outer vid action.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_tr2_l2_actions_nh_create(int unit, _field_action_t *da_fa, 
                                _field_action_t *sa_fa, 
                                _field_action_t *vid_fa)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* hw entry  buffer.            */
    bcm_l3_egress_t egr;            /* Egress forwarding object.     */
    bcm_l3_intf_t  intf;            /* Egress L3 interface id.       */
    int nh_index;                   /* Next hop index.               */
    int nh_flags;                   /* Next hop flags.               */
    int tmp_rv = BCM_E_NONE;        /* Operation return status.      */
    int rv = BCM_E_NONE;            /* Operation return status.      */


    /* Initialization. */
    bcm_l3_egress_t_init(&egr);
    bcm_l3_intf_t_init(&intf);

    /* Extract the policy info from the entry structure. */
    if (NULL != da_fa) {
        SAL_MAC_ADDR_FROM_UINT32(egr.mac_addr, da_fa->param);
    }

    if (NULL != sa_fa) {
        SAL_MAC_ADDR_FROM_UINT32(intf.l3a_mac_addr, sa_fa->param);
    }

    if (NULL != vid_fa) {
        intf.l3a_vid = vid_fa->param[0];
    }

    /* Create egress l3 interface. */
    rv = _bcm_xgs3_egress_l3_intf_id_alloc(unit, &intf.l3a_intf_id);
    BCM_IF_ERROR_RETURN(rv);

    /* Write egress interface to the hw. */
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set mac address. */
    soc_mem_mac_addr_set(unit, EGR_L3_INTFm, hw_buf, 
                         MAC_ADDRESSf, intf.l3a_mac_addr);

    /* Set vlan id. */
    soc_mem_field32_set(unit, EGR_L3_INTFm, hw_buf, VIDf, intf.l3a_vid);

    /* Write interface configuration to the HW. */
    rv = soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                       intf.l3a_intf_id, hw_buf);
    if (BCM_FAILURE(rv)) {
        tmp_rv = _bcm_xgs3_egress_l3_intf_id_free(unit, intf.l3a_intf_id);
        return rv; 
    }
    /* Allocate next hop entry. */
    nh_flags = _BCM_L3_SHR_MATCH_DISABLE | _BCM_L3_SHR_WRITE_DISABLE;
    rv = bcm_xgs3_nh_add(unit, nh_flags, &egr, &nh_index);
    if (BCM_FAILURE(rv)) {
        sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        tmp_rv = soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                               intf.l3a_intf_id, hw_buf);
        tmp_rv = _bcm_xgs3_egress_l3_intf_id_free(unit, intf.l3a_intf_id);
        return (rv);
    }

    /* Write egress next hop entry. */ 
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Set next hop mac address. */
    soc_mem_mac_addr_set(unit, EGR_L3_NEXT_HOPm, hw_buf, 
                         MAC_ADDRESSf, egr.mac_addr);

    /* Set interface id. */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf, 
                        INTF_NUMf, intf.l3a_intf_id);

    /* Set Disable flags. */
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf, 
                        L3__L3_UC_TTL_DISABLEf, 0x1);

    if (NULL == vid_fa) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf, 
                            L3__L3_UC_VLAN_DISABLEf, 0x1);
    }

    if (NULL == sa_fa) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf, 
                            L3__L3_UC_SA_DISABLEf, 0x1);
    }

    if (NULL == da_fa) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, hw_buf, 
                            L3__L3_UC_DA_DISABLEf, 0x1);
    }

    /* Insert next hop information. */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, SOC_BLOCK_ALL, 
                       nh_index, hw_buf);
    if (BCM_FAILURE(rv)) {
        sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        tmp_rv = soc_mem_write(unit, EGR_L3_INTFm, SOC_BLOCK_ALL,
                               intf.l3a_intf_id, hw_buf);
        tmp_rv = _bcm_xgs3_egress_l3_intf_id_free(unit, intf.l3a_intf_id);
        return (rv);
    }

    /* Preserve next hop index in the action structure. */
    if (NULL != vid_fa) {
        vid_fa->hw_index = nh_index;
    }

    if (NULL != sa_fa) {
        sa_fa->hw_index = nh_index;
    }

    if (NULL != da_fa) {
        da_fa->hw_index = nh_index;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr2_l2_actions_hw_alloc
 *
 * Purpose:
 *     Allocate l3 next hop and egress interface for  
 *     l2 fields update action. 
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Field entry descriptor.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_tr2_l2_actions_hw_alloc(int unit, _field_entry_t *f_ent)
{
    _field_action_t *vid_fa;        /* Update outer vid action.      */
    _field_action_t *sa_fa;         /* Update source mac action.     */
    _field_action_t *da_fa;         /* Update destination mac action.*/
    _field_action_t *fa;            /* Field action descriptor.      */
    int rv = BCM_E_NONE;            /* Operation return status.      */

    /* Applicable to stage ingress on TRX devices only. */
    if ((0 == SOC_IS_TRX(unit)) || 
        (_BCM_FIELD_STAGE_INGRESS != f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }

    /* Initialization. */
    sa_fa = da_fa = vid_fa = NULL;

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        switch (fa->action) {
          case bcmFieldActionSrcMacNew:
              if (_FP_INVALID_INDEX != fa->hw_index) {
                  fa->old_index = fa->hw_index;
              }
              sa_fa = fa;
              break;
          case bcmFieldActionDstMacNew:
              if (_FP_INVALID_INDEX != fa->hw_index) {
                  fa->old_index = fa->hw_index;
              }
              da_fa = fa;
              break;
          case bcmFieldActionOuterVlanNew:
              if (_FP_INVALID_INDEX != fa->hw_index) {
                  fa->old_index = fa->hw_index;
              }
              vid_fa = fa;
              break;
          default:
              continue;
        }
    }

    /* Create nh entry. */
    if ((NULL != vid_fa) || (NULL != da_fa) || (NULL != sa_fa)) {
        rv = _field_tr2_l2_actions_nh_create(unit, da_fa, sa_fa, vid_fa);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_tr2_l2_actions_hw_free
 *
 * Purpose:
 *     Free l3 next hop and egress interface for  
 *     l2 fields update action. 
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     f_ent     - (IN) Field entry descriptor.
 *     flags     - (IN) Free flags (old/new/both). 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_tr2_l2_actions_hw_free(int unit, _field_entry_t *f_ent, 
                                  uint32 flags)
{
    _field_action_t *fa;            /* Field action descriptor.      */
    int nh_index;                   /* Next hop index.               */
    int old_nh_index;               /* Old next hop index.           */
    int rv = BCM_E_NONE;            /* Operation return status.      */

    /* Applicable to stage ingress on TRX devices only. */
    if ((0 == SOC_IS_TRX(unit)) || 
        (_BCM_FIELD_STAGE_INGRESS != f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }

    /* Initialization. */
    nh_index = old_nh_index = _FP_INVALID_INDEX;

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        switch (fa->action) {
          case bcmFieldActionSrcMacNew:
          case bcmFieldActionOuterVlanNew:
          case bcmFieldActionDstMacNew:
              if ((flags & _FP_ACTION_RESOURCE_FREE) && 
                  (_FP_INVALID_INDEX != fa->hw_index)) {
                  nh_index = fa->hw_index;
                  fa->hw_index = _FP_INVALID_INDEX;
              }
              if ((flags & _FP_ACTION_OLD_RESOURCE_FREE) && 
                  (_FP_INVALID_INDEX != fa->old_index)) {
                  old_nh_index = fa->old_index;
                  fa->old_index = _FP_INVALID_INDEX;
              }
              break;
          default:
              break;
        }
    }

    /* Destroy old next hop if any. */
    if (_FP_INVALID_INDEX != old_nh_index) {
        rv = _field_tr2_l2_actions_nh_destroy(unit, old_nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }
    if (_FP_INVALID_INDEX != nh_index) {
        rv = _field_tr2_l2_actions_nh_destroy(unit, nh_index);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

#endif /* INCLUDE_L3 */

/*
 * Function:
 *     _field_tr2_functions_init
 *
 * Purpose:
 *     Set up functions pointers 
 *
 * Parameters:
 *     stage_fc - (IN/OUT) pointers to stage control block whe the device 
 *                         and stage specific functions will be registered.
 *
 * Returns:
 *     nothing
 * Notes:
 */
STATIC void
_field_tr2_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_udf_spec_set         = _bcm_field_trx_udf_spec_set;
    functions->fp_udf_spec_get         = _bcm_field_trx_udf_spec_get;
    functions->fp_udf_write            = _field_tr2_udf_write;
    functions->fp_udf_read             = _field_tr2_udf_read;
    functions->fp_group_install        = _bcm_field_fb_group_install;
    functions->fp_selcodes_install     = _bcm_field_trx_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_trx_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _bcm_field_tr_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_policer_install      = _bcm_field_trx_policer_install;
    functions->fp_slice_reinit	       = NULL;
    functions->fp_write_slice_map      = _bcm_field_tr_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_trx_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_trx_qualify_ip_type_get;
    functions->fp_action_support_check = _bcm_field_trx_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_trx_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_tr_counter_get;
    functions->fp_counter_set          = _bcm_field_tr_counter_set;
    functions->fp_stat_index_get       = _bcm_field_trx_stat_index_get;
    functions->fp_action_params_check  = _bcm_field_trx_action_params_check;
    functions->fp_egress_key_match_type_set = 
        _bcm_field_trx_egress_key_match_type_set;
    functions->fp_external_entry_install  = _bcm_field_tr_external_entry_install;
    functions->fp_external_entry_remove   = _bcm_field_tr_external_entry_remove;
    functions->fp_external_entry_prio_set = _bcm_field_tr_external_entry_prio_set;
}
#else /* BCM_TRIUMPH2_SUPPORT && BCM_FIELD_SUPPORT */
int _triumph2_field_not_empty;
#endif  /* BCM_TRIUMPH2_SUPPORT && BCM_FIELD_SUPPORT */
