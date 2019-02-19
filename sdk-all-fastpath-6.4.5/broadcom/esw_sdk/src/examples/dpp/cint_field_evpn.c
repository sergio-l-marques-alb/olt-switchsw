/* $Id: Exp $
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
 * File: cint_field_evpn.c 
 * Purpose: Example of how to FP for EVPN application. 
 *  
 * The application requires two DBs in ingress and one in egress. 
 * Ingress PMF: 
 * EVPN DB
 *  Use one ingress PMF entry to trigger the copy of ingress AC LIF additional data (20-bit ESI label + 20-bit PE ID) into learn-extension.
 * Pre-selector:  Forwarding type (L2) + EVPN inLIF profile
 * Key: EVPN inLIF profile
 * Action: copy inLIF.additional-data to learn-extension
 *  
 * ESI DB
 *  Use N ingress PMF entries for ESI label lookup to trigger the copy of forbidden-out-port into learn-extension.
 * Pre-selector: Forwarding type (L2) + EVPN inLIF profile + TT termination found
 * Key: ESI label
 * Action: copy forbidden-out-port to learn-extension if match. Otherwise, drop the packet
 *  
 *  
 * Ingress PMF: 
 * EVPN DB 
 *  Use 33 egress PMF entries in one DB to drop a bridge packet whose learn-extension (carried forbidden-out-port) equals to the destination system port
 * Pre-selector: bridge packet + learn-extension present
 * Key: 16-bit forbidden port in learn-extension-header + 16-bit destination system port
 * Action: Drop if forbidden port equals destination system port
 *  
 *
 * CINT Usage: 
 *  
 *  1. Run:
 *      cint cint_field_evpn.c

 *      field_processor_evpn_example(int unit);
 *  
 *
 *   Note: This is an initial draft.
 *
 */

/* Global variables */
int inlif_profile_evpn = 0x1;
int inlif_profile_evpn_mask = 0x1;
uint8 esi1_label[2] = {10,0};
uint8 esi2_label[2] = {20,0};
uint8 esi_label_mask[2] = {0xff,0xff};
int forbidden_out_port1 = 13;
int forbidden_out_port2 = 14; 


/*
 *  Example to ESI DB in ingress PMF. 
 *  
 *  Sequence:
 *  1. 	Define a preselection with TunnelTerminated, ForwardingType=L2 and inLif profile = EVPN as pre-selectors.
 *      Only EVPN packets will be processed.
 *      Qualifier is he ESI label, such that each ESI label points to a forbidden port that will be
 *      written on the Learn Extention 
 *  2. 	Create a Field Group and add 2 entries - for two different ESI labels.
 *  3. 	Create the connection between the Presel and Field Group.
 *  
 */

