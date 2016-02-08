/*
 * $Id: raptor_field.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
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
 * Purpose:     Field Processor installation functions.
 */
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/mirror.h>

#if defined(BCM_RAPTOR_SUPPORT) && defined(BCM_FIELD_SUPPORT)
 
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/raptor.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/mbcm.h>

STATIC void _field_raptor_functions_init(_field_funct_t *functions);
STATIC int _field_raptor_detach(int unit, _field_control_t *fc);
STATIC int _field_raptor_hw_clear(int unit, _field_stage_t *stage_fc);
STATIC int _field_raptor_udf_spec_set(int unit, 
           bcm_field_udf_spec_t *udf_spec, uint32 flags, uint32 offset);
STATIC int _field_raptor_udf_spec_get(int unit, 
           bcm_field_udf_spec_t *udf_spec, uint32 *flags, uint32 *offset);
STATIC int _field_raptor_entry_remove(int unit, _field_entry_t *f_ent,
                                      int tcam_idx);
STATIC int _field_raptor_tables_entry_clear(int unit, 
                                            _field_stage_id_t stage_id, 
                                            int tcam_idx);
STATIC int _field_raptor_group_install(int unit, _field_group_t *fg);
STATIC int _field_raptor_entry_install(int unit, _field_entry_t *f_ent, 
                                       int tcam_idx);
STATIC int _field_raptor_selcodes_install(int unit, _field_group_t *fg,
                                          uint8 slice_numb, 
                                          bcm_pbmp_t pbmp, int selcode_index);
STATIC int _field_raptor_policy_install(int unit, _field_entry_t *f_ent, 
                                        int tcam_idx);
STATIC int _field_raptor_ipbm_install(int unit, _field_entry_t *f_ent);
STATIC int _field_raptor_tcam_install(int unit, _field_entry_t *f_ent, 
                                      int tcam_idx);
STATIC int _field_raptor_tcam_get(int unit, _field_entry_t *f_ent, 
                                  fp_tcam_entry_t *t_entry);
STATIC int _field_raptor_action_get(int unit, _field_entry_t *f_ent, 
                                    int tcam_idx, _field_action_t *fa, 
                                    fp_policy_table_entry_t *p_entry);
STATIC int _field_raptor_meter_action_set(int unit, _field_entry_t *f_ent,
                                          fp_policy_table_entry_t *p_entry);
STATIC int _field_raptor_entry_move(int unit, _field_entry_t *f_ent, 
                                    int parts_count, int *tcam_idx_old, 
                                    int *tcam_idx_new);
STATIC int _field_raptor_entry_rule_copy(int unit, int old_index, 
                                         int new_index);
STATIC int _field_raptor_entry_rule_del(int unit, int index);
STATIC int _field_raptor_entry_ipbm_copy(int unit, _field_entry_t *f_ent, 
                                         int part, int new_tcam_idx);
STATIC int _field_raptor_ipbm_entry_set(int unit, int row, 
                                        int col, uint32 enable);
STATIC int _field_raptor_ipbm_entry_get(int unit, int row, 
                                        int col, uint32 *enable);
STATIC int _field_raptor_action_params_check(int unit,_field_entry_t *f_ent, 
                                             _field_action_t *fa);

/*
 * Function:
 *     _field_rp_ingress_qualifiers_init
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
_field_rp_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf4, 0, 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf4, 1, 0, 2);

    _key_fld_ = KEYf;
    offset = 0;

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
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


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 0, 2, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 0, 6, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 0, 9, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 0, 13, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf3, 1, 0, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 1, 0, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortTgid,
                 _bcmFieldSliceSelFpf3, 1, 0, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                 _bcmFieldSliceSelFpf3, 1, 7, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 1, 11, 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf3, 2, 0, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 2, 0, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortTgid,
                 _bcmFieldSliceSelFpf3, 2, 0, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                 _bcmFieldSliceSelFpf3, 2, 7, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 2, 11, 3);

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

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, 6, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, 6, 8, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf3, 7, 0, 16);

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
                 _bcmFieldSliceSelFpf2, 0xa, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 0xb, offset, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 0xb, offset, 64);


#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
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
    }
#endif /* BCM_RAVEN_SUPPORT */

    /* FPF1 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F2f);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 0, offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 1), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 0, (offset + 2), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 0, (offset + 3), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 5), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 0, (offset + 6), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 7), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf1, 0, (offset + 8), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 9), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 10), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 11), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 0, (offset + 13), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 0, (offset + 14), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 0, (offset + 15), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLookupStatus,
                 _bcmFieldSliceSelFpf1, 0, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMacGroup,
                 _bcmFieldSliceSelFpf1, 0, (offset + 16), 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 0, (offset + 23), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, (offset + 25), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 0, (offset + 27), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                 _bcmFieldSliceSelFpf1, 0, (offset + 31), 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                 _bcmFieldSliceSelFpf1, 1, offset, 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPortTgid,
                 _bcmFieldSliceSelFpf1, 1, offset, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf1, 1, offset, 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstModid,
                 _bcmFieldSliceSelFpf1, 1, (offset + 7), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                 _bcmFieldSliceSelFpf1, 1, (offset + 11), 11);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf1, 1, (offset + 11), 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPortTgid,
                 _bcmFieldSliceSelFpf1, 1, (offset + 11), 7);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcModid,
                 _bcmFieldSliceSelFpf1, 1, (offset + 18), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 1, (offset + 22), 3);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf1, 2, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf1, 2, (offset + 16), 16);

#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
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
    } else 
#endif /* BCM_RAVEN_SUPPORT */
        { 
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
#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
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
    } else 
#endif /* BCM_RAVEN_SUPPORT */
    {
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
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 7, (offset + 22), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 7, (offset + 25), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 7, (offset + 29), 3);

#if defined(BCM_RAVEN_SUPPORT) 
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {

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
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 31), 1);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf1, 0xd, offset , 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 8) , 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 23) , 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 24) , 8);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_rp_qualifiers_init
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
_field_rp_qualifiers_init(int unit, _field_stage_t *stage_fc)
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
          rv = _field_rp_ingress_qualifiers_init(unit, stage_fc);
          break;
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *    _bcm_field_raptor_init
 * Purpose:
 *     Perform initializations that are specific to Raptor. This
 *     includes initializing the FP field select bit offset tables 
 *     for FPF[1-4]
 * Parameters:
 *     unit - (IN) BCM device number
 *     fc   - (IN) FP control struct
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_raptor_init(int unit, _field_control_t *fc) 
{
    _field_stage_t *stage_p; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }
    
    stage_p = fc->stages;

    if (stage_p == NULL ||
        _BCM_FIELD_STAGE_INGRESS != stage_p->stage_id) {
        return (BCM_E_PARAM);
    }

    /* Clear the hardware tables */ 
    BCM_IF_ERROR_RETURN(_field_raptor_hw_clear(unit, stage_p));

    /* Initialize Qsets, bit offsets and width tables. */
    BCM_IF_ERROR_RETURN(_field_rp_qualifiers_init(unit, stage_p));

    if (!SOC_WARM_BOOT(unit)) {
        /* Set the Filter Enable flags in the port table */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Set meter refreshing enable */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
    }
 
    /* Initialize the function pointers */
    _field_raptor_functions_init(&fc->functions);

    return (BCM_E_NONE);
}

