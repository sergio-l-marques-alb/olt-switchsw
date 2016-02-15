/* Dependencies: port.c + bridge.c */

int vlan_port_create(int port, int vlan_ing, int inner_vlan_ing, int vlan_egr, int inner_vlan_egr, bcm_vlan_port_t *vp)
{
 bcm_vlan_action_set_t xlate_action;
 int rv;

 /* Create virtual port */
 bcm_vlan_port_t_init(vp);

 vp->criteria = (inner_vlan_ing>0) ? BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED : BCM_VLAN_PORT_MATCH_PORT_VLAN;
 vp->port = port;
 vp->match_vlan = vlan_ing;
 vp->match_inner_vlan = inner_vlan_ing;
 vp->egress_vlan = vlan_ing; /* when forwarded to this port, packet will be set with this out-vlan */
 vp->egress_inner_vlan = inner_vlan_ing;
 vp->vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp->flags = 0;

 rv = bcm_vlan_port_create(unit, vp);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create\n");
  print rv;
  return rv;
 }

 if (!advanced_vlan_editing)
 {
  /* Set egress vlan editor of NNI LIF to do nothing, when creating NNi LIF */
  if (vlan_egr > 0 && vlan_egr < 4096)
  {
   vp->egress_vlan = vlan_egr;

   bcm_vlan_action_set_t_init(&xlate_action);
   xlate_action.ut_outer = bcmVlanActionAdd;
   xlate_action.ot_outer = bcmVlanActionReplace;
   xlate_action.dt_outer = bcmVlanActionReplace;
   xlate_action.new_outer_vlan = vlan_egr;
   xlate_action.outer_tpid_action = bcmVlanTpidActionModify;
   if (inner_vlan_egr > 0 && inner_vlan_egr < 4096)
   {
    xlate_action.ut_inner = bcmVlanActionAdd;
    xlate_action.ot_inner = bcmVlanActionAdd;
    xlate_action.dt_inner = bcmVlanActionReplace;
    xlate_action.inner_tpid_action = bcmVlanTpidActionModify;
   }
   xlate_action.outer_tpid = outer_tpid;
   xlate_action.inner_tpid = inner_tpid;
   
   rv = bcm_vlan_translate_egress_action_add(unit, vp->vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &xlate_action);
   if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add of vp\n");
    print rv;
    return rv;
   }
  }
 }

 printf("vlan_port_id 0x%x created for port 0x%x, outerVLAN=%u, innerVLAN=%u\n", vp->vlan_port_id, vp->port, vp->match_vlan, vp->match_inner_vlan);

 return BCM_E_NONE;
}

int vlan_port_destroy(unsigned int vlan_port_id)
{
 int rv;

 if (!advanced_vlan_editing)
 {
  rv = bcm_vlan_translate_egress_action_delete(unit, vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE);
  if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
   printf("Error, in bcm_vlan_translate_egress_action_delete\n");
   print rv;
   return rv;
  }
 }

 rv = bcm_vlan_port_destroy(unit, vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_destroy\n");
  print rv;
  return rv;
 }

 printf("vlan_port_id 0x%x destroyed\n", vlan_port_id);

 return BCM_E_NONE;
}

int vlan_port_find(unsigned int vlan_port_id, bcm_vlan_port_t *vp)
{
 int rv;

 bcm_vlan_port_t_init(vp);

 vp->vlan_port_id = vlan_port_id;
 vp->criteria = 0;
 vp->vsi = 0;
 vp->flags = 0;
 rv = bcm_vlan_port_find(unit, vp);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_find\n");
  print rv;
  return rv;
 }
 printf("vlan_port_id 0x%x belongs to port=%u, vlan=%u+%u, egress_vlan=%u+%u (flags=0x%x)\n", vlan_port_id,
        vp->port, vp->match_vlan, vp->match_inner_vlan, vp->egress_vlan, vp->egress_inner_vlan, vp->flags);

 return BCM_E_NONE;
}


