/* Create Policer */
bcm_policer_t policer_id;
bcm_policer_config_t pol_cfg;

policer_id=30;
bcm_policer_config_t_init(&pol_cfg);
pol_cfg.max_pkbits_sec = 45000;
pol_cfg.max_ckbits_sec = 15000;
pol_cfg.pkbits_sec=45000;
pol_cfg.pkbits_burst=1000;
pol_cfg.ckbits_sec=15000;
pol_cfg.ckbits_burst=1000;
pol_cfg.mode=bcmPolicerModeTrTcmDs;
pol_cfg.flags=BCM_POLICER_WITH_ID /*| BCM_POLICER_COLOR_BLIND*/;
print bcm_policer_create(0, &pol_cfg, &policer_id); print policer_id;


/* Create PMF rule to attach policer */

bcm_field_qset_t qset;
bcm_field_aset_t aset;
bcm_field_group_t group;
bcm_field_entry_t entry;
int group_priority=6;

group=1;
BCM_FIELD_QSET_INIT(qset);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyStageIngress);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInVPort);
//BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOutVPort);
BCM_FIELD_ASET_INIT(aset);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionPolicerLevel0);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionUsePolicerResult);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionOuterVlanPrioNew);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionVlanActionSetNew);

print bcm_field_group_create_mode_id(0, qset, group_priority, bcmFieldGroupModeAuto, group);
print bcm_field_group_action_set(0, group, aset);
print bcm_field_entry_create(0, group, &entry);

print bcm_field_qualify_InVPort32(0, entry, 0x1000, 0xffff);
//print bcm_field_qualify_OutVPort32(0, entry, 0x1000, 0xffff);
print bcm_field_entry_policer_attach(0, entry, 0, (policer_id & 0xffff));
print bcm_field_action_add(0, entry, bcmFieldActionVlanActionSetNew, 5 /*Action ID*/, 0);
//print bcm_field_action_add(0, entry, bcmFieldActionPolicerLevel0, 0, 0);
//print bcm_field_action_add(0, entry, bcmFieldActionUsePolicerResult, 0, 0);

print bcm_field_entry_install(0, entry); print entry;