/*
 *     _field_raptor_hw_clear
 * Purpose:
 *     Clear hardware memory.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure.
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_raptor_hw_clear(int unit, _field_stage_t *stage_fc) 
{
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    COMPILER_REFERENCE(stage_fc);

    /* Clear udf match criteria registers. */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ipprotocol_delete_all(unit));
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ethertype_delete_all(unit));

    SOC_IF_ERROR_RETURN                                                
        (soc_mem_clear((unit), FP_UDF_OFFSETm, COPYNO_ALL, TRUE));     
    SOC_IF_ERROR_RETURN                                               
        (soc_mem_clear((unit), FP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN                                          
        (soc_mem_clear((unit), FP_RANGE_CHECKm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN                                          
        (soc_mem_clear((unit), FP_TCAMm, COPYNO_ALL, TRUE));          
    SOC_IF_ERROR_RETURN                                        
        (soc_mem_clear((unit), FP_POLICY_TABLEm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN                                       
        (soc_mem_clear((unit), FP_SLICE_ENTRY_PORT_SELm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN
        (soc_mem_clear((unit), FP_SLICE_MAPm, COPYNO_ALL, TRUE));
    SOC_IF_ERROR_RETURN
        (soc_mem_clear((unit), FP_METER_TABLEm, COPYNO_ALL, TRUE));  
    return (soc_mem_clear((unit), FP_COUNTER_TABLEm, COPYNO_ALL, TRUE));
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC soc_field_t _raptor_field_tbl[8][4] = {
                        {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f, SLICE0_F4f},
                        {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f, SLICE1_F4f},
                        {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f, SLICE2_F4f},
                        {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f, SLICE3_F4f},
                        {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f, SLICE4_F4f},
                        {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f, SLICE5_F4f},
                        {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f, SLICE6_F4f},
                        {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f, SLICE7_F4f}};



STATIC int
_field_raptor_slice_reinit(int unit, fp_port_field_sel_entry_t *pfs_entry, 
		           int slice_numb, _field_sel_t *selcodes)
{
    

    soc_field_t f1_field, f2_field, f3_field, f4_field;	
    f1_field = _raptor_field_tbl[slice_numb][0];
    selcodes->fpf1 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f1_field);

    f2_field = _raptor_field_tbl[slice_numb][1];
    selcodes->fpf2 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f2_field);

    f3_field = _raptor_field_tbl[slice_numb][2];
    selcodes->fpf3 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f3_field);

    f4_field = _raptor_field_tbl[slice_numb][3];
    selcodes->fpf4 = (int8) soc_FP_PORT_FIELD_SELm_field32_get
			            (unit, pfs_entry, f4_field);

    return BCM_E_NONE;    
}
#else
#define _field_raptor_slice_reinit (NULL)
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function:
 *     _field_raptor_qualify_ip_type
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
_field_raptor_qualify_ip_type(int unit, _field_entry_t *f_ent, 
                              bcm_field_IpType_t type) 
{
    _field_group_t   *fg;
    uint32  data = BCM_FIELD_IPTYPE_BAD,
            mask = BCM_FIELD_IPTYPE_BAD;

    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Confirm that IpType is in group's Qset. */
    fg = f_ent->group;
    if (!BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIpType)) {
        FP_ERR(("FP(unit %d) Error: IpType not in entry=%d Qset.\n", unit, f_ent->eid));
        return (BCM_E_PARAM);
    }

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

    if ((data == BCM_FIELD_IPTYPE_BAD) ||
        (mask == BCM_FIELD_IPTYPE_BAD)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_field_entry_qual_get(unit, f_ent->eid, 
                                   bcmFieldQualifyIpType, &f_ent));

    /* Add data & mask to entry. */
    f_ent->tcam.ip_type      = type;
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return (BCM_E_NONE);
}
#undef BCM_FIELD_IPTYPE_BAD 
/*
 * Function:
 *     _field_raptor_qualify_ip_type_get
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
_field_raptor_qualify_ip_type_get(int unit, _field_entry_t *f_ent, 
                                  bcm_field_IpType_t *type) 
{
    uint32 hw_data;          /* HW encoded qualifier data.  */
    uint32 hw_mask;          /* HW encoding qualifier mask. */
    int rv;                  /* Operation return status.    */

    /* Input parameters checks. */
    if ((NULL == f_ent) || (NULL == type)) {
        return (BCM_E_PARAM);
    }

    /* Read qualifier match value and mask. */
    rv = _bcm_field_entry_qualifier_uint32_get(unit, f_ent->eid,
                                               bcmFieldQualifyIpType,
                                               &hw_data, &hw_mask);
    BCM_IF_ERROR_RETURN(rv);

    /*
     * Devices with a separate PacketFormat and IpType use
     * different hardware encodings for IpType. 
     */
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
        *type  = bcmFieldIpTypeIpv4Any;
    } else if ((hw_data == 2) && (hw_mask == 3)) {
        *type = bcmFieldIpTypeIpv6;
    } else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_raptor_action_support_check
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
_field_raptor_action_support_check(int unit, _field_entry_t *f_ent,
                               bcm_field_action_t action, int *result)
{
    /* Input parameters check */
    if ((NULL == f_ent) || (NULL == result)) {
        return (BCM_E_PARAM);
    }

    switch (action) {
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
          case bcmFieldActionMirrorOverride:
          case bcmFieldActionMirrorIngress:
          case bcmFieldActionMirrorEgress:
          case bcmFieldActionL3ChangeVlan:
          case bcmFieldActionL3ChangeVlanCancel:
          case bcmFieldActionL3ChangeMacDa:
          case bcmFieldActionL3ChangeMacDaCancel:
          case bcmFieldActionL3Switch:
          case bcmFieldActionL3SwitchCancel:
          case bcmFieldActionAddClassTag:
          case bcmFieldActionEcnNew:
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
          case bcmFieldActionGpDrop:
          case bcmFieldActionGpDropCancel:
          case bcmFieldActionGpDropPrecedence:
          case bcmFieldActionGpCopyToCpu:
          case bcmFieldActionGpCopyToCpuCancel:
          case bcmFieldActionGpDscpNew:
          case bcmFieldActionUpdateCounter:
          case bcmFieldActionMeterConfig:
          case bcmFieldActionColorIndependent:
              *result = TRUE;
              break;
          case bcmFieldActionGpCosQNew:
          case bcmFieldActionGpPrioPktAndIntCopy:
          case bcmFieldActionGpPrioPktAndIntNew:
          case bcmFieldActionGpPrioPktAndIntTos:
          case bcmFieldActionGpPrioPktAndIntCancel:
          case bcmFieldActionGpPrioPktCopy:
          case bcmFieldActionGpPrioPktNew:
          case bcmFieldActionGpPrioPktTos:
          case bcmFieldActionGpPrioPktCancel:
          case bcmFieldActionGpPrioIntCopy:
          case bcmFieldActionGpPrioIntNew:
          case bcmFieldActionGpPrioIntTos:
          case bcmFieldActionGpPrioIntCancel:
          case bcmFieldActionRpCosQNew:
          case bcmFieldActionRpPrioPktAndIntCopy:
          case bcmFieldActionRpPrioPktAndIntNew:
          case bcmFieldActionRpPrioPktAndIntTos:
          case bcmFieldActionRpPrioPktAndIntCancel:
          case bcmFieldActionRpPrioPktCopy:
          case bcmFieldActionRpPrioPktNew:
          case bcmFieldActionRpPrioPktTos:
          case bcmFieldActionRpPrioPktCancel:
          case bcmFieldActionRpPrioIntCopy:
          case bcmFieldActionRpPrioIntNew:
          case bcmFieldActionRpPrioIntTos:
          case bcmFieldActionRpPrioIntCancel:
          case bcmFieldActionYpCosQNew:
          case bcmFieldActionYpPrioPktAndIntCopy:
          case bcmFieldActionYpPrioPktAndIntNew:
          case bcmFieldActionYpPrioPktAndIntTos:
          case bcmFieldActionYpPrioPktAndIntCancel:
          case bcmFieldActionYpPrioPktCopy:
          case bcmFieldActionYpPrioPktNew:
          case bcmFieldActionYpPrioPktTos:
          case bcmFieldActionYpPrioPktCancel:
          case bcmFieldActionYpPrioIntCopy:
          case bcmFieldActionYpPrioIntNew:
          case bcmFieldActionYpPrioIntTos:
          case bcmFieldActionYpPrioIntCancel:
              *result = (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) ? TRUE : FALSE;
              break;
          default:
              *result = FALSE;
        }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_action_conflict_check
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
_field_raptor_action_conflict_check(int unit, _field_entry_t *f_ent,
                                 bcm_field_action_t action1, 
                                 bcm_field_action_t action)
{
    /* Input parameters check */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Two identical actions are forbidden. */
    _FP_ACTIONS_CONFLICT(action1);

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
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
          break;
      case bcmFieldActionRpCosQNew:
      case bcmFieldActionRpPrioPktAndIntCopy:
      case bcmFieldActionRpPrioPktAndIntNew:
      case bcmFieldActionRpPrioPktAndIntTos:
      case bcmFieldActionRpPrioPktAndIntCancel:
      case bcmFieldActionRpPrioPktCopy:
      case bcmFieldActionRpPrioPktNew:
      case bcmFieldActionRpPrioPktTos:
      case bcmFieldActionRpPrioPktCancel:
      case bcmFieldActionRpPrioIntCopy:
      case bcmFieldActionRpPrioIntNew:
      case bcmFieldActionRpPrioIntTos:
      case bcmFieldActionRpPrioIntCancel:
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
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpPrioIntCancel);
          break;
      case bcmFieldActionYpCosQNew:
      case bcmFieldActionYpPrioPktAndIntCopy:
      case bcmFieldActionYpPrioPktAndIntNew:
      case bcmFieldActionYpPrioPktAndIntTos:
      case bcmFieldActionYpPrioPktAndIntCancel:
      case bcmFieldActionYpPrioPktCopy:
      case bcmFieldActionYpPrioPktNew:
      case bcmFieldActionYpPrioPktTos:
      case bcmFieldActionYpPrioPktCancel:
      case bcmFieldActionYpPrioIntCopy:
      case bcmFieldActionYpPrioIntNew:
      case bcmFieldActionYpPrioIntTos:
      case bcmFieldActionYpPrioIntCancel:
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
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpPrioIntCancel);
          break;
      case bcmFieldActionGpCosQNew:
      case bcmFieldActionGpPrioPktAndIntCopy:
      case bcmFieldActionGpPrioPktAndIntNew:
      case bcmFieldActionGpPrioPktAndIntTos:
      case bcmFieldActionGpPrioPktAndIntCancel:
      case bcmFieldActionGpPrioPktCopy:
      case bcmFieldActionGpPrioPktNew:
      case bcmFieldActionGpPrioPktTos:
      case bcmFieldActionGpPrioPktCancel:
      case bcmFieldActionGpPrioIntCopy:
      case bcmFieldActionGpPrioIntNew:
      case bcmFieldActionGpPrioIntTos:
      case bcmFieldActionGpPrioIntCancel:
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
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpPrioIntCancel);
          break;
      case bcmFieldActionTosNew:
      case bcmFieldActionTosCopy:
      case bcmFieldActionTosCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
          break;
      case bcmFieldActionDscpNew:
      case bcmFieldActionDscpCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionRpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          break;
      case bcmFieldActionYpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          break;
      case bcmFieldActionGpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
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
      case bcmFieldActionDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
          break;
      case bcmFieldActionRpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          break;
      case bcmFieldActionYpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          break;
      case bcmFieldActionGpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
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
 *     _field_raptor_functions_init
 * Purpose:
 *     Set up functions pointers 
 * Parameters:
 *     functions - pointers to device specific functions
 * Returns:
 *     nothing
 * Notes:
 *     Some function pointers point to functions in ../firebolt/field.c
 */
STATIC void
_field_raptor_functions_init(_field_funct_t *functions) 
{
    functions->fp_detach               = _field_raptor_detach;
    functions->fp_udf_spec_set         = _field_raptor_udf_spec_set;
    functions->fp_udf_spec_get         = _field_raptor_udf_spec_get;
    functions->fp_udf_write            = _bcm_field_fb_udf_write;
    functions->fp_udf_read             = _bcm_field_fb_udf_read;
    functions->fp_group_install        = _field_raptor_group_install;
    functions->fp_selcodes_install     = _field_raptor_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_fb_slice_clear;
    functions->fp_entry_remove         = _field_raptor_entry_remove;
    functions->fp_entry_move           = _field_raptor_entry_move;
    functions->fp_selcode_get          = _bcm_field_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = _field_raptor_tables_entry_clear;
    functions->fp_tcam_policy_install  = _field_raptor_entry_install;
    functions->fp_action_params_check  = _field_raptor_action_params_check;
    functions->fp_policer_install      = _bcm_field_fb_policer_install;
    functions->fp_slice_reinit	       = _field_raptor_slice_reinit; 
    functions->fp_write_slice_map      = _bcm_field_fb_write_slice_map;
    functions->fp_qualify_ip_type      = _field_raptor_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _field_raptor_qualify_ip_type_get;
    functions->fp_action_support_check = _field_raptor_action_support_check;
    functions->fp_action_conflict_check = _field_raptor_action_conflict_check;
    functions->fp_counter_get          = _bcm_field_fb_counter_get;
    functions->fp_counter_set          = _bcm_field_fb_counter_set;
    functions->fp_stat_index_get       = _bcm_field_fb_stat_index_get;
    functions->fp_egress_key_match_type_set = NULL;
    functions->fp_external_entry_install = NULL;
    functions->fp_external_entry_remove = NULL;
    functions->fp_external_entry_prio_set = NULL;
}

/*
 * Function:
 *     _field_raptor_detach
 * Purpose:
 *     Deallocates field tables.
 * Parameters:
 *     unit - (IN) BCM device number
 *     fc   - (IN) Control Structure
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_raptor_detach(int unit, _field_control_t *fc)
{
    _field_stage_t   *stage_p;  /* Stage iteration pointer. */

    if (NULL == fc) {
        return BCM_E_PARAM;
    }
    stage_p = fc->stages;

    if (NULL == stage_p ||
        _BCM_FIELD_STAGE_INGRESS != stage_p->stage_id) {
        return BCM_E_PARAM;
    } 

    /* Clear the hardware tables */
    BCM_IF_ERROR_RETURN(_field_raptor_hw_clear(unit, stage_p));

    /* Clear the Filter Enable flags in the port table */
    BCM_IF_ERROR_RETURN
        (_field_port_filter_enable_set(unit, fc, FALSE));

    /* Disable field meter refreshing */
    return (_field_meter_refresh_enable_set(unit, fc, FALSE));
}


/*
 * Function:
 *     _field_raptor_udf_spec_set
 * Purpose:
 *     Write user defined field configuration to udf_spec_t structure
 * Parameters:
 * Returns:
 *     BCM_E_PARAM - flags or offset out of range
 *     BCM_E_NONE
 */
STATIC int
_field_raptor_udf_spec_set(int unit, bcm_field_udf_spec_t *udf_spec, 
                           uint32 flags, uint32 offset)
{
    uint8               pkt_fmt_idx = 0u;     /* index into udf_spec */

    if (offset > 31) {
        FP_ERR(("FP(unit %d) Error: offset=%d out-of-range.\n", unit, offset));
        return BCM_E_PARAM;
    }
    if (offset >= 28) {
        if (soc_feature(unit, soc_feature_field_udf_higig2) && 
            !(flags & BCM_FIELD_USER_HIGIG2)) {
            FP_ERR(("FP(unit %d) Error: This is the offset for UDF-HiGig2 data.", 
                    "BCM_FIELD_USER_HIGIG2 flag should be set.\n" , unit));
    	    return BCM_E_PARAM;
        }
        if ((flags & BCM_FIELD_USER_HIGIG2) &&
            !soc_feature(unit, soc_feature_field_udf_higig2)) {
            FP_ERR(("FP(unit %d) Error: device does not support UDF HiGig2 data.\n", unit));
    	    return BCM_E_PARAM;
        }
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
 *     _field_raptor_udf_spec_get
 * Purpose:
 *     Get user defined field configuration entry from udf_spec_t structure
 * Parameters:
 * Returns:
 *     BCM_E_PARAM - flags or offset out of range
 *     BCM_E_NONE
 */
STATIC int
_field_raptor_udf_spec_get(int unit, bcm_field_udf_spec_t *udf_spec, 
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
 *     _field_raptor_group_install
 * Purpose:
 *     Writes the Group's mode and field select codes into hardware. This
 *     should be called both at group creation time and any time the select
 *     codes change (i.e. bcm_field_group_set calls).
 * Parameters:
 *     unit  - BCM device number
 *     fg    - group to install
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 * Note:
 *     Unit lock should be held by calling function.
 *     Uses function common to firebolt: _bcm_field_fb_mode_install
 */
STATIC int
_field_raptor_group_install(int unit, _field_group_t *fg) 
{
    assert(fg != NULL);

    if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
        return (BCM_E_INTERNAL);
    } 

    /* Write the field select codes*/
    if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        BCM_IF_ERROR_RETURN
            (_field_raptor_selcodes_install(unit, fg, 
                                            fg->slices[2].slice_number,
                                            fg->pbmp, 2));
    }

    if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) || 
        (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE)) {
        BCM_IF_ERROR_RETURN
            (_field_raptor_selcodes_install(unit, fg, 
                                            fg->slices[1].slice_number,
                                            fg->pbmp, 1));
    }
    BCM_IF_ERROR_RETURN 
        (_field_raptor_selcodes_install(unit, fg, 
                                        fg->slices[0].slice_number,
                                        fg->pbmp, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_selcodes_install
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_selcodes_install(int unit, _field_group_t *fg,
                               uint8 slice_numb, bcm_pbmp_t pbmp,
                               int selcode_index)
{
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
#if defined(BCM_RAVEN_SUPPORT)     
    static int                  field_tbl[16][4] = {
        {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f, SLICE0_F4f},
        {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f, SLICE1_F4f},
        {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f, SLICE2_F4f},
        {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f, SLICE3_F4f},
        {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f, SLICE4_F4f},
        {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f, SLICE5_F4f},
        {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f, SLICE6_F4f},
        {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f, SLICE7_F4f},
        {SLICE8_F1f, SLICE8_F2f, SLICE8_F3f, SLICE8_F4f},
        {SLICE9_F1f, SLICE9_F2f, SLICE9_F3f, SLICE9_F4f},
        {SLICE10_F1f, SLICE10_F2f, SLICE10_F3f, SLICE10_F4f},
        {SLICE11_F1f, SLICE11_F2f, SLICE11_F3f, SLICE11_F4f},
        {SLICE12_F1f, SLICE12_F2f, SLICE12_F3f, SLICE12_F4f},
        {SLICE13_F1f, SLICE13_F2f, SLICE13_F3f, SLICE13_F4f},
        {SLICE14_F1f, SLICE14_F2f, SLICE14_F3f, SLICE14_F4f},
        {SLICE15_F1f, SLICE15_F2f, SLICE15_F3f, SLICE15_F4f}};
#else  /* BCM_RAVEN_SUPPORT */
    static int                  field_tbl[8][4] = {
        {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f, SLICE0_F4f},
        {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f, SLICE1_F4f},
        {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f, SLICE2_F4f},
        {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f, SLICE3_F4f},
        {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f, SLICE4_F4f},
        {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f, SLICE5_F4f},
        {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f, SLICE6_F4f},
        {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f, SLICE7_F4f}};
#endif /* BCM_RAVEN_SUPPORT */
    soc_field_t                 f1_field, f2_field, f3_field, f4_field;
    _field_sel_t               *sel = NULL;

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
        /* Write the group mode into appropriate slices for 
         * wide-mode devices. 
         */
        if (soc_feature(unit, soc_feature_field_wide)) {
            BCM_IF_ERROR_RETURN(_bcm_field_fb_mode_set(unit, slice_numb,
                                                       fg, fg->flags));
        }  

        sel = &fg->sel_codes[selcode_index];
 
        sal_memcpy(&pfs_entry, soc_mem_entry_null(unit, FP_PORT_FIELD_SELm), 
                   sizeof(pfs_entry));

        /* Determine which 4 fields will be modified */
        f1_field = field_tbl[slice_numb][0];
        f2_field = field_tbl[slice_numb][1];
        f3_field = field_tbl[slice_numb][2];
        f4_field = field_tbl[slice_numb][3];

        /* Iterate over all ports */
        PBMP_ITER(pbmp, port) {
            /* Read Port's current entry in FP_PORT_FIELD_SEL table */
            SOC_IF_ERROR_RETURN(READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY,
                                                        port, &pfs_entry));

            /* modify 0-4 fields depending on state of SELCODE_DONT_CARE */
            if (sel->fpf1 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f1_field,
                                                   sel->fpf1);
            }
            if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f2_field,
                                                   sel->fpf2);
            }
            if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f3_field,
                                                   sel->fpf3);
            }
            if (sel->fpf4 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, f4_field,
                                                   sel->fpf4);
            }

            /* Write each port's new entry */
            SOC_IF_ERROR_RETURN(WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                                         port, &pfs_entry));

        }
    } else {
        return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_entry_install
 * Purpose:
 *     Write entry into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - common index of various tables
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_entry_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_TCAMm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_TCAMm));
    
    /* Next Write the Actions */
    BCM_IF_ERROR_RETURN(
        _field_raptor_policy_install(unit, f_ent, tcam_idx));

    /* Next write the IPBM */
    BCM_IF_ERROR_RETURN(
        _field_raptor_ipbm_install(unit, f_ent));

    /* Finally Write the TCAM, Valid bits = b11 */
    BCM_IF_ERROR_RETURN(
        _field_raptor_tcam_install(unit, f_ent, tcam_idx));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_ipbm_entry_set
 * Purpose:
 *     Set a bit in IPBM in the chip's memory.
 *     Helper function for ipbm related functions
 * Parameters:
 *     unit     -   BCM Unit
 *     row      -   row in IPBM: The port/slice #
 *     col      -   col. in IPBM: The entry #
 *     enable   -   value (bit) to be written
 * Returns:
 *     BCM_E_PARAM - incorrect col.
 *     BEM_E_NONE
 * Note:
 */
