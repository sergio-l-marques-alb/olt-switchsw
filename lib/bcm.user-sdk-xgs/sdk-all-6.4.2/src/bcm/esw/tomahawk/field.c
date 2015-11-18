/* $Id: field.c,v 1.0 Broadcom SDK $
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
 *
 * File:        field.c
 * Purpose:     BCM56960 Field Processor installation functions.
 */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/trident2.h>
#include <soc/triumph3.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/field.h>
#include <bcm/tunnel.h>

#include <bcm_int/common/multicast.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/policer.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>


#define ALIGN32(x)      (((x) + 31) & ~31)

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_3

#endif

static soc_field_t _th_efp_slice_mode[4][2] =  {
          {SLICE_0_MODEf, SLICE_0_IPV6_KEY_MODEf},
          {SLICE_1_MODEf, SLICE_1_IPV6_KEY_MODEf},
          {SLICE_2_MODEf, SLICE_2_IPV6_KEY_MODEf},
          {SLICE_3_MODEf, SLICE_3_IPV6_KEY_MODEf}
       };

/*
 * Function:
 *     _bcm_field_th_stage_init
 *
 * Purpose:
 *     Initialize required flags for all stages in FP module.
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     stage_fc         - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stage_init(int unit, _field_stage_t *stage_fc)
{
    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            /* Flags */
            stage_fc->flags |= (_FP_STAGE_SLICE_ENABLE
                                | _FP_STAGE_AUTO_EXPANSION
                                | _FP_STAGE_GLOBAL_COUNTERS);
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, VFP_TCAMm);
            stage_fc->tcam_slices = 4;
            break;
        case _BCM_FIELD_STAGE_INGRESS:
            /* Flags */
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            /* Flags */
            stage_fc->flags |= _FP_STAGE_SLICE_ENABLE
                | _FP_STAGE_GLOBAL_COUNTERS
                | _FP_STAGE_SEPARATE_PACKET_BYTE_COUNTERS
                | _FP_STAGE_AUTO_EXPANSION
                | _FP_STAGE_MULTI_PIPE_COUNTERS;
            /* Slice geometry */
            stage_fc->tcam_sz     = soc_mem_index_count(unit, EFP_TCAMm);
            stage_fc->tcam_slices = 4;
            stage_fc->pipes = 4;
            break;
        default:
            return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.

 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_th_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    const unsigned f1_offset = 164, f2_offset = 36, f3_offset = 0;
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

    /* FPF1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyInnerIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_offset, 3);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_offset, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 3, 1);
    /* 1-bit IpInfo is used to qualify on BCM_FIELD_IP_CHECKSUM_OK */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 4, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 9, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 13, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 13, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 25, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 26, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 29, 2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f1_offset + 31,
                               5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                               _bcmFieldSliceSelFpf1, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 31,
                               5);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 8, /* Port value in SGLP */
                     f1_offset + 44, 8, /* Module value in SGLP */
                     f1_offset + 52, 1, /* Trunk bit in SGLP (should be 0) */
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityGlp,
                     0,
                     f1_offset + 36, 16, /* trunk id field of SGLP */
                     f1_offset + 52, 1,  /* trunk bit of SGLP */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMplsGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMplsGport,
                     0,
                     f1_offset + 36, 17, /* S_FIELD */
                     f1_offset + 56, 1,  /* SVP_VALID */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcMimGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityMimGport,
                     0,
                     f1_offset + 36, 17, /* S_FIELD */
                     f1_offset + 56, 1,  /* SVP_VALID */
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcWlanGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, _bcmFieldFwdEntityWlanGport,
                     0,
                     f1_offset + 36, 17, /* S_FIELD */
                     f1_offset + 56, 1,  /* SVP_VALID */
                     0, 0,
                     0
                     );

     _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcVxlanGport,
                      _bcmFieldDevSelDisable, 0,
                      _bcmFieldSliceSelFpf1, 0,
                      _bcmFieldSliceSrcEntitySelect,
                      _bcmFieldFwdEntityVxlanGport,
                      0,
                      f1_offset + 36, 17, /* S_FIELD */
                      f1_offset + 56, 1,  /* SVP_VALID */
                      0, 0,
                      0
                      );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModPortGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, 
                     _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36, 16, /* mod + port field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, 
                     _bcmFieldFwdEntityModPortGport,
                     0,
                     f1_offset + 36 + 8, 8, /* mod field of unresolved SGLP */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, 
                     _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36, 8, /* ingress port field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf1, 0,
                     _bcmFieldSliceSrcEntitySelect, 
                     _bcmFieldFwdEntityPortGroupNum,
                     0,
                     f1_offset + 36 + 8, 8, /* ingress port group field */
                     0, 0,
                     0, 0,
                     0
                     );

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf1, 0, f1_offset + 56, 1);


    /* F2_0 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, f2_offset + 8, 6);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 14,
                               2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 14,
                               2);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 24,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 24,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 56,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 56,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 96,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 0,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 96,
                               32);

    /* F2_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf2, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 128);

    /* F2_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf2, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 128);

    /* F2_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 16, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 32, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, f2_offset + 80, 48);

    /* F2_4 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 48,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 4,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 48,
                               32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, f2_offset + 80, 48);

    /* F2_5 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 48,
                               32);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                               _bcmFieldSliceSelFpf2, 5,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 48,
                               32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, f2_offset + 80, 48);

    /* F2_6 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                               64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf2, 6,
                               _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                               64);

    /* F2_7 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVnTag,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 32, 33);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 64, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, f2_offset + 104, 24);

    /* F2_8 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 8, f2_offset, 128);

    /* F2_9 */
    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 9, f2_offset, 128);

    /* F2_10 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanInner,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 0, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanOuter,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 3, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFcoeSOF,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 6, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDFCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 14, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanCSCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 22, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanFCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 30, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanType,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 54, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanDstId,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 62, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanSrcId,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 86, 24);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanRCtl,
                 _bcmFieldSliceSelFpf2, 10, f2_offset + 110, 8);

    /* F3_0 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 13, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, f3_offset + 16, 16);

    /* F3_1 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 16,
                               16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                               _bcmFieldSliceSelFpf3, 1,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                               16);

    /* F3_2 */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 8,
                               8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                               _bcmFieldSliceSelFpf3, 2,
                               _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 8,
                               8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 16, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, 2, f3_offset + 24, 8);


    /* F3_3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 20, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 21, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModuleGport,
                 _bcmFieldSliceSelFpf3, 3, f3_offset + 24, 8);

    /* F3_4 ? */

    /* F3_5 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTHopCount,
                 _bcmFieldSliceSelFpf3, 5, f3_offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanId,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 8, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVsanPri,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 20, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFibreChanVFTVersion,
                 _bcmFieldSliceSelFpf3, 5, f3_offset + 23, 2);

    /* DWF3 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 0, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 8, 8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 16,
                            6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 22,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 22,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f3_offset + 24,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf3, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f3_offset + 24,
                            8);

    /* DWF2_0 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 8,
                            6);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 14,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 14,
                            2);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTos,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 24,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 24,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 40,
                            16);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 56,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 56,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 64,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 64,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset + 96,
                            32);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                            _bcmFieldSliceSelFpf2, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset + 96,
                            32);

    /* DWF2_1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInnerSrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 1, f2_offset,
                            128);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                            _bcmFieldSliceSelFpf2, 1,
                            _bcmFieldSliceIpHeaderSelect, 0, f2_offset,
                            128);

    /* DWF2_2 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                            _bcmFieldSliceSelFpf2, 2,
                            _bcmFieldSliceSelDisable, 0,
                            f2_offset, 128);

    /* DWF1 */
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                            bcmFieldQualifyExtensionHeaderSubCode,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 0,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 8,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 16,
                            8);
    _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                            _bcmFieldSliceSelFpf1, 0,
                            _bcmFieldSliceIpHeaderSelect, 0, f1_offset + 24,
                            16);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_th_egress_qualifiers_init
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
_field_th_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /* EFP_KEY1(IPv4 Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 33, 16);    
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 57, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 65, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 97, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 129, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 137, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 139, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 151, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 159, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 161, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 173, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 174, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 177, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 179, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 180, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 188, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 196, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1,
                               _bcmFieldSliceSelEgrClassF1, 0,
                               201, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 214, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 214, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 214, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 230, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 232, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    /* EFP_KEY2 (IPv6 Singlewide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 1, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_DIP6,
                               9, 128);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               9, 64);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceIp6AddrSelect,
                               _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP_DIP_64,
                               73, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 145, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 147, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 159, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 167, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 168, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 181, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 182, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 185, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 187, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 188, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 12
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 3,
                               196, 8
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2,
                               _bcmFieldSliceSelEgrClassF2, 0,
                               196, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 209, 5);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 214, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 230, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 232, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    /* EFP_KEY3 (IPv6 Double Wide Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 1, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 5, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 7, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 9, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 17, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIcmpTypeCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 33, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 49, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 57, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderSubCode,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 185, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyExtensionHeaderType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 193, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 201, 5);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               206, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               206, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 218, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 220, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    /* EFP_KEY4(L2 Key) Qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 1);
    /* EG_NEXT_HOP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 1,
                               2, 12);
    /* EG_L3_INTF_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, 
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 2,
                               2, 12);
    /* EG_DVP_CLASS_ID */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassWlan,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 3,
                               2, 12);

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               1, 17);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 22, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 24, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 26, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 42, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 90, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 138, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 139, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 140, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 141, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 142, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 145, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 157, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 167, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 179, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 180, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 183, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 185, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 186, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               194, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     194, 6, /* CpuCos - 6 bits */
                     0, 0,  
                     0, 0,
                     0
                     );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyIngressClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               194, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 207, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 13);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 212, 13);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 226, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 228, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 230, 1);

    /*EFP_KEY6 (HiGiG Key)  Qualifiers*/
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyFabricQueueTag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 0, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 160, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 161, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 162, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 170, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 178, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );			   
    						   
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               186, 13
                               );
							   
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               186, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               186, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 199, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY6, 201, 1);

    /* EFP_KEY7(Loopback Key) qualifiers */

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 128, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 129, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 130, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 138, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 146, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 154, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 158, 2);
			 
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );			   
    						   
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               160, 13
                               );
							   
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               160, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               160, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 173, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY7, 175, 1);

    /* EFP_KEY8(Bytes After L2 Key) Qualifiers */

    /* DVP_VALID + DVP + SPARE_DVP */
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMplsGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstMimGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               0, 16);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstVxlanGport,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrDvpKey4, 0,
                               0, 16);  
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClass,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 0,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyEgressClassL3Interface,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 1,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassTrill,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyEgressClassL2Gre,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF3, 2,
                               1, 12);					   
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntPriority,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 17, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyColor,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 21, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 23, 16); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 39, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 40, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 41, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 49, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 50, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 51, 8);
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 1,
                               203, 8);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyCpuQueue,
                     _bcmFieldDevSelDisable, 0,
                     _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                     _bcmFieldSliceSelEgrClassF4, 2,
                     0,
                     203, 6, /* CpuCos - 6 bits */
                     0, 0,
                     0, 0,
                     0
                     );			 
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifySrcClassField,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyDstClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL2,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassL3,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );			   
    						   
    _FP_QUAL_TWO_SLICE_SEL_ADD(unit, stage_fc,
                               bcmFieldQualifyIngressInterfaceClassPort,
                               _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8,
                               _bcmFieldSliceSelEgrClassF4, 0,
                               203, 13
                               );
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 216, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 216, 14);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVpn,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 216, 14);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 230, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIntCongestionNotification,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 232, 2); 
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY8, 234, 1);



    return (BCM_E_NONE);
}
 /*
  * Function:
 *     _bcm_field_th_egress_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_th_mode_install that sets the mode of a
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
_bcm_field_th_egress_mode_set(int unit, uint8 slice_numb,
                               _field_group_t *fg, uint8 flags)
{
    uint32 mode_val[2];

    /* Input parameters check. */
    if ((NULL == fg) || (slice_numb >= COUNTOF(_th_efp_slice_mode))) {
        return (BCM_E_PARAM);
    }

    mode_val[1]  = _BCM_FIELD_EGRESS_SLICE_V6_KEY_MODE_SIP6;

    if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {

        /* DstIP6 + SrcIp6 or IPv4 + L2 Double wide key. */
        if (((_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) && 
            (_BCM_FIELD_EFP_KEY3 == fg->sel_codes[1].fpf3))) {
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
            if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
            }
        }

        if (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[1].fpf3) {   
            if ((_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3) && 
                (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4))) {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
            } else if (_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6;
                 if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].ip6_addr_sel) {
                     mode_val[1] =fg->sel_codes[0].ip6_addr_sel;
                 }
            } else if (_BCM_FIELD_EFP_KEY8 == fg->sel_codes[0].fpf3) {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE;
            } else {
                 mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY;
            }
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY, 2,
                                    _th_efp_slice_mode[slice_numb], mode_val));
    } else {
        if (_BCM_FIELD_EFP_KEY4 == fg->sel_codes[0].fpf3) {
            /* L2 - Slice mode. */
            mode_val[0] = _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2;

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
                                    _th_efp_slice_mode[slice_numb], mode_val));

    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_mode_set
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
_bcm_field_th_mode_set(int unit, uint8 slice_numb, 
                       _field_group_t *fg, uint8 flags)
{
    int rv;     /* Operation return status. */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = BCM_E_NONE; /* Mode and select codes programmed together. */
          break;
      case _BCM_FIELD_STAGE_LOOKUP:
          rv  = _bcm_field_td2_lookup_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _bcm_field_th_egress_mode_set(unit, slice_numb, fg, flags);
          break;
      default:
          rv = BCM_E_PARAM;
    }
    return (rv);
}
STATIC int
_field_th_egress_selcodes_install(int            unit,
                                   _field_group_t *fg,
                                   uint8          slice_num,
                                   bcm_pbmp_t     *pbmp,
                                   int            selcode_idx
                                   )
{
    static const soc_field_t fldtbl[][4] = {
        { SLICE_0_F1f, SLICE_1_F1f, SLICE_2_F1f, SLICE_3_F1f },
        { SLICE_0_F2f, SLICE_1_F2f, SLICE_2_F2f, SLICE_3_F2f },
        { SLICE_0_F4f, SLICE_1_F4f, SLICE_2_F4f, SLICE_3_F4f }
    };

    static const soc_field_t dvpfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

    static const soc_field_t mdlfldtbl[4] = {
        SLICE_0f, SLICE_1f, SLICE_2f, SLICE_3f };

    _field_sel_t * const sel = &fg->sel_codes[selcode_idx];

    if (sel->egr_class_f1_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[0][slice_num],
                                                   sel->egr_class_f1_sel
                                                   )
                            );
    }
    if (sel->egr_class_f2_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[1][slice_num],
                                                   sel->egr_class_f2_sel
                                                   )
                            );
    }
    if (sel->egr_class_f4_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_CLASSID_SELECTORr,
                                                   REG_PORT_ANY,
                                                   fldtbl[2][slice_num],
                                                   sel->egr_class_f4_sel
                                                   )
                            );
    }
    /* SELECTOR CODES for EFP_KEY4_DVP_SELECTOR register */
    if (sel->egr_key4_dvp_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_DVP_SELECTORr,
                                                   REG_PORT_ANY,
                                                   dvpfldtbl[slice_num],
                                                   sel->egr_key4_dvp_sel
                                                  )
                            );
    }
    /* SELECTOR CODES for EFP_KEY4_MDL_SELECTOR register */
    if (sel->egr_key4_mdl_sel != _FP_SELCODE_DONT_CARE) {
        BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                                   EFP_KEY4_MDL_SELECTORr,
                                                   REG_PORT_ANY,
                                                   mdlfldtbl[slice_num],
                                                   sel->egr_key4_mdl_sel
                                                  )
                            );
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_selcodes_install
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
_bcm_field_th_selcodes_install(int unit, _field_group_t *fg,
                                uint8 slice_numb, bcm_pbmp_t pbmp,
                                int selcode_index)
{
    int rv;    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Set slice mode. Single/Double/Triple, Intraslice */
    rv = _bcm_field_th_mode_set(unit, slice_numb, fg, fg->flags);
    BCM_IF_ERROR_RETURN(rv);
    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _bcm_field_td2_lookup_selcodes_install(unit, fg, slice_numb,
                                                      selcode_index);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
            rv = _field_th_egress_selcodes_install(unit, fg, slice_numb,
                                                    &pbmp, selcode_index);;
            break;
      case _BCM_FIELD_STAGE_EXTERNAL:
      case _BCM_FIELD_STAGE_INGRESS:
          rv = (BCM_E_NONE);
          break;
      default:
          rv = (BCM_E_PARAM);
    }
    return (rv);
}
/*
 * Function:
 *     _field_th_stage_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage    - (IN) Field processor stage.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)              
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_stage_action_support_check(int unit,
                                     unsigned stage,
                                     bcm_field_action_t action,
                                     int *result)
{
    switch (action) {
        case bcmFieldActionHiGigDstModuleGportNew:
        case bcmFieldActionHiGigDstPortGportNew:
        case bcmFieldActionHiGigDstGportNew:
        case bcmFieldActionGpHiGigDropPrecedenceNew:
        case bcmFieldActionYpHiGigDropPrecedenceNew:
        case bcmFieldActionRpHiGigDropPrecedenceNew:
        case bcmFieldActionHiGigDropPrecedenceNew:
        case bcmFieldActionGpHiGigIntPriNew:
        case bcmFieldActionYpHiGigIntPriNew:
        case bcmFieldActionRpHiGigIntPriNew:
        case bcmFieldActionHiGigIntPriNew:
        case bcmFieldActionLoopbackSrcModuleGportNew:
        case bcmFieldActionLoopbackSrcPortGportNew:
        case bcmFieldActionLoopbackSrcGportNew:
        case bcmFieldActionLoopbackCpuMasqueradePktProfileNew:
        case bcmFieldActionLoopbackPacketProcessingPortNew:
        case bcmFieldActionLoopBackTypeNew:
            *result = (stage == _BCM_FIELD_STAGE_EGRESS);
            return (BCM_E_NONE);
        default:
        ;
    }

    return (_bcm_field_td2_stage_action_support_check(unit, stage, 
                                                      action, result));
}
/*
 * Function:
 *     _bcm_field_th_stage_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage    - (IN) Field processor stage.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)              
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_stage_action_support_check(int unit,
                                         unsigned stage,
                                         bcm_field_action_t action,
                                         int *result)
{
    return _field_th_stage_action_support_check(unit, stage, action, result);
}
/*
 * Function:
 *     _bcm_field_th_action_support_check
 * Purpose:
 *     Check if action is supported by tomahawk for the given stage.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     f_ent    - (IN) Field entry structure to get policy info from.
 *     action   - (IN) Action to check(bcmFieldActionXXX)
 *     result   - (OUT)              
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_action_support_check(int unit,
                                   _field_entry_t *f_ent,
                                   bcm_field_action_t action,
                                   int *result)
{
    return _bcm_field_th_stage_action_support_check(unit, 
                                                    f_ent->group->stage_id, 
                                                    action, result);
}
/*
 * Function:
 *     _field_th_action_params_check
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
_field_th_action_params_check(int unit,
                              _field_entry_t  *f_ent,
                              _field_action_t *fa)
{
    uint8    mod_id;     /* Module Id.              */
    uint8    port_id;    /* Port Id.                */
    soc_mem_t mem;       /* Policy table memory id. */
    soc_mem_t tcam_mem;  /* Tcam memory id.         */

    if (NULL == f_ent ||
        NULL == fa) {
        return BCM_E_PARAM;
    }

    if (_BCM_FIELD_STAGE_EXTERNAL == f_ent->group->stage_id) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(_field_fb_tcam_policy_mem_get(unit,
           f_ent->group->stage_id, &tcam_mem, &mem));

    switch (f_ent->group->stage_id) {
        case _BCM_FIELD_STAGE_EGRESS:
            switch (fa->action) {
                case bcmFieldActionHiGigDstModuleGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, HG_DST_MODID_NEWf, mod_id);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigDstPortGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, HG_DST_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigDstGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, HG_DST_MODID_NEWf, mod_id);
                    PolicyCheck(unit, mem, HG_DST_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionGpHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, G_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);                  
                case bcmFieldActionYpHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, Y_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionRpHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, R_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigDropPrecedenceNew:
                    PolicyCheck(unit, mem, G_HG_DP_NEWf, fa->param[0]);
                    PolicyCheck(unit, mem, Y_HG_DP_NEWf, fa->param[0]);
                    PolicyCheck(unit, mem, R_HG_DP_NEWf, fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionGpHiGigIntPriNew:
                    PolicyCheck(unit, mem, G_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionYpHiGigIntPriNew:
                    PolicyCheck(unit, mem, Y_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionRpHiGigIntPriNew:
                    PolicyCheck(unit, mem, R_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionHiGigIntPriNew:
                    PolicyCheck(unit, mem, G_HG_INT_PRI_NEWf , fa->param[0]);
                    PolicyCheck(unit, mem, Y_HG_INT_PRI_NEWf , fa->param[0]);
                    PolicyCheck(unit, mem, R_HG_INT_PRI_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackSrcModuleGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, LB_SRC_MODID_NEWf, mod_id);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackSrcPortGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, LB_SRC_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackSrcGportNew:
                    if (BCM_GPORT_IS_MODPORT(fa->param[0])) {
                        mod_id = BCM_GPORT_MODPORT_MODID_GET(fa->param[0]);
                        port_id = BCM_GPORT_MODPORT_PORT_GET(fa->param[0]);
                    } else {
                        return BCM_E_PARAM;
                    }
                    PolicyCheck(unit, mem, LB_SRC_MODID_NEWf, mod_id);
                    PolicyCheck(unit, mem, LB_SRC_PORT_NEWf, port_id);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackCpuMasqueradePktProfileNew:
                    PolicyCheck(unit, mem, 
                                LB_PACKET_PROFILE_NEWf, 
                                fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopbackPacketProcessingPortNew:
                    PolicyCheck(unit, mem, LB_PP_PORT_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                case bcmFieldActionLoopBackTypeNew:
                    PolicyCheck(unit, mem, LB_TYPE_NEWf , fa->param[0]);
                    return (BCM_E_NONE);
                default:
                    ;
            }
            break;
        default:
            ;
    }

    return _bcm_field_td2_action_params_check(unit, f_ent, fa);
}
/*
 * Function:
 *     _bcm_field_th_action_params_check
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
_bcm_field_th_action_params_check(int unit,
                                  _field_entry_t *f_ent,
                                  _field_action_t *fa)
{
    return _field_th_action_params_check(unit, f_ent, fa);
}

/*
 * Function:
 *     _field_th_qualifiers_init
 * Purpose:
 *     Initialize device qaualifiers select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 * 
 * Returns:
 *     BCM_E_NONE
 * Notes:
 * 
 */
STATIC int
_field_th_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr,
                   (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)),
                   "Field qualifiers");
    if (stage_fc->f_qual_arr == 0) {
        return (BCM_E_MEMORY);
    }

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_LOOKUP:
            return (_field_th_lookup_qualifiers_init(unit, stage_fc));
        case _BCM_FIELD_STAGE_EGRESS:
            return (_field_th_egress_qualifiers_init(unit, stage_fc));
        default:
            ;
    }

    sal_free(stage_fc->f_qual_arr);
    return (BCM_E_INTERNAL);
}
/*
 * Function:
 *     _bcm_field_th_egress_key_match_type_set
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
_bcm_field_th_egress_key_match_type_set (int unit, _field_entry_t *f_ent)
{
    _field_group_t    *fg;          /* Field group entry belongs to. */
    uint32            data;         /* Key match type.               */
    uint32            default_data; /* default key match type.       */
    uint32            mask;         /* Key match type mask.          */
    int               rv;           /* Operation return status.      */

    /* Key match type offset.        */
    _bcm_field_qual_offset_t q_offset = {KEYf, 236, 4, 0, 0, 0, 0};

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
                data = KEY_TYPE_IPv4_SINGLE; 
                default_data = KEY_TYPE_L2_SINGLE;
                break;
            case _BCM_FIELD_EFP_KEY2:
                data = KEY_TYPE_IPv6_SINGLE;
                default_data = KEY_TYPE_L2_SINGLE;
                break;
            case _BCM_FIELD_EFP_KEY4:
                data = KEY_TYPE_L2_SINGLE;
                default_data = KEY_TYPE_L2_SINGLE;
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    } else {
        switch (fg->sel_codes[1].fpf3) {
            case _BCM_FIELD_EFP_KEY3:
                data = KEY_TYPE_IPv6_DOUBLE;
                default_data = KEY_TYPE_L2_DOUBLE;
                break;
            case _BCM_FIELD_EFP_KEY4:
                if (_BCM_FIELD_EFP_KEY1 == fg->sel_codes[0].fpf3) {
                    if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)){
                        default_data = KEY_TYPE_L2_DOUBLE;
                    } else {
                        default_data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                    }
                    data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY2 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_IPv4_IPv6_DOUBLE;
                    default_data = KEY_TYPE_L2_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY6 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_HIGIG_DOUBLE;
                    default_data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY7 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_LOOPBACK_DOUBLE;
                    default_data = KEY_TYPE_IPv4_L2_L3_DOUBLE;
                } else if (_BCM_FIELD_EFP_KEY8 == fg->sel_codes[0].fpf3) {
                    data = KEY_TYPE_L2_DOUBLE;
                    default_data = KEY_TYPE_L2_DOUBLE; 
                }   
                break;
            default:
                return (BCM_E_INTERNAL);
        }
    }

    mask = 0xf;

   /*
    * COVERITY 
    * 
    *This flow takes care of the  Out-of-bounds access issue  
    * for data and mask.
    */
    /* coverity[callee_ptr_arith : FALSE] */
    rv = _bcm_field_qual_value_set_by_entry_type(unit, &q_offset, f_ent,
                                                 &data, &mask, 
                                                 _FP_ENTRY_TYPE_0);
    BCM_IF_ERROR_RETURN(rv);

   /*
    * COVERITY 
    * 
    * This flow takes care of the  Out-of-bounds access issue  
    * for data and mask.
    */

    /* coverity[callee_ptr_arith : FALSE] */

    rv = _bcm_field_qual_value_set_by_entry_type(unit, &q_offset, f_ent,
                                                 &default_data, &mask,
                                                 _FP_ENTRY_TYPE_1);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}
