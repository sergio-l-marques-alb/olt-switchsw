/* Dependencies: port.c */

/*
 * Create a LIF
 */
int vp_add(int lif_id, int criteria, int port, int vlan_ing, int inner_vlan_ing, int tunnel_id, int vsi, unsigned int flags)
{
 bcm_vlan_port_t vp;
 int rv;

 /* Create virtual port */
 bcm_vlan_port_t_init(&vp);

 if (criteria <= 0 || criteria >= BCM_VLAN_PORT_MATCH_COUNT)
 {
  if (inner_vlan_ing > 0)  criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
  else if (vlan_ing > 0)   criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  else if (vlan_ing < 0)   criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
  else                     criteria = BCM_VLAN_PORT_MATCH_PORT;
 }

 vp.criteria = criteria;
 vp.port = port;
 if (vlan_ing > 0)
  vp.match_vlan = vlan_ing;
 else if (vlan_ing < 0)
  vp.match_vlan = abs(vlan_ing);
 else
  vp.match_vlan = BCM_VLAN_NONE;
 vp.match_inner_vlan   = (inner_vlan_ing<=0 || inner_vlan_ing>4095) ? BCM_VLAN_NONE : inner_vlan_ing;
 vp.match_tunnel_value = tunnel_id;
 vp.egress_vlan        = vp.match_vlan;
 vp.egress_inner_vlan  = vp.match_inner_vlan;
 vp.egress_tunnel_value= vp.match_tunnel_value;
 vp.vsi = vsi; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp.vlan_port_id = lif_id;
 vp.flags = (lif_id != 0) ? (flags | BCM_VLAN_PORT_WITH_ID) : flags;

 rv = bcm_vlan_port_create(unit, &vp);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create\n");
  print rv;
  return rv;
 }

 rv = bcm_vlan_control_port_set(unit, vp.vlan_port_id, bcmVlanPortDoubleLookupEnable, (criteria != BCM_VLAN_PORT_MATCH_PORT));
 if (rv != BCM_E_NONE)
 {
  printf("Error configuring bcmVlanPortDoubleLookupEnable=%u to lif 0x%x: rv=%d\n", (criteria != BCM_VLAN_PORT_MATCH_PORT), vp.vlan_port_id, rv);
  return 0;
 }

 printf("vlan_port_id 0x%x created for port 0x%x, outerVLAN=%d, innerVLAN=%d, tunnel=%d (VSI=%u, criteria=%u flags=0x%x)\n", vp.vlan_port_id,
        vp.port, vp.match_vlan, vp.match_inner_vlan, vp.match_tunnel_value, vp.vsi, criteria, vp.flags);

 return BCM_E_NONE;
}

/*
 * Remove a LIF
 */
int vp_remove(unsigned int vlan_port_id)
{
 int rv;

 rv = bcm_vlan_port_destroy(unit, vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_destroy\n");
  print rv;
  return rv;
 }

 printf("vlan_port_id 0x%x destroyed\n", vlan_port_id);

 return BCM_E_NONE;
}

/*
 * ???
 */
int port_match_add(int port, int vlan, int vlan_port_id)
{
 bcm_port_match_info_t match_info;
 int rv;

 bcm_port_match_info_t_init(&match_info);

 match_info.match = BCM_PORT_MATCH_PORT;
 match_info.port = port;
 match_info.match_vlan = vlan;
 match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
 rv = bcm_port_match_add(unit, vlan_port_id, &match_info); /* associating the port to lif */
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_port_match_add\n");
  print rv;
  return rv;
 }
 printf("Port match added\n");
 return BCM_E_NONE;
}

/*
 * ???
 */
int port_match_remove(int port, int vlan, int vlan_port_id)
{
 bcm_port_match_info_t match_info;
 int rv;

 bcm_port_match_info_t_init(&match_info);

 match_info.match = BCM_PORT_MATCH_PORT;
 match_info.port = port;
 match_info.match_vlan = vlan;
 match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
 rv = bcm_port_match_delete(unit, vlan_port_id, &match_info); /* associating the port to lif */
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_port_match_delete\n");
  print rv;
  return rv;
 }
 printf("Port match removed\n");
 return BCM_E_NONE;
}