STATIC int
_field_raptor_ipbm_entry_set(int unit, int row, 
                             int col, uint32 enable)
{
    fp_slice_entry_port_sel_entry_t ipbm_entry;
    soc_field_t all_fields[4] = {ENTRIES_PER_PORT_31_0f,
                                 ENTRIES_PER_PORT_63_32f,
                                 ENTRIES_PER_PORT_95_64f,
                                 ENTRIES_PER_PORT_127_96f};
    soc_field_t field;
    uint32 field_offsets[4] = {0, 32, 64, 96};
    uint32 index;
    uint32 curr_value, new_value;
    
    SOC_IF_ERROR_RETURN(READ_FP_SLICE_ENTRY_PORT_SELm
                        (unit, MEM_BLOCK_ANY, row, &ipbm_entry));
  
    field = all_fields[col/32];
    index = col - field_offsets[col/32];

    curr_value = soc_FP_SLICE_ENTRY_PORT_SELm_field32_get
                 (unit, &ipbm_entry, field);
    
    new_value = enable ?
                (curr_value | (1 << index)) :
                (curr_value & ~(1 << index));

    /* This is an optimization: writing only if the bit is different */
    if (new_value != curr_value) {
        soc_FP_SLICE_ENTRY_PORT_SELm_field32_set
                  (unit, &ipbm_entry, field, new_value);

        SOC_IF_ERROR_RETURN(WRITE_FP_SLICE_ENTRY_PORT_SELm
                            (unit, MEM_BLOCK_ALL, row, &ipbm_entry));
    }

    return BCM_E_NONE;
}
    
