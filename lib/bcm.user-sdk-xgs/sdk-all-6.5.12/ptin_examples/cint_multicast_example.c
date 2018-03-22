//cint_reset();


#include "port.c"
#include "bridge.c"

//global_init(1);
bshell(1,"vlan destroy 1"); //By default, bcm.user sets (all?) interfaces to this VID

//bshell(0,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
//bshell(0,"sleep 1");
bshell(1,"1:ps xe30-40");



bcm_gport_t p[3]={37,38,39};
//BCM_GPORT_MODPORT_SET(p[0], 1, 37);
//BCM_GPORT_MODPORT_SET(p[1], 1, 38);
//BCM_GPORT_MODPORT_SET(p[2], 1, 39);

bcm_gport_t voqs_gport[3]={0x24000940,0x24000948,0x24000950};   //got these VOQs' gports from "gport" command output corresponding to above ports


int ingress=0;
int voqs=0;
int mc_id, vsi;

if (voqs) ingress=1;

if (ingress) vsi=mc_id=5005;
else         vsi=mc_id=4096;

unsigned short vid=100;
bcm_gport_t vid_prt_gport[3]={0x44801000, 0x44801001, 0x44801002};
//{
// int i;
// for (i=0; i<3; i++) {
//     BCM_GPORT_VLAN_PORT_ID_SET(vid_prt_gport[i], p[i]<<12 | vid);
// }
//} //results in index error


/* Create LIFs instantiating the physical ports + VLAN */
if (voqs) {
    vp_add(1, vid_prt_gport[0] /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, voqs_gport[0] /*Port1*/, vid /*VLAN*/, 0, 0, vsi, 0);
    vp_add(1, vid_prt_gport[1] /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, voqs_gport[1] /*Port2*/, vid /*VLAN*/, 0, 0, vsi, 0);
    vp_add(1, vid_prt_gport[2] /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, voqs_gport[2] /*Port3*/, vid /*VLAN*/, 0, 0, vsi, 0);
}
else {
    vp_add(1, vid_prt_gport[0] /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, p[0] /*Port1*/, vid /*VLAN*/, 0, 0, vsi, 0);
    vp_add(1, vid_prt_gport[1] /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, p[1] /*Port2*/, vid /*VLAN*/, 0, 0, vsi, 0);
    vp_add(1, vid_prt_gport[2] /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, p[2] /*Port3*/, vid /*VLAN*/, 0, 0, vsi, 0);
}

/* Create VSI and attach LIFs */
vswitch_create(1, vsi);
vswitch_add(1, vsi, vid_prt_gport[0] /*LIF1*/);
vswitch_add(1, vsi, vid_prt_gport[1] /*LIF2*/);
vswitch_add(1, vsi, vid_prt_gport[2] /*LIF3*/);


if (ingress) {
/* Create Multicast group */
    multicast_create(1, &mc_id, 1, 0/*Flags*/);
    /* Configure ingress replication */
    if (voqs) {
        multicast_ingress_add(1, mc_id /*MC group*/, voqs_gport[0], vid_prt_gport[0]);
        multicast_ingress_add(1, mc_id /*MC group*/, voqs_gport[1], vid_prt_gport[1]);
        multicast_ingress_add(1, mc_id /*MC group*/, voqs_gport[2], vid_prt_gport[2]);
    }
    else {
        multicast_ingress_add(1, mc_id /*MC group*/, p[0], vid_prt_gport[0]);
        multicast_ingress_add(1, mc_id /*MC group*/, p[1], vid_prt_gport[1]);
        multicast_ingress_add(1, mc_id /*MC group*/, p[2], vid_prt_gport[2]);
    }
}
else {
/* Create Multicast group */
    multicast_create(1, &mc_id, 0 /*Egress*/, 0/*Flags*/);
    /* Configure egress replication (MCgroup=16781312=0x1001000) */
    multicast_egress_add(1, mc_id /*MC group*/, p[0], vid_prt_gport[0]);
    multicast_egress_add(1, mc_id /*MC group*/, p[1], vid_prt_gport[1]);
    multicast_egress_add(1, mc_id /*MC group*/, p[2], vid_prt_gport[2]);
}