int esi_db(int unit, int group_priority) {
    bcm_field_presel_set_t psset;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent1, ent2, ent3;
    bcm_gport_t forbidden_out_gport;
    int presel_id;
    int result;
    int auxRes;

    /********* PRESELECTOR *************/

    /* Cretae a presel entity */
    result = bcm_field_presel_create_id(unit, presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_id\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    /* Configure the presel */
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_TunnelTerminated(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 1, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_TunnelTerminated\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, inlif_profile_evpn, inlif_profile_evpn_mask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /********* QUALIFIERS *********/

    BCM_FIELD_QSET_INIT(qset);

    /* Create data qualifier ESI label */
    bcm_field_data_qualifier_t bcm_FieldQualifyEsi;

    /* the qualify inner dmac1 of user define */
    bcm_field_data_qualifier_t_init(&bcm_FieldQualifyEsi);
    /* the qualify of user define */
    bcm_FieldQualifyEsi.offset_base = bcmFieldDataOffsetBaseForwardingHeader;  /* the position within the packet starting at which the qualifier is extracted packet */
    bcm_FieldQualifyEsi.offset = 32;   /* Packet byte offset */
    bcm_FieldQualifyEsi.length = 20;   /* Matched data byte length */
    bcm_FieldQualifyEsi.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE|BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES|BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES  ; /* Negative bit offset */

    /* Create a data/offset-based qualifier */
    result = bcm_field_data_qualifier_create(unit, &bcm_FieldQualifyEsi);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create $result\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &qset, bcm_FieldQualifyEsi.qual_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add $result\n");
        return result;
    }



    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }


    result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /********* ACTION *********/

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionLearnSrcPortNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    result = bcm_field_group_action_set(unit, grp, aset); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /* Entries - one for each ESI label */
    result = bcm_field_entry_create(unit, grp, &ent1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent1);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent2);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &ent3);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent3);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }


    /* Add data field that the packet must match to trigger qualifier */

    /* ESI label 1 */
    result = bcm_field_qualify_data(unit, ent1, bcm_FieldQualifyEsi.qual_id, esi1_label, esi_label_mask/*label should be exact, other fields masked*/, 2);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_data $result\n");
        return result;
    }

    /* ESI label 2 */
    result = bcm_field_qualify_data(unit, ent2, bcm_FieldQualifyEsi.qual_id, esi2_label, esi_label_mask/*label should be exact, other fields masked*/, 2);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qualify_data $result\n");
        return result;
    }

    /* Action: copy forbidden-out-port to learn-extension if match. */
    BCM_GPORT_LOCAL_SET(forbidden_out_gport, forbidden_out_port1);
    result = bcm_field_action_add(unit, ent1, bcmFieldActionLearnSrcPortNew, forbidden_out_gport, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    /* Set priority for the entry */
    result = bcm_field_entry_prio_set(unit, ent1, 1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_prio_set\n");
        return result;
    }

    BCM_GPORT_LOCAL_SET(forbidden_out_gport, forbidden_out_port2);
    result = bcm_field_action_add(unit, ent2, bcmFieldActionLearnSrcPortNew, forbidden_out_gport, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    /* Set priority for the entry */
    result = bcm_field_entry_prio_set(unit, ent2, 2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_prio_set\n");
        return result;
    }

    /* Otherwise, drop the packet (no qualifier) */
    result = bcm_field_action_add(unit, ent3, bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }
    
    /* Set priority for the entry - this entry must be lowest priority to act as default action */
    result = bcm_field_entry_prio_set(unit, ent3, 3);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_prio_set\n");
        return result;
    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, 0);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    return result;
}


/*
 *  Example to EVPN DB in ingress PMF. 
 *  
 *  Sequence:
 *  1. 	Define a preselection with ForwardingType=L2 and inLif profile = EVPN as pre-selectors.
 *      Only EVPN packets will be processed.
 *      Qualifier is the ESI label, such that each ESI label points to a forbidden port that will be
 *      written on the Learn Extention 
 *  2. 	Create a Field Group and add 2 entries - for two different ESI labels.
 *  3. 	Create the connection between the Presel and Field Group.
 *  
 */