/*
 * Function:
 *     _field_raptor_ipbm_entry_get
 * Purpose:
 *     Get a bit in IPBM in the chip's memory.
 *     Helper function for ipbm related functions
 * Parameters:
 *     unit     -   BCM Unit
 *     row      -   row in IPBM: The port/slice #
 *     col      -   col. in IPBM: The entry #
 *     enable   -   (OUT) value (bit) read
 * Returns:
 *     BCM_E_PARAM - incorrect col.
 *     BEM_E_NONE
 * Note:
 */
STATIC int
_field_raptor_ipbm_entry_get(int unit, int row, 
                             int col, uint32 *enable)
{
    fp_slice_entry_port_sel_entry_t ipbm_entry;
    soc_field_t all_fields[4] = {ENTRIES_PER_PORT_31_0f,
                                 ENTRIES_PER_PORT_63_32f,
                                 ENTRIES_PER_PORT_95_64f,
                                 ENTRIES_PER_PORT_127_96f};
    soc_field_t field;
    uint32 field_offsets[4] = {0, 32, 64, 96};
    uint32 index, result;
    
    SOC_IF_ERROR_RETURN(READ_FP_SLICE_ENTRY_PORT_SELm
                        (unit, MEM_BLOCK_ANY, row, &ipbm_entry));
  
    field = all_fields[col/32];
    index = col - field_offsets[col/32];

    result = soc_FP_SLICE_ENTRY_PORT_SELm_field32_get
             (unit, &ipbm_entry, field);

    *enable = (result >> index) & 0x1; /* Return the bit at col. */
    
    return BCM_E_NONE;
}
    