/*
 * Function:
 *     _bcm_field_th_egress_selcode_get
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
_bcm_field_th_egress_selcode_get(int unit, _field_stage_t *stage_fc,
                               bcm_field_qset_t *qset_req,
                               _field_group_t *fg)
{
    int             rv;          /* Operation return status. */
    bcm_field_qset_t new_qset;

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    if  (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        /* Attempt _BCM_FIELD_EFP_KEY4  (L2 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY4, 0, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY4, -1, &new_qset);
            BCM_IF_ERROR_RETURN(rv);

            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY4 (Any Packet type) */

            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY1  (IPv4 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY1, 0, fg);

        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY4, -1, &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY4 (Any Packet type) */

            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY2  (IPv6 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY2, 0, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY4, -1, &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY2;
            return rv;
        }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }
    } else {

        if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
            /* L3 + L2 double wide key for IPv4 packets */
            rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                               _BCM_FIELD_EFP_KEY1,
                                               _BCM_FIELD_EFP_KEY4, fg);
            if (BCM_SUCCESS(rv)) {

                rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                             _BCM_FIELD_EFP_KEY8,
                                             _BCM_FIELD_EFP_KEY4,
                                             &new_qset);
                BCM_IF_ERROR_RETURN(rv);


                /* In EFP, for a given qset we can have two selector code
                   combination. So attempt _BCM_FIELD_EFP_KEY8 and
                   _BCM_FIELD_EFP_KEY4 (Any Packet type) */


                rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                               &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                              fg);
                if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                   (rv != BCM_E_RESOURCE))) {
                    fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                    fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                    return rv;
                }
            } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
                return rv;
            }
        }

        /* L2 + L3 double wide predefined key. */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY1,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY1,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY1 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY1,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* HiGiG Doublewide key */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY6,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY1,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY1 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY1,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY6;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Loopback double wide key */

        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY7,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY1,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY1 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY1,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY7;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* L3 + L2 double wide key for IPv4 packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY1,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY1;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }    

        /* L3 + L2 double wide key for IPv6 packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY2,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY2;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* L3 key for IPv6 packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY2,
                                           _BCM_FIELD_EFP_KEY3, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY2;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY3;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }

        /* Bytes after L2 Double key for non IP packets */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req,
                                           _BCM_FIELD_EFP_KEY8,
                                           _BCM_FIELD_EFP_KEY4, fg);
        if (BCM_SUCCESS(rv)) {

            rv = _bcm_field_trx_qset_get(qset_req, stage_fc,
                                         _BCM_FIELD_EFP_KEY8,
                                         _BCM_FIELD_EFP_KEY4,
                                         &new_qset);
            BCM_IF_ERROR_RETURN(rv);


            /* In EFP, for a given qset we can have two selector code
               combination. So attempt _BCM_FIELD_EFP_KEY8 and
               _BCM_FIELD_EFP_KEY4 (Any Packet type) */


            rv = _bcm_field_trx_egress_default_selcode_get(unit, stage_fc,
                                                           &new_qset,
                                                           _BCM_FIELD_EFP_KEY8,
                                                           _BCM_FIELD_EFP_KEY4,
                                                           fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) &&
                                    (rv != BCM_E_RESOURCE))) {
                fg->sel_codes[0].fpf3= _BCM_FIELD_EFP_KEY8;
                fg->sel_codes[1].fpf3= _BCM_FIELD_EFP_KEY4;
                return rv;
            }
        } else if(BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE)) {
            return rv;
        }
    }

    return BCM_E_RESOURCE;
}
/*
 * Function:
 *     _bcm_field_th_selcode_get
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
_bcm_field_th_selcode_get(int unit, _field_stage_t *stage_fc,
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
      case _BCM_FIELD_STAGE_EGRESS:
          rv =  _bcm_field_th_egress_selcode_get(unit, stage_fc, qset_req, fg);
          break;
      default:
          rv =  _bcm_field_selcode_get(unit, stage_fc, qset_req, fg);
    }
    return (rv);
}

/*
 * Function:
 *    _field_th_group_install
 *
 * Purpose:
 *    Auxiliary routine used to install field group.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fg         - (IN) Installed group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_group_install(int unit, _field_group_t *fg)
{
    _field_slice_t *fs;        /* Slice pointer.           */
    uint8  slice_number;       /* Slices iterator.         */
    int    parts_count;        /* Number of entry parts.   */
    int    idx;                /* Iteration index.         */

    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    BCM_IF_ERROR_RETURN(_bcm_field_entry_tcam_parts_count(unit,
                                                          fg->flags,
                                                          &parts_count
                                                          )
                        );

    for (idx = 0; idx < parts_count; ++idx) {
        BCM_IF_ERROR_RETURN(_bcm_field_tcam_part_to_slice_number(idx,
                                                                 fg->flags,
                                                                 &slice_number
                                                                 )
                            );
        fs = fg->slices + slice_number;

        BCM_IF_ERROR_RETURN(_bcm_field_th_selcodes_install(unit,
                                                           fg,
                                                           fs->slice_number,
                                                           fg->pbmp,
                                                           idx)
                            );
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_group_install
 *
 * Purpose:
 *    Auxiliary routine used to install field group.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fg         - (IN) Installed group structure.
 *
 * Returns:
 *     BCM_E_XXX
 */

int 
_bcm_field_th_group_install(int unit, _field_group_t *fg)
{   
  return _field_th_group_install(unit, fg);
}  
           /* START OF COUNTER ROUTINES */

/*
 * Function:
 *     _bcm_field_th_multi_pipe_counter_mem_get
 *
 * Purpose:
 *     To get the pipe specific counter memory names for a given 
 *     Field Processor stage(VFP/IFP/EFP).
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     stage_fc     - (IN)  Field Processor stage control structure.
 *     counter_mem  - (OUT) pointer to array of counter memories.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_counter_mem_get(int unit,
                                         _field_stage_t *stage_fc,
                                         soc_mem_t *counter_mem)
{
    int              pipe;          /* XGS pipeline to operate on.        */
    int              max_pipes;     /* Maximum XGS pipelines in the stage */

    /* Input parameters check. */
    if ((NULL == stage_fc) ||
        (NULL == counter_mem)) {
        return BCM_E_PARAM;
    }

    max_pipes = stage_fc->pipes;
    for (pipe = 0; pipe < max_pipes; pipe++) {
        counter_mem[pipe] = INVALIDm;
    }

    switch(stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_EGRESS:
            counter_mem[0] = EFP_COUNTER_TABLE_PIPE0m;
            counter_mem[1] = EFP_COUNTER_TABLE_PIPE1m;
            counter_mem[2] = EFP_COUNTER_TABLE_PIPE2m;
            counter_mem[3] = EFP_COUNTER_TABLE_PIPE3m;
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            break;
        default:
            return BCM_E_INTERNAL; 
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _field_th_counter_write
 * Purpose:
 *     Write accumulated sw & hw counters
 * Parameters:
 *   unit          - (IN) BCM device number
 *   mem           - (IN) FieldProcessor(FP) Counter Memory name 
 *   buf           - (IN) Memory table entry for FP counter
 *   cntrs32_buf   - (IN) s/w packet Counter value
 *   cntrs64_buf   - (IN) s/w byte Counter value   
 *   packet_count  - (IN) Packet counter value
 *   byte_count    - (IN) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */

STATIC int
_field_th_counter_write(int unit, soc_mem_t mem, int idx, uint32 *buf,
                            _field_counter32_collect_t *cntrs32_buf,
                            _field_counter64_collect_t *cntrs64_buf,
                            uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/
    uint32  len;           /* field length */

    if (NULL == buf) {
        return BCM_E_PARAM;
    }

    /* Set byte count in the memory */
    if (NULL != byte_count) {
        /* For Tomahawk field_length = 34 */
        len = soc_mem_field_length(unit, mem, BYTE_COUNTERf);

        if (NULL == cntrs64_buf) {
            return BCM_E_PARAM;
        }

        COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
        COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*byte_count));
        COMPILER_64_SET(cntrs64_buf->last_hw_value,
                        COMPILER_64_HI(*byte_count) & ((1 << (len - 32)) - 1),
                        COMPILER_64_LO(*byte_count));
        hw_val[0] = COMPILER_64_LO(cntrs64_buf->last_hw_value);
        hw_val[1] = COMPILER_64_HI(cntrs64_buf->last_hw_value);
        soc_mem_field_set(unit, mem, buf, BYTE_COUNTERf, hw_val);
    }

    /* Set packet count in the memory */
    if (NULL != packet_count) {
        /* For Tomahawk field_length = 26 */
        len = soc_mem_field_length(unit, mem, PACKET_COUNTERf);

        if (NULL == cntrs32_buf) {
            return BCM_E_PARAM;
        }

        COMPILER_64_ZERO(cntrs32_buf->accumulated_counter);
        COMPILER_64_OR(cntrs32_buf->accumulated_counter,
                       (*packet_count));
        cntrs32_buf->last_hw_value =
            (COMPILER_64_LO(*packet_count) & ((1 << len) - 1));
        hw_val[0] = cntrs32_buf->last_hw_value;
        hw_val[1] = 0;
        soc_mem_field_set(unit, mem, buf, PACKET_COUNTERf, hw_val);
    }

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, buf);
}
/*
 * Function:
 *    _bcm_field_th_pipe_counter_set
 *
 * Description:
 *   Service routine used to set 64-bit software accumulated counters
 *   of a field processor counter for a given pipe
 *   (Ingress/Lookup/Egress/External).
 * Parameters:
 *   unit         - (IN) BCM device number. 
 *   stage_fc     - (IN) Stage field control structure.
 *   counter_mam  - (IN) FieldProcessor(FP) counter name.
 *   mem_buf      - (IN) Memory table entry for FP counter 
 *   idx          - (IN) Counter hw index. 
 *   pipe         - (IN) XGS pipeline to operate on.
 *   packet_count - (IN) Packet Counter value
 *   byte_count   - (IN) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_pipe_counter_set(int unit, 
                            _field_stage_t *stage_fc,
                            soc_mem_t counter_mem, 
                            uint32 *mem_buf,
                            int idx, int pipe, 
                            uint64 *packet_count, 
                            uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */

    /* Input parameters check. */
    if ((NULL == stage_fc) ||
        (NULL == mem_buf)) {
        return BCM_E_PARAM;
    }

    if (INVALIDm != counter_mem) {
        if (NULL != mem_buf) {
            cntrs64_buf = &stage_fc->_field_64_counters[pipe][idx];
            cntrs32_buf = &stage_fc->_field_32_counters[pipe][idx];
            BCM_IF_ERROR_RETURN
                (_field_th_counter_write(unit, counter_mem,
                                             idx, mem_buf,
                                             cntrs32_buf, cntrs64_buf,
                                             packet_count, byte_count));
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *    _field_th_multi_pipe_sw_counter_set
 *
 * Description:
 *   Service routine used to set 64-bit software accumulated counters.
 * Parameters:
 *   unit            - (IN) BCM device number. 
 *   stage_fc        - (IN) Stage field control structure.
 *   stat_group_pipe - (IN) XGS Pipline associated to the field group.
 *   idx             - (IN) Counter hw index.
 *   packet_count    - (IN) Packet Counter value
 *   byte_count      - (IN) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 *
 */
STATIC int
_field_th_multi_pipe_sw_counter_set(int unit, _field_stage_t *stage_fc,
                                 int stat_group_pipe, int idx, 
                                 uint64 *packet_count,
                                 uint64 *byte_count)
{
    int              rv;            /* Operation return status.           */
    int              pipe;          /* XGS pipeline to operate on.        */
    int              max_pipes;     /* Maximum XGS pipelines in the stage */
    uint32           *hw_buffer;    /* memory HW buffer.                  */
    uint32           *hw_buf;       /* memory HW buffer.                  */
    _field_control_t *fc;           /* Field control structure.           */
    soc_mem_t        counter_mem[_FIELD_MAX_NUM_PIPES]; /* FieldProcessor
                                                           (IFP/EFP/VFP) 
                                                           counter memories */

    hw_buffer = NULL;

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get counter memory. */
    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc, 
                                                     counter_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* HW index sanity check. */
    if (idx < soc_mem_index_min(unit, counter_mem[0]) ||
        idx > soc_mem_index_max(unit, counter_mem[0])) {
        return (BCM_E_INTERNAL);
    }

    max_pipes = stage_fc->pipes;
    if (_BCM_FIELD_GLOBAL_GROUP != stat_group_pipe) {
        for (pipe = 0; pipe < max_pipes; pipe++ ) {
             if (pipe != stat_group_pipe) {
                 counter_mem[pipe] = INVALIDm;
             }
        }
    }

    /* Allocate buffer for counter values in HW.  */
    _FP_XGS3_ALLOC(hw_buffer, 
                   _FIELD_MAX_NUM_PIPES * WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS),
                   "Counter buffers");
    if (NULL == hw_buffer) {
        return (BCM_E_MEMORY);
    }

    for (pipe = 0; pipe < max_pipes; pipe++) {
        if (INVALIDm != counter_mem[pipe]) {
            hw_buf = hw_buffer + (pipe * SOC_MAX_MEM_FIELD_WORDS);
            rv = soc_mem_read(unit, 
                              counter_mem[pipe], 
                              MEM_BLOCK_ANY, 
                              idx, hw_buf);
            if (BCM_FAILURE(rv)) {
                sal_free(hw_buffer);
                return rv;
            }

            rv = _bcm_field_th_pipe_counter_set(unit, stage_fc, 
                                             counter_mem[pipe],
                                             hw_buf, idx, pipe, 
                                             packet_count, byte_count);
            if (BCM_FAILURE(rv)) {
                sal_free(hw_buffer);
                return rv;
            }

            /* Write 0 to other pipeline counter value. */
            if (NULL != packet_count) {
                COMPILER_64_ZERO(*packet_count);
            }
            if (NULL != byte_count) {
                COMPILER_64_ZERO(*byte_count);
            }

        }
    }
    
    sal_free(hw_buffer);

    return rv;
}
/*
 * Function:
 *     _bcm_field_th_counter_read
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   stage_fc      - (IN)  Stage field control
 *   mamacc_byte   - (IN)  Memory access cache for byte counters
 *   memacc_pkt    - (IN)  Memory access cache for packet counters
 *   buf           - (IN)  Memory table entry for FP counter
 *   cntrs32_buf   - (IN)  s/w packet Counter value
 *   cntrs64_buf   - (IN)  s/w byte Counter value   
 *   packet_count  - (OUT) Packet counter value
 *   byte_count    - (OUT) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_counter_read(int unit, _field_stage_t *stage_fc,
                        soc_memacc_t *memacc_byte,
                        soc_memacc_t *memacc_pkt, uint32 *buf,
                        _field_counter32_collect_t *cntrs32_buf,
                        _field_counter64_collect_t *cntrs64_buf,
                        uint64 *packet_count, uint64 *byte_count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    if (NULL == buf ||
        NULL == memacc_byte ||
        NULL == memacc_pkt ||
        NULL == cntrs32_buf ||
        NULL == cntrs64_buf) {
        return BCM_E_PARAM;
    }

    /* Byte counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc_byte, buf, hw_val);

    _bcm_field_34bit_counter_update(unit, hw_val, cntrs64_buf);
    if (NULL != byte_count) {
        COMPILER_64_ADD_64(*byte_count, cntrs64_buf->accumulated_counter);
    }
    /* Packet counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc_pkt, buf, hw_val);
    _bcm_field_26bit_counter_update(unit, hw_val, cntrs32_buf);
    if (NULL != packet_count) {
        COMPILER_64_ADD_64(*packet_count, cntrs32_buf->accumulated_counter);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_field_th_pipe_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number
 *   stage_fc      - (IN)  Stage field control
 *   counter_mem   - (IN)  Memory name for FP counter
 *   mem_buf       - (IN)  Memory table entry for FP counter
 *   idx           - (IN)  Counter index
 *   pipe          - (IN)  XGS pipeline to operate on 
 *   packet_count  - (OUT) Packet counter value
 *   byte_count    - (OUT) Byte counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_th_pipe_counter_get(int unit, 
                            _field_stage_t *stage_fc,
                            soc_mem_t counter_mem, 
                            uint32 *mem_buf,
                            int idx, 
                            int pipe,
                            uint64 *packet_count, 
                            uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    _field_counter64_collect_t *cntrs64_buf;  /* Sw byte counter value      */
    soc_memacc_t *memacc_pkt, *memacc_byte;   /* Memory access cache.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return BCM_E_PARAM;
    }

    cntrs64_buf = &stage_fc->_field_64_counters[pipe][idx];
    cntrs32_buf = &stage_fc->_field_32_counters[pipe][idx];
    memacc_byte = &(stage_fc->_field_memacc_counters[(pipe * 2)]);
    memacc_pkt  = &(stage_fc->_field_memacc_counters[((pipe * 2) + 1)]);
    BCM_IF_ERROR_RETURN(_bcm_field_th_counter_read(unit, stage_fc, 
                                                   memacc_byte, 
                                                   memacc_pkt,
                                                   mem_buf,
                                                   cntrs32_buf, cntrs64_buf,
                                                   packet_count, byte_count));

    return BCM_E_NONE;
}
/*
 * Function:
 *    _field_th_multi_pipe_sw_counter_get
 *
 * Description:
 *       Service routine used to retrieve 64-bit software accumulated counters.
 * Parameters:
 *   unit         - (IN) BCM device number. 
 *   stage_fc     - (IN) Stage field control structure.
 *   stat_group_pipe - (IN) XGS Pipline associated to the field group.
 *   idx          - (IN) Counter hw index. 
 *   packet_count - (OUT) Packet Counter value
 *   byte_count   - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_th_multi_pipe_sw_counter_get(int unit, 
                                 _field_stage_t *stage_fc,
                                 int stat_group_pipe, 
                                 int idx, 
                                 uint64 *packet_count,
                                 uint64 *byte_count)
{
    int                       rv;              /* Operation return status.    */
    int                       pipe;            /* XGS pipeline to operate on. */
    int                       max_pipes;       /* Maximum pipeline per stage. */
    uint32                    *hw_buf;         /*  memory HW  buffer.         */
    uint32                    *hw_buffer;      /*  memory HW  buffer.         */
    _field_control_t          *fc;             /* Field control structure.    */
    _field_counter32_collect_t *cntrs32_buf;   /* Sw packet counter value     */
    _field_counter64_collect_t *cntrs64_buf;   /* Sw byte counter value       */
    soc_mem_t counter_mem[_FIELD_MAX_NUM_PIPES];  /* FieldProcessor
                                                     (IFP/EFP/VFP) 
                                                     counter memories */
 

    hw_buf = hw_buffer = NULL; 
  
    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get counter memory. */
    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc, 
                                                    counter_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* HW index sanity check. */
    if (idx < soc_mem_index_min(unit, counter_mem[0]) ||
        idx > soc_mem_index_max(unit, counter_mem[0])) {
        return (BCM_E_INTERNAL);
    }

    max_pipes = stage_fc->pipes;
    if (_BCM_FIELD_GLOBAL_GROUP != stat_group_pipe) {
        for (pipe = 0; pipe < max_pipes; pipe++ ) {
             if (pipe != stat_group_pipe) {
                 counter_mem[pipe] = INVALIDm; 
             } 
        }
    }
 
    if (0 == (fc->flags & _FP_STAT_SYNC_ENABLE)) {
        for (pipe = 0; pipe < max_pipes; pipe++) {
            if (INVALIDm != counter_mem[pipe]) {
                cntrs64_buf = &stage_fc->_field_64_counters[pipe][idx];
                COMPILER_64_ADD_64(*byte_count,
                                   cntrs64_buf->accumulated_counter);
                cntrs32_buf = &stage_fc->_field_32_counters[pipe][idx];
                COMPILER_64_ADD_64(*packet_count,
                                    cntrs32_buf->accumulated_counter);
            }
        }
    } else {
        /* Allocate buffer for counter values in HW.  */
        _FP_XGS3_ALLOC(hw_buffer, 
                     max_pipes * WORDS2BYTES(SOC_MAX_MEM_FIELD_WORDS),
                     "Counter buffers");
        if (NULL == hw_buffer) {
            return (BCM_E_MEMORY);
        }

        for (pipe = 0; pipe < max_pipes; pipe++) {
            if (INVALIDm != counter_mem[pipe]) {
                hw_buf = hw_buffer + (pipe * SOC_MAX_MEM_FIELD_WORDS);
                rv = soc_mem_read(unit, 
                                  counter_mem[pipe], 
                                  MEM_BLOCK_ANY, 
                                  idx, hw_buf);
                if (BCM_FAILURE(rv)) {
                    sal_free(hw_buffer);
                    return rv;
                }
                rv = _bcm_field_th_pipe_counter_get(unit, stage_fc,
                                                 counter_mem[pipe],
                                                 hw_buf, idx, pipe, 
                                                 packet_count, byte_count);
                if (BCM_FAILURE(rv)) {
                    sal_free(hw_buffer);
                    return rv;
                }

            }
        }

        sal_free(hw_buffer);

    }

    return rv;
}
/*
 * Function:
 *      _bcm_th_field_stat_value_set
 *
 * Description:
 *      Set 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (IN) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_field_stat_value_set(int unit, 
                             _field_stat_t *f_st, 
                             bcm_field_stat_t stat,
                             uint64 value)
{
    int                 rv;             /* Operation return status.          */ 
    int                 idx1;           /* First counter index to read.      */
    int                 idx2;           /* Second counter index to read.     */
    int                 idx3;           /* Third counter index to read.      */
    int                 len;            /* length of the h/w field.          */
    uint32              flags;          /* _FP_STAT_XXX.                     */
    uint64              hw_value;       /* h/w counter value to be written.  */
    uint64              reset_val;      /* Value to be set on reset          */
    _field_control_t    *fc;            /* Field control structure.          */
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t reg;

    /* Input parameters check. */
    if (NULL == f_st) {
        return (BCM_E_PARAM);
    }

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    if (_FP_INVALID_INDEX == idx1) {
        return (BCM_E_INTERNAL);
    }


    if (flags & _FP_STAT_BYTES) {
        reg = SOC_COUNTER_NON_DMA_EFP_BYTE;
        /* For Tomahawk BYTE_COUNTERf field_length = 34*/
        len = soc_mem_field_length(unit, EFP_COUNTER_TABLEm, BYTE_COUNTERf);
    } else {
        reg = SOC_COUNTER_NON_DMA_EFP_PKT;
        /* For Tomahawk field_length = 26 */
        len = soc_mem_field_length(unit, EFP_COUNTER_TABLEm, PACKET_COUNTERf);
    }

    if (len < 32) {
        COMPILER_64_SET(hw_value, 0, COMPILER_64_LO(value) & ((1 << len) - 1));
    } else {
        COMPILER_64_SET(hw_value, 
                        COMPILER_64_HI(value) & ((1 << (len - 32)) - 1),
                                                 COMPILER_64_LO(value));
    }

    
    ctrl_info.instance = -1;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PIPE;
    COMPILER_64_ZERO(reset_val);

    rv = soc_counter_generic_set(unit, reg, 
                                 ctrl_info, 0,
                                 idx1, reset_val);
    BCM_IF_ERROR_RETURN(rv);

    /* Reset accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx2) {
        rv = soc_counter_generic_set(unit, reg, 
                                     ctrl_info, 0, 
                                     idx2, reset_val);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (_FP_INVALID_INDEX != idx3) {
        rv = soc_counter_generic_set(unit, reg, 
                                     ctrl_info, 0, 
                                     idx3, reset_val);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set accumulated counter value at primary index. */
    ctrl_info.instance = 0;
    rv = soc_counter_generic_set(unit, reg,
                                 ctrl_info, 0,
                                 idx1, hw_value);
    BCM_IF_ERROR_RETURN(rv);
 
    return (rv);
}

