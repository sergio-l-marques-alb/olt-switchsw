int unit=0;
int gid_vcap=-1;
int gid_icap=-1;

int ptin_cap_init(void)
{
  bcm_field_qset_t qset_icap;
  bcm_field_qset_t qset_vcap;
  bcm_error_t rv;

  BCM_FIELD_QSET_INIT(qset_icap);
  BCM_FIELD_QSET_ADD(qset_icap, bcmFieldQualifyStageIngress);
  BCM_FIELD_QSET_ADD(qset_icap, bcmFieldQualifyInPort);
  BCM_FIELD_QSET_ADD(qset_icap, bcmFieldQualifyIntPriority);
  BCM_FIELD_QSET_ADD(qset_icap, bcmFieldQualifyDstClassField);
  BCM_FIELD_QSET_ADD(qset_icap, bcmFieldQualifySrcClassField);

  rv = bcm_field_group_create_mode(unit, qset_icap, 0, bcmFieldGroupModeAuto, &gid_icap);
  printf("ICAP - bcm_field_group_create_mode\r\n");
  print rv;
  if (rv != BCM_E_NONE)
  {
    return rv;
  }
  printf("GID_ICAP=%d created\r\n", gid_icap);
  
  BCM_FIELD_QSET_INIT(qset_vcap);
  BCM_FIELD_QSET_ADD(qset_vcap, bcmFieldQualifyStageLookup);
  BCM_FIELD_QSET_ADD(qset_vcap, bcmFieldQualifyInPort);
  BCM_FIELD_QSET_ADD(qset_vcap, bcmFieldQualifyOuterVlan);
  BCM_FIELD_QSET_ADD(qset_vcap, bcmFieldQualifyOuterVlanPri);

  rv = bcm_field_group_create_mode(unit, qset_vcap, 0, bcmFieldGroupModeAuto, &gid_vcap);
  printf("VCAP - bcm_field_group_create_mode\r\n");
  print rv;
  if (rv != BCM_E_NONE)
  {
    return rv;
  }
  printf("GID_VCAP=%d created\r\n", gid_vcap);
  
  return BCM_E_NONE;
}

int ptin_create_vcap_rules(bcm_port_t bcm_port, int cos0, int cos1, int cos2, int cos3, int cos4, int cos5, int cos6, int cos7)
{
  bcm_error_t rv;
  int eid;
  int prio;
  unsigned int vlan_tag;
  int cos[8]={cos0, cos1, cos2, cos3, cos4, cos5, cos6, cos7};
  
  if (gid_vcap < 0)
  {
    printf("Run ptin_cap_init first\r\n");
	return -1;
  }
  
  /* Run all priorities */
  for (prio=0; prio<8; prio++)
  {
      /* Create rule for VCAP */
      rv = bcm_field_entry_create(unit, gid_vcap, &eid);
      printf("prio=%u - bcm_field_entry_create\r\n", prio);
      print rv;
      if (rv != BCM_E_NONE)
		return rv;
	  
	  /* Add qualifiers */
      rv = bcm_field_qualify_InPort(unit, eid, bcm_port, 0xffffffff);
      printf("prio=%u - bcm_field_qualify_InPort\r\n", prio);
      print rv;
      if (rv != BCM_E_NONE)
        return rv;
      rv = bcm_field_qualify_OuterVlanPri(unit, eid, prio, 0x7);
      printf("prio=%u - bcm_field_qualify_OuterVlanPri\r\n", prio);
      print rv;
      if (rv != BCM_E_NONE)
        return rv;
	  
      /* Add actions */
      rv = bcm_field_action_add(unit, eid, bcmFieldActionPrioIntNew, cos[prio], 0);
      printf("prio=%u - bcm_field_action_add(PrioIntNew)\r\n", prio);
      print rv;
      if (rv != BCM_E_NONE)
	return rv;
      rv = bcm_field_action_add(unit, eid, bcmFieldActionClassDestSet, cos[prio], 0);
      printf("prio=%u - bcm_field_action_add(ClassDestSet)\r\n", prio);
      print rv;
      if (rv != BCM_E_NONE)
	return rv;
		
      /* Install rule */
      rv = bcm_field_entry_install(unit, eid);
      printf("prio=%u - bcm_field_entry_install(gid=%d, eid=%d)\r\n", prio, gid_vcap, eid);
      print rv;
      if (rv != BCM_E_NONE)
	return rv;
	
      printf("Configuration done sucessfully for prio %d\r\n\n", prio);
  }

  printf("Classification rules created!\r\n");
  
  return 0;
}

int ptin_create_icap_policer(bcm_port_t bcm_port, int cos, int cir, int cbs)
{
  bcm_error_t rv;
  int eid;
  bcm_policer_config_t policer_cfg;
  int policer_id;

  if (gid_icap < 0)
  {
    printf("Run ptin_cap_init first\r\n");
    return -1;
  }  

  /* Create policer */
  bcm_policer_config_t_init(&policer_cfg);
  policer_cfg.flags        = 0;
  policer_cfg.mode         = bcmPolicerModeTrTcm;
  policer_cfg.ckbits_sec   = cir*1000;
  policer_cfg.ckbits_burst = (cbs*8)/1000;
  policer_cfg.pkbits_sec   = cir*1000;
  policer_cfg.pkbits_burst = (cbs*8)/1000;
  policer_cfg.action_id    = bcmPolicerActionRpDrop;
  policer_cfg.sharing_mode = 0;
  
  rv = bcm_policer_create(unit, &policer_cfg, &policer_id);
  printf("bcm_policer_create\r\n");
  print rv;
  if (rv != BCM_E_NONE)
    return rv;

  printf("Policer created for CoS %d (policer_id=%d)\r\n", cos, policer_id);
  
  /* Create rule for VCAP */
  rv = bcm_field_entry_create(unit, gid_icap, &eid);
  printf("bcm_field_entry_create\r\n");
  print rv;
  if (rv != BCM_E_NONE)
    return rv;
	  
  /* Add qualifiers */
  rv = bcm_field_qualify_InPort(unit, eid, bcm_port, 0xffffffff);
  printf("bcm_field_qualify_InPort\r\n");
  print rv;
  if (rv != BCM_E_NONE)
    return rv;
  rv = bcm_field_qualify_DstClassField(unit, eid, cos, 0xffffffff);
  printf("bcm_field_qualify_DstClassField\r\n");
  print rv;
  if (rv != BCM_E_NONE)
    return rv;
	  
  /* Define actions */
  rv = bcm_field_action_add(unit, eid, bcmFieldActionRpDrop, 0, 0);
  printf("bcm_field_action_add(RpDrop)\r\n");
  print rv;
  if (rv != BCM_E_NONE)
    return rv;
  
  /* Attach policer */
  rv = bcm_field_entry_policer_attach(unit, eid, 0, policer_id);
  printf("bcm_field_entry_policer_attach(policer_id=%d)\r\n", policer_id);
  print rv;
  if (rv != BCM_E_NONE)
    return rv;
			
  /* Install rule */
  rv = bcm_field_entry_install(unit, eid);
  printf("bcm_field_entry_install(gid=%d, eid=%d)\r\n", gid_icap, eid);
  print rv;
  if (rv != BCM_E_NONE)
    return rv;
  
  printf("Policer configured!\r\n");

  return 0;
}

