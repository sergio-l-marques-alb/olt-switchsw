#include "port.c"
#include "bridge.c"

global_init();

bshell(0,"port xe9,xe10 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Create LIFs instantiating the physical ports + VLAN */
vp_add(0x44801000 /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000060 /*Port1*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801001 /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000068 /*Port2*/, 100 /*VLAN*/, 0, 0, 0, 0);

/* Create VSI and attach LIFs */
vswitch_create(4096);
vswitch_add(4096, 0x44801000 /*LIF1*/);
vswitch_add(4096, 0x44801001 /*LIF2*/);

/* Create Multicast group */
multicast_create(4096, 1 /*Ingress*/);
/* Configure egress replication (MCgroup=16781312=0x1001000) */
multicast_ingress_add(0x1001000 /*MC group*/, 0x24000060, 0x44801000);
multicast_ingress_add(0x1001000 /*MC group*/, 0x24000068, 0x44801001);


/* Create Multicast group */
//multicast_create(4096, 0 /*Egress*/);
/* Configure egress replication (MCgroup=16781312=0x1001000) */
//multicast_egress_add(0x1001000 /*MC group*/, 9 , 0x44801000);
//multicast_egress_add(0x1001000 /*MC group*/, 10, 0x44801001);


//m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=1
//diag cosq non
//m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=0

bcm_field_qset_t qset;
bcm_field_aset_t aset;
bcm_field_group_t group;
bcm_field_entry_t entry;
int group_priority=6;
bcm_field_extraction_field_t extractions[3];
bcm_field_extraction_action_t action;

BCM_FIELD_QSET_INIT(qset);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyStageIngress);
//BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyInVPort);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyFlowId);
BCM_FIELD_QSET_ADD(qset,bcmFieldQualifyIntPriority);
BCM_FIELD_ASET_INIT(aset);
BCM_FIELD_ASET_ADD(aset,bcmFieldActionStat0);

group=1;
print bcm_field_group_create_mode_id(0, qset, group_priority, bcmFieldGroupModeAuto | bcmFieldGroupModeDirectExtraction, group);
print bcm_field_group_action_set(0, group, aset);

/* Create Counters */
int stat_id, stat_id_base, stat_number, stat_step;
int stats[2]= {bcmFieldStatPackets,bcmFieldStatBytes};
uint64 val[2];

stat_id_base = 0x60>>3;
stat_number = 2;
stat_step = 1;
for (stat_id=stat_id_base; stat_id<stat_id_base+stat_number; stat_id+=stat_step)  print bcm_field_stat_create_id(0, group, 2, &stats, stat_id);
//for (stat_id=stat_id_base; stat_id<stat_id_base+stat_number; stat_id+=2)  print bcm_field_stat_destroy(0, stat_id);

print bcm_field_entry_create(0, group, &entry);

bcm_field_extraction_field_t_init(&(extractions[0]));
extractions[0].flags=BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
extractions[0].value=1;
extractions[0].bits=1;
bcm_field_extraction_field_t_init(&extractions[1]);
extractions[1].flags=0;
extractions[1].qualifier=bcmFieldQualifyFlowId; //bcmFieldQualifyInVPort;
extractions[1].lsb=3;
extractions[1].bits=14;
action.action=bcmFieldActionStat0;
action.bias=0;
print bcm_field_direct_extraction_action_add(0, entry, action, 2, extractions);

print bcm_field_qualify_IntPriority(0, entry, 0x2, 0x7);

print bcm_field_entry_install(0, entry); print entry;

for (stat_id=stat_id_base; stat_id<stat_id_base+stat_number; stat_id+=stat_step)  { bcm_field_stat_multi_get(0, stat_id, 2, stats, val); print val; }

