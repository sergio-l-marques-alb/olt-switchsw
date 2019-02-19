/* Dependencies: port.c */

void vswitch_config(int vswitch, int port1, int port2, int vlan1, int vlan2)
{
 bcm_gport_t gport1, gport2;
 bcm_vlan_port_t vp1, vp2;
 bcm_vlan_action_set_t xlate_action;
 int rv;

 BCM_GPORT_LOCAL_SET(gport1, port1);
 BCM_GPORT_LOCAL_SET(gport2, port2);

 printf("port1=%d/gport1=0x%x port2=%d/gport2=%d\n", port1, gport1, port2, gport2);

 rv = port_init(port1);
 if (rv != BCM_E_NONE)
 {
  printf("error initializing port %d.\n", port1);
  print rv;
  return rv;
 }
 rv = port_init(port2);
 if (rv != BCM_E_NONE)
 {
  printf("error initializing port %d.\n", port2);
  print rv;
  return rv;
 }

 /* Init VSWITCH */
 rv = bcm_vswitch_init(unit);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vswitch_init.\n");
  print rv;
  return rv;
 }

 /* Create VSWITCH instance */
 rv = bcm_vswitch_create_with_id(unit, vswitch);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vswitch_create_with_id.\n");
  print rv;
  return rv;
 }

 /* Create virtual port */
 bcm_vlan_port_t_init(&vp1);

 vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
 vp1.port = gport1;
 vp1.match_vlan = vlan1;
 vp1.egress_vlan = vlan1; /* when forwarded to this port, packet will be set with this out-vlan */
 vp1.vsi = vswitch; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp1.flags = 0;

 rv = bcm_vlan_port_create(unit, &vp1);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create vp1\n");
  print rv;
  return rv;
 }
 printf("vlan_port_id 0x%x created for gport 0x%x\n", vp1.vlan_port_id, vp1.port);

 /* MAC learning */
 rv = bcm_port_learn_set(unit, vp1.vlan_port_id, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_learn_set for vlan_port_id 0x%x.\n", vp1.vlan_port_id);
  print rv;
  return rv;
 }
 printf("MAC learning configured for vlan_port_id 0x%x\n", vp1.vlan_port_id);

 /* Create virtual port */
 bcm_vlan_port_t_init(&vp2);

 vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
 vp2.port = gport2;
 vp2.match_vlan = vlan2;
 vp2.egress_vlan = vlan2; /* when forwarded to this port, packet will be set with this out-vlan */
 vp2.vsi = vswitch; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp2.flags = 0;

 rv = bcm_vlan_port_create(unit, &vp2);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create vp2\n");
  print rv;
  return rv;
 }
 printf("vlan_port_id 0x%x created for gport 0x%x\n", vp2.vlan_port_id, vp2.port);

 /* MAC learning */
 rv = bcm_port_learn_set(unit, vp2.vlan_port_id, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_learn_set for vlan_port_id 0x%x.\n", vp2.vlan_port_id);
  print rv;
  return rv;
 }
 printf("MAC learning configured for vlan_port_id 0x%x\n", vp2.vlan_port_id);

 /* Configure ingress and egress translation */
 /*bcm_vlan_action_set_t_init(&xlate_action);
 xlate_action.ot_outer = bcmVlanActionReplace;
 xlate_action.ot_inner = bcmVlanActionNone;
 xlate_action.new_outer_vlan = 100;
 xlate_action.outer_pcp = bcmVlanPcpActionNone;
 xlate_action.outer_tpid = 0x8100;
 rv = bcm_vlan_translate_action_create(0, gport2, bcmVlanTranslateKeyPortOuter, 101, BCM_VLAN_NONE, &xlate_action);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vlan_translate_action_add for gport2 0x%x.\n", gport1);
  print rv;
  return rv;
 }*/

 /* set class for both ports */
 /*rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_port_class_set, port=%d, \n", port2);
  return rv;
 }
 xlate_action.new_outer_vlan = 101;
 rv = bcm_vlan_translate_egress_action_add(0, port2, 100, BCM_VLAN_NONE, &xlate_action);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vlan_translate_egress_action_add for gport2 0x%x.\n", gport1);
  print rv;
  return rv;
 }
 printf("Translations configured for gport2 0x%x\n", gport2);*/

 /* Add ports to VSWITCH instance */
 rv = bcm_vswitch_port_add(unit, vswitch, vp1.vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_port_add vp1\n");
  print rv;
  return rv;
 }
 rv = bcm_vswitch_port_add(unit, vswitch, vp2.vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_port_add vp2\n");
  print rv;
  return rv;
 }

 printf("Done!\n");
}