/*
 * Create a cross-connect
 */ 
int cc_add(unsigned int vlan_port_1, unsigned int vlan_port_2)
{
 bcm_vswitch_cross_connect_t cross_connect;
 int rv;

 /* Add ports to VSWITCH instance */
 cross_connect.port1 = vlan_port_1;
 cross_connect.port2 = vlan_port_2;
 //cross_connect.encap1 = 0;
 //cross_connect.encap2 = 0;
 //cross_connect.flags = 0;

 rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_cross_connect_add\n");
  print rv;
  return rv;
 }
 printf("Cross-connect added between vlan_port 0x%x and 0x%x\n", vlan_port_1, vlan_port_2);

 return BCM_E_NONE;
}

/*
 * Remove a cross-connect
 */
int cc_remove(unsigned int vlan_port_1, unsigned int vlan_port_2)
{
 bcm_vswitch_cross_connect_t cross_connect;
 int rv;

 /* Add ports to VSWITCH instance */
 cross_connect.port1 = vlan_port_1;
 cross_connect.port2 = vlan_port_2;
 //cross_connect.encap1 = 0;
 //cross_connect.encap2 = 0;
 //cross_connect.flags = 0;

 rv = bcm_vswitch_cross_connect_delete(unit, &cross_connect);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error, bcm_vswitch_cross_connect_add\n");
  print rv;
  return rv;
 }
 printf("Cross-connect between vlan_ports 0x%x and 0x%x removed\n", vlan_port_1, vlan_port_2);

 return BCM_E_NONE;
}

/*
 * Create a VLAN
 */
int vlan_create(int vlan)
{
 int rv;

 /* Create VLANs */
 rv = bcm_vlan_create(unit, vlan);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("error: bcm_vlan_create failed: rv=%d\n", rv);
  return rv;
 }

 printf("Vlan %u created!\n", vlan);

 return 0;
}

/*
 * Destroy a VLAN
 */
int vlan_destroy(int vlan)
{
 int rv;

 /* Create VLANs */
 rv = bcm_vlan_destroy(unit, vlan);
 if (rv != BCM_E_NONE && rv != BCM_NOT_EXIST)
 {
  printf("error: bcm_vlan_destroy failed: rv=%d\n", rv);
  return rv;
 }

 printf("Vlan %u destroyed!\n", vlan);

 return 0;
}

/*
 * Add a physical port to a VLAN
 */
int vlan_add(int vlan, int port)
{
 int rv;
 bcm_pbmp_t pbmp, ubmp;

 /* Add port member */
 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_CLEAR(ubmp);
 BCM_PBMP_PORT_ADD(pbmp, port);

 rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("error: bcm_vlan_port_add failed.\n");
  print rv;
  return rv;
 }
 if (rv == BCM_E_NONE)
 {
  printf("Port %u added to vlan %u\n", port, vlan);
 }

 return BCM_E_NONE;
}

/*
 * Remove a port from a VLAN
 */
int vlan_remove(int vlan, int port)
{
 int rv;
 bcm_pbmp_t pbmp, ubmp;

 /* Remove port member */
 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_CLEAR(ubmp);
 BCM_PBMP_PORT_ADD(pbmp, port);

 rv = bcm_vlan_port_remove(unit, vlan, pbmp);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vlan_port_remove failed.\n");
  print rv;
  return rv;
 }
 printf("Port %u removed from VLAN %u\n", port, vlan);

