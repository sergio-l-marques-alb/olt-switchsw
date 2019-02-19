/* $Id: cint_field_mpls_speculative_parse.c, Broadcom SDK $
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
 * File: cint_field_mpls_speculative_parse.c
 * Purpose: Example for utilizing the field APIs to filter all packets that are IPv4/6 over MPLS.
 *          This is used as a proof of concept for speculative parsing of headers over MPLS.
 *
 *
 *
 * CINT usage:
 *  *   Call mpls_lsr_run_with_defaults_multi_device from cint_mpls_lsr.c
 *  *   Call lsr_filter_non_tcp_udp.
 *  *   IPv4/6 over MPLS packets should now be filtered.
 *
 */


bcm_field_group_config_t drop_mpls_group_config;

int lsr_filter_non_tcp_udp(int *units_ids, int nof_units, int in_port) {
    int result;
    int auxRes;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent[3];
    int unit;
    int i, j, presel_id = 1;
    bcm_field_presel_set_t psset;

    uint32 drop_priority = 20,
           pass_tcp_udp_priority = 21;

    for (j = 0 ; j < nof_units ; j++){
        unit = units_ids[j];

        result = bcm_port_control_set(unit, in_port, bcmPortControlMplsSpeculativeParse, 1);
        if (BCM_E_NONE != result) {
          return result;
        }

        /*********************************/
        /* Build the drop all MPLS packets group */
        /*********************************/

        /* Create a presel entity */
        result = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != result) {
          printf("Error in bcm_field_presel_create_id\n");
          return result;
        }

        result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
        if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qualify_Stage\n");
          return result;
        }

        BCM_FIELD_PRESEL_INIT(psset);
        BCM_FIELD_PRESEL_ADD(psset, presel_id);

        /*
         * Build the qualifier set for the field group.
         */
        BCM_FIELD_QSET_INIT(drop_mpls_group_config.qset);
        BCM_FIELD_QSET_ADD(drop_mpls_group_config.qset, bcmFieldQualifyHeaderFormat);

        /*
         *  Build the action set for the group.
         *	We want to drop all packets except TCP UDP.
         */
        BCM_FIELD_ASET_INIT(drop_mpls_group_config.aset);
        BCM_FIELD_ASET_ADD(drop_mpls_group_config.aset, bcmFieldActionDrop);

        /*
         *  Create the group and set its actions.
         */
        drop_mpls_group_config.preselset = psset;
        drop_mpls_group_config.priority = drop_priority;
        drop_mpls_group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
        drop_mpls_group_config.mode = bcmFieldGroupModeAuto;
        result = bcm_field_group_config_create(unit, &drop_mpls_group_config);
        if (BCM_E_NONE != result) {
          printf("Error in bcm_field_group_config_create\n");
          return result;
        }

        /* Create per group: 3 entries for 1/2/3 MPLS labels header */
        for (i=0; i < 3; i++) {
            result = bcm_field_entry_create(unit, drop_mpls_group_config.group, &ent[i]);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_entry_create\n");
                auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                return result;
            }

            if (i%3==0) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp4MplsLabel1);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%3==1) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp4MplsLabel2);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            if (i%3==2) {
                result = bcm_field_qualify_HeaderFormat(unit, ent[i], bcmFieldHeaderFormatIp4MplsLabel3);
                if (BCM_E_NONE != result) {
                    printf("Error in bcm_field_qualify_HeaderFormat\n");
                    auxRes = bcm_field_entry_destroy(unit, ent[i]);
                    auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                    return result;
                }
            }

            result = bcm_field_action_add(unit, ent[i], bcmFieldActionDrop, 0, 0);
            if (BCM_E_NONE != result) {
                printf("Error in bcm_field_action_add\n");
                auxRes = bcm_field_entry_destroy(unit, ent[i]);
                auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
                return result;
            }
        }

        /*
         *  Commit the entire group to hardware.
         */
        result = bcm_field_group_install(unit, drop_mpls_group_config.group);
        if (BCM_E_NONE != result) {
            auxRes = bcm_field_entry_destroy(unit, ent[0]);
            auxRes = bcm_field_entry_destroy(unit, ent[1]);
            auxRes = bcm_field_entry_destroy(unit, ent[2]);
            auxRes = bcm_field_group_destroy(unit, drop_mpls_group_config.group);
            return result;
        }
    }
    return result;
}