/*
 * Function:
 *      _bcm_field_th_multi_pipe_stat_value_set
 *
 * Description:
 *      Set 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (IN) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_stat_value_set(int unit, 
                                 _field_stat_t *f_st, 
                                 bcm_field_stat_t stat,
                                 uint64 value)
{
    int                 rv;             /* Operation return status.          */ 
    int                 idx1;           /* First counter index to read.      */
    int                 idx2;           /* Second counter index to read.     */
    int                 idx3;           /* Third counter index to read.      */
    int                 stat_group_pipe; /* XGS Pipline associated 
                                            to the field group.              */
    uint32              flags;          /* _FP_STAT_XXX.                     */
    _field_stage_t      *stage_fc;      /* Stage field control.              */
    _field_control_t    *fc;            /* Field control structure.          */
    uint64              *byte_count;    /* Pointer to hold byte count value  */
    uint64              *packet_count;  /* Pointer to hold pkt counter value */

    /* Input parameters check. */
    if (NULL == f_st) {
        return (BCM_E_PARAM);
    }

    /* Get field control. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    stat_group_pipe = _BCM_FIELD_GLOBAL_GROUP;

    /* Set accumulated counter value at primary index. */
    if (_FP_INVALID_INDEX == idx1) {
        return (BCM_E_INTERNAL);
    }

    if (flags & _FP_STAT_BYTES) {
        packet_count = NULL;
        byte_count = &value;
    } else {
        byte_count = NULL;
        packet_count = &value;
    }

    rv = _field_th_multi_pipe_sw_counter_set(unit, stage_fc, 
                                             stat_group_pipe, 
                                             idx1, packet_count, 
                                             byte_count);
    BCM_IF_ERROR_RETURN(rv);

    COMPILER_64_ZERO(value);

    /* Reset accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx2) {
        rv = _field_th_multi_pipe_sw_counter_set(unit, stage_fc,
                                                 stat_group_pipe,
                                                 idx2, packet_count, 
                                                 byte_count);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (_FP_INVALID_INDEX != idx3) {
        rv = _field_th_multi_pipe_sw_counter_set(unit, stage_fc,
                                                 stat_group_pipe,
                                                 idx3, packet_count,
                                                 byte_count);
        BCM_IF_ERROR_RETURN(rv);
    }

    return (rv);
}
/*
 * Function:
 *      _bcm_th_field_stat_value_get
 *
 * Description:
 *      Get 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (OUT) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_th_field_stat_value_get(int unit, 
                             _field_stat_t *f_st, 
                             bcm_field_stat_t stat,
                             uint64 *value)
{
    int                 idx1;            /* First counter index to read.      */
    int                 idx2;            /* Second counter index to read.     */
    int                 idx3;            /* Third counter index to read.      */
    int                 rv;              /* Operation return status.          */
    uint32              flags;           /* _FP_STAT_XXX.                     */
    uint64              count_1;  /* Primary index packet count.       */
    uint64              count_2;    /* Primary index byte count.         */
    uint64              count_3;    /* Third counterindex byte count.    */
    _field_control_t    *fc;            /* Field control structure.          */
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t reg;           

    /* Initialization. */
    COMPILER_64_ZERO(count_1);
    COMPILER_64_ZERO(count_2);
    COMPILER_64_ZERO(count_3);
    idx1 = idx2 = idx3 = _FP_INVALID_INDEX;
    flags = 0;

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    ctrl_info.instance = -1;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PIPE;

    if (flags & _FP_STAT_BYTES) {
        reg = SOC_COUNTER_NON_DMA_EFP_BYTE;
    } else {
        reg = SOC_COUNTER_NON_DMA_EFP_PKT;
    }

    /* Get accumulated counter value at primary index. */
    if (_FP_INVALID_INDEX != idx1) {
        rv = soc_counter_generic_get (unit, reg,
                                      ctrl_info, 0, 
                                      idx1, &count_1);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx2) {
        rv = soc_counter_generic_get (unit, reg,
                                      ctrl_info, 0,
                                      idx2, &count_2);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx3) {
        rv = soc_counter_generic_get (unit, reg,
                                      ctrl_info, 0,
                                      idx3, &count_3);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get final counter value. */
    COMPILER_64_ZERO(*value);
    if (flags & _FP_STAT_ADD) {
        COMPILER_64_ADD_64(count_1, count_2);
        COMPILER_64_ADD_64(count_1, count_3);
    } else if (flags & _FP_STAT_SUBSTRACT) {
        COMPILER_64_SUB_64(count_1, count_2);
    }
    COMPILER_64_OR(*value, count_1);
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_field_th_multi_pipe_stat_value_get
 *
 * Description:
 *      Get 64 bit counter value for specific statistic type.
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      f_st      - (IN) Statistics entity descriptor.
 *      stat      - (IN) Collected statistics descriptor.
 *      value     - (OUT) Collected counters value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_stat_value_get(int unit, 
                                 _field_stat_t *f_st, 
                                 bcm_field_stat_t stat,
                                 uint64 *value)
{
    int                 idx1;            /* First counter index to read.      */
    int                 idx2;            /* Second counter index to read.     */
    int                 idx3;            /* Third counter index to read.      */
    int                 rv;              /* Operation return status.          */
    int                 stat_group_pipe; /* XGS Pipline associated 
                                            to the field group.               */
    uint32              flags;           /* _FP_STAT_XXX.                     */
    uint64              packet_count_1;  /* Primary index packet count.       */
    uint64              byte_count_1;    /* Primary index byte count.         */
    uint64              packet_count_2;  /* Secondary index packet count.     */
    uint64              byte_count_2;    /* Secondary index byte count.       */
    uint64              packet_count_3;  /* Third counter index packet count. */
    uint64              byte_count_3;    /* Third counterindex byte count.    */
    _field_stage_t      *stage_fc;       /* Stage field control.              */
    _field_control_t    *fc;             /* Field control structure.          */

    /* Initialization. */
    COMPILER_64_ZERO(packet_count_1);
    COMPILER_64_ZERO(packet_count_2);
    COMPILER_64_ZERO(packet_count_3);
    COMPILER_64_ZERO(byte_count_1);
    COMPILER_64_ZERO(byte_count_2);
    COMPILER_64_ZERO(byte_count_3);
    idx1 = idx2 = idx3 = _FP_INVALID_INDEX;
    flags = 0;

    /* Get field control. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Check that stat was requested during statistics entity creation. */
    for (idx1 = 0; idx1 < f_st->nstat; idx1++) {
        if (stat == f_st->stat_arr[idx1]) {
            break;
        }
    }
    if (idx1 == f_st->nstat) {
        return (BCM_E_PARAM);
    }

    /* Get hw indexes and flags needed to compose requested statistic.*/
    rv = fc->functions.fp_stat_index_get(unit, f_st, stat, &idx1,
                                         &idx2, &idx3, &flags);
    BCM_IF_ERROR_RETURN(rv);

    stat_group_pipe = _BCM_FIELD_GLOBAL_GROUP;

    /* Get accumulated counter value at primary index. */
    if (_FP_INVALID_INDEX != idx1) {
        rv = _field_th_multi_pipe_sw_counter_get(unit, stage_fc, 
                                              stat_group_pipe, idx1, 
                                              &packet_count_1,
                                              &byte_count_1);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx2) {
        rv = _field_th_multi_pipe_sw_counter_get(unit, stage_fc, 
                                              stat_group_pipe, idx2, 
                                              &packet_count_2,
                                              &byte_count_2);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get accumulated counter value at secondary index. */
    if (_FP_INVALID_INDEX != idx3) {
        rv = _field_th_multi_pipe_sw_counter_get(unit, stage_fc, 
                                              stat_group_pipe, idx3, 
                                              &packet_count_3,
                                              &byte_count_3);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Get final counter value. */
    COMPILER_64_ZERO(*value);
    if (flags & _FP_STAT_BYTES) {
        if (flags & _FP_STAT_ADD) {
            COMPILER_64_ADD_64(byte_count_1, byte_count_2);
            COMPILER_64_ADD_64(byte_count_1, byte_count_3);
        } else if (flags & _FP_STAT_SUBSTRACT) {
            COMPILER_64_SUB_64(byte_count_1, byte_count_2);
        }
        COMPILER_64_OR(*value, byte_count_1);
    } else {
        if (flags & _FP_STAT_ADD) {
            COMPILER_64_ADD_64(packet_count_1, packet_count_2);
            COMPILER_64_ADD_64(packet_count_1, packet_count_3);
        } else if (flags & _FP_STAT_SUBSTRACT) {
            COMPILER_64_SUB_64(packet_count_1, packet_count_2);
        }
        COMPILER_64_OR(*value, packet_count_1);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_th_counter_multi_pipe_memacc_alloc
 * Purpose:
 *     Initialize software cached memory access info for field counters 
 *
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     mem          - (IN) Counter memory.
 *     descr        - (IN) Counter descriptor. 
 *     ptr          - (OUT) Allocated pointer. 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_th_counter_multi_pipe_memacc_alloc(int unit, soc_memacc_t **memacc_ptr)
{
    int mem_size; /* Total memory size of memory access info of 
                     all different counter in all pipes */

    /* Input parameters check. */
    if (NULL == memacc_ptr) {
        return (BCM_E_PARAM);
    }

    mem_size = _FIELD_COUNTER_MEMACC_PIPE_NUM * sizeof(soc_memacc_t);
    *memacc_ptr = sal_alloc(mem_size, "FP counter memory access cache");
    if (NULL == *memacc_ptr) {
        return (BCM_E_MEMORY);
    }
    sal_memset(*memacc_ptr, 0, mem_size);
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _bcm_field_th_counter_multi_pipe_collect_init
 * Purpose:
 *     Initialize software field 64bit counters.
 *
 * Parameters:
 *     unit         - (IN) BCM device number.
 *     fc           - (IN) Field control structure.
 *     stage_fc     - (IN/OUT) Stage field control structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_counter_multi_pipe_collect_init(int unit, 
                                       _field_control_t *fc,
                                       _field_stage_t *stage_fc)
{
    int              rv = BCM_E_NONE; /* Operation return status.            */
    int              pipe;            /* XGS pipeline to operate on.         */
    int              max_pipes;       /* Maximum XGS pipelines in the stage. */
    soc_memacc_t     *memacc_list;    /* Memory access cache list.           */
    soc_mem_t        counter_mem[_FIELD_MAX_NUM_PIPES]; /* FieldProcessor
                                                           (IFP/EFP/VFP) 
                                                           counter memories  */
    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc,
                                                  counter_mem);
    BCM_IF_ERROR_RETURN(rv);


    max_pipes = stage_fc->pipes;

    /* Check if stage has counters. */
    if (INVALIDm == counter_mem[0]) {
        return (BCM_E_NONE);
    }

    /* Allocate space for cached counter memory access info */
    rv = _field_th_counter_multi_pipe_memacc_alloc(unit,
                                   &(stage_fc->_field_memacc_counters));
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    memacc_list = stage_fc->_field_memacc_counters;

    for (pipe = 0; pipe < max_pipes; pipe++) {

        rv = _bcm_field_counter32_collect_alloc(unit, counter_mem[pipe],
                                               "FP pipeline counters",
                                           &stage_fc->_field_32_counters[pipe]);
        if (BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }

        rv = _bcm_field_counter64_collect_alloc(unit, counter_mem[pipe],
                                            "FP pipeline byte counters",
                                           &stage_fc->_field_64_counters[pipe]);
        if (BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }

        rv = soc_memacc_init(unit, counter_mem[pipe], BYTE_COUNTERf,
                                 &(memacc_list[(pipe * 2)]));
        if(BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }

        rv = soc_memacc_init(unit, counter_mem[pipe], PACKET_COUNTERf,
                            &(memacc_list[((pipe * 2) + 1)]));

        if(BCM_FAILURE(rv)) {
            _bcm_field_counter_collect_deinit(unit, stage_fc);
            return (rv);
        }
    }
    return (rv);
}
/*
 * Function:
 *  _bcm_field_th_multi_pipe_sw_counter_update
 * Description:
 *  Service routine used to update 64-bit software accumulated counters.
 *  based on hw memory block.
 * Parameters:
 *   unit     - (IN) BCM device number. 
 *   stage_fc - (IN/OUT) Stage field control structure.
 *   mem      - (IN) Counters memory.
 *   idx_min  - (IN) First counter index in the buffer.
 *   idx_max  - (IN) Last counter index in the buffer. 
 *   buf      - (IN) Counters buffer.
 *   flags    - (IN) Device counters flags.     
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_sw_counter_update(int unit, _field_stage_t *stage_fc, 
                                        soc_mem_t mem, int idx_min, int idx_max,
                                        char *buf, int pipe)
{
    int                    rv = BCM_E_NONE; /* Operartion return status.      */
    int                    idx;             /* Iteration index.               */
    int                    buf_offset;      /* Offset into counters buffer.   */
    uint32                 *entry_p;        /* HW entry pointer.              */
    _field_control_t       *fc;             /* Field control structure.       */

    if ((NULL == stage_fc) || (NULL == buf)) {
        return BCM_E_PARAM;
    }

    /* Get field control structure. */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    for (idx = idx_min, buf_offset = 0; idx <= idx_max; idx++, buf_offset++) {
        /* Read counter value from the first memory. */
        entry_p = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                               buf, buf_offset);

        /* Process accumulated counter value. */
        rv = _bcm_field_th_pipe_counter_get(unit, stage_fc, mem, entry_p,
                                            idx, pipe, NULL, NULL);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return rv;
}
/*
 * Function:
 *    _field_th_multi_pipe_mem_counters_read
 * Description:
 *    Service routine used to coolect 64-bit software accumulated counters.
 *    for a specific stage.
 * Parameters:
 *    unit     - (IN) BCM device number. 
 *    mem      - (IN) Counters memory.
 *    pipe     - (IN) XGS pipeline to operate on. 
 *    stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_field_th_multi_pipe_mem_counters_read(int unit, 
                                    soc_mem_t mem, 
                                    int pipe,
                                    _field_stage_t *stage_fc)
{
    int       rv = BCM_E_NONE;       /* Operation return value.   */
    int       idx;                   /* Table iteration index.    */
    int       buf_end_idx;           /* Buffer end index.         */
    int       alloc_size;            /* DMA buffer size.          */
    int       max_idx;               /* Counter memory index max. */
    int       min_idx;               /* Counter memory index min. */
    char      *buf;                  /* Buffer to read the table. */


    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Make sure memory is valid */
    if (INVALIDm == mem) {
        return BCM_E_INTERNAL;
    }

    /* Get table boundaries. */
    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);

    /* Allocate memory buffer. */
    alloc_size = (_FP_64_COUNTER_BUCKET * sizeof(fp_counter_table_entry_t));
    buf = soc_cm_salloc(unit, alloc_size, "fp_64_bit_counter");
    if (buf == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Dma part of the table & update software counters. */
    soc_mem_lock(unit, mem);
    for (idx = min_idx; idx <= max_idx; idx += _FP_64_COUNTER_BUCKET) {
        buf_end_idx = MIN(max_idx, (idx + _FP_64_COUNTER_BUCKET - 1));
        /* Dma range of entries in counter table. */
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, idx,
                                    buf_end_idx, buf);
        if (BCM_FAILURE(rv)) {
            break;
        }

        rv = _bcm_field_th_multi_pipe_sw_counter_update(unit, stage_fc,
                                                     mem, idx,
                                                     buf_end_idx,
                                                     buf, pipe);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, buf);
    return (rv);
}
/*
 * Function:
 *  _bcm_field_th_stage_multi_pipe_counters_collect
 * Description:
 *  Service routine used to coolect  64-bit software accumulated counters.
 *  for a specific stage.
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   fc       - (IN) Field control structure.
 *   stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  None.
 */
int
_bcm_field_th_stage_multi_pipe_counters_collect (int unit, _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    int              rv = BCM_E_NONE; /* Operation return value.            */
    int              pipe;            /* XGS pipeline to operate on.        */
    int              max_pipes;       /* Maximum XGS pipelines in the stage */
    soc_mem_t        counter_mem[_FIELD_MAX_NUM_PIPES]; /* FieldProcessor
                                                           (IFP/EFP/VFP) 
                                                           counter memories */
    /* Input parameters check. */
    if (NULL == stage_fc || NULL == fc) {
        return (BCM_E_PARAM);
    }
    
    max_pipes = stage_fc->pipes;
    for (pipe = 0; pipe < max_pipes; pipe++ ) {
        if (NULL == stage_fc->_field_32_counters[pipe] ||
            NULL == stage_fc->_field_64_counters[pipe]) 
            return BCM_E_UNAVAIL;
    }   

    /* Get counters memory for the stage. */
    rv = _bcm_field_th_multi_pipe_counter_mem_get(unit, stage_fc,
                                                     counter_mem);
    BCM_IF_ERROR_RETURN(rv);

    for (pipe = 0; pipe < max_pipes; pipe++ ) {
        if (INVALIDm != counter_mem[pipe]) {
        BCM_IF_ERROR_RETURN
            (_field_th_multi_pipe_mem_counters_read(unit, counter_mem[pipe],
                                                 pipe, stage_fc));
        }
    }   

    return BCM_E_NONE; 
}
/*
 * Function:
 *     _bcm_th_field_counter_config_set
 * Description:
 *     To set the SOC configuration for EFP counters.
 * Parameters:
 *     unit     - (IN) BCM device number.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Field module must perform 2 steps to start the counter module to 
 *     start performing the DMA.
 *     1. Enable the DMA for the respective counter (Both PKT/BYTE)
 *     2. Setup a DMA profile. 
 */

int
_bcm_th_field_counter_config_set(int unit)
{
    int rv;
    int count;
    soc_ctr_control_info_t ctrl_info;
    soc_ctr_ctrl_config_t  ctrl_config;

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PIPE; 
    ctrl_info.instance = -1; 
    ctrl_config.config_type = SOC_CTR_CTRL_CONFIG_DMA_ENABLE; 
    ctrl_config.config_val = 1; /* Enable DMA */ 
    count = 1; 
    rv = soc_counter_config_multi_set(0, SOC_COUNTER_NON_DMA_EFP_PKT, 
                                      count, &ctrl_info, &ctrl_config); 
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_counter_config_multi_set(0, SOC_COUNTER_NON_DMA_EFP_BYTE, 
                                  count, &ctrl_info, &ctrl_config);

    BCM_IF_ERROR_RETURN(rv);

    ctrl_config.config_type = SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_ALL; 
    ctrl_config.config_val = 1; /* Enable ALL profiles for EFP_PKT */ 
    rv = soc_counter_config_multi_set (0, SOC_COUNTER_NON_DMA_EFP_PKT, 
                                       count, &ctrl_info, &ctrl_config);

    BCM_IF_ERROR_RETURN(rv);

    rv = soc_counter_config_multi_set(0, SOC_COUNTER_NON_DMA_EFP_BYTE,
                                  count, &ctrl_info, &ctrl_config);
    return rv;
}

              /*  END OF COUNTER ROUTINES */

              /*  START OF POLICER ROUTINES */
/*
 * Function:
 *     _bcm_field_th_multi_pipe_policer_mem_get
 * Purpose:
 *     Get policer table name.
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     stage_fc     - (IN)  Field Processor stage control structure.
 *     counter_mem  - (OUT) pointer to array of counter memories.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_multi_pipe_policer_mem_get(int unit, 
                                         _field_stage_t *stage_fc,
                                         soc_mem_t *policer_mem)
{
    int              pipe;            /* XGS pipeline to operate on.        */
    int              max_pipes;       /* Maximum XGS pipelines in the stage */

    if (NULL == policer_mem || NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    max_pipes = stage_fc->pipes;
    for (pipe = 0; pipe < max_pipes; pipe++) {
        policer_mem[pipe] = INVALIDm;
    }

    /* Resolve meter table name. */
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          policer_mem[0] = IFP_METER_TABLE_PIPE0m;
          policer_mem[1] = IFP_METER_TABLE_PIPE1m;
          policer_mem[2] = IFP_METER_TABLE_PIPE2m;
          policer_mem[3] = IFP_METER_TABLE_PIPE3m;
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          policer_mem[0] = EFP_METER_TABLE_PIPE0m;
          policer_mem[1] = EFP_METER_TABLE_PIPE1m;
          policer_mem[2] = EFP_METER_TABLE_PIPE2m;
          policer_mem[3] = EFP_METER_TABLE_PIPE3m;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_th_policer_install
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
_bcm_field_th_policer_install(int unit, _field_entry_t *f_ent,
                               _field_policer_t *f_pl)
{
    uint32    bucketsize_peak = 0;      /* Bucket size.                       */
    uint32    refresh_rate_peak = 0;    /* Policer refresh rate.              */
    uint32    granularity_peak = 0;     /* Policer granularity.               */
    uint32    bucketsize_commit = 0;    /* Bucket size.                       */
    uint32    refresh_rate_commit = 0;  /* Policer refresh rate.              */
    uint32    granularity_commit = 0;   /* Policer granularity.               */
    uint32    flags;                    /* Policer flags.                     */
    int       refresh_bitsize;          /* Number of bits for the
                                           refresh rate field.                */
    int       bucket_max_bitsize;       /* Number of bits for the
                                           bucket max field.                  */
    int       rv;                       /* Operation return status.           */
    int       group_mode;               /* Mode of filed group.               */
    int       pipe;                     /* XGS pipeline to operate on.        */
    int       max_pipes;                /* Maximum XGS pipelines in the stage */
    soc_mem_t policer_memory;           /* Meter table name.                  */
    soc_mem_t policer_mem[_FIELD_MAX_NUM_PIPES]; /* Meter table name's 
                                                    of all pipes.             */
    _field_stage_t *stage_fc;           /* Stage Field control structure.     */

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


    rv = _field_stage_control_get (unit, f_ent->fs->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Resolve meter table name. */
    rv = _bcm_field_th_multi_pipe_policer_mem_get(unit, stage_fc, policer_mem);
    BCM_IF_ERROR_RETURN(rv);

    group_mode = _BCM_FIELD_GLOBAL_GROUP;
    if (_BCM_FIELD_GLOBAL_GROUP == group_mode) {
        policer_memory = policer_mem[0];
    }

    max_pipes = stage_fc->pipes;

    refresh_bitsize = soc_mem_field_length(unit, policer_memory, REFRESHCOUNTf);
    bucket_max_bitsize = soc_mem_field_length(unit, 
                                              policer_memory, 
                                              BUCKETSIZEf);

    /* lookup bucket size from tables */
    flags = _BCM_XGS_METER_FLAG_GRANULARITY | _BCM_XGS_METER_FLAG_FP_POLICER;

    /* Set packet mode flags setting */
    if (f_pl->cfg.flags & BCM_POLICER_MODE_PACKETS) {
        flags |= _BCM_XGS_METER_FLAG_PACKET_MODE;
    } else {
        flags &= ~_BCM_XGS_METER_FLAG_PACKET_MODE;
    }
    
    if (f_pl->cfg.mode  != bcmPolicerModeSrTcm) {
        if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                   f_pl->cfg.ckbits_burst,
                                                   flags, refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_commit,
                                                   &bucketsize_commit,
                                                   &granularity_commit);

            max_pipes = stage_fc->pipes;
            for (pipe = 0; pipe < max_pipes; pipe++) {
                if ((INVALIDm != policer_mem[pipe]) && BCM_SUCCESS(rv)) {
                    /* Programm policer parameters into hw. */
                    rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                      BCM_FIELD_METER_COMMITTED,
                                                      bucketsize_commit,
                                                      refresh_rate_commit,
                                                      granularity_commit, 
                                                      policer_mem[pipe]);

                }
            }
            f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;
        }

        if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {

            if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                /*
                 * Rates are always set in committed variables,
                 * for flow meters.
                 */
                f_pl->cfg.pkbits_sec = f_pl->cfg.ckbits_sec;
                f_pl->cfg.pkbits_burst = f_pl->cfg.ckbits_burst;
            }

            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                   f_pl->cfg.pkbits_burst,
                                                   flags,
                                                   refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_peak,
                                                   &bucketsize_peak,
                                                   &granularity_peak);
            for (pipe = 0; pipe < max_pipes; pipe++) {
                if ((INVALIDm != policer_mem[pipe]) && BCM_SUCCESS(rv)) {
                    /* Programm policer parameters into hw. */
                    rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                           BCM_FIELD_METER_PEAK,
                                                           bucketsize_peak,
                                                           refresh_rate_peak,
                                                           granularity_peak, 
                                                           policer_mem[pipe]);

                }
            }

            f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;

            if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                /* Reset peak meter rates. */
                f_pl->cfg.pkbits_sec = 0;
                f_pl->cfg.pkbits_burst = 0;
            }
        }
    } else {
        if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.ckbits_sec,
                                                   f_pl->cfg.ckbits_burst,
                                                   flags, refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_commit,
                                                   &bucketsize_commit,
                                                   &granularity_commit);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
            /* Calculate policer bucket size/refresh_rate/granularity. */
            rv = _bcm_xgs_kbits_to_bucket_encoding(f_pl->cfg.pkbits_sec,
                                                   f_pl->cfg.pkbits_burst,
                                                   flags,
                                                   refresh_bitsize,
                                                   bucket_max_bitsize,
                                                   &refresh_rate_peak,
                                                   &bucketsize_peak,
                                                   &granularity_peak);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (granularity_commit != granularity_peak) {
            if (granularity_commit < granularity_peak) {
                rv = _bcm_xgs_kbits_to_dual_bucket_encoding(
                                                         f_pl->cfg.ckbits_sec,
                                                         f_pl->cfg.ckbits_burst,
                                                         flags,
                                                         refresh_bitsize,
                                                         bucket_max_bitsize,
                                                         granularity_peak,
                                                         &refresh_rate_commit,
                                                         &bucketsize_commit,
                                                         &granularity_commit);
            } else if (granularity_commit > granularity_peak) {
                rv = _bcm_xgs_kbits_to_dual_bucket_encoding(
                                                         f_pl->cfg.pkbits_sec,
                                                         f_pl->cfg.pkbits_burst,
                                                         flags,
                                                         refresh_bitsize,
                                                         bucket_max_bitsize,
                                                         granularity_commit,
                                                         &refresh_rate_peak,
                                                         &bucketsize_peak,
                                                         &granularity_peak);
            }
            BCM_IF_ERROR_RETURN(rv);
        }


        for (pipe = 0; pipe < max_pipes; pipe++) {
            if ((INVALIDm != policer_mem[pipe]) && BCM_SUCCESS(rv)) {
                /* Programm policer parameters into hw. */
                rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                      BCM_FIELD_METER_COMMITTED,
                                                      bucketsize_commit,
                                                      refresh_rate_commit,
                                                      granularity_commit, 
                                                      policer_mem[pipe]);
            }
        }

        f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;

        for (pipe = 0; pipe < max_pipes; pipe++) {
            if ((INVALIDm != policer_mem[pipe]) && BCM_SUCCESS(rv)) {
                /* Programm policer parameters into hw. */
                rv =  _bcm_field_trx_policer_hw_update(unit, f_ent, f_pl,
                                                     BCM_FIELD_METER_PEAK,
                                                     bucketsize_peak,
                                                     refresh_rate_peak,
                                                     granularity_peak,   
                                                     policer_mem[pipe]);
            }
        }

        f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;
    }
    return rv;
}

