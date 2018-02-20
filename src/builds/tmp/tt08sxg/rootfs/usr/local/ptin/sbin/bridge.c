/* Dependencies: port.c */

int vp_add(int port, int vlan_ing, int inner_vlan_ing, int vsi)
{
 bcm_vlan_port_t vp;
 int criteria;
 int rv;

 /* Create virtual port */
 bcm_vlan_port_t_init(&vp);

 if (inner_vlan_ing != 0)  criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
 else if (vlan_ing  != 0)  criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
 else                      criteria = BCM_VLAN_PORT_MATCH_PORT;

 vp.criteria = (inner_vlan_ing!=0) ? BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED : BCM_VLAN_PORT_MATCH_PORT_VLAN;
 vp.port = port;
 vp.match_vlan = (vlan_ing<0 || vlan_ing>4095) ? BCM_VLAN_NONE : vlan_ing;
 vp.match_inner_vlan = (inner_vlan_ing<0 || inner_vlan_ing>4095) ? BCM_VLAN_NONE : inner_vlan_ing;
 vp.egress_vlan = (vlan_ing<0 || vlan_ing>4095) ? BCM_VLAN_NONE : vlan_ing; /* when forwarded to this port, packet will be set with this out-vlan */
 vp.egress_inner_vlan = (inner_vlan_ing<0 || inner_vlan_ing>4095) ? BCM_VLAN_NONE : inner_vlan_ing;
 vp.vsi = vsi; /* will be populated when the gport is added to service, using vswitch_port_add */
 vp.flags = 0;

 rv = bcm_vlan_port_create(unit, &vp);
 if (rv != BCM_E_NONE) {
  printf("Error, bcm_vlan_port_create\n");
  print rv;
  return rv;
 }

 printf("vlan_port_id 0x%x created for port 0x%x, outerVLAN=%d, innerVLAN=%d (VSI=%u)\n", vp.vlan_port_id, vp.port, vp.match_vlan, vp.match_inner_vlan, vp.vsi);

 return BCM_E_NONE;
}

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

int vlan_add(int vlan, int port)
{
 int rv;
 bcm_pbmp_t pbmp, ubmp;

 /* Create VLANs */
 rv = bcm_vlan_create(unit, vlan);
 if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
 {
  printf("error: bcm_vlan_create failed.\n");
  print rv;
  return rv;
 }
 if (rv == BCM_E_NONE)
 {
  printf("Vlan %u created!\n", vlan);
 }

 /* Add port member */
 if (port >= 1)
 {
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
 }

 
 return BCM_E_NONE;
}

int vlan_remove(int vlan, int port)
{
 int rv;
 bcm_pbmp_t pbmp, ubmp;

 /* Remove port member */
 if (port >= 1)
 {
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
 }

 /* If all members were removed, destroy VLAN */
 rv = bcm_vlan_port_get(unit, vlan, &pbmp, &ubmp);
 if (rv != BCM_E_NONE)
 {
  printf("error: bcm_vlan_port_get failed.\n");
  print rv;
  return rv;
 }
 if (BCM_PBMP_IS_NULL(pbmp))
 {
  /* Destroy VLAN */
  rv = bcm_vlan_destroy(unit, vlan);
  if (rv != BCM_E_NONE)
  {
   printf("error: bcm_vlan_destroy failed.\n");
   print rv;
   return rv;
  }
  printf("VLAN %u destroyed!\n", vlan);
 }

 return BCM_E_NONE;
}


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

