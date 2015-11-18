/*
 * $Id: cint_field_mim_32k_isids.c,v 1.1 Broadcom SDK $
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
 */
/*
 * This cint should be used in conjunction with the soc property mim_num_vsis=32768. 
 * It Completes the configurations required in the field processor in order for the application of MiM with 32K I-SIDs to work. 
 * It creates two direct extraction entries, which are used to carry the I-SID information to the egress on the FTMH extensions. 
 *                                                                                                                             
 * To enable this feature, run mim_32k_isids_setup (mim_num_vsis=32768 must be set)
 * To disable it, run mim_32k_isids_teardown                                                                                                                                                                           .
 */

bcm_field_group_t mim_vsi_32k_mode_dir_ext_grp = -1;

int mim_32k_isids_setup(int unit) {

    bcm_error_t rv;
    bcm_field_group_config_t config;
    bcm_field_entry_t ent;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_pcp;


    bcm_field_group_config_t_init(&config);
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_pcp);

    if (soc_property_get(unit, spn_MIM_NUM_VSIS, 0) == 32768) {        

        BCM_FIELD_QSET_ADD(config.qset, bcmFieldQualifyInterfaceClassVPort);
        BCM_FIELD_QSET_ADD(config.qset, bcmFieldQualifyStageIngress);
        BCM_FIELD_QSET_ADD(config.qset, bcmFieldQualifyTranslatedInnerVlanId);
        BCM_FIELD_ASET_ADD(config.aset, bcmFieldActionOuterVlanPrioNew);
        BCM_FIELD_ASET_ADD(config.aset, bcmFieldActionVlanActionSetNew);
        config.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE;
        config.priority = 2;
        config.mode = bcmFieldGroupModeDirectExtraction;
        rv = bcm_field_group_config_create(unit, &config);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_group_config_create\n");
            print rv;
        }
        rv =  bcm_field_entry_create(unit, config.group, &ent);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_entry_create\n");
            print rv;
        }
        rv = bcm_field_qualify_InterfaceClassVPort(unit, ent, 0xf,0xf);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_qualify_InterfaceClassVPort\n");
            print rv;
        }

        extract.action = bcmFieldActionOuterVlanPrioNew;
        extract.bias = 0;
        ext_pcp.flags = 0;
        ext_pcp.bits = 4;
        ext_pcp.lsb = 0;
        ext_pcp.qualifier = bcmFieldQualifyTranslatedInnerVlanId;
        rv =  bcm_field_direct_extraction_action_add(unit,
                                                     ent,
                                                     extract,
                                                     1 /* count */,
                                                     &ext_pcp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_direct_extraction_action_add\n");
            print rv;
        }

        extract.action = bcmFieldActionVlanActionSetNew;
        extract.bias = 48;
        ext_pcp.flags = 0;
        ext_pcp.bits = 4;
        ext_pcp.lsb = 0;
        ext_pcp.qualifier = bcmFieldQualifyInterfaceClassVPort;
        rv = bcm_field_direct_extraction_action_add(unit,
                                                     ent,
                                                     extract,
                                                     1 /* count */,
                                                     &ext_pcp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_direct_extraction_action_add\n");
            print rv;
        }

        rv = bcm_field_group_install(unit, config.group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_group_install\n");
            print rv;
        }

        mim_vsi_32k_mode_dir_ext_grp = config.group;
    }

    return rv;
}


int mim_32k_isids_teardown(int unit) {

    bcm_error_t rv;

    if (mim_vsi_32k_mode_dir_ext_grp != -1) {        

        rv = bcm_field_group_flush(unit, mim_vsi_32k_mode_dir_ext_grp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_group_flush\n");
            print rv;
        }
    }

    return rv;
}