/*
 * Function:
 *     _bcm_field_th_policer_meter_hw_free
 *
 * Purpose:
 *     Clear the meter entries from the h/w associated with the policer
 * 
 * Parameters:
 *     unit      - (IN) BCM device number.  
 *     level     - (IN) Policer level.
 *     f_ent     - (IN) Entry policer belongs to.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_policer_meter_hw_free(int unit, _field_entry_t *f_ent,
                                 _field_entry_policer_t *f_ent_pl)
{
    _field_stage_t    *stage_fc;        /* Stage field control structure */
    _field_policer_t  *f_pl;            /* Policer descriptor            */
    soc_mem_t          meter_table;     /* Meter table name              */
    int                meter_idx;       /* Meter hw index                */
    int                idx;             /* Meter hw index                */
    int                rv = BCM_E_NONE; /* Return value                  */
    int                pipe;            /* XGS pipeline to operate on.        */
    int                max_pipes;       /* Maximum XGS pipelines in the stage */
    soc_mem_t policer_mem[_FIELD_MAX_NUM_PIPES]; /* Meter table name's 
                                                    of all pipes.             */

    /* Get field stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id,
                                  &stage_fc));

    /* Resolve meter table name. */
    rv = _bcm_field_th_multi_pipe_policer_mem_get(unit, stage_fc, policer_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Read policer configuration.*/
    BCM_IF_ERROR_RETURN
        (_bcm_field_policer_get(unit, f_ent_pl->pid, &f_pl));

    /* Must be a valid meter index. */
    if (f_pl->hw_index == _FP_INVALID_INDEX) {
        return (BCM_E_INTERNAL);
    }

    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        /* Get slice policer installed in. */
        idx =  (f_pl->pool_index *
                stage_fc->meter_pool[f_pl->pool_index]->pool_size)
                + (2 * f_pl->hw_index);

    } else {
        idx = stage_fc->slices[f_pl->pool_index].start_tcam_idx + \
                       (2 * f_pl->hw_index);
    }

    max_pipes = stage_fc->pipes;
    meter_idx = idx;
    for (pipe = 0; pipe < max_pipes; pipe++) {
        idx = meter_idx;

        meter_table = policer_mem[pipe];

        if (meter_table == INVALIDm) {
            continue;
        }

        if (idx < soc_mem_index_min(unit, meter_table) ||
            idx > soc_mem_index_max(unit, meter_table)) {
            return (BCM_E_INTERNAL);
        }

        if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {
            rv = soc_mem_write(unit, meter_table, MEM_BLOCK_ALL,
                    idx, soc_mem_entry_null(unit,meter_table));
            BCM_IF_ERROR_RETURN(rv);

            idx++;
            rv = soc_mem_write(unit, meter_table, MEM_BLOCK_ALL,
                    idx, soc_mem_entry_null(unit,meter_table));
        } else {
            if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                idx ++;
            }

            rv = soc_mem_write(unit, meter_table, MEM_BLOCK_ALL,
                    idx, soc_mem_entry_null(unit,meter_table));
        }
    }

    return (BCM_E_NONE);
}


         /* END OF POLICER ROUTINES */

