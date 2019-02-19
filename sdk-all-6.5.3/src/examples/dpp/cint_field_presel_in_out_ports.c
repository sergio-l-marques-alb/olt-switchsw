
/* $Id: cint_field_presel_in_out_ports.c,v 1.4 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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

/*
 *  Example to Pre-Selection usage in case of In/Out ports 
 * 	as qualifiers. 
 *  
 *  Sequence:
 *  1. 	Define a preselection with In/Out ports as qualifier.
 *  	Only packets which arrive or are destined to the specified
 *  	ports will be processed.  
 *  2. 	Create a Field Group and add an entry.
 *  3. 	Create the connection between the Presel and Field Group.
 *  
 */

int in_out_port_presel(int unit, int group_id, int group_priority, int presel_id, int in_stage, int single) 
{
	bcm_field_presel_set_t psset;
	bcm_mac_t macData;
	bcm_mac_t macMask;
	bcm_field_qset_t qset;
	bcm_field_aset_t aset;
	bcm_field_group_t grp = group_id;
	bcm_field_entry_t ent;
	bcm_pbmp_t pbm;
	bcm_pbmp_t pbm_mask;
	int i;
	int result;
	int auxRes;
	bcm_field_stage_t stage;
		
	/* Cretae a presel entity */
	result = bcm_field_presel_create_id(unit, presel_id);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_presel_create_id\n");
		auxRes = bcm_field_presel_destroy(unit, presel_id);
    return result;
  }

	/* Define the set of in/out ports */
	BCM_PBMP_CLEAR(pbm);
	BCM_PBMP_PORT_ADD(pbm, 1);
	BCM_PBMP_PORT_ADD(pbm, 32);
	BCM_PBMP_PORT_ADD(pbm, 64);
	for(i=0; i<256; i++) { BCM_PBMP_PORT_ADD(pbm_mask, i); }

	/* Depending on stage, configure the presel stage and in/out port qualifier */	
	if(in_stage == 0) 	{
		stage = bcmFieldStageIngress;
		result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, stage);
		if (BCM_E_NONE != result) {
			printf("Error in bcm_field_qualify_Stage\n");
    	return result;
  	}
  	
		if(single){
			result = bcm_field_qualify_InPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 1, 0xffffffff);	
			if (BCM_E_NONE != result) {
				printf("Error in bcm_field_qualify_InPort\n");
	    	return result;
	  	}
		} else {
			result = bcm_field_qualify_InPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, pbm, pbm_mask);
			if (BCM_E_NONE != result) {
				printf("Error in bcm_field_qualify_InPorts\n");
	    	return result;
	  	}
		}
		
	} else {
		stage = bcmFieldStageEgress;
		result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, stage);
		if (BCM_E_NONE != result) {
			printf("Error in bcm_field_qualify_Stage\n");
    	return result;
  	}

		if(single){
			result = bcm_field_qualify_OutPort(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, 1, 0xffffffff);	
			if (BCM_E_NONE != result) {
				printf("Error in bcm_field_qualify_OutPort\n");
	    	return result;
	  	}
		} else {
			result = bcm_field_qualify_OutPorts(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, pbm, pbm_mask);
			if (BCM_E_NONE != result) {
				printf("Error in bcm_field_qualify_OutPorts\n");
	    	return result;
	  	}
		}
	}
  
	BCM_FIELD_PRESEL_INIT(psset);
	BCM_FIELD_PRESEL_ADD(psset, presel_id);
	
	BCM_FIELD_QSET_INIT(qset);
	if(in_stage == 0) 	{
		BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
	} else {
		BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
	}
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
		
	BCM_FIELD_ASET_INIT(aset);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
	
	result = bcm_field_group_create_id(unit, qset, group_priority, grp);
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
  
	result = bcm_field_group_action_set(unit, grp, aset);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_group_action_set\n");
		auxRes = bcm_field_group_destroy(unit, grp);
		return result;
  }
	
	result = bcm_field_entry_create_id(unit, grp, 0);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_entry_create_id\n");
		auxRes = bcm_field_entry_destroy(unit, 0);
		auxRes = bcm_field_group_destroy(unit, grp);
		return result;
  }
	
	if(in_stage == 0){
		macData[0] = 0x00;
		macData[1] = 0x00;
		macData[2] = 0x00;
		macData[3] = 0x00;
		macData[4] = 0x00;
		macData[5] = 0x11;
	} else {
		macData[0] = 0x12;
		macData[1] = 0x35;
		macData[2] = 0x00;
		macData[3] = 0x00;
		macData[4] = 0x02;
		macData[5] = 0xa5;
	}
	macMask[0] = 0xff;
	macMask[1] = 0xff;
	macMask[2] = 0xff;
	macMask[3] = 0xff;
	macMask[4] = 0xff;
	macMask[5] = 0xff;
	
	result = bcm_field_qualify_DstMac(unit,0,macData,macMask);
	if (BCM_E_NONE != result) {
		printf("Error in bcm_field_qualify_DstMac\n");
		auxRes = bcm_field_entry_destroy(unit, 0);
		auxRes = bcm_field_group_destroy(unit, grp);
		return result;
       }
       result = bcm_field_action_add(unit, 0, bcmFieldActionDrop, 0, 0);
       if (BCM_E_NONE != result) {
         printf("Error in bcm_field_action_add\n");
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

int presel_example_set(int unit)
{
	int result = BCM_E_NONE;
	int presel_id = 0;
	int in_stage = 0; /* Ingress */
	int single_port = 0; /* False - configure several ports */
	int group_id = 0; 
	int group_priority = 10;
	
	result = in_out_port_presel(unit, group_id, group_priority, presel_id, in_stage, single_port);
	if (BCM_E_NONE != result) {
		printf("Error in in_out_port_presel\n");
    return result;
  }
}

int presel_example_teardown(int unit)
{
	int result;
	
	result = bcm_field_entry_destroy(unit, 0);
	result = bcm_field_group_destroy(unit, 0);
	result = bcm_field_presel_destroy(unit, 0);
}