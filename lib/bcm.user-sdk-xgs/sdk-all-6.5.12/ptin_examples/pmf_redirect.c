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
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOutVPort);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyDstPort);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifySrcMac);
BCM_FIELD_ASET_INIT(aset);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionRedirect);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionDrop);

print bcm_field_group_create_mode_id(0, qset, group_priority, bcmFieldGroupModeAuto, group);
print bcm_field_group_action_set(0, group, aset);
print bcm_field_entry_create(0, group, &entry);

//print bcm_field_qualify_InVPort32(0, entry, 0x1000,0xffff);
//print bcm_field_qualify_OutVPort32(0, entry, 0x1003,0xffff);
print bcm_field_qualify_DstPort(0, entry, 0, 0xffffffff, 0x0c000001, 0xffffffff);
print bcm_field_action_add(0, entry, bcmFieldActionRedirect, 0, 0x);

print bcm_field_entry_install(0, entry); print entry;