#ifdef BCM_WARM_BOOT_SUPPORT

              /*  START OF WARMBOOT ROUTINES */
/*
 * Function:
 *     _bcm_field_th_scache_sync
 *
 * Purpose:
 *     Save field module software state to external cache.
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Pointer to device field control structure
 *     stage_fc         - (IN) FP stage control info.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_scache_sync(int              unit,
                           _field_control_t *fc,
                           _field_stage_t   *stage_fc
                           )
{
    int                   idx;             /* Index to field entry.          */
    int                   rv = BCM_E_NONE; /* Operation return value.        */
    int                   efp_slice_mode;  /* Slice mode of EFP.             */
    int                   paired = 0;      /* TCAM paired status of a slice. */
    int                   ratio = 0;       /* TCAM entry ratio.              */
    int                   slice_idx;       /* Index of a slice in (V/I/E)FP  */
    uint8                 *buf;            /* Pointer to 1st scache part.    */
    uint8                 *buf1;           /* Pointer to 2nd scache part.    */
    uint32                start_char;      /* Start character for (V/I/E)FP  */
    uint32                end_char;        /* End caharacter for (V/I/E)FP.  */
    uint32                val;             /* Carry Register contents.       */
    uint64                rval;            /* Carry register contents.       */
    _field_slice_t        *fs;             /* Slice pointer.                 */
    _field_group_t        *fg;             /* Filed group pointer.           */
    _field_data_control_t *data_ctrl;      /* UDF control data pointer.      */
    _field_entry_t        *f_ent;          /* Pointer to field entry.        */
    soc_field_t           fld;             /* field in any memory/register.  */

    soc_field_t _efp_slice_mode[] = {SLICE_0_MODEf, SLICE_1_MODEf,
                                     SLICE_2_MODEf, SLICE_3_MODEf};

    buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];

    switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            start_char = _FIELD_IFP_DATA_START;
            end_char = _FIELD_IFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            start_char = _FIELD_EFP_DATA_START;
            end_char = _FIELD_EFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            start_char = _FIELD_VFP_DATA_START;
            end_char = _FIELD_VFP_DATA_END;
            break;
        case _BCM_FIELD_STAGE_EXTERNAL:
            /*
            if (_field_tr2_ext_scache_sync_chk(unit, fc, stage_fc)) {
                return (_field_tr2_ext_scache_sync(unit, fc, stage_fc));
            } */
            start_char = _FIELD_EXTFP_DATA_START;
            end_char   = _FIELD_EXTFP_DATA_END;
            break;
        default:
            return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d): _bcm_field_th_scache_sync() - "
                          "Synching scache for FP stage %d...\n"),
               unit, stage_fc->stage_id));

    _field_scache_stage_hdr_save(fc, start_char);

    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d): _bcm_field_th_scache_sync() -"
                              " Writing end of section @ byte %d.\n"),
                   unit, fc->scache_pos));

        /* Mark the end of the IFP section */
        buf[fc->scache_pos] = end_char & 0xFF;
        fc->scache_pos++;
        buf[fc->scache_pos] = (end_char >> 8) & 0xFF;
        fc->scache_pos++;
        buf[fc->scache_pos] = (end_char >> 16) & 0xFF;
        fc->scache_pos++;
        buf[fc->scache_pos] = (end_char >> 24) & 0xFF;
        fc->scache_pos++;
        fc->scache_usage = fc->scache_pos; /* Usage in bytes */

        if (NULL != buf1) {
            /* Mark the end of the IFP section */
            buf1[fc->scache_pos1] = end_char & 0xFF;
            fc->scache_pos1++;
            buf1[fc->scache_pos1] = (end_char >> 8) & 0xFF;
            fc->scache_pos1++;
            buf1[fc->scache_pos1] = (end_char >> 16) & 0xFF;
            fc->scache_pos1++;
            buf1[fc->scache_pos1] = (end_char >> 24) & 0xFF;
            fc->scache_pos1++;
        }
        return BCM_E_NONE;
    }

    /* Save data qualifiers */
    if ((data_ctrl = stage_fc->data_ctrl) != 0) {
        if (soc_mem_is_valid(unit, FP_UDF_TCAMm)) {
            /* Device has UDF TCAM =>
               Save internal information regarding TCAM entry usage
            */

            _field_data_tcam_entry_t *p;
            unsigned                 n;

            for (p = data_ctrl->tcam_entry_arr,
                     n = soc_mem_index_count(unit, FP_UDF_TCAMm);
                 n;
                 --n, ++p
                 ) {
                buf[fc->scache_pos] = p->ref_count;
                fc->scache_pos++;
            }
        }

        _field_scache_sync_data_quals_write(fc, data_ctrl);
    }

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d): _bcm_field_th_scache_sync() - "
                              "Checking slice %d...\n"),
                   unit, slice_idx));
        /* Skip slices without groups */
        fs = stage_fc->slices + slice_idx;
        fg = fc->groups;
        while (fg != NULL) {
            if (fg->stage_id != stage_fc->stage_id) {
                fg = fg->next;
                continue; /* Not in this stage */
            }
            if (fg->slices[0].slice_number == slice_idx) {
                break;
            }
            fg = fg->next;
        }

        if (fg == NULL) {
            continue; /* No group found */
        }

        /* Also skip expanded slices */
        if (stage_fc->slices[slice_idx].prev != NULL) {
            continue;
        }

        /* Ignore secondary slice in paired mode */
        switch (stage_fc->stage_id) {
            case _BCM_FIELD_STAGE_EGRESS:
                BCM_IF_ERROR_RETURN(READ_EFP_SLICE_CONTROLr(unit, &val));
                efp_slice_mode = soc_reg_field_get(unit,
                                     EFP_SLICE_CONTROLr,
                                     val,
                                     _efp_slice_mode[slice_idx]);
                if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) {
                    paired = 1;
                }
                break;
            case _BCM_FIELD_STAGE_LOOKUP:
                BCM_IF_ERROR_RETURN(READ_VFP_KEY_CONTROL_1r(unit, &rval));
                fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
                paired = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            rval,
                            fld);
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
        if (paired && (slice_idx % 2)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_field_tr2_group_entry_write(unit,
                slice_idx,
                fs,
                fc,
                stage_fc));
    }

    /* Now sync the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        fs = stage_fc->slices + slice_idx;
        /* Skip empty slices */
        if (fs->entry_count == fs->free_count) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "FP(unit %d): _bcm_field_th_scache_sync() - "
                                  "Slice is empty.\n"),
                       unit));
            continue;
        }

        /* Skip master slices */
        if (stage_fc->slices[slice_idx].prev == NULL) {
            continue;
        }

        /* 
         * Skip expanded slices with no entries installed in Hw
         * to match recovery logic.
         */
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
        } else {
            ratio = 1;
        }

        for (idx = 0; idx < fs->entry_count / ratio; idx++) {
            /* Find EID that matches this HW index */
            f_ent = fs->entries[idx];
            if (f_ent == NULL) {
                continue;
            }
            if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
                continue;
            }
            break;
        }

        if (idx == (fs->entry_count / ratio)) {
            continue;
        }

        /* Ignore secondary slice in paired mode */
        switch (stage_fc->stage_id) {
            case _BCM_FIELD_STAGE_EGRESS:
                BCM_IF_ERROR_RETURN(READ_EFP_SLICE_CONTROLr(unit, &val));
                efp_slice_mode = soc_reg_field_get(unit,
                                    EFP_SLICE_CONTROLr,
                                    val,
                                    _efp_slice_mode[slice_idx]);
                if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY)
                    || (efp_slice_mode
                    == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) {
                    paired = 1;
                }
                break;
            case _BCM_FIELD_STAGE_LOOKUP:
                BCM_IF_ERROR_RETURN(READ_VFP_KEY_CONTROL_1r(unit, &rval));
                fld = _bcm_field_trx_slice_pairing_field[slice_idx / 2];
                paired = soc_reg64_field32_get(unit,
                            VFP_KEY_CONTROL_1r,
                            rval,
                            fld);
                break;
            default:
                return BCM_E_PARAM;
                break;
        }
        if (paired && (slice_idx % 2)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_field_tr2_group_entry_write(unit, slice_idx, fs, fc, stage_fc));
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d): _bcm_field_th_scache_sync() -"
                          " Writing end of section @ byte %d.\n"),
               unit, fc->scache_pos));

    /* Mark the end of the section */
    buf[fc->scache_pos] = end_char & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 8) & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 16) & 0xFF;
    fc->scache_pos++;
    buf[fc->scache_pos] = (end_char >> 24) & 0xFF;
    fc->scache_pos++;
    fc->scache_usage = fc->scache_pos; /* Usage in bytes */

    if(NULL != buf1) {
        /* Mark the end of the IFP section */
        buf1[fc->scache_pos1] = end_char & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 8) & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 16) & 0xFF;
        fc->scache_pos1++;
        buf1[fc->scache_pos1] = (end_char >> 24) & 0xFF;
        fc->scache_pos1++;
    }

    return rv;
}
/*
 * Function:
 *  _bcm_field_th_stage_ingress_reinit
 * Description:
 *  Service routine used to retain the software sate of 
 *  IFP(Ingress Field Processor) configuration .
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   fc       - (IN) Field control structure.
 *   stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  None.
 */
