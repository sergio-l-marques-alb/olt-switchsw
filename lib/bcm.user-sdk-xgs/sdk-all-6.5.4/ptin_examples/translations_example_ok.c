#include "port.c"
#include "bridge.c"

bshell(0,"port xe en=1 speed=1000 rxpause=0 txpause=0");

global_init();

vlan_create(2 /*VSI*/);
vp_add(0x44801000 /*LIF1*/, BCM_VLAN_PORT_MATCH_PORT_VLAN /*Criteria*/, 9  /*Port1*/, 100 /*OVID1*/, 0 /*TunnelID*/, 0 /*VSI*/, 0 /*Flags*/);
vp_add(0x44801001 /*LIF2*/, BCM_VLAN_PORT_MATCH_PORT_VLAN /*Criteria*/, 10 /*Port2*/, 333 /*OVID2*/, 0 /*TunnelID*/, 0 /*VSI*/, 0 /*Flags*/);
vlan_add(2 /*VSI*/,  9 /*Port1*/);
vlan_add(2 /*VSI*/, 10 /*Port2*/);
vswitch_add(2 /*VSI*/, 0x44801000 /*LIF1*/);
vswitch_add(2 /*VSI*/, 0x44801001 /*LIF2*/);

/* ActionID=2: Replace outer VLAN, nothing done to inner VLAN */
adv_xlate_action_create(2 /*ActionId*/, 1 /*ut_outer=ADD*/, 0 /*ut_inner=NONE*/, 2 /*ot_outer=REPLACE*/, 0 /*ot_inner=NONE*/, 2 /*dt_outer=REPLACE*/, 0 /*dt_inner=NONE*/,
                        outer_tpid /*OuterTPID*/, inner_tpid /*InnerTPID*/,
                        0 /*NewOVID*/, 0 /*NewIVID*/,
                        0 /*Egress*/);

/* VLAN Edit Profile (ID=2) for LIF1 */
adv_xlate_vep(0x44801000 /*LIF1*/, 100 /*NewOVID1*/, 0 /*NewIVID1*/, 2 /*VEP Id*/, 0 /*Egress*/);
/* VLAN Edit Profile (ID=2) for LIF2 */
adv_xlate_vep(0x44801001 /*LIF2*/, 333 /*NewOVID2*/, 0 /*NewIVID2*/, 2 /*VEP Id*/, 0 /*Egress*/);

/* Associate Tag Format to Action and to VLAN Edit Profile */
/* Tag format 2 -> Single tagged: tpid1=0x8100, tpid2=BCM_PORT_TPID_CLASS_TPID_ANY */
/* Tag format 6 -> Double tagged: tpid1=0x8100, tpid2=0x8100 */
adv_xlate_action_class_set(2 /*Single Tag Format*/, 2 /*VEP*/, 2 /*ActionId*/, 0 /*Egress*/);
adv_xlate_action_class_set(6 /*Double Tag Format*/, 2 /*VEP*/, 2 /*ActionId*/, 0 /*Egress*/);
