/* Create PMF rule to attach policer */

bcm_field_qset_t qset;
bcm_field_aset_t aset;
bcm_field_group_t group;
bcm_field_entry_t entry;
int group_priority=6;

bcm_mac_t macData={0x00,0x01,0x02,0x03,0x04,0x05};
bcm_mac_t macMask={0xff,0xff,0xff,0xff,0xff,0xff};

group=1;
BCM_FIELD_QSET_INIT(qset);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyStageIngress);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInVPort);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyDstMac);
BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyVlanFormat);
BCM_FIELD_ASET_INIT(aset);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionPrioIntNew);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionOuterVlanPrioNew);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionVlanActionSetNew);

print bcm_field_group_create_mode_id(0, qset, group_priority, bcmFieldGroupModeAuto, group);
print bcm_field_group_action_set(0, group, aset);
print bcm_field_entry_create(0, group, &entry); print entry;

print bcm_field_qualify_InVPort32(0, entry, 0x1000, 0xffff);
//print bcm_field_qualify_DstMac(0, entry, macData, macMask);
//print bcm_field_qualify_VlanFormat(0, entry, BCM_FIELD_VLAN_FORMAT_INNER_TAGGED, -1);
print bcm_field_action_add(0, entry, bcmFieldActionVlanActionSetNew, 5 /*Action ID*/, 0);
//print bcm_field_action_add(0, entry, bcmFieldActionOuterVlanPrioNew, 0x7 /* PCP */, 0x1 /* DEI */);
//print bcm_field_action_add(0, entry, bcmFieldActionPrioIntNew, 0x7 /* Prio */, 0);

print bcm_field_entry_install(0, entry); print entry;