int
_bcm_field_th_stage_ingress_reinit(int unit,
                                   _field_control_t *fc,
                                   _field_stage_t   *stage_fc)
{
    int    rv = BCM_E_NONE; /* Operation return value.     */
    uint8  *buf;            /* Pointer to 1st scache part. */
    uint8  *buf1;           /* Pointer to 2nd scache part. */
    uint16 version;         /* Vesrion of the scache file. */
    uint32 temp;            /* Temporary variable.         */

    buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];

    fc->scache_pos = 0;
    fc->scache_pos1 = 0;

    if (fc->l2warm) {

        sal_memcpy(&version, 
                   fc->scache_ptr[_FIELD_SCACHE_PART_0], 
                   sizeof(uint16));

        if (version > BCM_WB_DEFAULT_VERSION) {
            /* Notify the application with an event */
            /* The application will then need to reconcile the
               version differences using the documented behavioral
               differences on per module (handle) basis */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                    BCM_MODULE_FIELD, version,
                                    BCM_WB_DEFAULT_VERSION));
        }

        fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

        if(NULL != fc->scache_ptr[_FIELD_SCACHE_PART_1]) {

            sal_memcpy(&version, 
                       fc->scache_ptr[_FIELD_SCACHE_PART_1], 
                       sizeof(uint16));

            if (version > BCM_WB_DEFAULT_VERSION) {
                /* Notify the application with an event */
                /* The application will then need to reconcile the
                   version differences using the documented behavioral
                   differences on per module (handle) basis */
                SOC_IF_ERROR_RETURN
                    (soc_event_generate(unit,
                                        SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                        BCM_MODULE_FIELD, version,
                                        BCM_WB_DEFAULT_VERSION));
            }

            fc->scache_pos1 += SOC_WB_SCACHE_CONTROL_SIZE;
        }
    }


    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_IFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    if (fc->l2warm) {
        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_IFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_IFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }

    }


    return rv;
}
/*
 * Function:
 *     _bcm_field_th_group_construct_quals_add
 *
 * Purpose:
 *     Recover all qualifiers for the given group, based on its selector codes
 *
 * Parameters:
 *     unit             - (IN) BCM device number
 *     fc               - (IN) Device field control structure pointer
 *     stage_fc         - (IN) FP stage control info
 *     fg               - (IN/OUT) Select code information filled into the group
 *     entry_type       - (IN) EFP entry type 
 *
 * Returns:
 *     BCM_E_NONE
 */
