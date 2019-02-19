/* $Id: cint_pon_field_mact_llid_limit.c,v 1.1 Broadcom SDK $
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
*/

/* Use first cycle's action to lookup in second cycle (cascaded) 
 * Two databases are created - the first is a regolar TCAM and the second is a Direct Table DB.
 * When a packet arrives, its tunnel ID (first 10 bits) is used to address the action table (direct table - second database).
 * The action holds a part of the key to be searched in the second cycle (in the TCAM - first database), 
 * along with other important firlds. 
 * In this CINT we create some entries for the Ditect Table database in order to allocate some bank for it to use.
 * In practice, these entries are not needed at this point, however without it there would be no bank.
 *
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/pon/cint_pon_field_mact_llid_limit.c
 *      BCM> cint
 *      cint> pon_mact_llid_limit(unit, pon_port1, pon_port2);
 */
bcm_field_group_config_t grp2;
int pon_mact_llid_limit(int unit, int pon_port1, int pon_port2) 
{
    bcm_field_group_config_t grp2;
    bcm_field_aset_t aset2;
    bcm_field_entry_t ent[16];
    bcm_field_entry_t ent_drop[16];
    int result;
    int auxRes;
    int index;
    int group_priority2 = 6;
    bcm_field_data_qualifier_t dqsma;
    bcm_field_data_qualifier_t dqsma_dt;
    uint8 dqMask[2], dqData[2];

    bcm_field_group_config_t_init(&grp2);

    grp2.group = -1;
 
    for (index = 0; index < 16; index++)
    {
        ent[index] = -1;
        ent_drop[index] = -1;
    }
 
    /*
     *  Set the cascaded key length to 16 bits
     */
    result = bcm_field_control_set(unit,
                                   bcmFieldControlCascadedKeyWidth,
                                   16 /* bits in cascaded key */);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_control_set result:%d\n", result);
        return result;
    }

    /* Create a Data qualifier for 16b including the Tunnel Id */
    bcm_field_data_qualifier_t_init(&dqsma); 
    dqsma.offset_base = bcmFieldDataOffsetBasePacketStart;
    dqsma.offset = 2; /* 2 bytes to skip */
    dqsma.length = 2; /* 16 bits */
    result = bcm_field_data_qualifier_create(unit, &dqsma);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_data_qualifier_create result:%d\n", result);
        return result;
    }
    /*
     *  On PetraB / Arad, the second group in a cascade must be created first.
     *  This specific requirement may be masked in the future, but for right
     *  now, it is obligatory even at the BCM layer.
     */
    BCM_FIELD_QSET_INIT(grp2.qset);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyCascadedKeyValue);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyL2SrcHit);
    BCM_FIELD_QSET_ADD(grp2.qset, bcmFieldQualifyStageIngress);
    result = bcm_field_qset_data_qualifier_add(unit,
                                               &(grp2.qset),
                                               dqsma.qual_id);
    grp2.priority = group_priority2;
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_qset_data_qualifier_add result:%d\n", result);
        return result;
    }

    BCM_FIELD_ASET_INIT(aset2);
    BCM_FIELD_ASET_ADD(aset2, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset2, bcmFieldActionDoNotLearn);
    result = bcm_field_group_config_create(unit, &grp2);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_group_config_create result:%d\n", result);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp2.group, aset2);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_group_action_set result:%d\n", result);
        return result;
    }


    /*
     *  Now create the initial group 
     */
    bcm_field_data_qualifier_t_init(&dqsma_dt); 
    dqsma_dt.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES|BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;
    dqsma_dt.offset = 0; 
    dqsma_dt.length = 10; 
    dqsma_dt.qualifier = bcmFieldQualifyTunnelId; 
    result = bcm_field_data_qualifier_create(unit, &dqsma_dt);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_data_qualifier_create result:%d\n", result);
        return result;
    }
	
    /* Add entries to the second group: when to drop */
    for (index = (pon_port1*2); index < ((pon_port2+1)*2); index++)
    {
        printf("index:%d\n", index);
	    result = bcm_field_entry_create(unit, grp2.group, &(ent[index]));
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_entry_create result:%d index:%d\n", result, index);
            return result;
        }

        /* Bit = index is up */
        result = bcm_field_qualify_CascadedKeyValue(unit, ent[index], (1 << index), (1 << index));
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_qualify_CascadedKeyValue result:%d index:%d\n", result, index);
            return result;
        }

        /* InPort = index >> 1 for the 3 first bits */
        result = bcm_field_qualify_InPort(unit, ent[index], (index >> 1), (~0));
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_qualify_InPort result:%d index:%d\n", result, index);
            return result;
        }

        /* SA lookup not found to drop */
        result = bcm_field_qualify_L2SrcHit(unit, ent[index], 0, 0x1);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_qualify_L2SrcHit result:%d index:%d\n", result, index);
            return result;
        }

        /* Bit 10 of Tunnel Id up if needed */
        dqData[1] = 0x00;
        dqData[0] = 0x04 * (index % 2);
        dqMask[1] = 0x00;
        dqMask[0] = 0x04;
        result = bcm_field_qualify_data(unit,
                                        ent[index],
                                        dqsma.qual_id,
                                        &(dqData[0]),
                                        &(dqMask[0]),
                                        2 /* len here always in bytes */);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_qualify_data result:%d index:%d\n", result, index);
            return result;
        }

        result = bcm_field_action_add(unit, ent[index], bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_action_add - bcmFieldActionDrop result:%d index:%d\n", result, index);
            return result;
        }

        result = bcm_field_action_add(unit, ent[index], bcmFieldActionDoNotLearn, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_action_add - bcmFieldActionDoNotLearn result:%d index:%d\n", result, index);
            return result;
        }
    }

    /* Drop until learnt - packet are dropped until the SA is learnt at PON port */
    /* For PON port 1 */
    index = 0;
    result = bcm_field_entry_create(unit, grp2.group, &(ent_drop[index]));
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_entry_create result:%d index:%d\n", result, index);
        return result;
    }

    result = bcm_field_qualify_L2SrcHit(unit, ent_drop[index], 0, 0x1);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_qualify_L2SrcHit result:%d index:%d\n", result, index);
        return result;
    }

    result = bcm_field_qualify_InPort(unit, ent_drop[index], pon_port1, (~0));
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_qualify_InPort result:%d index:%d\n", result, index);
        return result;
    }

    result = bcm_field_action_add(unit, ent_drop[index], bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_action_add result:%d index:%d\n", result, index);
        return result;
    }

    /* For PON port 2 */
    index++;
    result = bcm_field_entry_create(unit, grp2.group, &(ent_drop[index]));
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_entry_create result:%d index:%d\n", result, index);
        return result;
    }

    result = bcm_field_qualify_L2SrcHit(unit, ent_drop[index], 0, 0x1);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_qualify_L2SrcHit result:%d index:%d\n", result, index);
        return result;
    }

    result = bcm_field_qualify_InPort(unit, ent_drop[index], pon_port2, (~0));
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_qualify_InPort result:%d index:%d\n", result, index);
        return result;
    }

    result = bcm_field_action_add(unit, ent_drop[index], bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_action_add result:%d index:%d\n", result, index);
        return result;
    }
 
    result = bcm_field_group_install(unit, grp2.group);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_group_install result:%d\n", result);
        return result;
    }

    return result;
}