/*
 // If all members were removed, destroy VLAN
 rv = bcm_vlan_port_get(unit, vlan, &pbmp, &ubmp);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vlan_port_get failed.\n");
  print rv;
  return rv;
 }
 if (BCM_PBMP_IS_NULL(pbmp))
 {
  // Destroy VLAN
  rv = bcm_vlan_destroy(unit, vlan);
  if (rv != BCM_E_NONE)
  {
   printf("error: bcm_vlan_destroy failed.\n");
   print rv;
   return rv;
  }
  printf("VLAN %u destroyed!\n", vlan);
 }
*/

 return BCM_E_NONE;
}

/*
 * Create a VSI
 */
int vswitch_create(int vsi)
{
 int rv;

 /* Create VSWITCH instance */
 rv = bcm_vswitch_create_with_id(unit, vsi);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("error: bcm_vswitch_create_with_id.\n");
  print rv;
  return rv;
 }
 printf("VSI %u created!\n", vsi);
 return 0;
}

/*
 * Destroy a VSI
 */
int vswitch_destroy(int vsi)
{
 int rv;

 /* Create VSWITCH instance */
 rv = bcm_vswitch_destroy(unit, vsi);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("error: bcm_vswitch_destroy.\n");
  print rv;
  return rv;
 }
 printf("VSI %u destroyed!\n", vsi);
 return 0;
}

/*
 * Add a LIF to a VSI
 */
int vswitch_add(int vsi, int vlan_port_id)
{
 int rv;

 /* Add ports to VSWITCH instance */
 rv = bcm_vswitch_port_add(unit, vsi, vlan_port_id);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vswitch_port_add\n");
  print rv;
  return rv;
 }

 printf("Vlan_port_id 0x%x added to VSI %u!\n", vlan_port_id, vsi);

 return BCM_E_NONE;
}

/*
 * Remove a LIF from the VSI
 */
int vswitch_remove(int vsi, int vlan_port_id)
{
 int rv;

 /* Remove port from VSWITCH instance */
 rv = bcm_vswitch_port_delete(unit, vsi, vlan_port_id);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
  printf("Error, bcm_vswitch_port_delete\n");
  print rv;
  return rv;
 }

 printf("vlan_port_id 0x%x removed from VSI %u!\n", vlan_port_id, vsi);

 return BCM_E_NONE;
}

/*
 * Create a MC group for replication purposes
 */
int multicast_create(int mc_group)
{
 int rv;

 /* Create Multicast group */
 rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID| BCM_MULTICAST_TYPE_L2, &mc_group);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_multicast_create \n");
  return rv;
 }
 printf("Multicast group %d created\n", mc_group);

 return BCM_E_NONE;
}

/*
 * Destroy a MC group
 */
int multicast_destroy(int mc_group)
{
 int rv;

 /* Create Multicast group */
 rv = bcm_multicast_destroy(unit, mc_group);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_multicast_destroy\n");
  return rv;
 }
 printf("Multicast group %d destroyed\n", mc_group);

 return BCM_E_NONE;
}

/*
 * Add a LIF to a MC group (for replication purposes)
 */
int multicast_vlan_add(int mc_group, int port, int vlan_port_id)
{
 int rv;
 bcm_if_t encap_id;

 rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, vlan_port_id, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group, port, vlan_port_id);
  return rv;
 }

 rv = bcm_multicast_egress_add(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_multicast_egress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group, port, encap_id);
  return rv;
 }
 
 printf("Replication entry added\n");

 return BCM_E_NONE;
}

/*
 * Remove a LIF from a MC group
 */
int multicast_vlan_remove(int mc_group, int port, int vlan_port_id)
{
 int rv;
 bcm_if_t encap_id;

 rv = bcm_multicast_vlan_encap_get(unit, mc_group, port, vlan_port_id, &encap_id);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group, port, vlan_port_id);
  return rv;
 }

 rv = bcm_multicast_egress_delete(unit, mc_group, port, encap_id);
 if (rv != BCM_E_NONE) {
  printf("Error, in bcm_multicast_egress_delete mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group, port, encap_id);
  return rv;
 }
 
 printf("Replication entry removed\n");

 return BCM_E_NONE;
}