int
_bcm_field_th_group_construct_quals_add(int unit,
                                         _field_control_t *fc,
                                         _field_stage_t *stage_fc, 
                                         _field_group_t *fg,
                                         uint8 entry_type)
{
    int rv;        /* Operation return value.       */
    int parts_cnt; /* Parts count in a field group. */
    int part_idx;  /* Part index in a field group.  */

    if ((!fc) || (!stage_fc) || (!fg)) {
        return(BCM_E_INTERNAL);
    }

    rv = _bcm_field_entry_tcam_parts_count(unit, fg->flags, &parts_cnt);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    for (part_idx = 0; part_idx < parts_cnt; ++part_idx) {
        _bcm_field_group_qual_t *grp_qual = &fg->qual_arr[entry_type][part_idx];
        unsigned qid;

        for (qid = 0; qid < _bcmFieldQualifyCount; ++qid) {
            _bcm_field_qual_info_t *f_qual_arr = stage_fc->f_qual_arr[qid];
            unsigned               j;
            uint8                  diff_cnt;

            if (f_qual_arr == NULL) {
                continue; /* Qualifier does not exist in this stage */
            }

            if (fc->l2warm && !BCM_FIELD_QSET_TEST(fg->qset, qid)) {
                continue; /* Qualifier not present in the group */
            }

            /* Add all of the stage's qualifiers that match the recovered
               selector codes.  Qualifiers that appear more than once
               (because more than one configuration of a qualifier matches
               the recovered selector codes) will be cleaned up later.
            */

            for (j = 0; j < f_qual_arr->conf_sz; j++) {
                if (_field_selector_diff(unit,
                                         fg->sel_codes,
                                         part_idx,
                                         &f_qual_arr->conf_arr[j].selector,
                                         &diff_cnt
                                         )
                    != BCM_E_NONE
                    || diff_cnt != 0
                    ) {
                    continue;
                }

                if (!fc->l2warm)  {
                    BCM_FIELD_QSET_ADD(fg->qset, qid);
                }

                _field_trx_group_qual_add(grp_qual,
                                          qid,
                                          &f_qual_arr->conf_arr[j].offset
                                          );

                _field_qset_udf_bmap_reinit(unit,
                                            stage_fc,
                                            &fg->qset,
                                            qid
                                            );
            }
        }
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *  _bcm_field_th_stage_egress_reinit
 * Description:
 *  Service routine used to retain the software sate of 
 *  EFP(Egress Field Processor) configuration .
 * Parameters:
 *   unit     - (IN) BCM device number.
 *   fc       - (IN) Field control structure.
 *   stage_fc - (IN/OUT) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  None.
 */
int
_bcm_field_th_stage_egress_reinit(int unit, _field_control_t *fc,
                               _field_stage_t *stage_fc)
{
    char *efp_policy_buf = NULL;  /* Buffer to read the EFP_POLICY table      */
    int  idx;                     /* Index to field entry.                    */
    int slice_idx;                /* Slice index in (V/I/E)FP.                */
    int vslice_idx;               /* Slice index in a virtual group.          */
    int index_min;                /* Min index of a memory.                   */
    int index_max;                /* Max index of a field memory.             */
    int rv = BCM_E_NONE;          /* Operation status.                        */
    int mem_sz;                   /* Size of the memory.                      */
    int slice_ent_cnt;            /* Entry count in a slice.                  */
    int parts_count = 1;          /* Parts count in a field group.            */
    int i;                        /* Used to loop through parts i
                                     in a field group.                        */
    int pri_tcam_idx;             /* Priority of an entry in TCAM.            */
    int part_index;               /* Index to part(s) in a field group.       */
    int slice_number;             /* (V/I/E) slice number.                    */
    int phys_tcam_idx;            /* Index to physical TCAM in (V/I/E)FP.     */
    int prio;                     /* Priority of current field entry.         */
    int prev_prio;                /* Priority of previous field entry.        */
    int expanded[4];              /* Expansion status of slices in (V/I/E)FP. */
    int priority;                 /* Priority of a field entry.               */
    int multigroup;               /* Field group's multigroup status.         */
    int max;                      /*Temporary variable.                       */
    int master_slice;             /* Master slice number for expanded group.  */
    uint8 *buf = NULL;            /* Pointer to 1st scache part.              */
    uint8 *buf1 = NULL;           /* Pointer to 2nd scache part.              */
    uint8 old_physical_slice;     /* Physical slice number.                   */
    uint8 slice_num;              /* Physical slice number.                   */
    uint16 default_key_1;         /* Default selcode of a field group in EFP. */
    uint16 default_key_2;         /* Default selcode of a field group in EFP. */
    uint32 *efp_tcam_buf = NULL;  /* Buffer to read the EFP_TCAM table.       */
    uint32 rval;                  /* Holds conetets of register.              */
    uint32 efp_slice_mode;        /* Mode of a slice in EFP.                  */
    uint32 efp_slice_ipv6_mode;   /* IPv6 mode of a slice in EFP.             */
    uint32 temp;                  /* Temporary varibale.                      */
    uint32 key_match_type[2];     /* Key match types of field group in EFP.   */
    uint32 entry_type = 0;        /* Type of entry of filed group in EFP.     */
    uint32 entry_flags;           /* Flags set for a entry in field group.    */
    _field_slice_t *fs = NULL;    /* Slice pointer.                           */
    _field_group_t *fg = NULL;    /* Filed group pointer.                     */
    _field_entry_t *f_ent = NULL; /* Field entry pointer.                     */
    bcm_field_group_t gid;        /* Field group ID.                          */
    bcm_field_entry_t eid;        /* Field entry ID.                          */
    bcm_field_stat_t sid;         /* Field stat ID.                           */
    bcm_policer_t pid;            /* Field policer ID.                        */
    bcm_field_qset_t qset;        /* QSET of field group.                     */
    bcm_pbmp_t all_pbmp;          /* Port Bit Map.                            */
    int group_flags = 0;
    uint8 dvp_type;
    efp_tcam_entry_t *efp_tcam_entry = NULL;           /* Buffer to hold
                                                          contents of entry in  
                                                          EFP_TCAM entry. */
    efp_policy_table_entry_t *efp_policy_entry = NULL; /* Buffer to hold 
                                                          contetnts of entry in
                                                          EFP_POLICY_TABLE. */


    soc_field_t efp_en_flds[4] = {SLICE_ENABLE_SLICE_0f, SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f, SLICE_ENABLE_SLICE_3f};

    soc_field_t efp_lk_en_flds[4] =
                      {LOOKUP_ENABLE_SLICE_0f, LOOKUP_ENABLE_SLICE_1f,
                       LOOKUP_ENABLE_SLICE_2f, LOOKUP_ENABLE_SLICE_3f};

    buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];

    sal_memset(expanded, 0, 4 * sizeof(int));

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_EFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* DMA various tables */
    efp_tcam_buf = soc_cm_salloc(unit, sizeof(efp_tcam_entry_t) *
                                 soc_mem_index_count(unit, EFP_TCAMm),
                                 "EFP TCAM buffer");
    if (NULL == efp_tcam_buf) {
        return BCM_E_MEMORY;
    }

    sal_memset(efp_tcam_buf, 0, sizeof(efp_tcam_entry_t) *
               soc_mem_index_count(unit, EFP_TCAMm));
    index_min = soc_mem_index_min(unit, EFP_TCAMm);
    index_max = soc_mem_index_max(unit, EFP_TCAMm);
    fs = stage_fc->slices;
    if (stage_fc->flags & _FP_STAGE_HALF_SLICE) {
        slice_ent_cnt = fs->entry_count * 2;
        /* DMA in chunks */
        for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
            fs = stage_fc->slices + slice_idx;
            if ((rv = soc_mem_read_range(unit, EFP_TCAMm, MEM_BLOCK_ALL,
                                         slice_idx * slice_ent_cnt,
                                         slice_idx * slice_ent_cnt +
                                             fs->entry_count - 1,
                                         efp_tcam_buf + slice_idx *
                                             slice_ent_cnt *
                                  soc_mem_entry_words(unit, EFP_TCAMm))) < 0 ) {
                goto cleanup;
            }
        }
    } else {
        slice_ent_cnt = fs->entry_count;
        if ((rv = soc_mem_read_range(unit, EFP_TCAMm, MEM_BLOCK_ALL,
                                     index_min, index_max,
                                     efp_tcam_buf)) < 0 ) {
            goto cleanup;
        }
    }
    efp_policy_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                  (unit, EFP_POLICY_TABLEm),
                                  "EFP POLICY TABLE buffer");
    if (NULL == efp_policy_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, EFP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, EFP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit, EFP_POLICY_TABLEm, MEM_BLOCK_ALL,
                                 index_min, index_max,
                                 efp_policy_buf)) < 0 ) {
        goto cleanup;
    }

    /* Get slice expansion status and virtual map */
    if ((rv = _field_slice_expanded_status_get(unit, stage_fc, expanded)) < 0) {
        goto cleanup;
    }

    /* Iterate over the slices */
    if ((rv = READ_EFP_SLICE_CONTROLr(unit, &rval)) < 0) {
        goto cleanup;
    }

    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        /* Ignore disabled slice */
        if ((soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                               efp_en_flds[slice_idx]) == 0) ||
            (soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                               efp_lk_en_flds[slice_idx]) == 0)) {
            continue;
        }

        efp_slice_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                                           _th_efp_slice_mode[slice_idx][0]);
        efp_slice_ipv6_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                                           _th_efp_slice_mode[slice_idx][1]);

        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices + slice_idx;
        key_match_type[0] = ~0;
        key_match_type[1] = ~0;

        /* Get KEY_MATCH_TYPE from the valid entries of EFP_TCAM */
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            efp_tcam_entry = soc_mem_table_idx_to_pointer(unit, EFP_TCAMm, 
                                                          efp_tcam_entry_t *,
                                 efp_tcam_buf, idx + slice_ent_cnt * slice_idx);

            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) != 0) {

                /* In Tomahawk, EFP_TCAM entry KEY field 
                 *  width is  (240 + 2b VALID) 
                 */

                if (key_match_type[0] == ~0) {
                    _field_extract((uint32 *)efp_tcam_entry, 236 + 2, 4,
                                   &key_match_type[0]);
                    continue;
                }

                if (key_match_type[1] == ~0) {
                    _field_extract((uint32 *)efp_tcam_entry, 236 + 2, 4,
                                   &key_match_type[1]);
                    if (key_match_type[0] == key_match_type[1]) {
                        key_match_type[1] = ~0;
                    } else {
                        /* break once possible two key match types are 
                         * extracted from the valid entries 
                         */
                        break;
                    }
                }
            }
        }

        if (key_match_type[0] == ~0 && !fc->l2warm) {
            continue;
        }

        /* Skip second part of slice pair */
        if (((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) &&
            (slice_idx % 2)) {
            continue;
        }

        /* Don't need to read selectors for expanded slice */
        if (expanded[slice_idx]) {
            continue;
        }

        fg = NULL;
        if (efp_slice_mode <= 5) { /* Valid values */
            /* If Level 2, retrieve the GIDs in this slice */
            if (fc->l2warm) {
                rv = _field_trx_scache_slice_group_recover(unit,
                                                           fc,
                                                           slice_idx,
                                                           NULL,
                                                           stage_fc,
                                                           0);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    fc->l2warm = 0;
                    goto cleanup;
                }
                if (rv == BCM_E_NOT_FOUND) {
                    rv = BCM_E_NONE;
                    continue;
                }
            }

            rv = _field_tr2_group_construct_alloc(unit, &fg);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            group_flags = 0;
            if (fc->l2warm) {
                /* Get stored group ID and QSET for Level 2 */
                rv = _field_group_info_retrieve(unit,
                                                -1,
                                                &gid,
                                                &priority,
                                                &group_flags,
                                                &qset,
                                                fc
                                                );
                sal_memcpy(&fg->qset, &qset, sizeof(bcm_field_qset_t));
            } else {
                if ((rv = _field_group_id_generate(unit, &gid)) == BCM_E_NONE) {
                    unsigned vmap, vslice;

                    for (priority = -1, vmap = 0; 
                         priority == -1 && vmap < _FP_VMAP_CNT; ++vmap) {
                        for (vslice = 0; vslice < COUNTOF(stage_fc->vmap[0]); 
                             ++vslice) {
                            if (stage_fc->vmap[vmap][vslice].vmap_key == 
                                                                  slice_idx) {
                                priority = 
                                          stage_fc->vmap[vmap][vslice].priority;
                                break;
                            }
                        }
                    }

                    if (priority == -1) {
                        rv = BCM_E_INTERNAL;
                    }
                }
            }
            if (BCM_FAILURE(rv)) {
                sal_free(fg);
                goto cleanup;
            }

            fg->gid      = gid;
            fg->priority = slice_idx;
            fg->stage_id = stage_fc->stage_id;

            switch (efp_slice_mode) {

                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2:
                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    parts_count = 1;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc, 
                                                                 stage_fc, fg, 
                                                              _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                                 stage_fc, fg,
                                                              _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3:

                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    parts_count = 1;

                    if (efp_slice_ipv6_mode) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                        default_key_1 = _BCM_FIELD_EFP_KEY2;
                    } else {
                        if ((key_match_type[0] == KEY_TYPE_IPv6_SINGLE) ||
                            (key_match_type[1] == KEY_TYPE_IPv6_SINGLE)) {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                            default_key_1 = _BCM_FIELD_EFP_KEY2;
                        } else {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                            default_key_1 = _BCM_FIELD_EFP_KEY2;
                        }
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc, 
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = default_key_1;
 
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY:

                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    parts_count = 1;

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg); 
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE:

                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    parts_count = 2;

                    if (efp_slice_ipv6_mode) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY3;
                        default_key_1 = _BCM_FIELD_EFP_KEY2;
                        default_key_2 = _BCM_FIELD_EFP_KEY3;
                    } else {

                        if ((key_match_type[0] == KEY_TYPE_IPv6_DOUBLE) ||
                            (key_match_type[1] == KEY_TYPE_IPv6_DOUBLE)) {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY3;
                            default_key_1 = _BCM_FIELD_EFP_KEY2;
                            default_key_2 = _BCM_FIELD_EFP_KEY3;
                        } else if ((key_match_type[0] == 
                                    KEY_TYPE_IPv4_L2_L3_DOUBLE) ||
                                   (key_match_type[0] == 
                                    KEY_TYPE_IPv4_L2_L3_DOUBLE) ) {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                            default_key_1 = _BCM_FIELD_EFP_KEY1;
                            default_key_2 = _BCM_FIELD_EFP_KEY4;               
                        } else {
                            fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY8;
                            fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                            default_key_1 = _BCM_FIELD_EFP_KEY8;
                            default_key_2 = _BCM_FIELD_EFP_KEY4;
                        }
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY8;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = default_key_1;
                    fg->sel_codes[1].fpf3 = default_key_2;
                    break;

                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY:

                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    parts_count = 2;

                    if ((key_match_type[0] == KEY_TYPE_HIGIG_DOUBLE) ||
                        (key_match_type[1] == KEY_TYPE_HIGIG_DOUBLE)) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY6;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        default_key_1 = _BCM_FIELD_EFP_KEY6;
                        default_key_2 = _BCM_FIELD_EFP_KEY4;
                    } else if ((key_match_type[0] == 
                                KEY_TYPE_LOOPBACK_DOUBLE) ||
                               (key_match_type[0] == 
                                KEY_TYPE_LOOPBACK_DOUBLE) ) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY7;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        default_key_1 = _BCM_FIELD_EFP_KEY7;
                        default_key_2 = _BCM_FIELD_EFP_KEY4;
                    } else {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        default_key_1 = _BCM_FIELD_EFP_KEY1;
                        default_key_2 = _BCM_FIELD_EFP_KEY4;
                    }

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }
                    
                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = default_key_1;
                    fg->sel_codes[1].fpf3 = default_key_2;
                    break;

                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6:

                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    parts_count = 2;

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;


                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_0);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY8;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    rv = _bcm_field_th_group_construct_quals_add(unit, fc,
                                                             stage_fc, fg,
                                                             _FP_ENTRY_TYPE_1);
                    if (BCM_FAILURE(rv)) {
                        sal_free(fg);
                        goto cleanup;
                    }

                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY2;
                    fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;

                    break;
                default: 
                    rv = BCM_E_INTERNAL;
                    sal_free(fg);
                    goto cleanup;
            }

            /* Associate slice(s) to group */
            fg->slices = stage_fc->slices + slice_idx;
            SOC_PBMP_CLEAR(all_pbmp);
            SOC_PBMP_ASSIGN(all_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(all_pbmp, PBMP_CMIC(unit));
            SOC_PBMP_ASSIGN(fg->pbmp, all_pbmp);
            BCM_PBMP_OR(fs->pbmp, fg->pbmp);

            /* Initialize group default ASET list. */
            rv = _field_group_default_aset_set(unit, fg);
            if (BCM_FAILURE(rv)) {
                sal_free(fg);
                goto cleanup;
            }

            fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
            fg->next = fc->groups;
            fc->groups = fg;

            /* Now go over the entries in this slice */
            prev_prio = -1;
            for (idx = 0; idx < slice_ent_cnt; idx++) {
                efp_tcam_entry = soc_mem_table_idx_to_pointer
                                     (unit, EFP_TCAMm, efp_tcam_entry_t *,
                                      efp_tcam_buf, idx +
                                      slice_ent_cnt * slice_idx);
                if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry,
                                              VALIDf) == 0) {
                    continue;
                }

                _field_extract((uint32 *)efp_tcam_entry, 236 + 2, 4,
                               &entry_type);

                /* Allocate memory for this entry */
                mem_sz = parts_count * sizeof (_field_entry_t);
                _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
                if (f_ent == NULL) {
                    rv = BCM_E_MEMORY;
                    goto cleanup;
                }
                sid = pid = -1;
                if (fc->l2warm) {
                    rv = _field_trx_entry_info_retrieve(unit,
                                                        &eid,
                                                        &prio,
                                                        fc,
                                                        0,
                                                        &prev_prio,
                                                        &sid,
                                                        &pid,
                                                        stage_fc,
                                                        &dvp_type
                                                        );
                    
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                } else {
                    _bcm_field_last_alloc_eid_incr();
                }

                switch(efp_slice_mode) {
                    case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2:
                        f_ent->efp_key_match_type = 1;
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3:
                        if (fg->sel_codes[0].fpf3 == _BCM_FIELD_EFP_KEY1) {
                            if (KEY_TYPE_IPv4_SINGLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            } 
                        } else {
                            if (KEY_TYPE_IPv6_SINGLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            } 
                        }
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3_ANY:
                        if (KEY_TYPE_IPv4_SINGLE == entry_type) {
                            f_ent->efp_key_match_type = 0;
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE:
                        if ((fg->sel_codes[0].fpf3 == _BCM_FIELD_EFP_KEY1) &&
                            (fg->sel_codes[1].fpf3 == _BCM_FIELD_EFP_KEY4)) {
                            if (KEY_TYPE_IPv4_L2_L3_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }                         
                        } else if ((fg->sel_codes[0].fpf3 == 
                                    _BCM_FIELD_EFP_KEY2) &&
                                   (fg->sel_codes[1].fpf3 == 
                                    _BCM_FIELD_EFP_KEY3)) {
                            if (KEY_TYPE_IPv6_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else {
                            f_ent->efp_key_match_type = 1;
                        } 
                        break;
                    case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY:
                        if ((fg->sel_codes[0].fpf3 == _BCM_FIELD_EFP_KEY6) &&
                            (fg->sel_codes[1].fpf3 == _BCM_FIELD_EFP_KEY4)) {
                            if (KEY_TYPE_HIGIG_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else if ((fg->sel_codes[0].fpf3 ==
                                    _BCM_FIELD_EFP_KEY7) &&
                                   (fg->sel_codes[1].fpf3 ==
                                    _BCM_FIELD_EFP_KEY4)) {
                            if (KEY_TYPE_LOOPBACK_DOUBLE == entry_type) {
                                f_ent->efp_key_match_type = 0;
                            } else {
                                f_ent->efp_key_match_type = 1;
                            }
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break; 
                    case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6:
                        if (KEY_TYPE_IPv4_IPv6_DOUBLE == entry_type) {
                            f_ent->efp_key_match_type = 0;
                        } else {
                            f_ent->efp_key_match_type = 1;
                        }
                        break;
                    default:
                        rv = BCM_E_INTERNAL;
                        sal_free(f_ent);
                        goto cleanup;
                }
                
                pri_tcam_idx = idx + slice_ent_cnt * slice_idx;
                for (i = 0; i < parts_count; i++) {
                    if (fc->l2warm) {
                        f_ent[i].eid = eid;
                        /* Set retrieved dvp_type */
                        f_ent[i].dvp_type = dvp_type;
                    } else {
                        f_ent[i].eid = _bcm_field_last_alloc_eid_get();
                    }
                    f_ent[i].group = fg;
                    if (fc->flags & _FP_COLOR_INDEPENDENT) {
                        f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                    }
                    rv = _bcm_field_tcam_part_to_entry_flags(i, fg->flags,
                                                             &f_ent[i].flags);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent,
                                                            pri_tcam_idx,
                                                            i, &part_index);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                             part_index,
                                                             &slice_number,
                                                (int *)&f_ent[i].slice_idx);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    f_ent[i].fs = stage_fc->slices + slice_number;
                    if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                        /* Decrement slice free entry count for primary
                           entries. */
                        f_ent[i].fs->free_count--;
                    }
                    /* Assign entry to a slice */
                    f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                    f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                    if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 3) {
                        f_ent[i].flags |= _FP_ENTRY_ENABLED;
                    }

                    /* Get the actions associated with this entry part */
                    efp_policy_entry = soc_mem_table_idx_to_pointer
                                               (unit, EFP_POLICY_TABLEm,
                                                efp_policy_table_entry_t *,
                                                efp_policy_buf, part_index);
                    rv = _field_tr2_actions_recover(unit,
                                                    EFP_POLICY_TABLEm,
                                                    (uint32 *) efp_policy_entry,
                                                    f_ent,
                                                    i,
                                                    sid,
                                                    pid
                                                    );
                }
                /* Add to the group */
                rv = _field_group_entry_add(unit, fg, f_ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent = NULL;
            }
        }

        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit,
                                           fc,
                                           slice_idx
                                           );
        }
    }

    /* Now go over the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        if (!expanded[slice_idx]) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        efp_slice_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval,
                                           _th_efp_slice_mode[slice_idx][0]);
        if (((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_ANY) ||
             (efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE_L3_V6)) &&
            (slice_idx % 2)) {
            continue;
        }
        /* Skip if slice has no valid entries */
        fs = stage_fc->slices + slice_idx;
        slice_ent_cnt = fs->entry_count;
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            if (_bcm_field_slice_offset_to_tcam_idx(unit,
                stage_fc, slice_idx, idx, &phys_tcam_idx) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            efp_tcam_entry = soc_mem_table_idx_to_pointer(unit, 
                             EFP_TCAMm, efp_tcam_entry_t *,
                             efp_tcam_buf, phys_tcam_idx);
            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) != 0) {
                break;
            }
        }
        if (idx == slice_ent_cnt) {
            continue;
        }
        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_scache_slice_group_recover(unit,
                     fc, slice_idx, &multigroup);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }
        /* Now find the master slice for this virtual group */
        vslice_idx = _field_physical_to_virtual(unit, slice_idx, stage_fc);
        if (vslice_idx < 0) {
            rv = BCM_E_INTERNAL;

            goto cleanup;
        }

        max = -1;
        for (i = 0; i < stage_fc->tcam_slices; i++) {
            if ((stage_fc->vmap[0][vslice_idx].virtual_group ==
                stage_fc->vmap[0][i].virtual_group) && (i != vslice_idx)) {
                if (i > max) {
                    max = i;
                }
            }
        }
        if (max < 0) {
            rv = BCM_E_INTERNAL;

            goto cleanup;
        }

        master_slice = stage_fc->vmap[0][max].vmap_key;
        /* See which group is in this slice - can be only one */
        fg = fc->groups;
        while (fg != NULL) {
            /* Check if group is in this slice */
            fs = &fg->slices[0];
            if (fs->slice_number == master_slice) {
                break;
            }
            fg = fg->next;
        }
        if (fg == NULL) {
            rv = BCM_E_INTERNAL;

            goto cleanup;
        }

        old_physical_slice = fs->slice_number;

        /* Set up the new physical slice parameters in Software */
        for(part_index = parts_count - 1; part_index >= 0; part_index--) {
            /* Get entry flags. */
            rv = _bcm_field_tcam_part_to_entry_flags(part_index, fg->flags, &entry_flags);
            BCM_IF_ERROR_RETURN(rv);
    
            /* Get slice id for entry part */
            rv = _bcm_field_tcam_part_to_slice_number(part_index, fg->flags, &slice_num);
            BCM_IF_ERROR_RETURN(rv);
            
            /* Get slice pointer. */
            fs = stage_fc->slices + slice_idx + slice_num;

            if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
                /* Set per slice configuration &  number of free entries in the slice.*/
                fs->free_count = fs->entry_count;
                if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                    fs->free_count >>= 1;
                }
                /* Set group flags in in slice.*/ 
                fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;
    
                /* Add slice to slices linked list . */
                stage_fc->slices[old_physical_slice + slice_num].next = fs;
                fs->prev = &stage_fc->slices[old_physical_slice + slice_num];
            }
        }

        prev_prio = -1;
        SOC_PBMP_CLEAR(all_pbmp);
        SOC_PBMP_ASSIGN(all_pbmp, PBMP_PORT_ALL(unit));
        SOC_PBMP_OR(all_pbmp, PBMP_CMIC(unit));
        SOC_PBMP_ASSIGN(fg->pbmp, all_pbmp);
        BCM_PBMP_OR(fs->pbmp, fg->pbmp);
        for (idx = 0; idx < slice_ent_cnt; idx++) {
            if (_bcm_field_slice_offset_to_tcam_idx(unit,
                stage_fc, slice_idx, idx, &phys_tcam_idx) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto cleanup;
            }
            efp_tcam_entry = soc_mem_table_idx_to_pointer(unit,
                             EFP_TCAMm, efp_tcam_entry_t *,
                             efp_tcam_buf, phys_tcam_idx);
            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 0) {
                continue;
            }

            /* Allocate memory for the entry */
            rv = _bcm_field_entry_tcam_parts_count(unit, fg->flags,
                                                   &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            sid = pid = -1;
            if (fc->l2warm) {
                rv = _field_trx_entry_info_retrieve(unit,
                         &eid, &prio, fc, multigroup, &prev_prio, &sid, &pid,
                         stage_fc, &dvp_type);
                
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr();
            }
            pri_tcam_idx = phys_tcam_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = eid;
                    /* Set retrieved dvp_type */
                    f_ent[i].dvp_type = dvp_type;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get();
                }
                f_ent[i].group = fg;

                if (fc->flags & _FP_COLOR_INDEPENDENT) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_tcam_part_to_entry_flags(i, fg->flags,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit,
                         f_ent, pri_tcam_idx, i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit,
                         stage_fc, part_index, &slice_number,
                         (int *)&f_ent[i].slice_idx);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices + slice_number;
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* Get the actions associated with this part of the entry */
                efp_policy_entry = soc_mem_table_idx_to_pointer(
                    unit, EFP_POLICY_TABLEm, efp_policy_table_entry_t *,
                    efp_policy_buf, part_index);
                rv = _field_tr2_actions_recover(unit,
                    EFP_POLICY_TABLEm, (uint32 *) efp_policy_entry,
                    f_ent, i, sid, pid);
            }
            /* Add to the group */
            if (fc->l2warm) {
                f_ent->prio = prio;
            } else {
                f_ent->prio = (vslice_idx << 10) | (slice_ent_cnt - idx);
            }
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit, fc, slice_idx);
        }
    }

    if (fc->l2warm) {
        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_EFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_EFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }

    }

    _field_tr2_stage_reinit_all_groups_cleanup(unit, fc, 
                                               _BCM_FIELD_STAGE_EGRESS,
                                               NULL);