/*
 * Function:
 *     _field_raptor_ipbm_install
 * Purpose:
 *     Write IPBM into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location
 * Returns:
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_ipbm_install(int unit, _field_entry_t *f_ent)
{
    bcm_port_t p;
    bcm_pbmp_t pbmp, pbmp_mask;
    uint32 result;
    int    ipbm_index;
    _field_stage_t *stage_fc;
    bcm_port_config_t  port_config;
    
    pbmp = f_ent->pbmp.data;
    pbmp_mask = f_ent->pbmp.mask;

    /* Read device port configuration. */ 
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc));
    
    PBMP_ITER(port_config.all, p) {
        if (BCM_PBMP_MEMBER(pbmp_mask, p)) {
            if (BCM_PBMP_MEMBER(pbmp, p)) {
                result = 1;
            } else {
                result = 0;
            }
        } else {
            result = 1;
        }

        /* set result at the appropriate location */
        ipbm_index = _BCM_RP_FIELD_IPBM_INDEX(unit, p, f_ent->fs->slice_number);
        BCM_IF_ERROR_RETURN (_field_raptor_ipbm_entry_set(unit, ipbm_index, 
                                                          f_ent->slice_idx, 
                                                          result));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_tcam_install
 * Purpose:
 *     Write rules into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 * Returns:
 *     BCM_E_XXX        On read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_tcam_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    fp_tcam_entry_t t_entry;

    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_TCAMm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_TCAMm));
    
    /* Start with an empty TCAM entry */
    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));

    /* Extract the qualifier info from the entry structure. */
    BCM_IF_ERROR_RETURN(
        _field_raptor_tcam_get(unit, f_ent, &t_entry));

    /* Write the TCAM Table */
    SOC_IF_ERROR_RETURN
        (WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, tcam_idx, &t_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_tcam_get
 * Purpose:
 *    Get the rules to be written into tcam.
 * Parameters:
 *     unit      -  BCM Unit
 *     f_ent     -  entry structure to get tcam info from.
 *     t_entry - (OUT) TCAM entry
 * Returns:
 *     BCM_E_NONE  - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_tcam_get(int unit, _field_entry_t *f_ent, fp_tcam_entry_t *t_entry)
{
    _field_tcam_t       *tcam = NULL;

    assert(f_ent     != NULL);
    assert(t_entry != NULL);

    tcam = &f_ent->tcam;

    soc_mem_field_set(unit, FP_TCAMm, (uint32 *)t_entry, KEYf,  tcam->key);
    soc_mem_field_set(unit, FP_TCAMm, (uint32 *)t_entry, MASKf, tcam->mask);

    soc_FP_TCAMm_field32_set(unit, t_entry, F4f, tcam->f4);
    soc_FP_TCAMm_field32_set(unit, t_entry, F4_MASKf, tcam->f4_mask);

    /* Qualify on IP_TYPE */
    if (BCM_FIELD_QSET_TEST(f_ent->group->qset,
                            bcmFieldQualifyIpType)) {
        int data, mask;
        switch (tcam->ip_type) {
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
            case bcmFieldIpTypeIpv6Not:
            default:
                /* Should not come here */
                data = 0x0;
                mask = 0x0;
                break;
        }
        soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPEf, data);
        soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPE_MASKf, mask);
    } else {
       soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPEf, 0);
       soc_FP_TCAMm_field32_set(unit, t_entry, IP_TYPE_MASKf, 0);
    }

    /* Qualify on HiGig packets. */
    if (BCM_FIELD_QSET_TEST(f_ent->group->qset,
                            bcmFieldQualifyHiGig)) {
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKTf, tcam->higig ? 1 : 0);
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKT_MASKf, tcam->higig_mask ? 1 : 0);
    } else {
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKTf, 0);
        soc_FP_TCAMm_field32_set(unit, t_entry, HIGIG_PKT_MASKf, 0);
    }
#if defined(BCM_RAVEN_SUPPORT)
    if (soc_mem_field_valid(unit, FP_TCAMm, RESERVED_MASKf)) {
        soc_mem_field32_set(unit, FP_TCAMm, t_entry, RESERVED_MASKf, 0);
    }
    if (soc_mem_field_valid(unit, FP_TCAMm, RESERVED_KEYf)) {
        soc_mem_field32_set(unit, FP_TCAMm, t_entry, RESERVED_KEYf, 0);
    }
#endif /* BCM_RAVEN_SUPPORT */
    /* Set the Valid bits */
    soc_FP_TCAMm_field32_set(unit, t_entry, VALIDf, 3);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_stat_action_set
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
_field_raptor_stat_action_set(int unit, _field_entry_t *f_ent,
                              fp_policy_table_entry_t *p_entry)
{
    _field_stat_t  *f_st;  /* Field policer descriptor. */  
    int mode;              /* Counter hw mode.          */                    
    int idx;               /* Counter index.            */

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == p_entry))  {
        return (BCM_E_PARAM);
    }

    /* Disable counting if counter was not attached to the entry. */
    if ((0 == (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_STAT_IN_SECONDARY_SLICE))) {
        mode = 0;
        idx = 0;
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

    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, COUNTER_INDEXf, idx);
    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, COUNTER_MODEf, mode);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_raptor_policy_install
 * Purpose:
 *     Write policy into the chip's memory. 
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 * Returns:
 *     BCM_E_XXX        On read/write errors
 *     BCM_E_NONE
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_policy_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    fp_policy_table_entry_t p_entry;
    _field_action_t              *fa = NULL;

    assert(f_ent != NULL);
    assert(tcam_idx >= soc_mem_index_min(unit, FP_POLICY_TABLEm));
    assert(tcam_idx <= soc_mem_index_max(unit, FP_POLICY_TABLEm));
    
    /* Start with an empty POLICY entry */
    sal_memcpy(&p_entry, soc_mem_entry_null(unit, FP_POLICY_TABLEm), 
               sizeof(p_entry));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
        if ((f_ent->flags & _FP_ENTRY_POLICER_IN_SECONDARY_SLICE) && 
            (_FP_ACTION_IS_COLOR_BASED(fa->action))) {
            continue;
        }
        BCM_IF_ERROR_RETURN(
            _field_raptor_action_get(unit, f_ent, tcam_idx, fa, &p_entry));
    }

    /* Handle color dependence/independence */
    if (soc_feature(unit, soc_feature_field_color_indep)) {
        if (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) {
            soc_mem_field32_set(unit, FP_POLICY_TABLEm, &p_entry,
                                GREEN_TO_PIDf, 1);
        } else {
            soc_mem_field32_set(unit, FP_POLICY_TABLEm, &p_entry,
                                GREEN_TO_PIDf, 0);
        }
    }

    /* If the entry has a meter, get its index. */
    if (f_ent->policer[0].flags & _FP_POLICER_INSTALLED) {
        BCM_IF_ERROR_RETURN(_field_raptor_meter_action_set(unit, f_ent, 
                                                           &p_entry));
    }
   
    /* Install statistics entity. */
    BCM_IF_ERROR_RETURN(_field_raptor_stat_action_set(unit, f_ent, &p_entry));

    /* Write the POLICY Table */
    SOC_IF_ERROR_RETURN
        (WRITE_FP_POLICY_TABLEm(unit, MEM_BLOCK_ALL, tcam_idx, &p_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_action_get
 * Purpose:
 *     Get the actions to be written
 * Parameters:
 *     unit     - BCM device number
 *     tcam_idx - index into TCAM
 *     fa       - field action 
 *     p_entry  - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
STATIC int
_field_raptor_action_get(int unit, _field_entry_t *f_ent, int tcam_idx, 
                         _field_action_t *fa, fp_policy_table_entry_t *p_entry)
{
    uint32    reg_val;
    uint32    redir_field;
    uint8     raptor_actions_enable = TRUE;    

    FP_VVERB(("FP(unit %d) vverb: BEGIN _field_raptor_action_get(eid=%d, tcam_idx=0x%x, ",
              unit, f_ent->eid, tcam_idx));

#if defined(BCM_RAVEN_SUPPORT) 
    /* First deal with Raven-only actions */
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        raptor_actions_enable = FALSE;
        switch (fa->action) {
          case bcmFieldActionGpCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionGpPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionGpPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionGpPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1); 
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionGpPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionGpPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionGpPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionGpPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionGpPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 15);
              break;
          case bcmFieldActionYpCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionYpPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionYpPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionYpPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1); 
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionYpPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionYpPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionYpPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionYpPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionYpPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_PRIORITYf, 15);
              break;
          case bcmFieldActionRpCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionRpPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionRpPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionRpPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1); 
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionRpPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionRpPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionRpPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionRpPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionRpPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PRI_MODEf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_PRIORITYf, 15);
              break;
          default:
              raptor_actions_enable = TRUE; /* Check for Raptor  / 
                                               Raven common actions. */
              break;
        }
    }
