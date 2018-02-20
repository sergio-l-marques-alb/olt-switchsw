#include "port.c"
#include "bridge.c"

global_init();

bshell(0,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Create LIFs instantiating the physical ports + VLAN */
vp_add(0x44801000 /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 9  /*Port1*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801001 /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 10 /*Port2*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801002 /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 11 /*Port3*/, 100 /*VLAN*/, 0, 0, 0, 0);

/* Create VSI and attach LIFs */
vswitch_create(4096);
vswitch_add(4096, 0x44801000 /*LIF1*/);
vswitch_add(4096, 0x44801001 /*LIF2*/);
vswitch_add(4096, 0x44801002 /*LIF3*/);

/* Create Multicast group */
int mc_id=4096;
multicast_create(&mc_id, 0 /*Egress*/, 0/*Flags*/);
/* Configure egress replication (MCgroup=16781312=0x1001000) */
multicast_egress_add(mc_id /*MC group*/, 9,  0x44801000);
multicast_egress_add(mc_id /*MC group*/, 10, 0x44801001);
multicast_egress_add(mc_id /*MC group*/, 11, 0x44801002);

/* VLAN Translations */
adv_xlate_action_create(5 /*action_id*/, 1 /*is_ingress*/, bcmVlanActionReplace /*ot_outer*/, bcmVlanActionReplace /*dt_outer*/, 0 /*ot_inner*/, 0 /*dt_inner*/, bcmVlanActionReplace /*ot_outer_prio*/, bcmVlanActionReplace /*dt_outer_pri*/, 0 /*ot_inner_pri*/, 0 /*dt_inner_pri*/, pcp_out_map_id);
adv_xlate_action_class_set(2 /*Single Tag Format*/, 2 /*VEP id*/, 5 /*ActionId*/, 1 /*Ingress*/);
adv_xlate_action_class_set(6 /*Double Tag Format*/, 2 /*VEP id*/, 5 /*ActionId*/, 1 /*Ingress*/);
adv_xlate_vep(0x44801000 /*LIF1*/, 101 /*NewOVID1*/, 0 /*NewIVID1*/, 2 /*VEP Id*/, 1 /*Ingress*/);
adv_xlate_vep(0x44801001 /*LIF2*/, 102 /*NewOVID1*/, 0 /*NewIVID1*/, 2 /*VEP Id*/, 1 /*Ingress*/);