cleanup:
    if (efp_tcam_buf) {
        soc_cm_sfree(unit, efp_tcam_buf);
    }
    if (efp_policy_buf) {
        soc_cm_sfree(unit, efp_policy_buf);
    }
    return rv;
}

               /*  END OF WARMBOOT ROUTINES */
#endif
/*
 * Function:
 *     _field_th_functions_init
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
void
_field_th_functions_init(_field_funct_t *functions)
{
    functions->fp_detach               = _bcm_field_tr_detach;
    functions->fp_group_install        = _bcm_field_th_group_install;
    functions->fp_selcodes_install     = _bcm_field_th_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_td2_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_td2_entry_move;
    functions->fp_selcode_get          = _bcm_field_th_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = NULL;
    functions->fp_tcam_policy_install  = _bcm_field_tr_entry_install;
    functions->fp_tcam_policy_reinstall = _bcm_field_tr_entry_reinstall;
    functions->fp_policer_install      = _bcm_field_th_policer_install;
    functions->fp_write_slice_map      = _bcm_field_td2_write_slice_map;
    functions->fp_qualify_ip_type      = _bcm_field_trx_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _bcm_field_trx_qualify_ip_type_get;
    functions->fp_action_support_check = _bcm_field_th_action_support_check;
    functions->fp_action_conflict_check = _bcm_field_trx_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_td_counter_get;
    functions->fp_counter_set          = _bcm_field_td_counter_set;
    functions->fp_stat_value_get       = _bcm_th_field_stat_value_get;
    functions->fp_stat_value_set       = _bcm_th_field_stat_value_set;
    functions->fp_stat_index_get       = _bcm_field_trx_stat_index_get;
    functions->fp_action_params_check  = _bcm_field_th_action_params_check;
    functions->fp_action_depends_check = _bcm_field_trx_action_depends_check;
    functions->fp_egress_key_match_type_set
        = _bcm_field_th_egress_key_match_type_set;
    functions->fp_external_entry_install  = NULL;
    functions->fp_external_entry_reinstall  = NULL;
    functions->fp_external_entry_remove   = NULL;
    functions->fp_external_entry_prio_set = NULL;
    functions->fp_data_qualifier_ethertype_add
        = _bcm_field_trx2_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete
        = _bcm_field_trx2_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add
        = _bcm_field_trx2_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete
        = _bcm_field_trx2_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add
        = _bcm_field_trx2_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete
        = _bcm_field_trx2_data_qualifier_packet_format_delete;
}
/*
 * Function:
 *     _bcm_field_th_init
 * Purpose:
 *     Perform initializations that are specific to BCM56960. This
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
_bcm_field_th_init(int unit, _field_control_t *fc)
{
    _field_stage_t *stage_fc;

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_fc = fc->stages;
    while (stage_fc) {

        if (!SAL_BOOT_BCMSIM && !SAL_BOOT_QUICKTURN) {
            /* Clear hardware table */
            BCM_IF_ERROR_RETURN(_bcm_field_tr_hw_clear(unit, stage_fc));
        }

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_th_qualifiers_init(unit, stage_fc));

        /* Goto next stage */
        stage_fc = stage_fc->next;
    }

    if (0 == SOC_WARM_BOOT(unit)) {

        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Enable meter refresh */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));

    }

    /* Initialize the function pointers */
    _field_th_functions_init(&fc->functions);

    /* Register with Counter ejection */
    _bcm_th_field_counter_config_set(unit);

    return (BCM_E_NONE);
}
#else /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
int _th_field_not_empty;
#endif  /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */


