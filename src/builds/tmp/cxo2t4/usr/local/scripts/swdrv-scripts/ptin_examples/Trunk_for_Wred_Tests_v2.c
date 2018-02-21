//////////////TRUNK///SDK657////WRED//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

cint_reset();

#include <port.c>
#include <bridge.c>
#include <trunk.c>

global_init();
bshell (unit,"port xe1-xe12 speed=1000 rxpause=0 ls=sw txpause=0 en=1");
bshell (unit,"port xe4-xe5 speed=10000 rxpause=0 ls=sw txpause=0 en=1");
bshell (unit,"ps");
bshell (0,"gport");


/* Check if double lb key range is enabled (relevant for gracefull load balancing) */
print bcm_switch_control_set(0, bcmSwitchMcastTrunkIngressCommit, 0);
print bcm_switch_control_set(0, bcmSwitchMcastTrunkEgressCommit, 0);

/* Define VLAN-domains */
print bcm_port_class_set(0,1,bcmPortClassId,9);
print bcm_port_class_set(0,2,bcmPortClassId,9);
print bcm_port_class_set(0,3,bcmPortClassId,9);
print bcm_port_class_set(0,4,bcmPortClassId,9);

trunk_create(1,9);
trunk_member_add(1,1);
//trunk_member_add(1,2);
//trunk_member_add(1,3);

vp_add(0x44801000,3,0xc000001,100,0,0,0,0);//LIF TRUNK
vp_add(0x44801001,3,4,100,0,0,0,0);
vp_add(0x44801002,3,3,100,0,0,0,0);
vp_add(0x44801003,3,2,100,0,0,0,0);

vswitch_create(4096);
vswitch_add(4096,0x44801000);
vswitch_add(4096,0x44801001);
vswitch_add(4096,0x44801002);
vswitch_add(4096,0x44801003);

multicast_create(4096,0,0);
multicast_egress_add(16781312,0xc000001,0x44801000);
multicast_egress_add(16781312,        4,0x44801001);
multicast_egress_add(16781312,        3,0x44801002);
multicast_egress_add(16781312,        2,0x44801003);


/* Use MACSA to calculate hash for L2 packets */
//PG116
//
print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, 0);
print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI);

//print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, 0);
//print bcm_switch_control_set(unit, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI | BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI);

//           bshell (unit,"l2 clear all");

bshell (unit,"l2 show");
bshell (unit,"dune \"ppd_api frwrd_mact get_block\" ");
bshell (unit,"diag pp last");
bshell (unit,"diag pp fdt");
bshell (unit,"diag count g");
bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=1"); //STOP
bshell (unit,"diag cosq non");
bshell (unit,"m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=0"); //START




///////MAPPING COLORs
#include "ptin_color_map_v1.c"
ptin_Map_Switch_Port_Conf();

char l2_pckt_prio[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
char l2_pckt_cfi[16] =  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
char l2_int_prio[16] =  {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
char l2_int_color[16] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
ptin_qos_map_t qos_map_Linear;
sal_memcpy (qos_map_Linear.l2_pckt_prio,l2_pckt_prio,16);
sal_memcpy (qos_map_Linear.l2_pckt_cfi,  l2_pckt_cfi,16);
sal_memcpy (qos_map_Linear.l2_int_prio,  l2_int_prio,16);
sal_memcpy (qos_map_Linear.l2_int_color,l2_int_color,16);

//Map For Ingress Side: PrioPACK2PrioINT
int ingress_map_id;
print  ptin_qos_map_create(0, &qos_map_Linear, BCM_QOS_MAP_INGRESS,BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG, &ingress_map_id);
print bcm_qos_port_map_set(0/*unit*/, 0x44801000/*LIF*/, ingress_map_id /*ingress maping*/, -1 /*egress mapping*/);
print bcm_qos_port_map_set(0/*unit*/, 0x44801001/*LIF*/, ingress_map_id /*ingress maping*/, -1 /*egress mapping*/);
print bcm_qos_port_map_set(0/*unit*/, 0x44801002/*LIF*/, ingress_map_id /*ingress maping*/, -1 /*egress mapping*/);
print bcm_qos_port_map_set(0/*unit*/, 0x44801003/*LIF*/, ingress_map_id /*ingress maping*/, -1 /*egress mapping*/);


bshell (unit,"diag cosq non");
ptin_qos_map_t LIF_2_VoQ_map_linear;
char VoQ_map_pckt_prio[16] = {0, 1, 2, 3, 4, 5, 6, 7,     0, 0, 0, 0, 0, 0, 0, 0};
char VoQ_map_int_prio[16] =  {0, 1, 2, 3, 4, 5, 6, 7,     0, 0, 0, 0, 0, 0, 0, 0};
sal_memcpy (LIF_2_VoQ_map_linear.l2_pckt_prio,VoQ_map_pckt_prio,16);
sal_memcpy (LIF_2_VoQ_map_linear.l2_int_prio,  VoQ_map_int_prio,16);
print  ptin_map_LIF_IntPrio_2_VoQBundle(0, 0x241c0020,&LIF_2_VoQ_map_linear);
///////MAPPING COLORs





	