int evpn_db(int unit, int group_priority) {
    bcm_field_presel_set_t psset;
    bcm_field_qset_t qset_lsb, qset_msb;
    bcm_field_aset_t aset;
    bcm_field_group_config_t grp_lsb;
    bcm_field_group_config_t grp_msb;
    bcm_field_entry_t ent1, ent2;
    int presel_id;
    int result;
    int auxRes;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_inlif;

    /********* PRESELECTOR *************/

    /* Cretae a presel entity */
    result = bcm_field_presel_create(unit, &presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_id\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    /* Configure the presel */
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    result = bcm_field_qualify_InterfaceClassVPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, inlif_profile_evpn, inlif_profile_evpn_mask);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_InterfaceClassVPort\n");
        return result;
    }

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /********* QUALIFIERS *********/

    BCM_FIELD_QSET_INIT(qset_lsb);
    BCM_FIELD_QSET_INIT(qset_msb);

    /* Create data qualifier for inLif additional data */
    bcm_field_data_qualifier_t bcm_FieldQualifyAddtionalData_lsb;

    bcm_field_data_qualifier_t_init(&bcm_FieldQualifyAddtionalData_lsb);
    /* the qualify of user define */
    bcm_FieldQualifyAddtionalData_lsb.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;     
    bcm_FieldQualifyAddtionalData_lsb.offset = 0;   /* Packet bit offset */
    bcm_FieldQualifyAddtionalData_lsb.length = 16;   /* Matched data bit length */
    bcm_FieldQualifyAddtionalData_lsb.qualifier = bcmFieldQualifyInVPortWide;
    bcm_FieldQualifyAddtionalData_lsb.stage = bcmFieldStageIngress;

    /* Create data qualifier for inLif additional data */
    bcm_field_data_qualifier_t bcm_FieldQualifyAddtionalData_msb;

    bcm_field_data_qualifier_t_init(&bcm_FieldQualifyAddtionalData_msb);
    /* the qualify of user define */
    bcm_FieldQualifyAddtionalData_msb.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;     
    bcm_FieldQualifyAddtionalData_msb.offset = 16;   /* Packet bit offset */
    bcm_FieldQualifyAddtionalData_msb.length = 24;   /* Matched data bit length */
    bcm_FieldQualifyAddtionalData_msb.qualifier = bcmFieldQualifyInVPortWide;
    bcm_FieldQualifyAddtionalData_msb.stage = bcmFieldStageIngress;


    /* Create a data/offset-based qualifier */
    result = bcm_field_data_qualifier_create(unit, &bcm_FieldQualifyAddtionalData_lsb);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create $result\n");
        return result;
    }
    /* Create a data/offset-based qualifier */
    result = bcm_field_data_qualifier_create(unit, &bcm_FieldQualifyAddtionalData_msb);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_data_qualifier_create $result\n");
        return result;
    }

    result = bcm_field_qset_data_qualifier_add(unit, &qset_lsb, bcm_FieldQualifyAddtionalData_lsb.qual_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add $result\n");
        return result;
    }
    result = bcm_field_qset_data_qualifier_add(unit, &qset_msb, bcm_FieldQualifyAddtionalData_msb.qual_id);
    if (result != BCM_E_NONE) {
        printf("Error, bcm_field_qset_data_qualifier_add $result\n");
        return result;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionLearnSrcPortNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionLearnInVPortNew);

    bcm_field_group_config_t_init(&grp_lsb);
    bcm_field_group_config_t_init(&grp_msb);
    /*  Create two Field groups with type Direct Extraction */
    /*  One group for LSB of the Additional Data and one for the MSB */
    grp_lsb.priority = group_priority;
    grp_lsb.qset = qset_lsb;
    grp_lsb.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp_lsb.mode = bcmFieldGroupModeDirectExtraction;
    result = bcm_field_group_config_create(unit, &grp_lsb);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }
    result = bcm_field_group_presel_set(unit, grp_lsb.group, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp_lsb.group);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp_lsb.group, aset); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp_lsb.group);
        return result;
    }

    grp_msb.priority = group_priority+1;
    grp_msb.qset = qset_msb;
    grp_msb.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp_msb.mode = bcmFieldGroupModeDirectExtraction;
    result = bcm_field_group_config_create(unit, &grp_msb);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_config_create\n");
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp_msb.group, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp_msb.group);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp_msb.group, aset); 
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp_msb.group);
        return result;
    }


    /* Create the Direct Extraction entries: two entries are required:
     *  One entry is required for the LSBs of the Additional Data
     *  One entry is required for the MSBs of the Additional Data 
     */

    /* Entry1: LSB*/
    result = bcm_field_entry_create(unit, grp_lsb.group, &ent1);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        auxRes = bcm_field_entry_destroy(unit, ent1);
        auxRes = bcm_field_group_destroy(unit, grp_lsb.group);
        return result;
    }

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_inlif);

    extract.action = bcmFieldActionLearnSrcPortNew;
    extract.bias = 0;

    /* extraction structure indicates to use InLif qualifier */
    ext_inlif.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_inlif.bits  = 16;
    ext_inlif.lsb   = 0;
    ext_inlif.qualifier = bcm_FieldQualifyAddtionalData_lsb.qual_id;

    result = bcm_field_direct_extraction_action_add(unit,
                                                    ent1,
                                                    extract,
                                                    1 /* count */,
                                                    &ext_inlif);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    /* Entry2: MSB*/
    result = bcm_field_entry_create(unit, grp_msb.group, &ent2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_inlif);

    extract.action = bcmFieldActionLearnInVPortNew;
    extract.bias = 0;

    /* extraction structure indicates to use InLif qualifier */
    ext_inlif.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_inlif.bits  = 24;
    ext_inlif.lsb   = 0;
    ext_inlif.qualifier = bcm_FieldQualifyAddtionalData_msb.qual_id;
    result = bcm_field_direct_extraction_action_add(unit,
                                                    ent2,
                                                    extract,
                                                    1 /* count */,
                                                    &ext_inlif);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        return result;
    }

    return result;
}