#endif  /* BCM_RAVEN_SUPPORT */
    if (raptor_actions_enable) {
        switch (fa->action) {
          case bcmFieldActionCosQNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionCosQCpuNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktAndIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 4);
              break;
          case bcmFieldActionPrioPktAndIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 5);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktAndIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 6);
              break;
          case bcmFieldActionPrioPktAndIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 7);
              break;
          case bcmFieldActionPrioPktCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 8);
              break;
          case bcmFieldActionPrioPktNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 9);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 10);
              break;
          case bcmFieldActionPrioPktCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 11);
              break;
          case bcmFieldActionPrioIntCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 12);
              break;
          case bcmFieldActionPrioIntNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 13);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioIntTos:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 14);
              break;
          case bcmFieldActionPrioIntCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_PRIORITYf, 15);
              break;
          case bcmFieldActionTosNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWDSCP_TOSf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DSCPf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionTosCopy:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 2);
              break;
          case bcmFieldActionDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWDSCP_TOSf, fa->param[0]);
              break;
          case bcmFieldActionTosCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 4);
              break;
          case bcmFieldActionDscpCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 4);
              break;
          case bcmFieldActionCopyToCpu:
              /* if the action is CopyToCpu, and param0 is nonzero, which means the user
               * wants to install the value of param1 as the MATCHED_RULEf, we want to
               * make sure that param1 can fit into 8 bits
               */

              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 1);
              if(fa->param[0] != 0) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionSwitchToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 3);
              break;
          case bcmFieldActionRedirect: /* param0 = modid, param1 = port/tgid */
              /* modid = 4 bits, port/tgid = 7 bits */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 1);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, (fa->param[0] << 7) | fa->param[1]);
              } else {
                  redir_field = PolicyGet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf);
                  redir_field &= (0xfffff800);  /* Zero mod/port[11:0] bits */
                  redir_field |= (fa->param[0] << 7) | fa->param[1];
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, redir_field);       
              }
              break;
          case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
              /* Trunk indicator is bit 6 (7th from right) */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 1);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, (fa->param[0] & 0x3f) | 0x40);
              } else {
                  redir_field = PolicyGet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf);
                  redir_field &= (0xffffff80);  /* Zero trunk [6:0] bits */
                  redir_field |= (fa->param[0] & 0x3f) | 0x40;
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, redir_field);       
              }
              break;
          case bcmFieldActionRedirectCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 2);
              break;
          case bcmFieldActionRedirectPbmp:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 3);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, fa->param[0]);
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_HIf, fa->param[1]);
              } else {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionEgressMask:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, PACKET_REDIRECTIONf, 4);
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_LOf, fa->param[0]);
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTION_HIf, fa->param[1]);
              } else {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 1);
              break;
          case bcmFieldActionDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 2);
              break;
          case bcmFieldActionMirrorOverride:
              if (soc_feature(unit, soc_feature_field_mirror_ovr)) {
                  PolicySet(unit, FP_POLICY_TABLEm, p_entry, MIRROR_OVERRIDEf, 1);
              } else {
                  return BCM_E_PARAM;
              }
              break;
          case bcmFieldActionMirrorIngress:    /* param0=modid, param1=port/tgid */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf, 
                        PolicyGet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf) | 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, IM_MTP_INDEXf, fa->hw_index);
              if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
                  SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &reg_val));
                  soc_reg_field_set(unit, ING_CONFIGr, &reg_val,
                                    ENABLE_FP_FOR_MIRROR_PKTSf, 1 );
                  SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, reg_val));
              }
              break;
          case bcmFieldActionMirrorEgress:     /* param0=modid, param1=port/tgid */
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf, 
                        PolicyGet(unit, FP_POLICY_TABLEm, p_entry, MIRRORf) | 2);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, EM_MTP_INDEXf, fa->hw_index);
              if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
                  SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &reg_val));
                  soc_reg_field_set(unit, ING_CONFIGr, &reg_val,
                                    ENABLE_FP_FOR_MIRROR_PKTSf, 1 );
                  SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, reg_val));
              }
              break;
#ifdef INCLUDE_L3
          case bcmFieldActionL3ChangeVlan:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 1);
              BCM_IF_ERROR_RETURN
                  (_bcm_field_policy_set_l3_info(unit, FP_POLICY_TABLEm, 
                                                 fa->param[0], (uint32 *)p_entry)); 
              break;
          case bcmFieldActionL3ChangeVlanCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 2);
              break;
          case bcmFieldActionL3ChangeMacDa:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 4);
              BCM_IF_ERROR_RETURN
                  (_bcm_field_policy_set_l3_info(unit, FP_POLICY_TABLEm, 
                                                 fa->param[0], (uint32 *)p_entry)); 
              break;
          case bcmFieldActionL3ChangeMacDaCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 5);
              break;
          case bcmFieldActionL3Switch:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 6);
              BCM_IF_ERROR_RETURN
                  (_bcm_field_policy_set_l3_info(unit, FP_POLICY_TABLEm, 
                                                 fa->param[0], (uint32 *)p_entry)); 
              break;
          case bcmFieldActionL3SwitchCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 7);
              break;
#endif /* INCLUDE_L3 */
          case bcmFieldActionAddClassTag:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, L3SW_CHANGE_MACDA_OR_VLANf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CLASSIFICATION_TAGf, fa->param[0]);
              break;
          case bcmFieldActionEcnNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, ECN_CNGf, 0x1);
              /* fall through. */
          case bcmFieldActionDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROP_PRECEDENCEf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROP_PRECEDENCEf, fa->param[0]);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 1);
              break;
          case bcmFieldActionRpDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROPf, 2);
              break;
          case bcmFieldActionRpDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpCopyToCpu:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 1);
              break;
          case bcmFieldActionRpCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionRpDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionYpDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 1);
              break;
          case bcmFieldActionYpDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROPf, 2);
              break;
          case bcmFieldActionYpDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionYpCopyToCpu:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 1);
              break;
          case bcmFieldActionYpCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionYpDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_CHANGE_DSCPf, 1);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, YP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionGpDrop:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 1);
              break;
          case bcmFieldActionGpDropCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROPf, 2);
              break;
          case bcmFieldActionGpDropPrecedence:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionGpCopyToCpu:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 1);
              break;
          case bcmFieldActionGpCopyToCpuCancel:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, COPY_TO_CPUf, 2);
              break;
          case bcmFieldActionGpDscpNew:
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, CHANGE_DSCP_TOSf, 3);
              PolicySet(unit, FP_POLICY_TABLEm, p_entry, NEWDSCP_TOSf, fa->param[0]);
              break;
          case bcmFieldActionUpdateCounter:
          case bcmFieldActionMeterConfig:
              /*
               * This action is handled by the calling routine, not by this
               * routine.
               */
              return (BCM_E_INTERNAL);
          default:
              FP_ERR(("FP(unit %d) Error: Unknown action\n", unit, &fa->action));
              return BCM_E_PARAM;
        }
    }

    fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */

    FP_VVERB(("FP(unit %d) vverb: END _field_raptor_action_get()\n", unit));
    return BCM_E_NONE;

}

/*
 * Function:
 *     _field_raptor_action_params_check
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
_field_raptor_action_params_check(int unit,_field_entry_t *f_ent, 
                                  _field_action_t *fa)
{
#if defined(INCLUDE_L3)
    uint32 flags;            /* L3 forwarding flags.    */ 
    int nh_ecmp_id;          /* Next hop/Ecmp group id. */
#endif /* INCLUDE_L3 */
    int rv;                  /* Operation return value. */ 
    int raptor_actions_enable = TRUE;

#if defined(BCM_RAVEN_SUPPORT) 
    /* First deal with Raven-only actions */
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        raptor_actions_enable = FALSE;
        switch (fa->action) {
          case bcmFieldActionGpCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionGpPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionYpPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionRpPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_NEWPRIf, fa->param[0]);
              break;
          default:
              raptor_actions_enable = TRUE; /* Check for Raptor  / 
                                               Raven common actions. */
              break;
        }
    }
