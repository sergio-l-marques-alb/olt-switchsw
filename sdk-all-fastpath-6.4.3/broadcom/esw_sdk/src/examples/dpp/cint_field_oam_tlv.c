
/*
 * $id cint_end_tlv.c $
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
 * End TLV workaround:
 * Add a value to the ACE VAR to use the End TLV workaround for OAM CCM packets with
 * port/interface status TLV fields.
 *
 * In order to use port/interface status TLV in received OAM CCM packets, call:
 * > cint cint_field_oam_tlv.c
 * > cint
 * > prge_end_tlv_example(0);
 */

/*
 * prge_end_tlv_create_field_group
 *
 * Allocates PMF program and defines a key and action
 */
int prge_end_tlv_create_field_group(int unit, int group_priority, bcm_field_group_t grp_tcam, int qual_id)
{
    int result;
    int presel_id;
    bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_presel_set_t psset;
    bcm_field_group_config_t grp_conf;
    
    /* Cretae a presel entity */
    result = bcm_field_presel_create(unit, &presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_id\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeRxReason);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    bcm_field_group_config_t_init(&grp_conf);
    grp_conf.group = grp_tcam;
    grp_conf.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp_conf.mode = bcmFieldGroupModeAuto;
    grp_conf.priority = group_priority;
    grp_conf.preselset = psset;

    BCM_FIELD_QSET_INIT(grp_conf.qset);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyStageEgress); 
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyDstPort);
    BCM_FIELD_QSET_ADD(grp_conf.qset, bcmFieldQualifyFheiSize);

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_WITH_ID | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.qual_id = qual_id;
    data_qual.offset = 0;
    data_qual.qualifier = bcmFieldQualifyFhei;
    data_qual.length = 8;
    data_qual.stage = bcmFieldStageEgress;
    result = bcm_field_data_qualifier_create(unit, &data_qual);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_data_qualifier_create\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &grp_conf.qset, data_qual.qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qset_data_qualifier_add\n");
        return result;
    } 

    result = bcm_field_group_config_create(unit, &grp_conf);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create for group %d\n", grp_conf.group);
        return result;
    }

    /* Define the ASET - use counter 0. */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);

    /* Attach the action set */
    result = bcm_field_group_action_set(unit, grp_tcam, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set for group %d\n", grp_tcam);
      return result;
    }

    return result;
}

/*
 * prge_end_tlv_add_entry
 *
 * Adds an entry for the key and sets value=3 for the action created by prge_end_tlv_create_field_group
 */
int prge_end_tlv_add_entry(int unit, bcm_field_group_t grp_tcam, uint8 trap_code, uint32 dest_port, int qual_id)
{
    int result;
    bcm_field_entry_t ent_tcam;
    bcm_gport_t local_gport;
    int ace_var, statId;
    uint8 dqData[1], dqMask[1];
    
    result = bcm_field_entry_create(unit, grp_tcam, &ent_tcam);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    /* Entry qualifier values */
    result = bcm_field_qualify_DstPort(unit, ent_tcam, 0, 0xffffffff, dest_port, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_SrcMac\n");
        return result;
    }

    result = bcm_field_qualify_FheiSize(unit, ent_tcam, 3, 0xffffffff);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_SrcMac\n");
        return result;
    }

    dqData[0] = trap_code;
    dqMask[0] = 0xFF;
    result = bcm_field_qualify_data(unit, ent_tcam, qual_id, &(dqData[0]), &(dqMask[0]), 1 /* len in bytes */);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_data\n");
        return result;
    }

    /* Entry Action values */
    BCM_GPORT_LOCAL_SET(local_gport, dest_port);
    result = bcm_field_action_add(unit, ent_tcam, bcmFieldActionRedirect, 0, local_gport);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    ace_var = (0x3 << 8);
    statId = (ace_var << 19); /* Set internal data */
    result = bcm_field_action_add(unit, ent_tcam, bcmFieldActionStat, statId, local_gport);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
      return result;
    }

    result = bcm_field_entry_install(unit, ent_tcam);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_install\n");
        return result;
    }

    return result;
}

/*
 * Sets the key to be [OAM-Trap-code, OAMP-dest-port] and sets the value for that key
 */
int prge_end_tlv_example(int unit)
{
    int result;
    bcm_field_group_t grp_tcam = 1;
    int group_priority = 10;
    int qual_id = 1;

    result = prge_end_tlv_create_field_group(unit, group_priority, grp_tcam, qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_create_field_group\n");
        return result;
    }
    
    result = prge_end_tlv_add_entry(unit, grp_tcam, 0xE0/*trap_code (OAM)*/, 232/*dest_port (OAMP PP port)*/, qual_id);
    if (BCM_E_NONE != result) {
        printf("Error in prge_end_tlv_add_entry\n");
        return result;
    }

    return result;
}

