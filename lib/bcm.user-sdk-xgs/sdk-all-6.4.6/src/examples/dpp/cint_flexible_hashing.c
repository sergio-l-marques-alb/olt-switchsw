/* $Id: cint_stateful_load_balancing.c,v 1.3 Broadcom SDK $
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
 * $
*/

/**
 * This CINT details an example of flexible hashing. 
 * Flexible hashing utilizes the key construction of stateful-load-balancing 
 * without learning or looking-up the keys. 
 * Once a key has been constructed in the SLB, it may be copied to the 80 msbs 
 * of one (or more) of the PMF keys in the 2nd cycle. 
 *  
 * In this example, the SLB will be setup to generate some key. Next the key 
 * will be copied to one of the PMF keys, and from there to the LB-key, which 
 * will affect the ECMP load balancing of a setup ECMP group. 
 *  
 * Detailed Flow: 
 * 1. Setup SLB hash key generation as described in cint_stateful_load_balancing (slb_field_group_example). 
 * 2. Setup a PMF database to copy the 20 lsbs of the hashing 
 * key to the PMF ECMP LB key. 
 * 3. Set the ECMP LB key source to be the PMF ECMP LB key. 
 *  
 * SOC PROPERTIES
 * No soc properties are required.
 *  
 * run: 
   cd ../../../../src/examples/dpp 
   cint 
   #include cint_stateful_load_balancing.c
   #include cint_flexible_hashing.c
   int unit = 0;
   
   print slb_field_group_example(unit); 
   print setup_flexible_hash_database_in_pmf(unit); 
   print bcm_switch_control_set(0, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_NONE);

*/

/**
 * Setup a database in the PMF to copy the 20 lsbs of the final 
 * SLB hashing key to the PMF ECMP LB key. 
 */ 
int setup_flexible_hash_database_in_pmf(int unit)
{
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_extraction_field_t ext_qual;
    bcm_field_extraction_action_t ext_act;
    int group_priority = 33;
    int result;

    bcm_field_group_config_t_init(&grp);
    bcm_field_extraction_field_t_init(&ext_qual);
    bcm_field_extraction_action_t_init(&ext_act);

    grp.group = -1;

    /* Define the QSET */
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyHashValue);
    /* Define the ASET */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionMultipathHashAdditionalInfo);

    /* Use direct extraction. */
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    result = bcm_field_group_config_create(unit, &grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_create\n");
        bcm_field_group_destroy(unit, grp.group);
        return result;
    }

    result = bcm_field_entry_create(unit, grp.group, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        bcm_field_group_destroy(unit, grp.group);
        return result;
    }

    /* Build the action to set the PMF ECMP LB key. */
    ext_act.action = bcmFieldActionMultipathHashAdditionalInfo;
    ext_act.bias = 0;

    /* Build the source for the direct extraction to be the 20 lsbs of the SLB hashing value. */
    ext_qual.flags = 0;
    ext_qual.bits = 20;
    ext_qual.lsb = 0;
    ext_qual.qualifier = bcmFieldQualifyHashValue;
    result = bcm_field_direct_extraction_action_add(unit,
            ent,
            ext_act,
            1 /* count */,
            ext_qual);

    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_direct_extraction_action_add\n");
        bcm_field_group_destroy(unit, grp.group);
        return result;
    } 

    result = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != result) {
        bcm_field_group_destroy(unit, grp.group);
        return result;
    }

    print grp.group;
    return result;
}

/* Depends on cint_l3_multipath.c */
int flex_hash_l3_multipath_dvapi(int unit, int inPort, int vlan, int outPort0, int outPort1)
{
  int rc;
  int in_port = inPort;
  int nof_paths = 2;
  int out_port_arr[2] = {outPort0,outPort1};
  int modid_arr[2] = {unit,unit};

  rc = l3_multipath_run(unit,-1,inPort,vlan,nof_paths,out_port_arr,modid_arr);
  if (BCM_E_NONE != rc) {
      printf("Error in l3_multipath_run\n");
      return rc;
  }

  rc = bcm_switch_control_set(0, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_NONE);
  if (BCM_E_NONE != rc) {
      printf("Error in bcm_switch_control_set\n");
      return rc;
  }

  return rc;
}