/*
 *  Example to EVPN DB in egress PMF.
 *  
 *  Drop a bridge packet whose learn-extension (carried forbidden-out-port) equals to the destination system port
 *  
 *  Sequence:
 *  1. 	Define a preselection with ForwardingType=L2 and learn extention present (!!!!).
 *      Only EVPN packets with learn extention will be processed.
 *      Qualifier is the 16-bit forbidden port in learn-extension-header + 16-bit destination system port,
 *      such that if forbidden port equals destination system port packet is dropped
 *  2. 	Create a Field Group and add 3 entries - for three different forbidden ports.
 *  3. 	Create the connection between the Presel and Field Group.
 *  
 */

int evpn_egress_filter_db(int unit, int group_priority) {
    bcm_field_presel_set_t psset;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    int presel_id;
    int result;
    int auxRes;
    int ports[3] = { 13, 14, 15 };
    int cur_port;


    /********* PRESELECTOR *************/

    /* Cretae a presel entity */
    result = bcm_field_presel_create(unit, &presel_id);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_presel_create_id\n");
        auxRes = bcm_field_presel_destroy(unit, presel_id);
        return result;
    }

    /* Configure the presel */
    result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageEgress);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_Stage\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_qualify_ForwardingType\n");
        return result;
    }

    /* System-header-record-format-code (learn-extension-present): TBD */

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, presel_id);

    /********* QUALIFIERS *********/

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassVPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstPort);

    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create_id\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    result = bcm_field_group_presel_set(unit, grp, &psset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_presel_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    /********* ACTION *********/
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_action_set\n");
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }


    /* Entries */
    for (cur_port = 0; cur_port < 3; cur_port++) {
        result = bcm_field_entry_create(unit, grp, &ent);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_entry_create\n");
            auxRes = bcm_field_entry_destroy(unit, ent);
            auxRes = bcm_field_group_destroy(unit, grp);
            return result;
        }

        result = bcm_field_qualify_DstPort(unit, ent, unit, 0, ports[cur_port], 0);
        if (result != BCM_E_NONE) {
            printf("Error, bcm_field_qualify_DstPort $result\n");
            return result;
        }

        /* Learn Extention : TBD */
        /*result = bcm_field_qualify_LearnSrcPort(unit, ent, ports[cur_port], 0xfffff);
        if (result != BCM_E_NONE) {
            printf("Error, bcm_field_qualify_data $result\n");
            return result;
        }*/


        result = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != result) {
            printf("Error in bcm_field_action_add\n");
            return result;
        }

    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        auxRes = bcm_field_entry_destroy(unit, ent);
        auxRes = bcm_field_group_destroy(unit, grp);
        return result;
    }

    return result;
}



int field_processor_evpn_example(/* in */ int unit)
{
    int group_priority = 10;
    int result;


    result = esi_db(unit, 1/*group_priority*/);
        if (BCM_E_NONE != result) {
          printf("Error in esi_db\n");
            return result;
        }

    result = evpn_db(unit, 2/*group_priority*/);
        if (BCM_E_NONE != result) {
            printf("Error in evpn_db\n");
            return result;
        }

    result = evpn_egress_filter_db(unit, 5/*group_priority*/);
        if (BCM_E_NONE != result) {
            printf("Error in evpn_egress_filter_db\n");
            return result;
        }

    return result;
}
