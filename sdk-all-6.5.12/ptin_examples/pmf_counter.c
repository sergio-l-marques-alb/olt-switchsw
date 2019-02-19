/* Create PMF rule to attach policer */

bcm_field_qset_t qset;
bcm_field_aset_t aset;
bcm_field_group_t group;
bcm_field_entry_t entry;
uint8 mac_value[6]={0x00,0x00,0xc0, 0x02, 0x01, 0x01};
uint8 mac_mask[6]={0xff,0xff,0xff, 0xff, 0xff, 0xff};

int group_priority=6;

group=1;
BCM_FIELD_QSET_INIT(qset);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyStageIngress);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInVPort);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifySrcMac);
//BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyOutVPort);
BCM_FIELD_ASET_INIT(aset);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionStat0);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionStat1);

print bcm_field_group_create_mode_id(0, qset, group_priority, bcmFieldGroupModeAuto, group);
print bcm_field_group_action_set(0, group, aset);

/* Create Counter */

int stat_id;
int stats[12]= {bcmFieldStatAcceptedPackets, bcmFieldStatDroppedPackets,
                bcmFieldStatGreenPackets, bcmFieldStatNotGreenPackets,
                bcmFieldStatAcceptedGreenPackets, bcmFieldStatAcceptedNotGreenPackets,
                bcmFieldStatDroppedGreenPackets, bcmFieldStatDroppedNotGreenPackets,
                bcmFieldStatPackets,
                bcmFieldStatAcceptedYellowPackets, bcmFieldStatDroppedYellowPackets, bcmFieldStatDroppedRedPackets};

print bcm_field_entry_remove(0, entry);
print bcm_field_entry_destroy(0, entry);
print bcm_field_stat_destroy(0, stat_id);
print entry;

stat_id = 0x0;
print bcm_field_stat_create_id(0, group, 2, &stats, stat_id);

print bcm_field_entry_create(0, group, &entry);
//print bcm_field_qualify_InVPort32(0, entry, 0x1001, 0xffff);
print bcm_field_qualify_SrcMac(0, entry, mac_value, mac_mask);
print bcm_field_action_add(0, entry, bcmFieldActionStat0, stat_id, 0);
print bcm_field_entry_install(0, entry); print entry;

print bcm_field_stat_get32(0, stat_id, bcmFieldStatAcceptedPackets, &value); print value;

stat_id = 1;
print bcm_field_stat_create_id(0, group, 8, &stats, stat_id);

print bcm_field_entry_create(0, group, &entry);
print bcm_field_qualify_InVPort32(0, entry, 0x1001, 0xffff);
print bcm_field_action_add(0, entry, bcmFieldActionStat0, stat_id, 0);
print bcm_field_entry_stat_attach(0, entry, stat_id);
print bcm_field_entry_install(0, entry); print entry;

uint32 value;
print bcm_field_stat_get32(0, stat_id, bcmFieldStatPackets, &value); print value;
print bcm_field_stat_get32(0, 0x1000, bcmFieldStatGreenPackets, &value); print value;
print bcm_field_stat_get32(0, 0x1000, bcmFieldStatOffset0Packets, &value); print value;
print bcm_field_stat_get32(0, 0x1000, bcmFieldStatAcceptedPackets, &value); print value;
int i; for (i=0;i<42;i++) { print bcm_field_stat_get32(0, 0x1000, i, &value); printf("Counter %d: %u\n",i,value); }

uint32 val[12];
for (stat_id = 0; stat_id < 2; stat_id++) {
 print bcm_field_stat_multi_get32(0, stat_id, 8, stats, val);
 printf("stats(%u)={%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu}\n",stat_id,
		val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7],val[8],val[9],val[10],val[11]);
}
