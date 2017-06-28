
/* $Id: cint_field_dir_ext_example.c,v 1.2 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

/*
 * This cint creates a Field Processor Field Group of 
 * Direct Extraction type. 
 * It demonstrate how to use BCM_FIELD_GROUP_CREATE_LARGE flag in order to create multiple extractions on the same half (80-bits) key.
 *  
 * run:
 * cint cint_field_mult_dir_ext_example.c
 * cint
 * field_mult_dir_ext_example(int unit); 
 * 
 */


/*
 * Create the direct extraction field group. configure all data qualifiers.
 * Construct the key by defining BCM_FIELD_GROUP_CREATE_LARGE flag - it will construct the key according to data qualifier id order.
 * Note that when using the flag BCM_FIELD_GROUP_CREATE_LARGE, the data qualifiers sizes are limited to 16b. 
 * In case there are qualifiers that are bigger than 16b, the user is requested to split them to 2 or more, and order them inside key using  the data qualifier ID.
 * User must order the key according to the actions. Qualifiers that are destined for an action must be sequential. 
 * Note User is expected to add zero-padding to key when qualifiers according to the action type (e.g. for Dest action, size 19b, if the qualifiers that are used sum to 15b, user is expected to add a qualifier of Zeros of size 4b).
 * Note Lost bits are allowed for the first qualifier that constructs a single action, but not in the middle of the action.
 * Unlike the FEM macro, the FES must take consecutive bits, and size is according to action types.
 * 
 */
int field_direct_extraction_lookup_set(/* in */  int unit,
		                                  /* in */  int group_priority,
		                                  /* in */ bcm_field_group_t group) 
{
  int result;
  bcm_field_group_config_t grp;
  bcm_field_aset_t aset;
  bcm_field_entry_t ent;
  bcm_field_extraction_action_t extract;
  bcm_field_extraction_field_t ext_inlif[3];
  bcm_field_data_qualifier_t data_qual[5]; /* the 5 data qualifiers */
  uint32 dq_ndx, dq_length[5] = {1, 16, 15,16,3};
  bcm_field_qualify_t dq_qualifier[5] = {bcmFieldQualifyConstantOne, bcmFieldQualifyInVPort, bcmFieldQualifyConstantZero,bcmFieldQualifyEtherType,bcmFieldQualifyOuterVlanPri};

  for (dq_ndx = 0; dq_ndx < 5; dq_ndx++) {
      bcm_field_data_qualifier_t_init(&data_qual[dq_ndx]); 
      data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES; 
      data_qual[dq_ndx].offset = 0;  
      data_qual[dq_ndx].qualifier = dq_qualifier[dq_ndx]; 
      data_qual[dq_ndx].length = dq_length[dq_ndx]; 
      result = bcm_field_data_qualifier_create(unit, &data_qual[dq_ndx]);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_data_qualifier_create\n");
          return result;
      }
  }

  bcm_field_group_config_t_init(&grp);
  grp.group = group;

  /* 
   * Define the QSET - use InLIF as qualifier. 
   */
  BCM_FIELD_QSET_INIT(grp.qset);
  BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
  for (dq_ndx = 0; dq_ndx < 5; dq_ndx++) {
      /* Add the Data qualifier to the QSET */
      result = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[dq_ndx].qual_id);
      if (BCM_E_NONE != result) {
          printf("Error in bcm_field_qset_data_qualifier_add\n");
          return result;
      }
  }

  /*
   *  Define the ASET - use counter 0.
   */
  BCM_FIELD_ASET_INIT(aset);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionForward);
  BCM_FIELD_ASET_ADD(aset, bcmFieldActionVSwitchNew);

  /*  Create the Field group with type Direct Extraction */
  grp.priority = group_priority;
  grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_LARGE;
  grp.mode = bcmFieldGroupModeDirectExtraction;
  result = bcm_field_group_config_create(unit, &grp);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_create\n");
      return result;
  }

  /*  Attach the action set */
  result = bcm_field_group_action_set(unit, grp.group, aset);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_group_action_set\n");
    return result;
  }

  /* Create the Direct Extraction entry:
   *  1. create the entry
   *  2. Construct the action:
   *     bit 0 = action valid 
   *     bits 16:0 = InLIF
   *  3. Install entry (HW configuration)
   */
  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);
  bcm_field_extraction_field_t_init(&ext_inlif[1]);
  bcm_field_extraction_field_t_init(&ext_inlif[2]);

  extract.action = bcmFieldActionStat;
  extract.bias = 0;

  for (dq_ndx = 0; dq_ndx < 2; dq_ndx++) {
      bcm_field_extraction_field_t_init(&(ext_inlif[dq_ndx]));
      ext_inlif[dq_ndx].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[dq_ndx].qualifier = data_qual[dq_ndx].qual_id;
      ext_inlif[dq_ndx].lsb = 0;
      ext_inlif[dq_ndx].bits = dq_length[dq_ndx];
  }

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  2 /* count */,
                                                  &ext_inlif);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }
  
   result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);

  extract.action = bcmFieldActionForward;
  extract.bias = 0;


      ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[0].qualifier = data_qual[2].qual_id;
      ext_inlif[0].lsb = 0;
      ext_inlif[0].bits = dq_length[2];


  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
                                                  &ext_inlif);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }


  result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);

  extract.action = bcmFieldActionVSwitchNew;
  extract.bias = 0;

  bcm_field_extraction_field_t_init(&(ext_inlif[0]));
  ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
  ext_inlif[0].qualifier = data_qual[3].qual_id;
  ext_inlif[0].lsb = 0;
  ext_inlif[0].bits = dq_length[3];

  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
                                                  &ext_inlif);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }
  
    result = bcm_field_entry_create(unit, grp.group, &ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_create\n");
      return result;
  }

  bcm_field_extraction_action_t_init(&extract);
  bcm_field_extraction_field_t_init(&ext_inlif[0]);

  extract.action = bcmFieldActionForward;
  extract.bias = 0;


      ext_inlif[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
      ext_inlif[0].qualifier = data_qual[4].qual_id;
      ext_inlif[0].lsb = 0;
      ext_inlif[0].bits = dq_length[4];


  result = bcm_field_direct_extraction_action_add(unit,
                                                  ent,
                                                  extract,
                                                  1 /* count */,
                                                  &ext_inlif);

  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_direct_extraction_action_add\n");
      return result;
  }

  /* Write entry to HW */
  result = bcm_field_entry_install(unit, ent);
  if (BCM_E_NONE != result) {
      printf("Error in bcm_field_entry_install\n");
      return result;
  }
  
  return result;
}




/*
 *  Set an example configuration of this cint
 */
int field_mult_dir_ext_example(int unit) 
{
  int result;
  int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
  bcm_field_group_t group = 1;
  
  /*configure Direct Extraction Lookup*/
  result = field_direct_extraction_lookup_set(unit, group_priority, group);
  if (BCM_E_NONE != result) {
      printf("Error in field_direct_extraction_lookup_set\n");
      return result;
  }

  return result;
}