int vswitch_add(int port, int vlan, int inner_vlan, int vsi)
{
 bcm_vlan_port_t vp;
 bcm_pbmp_t pbmp, ubmp;
 bcm_vlan_action_set_t xlate_action;
 int rv;

 /* Create VSWITCH instance */
 rv = vswitch_create(vsi);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("error: bcm_vswitch_create_with_id.\n");
  print rv;
  return rv;
 }

 /* Create VLANs */
 if (vsi < 4096)
 {
  /* Add ports to its VLAN */
  rv = vlan_add(vsi, port);
  if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
   printf("Error, in vlan_add(vsi=%u, port=%u)\n", vsi, port);
   print rv;
   return rv;
  }
 }

 /* Create virtual ports */
 rv = vlan_port_create(port, vlan, inner_vlan, 0, 0, &vp);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error creating vlan_port for port %u\n", port);
  print rv;
  return rv;
 }

 /* MAC learning */
 rv = bcm_port_learn_set(unit, vp.vlan_port_id, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_port_learn_set for vlan_port_id 0x%x.\n", vp.vlan_port_id);
  print rv;
  return rv;
 }

 /* Add ports to VSWITCH instance */
 rv = bcm_vswitch_port_add(unit, vsi, vp.vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_port_add\n");
  print rv;
  return rv;
 }

 printf("Port %u + Vlan %u + InnerVlan %u (vlan_port=0x%x) added to VSI %u!\n", port, vlan, inner_vlan, vp.vlan_port_id, vsi);

 return BCM_E_NONE;
}

int vswitch_remove(int vlan_port_id, int vsi)
{
 bcm_pbmp_t pbmp, ubmp;
 int rv;

 /* Remove port from VSWITCH instance */
 rv = bcm_vswitch_port_delete(unit, vsi, vlan_port_id);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error, bcm_vswitch_port_delete\n");
  print rv;
  return rv;
 }

 /* Destroy virtual port */
 rv = vlan_port_destroy(vlan_port_id);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error destroying vlan_port %u\n", vlan_port_id);
  print rv;
  return rv;
 }

 printf("vlan_port_id 0x%x removed from VSI %u!\n", vlan_port_id, vsi);

 return BCM_E_NONE;
}

int crossconnect_add(int port1, int port2, int vlan1, int vlan2, int inner_vlan1, int inner_vlan2)
{
 bcm_vlan_port_t vp1, vp2;
 int rv;

 /* Create virtual ports */
 rv = vlan_port_create(port1, vlan1, inner_vlan1, 0, 0, &vp1);
 if (rv != BCM_E_NONE) {
  printf("Error creating vlan_port for port %u\n", port1);
  print rv;
  return rv;
 }
 rv = vlan_port_create(port2, vlan2, inner_vlan2, 0, 0, &vp2);
 if (rv != BCM_E_NONE) {
  printf("Error creating vlan_port for port %u\n", port2);
  print rv;
  return rv;
 }

 /* Add ports to its VLAN */
 rv = vlan_add(vlan1, port1);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error, in vlan_add of port %u to vlan %d\n", port1, vlan1);
  print rv;
  return rv;
 }
 rv = vlan_add(vlan2, port2);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error, in vlan_add of port %u to vlan %d\n", port2, vlan2);
  print rv;
  return rv;
 }

 /* Add crossconnect */
 rv = cc_add(vp1.vlan_port_id, vp2.vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, cc_add\n");
  print rv;
  return rv;
 }

 printf("Cross-connect between vlan_port 0x%x and 0x%x configured\n", vp1.vlan_port_id, vp2.vlan_port_id);

 return BCM_E_NONE;
}

int crossconnect_remove(int vlan_port_1, int vlan_port_2)
{
 bcm_vlan_port_t vp1, vp2;
 bcm_vswitch_cross_connect_t cross_connect;
 int rv;

 /* Get Virtual port informartion */
 rv = vlan_port_find(vlan_port_1, &vp1);
 if (rv != BCM_E_NONE) {
  printf("Error, vlan_port_find(&vp1)\n");
  print rv;
  return rv;
 }
 rv = vlan_port_find(vlan_port_2, &vp2);
 if (rv != BCM_E_NONE) {
  printf("Error, vlan_port_find(&vp2)\n");
  print rv;
  return rv;
 }

 /* Remove crossconnect */
 rv = cc_(vlan_port_1, vlan_port_2);
 if (rv != BCM_E_NONE) {
  printf("Error, cc_remove\n");
  print rv;
  return rv;
 }

 /* Destroy virtual ports */
 rv = vlan_port_destroy(vlan_port_1);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error destroying vlan_port 0x%x\n", vlan_port_1);
  print rv;
  return rv;
 }
 rv = vlan_port_destroy(vlan_port_2);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error destroying vlan_port 0x%x\n", vlan_port_2);
  print rv;
  return rv;
 }

 /* Remove ports from VLANs */
 rv = vlan_remove(vp1.match_vlan, vp1.port);
 if (rv != BCM_E_NONE) {
  printf("Error, vlan_remove port1\n");
  print rv;
  return rv;
 }
 rv = vlan_remove(vp2.match_vlan, vp2.port);
 if (rv != BCM_E_NONE) {
  printf("Error, vlan_remove port2\n");
  print rv;
  return rv;
 }

 printf("Cross-connect between vlan_ports 0x%x and 0x%x deconfigured\n", vlan_port_1, vlan_port_2);

 return BCM_E_NONE;
}

