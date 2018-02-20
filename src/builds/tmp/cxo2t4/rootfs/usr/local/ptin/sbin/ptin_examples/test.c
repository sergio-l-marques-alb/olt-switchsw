int unit = 0;

int port_init(bcm_port_t port)
{
 bcm_gport_t gport;
 int rv = 0;

 BCM_GPORT_LOCAL_SET(gport, port);

 /* TPIDs */
 rv = bcm_port_tpid_set(unit, port, 0x8100);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_tpid_set for port %d.\n", port);
  print rv;
  return rv;
 }
 rv = bcm_port_inner_tpid_set(unit, port, 0x8100);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_inner_tpid_set for port %d.\n", port);
  print rv;
  return rv;
 }

 /* TAG mode */
 rv = bcm_port_dtag_mode_set(unit, port, BCM_PORT_DTAG_MODE_INTERNAL);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_dtag_mode_set for port %d.\n", port);
  print rv;
  return rv;
 }

 /* Filtering */
 rv = bcm_port_vlan_member_set(unit, port, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS | BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_vlan_member_set for port %d.\n", port);
  print rv;
  return rv;
 }

 /* MAC learning */
 rv = bcm_port_learn_set(unit, gport, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_tpid_set for gport 0x%x.\n", gport);
  print rv;
  return rv;
 }

 /* Default VLAN */
 rv = bcm_port_untagged_vlan_set(unit, port, 1);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_untagged_vlan_set for port %u.\n", port);
  print rv;
  return rv;
 }

 printf("Port %d initialized!\n", port);

 return rv;
}

int vlan_port_create(int port, int vlan_ext, int vlan_int, bcm_vlan_port_t *vp)
{
 bcm_gport_t gport;
 bcm_vlan_action_set_t xlate_action;
 int rv;

 BCM_GPORT_LOCAL_SET(gport, port);

 printf("port=%d/gport=0x%x\n", port, gport);

 /* Create virtual port */
 bcm_vlan_port_t_init(vp);

 vp->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
 vp->port = port;
 vp->match_vlan = vlan_ext;
 vp->egress_vlan = vlan_int; /* when forwarded to this port, packet will be set with this out-vlan */
 vp->vsi = vlan_int; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp->flags = 0;

 rv = bcm_vlan_port_create(unit, vp);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create\n");
  print rv;
  return rv;
 }

 /* Set egress vlan editor of NNI LIF to do nothing, when creating NNi LIF */                                   
 bcm_vlan_action_set_t_init(&xlate_action);
 xlate_action.dt_outer = bcmVlanActionReplace;
 xlate_action.dt_inner = bcmVlanActionNone;
 xlate_action.ot_outer = bcmVlanActionReplace;
 xlate_action.ot_inner = bcmVlanActionNone;
 xlate_action.new_outer_vlan = vlan_ext;
 xlate_action.outer_tpid = 0x8100;
 rv = bcm_vlan_translate_egress_action_add(unit, vp->vlan_port_id, vlan_int, BCM_VLAN_NONE, &xlate_action);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_vlan_translate_egress_action_add\n");
  print rv;
  return rv;
 }

 printf("vlan_port_id 0x%x created for gport 0x%x\n", vp->vlan_port_id, vp->port);
 return BCM_E_NONE;
}

int vswitch_add(int port1, int port2, int vlan1_ext, int vlan2_ext, int vlan_int)
{
 bcm_vlan_port_t vp1, vp2;
 bcm_pbmp_t pbmp, ubmp;
 int rv;

 /* Create VSWITCH instance */
 rv = bcm_vlan_create(unit, vlan_int);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vswitch_create_with_id.\n");
  print rv;
  return rv;
 }

 /* Add ports to internal VLAN */
 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_CLEAR(ubmp);
 BCM_PBMP_PORT_ADD(pbmp, port1);
 BCM_PBMP_PORT_ADD(pbmp, port2);
 rv = bcm_vlan_port_add(unit, vlan_int, pbmp, ubmp);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_vlan_port_add with vlan %d\n", vlan_int);
  print rv;
  return rv;
 }

 /* Create virtual ports */
 rv = vlan_port_create(port1, vlan1_ext, vlan_int, &vp1);
 if (rv != BCM_E_NONE) {
  printf("Error creating vlan_port for port %u\n", port1);
  print rv;
  return rv;
 }
 rv = vlan_port_create(port2, vlan2_ext, vlan_int, &vp2);
 if (rv != BCM_E_NONE) {
  printf("Error creating vlan_port for port %u\n", port2);
  print rv;
  return rv;
 }

 /* MAC learning */
 /*rv = bcm_port_learn_set(unit, vp1.vlan_port_id, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_learn_set for vlan_port_id 0x%x.\n", vp1.vlan_port_id);
  print rv;
  return rv;
 }
 rv = bcm_port_learn_set(unit, vp2.vlan_port_id, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_learn_set for vlan_port_id 0x%x.\n", vp2.vlan_port_id);
  print rv;
  return rv;
 }*/

 /* Add ports to VSWITCH instance */
 rv = bcm_vswitch_port_add(unit, vlan_int, vp1.vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_port_add vp1\n");
  print rv;
  return rv;
 }
 rv = bcm_vswitch_port_add(unit, vlan_int, vp2.vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_port_add vp2\n");
  print rv;
  return rv;
 }

 printf("Vswitch added!\n");
 return BCM_E_NONE;
}

