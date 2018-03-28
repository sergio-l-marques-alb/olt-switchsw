//cint_reset();


#include "port.c"
#include "bridge.c"

int unit=0, core=1;


global_init(unit);

bshell(unit,"vlan destroy 1"); //By default, bcm.user sets (all?) interfaces to this VID

//bshell(unit,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
//bshell(unit,"sleep 1");
bshell(unit,"1:ps xe30-40");

bcm_vswitch_init(unit);


bcm_gport_t p[3]={37,38,39};
//BCM_GPORT_MODPORT_SET(p[0], 1, 37);
//BCM_GPORT_MODPORT_SET(p[1], 1, 38);
//BCM_GPORT_MODPORT_SET(p[2], 1, 39);

//bcm_gport_t voqs_gport[3]={0x24000940,0x24000948,0x24000950};   //got these VOQs' gports from "gport" command output corresponding to above ports
bcm_gport_t voqs_gport[3]={0x24000140,0x24000148,0x24000150};
BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voqs_gport[0], core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voqs_gport[0]));
BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voqs_gport[1], core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voqs_gport[1]));
BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voqs_gport[2], core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voqs_gport[2]));


int ingress=0;
int voqs=1;
int mc_id, vsi;

//if (voqs) ingress=1;

if (ingress) vsi=mc_id=5005;
else         vsi=mc_id=4096;

///* Create Multicast group */
//if (ingress)    multicast_create(unit, &mc_id, 1, 0/*Flags*/);
//else            multicast_create(unit, &mc_id, 0 /*Egress*/, 0/*Flags*/);
//vsi=mc_id;

unsigned short vid=100;
bcm_gport_t vid_prt_gport[3]={0x44801000, 0x44801001, 0x44801002};
//{
// int i;
// for (i=0; i<3; i++) {
//     BCM_GPORT_VLAN_PORT_ID_SET(vid_prt_gport[i], p[i]<<12 | vid);
// }
//} //results in index error



printf("unit=%d\tcore=%d\tvid=%d\n\r", unit, core, vid);
printf("ingress=%d \tvoqs=%d \tvsi=%d \tmc_id=%d\n\r", ingress, voqs, vsi, mc_id);
printf("ports: \t0x%x \t0x%x \t0x%x\n\r", p[0], p[1], p[2]);
printf("VOQ gports: \t0x%x \t0x%x \t0x%x\n\r", voqs_gport[0], voqs_gport[1], voqs_gport[2]);
printf("LIF gports: \t0x%x \t0x%x \t0x%x\n\r", vid_prt_gport[0], vid_prt_gport[1], vid_prt_gport[2]);




vlan_create(unit, vid);
vlan_add(unit, vid, p[0]);
vlan_add(unit, vid, p[1]);
vlan_add(unit, vid, p[2]);




/* Create LIFs instantiating the physical ports + VLAN */
if (voqs) {
    //Pass "0" instead the target VSI to vp_add, otherwise traffic is looped back. (Perhaps different behaviour if "vswitch_create" already called
    vp_add(unit, vid_prt_gport[0] /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, voqs_gport[0] /*Port1*/, vid /*VLAN*/, 0, 0, 0, 0);
    vp_add(unit, vid_prt_gport[1] /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, voqs_gport[1] /*Port2*/, vid /*VLAN*/, 0, 0, 0, 0);
    vp_add(unit, vid_prt_gport[2] /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, voqs_gport[2] /*Port3*/, vid /*VLAN*/, 0, 0, 0, 0);
}
else {
    //Pass "0" instead the target VSI to vp_add, otherwise traffic is looped back. (Perhaps different behaviour if "vswitch_create" already called
    vp_add(unit, vid_prt_gport[0] /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, p[0] /*Port1*/, vid /*VLAN*/, 0, 0, 0, 0);
    vp_add(unit, vid_prt_gport[1] /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, p[1] /*Port2*/, vid /*VLAN*/, 0, 0, 0, 0);
    vp_add(unit, vid_prt_gport[2] /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, p[2] /*Port3*/, vid /*VLAN*/, 0, 0, 0, 0);
}

/* Create VSI and attach LIFs */
vswitch_create(unit, vsi);
vswitch_add(unit, vsi, vid_prt_gport[0] /*LIF1*/);
vswitch_add(unit, vsi, vid_prt_gport[1] /*LIF2*/);
vswitch_add(unit, vsi, vid_prt_gport[2] /*LIF3*/);


if (ingress) {
/* Create Multicast group */
    multicast_create(unit, &mc_id, 1, 0/*Flags*/);
    /* Configure ingress replication */
    if (voqs) {
        multicast_ingress_add(unit, mc_id /*MC group*/, voqs_gport[0], vid_prt_gport[0]);
        multicast_ingress_add(unit, mc_id /*MC group*/, voqs_gport[1], vid_prt_gport[1]);
        multicast_ingress_add(unit, mc_id /*MC group*/, voqs_gport[2], vid_prt_gport[2]);
    }
    else {
        multicast_ingress_add(unit, mc_id /*MC group*/, p[0], vid_prt_gport[0]);
        multicast_ingress_add(unit, mc_id /*MC group*/, p[1], vid_prt_gport[1]);
        multicast_ingress_add(unit, mc_id /*MC group*/, p[2], vid_prt_gport[2]);
    }
}
else {
/* Create Multicast group */
    multicast_create(unit, &mc_id, 0 /*Egress*/, 0/*Flags*/);
    /* Configure egress replication (MCgroup=16781312=0x1001000) */
    multicast_egress_add(unit, mc_id /*MC group*/, p[0], vid_prt_gport[0]);
    multicast_egress_add(unit, mc_id /*MC group*/, p[1], vid_prt_gport[1]);
    multicast_egress_add(unit, mc_id /*MC group*/, p[2], vid_prt_gport[2]);
}

