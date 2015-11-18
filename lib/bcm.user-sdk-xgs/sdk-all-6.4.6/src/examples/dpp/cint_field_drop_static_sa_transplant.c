/* $Id: Exp $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File: cint_field_drop_static_sa_transplant.c 
 * Purpose: Example of how to use Learn_or_Transplant qualifier in Ingress FP. 
 *  
 * When inserting a Static entry to MACT with a specific destination, it is 
 * expected that packets with this L2 address as their SA, will arrive from 
 * the source that was specified as the destination. 
 * When there is no match between the Source port/vlan and the Destination, 
 * transplant can be performed. 
 * The decision of learn/transplant can be used in the Field Processor. 
 * 
 * CINT Usage: 
 *  
 *  1. Run:
 *      cint cint_field_drop_static_sa_transplant.c
 *      cint
 *      field_drop_static_sa_transplant_example(int unit);
 *  
 *  2. In order to verify add static entry with some destination (port/vlan),
 *     and send packet with SA as L2 address in entry, from a different port.
 *     Packet should be dropped.
 *  
 */
int field_drop_static_sa_transplant_setup(/* in */ int unit,
                                          /* in */ int group_priority,
                                          /* out */ bcm_field_group_t *group)
{
    int grp;
    int result = 0;

    uint16 dq_offset = 42;
    uint8 dq_length = 1;
    uint8 data, mask;

    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_data_qualifier_t dq_is_dynamic; /* dynamic bit */
    bcm_field_qualify_t dq_qualifier = bcmFieldQualifyL2SrcValue;

    bcm_field_data_qualifier_t_init(&dq_is_dynamic); 
    dq_is_dynamic.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
    dq_is_dynamic.qualifier = dq_qualifier; 
    dq_is_dynamic.offset = dq_offset; 
    dq_is_dynamic.length = dq_length; 
    result = bcm_field_data_qualifier_create(unit, &dq_is_dynamic); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2StationMove);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcHit);

    result = bcm_field_qset_data_qualifier_add(unit, &qset, dq_is_dynamic.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
     
    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    result = bcm_field_qualify_L2StationMove(unit, ent, 1, 1);
    if (BCM_E_NONE != result) {
                printf("Error in bcm_field_qualify_L2StationMove\n");
        return result;
    }

    result = bcm_field_qualify_L2SrcHit(unit, ent, 0x1, 0x1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_L2SrcHit\n");
        return result;
    }
    
    data = 0x0;
    mask = 0x1;
    result = bcm_field_qualify_data(unit, ent, dq_is_dynamic.qual_id, &data, &mask, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }
    
    result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }
    
    result = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    *group = grp;

    return result;
}

int field_drop_static_sa_transplant_example(/* in */ int unit)
{
    int result;
    int group_priority = 10;
    bcm_field_group_t group;

    result = field_drop_static_sa_transplant_setup(unit, group_priority, &group);
    if (BCM_E_NONE != result) {
        printf("Error in field_drop_static_sa_transplant_setup\n");
    }

    return result;
}