#endif  /* BCM_RAVEN_SUPPORT */
    if (raptor_actions_enable) {
        switch (fa->action) {
          case bcmFieldActionCosQNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionCosQCpuNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktAndIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioPktNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionPrioIntNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWPRIf, fa->param[0]);
              break;
          case bcmFieldActionTosNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWDSCP_TOSf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DSCPf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWDSCP_TOSf, fa->param[0]);
              break;
          case bcmFieldActionCopyToCpu:
              /* if the action is CopyToCpu, and param0 is nonzero, which means the user
               * wants to install the value of param1 as the MATCHED_RULEf, we want to
               * make sure that param1 can fit into 8 bits
               */
              if ((fa->param[0] != 0) && (fa->param[1] >= (1 << 8))) {
                  return BCM_E_PARAM;
              }

              if(fa->param[0] != 0) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, MATCHED_RULEf, fa->param[1]);
              }
              break;
          case bcmFieldActionRedirect: 
              /* param0 = modid, param1 = port/tgid */
              /* modid = 4 bits, port/tgid = 7 bits */
              rv = _bcm_field_action_dest_check(unit, fa);
              BCM_IF_ERROR_RETURN(rv);
              if (fa->param[0] > 0xf || fa->param[1] > 0x7f) {
                  return BCM_E_PARAM;
              }
              break;
          case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
              if (fa->param[0] > 0x3f) {
                  return BCM_E_PARAM;
              }
              break;
          case bcmFieldActionRedirectPbmp:
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_LOf, fa->param[0]);
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_HIf, fa->param[1]);
              } else {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTIONf, fa->param[0]);
              }
              break;
          case bcmFieldActionEgressMask:
              if (soc_feature(unit, soc_feature_table_hi)) {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_LOf, fa->param[0]);
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTION_HIf, fa->param[1]);
              } else {
                  PolicyCheck(unit, FP_POLICY_TABLEm, REDIRECTIONf, fa->param[0]);
              }
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
              PolicyCheck(unit, FP_POLICY_TABLEm, CLASSIFICATION_TAGf, fa->param[0]);
              break;
          case bcmFieldActionEcnNew:
          case bcmFieldActionDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DROP_PRECEDENCEf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DROP_PRECEDENCEf, fa->param[0]);
              PolicyCheck(unit, FP_POLICY_TABLEm, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionRpDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, RP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionYpDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionYpDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, YP_DSCPf, fa->param[0]);
              break;
          case bcmFieldActionGpDropPrecedence:
              PolicyCheck(unit, FP_POLICY_TABLEm, DROP_PRECEDENCEf, fa->param[0]);
              break;
          case bcmFieldActionGpDscpNew:
              PolicyCheck(unit, FP_POLICY_TABLEm, NEWDSCP_TOSf, fa->param[0]);
              break;
          default:
              return (BCM_E_NONE);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_meter_action_set
 * Purpose:
 *     Get metering portion of Policy Table.
 * Parameters:
 *     unit      -  BCM Unit
 *     f_ent     -  entry structure to get tcam info from.
 *     p_entry - (OUT) Policy hardware entry
 * Returns:
 *     BCM_E_NONE  - Success
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_raptor_meter_action_set(int unit, _field_entry_t *f_ent,
                        fp_policy_table_entry_t *p_entry)
{
    _field_policer_t *f_pl;
    uint32 meter_pair_mode; 

    if ((NULL == f_ent) || (NULL == p_entry)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_field_policer_get(unit, f_ent->policer[0].pid, &f_pl));

    if (((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_POLICER_IN_SECONDARY_SLICE))) {
        /* If no meter, install default meter pair mode.
         * Default meter pair mode doesn't need to burn a real meter.
         */
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_PAIR_MODEf, 0);
        return (BCM_E_NONE);
    }

    /* Get hw encoding for meter mode. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode));


    /* Set the even and odd indexes from the entry */
    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_EVENf,
                                     f_pl->hw_index);
    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_ODDf, 
                                     f_pl->hw_index);

    /* 
     * Flow mode is the only one that cares about the test and update bits.
     * Even = BCM_FIELD_METER_PEAK
     * Odd = BCM_FIELD_METER_COMMITTED
     */
    if (f_pl->cfg.mode == bcmPolicerModePeak) {
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_EVENf,    1);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_EVENf,  1);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_ODDf,   0);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_ODDf, 0);
    } else if (f_pl->cfg.mode == bcmPolicerModeCommitted) {
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_EVENf,    0);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_EVENf,  0);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_TEST_ODDf,   1);
        soc_FP_POLICY_TABLEm_field32_set(unit, p_entry,
                                         METER_UPDATE_ODDf, 1);
    }

    soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_PAIR_MODEf,
                                     meter_pair_mode);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     f_ent  - Physical entry data
 *     tcam_idx - TCAM index to remove entry from.
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     FP unit lock should be held by calling function.
 *     Not clearing the policy, ipbm tables; only TCAM.
 */
STATIC int
_field_raptor_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    _field_group_t           *fg;
    fp_tcam_entry_t          t_entry;

    fg = f_ent->group;
    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));
    soc_FP_TCAMm_field32_set(unit, &t_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL,
                                       tcam_idx, &t_entry));

    f_ent->flags |= _FP_ENTRY_DIRTY; /* Mark entry as not installed */

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_tables_entry_clear
 * Purpose:
 *     Writes a null entry into the the tables.
 * Parameters:
 *     unit      -   (IN)BCM device number.
 *     stage_id  -   (IN)FP stage pipeline id. 
 *     tcam_idx  -   (IN)Entry tcam index.
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     Unit lock should be held by calling function.
 *     Not clearing the policy, ipbm tables, only TCAM.
 */
STATIC int
_field_raptor_tables_entry_clear(int unit, _field_stage_id_t stage_id, int tcam_idx)
{
    fp_tcam_entry_t t_entry;

    if (_BCM_FIELD_STAGE_INGRESS != stage_id) {
        return (BCM_E_PARAM);
    }

    /* Memory index sanity check. */
    if (tcam_idx > soc_mem_index_max(unit, FP_TCAMm)) {
        return (BCM_E_PARAM);
    }  

    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));

    soc_FP_TCAMm_field32_set(unit, &t_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL,
                                       tcam_idx, &t_entry));

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_raptor_entry_rule_copy
 * Purpose:
 *     Helper function for _field_raptor_entry_move_single/double/triple
 * Parameters:
 *     unit           - BCM unit
 *     old_index      - old index
 *     new_index      - new_index
 * Returns:
 *     BCM_E_XXX      - Hardware R/W errors
 *     BCM_E_NONE     - Success
 * Note:
 */
