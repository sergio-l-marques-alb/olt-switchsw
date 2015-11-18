/* $Id: cint_field_ecn_dm.c,v 1.34 Broadcom SDK $
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

 /* Explanation:
 *  This cint includes functions that are required to activate ecn dm with a given threshold.
 *
 * Calling Sequence:
 * For activating this mode, set the following soc properties:
 * ecn_dm_enable.BCM88650=1
 * phy_1588_dpll_frequency_lock=1
 * phy_1588_dpll_phase_initial_lo=0x40000000
 * phy_1588_dpll_phase_initial_hi=0x10000000
 * stacking_enable.BCM88650=1
 * 
 * set a threshold and call from bcm shell:
 * cint
 * int unit = 0;
 * int threshold = <my_threshold>
 * field_ecn_dm_set(unit, threshold);
 */

/* 
 * Change for IPv4 Unicast packets: 
 * - OAM action so that OAM.Sub-Type=7
 * - ECN-Capable = 1 if ECN=1 
 * - Exclude-Source = 1 if ECN=1 
 */
int oam_action_example(int unit)
{
  int i;
  int j;
  int result;
  int presel_id;
  bcm_field_group_config_t grp_de;
  bcm_field_entry_t ent_de;
  bcm_field_aset_t aset;
  bcm_field_presel_set_t psset;
  bcm_field_data_qualifier_t data_qual_de;
  bcm_field_extraction_action_t action[3];
  bcm_field_extraction_field_t extract[3];
  bcm_field_extraction_field_t extract_1[1];
  uint8 dq_data[1], dq_mask[1];
  
  /* ******************************************* */ 
  /* Preselector Configuration */

  result = bcm_field_presel_create(unit, &presel_id);
  if (BCM_E_NONE != result) 
  {
      printf("Error in bcm_field_presel_create\n");
      auxRes = bcm_field_presel_destroy(unit, presel_id);
      return result;
  }

  result = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_Stage\n");
      return result;
  }


  result = bcm_field_qualify_ForwardingType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeIp4Ucast);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qualify_ForwardingType\n");
      return result;
  }

  BCM_FIELD_PRESEL_INIT(psset);
  BCM_FIELD_PRESEL_ADD(psset, presel_id);


  /* 
   * Define the Field Group
   */
  bcm_field_group_config_t_init(&grp_de);
  grp_de.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
  grp_de.mode = bcmFieldGroupModeDirectExtraction;
  grp_de.priority = 10;
  grp_de.preselset = psset;

  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyDSCP);
  BCM_FIELD_QSET_ADD(grp_de.qset, bcmFieldQualifyIp4);





  /* Add user defined data qualifier - L4-Source-Port. 
   * We're using UDF since the L4 header is not in the 
   * typical offset, because of VxLAN parsing.- will be used for the 0b01 ecn.
   */
  bcm_field_data_qualifier_t_init(&data_qual_de); 
  data_qual_de.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
  data_qual_de.offset = 9;  
  data_qual_de.length = 2; 
  data_qual_de.offset_base = bcmFieldDataOffsetBaseFirstHeader; 
  data_qual_de.stage = bcmFieldStageIngress;
  result = bcm_field_data_qualifier_create(unit, &data_qual_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }

   /* same as above - will be used for the 0b10 ecn.
   */
  bcm_field_data_qualifier_t_init(&data_qual_de); 
  data_qual_de.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES; 
  data_qual_de.offset = 9;  
  data_qual_de.length = 2; 
  data_qual_de.offset_base = bcmFieldDataOffsetBaseFirstHeader; 
  data_qual_de.stage = bcmFieldStageIngress;
  result = bcm_field_data_qualifier_create(unit, &data_qual_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_data_qualifier_create\n");
      return result;
  }

  result = bcm_field_qset_data_qualifier_add(unit, &grp_de.qset, data_qual_de.qual_id);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_qset_data_qualifier_add\n");
      return result;
  }



  /*  Create the Field group with type Direct Extraction */
  result = bcm_field_group_config_create(unit, &grp_de);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_config_create for group %d\n", grp_de.group);
      return result;
  }

  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionOam);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionFilters);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionEcnNew);


  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp_de.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set for group %d\n", grp_de.group);
    return result;
  }

  for (j =0; j<2; j++) 
  {
      if (j ==0) /* catching 0b01 */
      {
                dq_data[0] = 0x1; 
                dq_mask[0] = 0x3; 
      }
      else if ( j == 1 ) /* catching 0b10 */
      {
                dq_data[0] = 0x2; 
                dq_mask[0] = 0x3;
      }

      for (i =0; i<3; i++) 
      {


          result = bcm_field_entry_create(unit, grp_de.group, &ent_de);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
              return result;
          }


          result = bcm_field_qualify_data(unit, ent_de,data_qual_de.qual_id,dq_data ,dq_mask,1);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_create\n");
              return result;
          }

          bcm_field_extraction_action_t_init(&(action[i]));
          bcm_field_extraction_field_t_init(&extract[i]);


          if( i == 0 ) 
          {
              action[i].action = bcmFieldActionOam;
              action[i].bias = 0x1c000;
              extract[i].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
              extract[i].bits  = 1;
              extract[i].qualifier = bcmFieldQualifyConstantZero;
          }
          else if( i == 1 ) 
          {
              action[i].action = bcmFieldActionFilters;
              extract[i].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
              extract[i].bits  = 1;
              extract[i].value = 1;
              extract[i].qualifier = bcmFieldQualifyConstantOne;

          }
          /* Set the ECN-Capable bit */
          else if ( i == 2 )
          {                 
              action[i].action = bcmFieldActionEcnNew;
              extract[i].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
              extract[i].bits  = 1;
              extract[i].value = 1;
              extract[i].qualifier = bcmFieldQualifyConstantOne;
          }

          result = bcm_field_direct_extraction_action_add(unit,
                                                          ent_de,
                                                          action[i],
                                                          1 /* count */,
                                                          &extract[i]);

          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_direct_extraction_action_add\n");
              return result;
          }



          /* Write entry to HW */
          result = bcm_field_entry_install(unit, ent_de);
          if (BCM_E_NONE != result) {
              printf("Error in bcm_field_entry_install\n");
              return result;
          } 

        }

  }


  return result;

} /* generic_field_group_de_vxlan_ecmp */


/* 
 * Main function: receives threshold as parameter for ecn dm. 
 * set soc properties according to the above documentation 
 */ 
int field_ecn_dm_set(int unit, int threshold, int oam_action){

    int rv = BCM_E_NONE;
    if (oam_action) {
        rv = oam_action_example(unit);
        if (BCM_E_NONE != rv) {
             printf("Error in oam_action_example\n");
             return rv;
        }
    }


    rv = bcm_switch_control_set(unit,bcmSwitchEcnDelayMeasurementThreshold,threshold);
    if (BCM_E_NONE != rv) {
         printf("Error in bcm_switch_control_set\n");
         return rv;
     } 

    return rv;
}

int field_ecn_dm_get(int unit, int *threshold){

    int rv = BCM_E_NONE;
    rv = bcm_switch_control_get(0,bcmSwitchEcnDelayMeasurementThreshold,threshold);
    if (BCM_E_NONE != rv) {
         printf("Error in bcm_switch_control_get\n");
         return rv;
    }
    
    return rv;
}     