STATIC int
_field_raptor_entry_rule_copy(int unit, int old_index, int new_index)
{
    fp_tcam_entry_t t_entry;

    /* Read the entry rules from current tcam index. */
    SOC_IF_ERROR_RETURN(READ_FP_TCAMm(unit, MEM_BLOCK_ANY, 
                                      old_index, &t_entry));

    /* Write these at the new index */
    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, 
                                       new_index, &t_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_rule_del
 * Purpose:
 *     Helper function for _field_raptor_entry_move_single/double/triple
 * Parameters:
 *     unit       - BCM unit
 *     index      - old index
 * Returns:
 *     BCM_E_XXX      - Hardware R/W errors
 *     BCM_E_NONE     - Success
 * Note:
 */
STATIC int
_field_raptor_entry_rule_del(int unit, int index)
{
    fp_tcam_entry_t t_entry;

    /* Invalidate  index entry. */
    sal_memcpy(&t_entry, soc_mem_entry_null(unit, FP_TCAMm), 
               sizeof(t_entry));
    soc_FP_TCAMm_field32_set(unit, &t_entry, VALIDf, 0);

    SOC_IF_ERROR_RETURN(WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, 
                                       index, &t_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_ipbm_copy
 * Purpose:
 *     Helper function for _field_raptor_entry_move_single/double/triple
 * Parameters:
 *     unit         - BCM device number.
 *     f_ent        - Field entry structure.
 *     part         - Entry part.
 *     new_tcam_idx - New tcam index.
 * Returns:
 *     
 * Note:
 */
STATIC int
_field_raptor_entry_ipbm_copy(int unit, _field_entry_t *f_ent, 
                              int part, int tcam_idx_new)
{
    bcm_port_config_t  port_config;/* Device port config structure. */
    int new_slice_numb = 0;        /* Entry new slice number.       */
    int new_slice_idx = 0;         /* Entry new offset in a slice   */ 
    _field_stage_t *stage_fc;      /* Stage field control structure.*/    
    int old_slice_num;             /* Entry's old slice number.     */
    bcm_port_t p;                  /* Port number iterator.         */
    uint32 value;                  /* IPBM value.                   */
    int    ipbm_index;             /* IPBM index.                   */
    int    rv;                     /* Opeartion return status.      */

    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    /* Read device port configuration. */ 
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &port_config));

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    old_slice_num = f_ent->fs->slice_number;

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, tcam_idx_new,
                                             &new_slice_numb, &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    PBMP_ITER(port_config.all, p) {
        ipbm_index = _BCM_RP_FIELD_IPBM_INDEX(unit, p, old_slice_num);
        BCM_IF_ERROR_RETURN(_field_raptor_ipbm_entry_get(unit, ipbm_index, 
                                                         f_ent->slice_idx, 
                                                         &value));
        ipbm_index = _BCM_RP_FIELD_IPBM_INDEX(unit, p, new_slice_numb);
        BCM_IF_ERROR_RETURN(_field_raptor_ipbm_entry_set(unit, ipbm_index, 
                                                         new_slice_idx, value));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_raptor_entry_move
 * Purpose:
 *     Moves an entry (rules + policies) from one index to another. 
 *     It copies the values in hardware from the old index 
 *     to the new index. The old index is then cleared and set invalid.
 * Parameters:
 *     unit           - BCM device number.
 *     f_ent          - Field entry pointer. 
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_raptor_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new)
{
    fp_policy_table_entry_t     p_entry[_FP_MAX_ENTRY_WIDTH];
    _field_policer_t            *f_pl = NULL; /* Field policer descriptor.   */
    _field_stat_t               *f_st = NULL; /* Field statistic descriptor. */
    _field_group_t              *fg;          /* Field group structure.      */
    int new_slice_numb = 0;                   /* Entry new slice number.     */
    int new_slice_idx = 0;                    /* Entry new offset in a slice */ 
    _field_stage_t *stage_fc;                 /* Stage control structure.    */
    int idx;                                  /* Entry parts iterator.       */
    int rv;                                   /* Operation return status.    */
    int const  tcam_idx_max = soc_mem_index_max(unit, FP_TCAMm);

    if ((NULL == f_ent) || (NULL == tcam_idx_old) || (NULL == tcam_idx_new)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Get number of entry parts. */
    rv = _bcm_field_entry_tcam_parts_count (unit, fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    /* Entry read & destination clear loop. */
    for (idx = 0; idx < parts_count; idx++) {
        /* Index sanity check. */
        if ((tcam_idx_old[idx] < 0) || (tcam_idx_old[idx] > tcam_idx_max) ||
            (tcam_idx_new[idx] < 0) || (tcam_idx_new[idx] > tcam_idx_max)) {
            return (BCM_E_PARAM);
        }

        /* Invalidate the entry at new index
         * To avoid intermediate state as there are 3 tables in raptor:
         * ipbm, rules, policy */
        rv = _field_raptor_entry_rule_del(unit, tcam_idx_new[idx]);
        BCM_IF_ERROR_RETURN(rv);

        /* Copy the IPBM */
        rv = _field_raptor_entry_ipbm_copy(unit, f_ent + idx, idx, tcam_idx_new[idx]);
        BCM_IF_ERROR_RETURN(rv);

        /* Copy the policies */
        /* Read the entry policies from current index. */
        rv = READ_FP_POLICY_TABLEm(unit, MEM_BLOCK_ANY, tcam_idx_old[idx], 
                                   p_entry + idx);
        BCM_IF_ERROR_RETURN(rv);

    }

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, tcam_idx_new[0],
                                             &new_slice_numb, &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Update policy entry if moving across the slices. */
    if (f_ent->slice_idx != new_slice_numb) {

        if ( (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) {
            rv = _bcm_field_policer_get(unit, f_ent->policer[0].pid, &f_pl);
            BCM_IF_ERROR_RETURN(rv);
        }
        if ( (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
            rv = _bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
            if (NULL != f_st) {
                /* 
                 * Set the index of the counter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, COUNTER_INDEXf,
                                                 f_st->hw_index);
            }
            if (NULL != f_pl) {
                /* 
                 * Set the index of the meter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_EVENf,
                                                 f_pl->hw_index);
                soc_FP_POLICY_TABLEm_field32_set(unit, p_entry, METER_INDEX_ODDf,
                                                 f_pl->hw_index);
            }
        } else {
            if (NULL != f_st) {
                _bcm_field_fb_counter_adjust_wide_mode(unit, FP_POLICY_TABLEm,
                                                       f_st, f_ent, f_ent + 1,
                                                       (uint32*)&p_entry[0],
                                                       (uint32*)&p_entry[1]);
            }
            if (NULL != f_pl) {
                _bcm_field_fb_meter_adjust_wide_mode(unit, FP_POLICY_TABLEm,
                                                     f_pl, f_ent, f_ent + 1,
                                                     (uint32*)&p_entry[0],
                                                     (uint32*)&p_entry[1]);
            }
        }
    }

    /* 
     * Write entry to the destination
     * ORDER is important
     */ 
    for (idx = parts_count - 1; idx >= 0; idx--) {

        /* Write these at the new index */
        rv = WRITE_FP_POLICY_TABLEm(unit, MEM_BLOCK_ALL, tcam_idx_new[idx], 
                                    p_entry + idx);
        BCM_IF_ERROR_RETURN(rv);

        /* Copy rules */
        rv = _field_raptor_entry_rule_copy(unit, tcam_idx_old[idx],
                                           tcam_idx_new[idx]);
        BCM_IF_ERROR_RETURN(rv);
    }

    /*
     * Clear old location 
     * ORDER is important
     */
    for (idx = 0; idx < parts_count; idx++) {
        /* Invalidate the old entries */
        rv = _field_raptor_entry_rule_del(unit, tcam_idx_old[idx]);
    }
    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_raven_qualify_ttl_get
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - (IN)BCM device number
 *     entry - (IN)Field entry array.
 *     data - (OUT)Data to qualify with 
 *     mask - (OUT)Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_raven_qualify_ttl_get(int unit, bcm_field_entry_t entry,
                                 uint8 *data, uint8 *mask)
{
    _field_entry_t *entry_arr[_FP_MAX_ENTRY_WIDTH];
    _field_group_t *fg = NULL;
    uint8 result;
    int entry_idx = -1;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, entry_arr);
    BCM_IF_ERROR_RETURN(rv);

    fg = entry_arr[0]->group;
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (TRUE == result) {
        return _bcm_field_entry_qualifier_uint8_get(unit, entry, 
                                                    bcmFieldQualifyTtl, 
                                                    data, mask);
    }

    /* Find the entry for which f4 == 0 and write TTL_ENCODE */
    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } 
    } else if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        }
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        } else if (fg->sel_codes[2].fpf4 != 1) {
            entry_idx = 2;
        }
    }

    if (entry_idx == -1) {
        return BCM_E_PARAM;
    } 

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch (entry_arr[entry_idx]->tcam.f4 & 0x3) {
      case 0:
          *data = 0x0;
          *mask = 0xff;
          break;
      case 1:
          *data = 0x1;
          *mask = 0x3;
          break;
      case 3:
          *data = 0xff;
          *mask = 0xff;
          break;
      default:
          return (BCM_E_UNAVAIL);
    }
    return BCM_E_NONE;                    
}

/*
 * Function: _bcm_field_raven_qualify_ttl
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
_bcm_field_raven_qualify_ttl(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    _field_entry_t *entry_arr[_FP_MAX_ENTRY_WIDTH];
    _field_group_t *fg = NULL;
    uint8 result;
    int entry_idx = -1;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, entry_arr);
    BCM_IF_ERROR_RETURN(rv);

    fg = entry_arr[0]->group;
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (TRUE == result) {
        return _field_qualify32(unit, entry, bcmFieldQualifyTtl, data, mask);
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

    /* Find the entry for which f4 == 0 and write TTL_ENCODE */
    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } 
    } else if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        }
    } else if (fg->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
        if (fg->sel_codes[0].fpf4 != 1) {
            entry_idx = 0;
        } else if (fg->sel_codes[1].fpf4 != 1) {
            entry_idx = 1;
        } else if (fg->sel_codes[2].fpf4 != 1) {
            entry_idx = 2;
        }
    }

    if (entry_idx == -1) {
        return BCM_E_PARAM;
    } 

    entry_arr[entry_idx]->tcam.f4 &= ~(mask);
    entry_arr[entry_idx]->tcam.f4 |= data;

    entry_arr[entry_idx]->tcam.f4_mask &= ~(mask);
    entry_arr[entry_idx]->tcam.f4_mask |= mask;

    entry_arr[entry_idx]->flags |= _FP_ENTRY_DIRTY;
    return BCM_E_NONE;                    
}
#else
int _raptor_field_not_empty;
#endif  /* BCM_RAPTOR_SUPPORT) && BCM_FIELD_SUPPORT */
